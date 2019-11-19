# kshare
A keyboard and mouse sharing program for Linux utilizing the /dev/input interface and X11 with AES-256 encryption.

## Building and running
Run the `build.sh` script to build the project. The resulting executables will be placed in the `build` directory. 
You will need at least g++ 8 and CMake.

Then set up your server and client configurations, examples can be found in the `example` directory.
Due to relying on /dev/input, both the client and server programs need to be run as root.

## Third party libraries
- [ENet](https://github.com/lsalzman/enet) - copyright (c) 2002-2016 Lee Salzman
- [SHA256](https://github.com/B-Con/crypto-algorithms) - by Brad Conte
- [Tiny AES](https://github.com/kokke) - by  kokke