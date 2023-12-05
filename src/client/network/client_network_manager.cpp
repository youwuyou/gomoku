#include "client_network_manager.h"


#include "../game_controller.h"
#include "../../common/network/responses/server_response.h"
#include <sockpp/exception.h>


// initialize static members
sockpp::tcp_connector* client_network_manager::_connection = nullptr;

bool client_network_manager::_connection_success = false;
bool client_network_manager::_failed_to_connect = false;

void::client_network_manager::init(const std::string& host, const uint16_t port) {

    // initialize sockpp framework
    sockpp::socket_initializer sockInit;

    // reset connection status
    client_network_manager::_connection_success = false;
    client_network_manager::_failed_to_connect = false;

    // delete exiting connection and create new one
    if (client_network_manager::_connection != nullptr) {
        client_network_manager::_connection->shutdown();
        delete client_network_manager::_connection;
    }
    client_network_manager::_connection = new sockpp::tcp_connector();

    // try to connect to server
    if (client_network_manager::connect(host, port)) {
        game_controller::show_status("Connected to " + host + ":" + std::to_string(port));
        client_network_manager::_connection_success = true;

        // start network thread
        response_listener_thread* responseListenerThread = new response_listener_thread(client_network_manager::_connection);
        if (responseListenerThread->Run() != wxTHREAD_NO_ERROR) {
            game_controller::show_error("Connection error", "Could not create client network thread");
        }

    } else {
        client_network_manager::_failed_to_connect = true;
        game_controller::show_status("Not connected");
    }
}


bool client_network_manager::connect(const std::string& host, const uint16_t port) {

    // create sockpp address and catch any errors
    sockpp::inet_address address;
    try {
        address = sockpp::inet_address(host, port);
    } catch (const sockpp::getaddrinfo_error& e) {
        game_controller::show_error("Connection error", "Failed to resolve address " + e.hostname());
        return false;
    }

    // establish connection to given address
    if (!client_network_manager::_connection->connect(address)) {
        game_controller::show_error("Connection error", "Failed to connect to server " + address.to_string());
        return false;
    }

    return true;
}


void client_network_manager::send_request(const client_request &request) {
    // wait until network is connected (max. 5 seconds)
    int connectionCheckCounter = 0;
    while (!client_network_manager::_connection_success
          && !client_network_manager::_failed_to_connect
          && connectionCheckCounter < 200) {
        wxMilliSleep(25);
        connectionCheckCounter++;
    }

    // do not continue if failed to connect to server
    if (client_network_manager::_failed_to_connect) {
        return;
    }

    if (client_network_manager::_connection_success && client_network_manager::_connection->is_connected()) {
        // serialize request into JSON string
        rapidjson::Document* jsonDocument = request.to_json();
        std::string message = json_utils::to_string(jsonDocument);
        delete jsonDocument;

        // turn message into stream and prepend message length
        std::stringstream messageStream;
        messageStream << std::to_string(message.size()) << ':' << message;
        message = messageStream.str();

        // output message for debugging purposes
#ifdef PRINT_NETWORK_MESSAGES
        std::cout << "\nSending request : " << message << std::endl;
#endif

        // send message to server
        ssize_t bytesSent = client_network_manager::_connection->write(message);

        // if the number of bytes sent does not match the length of the message, probably something went wrong
        if (bytesSent != ssize_t(message.length())) {
            game_controller::show_error("Network error", "Error writing to the TCP stream: " +
                                                         client_network_manager::_connection->last_error_str());
        }

    } else {
        game_controller::show_error("Network error", "Lost connection to server");
    }
}


void client_network_manager::parse_response(const std::string& message) {

    // output message for debugging purposes
#ifdef PRINT_NETWORK_MESSAGES
    std::cout << "\nReceived response : " << message << std::endl;
#endif

    rapidjson::Document json = rapidjson::Document(rapidjson::kObjectType);
    json.Parse(message.c_str());

    try {
        server_response* res = server_response::from_json(json);
        res->Process();

    } catch (std::exception e) {
        game_controller::show_error("JSON parsing error",
                                    "Failed to parse message from server:\n" + message + "\n" + (std::string) e.what());
    }
}