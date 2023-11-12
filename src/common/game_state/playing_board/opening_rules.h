//
// Created by stephenlincon on 11.11.23.
//

#ifndef GOMOKU_OPENING_RULES_H
#define GOMOKU_OPENING_RULES_H

#include <string>
#include "../../serialization/uuid_generator.h"
#include "../../../../rapidjson/include/rapidjson/document.h"
#include "../../serialization/unique_serializable.h"
#include "../../serialization/serializable_value.h"

class opening_rules : public unique_serializable {
private:
    serializable_value<std::string>* _ruleset;
    serializable_value<std::string>* _description;

#ifdef GOMOKU_SERVER
    std::string _game_id;
#endif

    // Deserialization constructor
    opening_rules(std::string id,
                  serializable_value<std::string>* ruleset,
                  serializable_value<std::string>* description);

    // Checks if a string pertains to a valid ruleset, and generate description
    std::string generate_description_from_ruleset(std::string ruleset) const;


public:
    // constructors
    explicit opening_rules(std::string ruleset);   // for client
    ~opening_rules();

#ifdef GOMOKU_SERVER
    opening_rules(std::string id, std::string ruleset);  // for server
    std::string get_game_id();
    void set_game_id(std::string game_id);
#endif

    // accessors
    std::string get_ruleset() const noexcept;
    std::string get_description() const noexcept;

#ifdef GOMOKU_SERVER
    // state update functions
    void set_opening_rule(std::string ruleset);
#endif

    // serialization
    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;
    static opening_rules* from_json(const rapidjson::Value& json);
};


#endif //GOMOKU_OPENING_RULES_H
