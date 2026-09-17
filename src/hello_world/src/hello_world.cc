#include "hello_world/hello_world.hpp"

#include <string>

namespace hello_world {

namespace {

const std::string DEFAULT_NAME = "World";

} // namespace

std::string
Greet(
    const std::string& name) {
    if(name.empty())
        return Greet(DEFAULT_NAME);

    return "Hello, " + name + "!";
}

std::string
HelloWorld() {
    return Greet(DEFAULT_NAME);
}

} // namespace hello_world
