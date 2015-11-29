//
// Created by rtorres12 on 11/18/15.
//

#include <iostream>
#include <queue>
#include <iomanip>
#include <vector>
#include <typeinfo>
#include "range.hpp"

using namespace std;
const string BALL = "Ball";
const string DISK = "Disk";
const string WASHER = "Washer";
enum outputIdentifier {Ball,Disk,Washer};
/**
 * TODO : Drill Press Model [1st Priority]
 * TODO : Event Handler [2nd Priority] : Event Spawn
 */

/**
 * Event class
 * @param millisecond
 *      The millisecond real time this event is set to occur at.
 */
class Event {
public:
    Event(int _ms){
        this->ms = _ms;
    }
    int get_milli_seconds(){
        return ms;
    }
    //executes this events enclosed action.
    virtual void commit() = 0;
private:
    int ms; // >= 0
};

template <class OP, class O, class RC>
class Output_Event : public Event {
public:
    OP * outPutter;
    RC * receiver;
    O * output;
    Output_Event(OP * _outPutter, RC * _receiver, int _ms) : Event(_ms){
        this->outPutter = _outPutter;
        this->receiver = _receiver;
    }
    void commit(){

        cout << endl << "Outputting out to my subscribers or the real world...at : " << this->get_milli_seconds() << endl;
        cout << "And committing changes to the model by calling it's delta internal... " << endl;

        auto currState = outPutter->currentState;
        auto output = outPutter->lambda(currState, this->get_milli_seconds());

        outPutter->currentState = outPutter->deltaInternal(outPutter->currentState);
        cout << "Outputted : "; output->println();
    }
};

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
        cout << "Passing an input to : " << this->receiver->name << " ...at : " << this->get_milli_seconds() << endl;
        receiver->deltaExternal(receiver->currentState, input, get_milli_seconds());
    }
};

class Log : public Event {
public :
    string message;
    Log(int _ms, string _message) : Event(_ms) {
        this->message = _message;
    }
    /*
     * Commit for a log simply prints stuff out.
     */
    void commit(){
        cout << message << endl;
    }
};

class Event_Comparator {
public:
    bool operator()(Event* t1, Event* t2)
    {
        return t1->get_milli_seconds() > t2->get_milli_seconds();
    }
};

class Event_Queue {
public:
    void insert(Event * e){
        cout << "Inserting" << endl;
        eventQueue.push(e);
    }
    void insert_all(vector<Event *> events){
        for(auto& event : events){
            insert(event);
        }
    }
    void dump(){
        while(!eventQueue.empty()){
            //Get the top value
            auto e = eventQueue.top();
            e->commit();
            eventQueue.pop();
            //Do a check here to see if this event should be allowed to commit or if a confluent join should occur.
        }


    }
    void log(string message){
        cout << message << endl;
    }
private:
    priority_queue<Event*, vector<Event*>, Event_Comparator> eventQueue;
};

string DrillPressDestruct(outputIdentifier typeString){
    switch(typeString){
        case Ball : return BALL;
        case Disk : return DISK;
        case Washer : return WASHER;
    }
}

//Token's receive a type. For now that is an enum.
template <class T>
class Token {
public:
    Token(int _readyTime){
        this->readyTime = _readyTime;
    }
    T identifier;
    int readyTime;
    virtual void println() = 0;
};

template <class I, class O>
class ProcessableToken : public Token<I> {
    public:
    ProcessableToken(int _readyTime) : Token<outputIdentifier>(_readyTime) {
    }

    virtual O * processed() = 0;
};

class MetalWasher : public ProcessableToken<outputIdentifier, MetalWasher> {
public:
    MetalWasher(int _readyTime) : ProcessableToken(_readyTime){}
    outputIdentifier identifier = Washer;

    void println(){
        cout << "MetalWasher [" << readyTime << "]" << endl;
    }

    MetalWasher * processed(){
        return new MetalWasher(readyTime);
    }
};

