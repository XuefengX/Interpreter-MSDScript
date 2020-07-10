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
#include "gcable.hpp"

class Expr; // Forward Declaration
class Env;
class Cont;

class Val ENABLE_THIS(Val) : public GCable{
public:
    virtual bool equals(PTR(Val) val) = 0;
    virtual PTR(Expr) to_expr() = 0;
    virtual std::string to_string() = 0;
    
    PTR(Val) add_to(PTR(Val) other_val);
    PTR(Val) mult_with(PTR(Val) other_val);
    
    bool is_true();
    
    PTR(Val) call(PTR(Val) actual_arg);
    void call_step(PTR(Val) actual_arg_val, PTR(Cont) rest);
private:
    [[noreturn]] PTR(Val) not_a(std::string what);
protected:
    [[noreturn]] PTR(Val) val_not_a(PTR(Val) val, std::string what);
};

class NumVal : public Val {
public:
    int rep;
    NumVal(int rep);
    
    bool equals(PTR(Val) val);
    PTR(Expr) to_expr();
    std::string to_string();
    
    PTR(Val) add_to(PTR(Val) other_val);
    PTR(Val) mult_with(PTR(Val) other_val);

    int size();
    void trace();
private:
    int other_rep(PTR(Val) other_val);
};

class BoolVal : public Val {
public:
    bool rep;
    BoolVal(bool rep);
    
    bool equals(PTR(Val) val);
    PTR(Expr) to_expr();
    std::string to_string();
    
    bool is_true() ;
    
    int size();
    void trace();
};

class FunVal : public Val {
public:
    std::string formal_arg;
    PTR(Expr) body;
    PTR(Env) env;
    FunVal(std::string formal_arg, PTR(Expr) body, PTR(Env) env);
    
    bool equals(PTR(Val) val);
    PTR(Expr) to_expr();
    std::string to_string();
    
    PTR(Val) call(PTR(Val) actual_arg);
    void call_step(PTR(Val) actual_arg_val, PTR(Cont) rest);
    
    int size();
    void trace();
};

#endif /* value_hpp */
