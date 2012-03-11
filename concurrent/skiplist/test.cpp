#include <boost/shared_ptr.hpp>
#include <iostream>
#include "lock_free_skiplist.hpp"

int main(){

    std::cout << "Hello\n";

    boost::shared_ptr<int> p(new int(5));
    
    boost::shared_ptr<int> q = p;

    std::cout << *q.get() << std::endl;

    bool swap = __sync_bool_compare_and_swap(q.get(),5,2);
    
    std::cout << swap << " " << *q.get() << std::endl;

    sd::lock_free_skiplist<int,33> lfs;

    lfs.add(5);

    return 0;
}
