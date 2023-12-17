<img src="./assets/gomoku_logo.png" alt="Gomoku-logo" width="300"/>


This is a simple C++ implementation of a classical Japanese board game called "Gomoku". You can read the game's rules [here](https://www.wikiwand.com/en/Gomoku). The implementation features a client/server architecture for 2-player scenarios.

## 1. Compile instructions
This project only works on UNIX systems (Linux / MacOS). We recommend using [Ubuntu](https://ubuntu.com/#download), as it offers the easiest way to setup wxWidgets. Therefore, we explain installation only for Ubuntu systems. The following was tested on a Ubuntu 20.4 system, but should also work for earlier versions of Ubuntu.

**Note:** If you create a virtual machine, we recommend to give the virtual machine **at least 12GB** of (dynamic) harddrive space (CLion and wxWidgets need quite a lot of space).

### 1.1 Prepare OS Environment

#### Ubuntu 20.4
The OS should already have git installed. If not, you can use: 
`sudo apt-get install git`

Then use  `git clone` to fetch this repository.

Execute the following commands in a console:
1. `sudo apt-get update`
2. `sudo apt-get install build-essential` followed by `sudo reboot`
3. if on virtual machine : install guest-additions (https://askubuntu.com/questions/22743/how-do-i-install-guest-additions-in-a-virtualbox-vm) and then `sudo reboot`
4. `sudo snap install clion --classic` this installs the latest stable CLion version
5. `sudo apt-get install libwxgtk3.0-gtk3-dev libwxgtk-media3.0-gtk3-dev` this installs wxWidgets (GUI library used in this project)


### 1.2 Compile Code

#### Using CLion
1. Open CLion
2. Click `File > Open...` and there select the **/sockpp** folder of this project
3. Click `Build > Build all in 'Debug'`
4. Wait until sockpp is compiled (from now on you never have to touch sockpp again ;))
5. Click `File > Open...` select the **/gomoku** folder
6. Click `Build > Build all in 'Debug'`
7. Wait until Gomoku-server, Gomoku-client and Gomoku-tests are compiled


#### Without CLion
We also provide build scripts that you can utilize for the build process, in case you do not use the CLion IDE. For building the game, you could either run the `build-run.sh` script with either "Unix Makefiles" of "Ninja" as argument

1. Make sure you locate at the root, using `pwd` you should see `/path/to/gomoku`
2. Choose to build with/without [Ninja](https://ninja-build.org/). Build directories named `cmake-build-debug` will appear under sockpp directory and then the root of the project 
    - either `./build-run.sh "Unix Makefiles"`
    - or **way faster** with `./build-run.sh "Ninja"`, but requires the build tool Ninja to be installed
3. Wait until the executables are compiled

For your convinience, we also provide a script that allows us to remove the `cmake-build-debug` directories. Execute it using `./build-clean.sh`.


######  NOTE: Do not forget to grant permissions to the scripts using `chmod +x script_name.sh`




## 2. Run the Game
1. Open a console in the project folder, navigate into "cmake-build-debug" `cd cmake-build-debug`
2. Run server `./Gomoku-server`
3. In new consoles run two clients to get two players `./Gomoku-client`

## 3. Run the Unit Tests

### With CLion
1. CLion should automatically create a Google Test configuration Gomoku-tests which will run all tests. See [Google Test run/debug configuration﻿](https://www.jetbrains.com/help/clion/creating-google-test-run-debug-configuration-for-test.html#gtest-config) for more information.
2. From the list on the main toolbar, select the configuration Gomoku-tests.
3. Click ![run](https://resources.jetbrains.com/help/img/idea/2021.1/artwork.studio.icons.shell.toolbar.run.svg) or press `Shift+F10`.
   
You can run individual tests or test suites by opening the corresponding file in the **/unit-tests** folder and clicking ![run](https://resources.jetbrains.com/help/img/idea/2021.1/artwork.studio.icons.shell.toolbar.run.svg) next to the test method or class. For more information on testing in CLion read the [documentation](https://www.jetbrains.com/help/clion/performing-tests.html).


### Without CLion

1. Make sure you locate at the `cmake-build-debug` directory of the whole project

- you should see `path/to/gomoku/cmake-build-debug` when using `pwd`

2. Perform testing using `ctest -V`



## 4. Instruction for Developers
You don't need to look at the **/sockpp**, **/rapidjson** or **/googletest** folder, as they simply contain 3rd party code that should not be changed.

The code for the game can be found in **/src**, where it is separated into following folders:
- **/client** contains only code that is used on the client side (e.g. UI, sending messages)
- **/common** contains code that is shared between server and client.
    - **/exceptions** contains the exception class used on server and client side. You don't need to change anything in here (unless you want to rename the GomokuException class ;))
    - **/game_state** contains the `game_state` that is synchronized between client and server. We use the [conditional pre-compile directive](https://www.cplusplus.com/doc/tutorial/preprocessor/) Gomoku_SERVER to enable certain parts of the code only on the server side. Namely, these are the state update functions, as they should only happen on the server. The client simply reflects the current game state as sent by the server without modifying it directly. 
    - **/network** contains all the messages that are being passed between client and server. We use the Gomoku_CLIENT pre-compile directive to make `server_repsonses` only executable on the client side (through the function `Process()`) .
    - **/serialization** contains base classes for serializing `game_state`, `client_request` and `server_response` objects. **Serialization** is the process of transforming an object instance into a string that can be sent over a network, where the receiver deserializes it, i.e. recreates the object from the string. If you are interested, [read me on Wikipedia](https://en.wikipedia.org/wiki/Serialization).
- **/server** contains only code that is relevant for the server (e.g. player management, game instance management, receiving messages)

The **/asset** folder stores all the images that are being used to render the GUI.

The **/unit-tests** folder contains all unit tests, which validate the correct behaviour of the functions written in the source code of the game. 


## Dependencies

The Gomoku game is a C++-based project and it uses [wxWidgets](https://www.wxwidgets.org/) for the GUI, [sockpp](https://github.com/fpagliughi/sockpp) for the network interface, [rapidjson](https://rapidjson.org/md_doc_tutorial.html) for object serialization, and [googletest](https://github.com/google/googletest) for the unit tests. 
