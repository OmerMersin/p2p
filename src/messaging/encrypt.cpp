//
// Created by Omer Mersin on 11/16/24.
//
#include "networking/peer.h"
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <iostream>
#include <cstring>

std::string Peer::encryptMessage(const std::string &plaintext) {
    unsigned char key[32]; // 256-bit key
    std::memset(key, 0, sizeof(key));
    std::memcpy(key, sharedKey.c_str(), std::min(sharedKey.size(), sizeof(key)));

    unsigned char iv[AES_BLOCK_SIZE];
    if (!RAND_bytes(iv, AES_BLOCK_SIZE)) {
        throw std::runtime_error("Failed to generate IV");
    }

    AES_KEY aesKey;
    if (AES_set_encrypt_key(key, 256, &aesKey) < 0) {
        throw std::runtime_error("Failed to set AES encryption key");
    }

    std::vector<unsigned char> ciphertext(plaintext.size() + AES_BLOCK_SIZE);
    int len = 0;

    AES_cfb128_encrypt(reinterpret_cast<const unsigned char *>(plaintext.data()),
                       ciphertext.data(), plaintext.size(),
                       &aesKey, iv, &len, AES_ENCRYPT);

    std::string result(reinterpret_cast<char *>(ciphertext.data()), len);
    return std::string(reinterpret_cast<char *>(iv), AES_BLOCK_SIZE) + result;
}

std::string Peer::decryptMessage(const std::string &ciphertext) {
    if (ciphertext.size() < AES_BLOCK_SIZE) {
        throw std::runtime_error("Invalid ciphertext");
    }

    unsigned char key[32]; // 256-bit key
    std::memset(key, 0, sizeof(key));
    std::memcpy(key, sharedKey.c_str(), std::min(sharedKey.size(), sizeof(key)));

    const unsigned char *iv = reinterpret_cast<const unsigned char *>(ciphertext.data());
    const unsigned char *encryptedData = reinterpret_cast<const unsigned char *>(ciphertext.data() + AES_BLOCK_SIZE);

    AES_KEY aesKey;
    if (AES_set_decrypt_key(key, 256, &aesKey) < 0) {
        throw std::runtime_error("Failed to set AES decryption key");
    }

    std::vector<unsigned char> plaintext(ciphertext.size() - AES_BLOCK_SIZE);
    int len = 0;

    AES_cfb128_encrypt(encryptedData, plaintext.data(), ciphertext.size() - AES_BLOCK_SIZE,
                       &aesKey, const_cast<unsigned char *>(iv), &len, AES_DECRYPT);

    return std::string(reinterpret_cast<char *>(plaintext.data()), len);
}