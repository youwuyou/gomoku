//
// Created by Manuel on 25.01.2021.
//

#ifndef LAMA_PLAYER_H
#define LAMA_PLAYER_H


#include <string>
#include "hand.h"
#include "../../serialization/uuid_generator.h"
#include "../../../../rapidjson/include/rapidjson/document.h"
#include "../../serialization/unique_serializable.h"
#include "../../serialization/serializable_value.h"

class player : public unique_serializable {
private:
    serializable_value<std::string>* _player_name;
    serializable_value<bool>* _has_folded;
    serializable_value<int>* _score;
    hand* _hand;

#ifdef LAMA_SERVER
    std::string _game_id;
#endif

    /*
     * Deserialization constructor
     */
    player(std::string id,
           serializable_value<std::string>* name,
           serializable_value<int>* score,
           hand* hand,
           serializable_value<bool>* has_folded);

public:
// constructors
    explicit player(std::string name);   // for client
    ~player();

#ifdef LAMA_SERVER
    player(std::string id, std::string name);  // for server

    std::string get_game_id();
    void set_game_id(std::string game_id);
#endif

    // accessors
    int get_score() const noexcept;
    bool has_folded() const noexcept;
    int get_nof_cards() const noexcept;
    const hand* get_hand() const noexcept;
    std::string get_player_name() const noexcept;

#ifdef LAMA_SERVER
    // state update functions
    bool fold(std::string& err);
    bool add_card(card* card, std::string& err);
    bool remove_card(std::string card_id, card*& card, std::string& err);

    void wrap_up_round(std::string& err);
    void setup_round(std::string& err);
#endif


    // serialization
    static player* from_json(const rapidjson::Value& json);
    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;

};


#endif //LAMA_PLAYER_H
