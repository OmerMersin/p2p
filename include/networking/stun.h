//
// Created by Omer Mersin on 11/16/24.
//

#ifndef STUN_H
#define STUN_H

#include <string>
#include <boost/asio.hpp>

class STUN {
public:
    // Retrieve public IP and port from the STUN server
    static std::pair<std::string, int> getPublicAddress(const std::string &stunServer, int port);
};

#endif