//
// Created by Omer Mersin on 11/16/24.
//
#include "networking/dht.h"
#include <unordered_map>

// Add a peer to the DHT
void DHT::addPeer(const std::string &username, const std::string &ipPort) {
    peer_table[username] = ipPort;
}

// Find a peer in the DHT
std::string DHT::findPeer(const std::string &username) {
    if (peer_table.find(username) != peer_table.end()) {
        return peer_table[username];
    }
    return "";
}