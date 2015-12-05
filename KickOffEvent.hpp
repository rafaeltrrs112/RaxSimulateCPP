//
// Created by rtorres12 on 12/4/15.
//

#ifndef RAXSIMULATE_KICKOFFEVENT_HPP
#define RAXSIMULATE_KICKOFFEVENT_HPP

#include "Event.hpp"
#include "MemoryModule.hpp"

class KickOffEvent : public Event {
public:
    MemoryModule * memoryModule;
    Event_Queue * context;
    KickOffEvent(int _ms, MemoryModule * _memoryModule, Event_Queue * _context);

    void commit();
};



#endif //RAXSIMULATE_KICKOFFEVENT_HPP
