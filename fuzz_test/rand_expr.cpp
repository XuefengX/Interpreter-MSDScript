//
//  rand_expr.cpp
//  Interpreter
//
//  Created by Xuefeng Xu on 1/30/20.
//  Copyright © 2020 Xuefeng Xu. All rights reserved.
//

#include "rand_expr.hpp"
#include "../Interpreter/catch.hpp"

static std::string random_word();
static Expr *rand_expr();

std::string interp_result(){
    return rand_expr()->to_string();
}

std::string optimi_result(){
    return rand_expr()->to_string();
}

static Expr *rand_expr(){
    Expr *e = new NumExpr(rand() % 73);
    int rand_num = rand() % 8 + 1;
    if(rand_num == 1)
        e = new AddExpr(rand_expr(), rand_expr());
    else if(rand_num == 2)
        e = new MultExpr(rand_expr(), rand_expr());
    else if(rand_num == 3)
        e = new VarExpr(random_word());
    else if(rand_num == 4)
        e = new LetExpr(random_word(), rand_expr(), rand_expr());
    return e;
}

static std::string random_word() {
    std::string word = "x";
    for (int i = rand() % 2; i-- > 0; ) {
        word += 'a' + rand() % 26;
    }
    return word;
}


