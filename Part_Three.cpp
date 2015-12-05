//
// Created by rtorres12 on 12/4/15.
//
#include "Processable_Token.hpp"
#include "State.hpp"
#include "Model.hpp"
#include "OutPut_Event.hpp"
#include "range.hpp"
#include "Log.hpp"

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



/**
 * All states contain the next trigger time. And a collection of tokens.
 * I : type of input.
 * O : type of processed inputs return type.
 */
class DrillState : public State<MetalDisk, MetalWasher> {
public :
    DrillState() {}
    ~DrillState(){
        cout << "Size of unprocessed" << unprocessed.size() << endl;
        while(!unprocessed.empty()){
            cout << "Size of unprocessed" << unprocessed.size() << endl;
            auto item = unprocessed.front();
            unprocessed.pop();
            delete item;
        }
        cout << "Drill state cleaned!" << endl;
    }
    DrillState * reduce(){
        auto removed = unprocessed.front();
        unprocessed.pop();
        delete removed;
        return this;
    };

    DrillState * join(MetalDisk * incomingInput){
        unprocessed.push(incomingInput);
        return this;
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
    ~PressState(){
        while(!unprocessed.empty()){
            auto item = unprocessed.front();
            unprocessed.pop();
            delete item;
        }
        cout << "Press state cleaned" << endl;
    }
    PressState * reduce(){
        auto removed = unprocessed.front();
        unprocessed.pop();
        delete removed;
        return this;
    };
    PressState * join(MetalBall * incomingInput){
        unprocessed.push(incomingInput);
        return this;
    }
};


/**
 * Press class accepts disks only. Test delta external first.
 */
class Drill : public Model<MetalDisk, MetalWasher, DrillState, Log> {
public :
    int triggerTime = 120;
    DrillState * currentState;

    void setState(DrillState *state){
        this->currentState = state;
    }

    Drill(DrillState * _defaultState, Event_Queue * _context){
        name = "Drill";
        currentState = _defaultState;
        context = _context;
    }

    ~Drill(){
        delete currentState;
        cout << "Drill machine cleaned" << endl;
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

        string aString = "[Log] Outputting Washer ";
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

    void setState(PressState *state){
        this->currentState = state;
    }

    Press(PressState * _defaultState, Event_Queue * _context){
        currentState = _defaultState;
        context = _context;
        name = "Press";
    }

    ~Press(){
        delete currentState;
        cout << "Press machine cleaned" << endl;
    }

    PressState * deltaExternal(PressState * state, MetalBall * input, int insertionTime){
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
                context->insert(new Output_Event<Press, MetalDisk, Drill>(this, sub, outputTime, context));
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

void PartThree(){
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

    delete drill;
    delete press;
    delete eq;
}
