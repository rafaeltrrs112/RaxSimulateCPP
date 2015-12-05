//
// Created by rtorres12 on 12/4/15.
//

#include "RouteState.hpp"
RouteState::RouteState(XorModel *innerModel, const vector<int> &toInner, const vector<int> &toByPass)
        : innerModel(innerModel), toInner(toInner), toByPass(toByPass)
{
    vector<XorToken *> x;
    this->value = new XorTokenBag(0, x);
}

RouteState * RouteState::reduce(){
    //Reduce the model's state and this one.
    auto state = innerModel->currentState;
    innerModel->deltaInternal(state);
    return this;
}

//Must be of size 3
RouteState * RouteState::join(XorTokenBag * incomingInput){
    int ready = incomingInput->readyTime;
    vector<XorToken *> input = incomingInput->xorTokens;
    vector<XorToken *> inserted = vector<XorToken *>();
    vector<XorToken *> byPass = vector<XorToken *>();

    for(int i : toInner){
        inserted.push_back(input[i]);
    }

    for(int j : toByPass){
        byPass.push_back(input[j]);
    }


    //By pass the counting and just insert the token into the model.
    innerModel->currentState = innerModel->currentState->join(new XorTokenBag(0, inserted));

    auto innerout = innerModel->lambda(innerModel->currentState, 0);
    byPass.push_back(innerout);
    value = new XorTokenBag(0, byPass);
    //Create output event for this model now.
    return this;
}

