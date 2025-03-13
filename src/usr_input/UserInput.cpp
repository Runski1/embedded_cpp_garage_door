//
// Created By Matias Ruonala on 13.03.2025

#include "UserInput.h"
#include "RemoteCtrl.h"
#include <cstdio>
#include <iostream>
#include <sstream>

std::string UserInput::read_input() {
    std::string input;
    char ch = '\0';
    while (true) {
        ch = std::cin.get();

        if (ch == '\r' || ch == '\n') {
            std::cout << ch;
            break;
        }

        if (ch == '\b' || ch == 127) {
            if (!input.empty()) {
                input.pop_back();
                std::cout << "\b \b";
            }
        } else {
            std::cout << ch;
            input.push_back(ch);
        }
    }
    return input;
}

bool UserInput::get_validated_input(char *output, size_t max_len) {
    std::string input = read_input();
    if (input.length() > max_len) {
        std::cout << "Input is too long, please try again." << std::endl;
        return false;
    } else if (input.length() <= max_len && input.length() != 0) {
        std::cout << "User entered: [" << input
                  << "] (length: " << input.length() << ")\n";

        strncpy(output, input.c_str(), max_len - 1);
        output[max_len - 1] = '\0';
    }
    return true;
}
