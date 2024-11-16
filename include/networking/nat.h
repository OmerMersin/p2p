//
// Created by Omer Mersin on 11/16/24.
//

#ifndef NAT_H
#define NAT_H

#include <string>

class NAT {
public:
    void punchHole(const std::string &ip, int port);
};

#endif