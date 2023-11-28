//
// Created by Manuel on 25.01.2021.
//

#ifndef GOMOKU_PLAYER_H
#define GOMOKU_PLAYER_H


#include <string>
#include "../../serialization/uuid_generator.h"
#include "../../../../rapidjson/include/rapidjson/document.h"
#include "../../serialization/unique_serializable.h"
#include "../../serialization/serializable_value.h"


enum player_colour_type{
    black,
    white,
};

class player : public unique_serializable {
private:
    serializable_value<std::string>* _player_name;
    serializable_value<int>* _score;
    player_colour_type _colour;

#ifdef GOMOKU_SERVER
    std::string _game_id;
#endif

    /*
     * Deserialization constructor
     */
    player(std::string id,
           serializable_value<std::string>* name,
           serializable_value<int>* score,
           player_colour_type colour);

public:
// constructors
    explicit player(std::string name, player_colour_type colour);   // for client
    ~player();

    // for deserialization
    static const std::unordered_map<std::string, player_colour_type> _string_to_player_colour_type;
    // for serialization
    static const std::unordered_map<player_colour_type, std::string> _player_colour_type_to_string;

#ifdef GOMOKU_SERVER
    player(std::string id, std::string name, player_colour_type colour);  // for server

    std::string get_game_id();
    void set_game_id(std::string game_id);
#endif

    // accessors
    int get_score() const noexcept;
    player_colour_type get_colour() const noexcept;
    std::string get_player_name() const noexcept;

#ifdef GOMOKU_SERVER
    // state update functions
    void increment_score(std::string& err);
    bool reset_score(std::string& err);
    void swap_colour(std::string& err);

#endif


    // serialization
    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;
    static player* from_json(const rapidjson::Value& json);

};


#endif //GOMOKU_PLAYER_H
