#include <SGC/error.hpp>

SGCError::SGCError(SGCErrorType errorCode, std::string&& msg)
: errorCode(static_cast<int>(errorCode)), msg(std::move(msg)) {}

const char* SGCError::what() const noexcept {
    return msg.c_str();
}