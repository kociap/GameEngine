#include "debug_macros.hpp"

#include <iostream>
#include <string>

void _GE_log(std::string str) {
    std::cout << str << "\n";
}

void _GE_conditional_log(bool cond, std::string msg) {
    if (!cond) {
		std::cout << msg << "\n";
    }
}