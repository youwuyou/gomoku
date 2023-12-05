#ifndef GOMOKU_CLIENT_NETWORK_THREAD_H
#define GOMOKU_CLIENT_NETWORK_THREAD_H

#include <functional>
#include <wx/wx.h>
#include "sockpp/tcp_socket.h"
#include "sockpp/tcp_connector.h"


class response_listener_thread : public wxThread {

public:
    response_listener_thread(sockpp::tcp_connector* connection);
    ~response_listener_thread();

protected:
    virtual ExitCode Entry();

private:
    void output_error(std::string title, std::string message);


    sockpp::tcp_connector* _connection;

};

#endif //GOMOKU_CLIENT_NETWORK_THREAD_H