class MetalDisk : public ProcessableToken<outputIdentifier, MetalWasher> {
public:
    MetalDisk(int _readyTime) : ProcessableToken(_readyTime) {}
    outputIdentifier identifier = Disk;
    MetalWasher * processed(){
        return new MetalWasher(this->readyTime);
    }

    void println(){
        cout << "MetalDisk [" << this->readyTime << "]";
    }
};

class MetalBall : public ProcessableToken<outputIdentifier, MetalDisk> {
public:
    MetalBall(int _readyTime) : ProcessableToken(_readyTime) {}
    outputIdentifier identifier = Ball;
    MetalDisk * processed(){
        return new MetalDisk(this->readyTime);
    }

    void println(){
        cout << "MetalBall [" << this->readyTime << "]" << endl;
    }
};


//All inputs have an output type.
/**Dive into the Scala framework with this programming guide, created to help you learn Scala and to build intricate, modern, scalable concurrent applications

 * I : Input type.
 * O : Output type.
 * S : State type.
 */
template <typename I, typename O, typename S, typename SUB>
class Model {
public :
    Event_Queue * context;
    string name;
    int triggerTime;
    vector<SUB *> subscribed;
    S currentState;
    virtual S * deltaExternal(S * currentState, I * input, int insertionTime) = 0;
    virtual S * deltaInternal(S * currentState) = 0;
    virtual O * lambda(S * currentState, int peekTime) = 0;
};

/**
 * I : Input type.
 *
 * O : Output type.
 */
template <typename I, typename O>
class State {
public :

    //Make this a priority queue of drill press parts.
    queue<I *> unprocessed;
    //Called delta internal : assumes output can be created.
    virtual State * reduce() = 0;
    virtual State * join(I * incomingInput) = 0;
};

/**
 * All states contain the next trigger time. And a collection of tokens.
 * I : type of input.
 * O : type of processed inputs return type.
 */
class DrillState : public State<MetalDisk, MetalWasher> {
public :
    DrillState() {}

    DrillState * reduce(){
        unprocessed.pop();
        auto nextState = new DrillState();
        nextState->unprocessed = this->unprocessed;
        return nextState;
    };

    DrillState * join(MetalDisk * incomingInput){
        unprocessed.push(incomingInput);
        auto nextState = new DrillState();
        nextState->unprocessed = this->unprocessed;
        return nextState;
    }
};

/**
 * All states contain the next trigger time. And a collection of tokens.
 * I : type of input.
 * O : type of processed inputs return type.
 */
class PressState : public State<MetalBall, MetalDisk> {
public:
    PressState() {}

    PressState * reduce(){
        unprocessed.pop();
        auto nextState = new PressState();
        nextState->unprocessed = this->unprocessed;
        return nextState;
    };
    PressState * join(MetalBall * incomingInput){
        unprocessed.push(incomingInput);
        auto nextState = new PressState();
        nextState->unprocessed = this->unprocessed;
        return nextState;
    }
};


/**
 * Press class accepts disks only. Test delta external first.
 */
class Drill : public Model<MetalDisk, MetalWasher, DrillState, Log> {
public :
    int triggerTime = 120;
    DrillState * currentState;

    Drill(DrillState * _defaultState, Event_Queue * _context){
        name = "Drill";
        currentState = _defaultState;
        context = _context;
    }

    DrillState * deltaExternal(DrillState * state, MetalDisk * input, int insertionTime){
        int outputTime;
        if(currentState->unprocessed.size() > 0){
            int received = insertionTime;

            /*
             * Input was entered in the middle of a pressing action, therefore the output time
             * is 120 seconds after the ready time of the tail.
             */
            if(currentState->unprocessed.back()->readyTime > insertionTime){
                outputTime = triggerTime  + currentState->unprocessed.back()->readyTime;
                input -> readyTime = outputTime;
            } else {
                /*
                 * If the ready time of the tail is not greater than this item is arriving at a time where it will be the only one in the queue.
                 * It's output time is then simply it's insertion time plus trigger time.
                 */
                outputTime = triggerTime + insertionTime;
                input ->readyTime = outputTime;
            }
        } else {
            int received = insertionTime; //10
            outputTime = received + triggerTime;
            input->readyTime = outputTime;
        }

        string aString = "***********Outputting at time : ";
        aString.append(to_string(outputTime));
        context->insert(new Log(outputTime, aString));
        return state->join(input);
    }

