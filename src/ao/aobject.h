#ifndef aobject_h
#define aobject_h

#include "linked_list.h"
#include "avl_tree.h"

typedef unsigned long ao_time;

const ao_time ao_max_time_half = 0x80000000;

extern ao_time* (*ao_get_now)();

bool ao_time_ge(const ao_time& t1, const ao_time& t2);

class AObject {
	
private:
	void signal0();
	void signal1();	
	
public:
	//LinkedListItem<AObject> all;
	LinkedListItem<AObject> immediate;
	TreeNode<AObject> byTime;
	ao_time time;
	
	
public:	

	AObject();

	void getNow(ao_time& time);

	virtual void process() {}

	void processInternal() {
		process();
	}
	
	virtual bool shutdown() {
		return true;
	}
	
	void signal();
	
	void reSignal();	
	
	void add(ao_time& time, const long delta);
	
	bool reached(const ao_time& time);
	
	bool clearReached(ao_time& time);
	
	void schedule(const  ao_time& time);
	
	void delay(ao_time& time, const long delay);
	
	void addDelay(ao_time& time, const long delay);

};

/*
class AOManager {
public:
	//LinkedList<AObject> allObjectList;
	LinkedList<AObject> immediateQueue0;
	LinkedList<AObject> immediateQueue1;
	Tree<AObject> timeTable;		
	
	AOManager();
};
*/

typedef void (*Callback)();

class Signaler;

class Listener {
protected:
	char type;	
public:	
 	LinkedListItem<Listener> queue;	
	
	Listener(char type) {
		this->type = type;
	}
		
	void signal();
	
	bool waiting(const Signaler& signaler);

	bool wait(Signaler& signaler);		
	
	bool waiting() {
		return queue.list;
	}
	
	bool isSignaled() {
		return !queue.list;
	}
	
    void clear() {
      queue.remove();
    }
	
};

class AOListener : public Listener {
private:
	AObject* aObject;	
public:
	AOListener(AObject* aObject) : Listener(0) {
		this->aObject = aObject;
	}

	void signal() {
		aObject->signal();
	}
	
};

class CBListener : public Listener {
private:
	Callback callback;	
public:
	CBListener(Callback callback) : Listener(1) {
		this->callback = callback;
	}
	
	void signal() {
		callback();
	}
	
};

class Signaler {
public:	
	LinkedList<Listener> queue;
	Signaler();
	
	void signalAll() {
		while(LinkedListItem<Listener>* i = queue.removeFirstItem()) {
			queue.get(i)->signal();
		}
	}

  Listener* first() {
    return queue.getFirst();
  }

  bool hasListeners() {
    return queue.first;
  }

  bool isEmpty() {
    return !queue.first;
  }

  void clear() {
    queue.clear();
  }

};

class ExclusiveResource: public AObject {
private:
  Signaler waiting;    
  Signaler current;
  bool alive;
  bool busy;  
  void startNext();
public:

  bool keepLock(Listener& listener);

  void unlock();
  
  void signalLocker();

  void process();
  
  void setBusy(bool busy);
  
  bool isBusy();

};

ao_time ao_loop();

ao_time ao_emulate(const ao_time fromTime, const ao_time toTime);

#endif


