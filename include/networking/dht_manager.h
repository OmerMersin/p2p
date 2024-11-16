//
// Created by Omer Mersin on 11/16/24.
//

#ifndef DHT_MANAGER_H
#define DHT_MANAGER_H

#include <libtorrent/session.hpp>
#include <string>
#include <utility> // for std::pair

class DHTManager {
private:
    libtorrent::session session;

public:
    DHTManager();

    // Start the DHT
    void startDHT(int listenPort);

    // Announce username with public IP and port
    void announceUsername(const std::string& username, const std::string& publicIP, int publicPort);

    // Find peer by username
    std::pair<std::string, int> findPeer(const std::string& username);
};

#endif