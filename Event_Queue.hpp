//
// Created by rtorres12 on 11/29/15.
//

#ifndef RAXSIMULATE_EVENT_QUEUE_HPP
#define RAXSIMULATE_EVENT_QUEUE_HPP
#include <iomanip>
#include <queue>
#include <iostream>
#include "Event.hpp"

class Event_Queue {
public:
    Event_Queue();
    ~Event_Queue();
    void insert(Event * e);
    void insert_all(std::vector<Event *> events);
    virtual void dump();
    void log(std::__cxx11::string message);
    std::priority_queue<Event*, std::vector<Event*>, Event_Comparator> eventQueue;
private:
};
#endif //RAXSIMULATE_EVENT_QUEUE_HPP
