#ifndef linked_list_h
#define linked_list_h

template <typename T>
class LinkedList;

template <typename T>
class LinkedListItem {

private:  

public:
	LinkedList<T>* list;
	LinkedListItem<T>* next;
	LinkedListItem<T>* prev;
	
	void remove() {
		if(list) list->remove(this);
	}

	~LinkedListItem() {
		remove();
	}

	bool inList() {
		return list;
	}
	
	bool inList(LinkedList<T> const& list) {
		return &list == this->list;
	}	
};

template <typename T>
class LinkedList {

private:  
  unsigned int shift;
  int count;
	LinkedListItem<T>* first;
	LinkedListItem<T>* last;  
	
public:

  LinkedList(unsigned int shift) {
    this->shift = shift;
  }
  
  ~LinkedList() {
    clear();
  }  

  T* get(LinkedListItem<T>* item) {
  	if(item)
    	return (T*)((char*)item - shift);
    else
    	return 0;     
  }

  LinkedListItem<T>* get(T* item) {
    if(item)
		return (LinkedListItem<T>*)((char*)item + shift);
	else
		return 0;
  }

  void add(LinkedListItem<T>* item) {
  	  if(item->list) {
  	 	remove(item);		
	  }
	  if(!first) {
	  	first = item;
	  	last = item;
	    item->next = 0;
	    item->prev = 0;
	} else {
	    last->next = item;
	    item->prev = last;
	    item->next = 0;
	    last = item;
	  }
	  item->list = this;
	  count++;
  }
  
  void remove(LinkedListItem<T>* item) {
	  if(!item->list) return;
	  if(!item->next) {
	    if(!item->prev) {
	      first =  0;
	      last =  0;
	    } else {
	      item->prev->next =  0;
	      last = item->prev;
	    }
	  } else {
	    if(!item->prev) {
	      first = item->next;
	      first->prev =  0;
	    } else {
	      item->next->prev = item->prev;
	      item->prev->next = item->next;
	    }
	  }
	  count--;
	  item->list =  0;
  }
  
  LinkedListItem<T>* removeFirstItem() {
	  LinkedListItem<T>* Result = first;
	  if(!Result) return 0;
	  if(!Result->next) {
	    first =  0;
	    last =  0;
	  } else {
	    first = Result->next;
	    first->prev =  0;
	  }
	  count--;
	  Result->list =  0;
	  return Result;
  }

  T* removeFirst() {
  	return get(removeFirstItem());  	
  }
  
  void clear() {
	  LinkedListItem<T>* p = first;
	  if(p) {
	    LinkedListItem<T>* p2 = p;
	    p = p->next;
	    p2->list =  0;
	  }
	  first =  0;
	  last =  0;
	  count = 0;  	
  }
  
  void add(T* item) {
  	add(get(item));
  }

};

#endif
