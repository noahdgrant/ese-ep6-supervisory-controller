#include <fstream>
#include <iostream>
#include <string>

#include "environment.hpp"

void LoadEnvironmentVariables(const std::string &filename) {
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line)) {
        size_t pos = line.find('=');
        if (pos == std::string::npos)
            continue;

        std::string name = line.substr(0, pos);
        std::string value = line.substr(pos + 1);

        if (setenv(name.c_str(), value.c_str(), 1) != 0) {
            std::cerr << "Failed to set environment variable " << name << std::endl;
        }
    }
}
