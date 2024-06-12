#include "Nimpl.h"

#include <iostream>

void crash(const char *func, const char *msg) {
    std::cerr << func << msg << std::endl << std::flush;
    std::abort();
}