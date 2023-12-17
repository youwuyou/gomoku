#!/bin/bash

# Function to run a server and two clients
gnome-terminal -- cmake-build-debug/Gomoku-server
gnome-terminal -- cmake-build-debug/Gomoku-client
gnome-terminal -- cmake-build-debug/Gomoku-client
