//
// Created by Manuel on 10.02.2021.
//
// Helper functions to serialize vectors containing objects that implement the 'serializable' class.

#ifndef GOMOKU_VECTOR_UTILS_H
#define GOMOKU_VECTOR_UTILS_H

#include <vector>
#include "serializable.h"
#include "unique_serializable.h"
#include "../../rapidjson/include/rapidjson/document.h"


namespace vector_utils {

    template<class T, class B> struct derived_from {
        static void constraints(T* p) { B* pb = p; }
        derived_from() { void(*p)(T*) = constraints; }
    };

    template<class T>
    /* WARNING: can only be called with vectors containing elements of type T that derives from "serializable" */
    static rapidjson::Value serialize_vector(const std::vector<T*>& serializables, rapidjson::Document::AllocatorType& allocator) {
        derived_from<T,serializable>(); // ensure T derives from serializable
        rapidjson::Value arr_val(rapidjson::kArrayType);
        for (int i = 0; i < serializables.size(); i++) {
            rapidjson::Value elem(rapidjson::kObjectType);
            serializables[i]->write_into_json(elem, allocator);
            arr_val.PushBack(elem, allocator);
        }
        return arr_val;
    }

    template<class T>
    static rapidjson::Value serialize_vector(const std::vector<T>& serializables, rapidjson::Document::AllocatorType& allocator) {
        derived_from<T,serializable>(); // ensure T derives from serializable
        rapidjson::Value arr_val(rapidjson::kArrayType);
        for (int i = 0; i < serializables.size(); i++) {
            rapidjson::Value elem(rapidjson::kObjectType);
            T current_value = serializables[i];
            current_value.write_into_json(elem, allocator);
            arr_val.PushBack(elem, allocator);
        }
        return arr_val;
    }
}

#endif //GOMOKU_VECTOR_UTILS_H
