//
// Created by Omer Mersin on 11/16/24.
//

#ifndef DHT_H
#define DHT_H

#include <string>
#include <unordered_map>

class DHT {
private:
    std::unordered_map<std::string, std::string> peer_table;

public:
    void addPeer(const std::string &publicKey, const std::string &ip);
    std::string findPeer(const std::string &publicKey);
};

#endif