//
// Created by rtorres12 on 12/4/15.
//

#include "Model.hpp"
#include "State.hpp"
#include "Token.hpp"
#include "Input_Event.hpp"
#include "BinaryRouter.hpp"

enum valueIdentifier {Zero, One};

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

    void setState(CellState *state){
        this->currentState = state;
    }

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

//Instructs a cell to update it's self value
class Update_Event : public Event {
public:
    Cell * shouterCell;
    Event_Queue * context;
    Update_Event(Cell * shouterCell, int _ms, Event_Queue * context) : Event(_ms), shouterCell(shouterCell) {
        this->context = context;
    }
    void commit(){
        cout << "Updating [" << shouterCell-> name << "] at [" << get_milli_seconds() << "]" << endl;
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
        cout << "Waking [" << snoozingCell-> name << "] at [" << get_milli_seconds() << "]" << endl;

        int left = snoozingCell->binaryRouter->leftContact->currentState->selfValue;
        int right = snoozingCell->binaryRouter->rightContact->currentState->selfValue;

        auto input = new CellInput(get_milli_seconds(), left, right);
        Input_Event<Cell, CellInput> * e = new Input_Event<Cell, CellInput>(snoozingCell, input, get_milli_seconds());

        context->insert(e);

        int rightWakeTime = snoozingCell->tailer ? get_milli_seconds() + 1 : get_milli_seconds();
        cout << "[" <<snoozingCell-> name << "]";
        auto rightWake = new Wake_Event(snoozingCell->binaryRouter->rightContact, rightWakeTime, context);
        context->insert(rightWake);

        auto update_event = new Update_Event(snoozingCell, get_milli_seconds() + 1, context);
        context->insert(update_event);
        getchar();
    }

};

void partTwo(){
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


