//
// Created by Omer Mersin on 11/16/24.
//

#ifndef DHT_H
#define DHT_H

#include <string>
#include <map>
#include <vector>
#include <mutex>

struct DHTNode {
    std::string id;       // Unique node ID
    std::string ip;       // Node IP address
    int port;             // Node port
};

class DHT {
public:
    DHT(const std::string &selfID, const std::string &selfIP, int selfPort);

    void addNode(const DHTNode &node);
    void removeNode(const std::string &id);
    DHTNode findNode(const std::string &id);
    void publish(const std::string &key, const std::string &value);
    std::string lookup(const std::string &key);
    void announceSelf();
    void handleIncomingMessage(const std::string &message, const std::string &ip, int port);

    // Get all nodes in the routing table
    std::vector<DHTNode> getRoutingTable() const;
    void sendMessage(const std::string &message, const std::string &ip, int port);
    void discoverNodes(const std::string &bootstrapIP, int bootstrapPort);
private:
    std::string selfID;
    std::string selfIP;
    int selfPort;

    std::map<std::string, DHTNode> routingTable; // Maps node ID to node details
    std::map<std::string, std::string> keyValueStore; // Stores key-value pairs
    mutable std::mutex dhtMutex;

};

#endif // DHT_H