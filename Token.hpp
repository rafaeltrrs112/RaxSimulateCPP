//
// Created by rtorres12 on 12/4/15.
//

#ifndef RAXSIMULATE_TOKEN_HPP
#define RAXSIMULATE_TOKEN_HPP
//Token's receive a type. For now that is an enum.
using namespace std;
#include <iostream>

template <class T>
class Token {
public:
    string print;
    Token(int _readyTime){
        this->readyTime = _readyTime;
    }
    T identifier;
    int readyTime;
    virtual void println(){
        cout << print;
    }
};
#endif //RAXSIMULATE_TOKEN_HPP
