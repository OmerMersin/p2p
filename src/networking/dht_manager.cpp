//
// Created by Omer Mersin on 11/16/24.
//
#include "networking/dht_manager.h"
#include <libtorrent/session.hpp>
#include <libtorrent/alert.hpp>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/entry.hpp>
#include <libtorrent/sha1_hash.hpp>
#include <libtorrent/hasher.hpp>
#include <libtorrent/bencode.hpp>
#include <iostream>
#include <thread>

DHTManager::DHTManager() {
    libtorrent::settings_pack settings;
    settings.set_bool(libtorrent::settings_pack::enable_dht, true);
    settings.set_str(libtorrent::settings_pack::dht_bootstrap_nodes,
                     "dht.libtorrent.org:25401, router.bittorrent.com:6881, router.utorrent.com:6881");
    settings.set_int(libtorrent::settings_pack::alert_mask, libtorrent::alert::dht_notification);
    session.apply_settings(settings);
}

void DHTManager::startDHT(int listenPort) {
    libtorrent::settings_pack settings;
    settings.set_str(libtorrent::settings_pack::listen_interfaces, "0.0.0.0:" + std::to_string(listenPort));
    session.apply_settings(settings);

    std::cout << "DHT started on port " << listenPort << std::endl;
}

void DHTManager::announceUsername(const std::string& username, const std::string& publicIP, int publicPort) {
    std::string value = publicIP + ":" + std::to_string(publicPort);

    // Generate a 32-byte key for the username
    std::array<char, 32> key;
    auto hash = libtorrent::hasher(username).final();
    std::copy(hash.begin(), hash.end(), key.begin());

    // Announce value in the DHT
    session.dht_put_item(key, [value](libtorrent::entry& e, std::array<char, 64>& sig,
                                      std::int64_t& seq, const std::string& salt) {
        e = libtorrent::entry(value); // Set the value in the DHT item
        sig.fill(0);                  // No signature (unsigned item)
        seq = 0;                      // Sequence number for updates
    });

    std::cout << "Announced username: " << username << " with public IP: " << publicIP
              << " and port: " << publicPort << std::endl;
}

std::pair<std::string, int> DHTManager::findPeer(const std::string& username) {
    // Generate the key for the username
    std::array<char, 32> key;
    auto hash = libtorrent::hasher(username).final();
    std::copy(hash.begin(), hash.end(), key.begin());

    // Request the item from the DHT
    session.dht_get_item(key);

    // Poll alerts to retrieve the response
    std::string peerInfo;
    for (;;) {
        std::vector<libtorrent::alert*> alerts;
        session.pop_alerts(&alerts);

        for (auto alert : alerts) {
            // Use dht_immutable_item_alert for immutable DHT items
            if (auto dhtAlert = libtorrent::alert_cast<libtorrent::dht_immutable_item_alert>(alert)) {
                peerInfo = dhtAlert->item.string();
                break;
            }
        }

        if (!peerInfo.empty()) {
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Parse the IP and port from the retrieved value
    auto separator = peerInfo.find(':');
    if (separator != std::string::npos) {
        std::string ip = peerInfo.substr(0, separator);
        int port = std::stoi(peerInfo.substr(separator + 1));
        return {ip, port};
    }

    return {"", 0};
}