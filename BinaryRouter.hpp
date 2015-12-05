//
// Created by rtorres12 on 12/4/15.
//

#ifndef RAXSIMULATE_BINARY_ROUTER_HPP
#define RAXSIMULATE_BINARY_ROUTE_HPP

template<typename L, typename R>
class BinaryRouter {
public :
    L leftContact;
    R rightContact;
    BinaryRouter(L leftContact, R rightContact) : leftContact(leftContact), rightContact(rightContact){ }
};

#endif //RAXSIMULATE_BINARY_ROUTE_HPP
