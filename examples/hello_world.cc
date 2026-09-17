#include <iostream>
#include <string>

#include "hello_world/hello_world.hpp"

// Greets the name passed on the command line, or the whole world by default.
int main(int argc, char** argv) {
    if(argc > 1)
        std::cout << hello_world::Greet(std::string(argv[1])) << std::endl;
    else
        std::cout << hello_world::HelloWorld() << std::endl;

    return 0;
}
