//
// Created by Manuel on 09.03.2021.
//

#ifndef LAMA_LAMAEXCEPTION_H
#define LAMA_LAMAEXCEPTION_H

#include <string>

class LamaException : public std::exception {
private:
    std::string _msg;
public:
    explicit LamaException(const std::string& message) : _msg(message) { };

    const char* what() const noexcept override {
        return _msg.c_str();
    }
};

#endif //LAMA_LAMAEXCEPTION_H
