#ifndef GOMOKU_CLIENT_NETWORK_MANAGER_H
#define GOMOKU_CLIENT_NETWORK_MANAGER_H


#include <string>
#include "response_listener_thread.h"
#include "../../common/network/requests/client_request.h"


class client_network_manager {

public:
    static void init(const std::string& host, const uint16_t port);

    static void send_request(const client_request& request);

    static void parse_response(const std::string& message);

private:
    static bool connect(const std::string& host, const uint16_t port);


    static sockpp::tcp_connector* _connection;

    static bool _connection_success;
    static bool _failed_to_connect;

};


#endif //GOMOKU_CLIENT_NETWORK_MANAGER_H
