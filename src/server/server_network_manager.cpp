//
// Created by Manuel on 12.02.2021.
//
// The server_network_manager handles all incoming messages and offers functionality to broadcast messages
// to all connected players of a game.

#include "server_network_manager.h"
#include "request_handler.h"

// include server address configurations
#include "../common/network/default.conf"
#include "../common/network/responses/request_response.h"


server_network_manager::server_network_manager() {
    if (_instance == nullptr) {
        _instance = this;
    }
    sockpp::socket_initializer socket_initializer; // Required to initialise sockpp
    this->connect(default_server_host, default_port);   // variables from "default.conf"
}

server_network_manager::~server_network_manager() = default;

void server_network_manager::connect(const std::string &url, const uint16_t port) {
    this->_acc = sockpp::tcp_acceptor(port);

    if (!_acc) {
        std::cerr << "Error creating the acceptor: " << _acc.last_error_str() << std::endl;
        return;
    }

    std::cout << "Awaiting connections on port " << port << "..." << std::endl;
    listener_loop();    // start endless loop
}

void server_network_manager::listener_loop() {
    // intentional endless loop
    while (true) {
        sockpp::inet_address peer;

        // Accept a new client connection
        sockpp::tcp_socket sock = _acc.accept(&peer);
        std::cout << "Received a connection request from " << peer << std::endl;

        if (!sock) {
            std::cerr << "Error accepting incoming connection: "
                      << _acc.last_error_str() << std::endl;
        } else {
            _rw_lock.lock();
            _address_to_socket.emplace(sock.peer_address().to_string(), std::move(sock.clone()));
            _rw_lock.unlock();
            // Create a listener thread and transfer the new stream to it.
            // Incoming messages will be passed to handle_incoming_message().
            std::thread listener(
                    read_message,
                    std::move(sock),
                    handle_incoming_message);

            listener.detach();
        }
    }
}

// Runs in a thread and reads anything coming in on the 'socket'.
// Once a message is fully received, the string is passed on to the 'handle_incoming_message()' function
void server_network_manager::read_message(sockpp::tcp_socket socket, const std::function<void(const std::string&,
                                                                                              const sockpp::tcp_socket::addr_t &)> &message_handler) {
    sockpp::socket_initializer sockInit;    // initializes socket framework underneath

    char buffer[512]; // 512 bytes
    ssize_t count = 0;
    ssize_t msg_bytes_read = 0;
    ssize_t msg_length = 0;

    while ((count = socket.read(buffer, sizeof(buffer))) > 0) {
        try {
            int i = 0;
            std::stringstream ss_msg_length;
            while (buffer[i] != ':' && i < count) {
                ss_msg_length << buffer[i];
                i++;
            }
            msg_length = std::stoi(ss_msg_length.str());
            std::cout << "Expecting message of length " << msg_length << std::endl;

            // put everything after the message length declaration into a stringstream
            std::stringstream ss_msg;
            ss_msg.write(&buffer[i+1], count - (i + 1));
            msg_bytes_read = count - (i + 1);

            // read the remaining packages
            while (msg_bytes_read < msg_length && count > 0) {
                count = socket.read(buffer, sizeof(buffer));
                msg_bytes_read += count;
                ss_msg.write(buffer, count);
            }

            if (msg_bytes_read == msg_length) {
                // sanity check that really all bytes got read (possibility that count was <= 0, indicating a read error)
                std::string msg = ss_msg.str();
                message_handler(msg, socket.peer_address());    // attempt to parse client_request from 'msg'
            } else {
                std::cerr << "Could not read entire message. TCP stream ended before. Difference is " << msg_length - msg_bytes_read << std::endl;
            }
        } catch (std::exception& e) { // Make sure the connection isn't torn down only because of a read error
            std::cerr << "Error while reading message from " << socket.peer_address() << std::endl << e.what() << std::endl;
        }
    }
    if (count <= 0) {
        std::cout << "Read error [" << socket.last_error() << "]: "
                  << socket.last_error_str() << std::endl;
    }

    std::cout << "Closing connection to " << socket.peer_address() << std::endl;
    socket.shutdown();
}


void server_network_manager::handle_incoming_message(const std::string& msg, const sockpp::tcp_socket::addr_t& peer_address) {
    try {
        // try to parse a json from the 'msg'
        rapidjson::Document req_json;
        req_json.Parse(msg.c_str());
        // try to parse a client_request from the json
        client_request* req = client_request::from_json(req_json);

        // check if this is a connection to a new player
        std::string player_id = req->get_player_id();
        _rw_lock.lock_shared();
        if (_player_id_to_address.find(player_id) == _player_id_to_address.end()) {
            // save connection to this client
            _rw_lock.unlock_shared();
            std::cout << "New client with id " << player_id << std::endl;
            _rw_lock.lock();
            _player_id_to_address.emplace(player_id, peer_address.to_string());
            _rw_lock.unlock();
        } else {
            _rw_lock.unlock_shared();
        }
#ifdef PRINT_NETWORK_MESSAGES
        std::cout << "Received valid request : " << msg << std::endl;
#endif
        // execute client request
        server_response* res = request_handler::handle_request(req);
        delete req;

        // transform response into a json
        rapidjson::Document* res_json = res->to_json();
        delete res;

        // transform json to string
        std::string res_msg = json_utils::to_string(res_json);

#ifdef PRINT_NETWORK_MESSAGES
        std::cout << "Sending response : " << res_msg << std::endl;
#endif

        // send response back to client
        send_message(res_msg, peer_address.to_string());
        delete res_json;
    } catch (const std::exception& e) {
        std::cerr << "Failed to execute client request. Content was :\n"
                  << msg << std::endl
                  << "Error was " << e.what() << std::endl;
    }
}


void server_network_manager::on_player_left(std::string player_id) {
    _rw_lock.lock();
    std::string address = _player_id_to_address[player_id];
    _player_id_to_address.erase(player_id);
    _address_to_socket.erase(address);
    _rw_lock.unlock();
}

ssize_t server_network_manager::send_message(const std::string &msg, const std::string& address) {

    std::stringstream ss_msg;
    ss_msg << std::to_string(msg.size()) << ':' << msg; // prepend message length
    return _address_to_socket.at(address).write(ss_msg.str());
}

void server_network_manager::broadcast_message(server_response &msg, const std::vector<player *> &players,
                                               const player *exclude) {
    rapidjson::Document* msg_json = msg.to_json();  // write to JSON format
    std::string msg_string = json_utils::to_string(msg_json);   // convert to string

#ifdef PRINT_NETWORK_MESSAGES
    std::cout << "Broadcasting message : " << msg_string << std::endl;
#endif

    _rw_lock.lock_shared();
    // send object_diff to all requested players
    try {
        for (auto& player : players) {
            if (player != exclude) {
                int nof_bytes_written = send_message(msg_string, _player_id_to_address.at(player->get_id()));
            }
        }
    } catch (std::exception& e) {
        std::cerr << "Encountered error when sending state update: " << e.what() << std::endl;
    }
    _rw_lock.unlock_shared();
    delete msg_json;
}



