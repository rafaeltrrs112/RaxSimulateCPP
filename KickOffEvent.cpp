//
// Created by rtorres12 on 12/4/15.
//

#include "KickOffEvent.hpp"

KickOffEvent::KickOffEvent(int _ms, MemoryModule * _memoryModule, Event_Queue * _context) : Event(_ms) {
    this->context = _context;
    this->memoryModule = _memoryModule;
}



void KickOffEvent::commit(){
    int inputOne;
    int inputTwo;

    cout << "Enter input one" << endl;
    cin >> inputOne;


    cout << "Enter input two" << endl;
    cin >> inputTwo;

    if(inputOne == -1 || inputTwo == -1){
        return;
    }

    XorToken * tokenOne = inputOne == 0 ? new XorToken(get_milli_seconds(), False, false) : new XorToken(get_milli_seconds(), True, true) ;
    XorToken * tokenTwo = inputTwo == 0 ? new XorToken(get_milli_seconds(), False, false) : new XorToken(get_milli_seconds(), True, true) ;

    XorToken * tokenThree = memoryModule->lambda(memoryModule->currentState, -1);

    vector<XorToken *> inputs;
    inputs.push_back(tokenOne);
    inputs.push_back(tokenTwo);
    inputs.push_back(tokenThree);

    //tokenOne->println();tokenTwo->println();tokenThree->println();

    for(auto& i : inputs){
        i->println() ; cout << endl;
    }

    auto insert = new XorTokenBag(0, inputs);
    auto targetModel = memoryModule->subscribed[0];

    int nextTime = get_milli_seconds();

    auto inEvent = new Input_Event<PassThroughRouter, XorTokenBag>(targetModel, insert, nextTime);

    cout << "\nIE at [" << nextTime << "]" << endl;

    context->insert(inEvent);
}
