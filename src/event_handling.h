#pragma once
#include <list>


template <class event_type>
class EventListener{
public:
  virtual void handle_event(event_type evt){
    
  }
};


template <class event_type>
class EventSpawner{
public:

  std::list<EventListener<event_type> *> listeners;

  void register_listener(EventListener<event_type> * listener){
    listeners.push_back(listener);
  }

  void unregister_listener(EventListener<event_type> * listener){
    listeners.remove(listener);
  }

  void spawn_event(event_type evt){
    for(
	typename std::list<EventListener<event_type> *>::iterator it = listeners.begin(); 
	it != listeners.end();
	it++){
      (*it)->handle_event(evt);
    }
  }

};
