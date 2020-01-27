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
#include "value.hpp"

class Expr {
public:
    virtual bool equals(Expr *e) = 0;
    // Compute the value of an expression
    virtual Val *value() = 0;
    // Substitute a number in place of a variable
    virtual Expr *subst(std::string var, Val* new_val) = 0;
    // To String
    virtual std::string to_string() = 0;
};

class NumExpr : public Expr {
public:
    int val;
    
    NumExpr(int val);
    bool equals(Expr *e);
    Val *value();
    Expr *subst(std::string var, Val* new_val);
    std::string to_string();
};

class AddExpr : public Expr {
public:
    Expr *lhs;
    Expr *rhs;
    
    AddExpr(Expr *lhs, Expr *rhs);
    bool equals(Expr *e);
    Val *value();
    Expr *subst(std::string var, Val* new_val);
    std::string to_string();
};

class MultExpr : public Expr {
public:
    Expr *lhs;
    Expr *rhs;
    
    MultExpr(Expr *lhs, Expr *rhs);
    bool equals(Expr *e);
    Val *value();
    Expr *subst(std::string var, Val* new_val);
    std::string to_string();
};

class VarExpr : public Expr{
public:
    std::string name;
    
    VarExpr(std::string name);
    bool equals(Expr *e);
    Val *value();
    Expr *subst(std::string var, Val* new_val);
    std::string to_string();
};

class BoolExpr : public Expr{
public:
    bool val;
    
    BoolExpr(bool val);
    bool equals(Expr *e);
    Val *value();
    Expr *subst(std::string var, Val* new_val);
    std::string to_string();
};

class LetExpr : public Expr{
public:
    Expr *let_var;
    Expr *eq_expr;
    Expr *in_expr;
    
    LetExpr(Expr *let_var, Expr *eq_expr, Expr *in_expr);
    bool equals(Expr *e);
    Val *value();
    Expr *subst(std::string var, Val* new_val);
    std::string to_string();
};

#endif /* expr_h */
