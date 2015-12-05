//
// Created by rtorres12 on 12/4/15.
//

#ifndef RAXSIMULATE_ROUTESTATE_HPP
#define RAXSIMULATE_ROUTESTATE_HPP


#include "XorModel.hpp"

class RouteState : public State<XorTokenBag, XorTokenBag> {
public:
    RouteState(XorModel *innerModel, const vector<int> &toInner, const vector<int> &toByPass);

    XorModel * innerModel;

    vector<int> toInner;
    vector<int> toByPass;

    XorTokenBag * value;

    RouteState * reduce();

    //Must be of size 3
    RouteState * join(XorTokenBag * incomingInput);
};



#endif //RAXSIMULATE_ROUTESTATE_HPP
