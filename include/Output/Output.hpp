#pragma once
#include <string>

#include "Output/Codes/ErrorCodes.hpp"
#include "Output/Codes/InformationCodes.hpp"
#include "Output/Codes/SuccessCodes.hpp"
class Output{
public:
    static void error(const ErrorCode& type, const std::string& function=__FUNCTION__);
    static void info(const InfoCode& type, const std::string& extra="");
    static void success(const SuccessCode& type, const std::string& extra="");
    static void print(const std::string& message);
};