//
// Created by rtorres12 on 11/18/15.
//

#include <iostream>
#include <queue>
#include <iomanip>
#include <vector>
#include <functional>
#include "range.hpp"
#include "Event_Queue.hpp"
#include "BinaryRouter.hpp"
#include "Input_Event.hpp"
#include "OutPut_Event.hpp"
#include "Token.hpp"
#include "Processable_Token.hpp"
#include "Model.hpp"
#include "State.hpp"
#include "MemoryModule.hpp"
#include "KickOffEvent.hpp"

using namespace std;
const string BALL = "Ball";
const string DISK = "Disk";
const string WASHER = "Washer";

/**
 * TODO : Drill Press Model [1st Priority]
 * TODO : Event Handler [2nd Priority] : Event Spawn
 */



class Lambda_Event : public Event {
public :
    Lambda_Event(int _ms, function<void()> event) : Event(_ms) {}

    void commit(){
        event();
    }
private :
    function<void()> event;
};
/**
 * Launch event.
 */

void PartOne(){
    auto eq = new Event_Queue();

    auto xorOneState = new XorState(False);
    auto xorTwoState = new XorState(False);

    auto innerModel = new XorModel("XOR Inner", xorOneState, eq);
    cout << "1" << endl;

    auto toInner = vector<int>{0,1};
    auto toOuter = vector<int>{2};
    cout << "2" << endl;

    auto routeState = new RouteState(innerModel, toInner, toOuter);
    cout << "2" << endl;

    auto xorOne = new PassThroughRouter(routeState, eq);

    auto xorTwo = new XorModel("XOR 2", xorTwoState, eq);

    xorOne->subscribed.push_back(xorTwo);

    auto defaultMem = queue<XorToken *>();

    auto inOne = new XorToken(-1, True);
    auto inTwo = new XorToken(-1, False);

    defaultMem.push(inOne);
    defaultMem.push(inTwo);
    cout << "3" << endl;

    auto memState = new MemoryState(defaultMem);
    cout << "4" << endl;

    auto memoryModel = new MemoryModule("Memory Module", eq, memState);

    cout << "5" << endl;
    memoryModel->subscribed.push_back(xorOne);
    xorTwo->subscribed.push_back(memoryModel);
    cout << "6" << endl;
    auto kickOff = new KickOffEvent(0, memoryModel, eq);

    eq->insert(kickOff);

    eq->dump();
}

int main()
{
    PartOne();
    return 0;
}
