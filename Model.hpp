//
// Created by rtorres12 on 12/4/15.
//

#ifndef RAXSIMULATE_MODEL_HPP
#define RAXSIMULATE_MODEL_HPP
using namespace std;
#include <vector>
#include "Event_Queue.hpp"
#include <iostream>
#include <string>
/*
* I : Input type.
* O : Output type.
* S : State type.
*/
template<class I, class O, class S, class SUB>
class Model {
public :
    Event_Queue * context;
    string name;
    int triggerTime;
    vector<SUB *> subscribed;
    S * currentState;
    virtual S * deltaExternal(S * currentState, I * input, int insertionTime) = 0;
    virtual S * deltaInternal(S * currentState) = 0;
    virtual O * lambda(S * currentState, int peekTime) = 0;
    virtual void setState(S * state) = 0;
};


#endif //RAXSIMULATE_MODEL_HPP
