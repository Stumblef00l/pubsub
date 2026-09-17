#ifndef _HELLO_WORLD_HELLO_WORLD_HPP_
#define _HELLO_WORLD_HELLO_WORLD_HPP_

#include <string>

namespace hello_world {

// Builds a greeting for the given name. An empty name is greeted as "World".
std::string
Greet(
    const std::string& name);

// The canonical "Hello, World!" greeting.
std::string
HelloWorld();

} // namespace hello_world

#endif
