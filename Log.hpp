//
// Created by rtorres12 on 12/4/15.
//

#ifndef RAXSIMULATE_LOG_HPP
#define RAXSIMULATE_LOG_HPP


#include "Event.hpp"
using namespace std;
class Log : public Event {
public :
    string message;
    Log(int _ms, string _message);
    ~Log();
    /*
     * Commit for a log simply prints stuff out.
     */
    void commit();
};


#endif //RAXSIMULATE_LOG_HPP
