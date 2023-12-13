#include "response_listener_thread.h"


#include <iostream>
#include <string>
#include "../game_controller.h"
#include "client_network_manager.h"


response_listener_thread::response_listener_thread(sockpp::tcp_connector* connection) {
    this->_connection = connection;
}


response_listener_thread::~response_listener_thread() {
    this->_connection->shutdown();
}


wxThread::ExitCode response_listener_thread::Entry() {
    try {
        char buffer[512]; // 512 bytes
        ssize_t count = 0;

        while ((count = this->_connection->read(buffer, sizeof(buffer))) > 0) {
            try {
                int pos = 0;

                // extract length of message in bytes (which is sent at the start of the message, and is separated by a ":")
                std::stringstream message_length_stream;
                while (buffer[pos] != ':' && pos < count) {
                    message_length_stream << buffer[pos];
                    pos++;
                }
                ssize_t message_length = std::stoi(message_length_stream.str());

                // initialize a stream for the message
                std::stringstream message_stream;

                // copy everything following the message length declaration into a stringstream
                message_stream.write(&buffer[pos + 1], count - (pos + 1));
                ssize_t bytes_read_so_far = count - (pos + 1);

                // read remaining packages until full message length is reached
                while (bytes_read_so_far < message_length && count != 0) {
                    count = this->_connection->read(buffer, sizeof(buffer));
                    message_stream.write(buffer, count);
                    bytes_read_so_far += count;
                }

                // process message (if we've received entire message)
                if (bytes_read_so_far == message_length) {
                    std::string message = message_stream.str();
                    game_controller::get_main_thread_event_handler()->CallAfter([message]{
                        client_network_manager::parse_response(message);
                    });

                } else {
                    this->output_error("Network error",
                                       "Could not read entire message. TCP stream ended early. Difference is " +
                                       std::to_string(message_length - bytes_read_so_far) + " bytes");
                }

            } catch (std::exception& e) {
                // Make sure the connection isn't terminated only because of a read error
                this->output_error("Network error", "Error while reading message: " + (std::string) e.what());
            }
        }

        if (count <= 0) {
            this->output_error("Network error",
                               "Read error [" + std::to_string(this->_connection->last_error()) + "]: " +
                               this->_connection->last_error_str());
        }

    } catch(const std::exception& e) {
        this->output_error("Network error", "Error in listener thread: " + (std::string) e.what());
    }

    this->_connection->shutdown();

    return (wxThread::ExitCode) 0; // everything okay
}


void response_listener_thread::output_error(std::string title, std::string message) {
    game_controller::get_main_thread_event_handler()->CallAfter([title, message]{
        game_controller::show_error(title, message);
    });
}
