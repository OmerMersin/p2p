//
// Created by Omer Mersin on 11/16/24.
//
#include "networking/dht.h"
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <vector>
#include "utils.h"
#include <QHash>
#include <QString>

// Discover nodes by sending a DISCOVER message to a bootstrap node
void DHT::discoverNodes(const std::string &bootstrapIP, int bootstrapPort) {
    sendMessage("DISCOVER", bootstrapIP, bootstrapPort);
}

void DHT::handleIncomingMessage(const std::string &message, const std::string &ip, int port) {
    if (message == "DISCOVER") {
        std::cout << "[DEBUG] Received DISCOVER from " << ip << ":" << port << std::endl;

        // Add the sender to the routing table
        DHTNode newNode{std::to_string(qHash(QString::fromStdString(ip + ":" + std::to_string(port)))), ip, port};
        addNode(newNode);

        // Log routing table after adding the node
        std::cout << "[DEBUG] Routing Table after DISCOVER:" << std::endl;
        for (const auto &[id, node] : routingTable) {
            std::cout << "Node ID: " << id << ", IP: " << node.ip << ", Port: " << node.port << std::endl;
        }

        // Prepare the routing table response
        std::string response = "ROUTING_TABLE ";
        {
            std::lock_guard<std::mutex> lock(dhtMutex);
            for (const auto &[id, node] : routingTable) {
                response += id + "," + node.ip + "," + std::to_string(node.port) + ";";
            }
        }

        // Send routing table back to the sender
        sendMessage(response, ip, port);
        std::cout << "[DEBUG] Sent routing table to " << ip << ":" << port << std::endl;
    } else if (startsWith(message, "ROUTING_TABLE")) {
        std::cout << "[DEBUG] Received ROUTING_TABLE from " << ip << ":" << port << std::endl;
        std::string nodes = message.substr(14);
        std::istringstream iss(nodes);
        std::string nodeInfo;

        while (std::getline(iss, nodeInfo, ';')) {
            if (nodeInfo.empty()) continue;
            auto parts = split(nodeInfo, ',');
            if (parts.size() == 3) {
                addNode({parts[0], parts[1], std::stoi(parts[2])});
            }
        }

        // Log routing table after update
        std::cout << "[DEBUG] Routing Table after receiving ROUTING_TABLE:" << std::endl;
        for (const auto &[id, node] : routingTable) {
            std::cout << "Node ID: " << id << ", IP: " << node.ip << ", Port: " << node.port << std::endl;
        }
    } else if (startsWith(message, "ANNOUNCE")) {
        std::string nodeID = message.substr(9); // Extract the node ID
        std::cout << "[DEBUG] Received ANNOUNCE from " << ip << ":" << port << " - Node ID: " << nodeID << std::endl;
        addNode({nodeID, ip, port});

        // Propagate ANNOUNCE to other nodes
        std::lock_guard<std::mutex> lock(dhtMutex);
        for (const auto &[id, node] : routingTable) {
            if (node.ip != ip || node.port != port) {
                sendMessage("ANNOUNCE " + nodeID, node.ip, node.port);
            }
        }
    } else {
        std::cout << "[DEBUG] Unknown message type received: " << message << std::endl;
    }
}

// Constructor: Initialize the DHT with self-node information
DHT::DHT(const std::string &selfID, const std::string &selfIP, int selfPort)
        : selfID(selfID), selfIP(selfIP), selfPort(selfPort) {
    if (!selfIP.empty() && selfPort > 0) {
        addNode({selfID, selfIP, selfPort});
    } else {
        std::cerr << "Error: Bootstrap node must have a valid IP and Port." << std::endl;
    }
}

// Add a node to the routing table
void DHT::addNode(const DHTNode &node) {
    std::lock_guard<std::mutex> lock(dhtMutex);

    // Avoid duplicate or invalid entries
    if (routingTable.find(node.id) != routingTable.end()) {
        std::cout << "[DEBUG] Node already exists in the routing table: " << node.id << std::endl;
        return;
    }

    std::cout << "[DEBUG] Adding node to routing table: ID=" << node.id
              << ", IP=" << node.ip << ", Port=" << node.port << std::endl;
    routingTable[node.id] = node;
}

// Remove a node from the routing table
void DHT::removeNode(const std::string &id) {
    std::lock_guard<std::mutex> lock(dhtMutex);
    routingTable.erase(id);
    std::cout << "[DEBUG] Removed node from routing table: ID=" << id << std::endl;
}

// Find a node in the routing table by its ID
DHTNode DHT::findNode(const std::string &id) {
    std::lock_guard<std::mutex> lock(dhtMutex);
    auto it = routingTable.find(id);
    if (it == routingTable.end()) {
        throw std::runtime_error("Node not found in the routing table.");
    }
    return it->second;
}

// Publish a key-value pair to the DHT
void DHT::publish(const std::string &key, const std::string &value) {
    std::lock_guard<std::mutex> lock(dhtMutex);
    keyValueStore[key] = value;
    std::cout << "[DEBUG] Published key-value pair: " << key << " -> " << value << std::endl;
}

// Lookup a key in the DHT
std::string DHT::lookup(const std::string &key) {
    std::lock_guard<std::mutex> lock(dhtMutex);
    auto it = keyValueStore.find(key);
    if (it == keyValueStore.end()) {
        throw std::runtime_error("Key not found in the DHT.");
    }
    return it->second;
}

// Announce self to all nodes in the routing table
void DHT::announceSelf() {
    std::lock_guard<std::mutex> lock(dhtMutex);
    for (const auto &[id, node] : routingTable) {
        if (id != selfID) {
            sendMessage("ANNOUNCE " + selfID, node.ip, node.port);
        }
    }
}

// Retrieve the current routing table as a vector of nodes
std::vector<DHTNode> DHT::getRoutingTable() const {
    std::lock_guard<std::mutex> lock(dhtMutex);
    std::vector<DHTNode> nodes;
    for (const auto &[id, node] : routingTable) {
        nodes.push_back(node);
    }
    return nodes;
}

// Simulate sending a message to a node
void DHT::sendMessage(const std::string &message, const std::string &ip, int port) {
    // Simulated sending of a message
    std::cout << "[DEBUG] Sending message to " << ip << ":" << port << " - " << message << std::endl;
}