#pragma once
#include "../Systems.hpp"
#include <iostream>
#include <string>

enum class Sender {
    CLIENT,
    SERVER
};

class Mediator {
    public:
        static void notify(Sender sender, const std::string& action, const std::vector<std::string>& params, registry& reg);
};