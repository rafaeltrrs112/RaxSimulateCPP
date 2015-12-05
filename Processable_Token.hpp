//
// Created by rtorres12 on 12/4/15.
//

#ifndef RAXSIMULATE_PROCESSABLE_TOKEN_HPP
#define RAXSIMULATE_PROCESSABLE_TOKEN_HPP


#include "Token.hpp"
enum outputIdentifier {Ball,Disk,Washer};
template <class I, class O>
class ProcessableToken : public Token<I> {
    public:
    ProcessableToken(int _readyTime) : Token<outputIdentifier>(_readyTime) {
    }

    virtual O * processed() = 0;
};
#endif //RAXSIMULATE_PROCESSABLE_TOKEN_HPP
