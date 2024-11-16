//
// Created by Omer Mersin on 11/16/24.
//

#ifndef PEER_H
#define PEER_H

#include <boost/asio.hpp>
#include <string>
#include <thread>

class Peer {
public:
    Peer();
    ~Peer();

    void bind(int localPort);
    void sendMessage(const std::string &message, const std::string &ip, int port);
    void startListening();
    void stopListening();
    std::pair<std::string, int> getPublicAddress(const std::string &stunServer, int port);

    // Encryption methods
    void setSharedKey(const std::string &key);
    std::string encryptMessage(const std::string &plaintext);
    std::string decryptMessage(const std::string &ciphertext);

    // Callback for received messages
    void setMessageCallback(std::function<void(const std::string&, const std::string&, int)> callback);

private:
    boost::asio::io_context io_context;
    boost::asio::ip::udp::socket socket;
    std::thread listenerThread;
    bool running;
    std::string sharedKey; // Shared encryption key
    // Callback function for message notifications
    std::function<void(const std::string&, const std::string&, int)> messageCallback;
};

#endif // PEER_H