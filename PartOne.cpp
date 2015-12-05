//
// Created by rtorres on 12/5/15.
//

#include "Event_Queue.hpp"
#include "XorState.hpp"
#include "XorModel.hpp"
#include "RouteState.hpp"
#include "PassThroughRouter.hpp"
#include "MemoryState.hpp"
#include "MemoryModule.hpp"
#include "KickOffEvent.hpp"

class PartOne {
public :
    void start(){
        auto eq = new Event_Queue();

        auto xorOneState = new XorState(False);
        auto xorTwoState = new XorState(False);

        auto innerModel = new XorModel("XOR Inner", xorOneState, eq);

        auto toInner = vector<int>{0,1};
        auto toOuter = vector<int>{2};

        auto routeState = new RouteState(innerModel, toInner, toOuter);

        auto xorOne = new PassThroughRouter(routeState, eq);

        auto xorTwo = new XorModel("XOR 2", xorTwoState, eq);

        xorOne->subscribed.push_back(xorTwo);

        auto defaultMem = queue<XorToken *>();

        auto inOne = new XorToken(-1, True);
        auto inTwo = new XorToken(-1, False);

        defaultMem.push(inOne);
        defaultMem.push(inTwo);

        auto memState = new MemoryState(defaultMem);

        auto memoryModel = new MemoryModule("Memory Module", eq, memState);

        memoryModel->subscribed.push_back(xorOne);
        xorTwo->subscribed.push_back(memoryModel);

        auto kickOff = new KickOffEvent(0, memoryModel, eq);

        eq->insert(kickOff);
        eq->dump();

        delete innerModel;
        delete xorOne;
        delete xorTwo;
        delete memoryModel;

        delete eq;

    }
};
