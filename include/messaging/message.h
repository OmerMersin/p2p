//
// Created by Omer Mersin on 11/16/24.
//

#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>

class Message {
private:
    std::string sender;
    std::string recipient;
    std::string content;

public:
    Message(const std::string &sender, const std::string &recipient, const std::string &content);
    void saveToDatabase();
};

#endif