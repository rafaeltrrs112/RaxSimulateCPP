//
// Created by rtorres12 on 11/18/15.
//

#include "Model.hpp"
#include "range.hpp"
#include <iostream>
#include <queue>
#include <iomanip>

using namespace std;

/**
 * Event class
 * @param millisecond
 *      The millisecond real time this event is set to occur at.
 */
class Event {
public:
    Event(int _ms){
        this->ms = _ms;
    }
    int get_milli_seconds(){
        return ms;
    }
    //executes this events enclosed action.
    virtual void commit() = 0;
private:
    int ms; // >= 0
};

class Output_Event : public Event {
public:
    Output_Event(int _ms) : Event(_ms){}
    void commit(){
        cout << "Outputting out to my subscribers or the real world...";
    }
};

class Input_Event : public Event {
public:
    Input_Event(int _ms) : Event(_ms){}
    void commit(){
        cout << "Passing an input to a Model...";
    }
};

class Event_Queue {
public:
    void insert(Event& e){
        //TODO insert one event
    }
    void insert_all(vector<Event &> events){
        for(auto& event : events){
            insert(event);
        }
    }
    void dump(){
        while(!event_queue->empty()){
            //Get the top value
            auto e = dequeue();

            //Do a check here to see if this event should be allowed to commit or if a confluent join should occur.
        }
    }
private:
    auto event_queue = new priority_queue<Event*, vector<Event*>, Event_Comparator>();
    Event* dequeue(){
        auto next = event_queue->top();
        event_queue->pop();
        return next;
    }

};


class Event_Comparator {
public:
    bool operator()(Event* t1, Event* t2)
    {
        return t1->get_milli_seconds() > t2->get_milli_seconds();
    }
};

class Model{
public:
private:
};

int main()
{
    priority_queue<Event*, vector<Event*>, Event_Comparator> pq;
    pq.push(new Output_Event(12));
    while (! pq.empty()) {
        //In order to use an interface you need to use a pointer of the interfaces type to call virtual methods.
        auto t2 = pq.top();
        cout << setw(3) << t2->get_milli_seconds() << " " << setw(3) << " " << endl;
        pq.pop();
    }


    return 0;
}