//
// Created by Omer Mersin on 11/16/24.
//

#ifndef ENCRYPT_H
#define ENCRYPT_H

#include <string>

class Encryption {
public:
    static std::string encrypt(const std::string &plaintext, const std::string &key);
    static std::string decrypt(const std::string &ciphertext, const std::string &key);
};

#endif