//
// Created by Omer Mersin on 11/16/24.
//
#include "networking/stun.h"
#include <iostream>
#include <boost/asio.hpp>
#include <cstring>

std::pair<std::string, int> STUN::getPublicAddress(const std::string &stunServer, int port) {
    try {
        boost::asio::io_context io_context;

        // Create a UDP socket
        boost::asio::ip::udp::socket socket(io_context);

        // Resolve the STUN server address
        boost::asio::ip::udp::resolver resolver(io_context);
        auto endpoints = resolver.resolve(stunServer, std::to_string(port));
        boost::asio::ip::udp::endpoint stunEndpoint = *endpoints.begin();

        // Determine the address type and open the socket accordingly
        if (stunEndpoint.address().is_v4()) {
            socket.open(boost::asio::ip::udp::v4());
        } else if (stunEndpoint.address().is_v6()) {
            socket.open(boost::asio::ip::udp::v6());
        } else {
            throw std::runtime_error("Unknown address type for STUN server");
        }

        std::cout << "Resolved STUN server: " << stunEndpoint.address().to_string()
                  << ":" << stunEndpoint.port() << std::endl;

        // Create a STUN binding request
        unsigned char request[20] = {0};
        request[0] = 0x00; // Type: Binding Request
        request[1] = 0x01;
        request[2] = 0x00; // Length (16 bytes)
        request[3] = 0x00;

        // Generate a random Transaction ID (12 bytes)
        for (int i = 4; i < 20; ++i) {
            request[i] = static_cast<unsigned char>(rand() % 256);
        }

        // Send the request to the STUN server
        socket.send_to(boost::asio::buffer(request, 20), stunEndpoint);

        // Receive the response
        unsigned char response[1024];
        boost::asio::ip::udp::endpoint senderEndpoint;
        size_t length = socket.receive_from(boost::asio::buffer(response, 1024), senderEndpoint);

        // Parse the response (Basic Parsing)
        if (length > 0 && response[0] == 0x01 && response[1] == 0x01) { // Binding Success Response
            unsigned short port = (response[26] << 8) | response[27];
            port ^= 0x2112; // XOR port with magic cookie

            char ip[16];
            snprintf(ip, sizeof(ip), "%d.%d.%d.%d",
                     response[28] ^ 0x21, response[29] ^ 0x12,
                     response[30] ^ 0xA4, response[31] ^ 0x42);

            return {std::string(ip), port};
        } else {
            throw std::runtime_error("Invalid STUN response");
        }
    } catch (std::exception &e) {
        std::cerr << "STUN Error: " << e.what() << std::endl;
        return {"", 0};
    }
}
