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

EquExpr::EquExpr(Expr *lhs, Expr *rhs){
    this->lhs = lhs;
    this->rhs = rhs;
}

bool EquExpr::equals(Expr *e){
    EquExpr *ee = dynamic_cast<EquExpr *>(e);
    return lhs->equals(ee->lhs) && rhs->equals(ee->rhs);
}

Val *EquExpr::interp(){
    return lhs->equals(rhs) ? new BoolVal(true) : new BoolVal(false);
}

Expr *EquExpr::subst(std::string var, Val* new_val){
    return new EquExpr(lhs->subst(var, new_val), rhs->subst(var, new_val));
}

Expr *EquExpr::optimize(){
    Expr *lhs_opti = lhs->optimize();
    Expr *rhs_opti = rhs->optimize();
    return lhs_opti->containsVar() || rhs_opti->containsVar() ? new EquExpr(lhs_opti, rhs_opti) : interp()->to_expr();
}

bool EquExpr::containsVar(){
    return lhs->containsVar() || rhs->containsVar();
}

std::string EquExpr::to_string(){
    return lhs->to_string() + " == " + rhs->to_string();
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


CallExpr::CallExpr(Expr *to_be_called, Expr *actual_arg){
    this->to_be_called = to_be_called;
    this->actual_arg = actual_arg;
}

bool CallExpr::equals(Expr *e){
    CallExpr *ce = dynamic_cast<CallExpr *>(e);
    return to_be_called->equals(ce->to_be_called) && actual_arg->equals(ce->actual_arg);
}

Val *CallExpr::interp(){
    return to_be_called->interp()->call(actual_arg->interp());
}

Expr *CallExpr::subst(std::string var, Val* new_val){
    return new CallExpr(to_be_called->subst(var, new_val), actual_arg);
}

Expr *CallExpr::optimize(){
    return new CallExpr(to_be_called->optimize(), actual_arg->optimize());
}

bool CallExpr::containsVar(){
    return actual_arg->containsVar();
}

std::string CallExpr::to_string(){
    return to_be_called->to_string() + "(" + actual_arg->to_string() + ")";
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
    return let_var == le->let_var && rhs->equals(le->rhs) && body->equals(le->body);
}

Val *LetExpr::interp(){
    Val *rhs_val = rhs->interp();
    Expr *new_body = body->subst(let_var, rhs_val);
    return new_body->interp();
}

Expr *LetExpr::subst(std::string var, Val* new_val){
    // substitute body only when the variables are not the same
    if(let_var == var)
        return new LetExpr(let_var, rhs, body);
    // always substitute the rhs
    return new LetExpr(let_var, rhs->subst(var, new_val), body->subst(var, new_val));
}

Expr *LetExpr::optimize(){
    if(rhs->containsVar()){
        return new LetExpr(let_var, rhs->optimize(), body->optimize());
    }
    // If the expression after equal sign contains no variable
    // We can eliminate the let expression
    Val *rhs_val = rhs->interp();
    Expr *new_body = body->subst(let_var, rhs_val);
    return new_body->optimize();
}

bool LetExpr::containsVar(){
    return rhs->containsVar() || body->subst(let_var, rhs->interp())->containsVar();
}

std::string LetExpr::to_string(){
    return "_let " + let_var + " = " + rhs->to_string() + " _in " + body->to_string();
}


IfExpr::IfExpr(Expr *test_part, Expr *then_part, Expr *else_part){
    this->test_part = test_part;
    this->then_part = then_part;
    this->else_part = else_part;
}

bool IfExpr::equals(Expr *e){
    IfExpr *ie = dynamic_cast<IfExpr *>(e);
    return test_part->equals(ie->test_part) && then_part->equals(ie->then_part) && else_part->equals(ie->else_part);
}

Val *IfExpr::interp(){
    if(test_part->interp()->is_ture()){
        return then_part->interp();
    }else{
        return else_part->interp();
    }
}
    
Expr *IfExpr::subst(std::string var, Val* new_val){
    return new IfExpr(test_part->subst(var, new_val), then_part->subst(var, new_val), else_part->subst(var, new_val));
}

Expr *IfExpr::optimize(){
    if(test_part->containsVar())
        return this;
    else{
        if(test_part->interp()->is_ture())
            return then_part->optimize();
        else
            return else_part->optimize();
    }
}

bool IfExpr::containsVar(){
    return test_part->containsVar() ? true : test_part->interp()->is_ture() ? then_part->containsVar() : else_part->containsVar();
}

std::string IfExpr::to_string(){
    return "_if " + test_part->to_string() + " _then " + then_part->to_string() + " _else " + else_part->to_string();
}

FuncExpr::FuncExpr(std::string formal_arg, Expr *body){
    this->formal_arg = formal_arg;
    this->body = body;
}

bool FuncExpr::equals(Expr *e){
    FuncExpr *fe = dynamic_cast<FuncExpr *>(e);
    return formal_arg == fe->formal_arg && body->equals(fe->body);
}

Val *FuncExpr::interp(){
    return new FuncVal(formal_arg, body);
}

Expr *FuncExpr::subst(std::string var, Val* new_val){
    if(var == formal_arg)
        return this;
    else
        return new FuncExpr(formal_arg, body->subst(var, new_val));
}

Expr *FuncExpr::optimize(){
    return new FuncExpr(formal_arg, body->optimize());
}

bool FuncExpr::containsVar(){
    return body->containsVar();
}

std::string FuncExpr::to_string(){
    return "_fun (" + formal_arg + ") " + body->to_string();
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
    CHECK_THROWS_WITH((new VarExpr("fish"))->interp(), "cannot interpret a variable");
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

