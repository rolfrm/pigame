#pragma once
#include <list>


template <class event_type>
class EventListener{
public:
  int priority;
  EventListener(){
    priority = 0;
  }
  virtual bool handle_event(event_type evt){
    
  }
  bool operator<(const EventListener & other){
    return other.priority > priority;
  }

};



template <class event_type>
class EventSpawner{
public:

  std::list<EventListener<event_type> *> listeners;
  bool sorted;
  
  void register_listener(EventListener<event_type> * listener){
    sorted = false;
    listeners.push_back(listener);
  }

  void unregister_listener(EventListener<event_type> * listener){
    listeners.remove(listener);
  }
  

  void spawn_event(event_type evt){
    if(!sorted){
      listeners.sort();
    }
    for(
	typename std::list<EventListener<event_type> *>::iterator it = listeners.begin(); 
	it != listeners.end();
	it++){
      if((*it)->handle_event(evt) == false){
	break;
      }
    }
  }

};
