//
// Created by rtorres12 on 12/4/15.
//

#ifndef RAXSIMULATE_INPUT_EVENT_HPP
#define RAXSIMULATE_INPUT_EVENT_HPP

#include <array>
#include <iostream>
#include "Event.hpp"
using namespace std;
template <class M, class I>
class Input_Event : public Event {
public:
    M * receiver;
    I * input;

    Input_Event(M * _receiver, I * _input, int _ms) : Event(_ms){
        this->receiver = _receiver;
        this->input = _input;
    }

    /*
     * An input being committed is the input being passed into the receiver throught delta external,
     * and it's state being equated to deltaExternal's output.
     */
    void commit(){
    cout << "Passing an input"; input->println();  cout << "to [" << this->receiver->name << "] at [" << this->get_milli_seconds() << "]" << endl;

    receiver->setState(receiver->deltaExternal(receiver->currentState, input, get_milli_seconds()));
    }

};
#endif //RAXSIMULATE_INPUT_EVENT_HPP
