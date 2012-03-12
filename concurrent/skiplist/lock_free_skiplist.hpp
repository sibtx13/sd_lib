#ifndef LOCK_FREE_SKIPLIST_H
#define LOCK_FREE_SKIPLIST_H

#include <boost/shared_ptr.hpp>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include "../atomic_markable.hpp"


namespace sd{

    template< typename V , int H>
    class node
    {
    public:
        V value;
        //an array of atomic_markable nodes
        atomic_markable_reference<node>* next;
        int top_level;
        //constructor for sentinels
        node(){
            top_level = H;
            //value = NULL;
            next = new atomic_markable_reference<node>[H];
                
            for(int i=0;i<H;i++){
                //null reference
                boost::shared_ptr<node> null;
                next[i] = atomic_markable_reference<node > (null,false);
            }
        }
         
        //constructor for inner nodes
        node(V val , int height){
            value = val;
            top_level = height;
            //TODO might want to make sure that height is less than H
            next = new atomic_markable_reference<node>[top_level];
                
            for(int i=0;i<top_level;i++){
                    
                //null reference
                boost::shared_ptr<node> null;
                next[i] =  atomic_markable_reference<node > (null,false);
            }
		

        }

        //destructor
        ~node(){
            //TODO check for memory leak, will have to delete entries separately
            delete next;
        }

    };



    /*an implementation for a lock_free_skiplist based on
    Herlihy, Lev, and Shavit.
    */
    template< typename V , int H = 33>
    class lock_free_skiplist
    {
    private:
        
        typedef typename sd::node<V,H> node_t;
	typedef  atomic_markable_reference<node_t> marked_node;
	typedef typename boost::shared_ptr<node_t> shared_ptr;
        typedef typename marked_node::ref_pair ref_pair;

	shared_ptr head,tail;
	//the probability of a node existing at level 1 (squared for level 2, etc)
	double prob_level;

	//this function computes the top level for a new node
	int _random_level(){
	    int level;
	    for(level=0;level<H;level++){
		double r =((double)rand()/(double)RAND_MAX);
		if(r < prob_level)
		    break;
	    }
	    return level;
	}

        /*
          This helper function is defined in the original algorithm.
          It searches for the succesors and predecessors of val.
          It never traverses a marked node, instead it removes them.
          Every predecessor's value is strictly less than val.
         */
	bool _find(V val,shared_ptr* succs, shared_ptr* preds){
            bool marked = false;
            bool snip;
            shared_ptr pred,succ,curr;
            bool throwaway;
            //label to escape a nested loop
        retry:
            while(true){
                pred = head;
                //travel from the top level to the bottom to get expected logn search
                for(int level=H-1;level>=0;level--){
                    curr = pred->next[level].get(throwaway);
                    //look on this level for the right pred
                    while(true){
                        succ = curr->next[level].get(marked);
                        //if marked, remove it
                        while(marked){
                            snip = pred->next[level].
                                compare_and_set(curr,false,succ,false);
                            //if we didnt snip it, something disapeared so start over.
                            //goto is merely used to escape the nested loop
                            if(!snip) goto retry;
                            curr = pred->next[level].get(throwaway);
                            succ = pred->next[level].get(marked);
                        }
                        //if we've reached the tail, then we have our pred
                        if(curr == tail)
                            break;
                        //else if we're at the head or curr's value is 
                        //less than val, then update pred and succ
                        else if(curr == head || curr->value < val){
                            pred = curr;
                            curr = succ;
                        }else{
                            //else we have found this level's pred and succ
                            break;
                        }
                    }
                    //update the pred and succ for this level
                    preds[level] = pred;
                    succs[level] = curr;

                }
                return curr->value == val;

            }
	    
	}


    public:
	lock_free_skiplist():
	    head(new node_t()),
	    tail(new node_t()),
	    prob_level(0.5)
	{
	    //seed random
	    srand ( time(NULL) );

	    //init head's next to tail
	    for(int i=0;i<head->top_level;i++){
		//set new value
		//head.next[i] =  new std::pair<node*,bool>(&tail,false);
		head->next[i] = marked_node(tail,false); 
                    
	    }
	}
	
