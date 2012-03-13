#include <boost/shared_ptr.hpp>
#include <iostream>
#include "atomic_markable.hpp"
#include <assert.h>

int main(){

    typedef atomic_markable_reference<std::string> amr;
    typedef amr::pair_ptr pair_ptr;

    std::cout << "starting testing...\n";
    
    
    // std::string* s1 = new std::string("hello");
    //std::string* s2 = new std::string("bye");
    amr::shared_ptr s1 = amr::shared_ptr(new std::string("hello"));
    amr::shared_ptr s2 = amr::shared_ptr(new std::string("bye"));


    amr a(s1,false);
    
    //make sure initial values are right
    pair_ptr rp = a.get_pair();
    assert(rp->first==s1 && rp->second == false );

    bool done = a.compare_and_set(rp,s2,true);
    assert(done);
    

    //make sure new values are right
    rp = a.get_pair();
    assert(rp->first==s2 && rp->second == true );

    bool marked = a.attempt_mark(s2,false);
    assert(marked);

    rp = a.get_pair();
    assert(rp->first ==s2 && rp->second == false );

    //test the get call
    bool mark = true;
    amr::shared_ptr s3 = a.get(mark);
    assert( !mark && s3 == s2);

    //test convinience compare and set
    done = a.compare_and_set(s2,false,s1,true);
    assert(done);
    rp = a.get_pair();
    assert(rp->first ==s1 && rp->second == true );

    //test with null ptr
    {
    amr::shared_ptr p;
    done = a.compare_and_set(s1,true,p,false);
    assert(done);
    rp = a.get_pair();
    assert(rp->first ==p && rp->second == false );
    assert(!p);
    }
    mark = true;
    amr::shared_ptr p2 = a.get(mark);
    assert(!p2);
    assert(!mark);

    //test shared_ptr arrays
    boost::shared_ptr<int> i_ptr(new int(5));
    boost::shared_ptr<int> ptrs[5];
    ptrs[0] = i_ptr;
    ptrs[1] = i_ptr;

    assert(ptrs[0] == i_ptr);
    assert(ptrs[0] == ptrs[1]);
    assert( *ptrs[0] == 5);

    //test on array
    amr::shared_ptr s_ptr(new std::string("h"));
    boost::shared_ptr<amr> a_ptr(new amr(s_ptr,false));

    boost::shared_ptr<amr> ptr_arr[5];
    ptr_arr[0] = a_ptr;
    ptr_arr[1] = a_ptr;

    assert(ptr_arr[0] == a_ptr);
    assert( ptr_arr[0]->get_ref() == s_ptr  );
    
    std::cout << "tests passed" << std::endl;
    
    return 0;
}
