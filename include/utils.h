//
// Created by Omer Mersin on 11/17/24.
//

#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <sstream>

inline std::vector<std::string> split(const std::string &str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

inline bool startsWith(const std::string &str, const std::string &prefix) {
    return str.rfind(prefix, 0) == 0;
}

#endif // UTILS_H