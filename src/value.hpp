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
#include "pointer.hpp"

class Expr; // Forward Declaration
class Env;
class Cont;

class Val ENABLE_THIS(Val){
public:
    virtual bool equals(PTR(Val) other_val) = 0;
    virtual PTR(Val) add_to(PTR(Val) other_val) = 0;
    virtual PTR(Val) mult_with(PTR(Val) other_val) = 0;
    virtual bool is_ture() = 0;
    virtual PTR(Expr) to_expr() = 0;
    virtual PTR(Val) call(PTR(Val) actual_arg) = 0;
    virtual void call_step(PTR(Val) actual_arg_val, PTR(Cont) rest) = 0;
    virtual std::string to_string() = 0;
};

class NumVal : public Val{
public:
    int rep;  // representation of the val: int or bool
    
    NumVal(int rep);
    bool equals(PTR(Val) other_val);
    PTR(Val) add_to(PTR(Val) other_val);
    PTR(Val) mult_with(PTR(Val) other_val);
    bool is_ture();
    PTR(Expr) to_expr();
    PTR(Val) call(PTR(Val) actual_arg);
    void call_step(PTR(Val) actual_arg_val, PTR(Cont) rest);
    std::string to_string();
};

class BoolVal : public Val{
public:
    bool rep;
    
    BoolVal(bool rep);
    bool equals(PTR(Val) other_val);
    PTR(Val) add_to(PTR(Val) other_val);
    PTR(Val) mult_with(PTR(Val) other_val);
    bool is_ture();
    PTR(Expr) to_expr();
    PTR(Val) call(PTR(Val) actual_arg);
    void call_step(PTR(Val) actual_arg_val, PTR(Cont) rest);
    std::string to_string();
};

class FuncVal : public Val{
public:
    std::string formal_arg;
    PTR(Expr) body;
    PTR(Env) env;
    
    FuncVal(std::string formal_arg, PTR(Expr) body, PTR(Env) env);
    bool equals(PTR(Val) other_val);
    PTR(Val) add_to(PTR(Val) other_val);
    PTR(Val) mult_with(PTR(Val) other_val);
    bool is_ture();
    PTR(Expr) to_expr();
    PTR(Val) call(PTR(Val) actual_arg);
    void call_step(PTR(Val) actual_arg_val, PTR(Cont) rest);
    std::string to_string();
};

#endif /* value_hpp */
