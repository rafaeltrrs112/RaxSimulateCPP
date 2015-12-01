//
// Created by rtorres12 on 11/29/15.
//

#ifndef RAXSIMULATE_EVENT_HPP
#define RAXSIMULATE_EVENT_HPP
#include "Event.hpp"
/**
 * Event class
 * @param millisecond
 *      The millisecond real time this event is set to occur at.
 */
class Event {
public:
    Event(int _ms);
    int get_milli_seconds();
    //executes this events enclosed action.
    virtual void commit() = 0;
private:
    int ms; // >= 0
};

class Event_Comparator {
public:
    bool operator()(Event* t1, Event* t2)
    {
        return t1->get_milli_seconds() > t2->get_milli_seconds();
    }
};


#endif //RAXSIMULATE_EVENT_HPP
