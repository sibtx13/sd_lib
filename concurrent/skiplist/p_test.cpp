#include <boost/shared_ptr.hpp>
#include <iostream>
#include "lock_free_skiplist.hpp"
#include <stdlib.h>
#include <boost/thread.hpp>


typedef typename sd::lock_free_skiplist<int,32> skiplist;

// creates n random values between 1-100,000
void creator(skiplist* lfs , int n){
    
    for(int i=0;i<n;i++){
        
        bool added = false;
        while(!added)
        {
            int x = rand() % 100000;
            added = lfs->add(x);
            //std::cout << lfs->size() << std::endl;
        }
    }
    

}

//removes n values between 1-100,000
void remover(skiplist* lfs , int n){

    int count = 0;
    int val = 0;
    while(count < n){
        if(lfs->contains(val))
        {
            if(lfs->remove(val))
                count++;

        }
        val++;
        if(val > 100000)
            break;

    }

}


int main(){

    std::cout << "starting testing...\n";
    srand( 0 );
    std::cout << RAND_MAX << " is max random, should be > 100,000" << std::endl;
    skiplist lfs;

    boost::thread_group threads;
    std::cout << "starting creator threads " << std::endl;
    //create 10 threads of 100
    for (int i=0;i<10;i++)
    {
        threads.create_thread(boost::bind(creator , &lfs, 100));
        //boost::thread* t = new boost::thread(creator,lfs,100);
        //threads.add_thread(t);
    }
    //wait for it
    std::cout << "waiting for them to finish... " << std::endl;
    threads.join_all();
    std::cout << lfs.size() << std::endl;
    assert(lfs.size() == 100*10);

    std::cout << "starting remover threads " << std::endl;
    //create 10 threads of 100 to remove
    for (int i=0;i<10;i++)
    {
        threads.create_thread(boost::bind(remover , &lfs, 100));
        //boost::thread* t = new boost::thread(remover,lfs,100);
        //threads.add_thread(t);
    }     
   
    std::cout << "waiting for them to finish... " << std::endl;
    threads.join_all();

    assert(lfs.size() == 0);

    std::cout << "test done" << std::endl;

    return 0;
}
