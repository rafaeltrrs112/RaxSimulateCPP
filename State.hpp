//
// Created by rtorres12 on 12/4/15.
//

#ifndef RAXSIMULATE_STATE_HPP
#define RAXSIMULATE_STATE_HPP
/**
 * I : Input type.
 *
 * O : Output type.
 */
using namespace std;
#include <queue>
template <typename I, typename O>
class State {
public :
    //Make this a priority queue of drill press parts.
    queue<I *> unprocessed;
    //Called delta internal : assumes output can be created.
    virtual State * reduce() = 0;
    virtual State * join(I * incomingInput) = 0;
};
#endif //RAXSIMULATE_STATE_HPP
