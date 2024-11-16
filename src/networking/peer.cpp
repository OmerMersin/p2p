//
// Created by Omer Mersin on 11/16/24.
//

#include "networking/peer.h"
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

Peer::Peer() : socket(io_context), running(false) {}

Peer::~Peer() {
    stopListening();
}

void Peer::bind(int localPort) {
    socket.open(udp::v4());
    socket.bind(udp::endpoint(udp::v4(), localPort));
    std::cout << "Bound to local port: " << localPort << std::endl;
}

void Peer::sendMessage(const std::string &message, const std::string &ip, int port) {
    try {
        udp::endpoint remoteEndpoint(boost::asio::ip::make_address(ip), port);
        socket.send_to(boost::asio::buffer(message), remoteEndpoint);
        std::cout << "Sent message: " << message << " to " << ip << ":" << port << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Error sending message: " << e.what() << std::endl;
    }
}

void Peer::startListening() {
    running = true;
    std::cout << "[DEBUG] Starting to listen on: " << socket.local_endpoint().port() << std::endl;
    listenerThread = std::thread([this]() {
        try {
            char buffer[1024];
            udp::endpoint senderEndpoint;
            while (running) {
                size_t len = socket.receive_from(boost::asio::buffer(buffer), senderEndpoint);
                std::string message(buffer, len);

                std::cout << "[DEBUG] Received from " << senderEndpoint.address().to_string()
                          << ":" << senderEndpoint.port() << " - " << message << std::endl;

                // Notify via callback
                if (messageCallback) {
                    messageCallback(message, senderEndpoint.address().to_string(), senderEndpoint.port());
                }
            }
        } catch (const std::exception &e) {
            if (running) {
                std::cerr << "[ERROR] Error receiving message: " << e.what() << std::endl;
            }
        }
    });
}

void Peer::stopListening() {
    running = false;
    if (listenerThread.joinable()) {
        listenerThread.join();
    }
    socket.close();
}

std::pair<std::string, int> Peer::getPublicAddress(const std::string &stunServer, int port) {
    try {
        boost::asio::ip::udp::resolver resolver(io_context);
        auto endpoints = resolver.resolve(stunServer, std::to_string(port));

        if (endpoints.empty()) {
            throw std::runtime_error("Failed to resolve STUN server address.");
        }

        udp::endpoint stunEndpoint = *endpoints.begin();
        udp::socket stunSocket(io_context, udp::v4());

        stunSocket.bind(udp::endpoint(udp::v4(), 0));

        unsigned char request[20] = {0x00, 0x01, 0x00, 0x00}; // STUN binding request header
        for (int i = 4; i < 20; ++i) {
            request[i] = static_cast<unsigned char>(std::rand() % 256); // Random transaction ID
        }

        stunSocket.send_to(boost::asio::buffer(request, 20), stunEndpoint);

        unsigned char response[1024];
        udp::endpoint senderEndpoint;
        size_t len = stunSocket.receive_from(boost::asio::buffer(response, 1024), senderEndpoint);

        if (len < 20 || response[0] != 0x01 || response[1] != 0x01) {
            throw std::runtime_error("Invalid STUN response.");
        }

        uint16_t mappedPort = (response[26] << 8 | response[27]) ^ 0x2112;
        std::string mappedIP = std::to_string(response[28] ^ 0x21) + "." +
                               std::to_string(response[29] ^ 0x12) + "." +
                               std::to_string(response[30] ^ 0xA4) + "." +
                               std::to_string(response[31] ^ 0x42);

        return {mappedIP, mappedPort};
    } catch (const std::exception &e) {
        std::cerr << "Error during STUN resolution: " << e.what() << std::endl;
        return {"", 0};
    }
}

void Peer::setSharedKey(const std::string &key) {
    sharedKey = key;
}

std::string Peer::encryptMessage(const std::string &plaintext) {
    unsigned char iv[EVP_MAX_IV_LENGTH];
    unsigned char ciphertext[1024];
    int len = 0, ciphertextLen = 0;

    if (RAND_bytes(iv, sizeof(iv)) != 1) {
        throw std::runtime_error("Failed to generate IV.");
    }

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        throw std::runtime_error("Failed to create EVP_CIPHER_CTX.");
    }

    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL,
                           reinterpret_cast<const unsigned char *>(sharedKey.c_str()), iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to initialize encryption.");
    }

    if (EVP_EncryptUpdate(ctx, ciphertext, &len,
                          reinterpret_cast<const unsigned char *>(plaintext.c_str()), plaintext.size()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Encryption failed.");
    }
    ciphertextLen = len;

    if (EVP_EncryptFinal_ex(ctx, ciphertext + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Final encryption step failed.");
    }
    ciphertextLen += len;

    EVP_CIPHER_CTX_free(ctx);

    return std::string(reinterpret_cast<char *>(ciphertext), ciphertextLen);
}

std::string Peer::decryptMessage(const std::string &ciphertext) {
    unsigned char plaintext[1024];
    unsigned char iv[EVP_MAX_IV_LENGTH];
    int len = 0, plaintextLen = 0;

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        throw std::runtime_error("Failed to create EVP_CIPHER_CTX.");
    }

    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL,
                           reinterpret_cast<const unsigned char *>(sharedKey.c_str()), iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to initialize decryption.");
    }

    if (EVP_DecryptUpdate(ctx, plaintext, &len,
                          reinterpret_cast<const unsigned char *>(ciphertext.c_str()), ciphertext.size()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Decryption failed.");
    }
    plaintextLen = len;

    if (EVP_DecryptFinal_ex(ctx, plaintext + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Final decryption step failed.");
    }
    plaintextLen += len;

    EVP_CIPHER_CTX_free(ctx);

    return std::string(reinterpret_cast<char *>(plaintext), plaintextLen);
}

void Peer::setMessageCallback(std::function<void(const std::string&, const std::string&, int)> callback) {
    messageCallback = std::move(callback);
}