	/*adds the new value atomically, not wait-free. Returns true
	if the value did not already exist and was added, false otherwise. 
	*/
	bool add(V val){
            int top_level = _random_level();
            //for the find call
            shared_ptr preds[H];
            shared_ptr succs[H];
            //keep trying to insert until we insert or node exists due to another thread
            while(true){
                bool found = _find(val,preds,succs);
                //if already exists, return false
                if(found)
                    return false;
                else{
                    shared_ptr new_node(new node_t(val,top_level));
                    //fill in succs for new node
                    for(int i=0;i<top_level;i++){
                        shared_ptr succ = succs[i];
                        new_node->next[i] = marked_node(succ,false);
                    }
                    //first do the bottom level insertion
                    shared_ptr succ = succs[0];
                    shared_ptr pred = preds[0];
                    //redundant line
                    //new_node->next[0] = marked_node(succ,false);

                    if(!pred->next[0].compare_and_set(succ,false,new_node,false)){
                        //if we the pred changed on us, try again from the start
                        continue;
                    }
                    //now that we have the bottom level, do the rest
                    for(int l=1;l<new_node->top_level;l++){
                        //keep going until we insert the node at the other levels
                        while(true){
                            succ = succs[l];
                            pred = preds[l];
                            if(pred->next[l].compare_and_set(succ,false,
                                                             new_node,false))
                                break;
                            //if the preds or succs changed, re-find them
                            _find(val,preds,succs);
                        }
                    }
                    //we've inserted the node at all of its levels
                    return true;
                }

            }

            
	} // add

	/*attempts to mark val for removal. Returns true if successful. 
	If val is not present or is already marked, returns false.
	*/
	bool remove(V val){
            
            //for the find call
            shared_ptr preds[H];
            shared_ptr succs[H];            
            shared_ptr succ;

            //keep trying until either we mark it or another thread does
            while(true){
                bool found = _find(val,preds,succs);
                if(!found)
                    return false;
                //else it hasnt been marked and we'll attempt it
                else{
                    shared_ptr node_to_remove = succs[0];
                    //mark it at every level top-down except for level 0
                    for(int i = node_to_remove->top_level-1;i>0;i--){
                        bool marked = false;
                        //make sure successor is marked (by any thread)
                        succ = node_to_remove->next[i].get(marked);
                        while(!marked){
                            node_to_remove->next[i].
                                compare_and_set(succ,false,succ,true);
                            succ = node_to_remove->next[i].get(marked);
                        }

                    }
                    //now mark it at level 0
                    bool marked = false;
                    succ = node_to_remove->next[0].get(marked);
                    //keep going until either I mark it or another thread does
                    while(true){
                        bool i_marked = node_to_remove->next[0].
                            compare_and_set(succ,false,succ,true);
                        succ = node_to_remove->next[0].get(marked);
                        //return whether I marked it or someone else did
                        if(i_marked){
                            //optimization to remove nodes
                            _find(val,preds,succs);
                            return true;
                        }else if(marked){
                            //means someone else marked it
                            return false;
                        }
                        //otherwise keep going until its marked
                    }

                }

            }
            
	} //remove

        
        bool contains(V val){

            bool marked = false;
            shared_ptr pred = head;
            shared_ptr curr,succ;
            bool throwaway;
            //search top-down, but make sure it is not marked on the bottom level
            for(int level = H-1;level>=0;level--){
                curr = pred->next[level].get(throwaway);
                //traverse level
                while(true){
                    succ = curr->next[level].get(marked);
                    while(marked){
                        //jump over marked nodes
                        //TODO is book wrong here? I think so
                        //curr = pred->next[level].get(throwaway);
                        curr = curr->next[level].get(throwaway);
                        succ = curr->next[level].get(marked);
                    }
                    //if curr is tail, then we have found our pred and curr
                    if(curr == tail)
                        break;
                    //else if curr is head or its value is less than val
                    //we update pred and curr
                    else if(curr == head || curr->value < val){
                        pred = curr;
                        curr = succ;
                    }
                    else
                        break;

                }

            }

            return curr->value == val;
        } //contains


    };


} // namespace sd
#endif /* LOCK_FREE_SKIPLIST_H */
