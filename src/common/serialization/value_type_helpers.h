//
// Created by Manuel on 09.02.2021.
//
// Helper functions for serializing and deserializing instances of serializable_value.

#ifndef LAMA_VALUE_TYPE_HELPERS_H
#define LAMA_VALUE_TYPE_HELPERS_H

#include <unordered_map>


namespace value_type_helpers {

    enum ValueType {
        intType,
        uintType,
        boolType,
        floatType,
        doubleType,
        stringType,
        int64tType,
        uint64tType
    };


// for deserialization
    const std::unordered_map<std::string, ValueType> _string_to_value_type = {
            {"int", ValueType::intType },
            {"uint", ValueType::uintType},
            {"bool", ValueType::boolType},
            {"float", ValueType::floatType},
            {"double", ValueType::doubleType},
            {"int64t", ValueType::int64tType},
            {"uint64t", ValueType::uint64tType},
            {"string", ValueType::stringType}
    };

// for serialization
    const std::unordered_map<ValueType, std::string> _value_type_to_string = {
            { ValueType::intType,     "int" },
            { ValueType::uintType,    "uint"},
            { ValueType::boolType,    "bool"},
            { ValueType::floatType,   "float"},
            { ValueType::doubleType,  "double"},
            { ValueType::int64tType,  "int64t"},
            { ValueType::uint64tType, "uint64t"},
            { ValueType::stringType,  "string"},
    };

    template<typename T>
    static rapidjson::Value get_json_value(T val, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator);

    template<typename T>
    static rapidjson::Value get_json_value_type(T val, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator);

    template<>
    rapidjson::Value
    get_json_value<bool>(bool val, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) {
        return rapidjson::Value(val);
    }

    template<>
    rapidjson::Value
    get_json_value<int>(int val, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) {
        return rapidjson::Value(val);
    }

    template<>
    rapidjson::Value
    get_json_value<float>(float val, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) {
        return rapidjson::Value(val);
    }

    template<>
    rapidjson::Value
    get_json_value<double>(double val, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) {
        return rapidjson::Value(val);
    }

    template<>
    rapidjson::Value
    get_json_value<unsigned int>(unsigned int val, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) {
        return rapidjson::Value(val);
    }

    template<>
    rapidjson::Value
    get_json_value<int64_t>(int64_t val, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) {
        return rapidjson::Value(val);
    }

    template<>
    rapidjson::Value
    get_json_value<uint64_t>(uint64_t val, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) {
        return rapidjson::Value(val);
    }

    template<>
    rapidjson::Value
    get_json_value<std::string>(std::string val, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) {
        rapidjson::Value json_val;
        json_val.SetString(val.c_str(), allocator);
        return json_val;
    }


    template<>
    rapidjson::Value
    get_json_value_type<bool>(bool val, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) {
        return rapidjson::Value(std::string("bool").c_str(), allocator);
    }

    template<>
    rapidjson::Value
    get_json_value_type<int>(int val, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) {
        return rapidjson::Value(std::string("int").c_str(), allocator);
    }

    template<>
    rapidjson::Value
    get_json_value_type<float>(float val, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) {
        return rapidjson::Value(std::string("float").c_str(), allocator);
    }

    template<>
    rapidjson::Value
    get_json_value_type<double>(double val, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) {
        return rapidjson::Value(std::string("double").c_str(), allocator);
    }

    template<>
    rapidjson::Value
    get_json_value_type<unsigned int>(unsigned int val, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) {
        return rapidjson::Value(std::string("unsigned int").c_str(), allocator);
    }

    template<>
    rapidjson::Value
    get_json_value_type<int64_t>(int64_t val, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) {
        return rapidjson::Value(std::string("int64_t").c_str(), allocator);
    }

    template<>
    rapidjson::Value
    get_json_value_type<uint64_t>(uint64_t val, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) {
        return rapidjson::Value(std::string("int64_t").c_str(), allocator);
    }

    template<>
    rapidjson::Value
    get_json_value_type<std::string>(std::string val, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) {
        return rapidjson::Value(std::string("string").c_str(), allocator);
    }
}



#endif //LAMA_VALUE_TYPE_HELPERS_H
