#pragma once

#include <iostream>

void crash(const char *func, const char *msg) {
    std::cout << func << msg << std::endl << std::flush;
    std::abort();
}

#define NOT_IMPLEMENTED crash(__PRETTY_FUNCTION__, " not implemented yet")
