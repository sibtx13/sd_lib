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
        	
        struct node{
            V value;
	    //an array of shared_ptrs of node*,bool pairs
	    boost::shared_ptr<std::pair<node*,bool> >* next;
            int top_level;
            //constructor for sentinels
            node(){
                top_level = H;
                //value = NULL;
                next = new boost::shared_ptr<std::pair<node*,bool> >[H];
                
                for(int i=0;i<H;i++){
                    next[i] = boost::shared_ptr<std::pair<node*,bool> >
			(new std::pair<node*,bool>(NULL,false));
                }
            }
         
	    //constructor for inner nodes
	    node(V val , int height){
		value = val;
		top_level = height;
		//TODO might want to make sure that height is less than H
		next = new std::pair<node*,bool>*[top_level];
                
                for(int i=0;i<top_level;i++){
                    next[i] = new std::pair<node*,bool>(NULL,false);
                }
		
		next = new boost::shared_ptr<std::pair<node*,bool> >[top_level];
                
                for(int i=0;i<top_level;i++){
                    next[i] = boost::shared_ptr<std::pair<node*,bool> >
			(new std::pair<node*,bool>(NULL,false));
                }

	    }

	    //destructor
	    ~node(){
		//TODO check for memory leak, will have to delete entries separately
		delete next;
	    }
        };

	
	typedef std::pair<node*,bool> marked_node;
	typedef boost::shared_ptr<marked_node> shared_m_node;

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
		//set new value
		//head.next[i] =  new std::pair<node*,bool>(&tail,false);
		head.next[i] =  shared_m_node(new marked_node(&tail,false));
	    }
	}
	
	/*adds the new value atomically, not wait-free. Returns true
	if the value did not already exist and was added, false otherwise. 
	*/
	bool add(V val){
	    int top_level = _random_level();
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
		    for(int l=0;l<top_level;l++){
			node* succ = succs[l];
			
			//set new value
			//new_node->next[l] = new marked_node(succ,false);
			new_node->next[l] =  shared_m_node(new marked_node(succ,false));
		    }
		    //bottom level pred and succ, since we work our way up
		    node* succ = succs[0];
		    node* pred = preds[0];
		    //try to set bottom atomically
		    //CAS works because we treat marked_nodes as immutables
		    //first check that succ is not marked
		    marked_node* m_succ = pred->next[0].get();
		    if(m_succ->second ||  !__sync_bool_compare_and_swap(
									pred->next[0],
									m_succ,
									new marked_node(new_node,false))
		       ){
			//if not, try again
			continue;
		    }
		    //we can get rid of old edge, its been replaced
		    delete m_succ;
		 
		    //now take care of the higher levels
		    for(int level=1;level<top_level;level++){
			//keep retrying at each level
			while(true){
			    succ = succs[level];
			    pred = preds[level];
			    //attempt to set level atomically
			    marked_node* m_succ = pred->next[level].get();
			    if(!m_succ->second && __sync_bool_compare_and_swap(
									       &pred->next[level].get(),
									       m_succ,
									       new marked_node(new_node,false))
			       ){
				//we can get rid of old edge
				delete m_succ;

				//and done with this level
				break;
			    }
			    //otherwise retry
			    _find(val,succs,preds);


			}

		    }
		    return true;
		    

		}
		



	    }

	} // add

	/*attempts to mark val for removal. Returns true if successful. 
	If val is not present or is already marked, returns false.
	*/
	bool remove(V val){
	    int bottom_level = 0;
	    
	    node* preds[H];
	    node* succs[H];
	    node* succ;

	    //loop to mark nodes
	    while(true){

		bool found = _find(val,preds,succs);
		//return false if not present
		if(!found)
		    return false;
		else{
		    node* node_to_remove = succs[0];
		    //mark it top-down except for the bottom level
		    for(int level = node_to_remove->top_level-1;level>0;level--){
			marked_node* m_succ = node_to_remove->next[level];
			succ = m_succ->first;
			bool marked = m_succ->second;
			//keep going until the node gets marked here or elsewhere
			while(!marked){
			    //try to mark
			    __sync_bool_compare_and_swap(
							 &(node_to_remove->next[level]),
							 m_succ,
							 new marked_node(succ,true)
							 );
			    //see if it got marked
			    m_succ = node_to_remove->next[level];
			    succ = m_succ->first;
			    marked = m_succ->second;
			    
			}

		    }

		}
	    }

	} //remove




    };


} // namespace sd
#endif /* LOCK_FREE_SKIPLIST_H */
