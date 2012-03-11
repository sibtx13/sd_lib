#ifndef LOCK_FREE_SKIPLIST_H
#define LOCK_FREE_SKIPLIST_H

#include <boost/shared_ptr.hpp>
#include <iostream>

namespace sd{

    template< typename V , int H = 33>
    class lock_free_skiplist
    {
    private:
        typedef boost::shared_ptr<V> shared_ptr;
        
        struct node{
            V value;
            std::pair<node,bool>* next;
            int top_level;
            //constructor for sentinels
            node(){
                top_level = H;
                value = NULL;
                next = new std::pair<node,bool>[H];
                
                for(int i=0;i<H;i++){
                    next[i] = new std::pair<node,bool>(NULL,false);
                }
            }
            

        };



    }


} // namespace sd
#endif /* LOCK_FREE_SKIPLIST_H */
