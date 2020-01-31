//
//  parse.h
//  Arithmetic Calculator and Optimizer
//
//  Created by Xuefeng Xu on 1/21/20.
//  Copyright © 2020 Xuefeng Xu. All rights reserved.
//

#ifndef parse_h
#define parse_h

#include <iostream>
#include <string>
#include "expr.hpp"


Expr *parse(std::istream &in);

#endif /* parse_h */
