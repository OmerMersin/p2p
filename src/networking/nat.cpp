//
// Created by Omer Mersin on 11/16/24.
//
#include "networking/nat.h"
#include <iostream>

void NAT::punchHole(const std::string &ip, int port) {
    std::cout << "Attempting NAT hole punching for " << ip << ":" << port << std::endl;
    // Add hole punching logic here (e.g., using UDP).
}