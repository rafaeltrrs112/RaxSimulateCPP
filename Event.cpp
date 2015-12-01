/**
 * CPP file for event class.
 */
#include "Event.hpp"

Event::Event(int _ms) {
    this->ms = _ms;
}

int Event::get_milli_seconds() {
    return this->ms;
}