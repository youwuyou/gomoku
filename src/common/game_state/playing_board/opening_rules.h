//
// Created by stephenlincon on 11.11.23.
//

#ifndef GOMOKU_OPENING_RULES_H
#define GOMOKU_OPENING_RULES_H

#include <string>
#include <unordered_map>
#include "../../serialization/uuid_generator.h"
#include "../../../../rapidjson/include/rapidjson/document.h"
#include "../../serialization/unique_serializable.h"
#include "../../serialization/serializable_value.h"

enum ruleset_type {
    freestyle,
    swap2,
    swap_after_first_move,
    uninitialized
};

class opening_rules : public unique_serializable {
private:

    // serializable_value<std::string>* _ruleset;

    ruleset_type _ruleset;
    serializable_value<std::string>* _description;

#ifdef GOMOKU_SERVER
    std::string _game_id;
#endif

    // Deserialization constructor
    opening_rules(std::string id,
                  ruleset_type ruleset,
                  serializable_value<std::string>* description);

    // Checks if a string pertains to a valid ruleset, and generate description
    static std::string generate_description_from_ruleset(ruleset_type ruleset);


public:
    // constructors
    explicit opening_rules(ruleset_type ruleset);   // for client
    ~opening_rules();

#ifdef GOMOKU_SERVER
    opening_rules(std::string id, ruleset_type ruleset);  // for server
    std::string get_game_id();
    void set_game_id(std::string game_id);
#endif

    // accessors
    ruleset_type get_ruleset() const noexcept;
    std::string get_description() const noexcept;

    // for deserialization of ruleset
    static const std::unordered_map<std::string, ruleset_type> _string_to_ruleset_type;
    // for serialization of ruleset
    static const std::unordered_map<ruleset_type, std::string> _ruleset_type_to_string;

#ifdef GOMOKU_SERVER
    // state update functions
    void set_opening_rule(std::string ruleset_string);
#endif

    // serialization
    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;
    static opening_rules* from_json(const rapidjson::Value& json);
};

#endif //GOMOKU_OPENING_RULES_H
