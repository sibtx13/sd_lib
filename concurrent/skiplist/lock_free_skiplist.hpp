#ifndef LOCK_FREE_SKIPLIST_H
#define LOCK_FREE_SKIPLIST_H

#include <boost/shared_ptr.hpp>
#include <iostream>
#include <stdlib.h>
#include <time.h>

namespace sd{

    /*an implementation for a lock_free_skiplist based on
    Herlihy, Lev, and Shavit.
    */
    template< typename V , int H = 33>
    class lock_free_skiplist
    {
    private:
        typedef boost::shared_ptr<V> shared_ptr;
	
        struct node{
            V value;
            std::pair<node*,bool>** next;
            int top_level;
            //constructor for sentinels
            node(){
                top_level = H;
                //value = NULL;
                next = new std::pair<node*,bool>*[H];
                
                for(int i=0;i<H;i++){
                    next[i] = new std::pair<node*,bool>(NULL,false);
                }
            }
         
	    //constructor for inner nodes
	    node(V val , int height){
		value = val;
		top_level = height;
		//TODO might want to make sure that height is less than H
		next = new std::pair<node*,bool>[top_level];
                
                for(int i=0;i<top_level;i++){
                    next[i] = new std::pair<node*,bool>(NULL,false);
                }
	    }

	    //destructor
	    ~node(){
		//TODO check for memory leak
		delete next;
	    }
        };

	
	typedef std::pair<node*,bool> marked_node;
	node head,tail;
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

	bool _find(V val,node** succs, node** preds){
	    return true;
	}


    public:
	lock_free_skiplist():
	    head(),
	    tail(),
	    prob_level(0.5)
	{
	    //seed random
	    srand ( time(NULL) );

	    //init head's next to tail
	    for(int i=0;i<head.top_level;i++){
		head.next[i] =  new std::pair<node*,bool>(&tail,false);
	    }
	}
	
	/*adds the new value atomically, not wait-free. Returns true
	if the value did not already exist and was added, false otherwise. 
	*/
	bool add(V val){
	    int top_level = _random_level();
	    int bottom_level = 0;
	    //array of pointers to all predecessors and successors 
	    //for the new node from find
	    node* preds[H];
	    node* succs[H];

	    while(true){
		bool found = _find(val,preds,succs);
		//the node already exists
		if(found)
		    return false;
		//we can attempt to add the node
		else{
		    //construct new node
		    node* new_node = new node(val,top_level);
		    //set the next ptrs of the new node to successors
		    for(int l=bottom_level;l<top_level;l++){
			node* succ = succs[l];
			new_node->next[l] = new marked_node(succ,false);
		    }
		    //bottom level pred and succ, since we work our way up
		    node* succ = succs[0];
		    node* pred = preds[0];
		    //try to set bottom atomically
		    //TODO ---------------------------------------
		    //figure out how to CAS right (study marking in algo)
		    if(!__sync_bool_compare_and_swap(
						     pred->next[0],
						     marked_node(pred,false),
						     marked_node(new_node,false))
		       ){
			//if not, try again
			continue;
		    }
		 
		    //now take care of the higher levels
		    for(int level=1;level<top_level;level++){
			//keep retrying at each level
			while(true){
			    succ = succs[level];
			    pred = preds[level];
			    //attempt to set level atomically
			    if(__sync_bool_compare_and_swap(
							     pred->next[level],
							     marked_node(pred,false),
							     marked_node(new_node,false))
			       ){
				//then done with this level
				break;
			    }
			    //otherwise retry
			    _find(val,succs,preds);


			}

		    }
		    return true;
		    

		}
		



	    }

	}



    };


} // namespace sd
#endif /* LOCK_FREE_SKIPLIST_H */
