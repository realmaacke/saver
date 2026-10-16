#pragma once

#include <iostream>
#include <sstream>
#include <streambuf>
#include <string>

//NOLINTNEXTLINE(cppcoreguidelines-special-member-functions)
class StdoutCapture {
public:
    StdoutCapture() : old_buf_(std::cout.rdbuf(buffer_.rdbuf())) {}
    ~StdoutCapture() { std::cout.rdbuf(old_buf_); }
    StdoutCapture(const StdoutCapture&) = delete;
    StdoutCapture& operator = (const StdoutCapture&) = delete;

    std::string str() const { return buffer_.str(); }

private:
    std::ostringstream buffer_;
    std::streambuf* old_buf_;
};
