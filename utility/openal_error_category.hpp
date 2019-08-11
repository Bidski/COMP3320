#ifndef UTILITY_OPENAL_ERROR_CATEGORY_HPP
#define UTILITY_OPENAL_ERROR_CATEGORY_HPP

#include <system_error>

#include "AL/al.h"
#include "AL/alc.h"

enum class openal_error_code {
    NO_ERROR          = AL_NO_ERROR,
    INVALID_NAME      = AL_INVALID_NAME,
    INVALID_ENUM      = AL_INVALID_ENUM,
    INVALID_VALUE     = AL_INVALID_VALUE,
    INVALID_OPERATION = AL_INVALID_OPERATION,
    OUT_OF_MEMORY     = AL_OUT_OF_MEMORY,
    UNKNOWN
};

namespace std {
template <>
struct is_error_condition_enum<openal_error_code> : public true_type {};
}  // namespace std


class openal_error_category_t : public std::error_category {
public:
    virtual const char* name() const noexcept;

    virtual std::error_condition default_error_condition(int code) const noexcept;

    virtual bool equivalent(const std::error_code& code, int condition) const noexcept;

    virtual std::string message(int code) const noexcept;
};

const std::error_category& openal_error_category() {
    static openal_error_category_t instance;
    return instance;
}

std::error_condition make_error_condition(openal_error_code e) {
    return std::error_condition(static_cast<int>(e), openal_error_category());
}

const char* openal_error_category_t::name() const noexcept {
    return "openal_error_category";
}

std::error_condition openal_error_category_t::default_error_condition(int code) const noexcept {
    using ale = openal_error_code;
    switch (code) {
        case AL_NO_ERROR: return std::error_condition(ale::NO_ERROR);
        case AL_INVALID_NAME: return std::error_condition(ale::INVALID_NAME);
        case AL_INVALID_ENUM: return std::error_condition(ale::INVALID_ENUM);
        case AL_INVALID_VALUE: return std::error_condition(ale::INVALID_VALUE);
        case AL_INVALID_OPERATION: return std::error_condition(ale::INVALID_OPERATION);
        case AL_OUT_OF_MEMORY: return std::error_condition(ale::OUT_OF_MEMORY);
        default: return std::error_condition(ale::UNKNOWN);
    }
}

bool openal_error_category_t::equivalent(const std::error_code& code, int condition) const noexcept {
    return *this == code.category() && static_cast<int>(default_error_condition(code.value()).value()) == condition;
}

std::string openal_error_category_t::message(int code) const noexcept {
    switch (code) {
        case AL_NO_ERROR: return "No error";
        case AL_INVALID_NAME: return "Invalid name";
        case AL_INVALID_ENUM: return "Invalid enum";
        case AL_INVALID_VALUE: return "Invalid value";
        case AL_INVALID_OPERATION: return "Invalid operation";
        case AL_OUT_OF_MEMORY: return "Out of memory";
        default: return "Unknown error";
    }
}

#endif  // UTILITY_OPENAL_ERROR_CATEGORY_HPP
