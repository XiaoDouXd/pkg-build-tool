#pragma once

#include <exception>
#include <string>


class exce : public std::exception {
public:
    explicit exce(const std::string &str) : _what(str.c_str()) {}
    explicit exce(const char *str) : _what(str) {}
    [[nodiscard]] const char *what() const noexcept override { return _what; }

private:
    const char *_what;
};
