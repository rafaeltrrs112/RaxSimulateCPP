//
// Created by rtorres12 on 11/18/15.
//

#include <iostream>
#include <queue>
#include <iomanip>
#include <vector>
#include <typeinfo>
#include "range.hpp"
#include "Event_Queue.hpp"

using namespace std;
const string BALL = "Ball";
const string DISK = "Disk";
const string WASHER = "Washer";
enum outputIdentifier {Ball,Disk,Washer};
enum valueIdentifier {Zero, One};
/**
 * TODO : Drill Press Model [1st Priority]
 * TODO : Event Handler [2nd Priority] : Event Spawn
 */


template<typename L, typename R>
class BinaryRouter {
public :
    L leftContact;
    R rightContact;
    BinaryRouter(L leftContact, R rightContact) : leftContact(leftContact), rightContact(rightContact){ }
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
        //cout << "Passing an input to [" << this->receiver->name << "] at [" << this->get_milli_seconds() << "]" << endl;

        receiver->currentState = receiver->deltaExternal(receiver->currentState, input, get_milli_seconds());
    }
};

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
    }
    void commit(){

        auto currState = outPutter->currentState;
        auto output = outPutter->lambda(currState, this->get_milli_seconds());


        cout << outPutter->name << " outputted "; output->println(); cout << endl;
        Input_Event<RC, O> * input_event;
        input_event = new Input_Event<RC, O>(receiver, output, this->get_milli_seconds());
        context->insert(input_event);
        outPutter->currentState = outPutter->deltaInternal(outPutter->currentState);
    }
};


class Log : public Event {
public :
    string message;

    Log(int _ms, string _message);
    ~Log(){
        cout << "Cleaning Log!" << endl;
    }
    /*
     * Commit for a log simply prints stuff out.
     */
    void commit(){
        cout << message << endl;
    }
};

Log::Log(int _ms, string _message) : Event(_ms) {
    this->message = _message;
}

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

//input is either 1 or negative one
class CellInput : public Token<valueIdentifier> {
public:
    int left;
    int right;

    CellInput(int _readyTime, int _left, int _right) :
            Token(_readyTime), left(_left), right(_right) {}
    void println(){}
};

class CellOutput : public Token<valueIdentifier> {
public :
    int value;
    CellOutput(int _readyTime, int _value) :
    Token(_readyTime), value(_value){}
    void println(){}
};

class CellState : public State<CellInput, CellOutput> {
public:
    int selfValue;
    int left = -1;
    int right = -1;

    CellState(int _defaultState = 0) : selfValue(_defaultState){
        left = 0;
        right = 0;
    }

    CellState * reduce(){
        auto newState = new CellState();
        switch (selfValue) {
            case 0 : {
                if(left == 1 && right == 1){
                    selfValue = 1;
                }
                else if(left == 1 & right == 0){
                    selfValue = 0;
                }
                else if(left == 0 && right == 1){
                    selfValue = 1;
                } else {
                    selfValue = 0;
                }
                break;
            }
            case 1 : {
                if(left == 1 && right == 1){
                    selfValue = 0;
                }
                else if(left == 1 & right == 0){
                    selfValue = 1;
                }
                else if(left == 0 && right == 1){
                    selfValue = 1;
                } else {
                    selfValue = 0;
                }
                break;
            }
            default : throw 9000;
        }
        return this;
    }

    CellState * join(CellInput * incomingInput){
        int incomingLeft = incomingInput->left;
        int incomingRight = incomingInput->right;
        left = incomingLeft;
        right = incomingRight;
        return this;
    }
};

class Cell : public Model<CellInput, CellOutput, CellState, Cell>  {
public :
    CellState * currentState;
    BinaryRouter<Cell *, Cell *> * binaryRouter;

    bool tailer;

    Cell(CellState * _defaultState, Event_Queue * _context){
        currentState = _defaultState;
        context = _context;
    }

    CellState * deltaExternal(CellState * currentState, CellInput * input, int insertionTime){
        auto newState = currentState->join(input);
        return newState;
    }

