//
// Created by Manuel on 09.03.2021.
//

#ifndef GOMOKU_GOMOKU_EXCEPTION_H
#define GOMOKU_GOMOKU_EXCEPTION_H

#include <string>

class gomoku_exception : public std::exception {
private:
    std::string _msg;
public:
    explicit gomoku_exception(const std::string& message) : _msg(message) { };

    const char* what() const noexcept override {
        return _msg.c_str();
    }
};

#endif //GOMOKU_GOMOKU_EXCEPTION_H
