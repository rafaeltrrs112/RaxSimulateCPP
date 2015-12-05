//
// Created by rtorres12 on 12/4/15.
//

#include <array>
#include "Log.hpp"
#include "Event.hpp"
using namespace std;
#include "iostream"
Log::Log(int _ms, string _message) : Event(_ms) {
    this->message = _message;
}
Log::~Log(){
    cout << "Cleaning Log!" << endl;
}
/*
 * Commit for a log simply prints stuff out.
 */
void Log::commit(){
    cout << message << endl;
}
