//
// Created by rtorres12 on 11/18/15.
//

#include <iostream>
#include <queue>
#include <iomanip>
#include <vector>
#include <typeinfo>
#include <functional>
#include "range.hpp"
#include "Event_Queue.hpp"

using namespace std;
const string BALL = "Ball";
const string DISK = "Disk";
const string WASHER = "Washer";
enum outputIdentifier {Ball,Disk,Washer};
enum valueIdentifier {Zero, One};
enum xorIdentifier {False, True};
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
        cout << "Passing an input"; input->println();  cout << "to [" << this->receiver->name << "] at [" << this->get_milli_seconds() << "]" << endl;

        receiver->setState(receiver->deltaExternal(receiver->currentState, input, get_milli_seconds()));
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

class Lambda_Event : public Event {
public :
    Lambda_Event(int _ms, function<void()> event) : Event(_ms) {}

    void commit(){
        event();
    }
private :
    function<void()> event;

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
    S * currentState;
    virtual S * deltaExternal(S * currentState, I * input, int insertionTime) = 0;
    virtual S * deltaInternal(S * currentState) = 0;
    virtual O * lambda(S * currentState, int peekTime) = 0;
    virtual void setState(S * state) = 0;
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

    virtual void setState(DrillState *state);

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

    virtual void setState(PressState *state);

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

    virtual void setState(CellState *state);

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
        cout << "[" <<snoozingCell->name << "]";
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

class XorToken : public Token<xorIdentifier > {
public:
    xorIdentifier identifier;
    bool value;
    XorToken(int _readyTime, xorIdentifier _identity, bool identity = false) : Token(_readyTime), value(identity), identifier(_identity) {
        switch(_identity){
            case True : {
                print = "[T]";
                value = true;
                break;
            }
            case False : {
                print = "[F]";
                value = false;
                break;
            }
            default : {
                throw 99;
            }
        }
    }
    void println(){
        cout << print;
    }

private:
    string print;
};

class XorTokenBag: public Token<xorIdentifier>{
public:
    vector<XorToken *> xorTokens;
    XorTokenBag(int _readyTime, vector<XorToken *> & xorTokens) : Token(_readyTime) {
        int count = 0;
        for(auto& token : xorTokens){
            switch(token->identifier){
                case True : {
                    count++;
                    break;
                }
                case False :
                default: break;
            }
        }
        identifier = (count % 2 == 0) ? False : True;
        this->xorTokens = xorTokens;
    }
    XorToken * result(int readyTime){
        return identifier == False ? new XorToken(readyTime, False) : new XorToken(readyTime, True);
    }
    void println(){
        for(auto token : xorTokens){
            token->println();
        }
    }
};

class XorState : public State<XorTokenBag, XorToken> {
public:
    XorState(){};
    xorIdentifier value;
    XorState(xorIdentifier _value) : value(_value){}
    //Make this a priority queue of drill press parts.
    //Called delta internal : assumes output can be created.
    XorState * reduce(){
        this->value = False;
        return this;
    }

    XorState * join(XorTokenBag * incomingInput){
        this->value = incomingInput->identifier;
        return this;
    }
};

class XorModel : public Model<XorTokenBag, XorToken, XorState, XorModel> {
public:
    Event_Queue * context;
    string name;
    int triggerTime = 10;
    XorState * currentState;

    virtual void setState(XorState *state);

public:
    XorModel(string name, XorState * defaultState, Event_Queue * _context){
        currentState = defaultState;
        this->name = name;
        context = _context;
        subscribed = vector<XorModel*>();
    }

    XorModel(string name, Event_Queue * _context){
        auto state = new XorState(False);
        currentState = state;
        this->name = name;
        context = _context;
    }

    XorState * deltaExternal(XorState * currentState, XorTokenBag * input, int insertionTime){
        cout << name << "DELTA EXT CALL" << endl;
        input->readyTime = insertionTime;

        auto receiver = subscribed[0];
        int outPutTime = insertionTime + triggerTime;

        //TODO
        function<void()> event = [](){
            auto defaultMem = queue<XorToken *>();

            auto inOne = new XorToken(-1, True);
            auto inTwo = new XorToken(-1, False);

            defaultMem.push(inOne);
            defaultMem.push(inTwo);
            cout << "3" << endl;

            MemoryState * memState = new MemoryState(defaultMem);
            cout << "4" << endl;

            auto memoryModel = new MemoryModule("Memory Module", new Event_Queue(), memState);
        };

        cout << name << "DELTA EXT RETURN" << endl;
        return currentState->join(input);
    }

    XorState * deltaInternal(XorState * currentState){
        return currentState->reduce();
    }

    XorToken * lambda(XorState * currentState, int peekTime){
        xorIdentifier id = currentState->value;
        return new XorToken(peekTime, id);
    }
};

class RouteState : public State<XorTokenBag, XorTokenBag> {
public:
    RouteState(XorModel *innerModel, const vector<int> &toInner, const vector<int> &toByPass)
            : innerModel(innerModel), toInner(toInner), toByPass(toByPass)
    {
        vector<XorToken *> x;
        this->value = new XorTokenBag(0, x);
    }

