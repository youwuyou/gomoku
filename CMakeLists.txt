cmake_minimum_required(VERSION 3.15)
project(Gomoku) # your project name

set(CMAKE_CXX_STANDARD 20)

include_directories(sockpp/include)

find_package(wxWidgets COMPONENTS core base net REQUIRED)
include(${wxWidgets_USE_FILE})

# define a variable CLIENT_SOURCE_FILES that contains the paths to all source files required to compile the client executable
set(CLIENT_SOURCE_FILES
        src/client/main.cpp
        src/client/app/Gomoku.cpp src/client/app/Gomoku.h
        src/client/GameController.cpp src/client/GameController.h
        # UI
        src/client/windows/GameWindow.cpp src/client/windows/GameWindow.h
        src/client/uiElements/ImagePanel.cpp src/client/uiElements/ImagePanel.h
        src/client/panels/ConnectionPanel.cpp src/client/panels/ConnectionPanel.h
        src/client/panels/MainGamePanel.cpp src/client/panels/MainGamePanel.h
        src/client/uiElements/InputField.cpp src/client/uiElements/InputField.h
        src/client/uiElements/ImagePanel.cpp src/client/uiElements/ImagePanel.h
        # network
        src/client/network/ClientNetworkManager.cpp src/client/network/ClientNetworkManager.h
        src/client/network/ResponseListenerThread.cpp src/client/network/ResponseListenerThread.h
        # game state
        src/common/game_state/cards/card.cpp src/common/game_state/cards/card.h
        src/common/game_state/game_state.cpp src/common/game_state/game_state.h
        src/common/game_state/player/hand.cpp src/common/game_state/player/hand.h
        src/common/game_state/player/player.cpp src/common/game_state/player/player.h
        src/common/game_state/cards/draw_pile.cpp src/common/game_state/cards/draw_pile.h
        src/common/game_state/cards/discard_pile.cpp src/common/game_state/cards/discard_pile.h
        # client requests
        src/common/network/requests/client_request.cpp src/common/network/requests/client_request.h
        src/common/network/requests/join_game_request.cpp src/common/network/requests/join_game_request.h
        src/common/network/requests/start_game_request.cpp src/common/network/requests/start_game_request.h
        # server responses
        src/common/network/responses/server_response.cpp src/common/network/responses/server_response.h
        src/common/network/responses/request_response.cpp src/common/network/responses/request_response.h
        src/common/network/responses/full_state_response.cpp src/common/network/responses/full_state_response.h
        # serialization
        src/common/serialization/serializable.h
        src/common/serialization/value_type_helpers.h
        src/common/serialization/vector_utils.h
        src/common/serialization/serializable_value.h
        src/common/serialization/json_utils.h
        src/common/serialization/uuid_generator.h
        src/common/serialization/unique_serializable.cpp src/common/serialization/unique_serializable.h)

# define a variable SERVER_SOURCE_FILES that contains the paths to all source files required to compile the server executable
set(SERVER_SOURCE_FILES
        src/server/main.cpp
        src/server/game_instance.cpp src/server/game_instance.h
        src/server/game_instance_manager.cpp src/server/game_instance_manager.h
        src/server/player_manager.cpp src/server/player_manager.h
        src/server/server_network_manager.cpp src/server/server_network_manager.h
        # game state
        src/common/game_state/cards/card.cpp src/common/game_state/cards/card.h
        src/common/game_state/game_state.cpp src/common/game_state/game_state.h
        src/common/game_state/player/hand.cpp src/common/game_state/player/hand.h
        src/common/game_state/player/player.cpp src/common/game_state/player/player.h
        src/common/game_state/cards/draw_pile.cpp src/common/game_state/cards/draw_pile.h
        src/common/game_state/cards/discard_pile.cpp src/common/game_state/cards/discard_pile.h
        # client requests
        src/common/network/requests/client_request.cpp src/common/network/requests/client_request.h
        src/common/network/requests/join_game_request.cpp src/common/network/requests/join_game_request.h
        src/common/network/requests/start_game_request.cpp src/common/network/requests/start_game_request.h
        # server responses
        src/common/network/responses/server_response.cpp src/common/network/responses/server_response.h
        src/common/network/responses/request_response.cpp src/common/network/responses/request_response.h
        src/common/network/responses/full_state_response.cpp src/common/network/responses/full_state_response.h
        # serialization
        src/common/serialization/serializable.h
        src/common/serialization/value_type_helpers.h
        src/common/serialization/vector_utils.h
        src/common/serialization/serializable_value.h
        src/common/serialization/json_utils.h
        src/common/serialization/uuid_generator.h
        src/common/serialization/unique_serializable.cpp src/common/serialization/unique_serializable.h src/server/request_handler.h src/server/request_handler.cpp)


# set source files for client-executable
add_executable(Gomoku-client ${CLIENT_SOURCE_FILES})
# set compile directives for client-executable
target_compile_definitions(Gomoku-client PRIVATE GOMOKU_CLIENT=1 RAPIDJSON_HAS_STDSTRING=1)
# link with wxWidgets
target_link_libraries(Gomoku-client ${wxWidgets_LIBRARIES})
# Comment out if you don't want to print network-related messages into the console
target_compile_definitions(Gomoku-client PRIVATE PRINT_NETWORK_MESSAGES=1)

# set source files for server-executable
add_executable(Gomoku-server ${SERVER_SOURCE_FILES})
# set compile directives for server-executable
target_compile_definitions(Gomoku-server PRIVATE GOMOKU_SERVER=1 RAPIDJSON_HAS_STDSTRING=1)
# Comment out if you don't want to print network-related messages into the console
target_compile_definitions(Gomoku-server PRIVATE PRINT_NETWORK_MESSAGES=1)


# linking to sockpp
if(WIN32)
    message("Detected Win32")

    target_link_libraries(Gomoku-client ${CMAKE_SOURCE_DIR}/sockpp/cmake-build-debug/sockpp-static.lib)
    target_link_libraries(Gomoku-server ${CMAKE_SOURCE_DIR}/sockpp/cmake-build-debug/sockpp-static.lib)

    # Necessary to get sockets working under Windows (with MingW)
    target_link_libraries(Gomoku-client wsock32 ws2_32)
else()
    message("Not Win32, so probably a Linux") # We assume it's Linux in this case

    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)

    target_link_libraries(Gomoku-client ${CMAKE_SOURCE_DIR}/sockpp/cmake-build-debug/libsockpp.so Threads::Threads)
    target_link_libraries(Gomoku-server ${CMAKE_SOURCE_DIR}/sockpp/cmake-build-debug/libsockpp.so Threads::Threads)
endif()

# copy assets (images) to binary directory
file(INSTALL assets DESTINATION ${CMAKE_BINARY_DIR})


set(CMAKE_CXX_FLAGS "--coverage")
# set source files for server-library
add_library(Gomoku-lib ${SERVER_SOURCE_FILES})
# set compile directives for server-library
target_compile_definitions(Gomoku-lib PRIVATE GOMOKU_SERVER=1 RAPIDJSON_HAS_STDSTRING=1)

# Enables testing for this directory and below
enable_testing()
add_subdirectory(googletest)
add_subdirectory(unit-tests)