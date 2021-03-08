//
// Created by Administrator on 2021/3/8.
//
#include "csman/command.hpp"

int main(int argc, char **argv) {
    try {
        std::vector<std::string> args(&argv[1], argv + argc);

        context *cxt = new context();

        parser ps(cxt,args);

        ps.parse();

        return 0;
    }
    catch (std::exception &e) {
        std::cout << e.what();
    }
    return 0;
}