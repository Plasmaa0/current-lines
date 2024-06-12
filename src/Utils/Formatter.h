#pragma once
#include <sstream>
#define DEFINE_FORMATTER(CLASS) \
template <>\
struct std::formatter<CLASS> : std::formatter<string_view> {\
    auto format(const CLASS &obj, std::format_context &ctx) const {\
        std::stringstream temp;\
        temp << obj;\
        return std::formatter<string_view>::format(temp.str(), ctx);\
    }\
};
