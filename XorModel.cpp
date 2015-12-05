//
// Created by rtorres12 on 12/4/15.
//

#include <functional>
#include "XorModel.hpp"
#include "MemoryState.hpp"
#include "MemoryModule.hpp"

Event_Queue * context;
string name;
int triggerTime = 10;
XorState * currentState;

XorModel::XorModel(string name, XorState * defaultState, Event_Queue * _context){
    currentState = defaultState;
    this->name = name;
    context = _context;
    subscribed = vector<XorModel*>();
}

XorModel::XorModel(string name, Event_Queue * _context){
    auto state = new XorState(False);
    currentState = state;
    this->name = name;
    context = _context;
}

XorState * XorModel::deltaExternal(XorState * currentState, XorTokenBag * input, int insertionTime){
    cout << name << "DELTA EXT CALL" << endl;
    input->readyTime = insertionTime;

    auto receiver = subscribed[0];
    int outPutTime = insertionTime + triggerTime;

    //TODO turn this into a enum switch case declaration;
    auto memReceiver = static_cast<MemoryModule *>(receiver);

    auto out = new Output_Event<XorModel, XorToken, MemoryModule>(this, memReceiver, outPutTime, context);
    context->insert(out);
    cout << name << "DELTA EXT RETURN" << endl;
    return currentState->join(input);
}

XorState * XorModel::deltaInternal(XorState * currentState){
    return currentState->reduce();
}

XorToken * XorModel::lambda(XorState * currentState, int peekTime){
    xorIdentifier id = currentState->value;
    return new XorToken(peekTime, id);
}
