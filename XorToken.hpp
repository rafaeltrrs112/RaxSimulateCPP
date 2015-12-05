//
// Created by rtorres12 on 12/4/15.
//

#ifndef RAXSIMULATE_XORTOKEN_HPP
#define RAXSIMULATE_XORTOKEN_HPP
using namespace std;
enum xorIdentifier {False, True};

#include <iostream>
#include "Token.hpp"

class XorToken : public Token<xorIdentifier> {
public:
    xorIdentifier identifier;
    bool value;
    XorToken(int _readyTime, xorIdentifier _identity, bool identity = false);
    void println();
private:
    string print;
};

#endif //RAXSIMULATE_XORTOKEN_HPP