    CellState * deltaInternal(CellState * currentState){
        auto newState = currentState->reduce();
        return currentState;
    }

    CellOutput * lambda(CellState * currentState, int peekTime){
        return new CellOutput(peekTime, currentState->selfValue);
    }
};

void PartThree(){
    //TODO Handle confluent case...just a simple ordered swap...

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

//Instructs a cell to update it's self value
class Update_Event : public Event {
public:
    Cell * shouterCell;
    Event_Queue * context;
    Update_Event(Cell * shouterCell, int _ms, Event_Queue * context) : Event(_ms), shouterCell(shouterCell) {
        this->context = context;
    }
    void commit(){
        cout << "Updating [" << shouterCell->name << "] at [" << get_milli_seconds() << "]" << endl;
        //Update the cell's state...and output the current output.
        shouterCell->currentState = shouterCell->deltaInternal(shouterCell->currentState);
        if(shouterCell->tailer){
            cout << shouterCell->lambda(shouterCell->currentState, get_milli_seconds())->value << endl;
        } else {
            cout << shouterCell->lambda(shouterCell->currentState, get_milli_seconds())->value;
        }
    }
};

//Updates a cell's left and right values and wakes up the right cell.
class Wake_Event : public Event {
public:
    Input_Event<Cell, CellInput> * buildFrom;
    Cell * snoozingCell;
    Event_Queue * context;
    Wake_Event(Cell * snoozingCell, int _ms, Event_Queue * context) : Event(_ms), buildFrom(buildFrom), snoozingCell(snoozingCell) {
        this->context = context;
    }

    void commit(){
        cout << "Waking [" << snoozingCell->name << "] at [" << get_milli_seconds() << "]" << endl;

        int left = snoozingCell->binaryRouter->leftContact->currentState->selfValue;
        int right = snoozingCell->binaryRouter->rightContact->currentState->selfValue;

        auto input = new CellInput(get_milli_seconds(), left, right);
        Input_Event<Cell, CellInput> * e = new Input_Event<Cell, CellInput>(snoozingCell, input, get_milli_seconds());

        context->insert(e);

        int rightWakeTime = snoozingCell->tailer ? get_milli_seconds() + 1 : get_milli_seconds();
        cout << "[" <<snoozingCell->name << "]"
        auto rightWake = new Wake_Event(snoozingCell->binaryRouter->rightContact, rightWakeTime, context);
        context->insert(rightWake);

        auto update_event = new Update_Event(snoozingCell, get_milli_seconds() + 1, context);
        context->insert(update_event);
        getchar();
    }

};

void PartTwo(){
    auto eq = new Event_Queue();

    CellState * defaultState = new CellState(1);

    Cell * cellOne = new Cell(defaultState, eq);
    Cell * cellTwo = new Cell(new CellState(0), eq);
    Cell * cellThree = new Cell(new CellState(0), eq);

    cellThree->tailer = false;
    cellOne->tailer = true;
    cellOne->tailer = true;

    auto routeOne = new BinaryRouter<Cell *, Cell *>(cellThree, cellTwo);
    auto routeTwo = new BinaryRouter<Cell *, Cell *>(cellOne, cellThree);
    auto routeThree = new BinaryRouter<Cell *, Cell *>(cellTwo, cellOne);

    cellOne->binaryRouter = routeOne;
    cellTwo->binaryRouter = routeTwo;
    cellThree->binaryRouter = routeThree;

    cellOne->name = "Cell One";
    cellTwo->name = "Cell Two";
    cellThree->name = "Cell Three";


    //Input event is a wake event
    auto e = new Wake_Event(cellOne, 0, eq);

    eq->insert(e);

    cout << "Left value : " << cellOne->currentState->left << "\nRight value : " << cellOne->currentState->right << endl;
    eq->dump();
    cout << "Left value : " << cellOne->currentState->left << "\nRight value : " << cellOne->currentState->right << endl;

}
int main()
{
    PartTwo();
    return 0;
}