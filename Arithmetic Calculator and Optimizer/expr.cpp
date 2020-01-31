//
//  expr.cpp
//  Arithmetic Calculator and Optimizer
//
//  Created by Xuefeng Xu on 1/21/20.
//  Copyright © 2020 Xuefeng Xu. All rights reserved.
//

#include <iostream>
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

Val *NumExpr::interp(){
    return new NumVal(val);
}

Expr *NumExpr::subst(std::string var, Val* new_val){
    return this;
}

Expr *NumExpr::optimize(){
    return this;
}

bool NumExpr::containsVar(){
    return false;
}

std::string NumExpr::to_string(){
    return interp()->to_string();
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

Val *AddExpr::interp(){
    return lhs->interp()->add_to(rhs->interp());
}

Expr *AddExpr::subst(std::string var, Val* new_val){
    return new AddExpr(lhs->subst(var, new_val), rhs->subst(var, new_val));
}

Expr *AddExpr::optimize(){
    lhs = lhs->optimize();
    rhs = rhs->optimize();
    return containsVar() ? this : interp()->to_expr();
}

bool AddExpr::containsVar(){
    return lhs->containsVar() || rhs->containsVar();
}

std::string AddExpr::to_string(){
    return lhs->to_string() + " + " + rhs->to_string();
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

Val *MultExpr::interp(){
    return lhs->interp()->mult_with(rhs->interp());
}

Expr *MultExpr::subst(std::string var, Val* new_val){
    return new MultExpr(lhs->subst(var, new_val), rhs->subst(var, new_val));
}

Expr *MultExpr::optimize(){
    lhs = lhs->optimize();
    rhs = rhs->optimize();
    return containsVar() ? this : interp()->to_expr();
}

bool MultExpr::containsVar(){
    return lhs->containsVar() || rhs->containsVar();
}

std::string MultExpr::to_string(){
    return lhs->to_string() + " * " + rhs->to_string();
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

Val *VarExpr::interp(){
    throw std::runtime_error((std::string)"cannot interpret a variable");
}

Expr *VarExpr::subst(std::string var, Val* new_val){
    if(name == var)
        return new_val->to_expr();
    else
        return this;
}

Expr *VarExpr::optimize(){
    return this;
}

bool VarExpr::containsVar(){
    return true;
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

Val *BoolExpr::interp(){
    return new BoolVal(val);
}

Expr *BoolExpr::subst(std::string var, Val* new_val){
    return this;
}

Expr *BoolExpr::optimize(){
    return this;
}

bool BoolExpr::containsVar(){
    return false;
}

std::string BoolExpr::to_string(){
    return interp()->to_string();
}

LetExpr::LetExpr(std::string let_var, Expr *eq_expr, Expr *in_expr){
    this->let_var = let_var;
    this->rhs = eq_expr;
    this->body = in_expr;
}

bool LetExpr::equals(Expr *e){
    LetExpr *le = dynamic_cast<LetExpr*>(e);
    if(le == nullptr)
        return false;
    return let_var == le->let_var && rhs->equals(le->rhs) && body->equals(le->rhs);
}

Val *LetExpr::interp(){
    Val *rhs_val = rhs->interp();
    Expr *new_body = body->subst(let_var, rhs_val);
    return new_body->interp();
}

Expr *LetExpr::subst(std::string var, Val* new_val){
    // substitute body only when the variables are not the same
    if(let_var != var){
        body = body->subst(var, new_val);
    }
    // always substitute the rhs
    rhs = rhs->subst(var, new_val);
    return this;
}

Expr *LetExpr::optimize(){
    if(rhs->containsVar()){
        rhs = rhs->optimize();
        body = body->optimize();
        std::cout << "old body: " << body->to_string() << std::endl;
        std::cout << "old let: " << to_string() << std::endl;
        return this;
    }
    // If the expression after equal sign contains no variable
    // We can eliminate the let expression
    Val *rhs_val = rhs->interp();
    Expr *new_body = body->subst(let_var, rhs_val);
    std::cout << "new body: " << new_body->to_string() << std::endl;
    return new_body->optimize();
}

bool LetExpr::containsVar(){
    LetExpr *le = dynamic_cast<LetExpr *>(optimize());
    return le->body->containsVar();
}

std::string LetExpr::to_string(){
    return "_let " + let_var + " = " + rhs->to_string() + " _in " + body->to_string();
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
    CHECK( (new NumExpr(10))->interp()->equals(new NumVal(10)) );
    CHECK( (new VarExpr("fish"))->interp()->equals(new NumVal(0)) );
    CHECK( (new AddExpr(new NumExpr(3), new NumExpr(2)))->interp()
          ->equals(new NumVal(5)) );
    CHECK( (new MultExpr(new NumExpr(3), new NumExpr(2)))->interp()
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

