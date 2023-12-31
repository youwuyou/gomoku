//
// Created by Manuel on 08.02.2022.
//

#ifndef GOMOKU_REQUEST_HANDLER_CPP
#define GOMOKU_REQUEST_HANDLER_CPP

#include "request_handler.h"

#include "player_manager.h"
#include "game_instance_manager.h"
#include "game_instance.h"

#include "../common/network/requests/join_game_request.h"
#include "../common/network/requests/place_stone_request.h"
#include "../common/network/requests/swap_decision_request.h"
#include "../common/network/requests/select_game_mode_request.h"
#include "../common/network/requests/restart_game_request.h"
#include "../common/network/requests/forfeit_request.h"


request_response* request_handler::handle_request(const client_request* const req) {

    // Prepare variables that are used by every request type
    player* player;
    std::string err;
    game_instance* game_instance_ptr = nullptr;

    // Get common properties of requests
    request_type type = req->get_type();
    std::string req_id = req->get_req_id();
    std::string game_id = req->get_game_id();
    std::string player_id = req->get_player_id();


    // Switch behavior according to request type
    switch(type) {

        // ##################### JOIN GAME #####################  //
        case request_type::join_game: {
            std::string player_name = ((join_game_request *) req)->get_player_name();

            // Create new player or get existing one with that name
            player_manager::add_or_get_player(player_name, player_id, player);

            if (game_id.empty()) {
                // join any game
                if (game_instance_manager::try_add_player_to_any_game(player, game_instance_ptr, err)) {
                    // game_instance_ptr got updated to the joined game

                    // return response with full game_state attached
                    return new request_response(game_instance_ptr->get_id(), req_id, true,
                                                game_instance_ptr->get_game_state()->to_json(), err);
                } else {
                    // failed to find game to join
                    return new request_response("", req_id, false, nullptr, err);
                }
            } else {
                // join a specific game denoted by req->get_game_id()
                if (game_instance_manager::try_get_game_instance(game_id, game_instance_ptr)) {
                    if (game_instance_manager::try_add_player(player, game_instance_ptr, err)) {
                        // return response with full game_state attached
                        return new request_response(game_id, req_id, true,
                                                    game_instance_ptr->get_game_state()->to_json(), err);
                    } else {
                        // failed to join requested game
                        return new request_response("", req_id, false, nullptr, err);
                    }
                } else {
                    // failed to find requested game
                    return new request_response("", req_id, false, nullptr, "Requested game could not be found.");
                }
            }
        }


        // ##################### START GAME ##################### //
        case request_type::start_game: {
            if (game_instance_manager::try_get_player_and_game_instance(player_id, player, game_instance_ptr, err)) {
                if (game_instance_ptr->start_game(player, err)) {
                    return new request_response(game_instance_ptr->get_id(), req_id, true,
                                                game_instance_ptr->get_game_state()->to_json(), err);
                }
            }
            return new request_response("", req_id, false, nullptr, err);
        }

        // ##################### PLACE STONE ##################### //
        case request_type::place_stone: {
            if (game_instance_manager::try_get_player_and_game_instance(player_id, player, game_instance_ptr, err)) {
                unsigned int x = (dynamic_cast<const place_stone_request *>(req))->get_stone_x();
                unsigned int y = (dynamic_cast<const place_stone_request *>(req))->get_stone_y();
                field_type colour = (dynamic_cast<const place_stone_request *>(req))->get_stone_colour();
                if (game_instance_ptr->place_stone(player, x, y, colour, err)) {
                    return new request_response(game_instance_ptr->get_id(), req_id, true,
                                                game_instance_ptr->get_game_state()->to_json(), err);
                }
            }
            return new request_response("", req_id, false, nullptr, err);
        }

        // ##################### SWAP COLOUR ##################### //
        case request_type::swap_colour: {
            if (game_instance_manager::try_get_player_and_game_instance(player_id, player, game_instance_ptr, err)) {
                const swap_decision_type swap_decision = (dynamic_cast<const swap_decision_request *>(req))->get_swap_decision();
                if (game_instance_ptr->do_swap_decision(player, swap_decision, err)) {
                    return new request_response(game_instance_ptr->get_id(), req_id, true,
                                                game_instance_ptr->get_game_state()->to_json(), err);
                }
            }
            return new request_response("", req_id, false, nullptr, err);
        }

        // ##################### SELECT GAME MODE ##################### //
        case request_type::select_game_mode: {
            if (game_instance_manager::try_get_player_and_game_instance(player_id, player, game_instance_ptr, err)) {
                const std::string& ruleset_string = (dynamic_cast<const select_game_mode_request *>(req))->get_ruleset_string();
                if (game_instance_ptr->set_game_mode(player, ruleset_string, err)) {
                    return new request_response(game_instance_ptr->get_id(), req_id, true,
                                                game_instance_ptr->get_game_state()->to_json(), err);
                }
            }
            return new request_response("", req_id, false, nullptr, err);
        }

        // ##################### RESTART GAME ##################### //
        case request_type::restart_game: {
            if (game_instance_manager::try_get_player_and_game_instance(player_id, player, game_instance_ptr, err)) {
                bool change_ruleset = (dynamic_cast<const restart_game_request *>(req))->get_change_ruleset();
                if (change_ruleset){
                    if(game_instance_ptr->get_game_state()->prepare_game(player, err)){
                        if (game_instance_ptr->get_game_state()->get_players().at(0)->reset_score(err) &&
                            game_instance_ptr->get_game_state()->get_players().at(1)->reset_score(err)){
                            if (game_instance_ptr->set_game_mode(player, "uninitialized", err)){
                                return new request_response(game_instance_ptr->get_id(), req_id, true,
                                                        game_instance_ptr->get_game_state()->to_json(), err);
                            }
                        }
                    }
                } else {
                    if (game_instance_ptr->start_game(player, err)) {
                        return new request_response(game_instance_ptr->get_id(), req_id, true,
                                                    game_instance_ptr->get_game_state()->to_json(), err);
                    }
                }
            }
            return new request_response("", req_id, false, nullptr, err);
        }

        // ##################### FORFEIT ##################### //
        case request_type::forfeit: {
            if (game_instance_manager::try_get_player_and_game_instance(player_id, player, game_instance_ptr, err)) {
                if (game_instance_ptr->do_forfeit(player, err)) {
                    return new request_response(game_instance_ptr->get_id(), req_id, true,
                                                game_instance_ptr->get_game_state()->to_json(), err);
                }
            }
        }

        // ##################### UNKNOWN REQUEST ##################### //
        default:
            return new request_response("", req_id, false, nullptr, "Unknown request_type " + type);
    }
}

#endif //GOMOKU_REQUEST_HANDLER_CPP
