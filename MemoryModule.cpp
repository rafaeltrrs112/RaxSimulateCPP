//
// Created by rtorres12 on 12/4/15.
//

#include "MemoryModule.hpp"
#include "KickOffEvent.hpp"

MemoryModule::MemoryModule(string name, Event_Queue * context, MemoryState * _currentState) : XorModel(name, context){
    this->triggerTime = 10;
    this->currentState = _currentState;
    this->context = context;
}

/**
 * When receiving input start a kick off event 10 seconds later.
 */
MemoryState * MemoryModule::deltaExternal(MemoryState * currentState, XorToken * input, int insertionTime){
    int nextKickTime = insertionTime + triggerTime;

    auto kickAgain = new KickOffEvent(nextKickTime, this, context);
    context->insert(kickAgain);
    return this->currentState->join(input);
}

MemoryState * MemoryModule::deltaInternal(MemoryState * currentState){
    return this->currentState->reduce();
}

XorToken * MemoryModule::lambda(MemoryState * currentState, int peekTime){
    return this->currentState->state.front();
}

MemoryModule::~MemoryModule(){
    delete currentState;
}
