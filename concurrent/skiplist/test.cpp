#include <boost/shared_ptr.hpp>
#include <iostream>
#include "lock_free_skiplist.hpp"

int main(){

    std::cout << "Hello\n";

    
    std::pair<bool,bool>* i = new std::pair<bool,bool>(false,false);
    std::pair<bool,bool>* j = new std::pair<bool,bool>(true,true);
    

    std::pair<bool,bool>** m = &i;

    std::cout << (**m).first << std::endl;

    bool swap = __sync_bool_compare_and_swap(m,i,j);

    std::cout << swap << " " << (**m).first << std::endl;

    
    
    sd::lock_free_skiplist<int,33> lfs;

    lfs.add(5);
    
    return 0;
}
