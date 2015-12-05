//
// Created by rtorres12 on 12/4/15.
//

#ifndef RAXSIMULATE_XORSTATE_HPP
#define RAXSIMULATE_XORSTATE_HPP


#include "XorToken.hpp"
#include "XorTokenBag.hpp"
#include "State.hpp"

class XorState : public State<XorTokenBag, XorToken> {
public:
    XorState();
    xorIdentifier value;
    XorState(xorIdentifier _value);
    //Make this a priority queue of drill press parts.
    //Called delta internal : assumes output can be created.
    XorState * reduce();
    ~XorState(){
        cout << "State Deleted" << endl;
    }
    XorState * join(XorTokenBag * incomingInput);
};


#endif //RAXSIMULATE_XORSTATE_HPP
