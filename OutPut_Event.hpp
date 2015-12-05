//
// Created by rtorres12 on 12/4/15.
//

#ifndef RAXSIMULATE_OUTPUT_EVENT_HPP
#define RAXSIMULATE_OUTPUT_EVENT_HPP

#include "Event.hpp"
#include "Event_Queue.hpp"
#include "Input_Event.hpp"

template <class OP, class O, class RC>
class Output_Event : public Event {
public:
    OP * outPutter;
    RC * receiver;
    O * output;
    Event_Queue * context;

    Output_Event(OP * _outPutter, RC * _receiver, int _ms, Event_Queue * _context) : Event(_ms){
        outPutter = _outPutter;
        receiver = _receiver;
        context = _context;
        cout << receiver->name << endl;
    }
    void commit(){

        auto currState = outPutter->currentState;
        auto output = outPutter->lambda(currState, this->get_milli_seconds());

        cout << outPutter->name << " outputted "; output->println(); cout << " TIME [" << get_milli_seconds() << "]"<< endl;
        Input_Event<RC, O> * input_event;
        input_event = new Input_Event<RC, O>(receiver, output, this->get_milli_seconds());
        context->insert(input_event);
        outPutter->currentState = outPutter->deltaInternal(outPutter->currentState);
    }
};
#endif //RAXSIMULATE_OUTPUT_EVENT_HPP
