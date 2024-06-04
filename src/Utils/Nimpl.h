#pragma once

void crash(const char *func, const char *msg);

#define NOT_IMPLEMENTED crash(__PRETTY_FUNCTION__, " not implemented yet")
