#pragma once
#include <string_view>
#include <variant>
#include "Codes/ErrorCodes.hpp"
#include "Codes/InformationCodes.hpp"
#include "Codes/SuccessCodes.hpp"

using CodeVariant = std::variant<ErrorCode, SuccessCode, InfoCode>;

constexpr std::string_view code_to_message(CodeVariant type) {
    return std::visit([](auto code) {return to_message(code); }, type);
}