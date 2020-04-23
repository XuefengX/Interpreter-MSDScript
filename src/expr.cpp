//
//  expr.cpp
//  Arithmetic Calculator and Optimizer
//
//  Created by Xuefeng Xu on 1/21/20.
//  Copyright © 2020 Xuefeng Xu. All rights reserved.
//

#include <iostream>
#include "expr.hpp"
#include "env.hpp"
#include "step.hpp"
#include "value.hpp"
#include "cont.hpp"
#include "catch.hpp"

NumExpr::NumExpr(int val) {
    this->val = val;
}

bool NumExpr::equals(PTR(Expr) e) {
    PTR(NumExpr) n = CAST(NumExpr)(e);
    if (n == NULL)
        return false;
    else
        return val == n->val;
}

PTR(Val) NumExpr::interp(PTR(Env) env){
    return NEW(NumVal)(val);
}

void NumExpr::step_interp(){
    Step::mode = Step::continue_mode;
    Step::val = NEW(NumVal)(val);
    Step::cont = Step::cont;
}

PTR(Expr) NumExpr::subst(std::string var, PTR(Val) new_val){
    return THIS;
}

PTR(Expr) NumExpr::optimize(){
    return THIS;
}

bool NumExpr::containsVar(){
    return false;
}

std::string NumExpr::to_string(){
    return "0";
}

EquExpr::EquExpr(PTR(Expr) lhs, PTR(Expr) rhs){
    this->lhs = lhs;
    this->rhs = rhs;
}

bool EquExpr::equals(PTR(Expr) e){
    PTR(EquExpr) ee = CAST(EquExpr)(e);
    return lhs->equals(ee->lhs) && rhs->equals(ee->rhs);
}

PTR(Val) EquExpr::interp(PTR(Env) env){
    return lhs->interp(env)->equals(rhs->interp(env)) ? NEW(BoolVal)(true) : NEW(BoolVal)(false);
}

void EquExpr::step_interp(){
    Step::mode = Step::interp_mode;
    Step::expr = lhs;
    Step::cont = NEW(RightThenCompCont)(rhs, Step::env, Step::cont);
}

PTR(Expr) EquExpr::subst(std::string var, PTR(Val) new_val){
    return NEW(EquExpr)(lhs->subst(var, new_val), rhs->subst(var, new_val));
}

PTR(Expr) EquExpr::optimize(){
    PTR(Expr) lhs_opti = lhs->optimize();
    PTR(Expr) rhs_opti = rhs->optimize();
    if(lhs_opti->containsVar() || rhs_opti->containsVar())
        return NEW(EquExpr)(lhs_opti, rhs_opti);
    else return NEW(BoolExpr)(lhs_opti->equals(rhs_opti));
}

bool EquExpr::containsVar(){
    return lhs->containsVar() || rhs->containsVar();
}

std::string EquExpr::to_string(){
    return lhs->to_string() + " == " + rhs->to_string();
}

AddExpr::AddExpr(PTR(Expr) lhs, PTR(Expr) rhs) {
    this->lhs = lhs;
    this->rhs = rhs;
}

bool AddExpr::equals(PTR(Expr) e) {
    PTR(AddExpr) a = CAST(AddExpr)(e);
    if (a == NULL)
        return false;
    else
        return (lhs->equals(a->lhs)
                && rhs->equals(a->rhs));
}

PTR(Val) AddExpr::interp(PTR(Env) env){
    return lhs->interp(env)->add_to(rhs->interp(env));
}

void AddExpr::step_interp(){
    Step::mode = Step::interp_mode;
    Step::expr = lhs;
    Step::cont = NEW(RightThenAddCont)(rhs, Step::env, Step::cont);
}


PTR(Expr) AddExpr::subst(std::string var, PTR(Val) new_val){
    return NEW(AddExpr)(lhs->subst(var, new_val), rhs->subst(var, new_val));
}

PTR(Expr) AddExpr::optimize(){
    lhs = lhs->optimize();
    rhs = rhs->optimize();
    if(!lhs->containsVar() && !rhs->containsVar()){
        PTR(Val) new_val = lhs->interp(Env::emptyenv)->add_to(rhs->interp(Env::emptyenv));
        return new_val->to_expr();
    }
    return NEW(AddExpr)(lhs, rhs);
}

