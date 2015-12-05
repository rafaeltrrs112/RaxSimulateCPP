//
// Created by rtorres12 on 12/4/15.
//

#include "XorTokenBag.hpp"
XorTokenBag::XorTokenBag(int _readyTime, vector<XorToken *> & xorTokens) : Token(_readyTime) {
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

XorToken * ::XorTokenBag::result(int readyTime){
    return identifier == False ? new XorToken(readyTime, False) : new XorToken(readyTime, True);
}

void XorTokenBag::println(){
    for(auto token : xorTokens){
        token->println();
    }
}