    XorModel * innerModel;

    vector<int> toInner;
    vector<int> toByPass;

    XorTokenBag * value;

    RouteState * reduce(){
        //Reduce the model's state and this one.
        auto state = innerModel->currentState;
        innerModel->deltaInternal(state);
        return this;
    }

    //Must be of size 3
    RouteState * join(XorTokenBag * incomingInput){
        int ready = incomingInput->readyTime;
        vector<XorToken *> input = incomingInput->xorTokens;
        vector<XorToken *> inserted = vector<XorToken *>();
        vector<XorToken *> byPass = vector<XorToken *>();

        for(int i : toInner){
            inserted.push_back(input[i]);
        }

        for(int j : toByPass){
            byPass.push_back(input[j]);
        }


        //By pass the counting and just insert the token into the model.
        innerModel->currentState = innerModel->currentState->join(new XorTokenBag(0, inserted));

        auto innerout = innerModel->lambda(innerModel->currentState, 0);
        byPass.push_back(innerout);
        value = new XorTokenBag(0, byPass);
        //Create output event for this model now.
        return this;
    }
};

class PassThroughRouter : public Model<XorTokenBag, XorTokenBag, RouteState, XorModel> {
public:
    Event_Queue * context;

    string name = "Router";
    int triggerTime = 10;

    virtual void setState(RouteState *state);

    vector<XorModel *> subscribed = vector<XorModel *>();

    RouteState * currentState;

    PassThroughRouter(RouteState * defaultState, Event_Queue * _context){
        this->context = _context;
        currentState = defaultState;
    }

    RouteState * deltaExternal(RouteState * currentState, XorTokenBag * input, int insertionTime){
        input->readyTime = insertionTime;

        int outputTime = insertionTime + triggerTime;
        auto receiver = subscribed[0];

        auto outputEvent = new Output_Event<PassThroughRouter, XorTokenBag, XorModel>(this, receiver, outputTime, context);
        cout << name << " SUBS [" << subscribed[0]->name << "]" << endl;
        cout << "\nOE at [" << outputTime << "]" << endl;
        cout << " IS "; input->println(); cout << endl;

        context->insert(outputEvent);
        return currentState->join(input);
    };

    RouteState * deltaInternal(RouteState * currentState){
        return currentState->reduce();
    }

    XorTokenBag * lambda(RouteState * currentState, int peekTime){
        auto input = currentState->value;
        input->readyTime = peekTime;
        return input;
    }

};

class MemoryState : public State<XorToken, XorToken> {
public :
    queue<XorToken *> state = queue<XorToken *>();

    MemoryState(queue<XorToken *> &state) : state(state) { }

    MemoryState * join(XorToken * input){
        state.push(input);
        return this;
    }
    MemoryState * reduce(){
        state.pop();
        return this;
    }
};

class MemoryModule : public Model<XorToken, XorToken, MemoryState, PassThroughRouter>, public XorModel {
public :
    Event_Queue * context;

    virtual void setState(MemoryState *state);

    string name = "MEM MODULE";

    int triggerTime = 10;

    vector<PassThroughRouter *> subscribed = vector<PassThroughRouter *>();
    MemoryState * currentState;

    MemoryModule(string name, Event_Queue * context, MemoryState * _currentState) : XorModel(name, context){
        this->currentState = _currentState;
        this->context = context;
    }

    virtual void setState(XorState *state);

    MemoryState * deltaExternal(MemoryState * currentState, XorToken * input, int insertionTime){
        return this->currentState->join(input);
    }

    MemoryState * deltaInternal(MemoryState * currentState){
        return this->currentState->reduce();
    }

    XorToken * lambda(MemoryState * currentState, int peekTime){
        return this->currentState->state.front();
    }
};

/**
 * Launch event.
 */
class KickOffEvent : public Event {
public:
    MemoryModule * memoryModule;
    Event_Queue * context;
    KickOffEvent(int _ms, MemoryModule * _memoryModule, Event_Queue * _context) : Event(_ms) {
        this->context = _context;
        this->memoryModule = _memoryModule;
    }

    void commit(){
        int inputOne;
        int inputTwo;

        cout << "Enter input one" << endl;
        cin >> inputOne;

        cout << "Enter input two" << endl;
        cin >> inputTwo;

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
};

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

void MemoryModule::setState(MemoryState *state) {
    this->currentState = state;
}

void Drill::setState(DrillState * state) {
    this->currentState = state;
}

void Press::setState(PressState *state) {
    this->currentState = state;

}

void Cell::setState(CellState *state) {
    this->currentState = state;

}

void XorModel::setState(XorState *state) {
    this->currentState = state;

}

void PassThroughRouter::setState(RouteState *state) {
    this->currentState = state;

}

void MemoryModule::setState(XorState * state) {
}
