//
//  value.hpp
//  Arithmetic Calculator and Optimizer
//
//  Created by Xuefeng Xu on 1/21/20.
//  Copyright © 2020 Xuefeng Xu. All rights reserved.
//

#ifndef value_hpp
#define value_hpp

#include <string>

class Expr; // Forward Declaration

class Val{
public:
    virtual bool equals(Val *other_val) = 0;
    virtual Val *add_to(Val *other_val) = 0;
    virtual Val *mult_with(Val *other_val) = 0;
    virtual Expr *to_expr() = 0;
    virtual std::string to_string() = 0;
};

class NumVal : public Val{
public:
    int rep;  // representation of the val: int or bool
    
    NumVal(int rep);
    bool equals(Val *other_val);
    Val *add_to(Val *other_val);
    Val *mult_with(Val *other_val);
    Expr *to_expr();
    std::string to_string();
};

class BoolVal : public Val{
public:
    bool rep;
    
    BoolVal(bool rep);
    bool equals(Val *other_val);
    Val *add_to(Val *other_val);
    Val *mult_with(Val *other_val);
    Expr *to_expr();
    std::string to_string();
};


#endif /* value_hpp */
