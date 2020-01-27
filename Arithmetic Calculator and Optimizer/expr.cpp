//
//  expr.cpp
//  Arithmetic Calculator and Optimizer
//
//  Created by Xuefeng Xu on 1/21/20.
//  Copyright © 2020 Xuefeng Xu. All rights reserved.
//

#include "expr.hpp"
#include "catch.hpp"

NumExpr::NumExpr(int val) {
    this->val = val;
}

bool NumExpr::equals(Expr *e) {
    NumExpr *n = dynamic_cast<NumExpr*>(e);
    if (n == NULL)
        return false;
    else
        return val == n->val;
}

Val *NumExpr::value(){
    return new NumVal(val);
}

Expr *NumExpr::subst(std::string var, Val* new_val){
    return this;
}

std::string NumExpr::to_string(){
    return this->value()->to_string();
}

AddExpr::AddExpr(Expr *lhs, Expr *rhs) {
    this->lhs = lhs;
    this->rhs = rhs;
}

bool AddExpr::equals(Expr *e) {
    AddExpr *a = dynamic_cast<AddExpr*>(e);
    if (a == NULL)
        return false;
    else
        return (lhs->equals(a->lhs)
                && rhs->equals(a->rhs));
}

Val *AddExpr::value(){
    return lhs->value()->add_to(rhs->value());
}

Expr *AddExpr::subst(std::string var, Val* new_val){
    return new AddExpr(lhs->subst(var, new_val), rhs->subst(var, new_val));
}

std::string AddExpr::to_string(){
    return lhs->value()->to_string() + " + " + rhs->value()->to_string();
}

MultExpr::MultExpr(Expr *lhs, Expr *rhs) {
    this->lhs = lhs;
    this->rhs = rhs;
}

bool MultExpr::equals(Expr *e) {
    MultExpr *m = dynamic_cast<MultExpr*>(e);
    if (m == NULL)
        return false;
    else
        return (lhs->equals(m->lhs)
                && rhs->equals(m->rhs));
}

Val *MultExpr::value(){
    return lhs->value()->mult_with(rhs->value());
}

Expr *MultExpr::subst(std::string var, Val* new_val){
    return new MultExpr(lhs->subst(var, new_val), rhs->subst(var, new_val));
}

std::string MultExpr::to_string(){
    return lhs->value()->to_string() + " * " + rhs->value()->to_string();
}

VarExpr::VarExpr(std::string name){
    this->name = name;
}

bool VarExpr::equals(Expr *e){
    VarExpr *n = dynamic_cast<VarExpr*>(e);
    if (n == nullptr)
        return false;
    else
        return name == n->name;
}

Val *VarExpr::value(){
    return new NumVal(0);
}

Expr *VarExpr::subst(std::string var, Val* new_val){
    if(name == var)
        return new_val->to_expr();
    else
        return this;
}

std::string VarExpr::to_string(){
    return name;
}

BoolExpr::BoolExpr(bool val){
    this->val = val;
}

bool BoolExpr::equals(Expr *e){
    BoolExpr *be = dynamic_cast<BoolExpr*>(e);
    if(be == nullptr)
        return false;
    else
        return val == be->val;
}

Val *BoolExpr::value(){
    return new BoolVal(val);
}

Expr *BoolExpr::subst(std::string var, Val* new_val){
    return this;
}

std::string BoolExpr::to_string(){
    return val ? "true" : "false";
}

LetExpr::LetExpr(Expr *let_var, Expr *eq_expr, Expr *in_expr){
    this->let_var = let_var;
    this->eq_expr = eq_expr;
    this->in_expr = in_expr;
}

bool LetExpr::equals(Expr *e){
    LetExpr *le = dynamic_cast<LetExpr*>(e);
    if(le == nullptr)
        return false;
    return let_var->equals(le->let_var) && eq_expr->equals(le->eq_expr) && in_expr->equals(le->eq_expr);
}

Val *LetExpr::value(){
    return in_expr->value();
}

Expr *LetExpr::subst(std::string var, Val* new_val){
    VarExpr *ve = dynamic_cast<VarExpr*>(let_var);
    return in_expr->subst(ve->name, eq_expr->value());
}

std::string LetExpr::to_string(){
    VarExpr *ve = dynamic_cast<VarExpr*>(let_var);
    return "_let " + ve->name + " = " + eq_expr->to_string() + " _in " + in_expr->to_string();
}


TEST_CASE( "equals" ) {
    CHECK( (new NumExpr(1))->equals(new NumExpr(1)) );
    CHECK( ! (new NumExpr(1))->equals(new NumExpr(2)) );
    CHECK( ! (new NumExpr(1))->equals(new MultExpr(new NumExpr(2), new NumExpr(4))) );
    CHECK( (new VarExpr("x"))->equals(new VarExpr("x")) );
    CHECK( ! (new VarExpr("x"))->equals(new NumExpr(5)) );
    
    CHECK( (new AddExpr(new NumExpr(8), new NumExpr(9)))
          ->equals(new AddExpr(new NumExpr(8), new NumExpr(9))) );
    CHECK( ! (new AddExpr(new NumExpr(8), new NumExpr(9)))
          ->equals(new AddExpr(new NumExpr(8), new NumExpr(10))) );
    CHECK( ! (new AddExpr(new NumExpr(8), new NumExpr(9)))
          ->equals(new AddExpr(new NumExpr(10), new NumExpr(9))) );
    CHECK( ! (new AddExpr(new NumExpr(8), new NumExpr(9)))
          ->equals(new NumExpr(8)) );
    
}

TEST_CASE( "value") {
    CHECK( (new NumExpr(10))->value()->equals(new NumVal(10)) );
    CHECK( (new VarExpr("fish"))->value()->equals(new NumVal(0)) );
    CHECK( (new AddExpr(new NumExpr(3), new NumExpr(2)))->value()
          ->equals(new NumVal(5)) );
    CHECK( (new MultExpr(new NumExpr(3), new NumExpr(2)))->value()
          ->equals(new NumVal(6)) );
}

TEST_CASE( "subst") {
    CHECK( (new NumExpr(10))->subst("dog", new NumVal(3))
          ->equals(new NumExpr(10)) );
    CHECK( (new VarExpr("fish"))->subst("dog", new NumVal(3))
          ->equals(new VarExpr("fish")) );
    CHECK( (new VarExpr("dog"))->subst("dog", new NumVal(3) )
          ->equals(new NumExpr(3) ) );
    CHECK( (new AddExpr(new NumExpr(2), new VarExpr("dog")))->subst("dog", new NumVal(3))
          ->equals(new AddExpr(new NumExpr(2), new NumExpr(3))) );
    CHECK( (new MultExpr(new NumExpr(2), new VarExpr("dog")))->subst("dog", new NumVal(3))
          ->equals(new MultExpr(new NumExpr(2), new NumExpr(3))) );
}

