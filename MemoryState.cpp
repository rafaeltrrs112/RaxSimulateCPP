//
// Created by rtorres12 on 12/4/15.
//

#include "MemoryState.hpp"

MemoryState::MemoryState(queue<XorToken *> &state) : state(state) { }

MemoryState * MemoryState::join(XorToken * input){
    state.push(input);
    return this;
}
MemoryState * MemoryState::reduce(){
    state.pop();
    return this;
}

