//
// Created by Manuel on 09.03.2021.
//

#ifndef GOMOKU_GOMOKUEXCEPTION_H
#define GOMOKU_GOMOKUEXCEPTION_H

#include <string>

class GomokuException : public std::exception {
private:
    std::string _msg;
public:
    explicit GomokuException(const std::string& message) : _msg(message) { };

    const char* what() const noexcept override {
        return _msg.c_str();
    }
};

#endif //GOMOKU_GOMOKUEXCEPTION_H
