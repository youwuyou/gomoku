//
// Created by Manuel on 27.01.2021.
//
// Helper class to generate unique ids.
// from https://stackoverflow.com/questions/24365331/how-can-i-generate-uuid-in-c-without-using-boost-library

#ifndef UUID_GENERATOR_H
#define UUID_GENERATOR_H

#include <random>
#include <sstream>

class uuid_generator {

private:

public:
    static std::string generate_uuid_v4() {
        std::stringstream ss;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 15);
        std::uniform_int_distribution<> dis2(8, 11);

        int i;
        ss << std::hex;
        for (i = 0; i < 8; i++) {
            ss << dis(gen);
        }
        ss << "-";
        for (i = 0; i < 4; i++) {
            ss << dis(gen);
        }
        ss << "-4";
        for (i = 0; i < 3; i++) {
            ss << dis(gen);
        }
        ss << "-";
        ss << dis2(gen);
        for (i = 0; i < 3; i++) {
            ss << dis(gen);
        }
        ss << "-";
        for (i = 0; i < 12; i++) {
            ss << dis(gen);
        };
        return ss.str();
    }
};

#endif //UUID_GENERATOR_H
