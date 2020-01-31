//
//  fuzz.cpp
//  Arithmetic Calculator and Optimizer
//
//  Created by Xuefeng Xu on 1/30/20.
//  Copyright © 2020 Xuefeng Xu. All rights reserved.
//

#include <iostream>
#include <string>
#include <cstdlib>

#include "exec.hpp"
#include "fuzz.hpp"
#include "rand_expr.hpp"

const int RANDOM_ITERS = 100;

static void check_success(ExecResult &r);

void fuzz() {
    // Test interpretor
    const char * const interp_command[] = { "/Users/xuefeng/Library/Developer/Xcode/DerivedData/Interpreter-gwepesszcvoefddlgmieqjhwiqmv/Build/Products/Debug/Interpreter", "1", nullptr};
    const char * const optimi_command[] = { "/Users/xuefeng/Library/Developer/Xcode/DerivedData/Interpreter-gwepesszcvoefddlgmieqjhwiqmv/Build/Products/Debug/Interpreter", "2", nullptr};
    for(int i = 0; i < RANDOM_ITERS; i++){
        std::string s = interp_result();
        ExecResult interp_result = exec_program(interp_command, s);
        ExecResult opitmi_result = exec_program(interp_command, s);
        check_success(interp_result);
        check_success(opitmi_result);
        std::cout << "Input: " << s << "\nOutput:\n" << interp_result.out << std::endl;
    }
}

static void check_success(ExecResult &result) {
    std::cerr << result.err;
    if (result.exit_code != 0)
        std::cerr << "non-zero exit: " << result.exit_code << "\n";
}

