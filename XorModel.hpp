//
// Created by rtorres12 on 12/4/15.
//

#ifndef RAXSIMULATE_XORMODEL_HPP
#define RAXSIMULATE_XORMODEL_HPP


#include "XorState.hpp"
#include "Event_Queue.hpp"
#include "Model.hpp"

class XorModel : public Model<XorTokenBag, XorToken, XorState, XorModel> {
public:
    Event_Queue * context;
    string name;
    int triggerTime = 10;
    XorState * currentState;

    void setState(XorState *currentState) {
        XorModel::currentState = currentState;
    }

public:
    XorModel(string name, XorState * defaultState, Event_Queue * _context);

    XorModel(string name, Event_Queue * _context);
    ~XorModel(){
        delete currentState;
    }
    XorState * deltaExternal(XorState * currentState, XorTokenBag * input, int insertionTime);

    XorState * deltaInternal(XorState * currentState);

    XorToken * lambda(XorState * currentState, int peekTime);
};



#endif //RAXSIMULATE_XORMODEL_HPP
