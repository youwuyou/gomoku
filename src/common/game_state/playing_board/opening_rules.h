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

enum RulesetType {
    freestyle,
    swap2,
    swap_after_first_move,
    uninitialized
};

class opening_rules : public unique_serializable {
private:

    // for deserialization of ruleset
    static const std::unordered_map<std::string, RulesetType> _string_to_ruleset_type;
    // for serialization of ruleset
    static const std::unordered_map<RulesetType, std::string> _ruleset_type_to_string;

    // serializable_value<std::string>* _ruleset;

    RulesetType _ruleset;
    serializable_value<std::string>* _description;

#ifdef GOMOKU_SERVER
    std::string _game_id;
#endif

    // Deserialization constructor
    opening_rules(std::string id,
                  RulesetType ruleset,
                  serializable_value<std::string>* description);

    // Checks if a string pertains to a valid ruleset, and generate description
    static std::string generate_description_from_ruleset(RulesetType ruleset);


public:
    // constructors
    explicit opening_rules(RulesetType ruleset);   // for client
    ~opening_rules();

#ifdef GOMOKU_SERVER
    opening_rules(std::string id, RulesetType ruleset);  // for server
    std::string get_game_id();
    void set_game_id(std::string game_id);
#endif

    // accessors
    RulesetType get_ruleset() const noexcept;
    std::string get_description() const noexcept;

#ifdef GOMOKU_SERVER
    // state update functions
    void set_opening_rule(RulesetType ruleset);
#endif

    // serialization
    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;
    static opening_rules* from_json(const rapidjson::Value& json);
};

#endif //GOMOKU_OPENING_RULES_H
