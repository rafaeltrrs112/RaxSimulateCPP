//
// Created by rtorres12 on 12/4/15.
//

#ifndef RAXSIMULATE_XORTOKENBAG_HPP
#define RAXSIMULATE_XORTOKENBAG_HPP

#include "XorToken.hpp"
using namespace std;
#include <vector>
class XorTokenBag: public Token<xorIdentifier>{
public:
    vector<XorToken *> xorTokens;
    XorTokenBag(int _readyTime, vector<XorToken *> & xorTokens);
    XorToken * result(int readyTime);
    void println();
};


#endif //RAXSIMULATE_XORTOKENBAG_HPP
