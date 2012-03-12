#include <boost/shared_ptr.hpp>
#include <iostream>
#include "lock_free_skiplist.hpp"
#include <stdlib.h>

typedef typename sd::lock_free_skiplist<int,10> skiplist;

// creates 10 random values between 1-1000
void creator(skiplist lfs){

    

}

//removes 10 values between 1-1000
void remover(skiplist lfs){

}


int main(){

    std::cout << "starting testing...\n";
    srand( 0 );
    skiplist lfs;

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
