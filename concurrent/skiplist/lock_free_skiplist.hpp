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
            node* next;

            

        };



    }


} // namespace sd
#endif /* LOCK_FREE_SKIPLIST_H */