    DrillState * deltaInternal(DrillState * state){
        return state->reduce();
    }

    MetalWasher * lambda(DrillState * state, int peekTime){
        /*
         * If the peekTime is equal to the ready time then lambda
         * is being polled at the right time so output something...
         */
        if(state->unprocessed.front()->readyTime == peekTime){
            return state->unprocessed.front()->processed();
        } else {
            throw -1;
        }
    }

    void println(){
        cout << "Unprocessed : "    << currentState->unprocessed.size()                << endl <<
                "Next output at : " << currentState->unprocessed.front()->readyTime    << endl;
    }
};

/**
 * Press class accepts disks only. Test delta external first.
 */
class Press : public Model<MetalBall, MetalDisk, PressState, Drill> {
public :
    int triggerTime = 120;
    PressState * currentState;

    Press(PressState * _defaultState, Event_Queue * _context){
        currentState = _defaultState;
        context = _context;
        name = "Press";
    }

    PressState * deltaExternal(PressState *state, MetalBall *input, int insertionTime){
        int outputTime;
        if(currentState->unprocessed.size() > 0){
            int received = insertionTime;

            /*
             * Input was entered in the middle of a pressing action, therefore the output time
             * is 120 seconds after the ready time of the tail.
             */
            if(currentState->unprocessed.back()->readyTime > insertionTime){
                outputTime = triggerTime  + currentState->unprocessed.back()->readyTime;
                input -> readyTime = outputTime;
            } else {
                /*
                 * If the ready time of the tail is not greater than this item is arriving at a time where it will be the only one in the queue.
                 * It's output time is then simply it's insertion time plus trigger time.
                 */
                outputTime = triggerTime + insertionTime;
                input ->readyTime = outputTime;
            }
        } else {
            int received = insertionTime; //10
            outputTime = received + triggerTime;
            input->readyTime = outputTime;
        }
        //If their are subscribers make an input event event for each.
        if(subscribed.size() > 0){
            for(auto& sub : subscribed){
                context->insert(new Output_Event<Press, MetalDisk, Drill>(this, sub, outputTime));
            }
        } else {
            //If their isn't then just make a log event and log it.
        }
        return state->join(input);
    }

    PressState * deltaInternal(PressState * state){
        return state->reduce();
    }

    MetalDisk * lambda(PressState * state, int peekTime){
        /*
         * If the peekTime is equal to the ready time then lambda
         * is being polled at the right time so output something...
         */
        if(state->unprocessed.front()->readyTime == peekTime){
            return state->unprocessed.front()->processed();
        } else {
            throw -1;
        }
    }

    void println(){
        cout << "Unprocessed : " << currentState->unprocessed.size() << endl <<
        "Next output at : " << currentState->unprocessed.front()->readyTime << endl;
    }
};

//T is token type acceptable
int main()
{
    Event_Queue * eq = new Event_Queue();

    DrillState * drillState = new DrillState();
    PressState * pressState = new PressState();

    Drill * drill = new Drill(drillState, eq);
    Press * press = new Press(pressState, eq);

    press->subscribed.push_back(drill);

    /**
     * Do this three times.
     */

    for(auto & i : range<int>(0, 3)){
        auto ball = new MetalBall(0);
        Input_Event<Press, MetalBall> * e = new Input_Event<Press, MetalBall>(press, ball, 10);
        eq->insert(e);
    }

    eq->dump();

    cout << drill -> currentState->unprocessed.size() << endl;

    return 0;
}