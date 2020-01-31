//
//  main.cpp
//  Arithmetic Calculator and Optimizer
//
//  Created by Xuefeng Xu on 1/21/20.
//  Copyright © 2020 Xuefeng Xu. All rights reserved.
//

#include <iostream>
#include "parse.hpp"

#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

int main(int argc, char **argv)
{
    std::cout << "Choose function:\n  1. Parser\n  2. Optimizer" << std::endl;
    char c = std::cin.get();
    if(c == '1'){
        Expr *e = parse(std::cin);
        std::cout << e->interp()->to_string() << "\n";
    } else if (c == '2'){
        Expr *e = parse(std::cin);
        std::cout << e->optimize()->to_string() << "\n";
    } else {
        std::cerr << "Wrong Parameter" << std::endl;
    }
    return 0;
}
