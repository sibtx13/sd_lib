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
    typedef boost::shared_ptr<T> shared_ptr;
    typedef std::pair<shared_ptr ,bool> ref_pair;

private:
    //extra level of indirection so we can do compare and swap on ptrs
    ref_pair** marked_pair;
    
public:
    
    atomic_markable_reference(shared_ptr val, bool mark){
	marked_pair = (ref_pair**) malloc(sizeof(ref_pair*));
	ref_pair* rp = new ref_pair(val,mark);
	*marked_pair = rp;
    }

    ~atomic_markable_reference(){
        //clean up malloc
        delete *marked_pair;
        free (marked_pair);
    }

    ref_pair* get_pair(){
	return *marked_pair;
    }

    /*
      Performs an atomic compare and set on both fields val and mark.
      The old_pair must be exactly equal (i.e. strictly the same reference object)
      for the swap to work. The reference can be gotten from get_pair().
      If call works, caller shoudl delete old_pair.
     */
    bool compare_and_set(ref_pair* old_pair, shared_ptr new_val, bool new_mark){
	
	ref_pair* new_pair = new ref_pair(new_val,new_mark);
	
	if(__sync_bool_compare_and_swap(
					marked_pair,
					old_pair,
					new_pair))
	    {
		//means it worked, so we can delete old_pair and return true
                delete old_pair;
		return true;
	    }
	//get rid of new_pair and return false
	delete new_pair;
	return false;

    }

    

    bool attempt_mark(shared_ptr expected_val, bool new_mark){
	ref_pair* old_pair = get_pair();
	
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
	ref_pair* old_pair = get_pair();
	_mark = old_pair->second;
	return old_pair->first;
    }







};

#endif /* ATOMIC_MARKABLE_H */
