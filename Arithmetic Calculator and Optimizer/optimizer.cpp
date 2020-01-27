//
//  optimizer.cpp
//  Arithmetic Calculator and Optimizer
//
//  Created by Xuefeng Xu on 1/22/20.
//  Copyright © 2020 Xuefeng Xu. All rights reserved.
//

#include "optimizer.hpp"
#include "parse.hpp"

Expr *optimizer(std::istream &in){
    return parse(in);
}
