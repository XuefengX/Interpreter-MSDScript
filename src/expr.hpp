//
//  expr.hpp
//  Arithmetic_Parser
//
//  Created by Xuefeng Xu on 1/15/20.
//  Copyright © 2020 Xuefeng Xu. All rights reserved.
//

#ifndef expr_h
#define expr_h

#include <string>
#include "pointer.hpp"

class Val;
class Env;

class Expr ENABLE_THIS(Expr){
public:
    virtual bool equals(PTR(Expr) e) = 0;
    // Compute the value of an expression
    virtual PTR(Val) interp(PTR(Env) env) = 0;
    // step for continuation
    virtual void step_interp() = 0;
    // Substitute a number in place of a variable
    virtual PTR(Expr) subst(std::string var, PTR(Val) new_val) = 0;
    // Optimize the code to make it easy to deal with
    virtual PTR(Expr) optimize() = 0;
    // Return if the current expresssion contians variable
    virtual bool containsVar() = 0;
    // Get expression as a string format
    virtual std::string to_string() = 0;
};

class NumExpr : public Expr {
public:
    int val; // int rep;
    // PTR(Val) val; // allocate only once instead of everytime in interpret
    
    NumExpr(int val);
    bool equals(PTR(Expr) e);
    PTR(Val) interp(PTR(Env) env);
    void step_interp();
    PTR(Expr) subst(std::string var, PTR(Val) new_val);
    PTR(Expr) optimize();
    bool containsVar();
    std::string to_string();
};

class EquExpr : public Expr{
public:
    PTR(Expr) lhs;
    PTR(Expr) rhs;
    
    EquExpr(PTR(Expr) lhs, PTR(Expr) rhs);
    bool equals(PTR(Expr) e);
    PTR(Val) interp(PTR(Env) env);
    void step_interp();
    PTR(Expr) subst(std::string var, PTR(Val) new_val);
    PTR(Expr) optimize();
    bool containsVar();
    std::string to_string();
};

class AddExpr : public Expr {
public:
    PTR(Expr) lhs;
    PTR(Expr) rhs;
    
    AddExpr(PTR(Expr) lhs, PTR(Expr) rhs);
    bool equals(PTR(Expr) e);
    PTR(Val) interp(PTR(Env) env);
    void step_interp();
    PTR(Expr) subst(std::string var, PTR(Val) new_val);
    PTR(Expr) optimize();
    bool containsVar();
    std::string to_string();
};

class MultExpr : public Expr {
public:
    PTR(Expr) lhs;
    PTR(Expr) rhs;
    
    MultExpr(PTR(Expr) lhs, PTR(Expr) rhs);
    bool equals(PTR(Expr) e);
    PTR(Val) interp(PTR(Env) env);
    void step_interp();
    PTR(Expr) subst(std::string var, PTR(Val) new_val);
    PTR(Expr) optimize();
    bool containsVar();
    std::string to_string();
};

class VarExpr : public Expr{
public:
    std::string name;
    
    VarExpr(std::string name);
    bool equals(PTR(Expr) e);
    PTR(Val) interp(PTR(Env) env);
    void step_interp();
    PTR(Expr) subst(std::string var, PTR(Val) new_val);
    PTR(Expr) optimize();
    bool containsVar();
    std::string to_string();
};

class BoolExpr : public Expr{
public:
    bool val;
    
    BoolExpr(bool val);
    bool equals(PTR(Expr) e);
    PTR(Val) interp(PTR(Env) env);
    void step_interp();
    PTR(Expr) subst(std::string var, PTR(Val) new_val);
    PTR(Expr) optimize();
    bool containsVar();
    std::string to_string();
};

class CallExpr : public Expr{
public:
    PTR(Expr) to_be_called;
    PTR(Expr) actual_arg;
    
    CallExpr(PTR(Expr) to_be_called, PTR(Expr) actual_arg);
    bool equals(PTR(Expr) e);
    PTR(Val) interp(PTR(Env) env);
    void step_interp();
    PTR(Expr) subst(std::string var, PTR(Val) new_val);
    PTR(Expr) optimize();
    bool containsVar();
    std::string to_string();
};

class LetExpr : public Expr{
public:
    std::string let_var;
    PTR(Expr) rhs;
    PTR(Expr) body;
    
    LetExpr(std::string let_var, PTR(Expr) eq_expr, PTR(Expr) in_expr);
    bool equals(PTR(Expr) e);
    PTR(Val) interp(PTR(Env) env);
    void step_interp();
    PTR(Expr) subst(std::string var, PTR(Val) new_val);
    PTR(Expr) optimize();
    bool containsVar();
    std::string to_string();
};

class IfExpr : public Expr{
public:
    PTR(Expr) test_part;
    PTR(Expr) then_part;
    PTR(Expr) else_part;
    
    IfExpr(PTR(Expr) test_part, PTR(Expr) then_part, PTR(Expr) else_part);
    bool equals(PTR(Expr) e);
    PTR(Val) interp(PTR(Env) env);
    void step_interp();
    PTR(Expr) subst(std::string var, PTR(Val) new_val);
    PTR(Expr) optimize();
    bool containsVar();
    std::string to_string();
};

class FuncExpr : public Expr{
public:
    std::string formal_arg;
    PTR(Expr) body;
    
    FuncExpr(std::string formal_arg, PTR(Expr) body);
    bool equals(PTR(Expr) e);
    PTR(Val) interp(PTR(Env) env);
    void step_interp();
    PTR(Expr) subst(std::string var, PTR(Val) new_val);
    PTR(Expr) optimize();
    bool containsVar();
    std::string to_string();
};

#endif /* expr_h */
