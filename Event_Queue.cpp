//
// Created by rtorres12 on 11/29/15.
//

#include "Event_Queue.hpp"
using namespace std;
Event_Queue::Event_Queue(){}

Event_Queue::~Event_Queue(){
    while(!eventQueue.empty()){
        //Get the top value
        auto e = eventQueue.top();
        eventQueue.pop();
        delete e;
    }
    cout << "Event_Queue cleaned." << endl;
}

void Event_Queue::insert(Event * e){
    eventQueue.push(e);
}

void Event_Queue::insert_all(std::vector<Event *> events){
    for(auto& event : events){
        insert(event);
    }
}

void Event_Queue::dump(){
    while(!eventQueue.empty()){
        //Get the top value
        cout << eventQueue.size() << endl;
        auto e = eventQueue.top();
        e->commit();
        eventQueue.pop();
        delete e;
        //Do a check here to see if this event should be allowed to commit or if a confluent join should occur.
    }
}

void Event_Queue::log(std::__cxx11::string message){
    std::cout << message << std::endl;
}