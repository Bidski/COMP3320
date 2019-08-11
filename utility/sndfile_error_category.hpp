#ifndef UTILITY_SNDFILE_ERROR_CATEGORY_HPP
#define UTILITY_SNDFILE_ERROR_CATEGORY_HPP

#include <system_error>

#include "sndfile.h"

enum class sndfile_error_code {
    NO_ERROR              = SF_ERR_NO_ERROR,
    UNRECOGNISED_FORMAT   = SF_ERR_UNRECOGNISED_FORMAT,
    SYSTEM                = SF_ERR_SYSTEM,
    MALFORMED_FILE        = SF_ERR_MALFORMED_FILE,
    UNSUPPORTED_ENCONDING = SF_ERR_UNSUPPORTED_ENCODING,
    UNKNOWN
};

namespace std {
template <>
struct is_error_condition_enum<sndfile_error_code> : public true_type {};
}  // namespace std


class sndfile_error_category_t : public std::error_category {
public:
    virtual const char* name() const noexcept;

    virtual std::error_condition default_error_condition(int code) const noexcept;

    virtual bool equivalent(const std::error_code& code, int condition) const noexcept;

    virtual std::string message(int code) const noexcept;
};

const std::error_category& sndfile_error_category() {
    static sndfile_error_category_t instance;
    return instance;
}

std::error_condition make_error_condition(sndfile_error_code e) {
    return std::error_condition(static_cast<int>(e), sndfile_error_category());
}

const char* sndfile_error_category_t::name() const noexcept {
    return "sndfile_error_category";
}

std::error_condition sndfile_error_category_t::default_error_condition(int code) const noexcept {
    using sfe = sndfile_error_code;
    switch (code) {
        case SF_ERR_NO_ERROR: return std::error_condition(sfe::NO_ERROR);
        case SF_ERR_UNRECOGNISED_FORMAT: return std::error_condition(sfe::UNRECOGNISED_FORMAT);
        case SF_ERR_SYSTEM: return std::error_condition(sfe::SYSTEM);
        case SF_ERR_MALFORMED_FILE: return std::error_condition(sfe::MALFORMED_FILE);
        case SF_ERR_UNSUPPORTED_ENCODING: return std::error_condition(sfe::UNSUPPORTED_ENCONDING);
        default: return std::error_condition(sfe::UNKNOWN);
    }
}

bool sndfile_error_category_t::equivalent(const std::error_code& code, int condition) const noexcept {
    return *this == code.category() && static_cast<int>(default_error_condition(code.value()).value()) == condition;
}

std::string sndfile_error_category_t::message(int code) const noexcept {
    switch (code) {
        case SF_ERR_NO_ERROR: return "No error";
        case SF_ERR_UNRECOGNISED_FORMAT: return "Unrecognised format";
        case SF_ERR_SYSTEM: return "System error";
        case SF_ERR_MALFORMED_FILE: return "Malformed file";
        case SF_ERR_UNSUPPORTED_ENCODING: return "Unsupported encoding";
        default: return "Unknown error";
    }
}

#endif  // UTILITY_SNDFILE_ERROR_CATEGORY_HPP