bool AddExpr::containsVar(){
    return lhs->containsVar() || rhs->containsVar();
}

std::string AddExpr::to_string(){
    return lhs->to_string() + " + " + rhs->to_string();
}

MultExpr::MultExpr(PTR(Expr) lhs, PTR(Expr) rhs) {
    this->lhs = lhs;
    this->rhs = rhs;
}

bool MultExpr::equals(PTR(Expr) e) {
    PTR(MultExpr) m = CAST(MultExpr)(e);
    if (m == NULL)
        return false;
    else
        return (lhs->equals(m->lhs)
                && rhs->equals(m->rhs));
}

PTR(Val) MultExpr::interp(PTR(Env) env){
    return lhs->interp(env)->mult_with(rhs->interp(env));
}

void MultExpr::step_interp(){
    Step::mode = Step::interp_mode;
    Step::expr = lhs;
    Step::cont = NEW(RightThenMultCont)(rhs, Step::env, Step::cont);
}


PTR(Expr) MultExpr::subst(std::string var, PTR(Val) new_val){
    return NEW(MultExpr)(lhs->subst(var, new_val), rhs->subst(var, new_val));
}

PTR(Expr) MultExpr::optimize(){
    lhs = lhs->optimize();
    rhs = rhs->optimize();
    if(!lhs->containsVar() && !rhs->containsVar()){
        PTR(Val) new_val = lhs->interp(Env::emptyenv)->mult_with(rhs->interp(Env::emptyenv));
        return new_val->to_expr();
    }
    return NEW(MultExpr)(lhs, rhs);
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

bool VarExpr::equals(PTR(Expr) e){
    PTR(VarExpr) n = CAST(VarExpr)(e);
    if (n == nullptr)
        return false;
    else
        return name == n->name;
}

PTR(Val) VarExpr::interp(PTR(Env) env){
    return env->lookup(name);
}

void VarExpr::step_interp(){
    Step::mode = Step::continue_mode;
    Step::val = Step::env->lookup(name);
    Step::cont = Step::cont;
}

PTR(Expr) VarExpr::subst(std::string var, PTR(Val) new_val){
    if(name == var)
        return new_val->to_expr();
    else
        return THIS;
}

PTR(Expr) VarExpr::optimize(){
    return THIS;
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

bool BoolExpr::equals(PTR(Expr) e){
    PTR(BoolExpr) be = CAST(BoolExpr)(e);
    if(be == nullptr)
        return false;
    else
        return val == be->val;
}

PTR(Val) BoolExpr::interp(PTR(Env) env){
    return NEW(BoolVal)(val);
}

void BoolExpr::step_interp(){
    Step::mode = Step::continue_mode;
    Step::val = NEW(BoolVal)(val);
    Step::cont = Step::cont;
}

PTR(Expr) BoolExpr::subst(std::string var, PTR(Val) new_val){
    return THIS;
}

PTR(Expr) BoolExpr::optimize(){
    return THIS;
}

bool BoolExpr::containsVar(){
    return false;
}

std::string BoolExpr::to_string(){
    return val == true ? "_true" : "_false";
}


CallExpr::CallExpr(PTR(Expr) to_be_called, PTR(Expr) actual_arg){
    this->to_be_called = to_be_called;
    this->actual_arg = actual_arg;
}

bool CallExpr::equals(PTR(Expr) e){
    PTR(CallExpr) ce = CAST(CallExpr)(e);
    return to_be_called->equals(ce->to_be_called) && actual_arg->equals(ce->actual_arg);
}

PTR(Val) CallExpr::interp(PTR(Env) env){
    return to_be_called->interp(env)->call(actual_arg->interp(env));
}

void CallExpr::step_interp(){
    Step::mode = Step::interp_mode;
    Step::expr = to_be_called;
    Step::cont = NEW(ArgThenCallCont)(actual_arg, Step::env, Step::cont);
}

PTR(Expr) CallExpr::subst(std::string var, PTR(Val) new_val){
    return NEW(CallExpr)(to_be_called->subst(var, new_val), actual_arg);
}

PTR(Expr) CallExpr::optimize(){
    return NEW(CallExpr)(to_be_called->optimize(), actual_arg->optimize());
}

bool CallExpr::containsVar(){
    return actual_arg->containsVar();
}

std::string CallExpr::to_string(){
    return to_be_called->to_string() + "(" + actual_arg->to_string() + ")";
}

LetExpr::LetExpr(std::string let_var, PTR(Expr) eq_expr, PTR(Expr) in_expr){
    this->let_var = let_var;
    this->rhs = eq_expr;
    this->body = in_expr;
}

bool LetExpr::equals(PTR(Expr) e){
    PTR(LetExpr) le = CAST(LetExpr)(e);
    if(le == nullptr)
        return false;
    return let_var == le->let_var && rhs->equals(le->rhs) && body->equals(le->body);
}

PTR(Val) LetExpr::interp(PTR(Env) env){
    PTR(Val) rhs_val = rhs->interp(env);
    PTR(Env) new_env = NEW(ExtendedEnv)(let_var, rhs_val, env);
    return body->interp(new_env);
}

void LetExpr::step_interp(){
    Step::mode = Step::interp_mode;
    Step::expr = rhs;
    Step::cont = NEW(LetBodyCont)(let_var, body, Step::env, Step::cont);
}

PTR(Expr) LetExpr::subst(std::string var, PTR(Val) new_val){
    // substitute body only when the variables are not the same
    if(let_var == var)
        return NEW(LetExpr)(let_var, rhs, body);
    // always substitute the rhs
    return NEW(LetExpr)(let_var, rhs->subst(var, new_val), body->subst(var, new_val));
}

PTR(Expr) LetExpr::optimize(){
    rhs = rhs->optimize();
    body = body->optimize();
    if(!rhs->containsVar())
        return body->subst(let_var, rhs->interp(Env::emptyenv))->optimize();
    return NEW(LetExpr)(let_var, rhs, body);
}

bool LetExpr::containsVar(){
    return rhs->containsVar() || body->subst(let_var, rhs->interp(Env::emptyenv))->containsVar();
}

std::string LetExpr::to_string(){
    return "_let " + let_var + " = " + rhs->to_string() + " _in " + body->to_string();
}


IfExpr::IfExpr(PTR(Expr) test_part, PTR(Expr) then_part, PTR(Expr) else_part){
    this->test_part = test_part;
    this->then_part = then_part;
    this->else_part = else_part;
}

bool IfExpr::equals(PTR(Expr) e){
    PTR(IfExpr) ie = CAST(IfExpr)(e);
    return test_part->equals(ie->test_part) && then_part->equals(ie->then_part) && else_part->equals(ie->else_part);
}

PTR(Val) IfExpr::interp(PTR(Env) env){
    if(test_part->interp(env)->is_ture()){
        return then_part->interp(env);
    }else{
        return else_part->interp(env);
    }
}

void IfExpr::step_interp(){
    Step::mode = Step::interp_mode;
    Step::expr = test_part;
    Step::cont = NEW(IfBranchCont)(then_part, else_part, Step::env, Step::cont);
}
    
PTR(Expr) IfExpr::subst(std::string var, PTR(Val) new_val){
    return NEW(IfExpr)(test_part->subst(var, new_val), then_part->subst(var, new_val), else_part->subst(var, new_val));
}

PTR(Expr) IfExpr::optimize(){
    PTR(Expr) test_part_op = test_part->optimize();
    if(test_part_op->equals(NEW(BoolExpr)(true)))
        return then_part->optimize();
    else if(test_part_op->equals(NEW(BoolExpr)(false)))
        return else_part->optimize();
    else
        return NEW(IfExpr)(test_part_op, then_part->optimize(), else_part->optimize());
}

bool IfExpr::containsVar(){
    return test_part->containsVar() ? true : test_part->interp(Env::emptyenv)->is_ture() ? then_part->containsVar() : else_part->containsVar();
}

std::string IfExpr::to_string(){
    return "_if " + test_part->to_string() + " _then " + then_part->to_string() + " _else " + else_part->to_string();
}

FuncExpr::FuncExpr(std::string formal_arg, PTR(Expr) body){
    this->formal_arg = formal_arg;
    this->body = body;
}

bool FuncExpr::equals(PTR(Expr) e){
    PTR(FuncExpr) fe = CAST(FuncExpr)(e);
    return formal_arg == fe->formal_arg && body->equals(fe->body);
}

PTR(Val) FuncExpr::interp(PTR(Env) env){
    return NEW(FuncVal)(formal_arg, body, env);
}

void FuncExpr::step_interp(){
    Step::mode = Step::continue_mode;
    Step::val = NEW(FuncVal)(formal_arg, body, Step::env);
}

PTR(Expr) FuncExpr::subst(std::string var, PTR(Val) new_val){
    if(var == formal_arg)
        return THIS;
    else
        return NEW(FuncExpr)(formal_arg, body->subst(var, new_val));
}

PTR(Expr) FuncExpr::optimize(){
    return NEW(FuncExpr)(formal_arg, body->optimize());
}

bool FuncExpr::containsVar(){
    return body->containsVar();
}

std::string FuncExpr::to_string(){
    return "_fun (" + formal_arg + ") " + body->to_string();
}


TEST_CASE( "equals" ) {
    CHECK( (NEW(NumExpr)(1))->equals(NEW(NumExpr)(1)) );
    CHECK( ! (NEW(NumExpr)(1))->equals(NEW(NumExpr)(2)) );
    CHECK( ! (NEW(NumExpr)(1))->equals(NEW(MultExpr)(NEW(NumExpr)(2), NEW(NumExpr)(4))) );
    CHECK( (NEW(VarExpr)("x"))->equals(NEW(VarExpr)("x")) );
    CHECK( ! (NEW(VarExpr)("x"))->equals(NEW(NumExpr)(5)) );
    
    CHECK( (NEW(AddExpr)(NEW(NumExpr)(8), NEW(NumExpr)(9)))
          ->equals(NEW(AddExpr)(NEW(NumExpr)(8), NEW(NumExpr)(9))) );
    CHECK( ! (NEW(AddExpr)(NEW(NumExpr)(8), NEW(NumExpr)(9)))
          ->equals(NEW(AddExpr)(NEW(NumExpr)(8), NEW(NumExpr)(10))) );
    CHECK( ! (NEW(AddExpr)(NEW(NumExpr)(8), NEW(NumExpr)(9)))
          ->equals(NEW(AddExpr)(NEW(NumExpr)(10), NEW(NumExpr)(9))) );
    CHECK( ! (NEW(AddExpr)(NEW(NumExpr)(8), NEW(NumExpr)(9)))
          ->equals(NEW(NumExpr)(8)) );
    
}

TEST_CASE( "value") {
    CHECK( (NEW(NumExpr)(10))->interp(Env::emptyenv)->equals(NEW(NumVal)(10)) );
    CHECK( (NEW(AddExpr)(NEW(NumExpr)(3), NEW(NumExpr)(2)))->interp(Env::emptyenv)
          ->equals(NEW(NumVal)(5)) );
    CHECK( (NEW(MultExpr)(NEW(NumExpr)(3), NEW(NumExpr)(2)))->interp(Env::emptyenv)
          ->equals(NEW(NumVal)(6)) );
}

TEST_CASE( "subst") {
    CHECK( (NEW(NumExpr)(10))->subst("dog", NEW(NumVal)(3))
          ->equals(NEW(NumExpr)(10)) );
    CHECK( (NEW(VarExpr)("fish"))->subst("dog", NEW(NumVal)(3))
          ->equals(NEW(VarExpr)("fish")) );
    CHECK( (NEW(VarExpr)("dog"))->subst("dog", NEW(NumVal)(3) )
          ->equals(NEW(NumExpr)(3) ) );
    CHECK( (NEW(AddExpr)(NEW(NumExpr)(2), NEW(VarExpr)("dog")))->subst("dog", NEW(NumVal)(3))
          ->equals(NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3))) );
    CHECK( (NEW(MultExpr)(NEW(NumExpr)(2), NEW(VarExpr)("dog")))->subst("dog", NEW(NumVal)(3))
          ->equals(NEW(MultExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3))) );
}

