//
// Created by Omer Mersin on 11/16/24.
//
#include "ui/ui.h"
#include <iostream>

void UI::printWelcomeMessage() {
    std::cout << "=== Welcome to the P2P Messaging App ===" << std::endl;
}

std::string UI::getInput(const std::string &prompt) {
    std::cout << prompt;
    std::string input;
    std::getline(std::cin, input);
    return input;
}

void UI::displayMessage(const std::string &message) {
    std::cout << ">>> " << message << std::endl;
}