//
// Created by rtorres12 on 12/4/15.
//

#include "XorToken.hpp"
XorToken::XorToken(int _readyTime, xorIdentifier _identity, bool identity) : Token(_readyTime), value(identity), identifier(_identity){
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
void XorToken::println(){
    cout << print;
}