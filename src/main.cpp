//
//  main.cpp
//  Arithmetic Calculator and Optimizer
//
//  Created by Xuefeng Xu on 1/21/20.
//  Copyright © 2020 Xuefeng Xu. All rights reserved.
//

#include <iostream>
#include "parse.hpp"
#include "env.hpp"
#include "expr.hpp"
#include "step.hpp"
#include "value.hpp"
#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

int main(int argc, char **argv){
    std::cout << "MSDScript is running ... " << std::endl;
    if(argc <= 1){
        std::cout << "MSDscript Interpreter is running...\nEnter an expression: " << std::endl;
        PTR(Expr) e = parse(std::cin);
        std::cout << e->interp(Env::emptyenv)->to_string() << std::endl;
    }else{
        std::string arg = argv[1];
        if(arg == "--opt"){
            std::cout << "MSDscript Optimizer is running...\nEnter an expression: " << std::endl;
            PTR(Expr) e = parse(std::cin);
                    std::cout << e->optimize()->to_string() << "\n";
        } else if (arg == "--step") {
            std::cout << "MSDscript Interpreter is running with steps...\nEnter an expression: " << std::endl;
            std::cout << Step::interp_by_steps(parse(std::cin))->to_string() << std::endl;
        } else if (arg == "--script"){
            std::ifstream file;
            file.open(argv[2], std::ios::in);
            std::cout << "MSDscript Interpreter is running with steps..." << std::endl;
            std::cout << Step::interp_by_steps(parse(file))->to_string() << std::endl;
            file.close();
        } else {
            std::cout << "Usage: ./msdscript for interpreter\n./msdscript --opt for optimizer" << std::endl;
            return 2;
        }
    }
    return 0;
}
