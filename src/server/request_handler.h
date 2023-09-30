//
// Created by Manuel on 08.02.2022.
//

#ifndef GOMOKU_REQUEST_HANDLER_H
#define GOMOKU_REQUEST_HANDLER_H

#include "../common/network/responses/server_response.h"
#include "../common/network/requests/client_request.h"
#include "../common/network/responses/request_response.h"

class request_handler {
public:
    static request_response* handle_request(const client_request* const req);
};
#endif //GOMOKU_REQUEST_HANDLER_H
