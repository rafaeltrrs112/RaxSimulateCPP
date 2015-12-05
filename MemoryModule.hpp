//
// Created by rtorres12 on 12/4/15.
//

#ifndef RAXSIMULATE_MEMORYMODULE_HPP
#define RAXSIMULATE_MEMORYMODULE_HPP


#include "XorToken.hpp"
#include "MemoryState.hpp"
#include "PassThroughRouter.hpp"

class MemoryModule : public Model<XorToken, XorToken, MemoryState, PassThroughRouter>, public XorModel {
public :
    Event_Queue * context;


    void setState(MemoryState *currentState) {
        MemoryModule::currentState = currentState;
    }

    string name = "MEM MODULE";

    int triggerTime = 10;

    vector<PassThroughRouter *> subscribed = vector<PassThroughRouter *>();
    MemoryState * currentState;

    MemoryModule(string name, Event_Queue * context, MemoryState * _currentState) ;

    ~MemoryModule();

    MemoryState * deltaExternal(MemoryState * currentState, XorToken * input, int insertionTime);

    MemoryState * deltaInternal(MemoryState * currentState);

    XorToken * lambda(MemoryState * currentState, int peekTime);
};



#endif //RAXSIMULATE_MEMORYMODULE_HPP
