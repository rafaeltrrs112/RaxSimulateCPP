//
// Created by rtorres12 on 12/4/15.
//

#include "PassThroughRouter.hpp"
PassThroughRouter::PassThroughRouter(RouteState * defaultState, Event_Queue * _context){
    this->context = _context;
    currentState = defaultState;
}

RouteState * PassThroughRouter::deltaExternal(RouteState * currentState, XorTokenBag * input, int insertionTime){
    input->readyTime = insertionTime;

    int outputTime = insertionTime + triggerTime;
    auto receiver = subscribed[0];

    auto outputEvent = new Output_Event<PassThroughRouter, XorTokenBag, XorModel>(this, receiver, outputTime, context);
    cout << name << " SUBS [" << subscribed[0]->name << "]" << endl;
    cout << "\nOE at [" << outputTime << "]" << endl;
    cout << " IS "; input->println(); cout << endl;

    context->insert(outputEvent);
    return currentState->join(input);
};

RouteState * PassThroughRouter::deltaInternal(RouteState * currentState){
    return currentState->reduce();
}

XorTokenBag * PassThroughRouter::lambda(RouteState * currentState, int peekTime){
    auto input = currentState->value;
    input->readyTime = peekTime;
    return input;
}

