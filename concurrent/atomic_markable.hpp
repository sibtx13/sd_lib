#ifndef ATOMIC_MARKABLE_H
#define ATOMIC_MARKABLE_H

#include <utility>
#include <boost/shared_ptr.hpp>

/*
  The Atomic_Markable_Reference class defined in
  The Art of Multiprocessor Programming in section 9.8
  It represents a value and a boolean marked. The two fields 
  may be atomically modified together or individually via
  compare_and_set or attempt_mark.

  leak free via valgrind (when run with test)
 */
template <typename T>
class atomic_markable_reference{

public:
    typedef typename boost::shared_ptr<T> shared_ptr;
    typedef typename std::pair<shared_ptr ,bool> ref_pair;
    typedef typename boost::shared_ptr<ref_pair> pair_ptr;

private:
    //extra level of indirection so we can do compare and swap on ptrs
    ref_pair** marked_pair;
    pair_ptr curr;

public:
    
    atomic_markable_reference(shared_ptr val, bool mark){
	marked_pair = (ref_pair**) malloc(sizeof(ref_pair*));
	//ref_pair* rp = new ref_pair(val,mark);
        curr = pair_ptr(new ref_pair(val,mark));
	*marked_pair = curr.get();
    }
    //only for use by containers and arrays
    atomic_markable_reference(){
	marked_pair = (ref_pair**) malloc(sizeof(ref_pair*));
	
    }

    ~atomic_markable_reference(){
        //clean up malloc
        //delete *marked_pair;
        free (marked_pair);
    }

    pair_ptr get_pair(){
	return curr;
    }

    shared_ptr get_ref(){
        return get_pair()->first;
    }

    /*
      A convinience function. It is not atomic or thread-safe, so should only be called
      when owned by only 1 thread. Otherwise use compare_and_set
     */
    void set(shared_ptr new_val, bool new_mark){
        pair_ptr p = get_pair();
        p->first = new_val;
        p->second = new_mark;
    }

    /*
      Performs an atomic compare and set on both fields val and mark.
      The old_pair must be exactly equal (i.e. strictly the same reference object)
      for the swap to work. The reference can be gotten from get_pair().
      If call works, caller shoudl delete old_pair.
     */
    bool compare_and_set(pair_ptr old_pair, shared_ptr new_val, bool new_mark){
	
	pair_ptr new_pair(new ref_pair(new_val,new_mark));
	
	if(__sync_bool_compare_and_swap(
					marked_pair,
					old_pair.get(),
					new_pair.get()))
	    {
		//means it worked, so we can delete old_pair and return true
                curr = new_pair;
		return true;
	    }
	//get rid of new_pair and return false
	//delete new_pair;
	return false;

    }

    /*
      A convinience function for above. This function works atomically
      if *marked_pair is treated as immutable
     */
    bool compare_and_set(shared_ptr old_val, bool old_mark, 
                         shared_ptr new_val, bool new_mark){
        //get old pair and make sure its values match old value and old mark
        pair_ptr old_pair = get_pair();
        if( old_pair->first != old_val || old_pair->second != old_mark)
            return false;
        return compare_and_set(old_pair,new_val,new_mark);
        
    }

    /*
      attempts to set the mark to new_mark atomically iff val is
      equal to expected_val
     */
    bool attempt_mark(shared_ptr expected_val, bool new_mark){
	pair_ptr old_pair = get_pair();
	
	//make sure it conforms to input
	if(old_pair->first !=  expected_val)
	    return false;
	//atomic since old_pair won't change, only the pointer to it can.
	return compare_and_set(old_pair,expected_val,new_mark);

    }

    /*
      This is just to conform to the inteface, get_pair is better
     */
    shared_ptr get(bool &_mark){
	pair_ptr old_pair = get_pair();
        //if null, return null shared_ptr
        //if(!old_pair)
        //    return shared_ptr();
	_mark = old_pair->second;
	return old_pair->first;
    }

    





};

#endif /* ATOMIC_MARKABLE_H */
