#include "aobject.h"

ao_time* (*ao_get_now)();

signed char compareAObjectsByTime(const AObject& ao1, const AObject& ao2) {
	if(ao1.time > ao2.time)
		return 1;
	else if(ao1.time == ao2.time)
		return 0;
	else
		return -1;
}

signed char compareAObjectsWithKey(const void* key, const AObject& ao2) {	
	if(*((ao_time*)key) > ao2.time)
		return 1;
	else if(*((ao_time*)key) == ao2.time)
		return 0;
	else
		return -1;
}

AObject* testAo;

//LinkedList<AObject> allObjectList((char*)(void*)&testAo->immediate - (char*)(void*)testAo);
LinkedList<AObject> immediateQueue0((char*)(void*)&testAo->immediate - (char*)(void*)testAo);
LinkedList<AObject> immediateQueue1((char*)(void*)&testAo->immediate - (char*)(void*)testAo);
Tree<AObject> timeTable((char*)&((AObject*)1)->byTime - (char*)1, &compareAObjectsByTime);

/*
AOManager::AOManager():
	immediateQueue0((char*)(void*)&testAo->immediate - (char*)(void*)testAo),
	immediateQueue1((char*)(void*)&testAo->immediate - (char*)(void*)testAo),
	timeTable((char*)(void*)&testAo->byTime - (char*)(void*)testAo, &compareAObjectsByTime)	{	
}
*/

AObject::AObject() {
	//allObjectList.add(&all);
	signal();
}

void AObject::getNow(ao_time& time) {
	time = *ao_get_now();
}

void AObject::signal0() {
	if(!immediate.list) {
		immediateQueue0.add(&immediate);
	}
}

void AObject::signal1() {
	if(!immediate.list) {
		immediateQueue0.add(&immediate);
	}
}

void AObject::signal() {
	signal0();
}

void AObject::add(ao_time& time, long delta) {
	time += delta;
	if(!time) time = 1;
}

bool AObject::reached(const ao_time& time) {
	if(!time || ao_time_ge(*ao_get_now(), time)) return true;
	schedule(time);
	return false;
}

bool AObject::clearReached(ao_time& time) {
	if(reached(time)) {
		time = 0;
		return true;		
	}    
	return false;
}

void AObject::schedule(const ao_time& time) {	
  if(!byTime.isInTree() || !ao_time_ge(time, this->time)) {
      timeTable.remove(&byTime);
      this->time = time;
      timeTable.add(&byTime);
  }
}

void AObject::delay(ao_time& time, long delay) {
	getNow(time);
	add(time, delay);
	schedule(time);
}

void AObject::addDelay(ao_time& time, long delay) {
	ao_time* t = ao_get_now();
	if(!time) time = *t;
	do {  
		add(time, delay);
	} while(!ao_time_ge(time, *t));
	schedule(time);	
}

ao_time last_time;

AObject* removeFirstImmediate() {
	AObject* ao = immediateQueue0.removeFirst();
	if(!ao) {
		ao = immediateQueue1.removeFirst();
	}
	return ao;
}

ao_time ao_loop() {

	ao_time t = *ao_get_now();
	while(true) {				
		TreeNode<AObject>* lastNode = 0;
		if(last_time  > t) {
			lastNode = timeTable.findLeftmostGE(&last_time, compareAObjectsWithKey);
			while(lastNode) {
				TreeNode<AObject>* n2 = lastNode->successor();
				timeTable.remove(lastNode);
				timeTable.get(lastNode)->signal();
				lastNode=n2;
			}
			lastNode = timeTable.getLeftmostNode();
			while(lastNode) {
				AObject* ao = timeTable.get(lastNode);
				if(ao->time > t) break;				
				TreeNode<AObject>* n2 = lastNode->successor();
				timeTable.remove(lastNode);
				ao->signal();
				lastNode=n2;
			}
		} else {
			lastNode = timeTable.findLeftmostGE(&last_time, compareAObjectsWithKey);
			while(lastNode) {
				AObject* ao = timeTable.get(lastNode);
				if(ao->time > t) break;				
				TreeNode<AObject>* n2 = lastNode->successor();
				timeTable.remove(lastNode);
				ao->signal();
				lastNode=n2;
			}		
		}
		last_time = t;
		
		AObject* ao = removeFirstImmediate();
		if(!ao) {
			if(lastNode)
				return timeTable.get(lastNode)->time;
			else
				return 0;
		}

		int maxn = 10;
		while(ao) {
			ao->processInternal();
			if(maxn-- <= 0) {
				break;
			}
			ao = removeFirstImmediate();
		}
	}
}

Listener* dummyListener;

Signaler::Signaler() : queue((char*)(&(dummyListener->queue)) - (char*)dummyListener) {		
}

void Listener::signal() {
	queue.remove();
	switch(type) {
		case 0:
			((AOListener*)this)->signal();
			break;
		case 1:
			((CBListener*)this)->signal();
			break;							
	}
}

bool Listener::waiting(const Signaler& signaler) {
	return queue.inList(signaler.queue);
}

bool Listener::wait(Signaler& signaler) {
	if(queue.inList(signaler.queue)) return true;
	signaler.queue.add(&queue);
	return false;	
}

bool ao_time_ge(const ao_time& t1, const ao_time& t2) {
	return (t1 - t2) < ao_max_time_half;
}

ao_time emulate_time;
ao_time* get_emulate_time() {
	return &emulate_time;
}

ao_time ao_emulate(ao_time fromTime, ao_time toTime) {
	ao_get_now = get_emulate_time;
	emulate_time = fromTime;
	while(ao_time_ge(toTime, emulate_time)) {
		ao_time nextTime = ao_loop();
		if(!nextTime) {
			return emulate_time;			
		}
		emulate_time = nextTime;
	}
	return 0;
}
	
void get_now(ao_time& time) {
	time = *ao_get_now();
}