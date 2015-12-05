//
// Created by rtorres12 on 12/4/15.
//

#ifndef RAXSIMULATE_PASSTHROUGHROUTER_HPP
#define RAXSIMULATE_PASSTHROUGHROUTER_HPP


#include "XorModel.hpp"
#include "RouteState.hpp"
#include "OutPut_Event.hpp"

class PassThroughRouter : public Model<XorTokenBag, XorTokenBag, RouteState, XorModel> {
public:
    Event_Queue * context;

    string name = "Router";
    int triggerTime = 10;

    vector<XorModel *> subscribed = vector<XorModel *>();

    RouteState * currentState;

    PassThroughRouter(RouteState * defaultState, Event_Queue * _context);

    RouteState * deltaExternal(RouteState * currentState, XorTokenBag * input, int insertionTime);

    RouteState * deltaInternal(RouteState * currentState);

    XorTokenBag * lambda(RouteState * currentState, int peekTime);

    void setState(RouteState * state){
        this->currentState = state;
    }
};



#endif //RAXSIMULATE_PASSTHROUGHROUTER_HPP
