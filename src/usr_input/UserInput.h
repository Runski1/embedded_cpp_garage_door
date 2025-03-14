#ifndef _USERINPUT_H
#define _USERINPUT_H
//
// Created By Matias Ruonala on 13.03.2025

#include "RemoteCtrl.h"
#include <iostream>
#include <sstream>

class UserInput {
public:
    static std::string read_input();
    static bool get_validated_input(char *output, size_t max_len);
};

#endif
