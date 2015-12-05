//
// Created by rtorres12 on 12/4/15.
//

#include "XorState.hpp"

XorState::XorState(){};
XorState::XorState(xorIdentifier _value) : value(_value){}
//Make this a priority queue of drill press parts.
//Called delta internal : assumes output can be created.
XorState * XorState::reduce(){
    this->value = False;
    return this;
}

XorState * XorState::join(XorTokenBag * incomingInput){
    this->value = incomingInput->identifier;
    return this;
}

