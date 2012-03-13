#include <boost/shared_ptr.hpp>
#include <iostream>
#include "lock_free_skiplist.hpp"

int main(){

    std::cout << "starting testing...\n";


    
    sd::lock_free_skiplist<int,32> lfs;

    lfs.add(5);
    std::cout << "element added" << std::endl;
    bool done = lfs.contains(5);

    assert(done);

    lfs.remove(5);
    done = lfs.contains(5);

    assert(!done);


    std::cout << "test done" << std::endl;

    return 0;
}
