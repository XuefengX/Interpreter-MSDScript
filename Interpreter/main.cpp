//
//  main.cpp
//  Arithmetic Calculator and Optimizer
//
//  Created by Xuefeng Xu on 1/21/20.
//  Copyright © 2020 Xuefeng Xu. All rights reserved.
//

#include <iostream>
#include "parse.hpp"
#include "../fuzz_test/fuzz.hpp"
#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

int main(int argc, char **argv){
    if(argc <= 1)
        std::cout << "Please choose a program to run:\n" << " 1 -> Interpretor\n 2 -> Optimizer\n 3 -> Fuzz Test" << std::endl;
    char c = argc > 1 ? argv[1][0] : std::cin.get();
    if(c == '1'){
        std::cout << "Interpretor is running\n" << "Enter an expression:" << std::endl;
        Expr *e = parse(std::cin);
        std::cout << e->interp()->to_string() << "\n";
    } else if (c == '2'){
        std::cout << "Opitmizer is running\n" << "Enter an expression:" << std::endl;
        Expr *e = parse(std::cin);
        std::cout << e->optimize()->to_string() << "\n";
    } else if (c == '3'){
        std::cout << "Fuzz Test is running..." << std::endl;
        fuzz();
    } else {
        std::cerr << "Wrong Parameter" << std::endl;
    }
    return 0;
}
