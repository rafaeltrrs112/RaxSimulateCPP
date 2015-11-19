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

class Output_Event : public Event {
public:
    Output_Event(int _ms) : Event(_ms){}
    void commit(){
        cout << "Outputting out to my subscribers or the real world...at: " << this->get_milli_seconds() << endl;
    }
};

class Input_Event : public Event {
public:
    Input_Event(int _ms) : Event(_ms){}
    void commit(){
        cout << "Passing an input to a Model...at: " << this->get_milli_seconds() << endl;
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
    void insert(Event* e){
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
     T identifier;
};

template <class I, class O>
class ProcessableToken : public Token<I> {
    virtual O * processed() = 0;
};

class MetalWasher : public Token<outputIdentifier> {
public:
    MetalWasher(){}
    outputIdentifier identifier = Washer;
};

class MetalDisk : public ProcessableToken<outputIdentifier, MetalWasher> {
public:
    MetalDisk(){}
    outputIdentifier identifier = Disk;
    MetalWasher * processed(){
        return new MetalWasher();
    }
};

class MetalBall : public ProcessableToken<outputIdentifier, MetalDisk> {
public:
    MetalBall(){}
    outputIdentifier identifier = Ball;
    MetalDisk * processed(){
        return new MetalDisk();
    }
};


//All inputs have an output type.
/**
 * I : Input type.
 * O : Output type.
 * S : State type.
 */
template <typename I, typename O, typename S>
class Model{
public :
    int triggerTime;
    virtual S deltaExternal(S currentState, I input, int insertionTime) = 0;
    virtual S deltaInternal(S currentState) = 0;
    virtual O lambda(S currentState) = 0;
};

/**
 * A model of a drill or press.
 */
class DrillPress : public Model<int, int, int>{
public :
    int triggerTime = 120;
    int deltaExternal(int currentState, int input, int insertionTime){
        if(currentState == 100) return 999;
        else return -1;
    }
    int lambda(int currentState){
        return 112;
    }
    int deltaInternal(int currentState){
        return 190;
    }
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
     State * reduce(){
         unprocessed.pop();
         auto nextState = new DrillState();
         nextState->unprocessed = this->unprocessed;
         return nextState;
     };
     State * join(MetalDisk * incomingInput){
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
    State * reduce(){
        unprocessed.pop();
        auto nextState = new PressState();
        nextState->unprocessed = this->unprocessed;
        return nextState;
    };
    State * join(MetalBall * incomingInput){
        unprocessed.push(incomingInput);
        auto nextState = new PressState();
        nextState->unprocessed = this->unprocessed;
        return nextState;
    }
};


//T is token type acceptable
int main()
{
    auto pressState = new PressState;

    for(auto& i : range<int>(0,5)){
        auto metalBall = new MetalBall();
        pressState->unprocessed.push(metalBall);
    }

    for(auto& i : range<int>(0,5)){
        auto metalBall = new MetalBall();
        pressState = static_cast<PressState *> (pressState->join(metalBall));
    }

    cout << pressState->reduce()->reduce()->reduce()->unprocessed.size() << endl;

//    auto eventQueue = new Event_Queue();
//    auto outputEvent = new Output_Event(89);
//    auto outputEventTwo = new Output_Event(34);
//    vector<Event *> testEvent;
//    testEvent.push_back(outputEvent);
//    testEvent.push_back(outputEventTwo);
//    eventQueue->insert_all(testEvent);
//    auto drillPress = new DrillPress();
//    cout << drillPress->deltaExternal(100,0,0) << endl;
    return 0;
}