//
// Created by Omer Mersin on 11/16/24.
//

#ifndef UI_H
#define UI_H

#include <string>

class UI {
public:
    static void printWelcomeMessage();
    static std::string getInput(const std::string &prompt);
    static void displayMessage(const std::string &message);
};

#endif