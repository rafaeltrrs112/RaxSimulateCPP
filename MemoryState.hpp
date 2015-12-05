//
// Created by rtorres12 on 12/4/15.
//

#ifndef RAXSIMULATE_MEMORYSTATE_HPP
#define RAXSIMULATE_MEMORYSTATE_HPP


#include "XorToken.hpp"
#include "State.hpp"
using namespace std;
#include "queue"
class MemoryState : public State<XorToken, XorToken> {
public :
    queue<XorToken *> state = queue<XorToken *>();
    MemoryState(queue<XorToken *> &state);

    MemoryState * join(XorToken * input);
    MemoryState * reduce();
};



#endif //RAXSIMULATE_MEMORYSTATE_HPP
