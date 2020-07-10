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

NumExpr::NumExpr(int rep) {
    this->rep = rep;
}

bool NumExpr::equals(PTR(Expr) e) {
    PTR(NumExpr) n = CAST(NumExpr)(e);
    if (n == NULL)
        return false;
    else
        return rep == n->rep;
}

PTR(Val) NumExpr::interp(PTR(Env) env) {
    return NEW(NumVal)(rep);
}

void NumExpr::step_interp() {
    Step::mode = Step::continue_mode;
    Step::val = NEW(NumVal)(rep);
    Step::cont = Step::cont; /* no-op */
}

PTR(Expr) NumExpr::subst(std::string var, PTR(Val) new_val) {
    return NEW(NumExpr)(rep);
}

bool NumExpr::haveVariable() {
    return false;
}

PTR(Expr) NumExpr::optimize() {
    return THIS;
}

std::string NumExpr::to_string() {
    return std::to_string(rep);
}

int NumExpr::size() {
    return sizeof(NumExpr);
}

void NumExpr::trace() {
    
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

PTR(Val) AddExpr::interp(PTR(Env) env) {
    return lhs->interp(env)->add_to(rhs->interp(env));
}

void AddExpr::step_interp() {
    Step::mode = Step::interp_mode;
    Step::expr = lhs;
    Step::cont = NEW(RightThenAddCont)(rhs, Step::env, Step::cont);
}


PTR(Expr) AddExpr::subst(std::string var, PTR(Val) new_val) {
    return NEW(AddExpr)(lhs->subst(var, new_val),
                        rhs->subst(var, new_val));
}

bool AddExpr::haveVariable() {
    return lhs->haveVariable() || rhs->haveVariable();
}

PTR(Expr) AddExpr::optimize() {
    PTR(Expr) lhs_optimized = lhs->optimize();
    PTR(Expr) rhs_optimized = rhs->optimize();
    
    if (!lhs_optimized->haveVariable() && !rhs_optimized->haveVariable()) {
        PTR(Val) new_val = lhs_optimized->interp(Env::emptyenv)->add_to(rhs_optimized->interp(Env::emptyenv));
        return new_val->to_expr();
    }
    return NEW(AddExpr)(lhs_optimized, rhs_optimized);
}

std::string AddExpr::to_string() {
    return "(" + lhs->to_string() + " + " + rhs->to_string() + ")";
}

int AddExpr::size() {
    return sizeof(AddExpr);
}

void AddExpr::trace() {
    UPDATE(lhs);
    UPDATE(rhs);
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

PTR(Val) MultExpr::interp(PTR(Env) env) {
    return lhs->interp(env)->mult_with(rhs->interp(env));
}

void MultExpr::step_interp() {
    Step::mode = Step::interp_mode;
    Step::expr = lhs;
    Step::cont = NEW(RightThenMultCont)(rhs, Step::env, Step::cont);
}

PTR(Expr) MultExpr::subst(std::string var, PTR(Val) new_val)
{
    return NEW(MultExpr)(lhs->subst(var, new_val), rhs->subst(var, new_val));
}

bool MultExpr::haveVariable() {
    return lhs->haveVariable() || rhs->haveVariable();
}

PTR(Expr) MultExpr::optimize() {
    PTR(Expr) lhs_optimized = this->lhs->optimize();
    PTR(Expr) rhs_optimized = this->rhs->optimize();
    
    if (!lhs_optimized->haveVariable() && !rhs_optimized->haveVariable()) {
        PTR(Val) new_val = lhs_optimized->interp(Env::emptyenv)->mult_with(rhs_optimized->interp(Env::emptyenv));
        return new_val->to_expr();
    }
    return NEW(MultExpr)(lhs_optimized, rhs_optimized);
}

std::string MultExpr::to_string() {
    return "(" + lhs->to_string() + " * " + rhs->to_string() + ")";
}

int MultExpr::size() {
    return sizeof(MultExpr);
}

void MultExpr::trace() {
    UPDATE(lhs);
    UPDATE(rhs);
}

VarExpr::VarExpr(std::string name) {
    this->name = name;
}

bool VarExpr::equals(PTR(Expr) e) {
    PTR(VarExpr) v = CAST(VarExpr)(e);
    if (v == NULL)
        return false;
    else
        return name == v->name;
}

PTR(Val) VarExpr::interp(PTR(Env) env) {
    return env->lookup(name);
}

void VarExpr::step_interp() {
    Step::mode = Step::continue_mode;
    Step::val = Step::env->lookup(name);
    Step::cont = Step::cont; /* no-op */
}

PTR(Expr) VarExpr::subst(std::string var, PTR(Val) new_val) {
    if (name == var)
        return new_val->to_expr();
    else
        return NEW(VarExpr)(name);
}

bool VarExpr::haveVariable() {
    return true;
}

PTR(Expr) VarExpr::optimize() {
    return NEW(VarExpr)(name);
}

std::string VarExpr::to_string() {
    return name;
}

int VarExpr::size() {
    return sizeof(VarExpr);
}

void VarExpr::trace() {
    
}

BoolExpr::BoolExpr(bool rep) {
    this->rep = rep;
}

bool BoolExpr::equals(PTR(Expr) e) {
    PTR(BoolExpr) b = CAST(BoolExpr)(e);
    if (b == NULL) {
        return false;
    } else {
        return rep == b->rep;
    }
}

PTR(Val) BoolExpr::interp(PTR(Env) env) {
    return NEW(BoolVal)(rep);
}

void BoolExpr::step_interp() {
    Step::mode = Step::continue_mode;
    Step::val = NEW(BoolVal)(rep);
    Step::cont = Step::cont; /* no-op */
}


PTR(Expr) BoolExpr::subst(std::string var, PTR(Val) new_val) {
    return NEW(BoolExpr)(rep);
}

bool BoolExpr::haveVariable() {
    return false;
}

PTR(Expr) BoolExpr::optimize() {
    return NEW(BoolExpr)(rep);
}

std::string BoolExpr::to_string() {
    if (rep == true)
        return "_true";
    else
        return "_false";
}

int BoolExpr::size() {
    return sizeof(BoolExpr);
}

void BoolExpr::trace() {
    
}

LetExpr::LetExpr(std::string var_name, PTR(Expr) rhs, PTR(Expr) body) {
    this->var_name = var_name;
    this->rhs = rhs;
    this->body = body;
}

bool LetExpr::equals(PTR(Expr) e) {
    PTR(LetExpr) l = CAST(LetExpr)(e);
    
    if (l == NULL)
        return false;
    else {
        return var_name == l->var_name && rhs->equals(l->rhs) && body->equals(l->body);
    }
}

PTR(Val) LetExpr::interp(PTR(Env) env) {
    PTR(Val) rhs_value = rhs->interp(env);
    PTR(Env) new_env = NEW(ExtendedEnv)(var_name, rhs_value, env);
    
    return body->interp(new_env);
}

void LetExpr::step_interp() {
    Step::mode = Step::interp_mode;
    Step::expr = rhs;
    Step::cont = NEW(LetBodyCont)(var_name, body, Step::env, Step::cont);
}

PTR(Expr) LetExpr::subst(std::string var, PTR(Val) val) {
    if (var == var_name) {
        return NEW(LetExpr)(var, rhs, body);
    }
    return NEW(LetExpr)(var_name, rhs->subst(var, val), body->subst(var, val));
}

bool LetExpr::haveVariable() {
    return rhs->haveVariable() || body->subst(var_name, rhs->interp(Env::emptyenv))->haveVariable();
}

PTR(Expr) LetExpr::optimize() {
    PTR(Expr) rhs_optimized = rhs->optimize();
    PTR(Expr) body_optimized = body->optimize();
    
    if (!rhs_optimized->haveVariable()) {
        return body_optimized->subst(var_name, rhs_optimized->interp(Env::emptyenv))->optimize();
    }
    return NEW(LetExpr)(var_name, rhs_optimized, body_optimized);
}

std::string LetExpr::to_string() {
    return "(_let " + var_name + " = " + rhs->to_string() + " _in " + body->to_string() + ")";
}

int LetExpr::size() {
    return sizeof(LetExpr);
}

void LetExpr::trace() {
    UPDATE(rhs);
    UPDATE(body);
}

IfExpr::IfExpr(PTR(Expr) if_part, PTR(Expr) then_part, PTR(Expr) else_part) {
    this->if_part = if_part;
    this->then_part = then_part;
    this->else_part = else_part;
}

bool IfExpr::equals(PTR(Expr) e) {
    PTR(IfExpr) i = CAST(IfExpr)(e);
    
    if (i == NULL)
        return false;
    else
        return if_part->equals(i->if_part)
        && then_part->equals(i->then_part)
        && else_part->equals(i->else_part);
}

PTR(Val) IfExpr::interp(PTR(Env) env) {
    PTR(Val) if_value= if_part->interp(env);
    
    if (if_value->equals(NEW(BoolVal)(true))) {
        return then_part->interp(env);
    } else {
        return else_part->interp(env);
    }
}

void IfExpr::step_interp() {
    Step::mode = Step::interp_mode;
    Step::expr = if_part;
    Step::cont = NEW(IfBranchCont)(then_part, else_part, Step::env, Step::cont);
}

PTR(Expr) IfExpr::subst(std::string var, PTR(Val) val) {
    return NEW(IfExpr)(if_part->subst(var, val), then_part->subst(var, val), else_part->subst(var, val));
}

bool IfExpr::haveVariable() {
    if (if_part->haveVariable()) {
        return true;
    }
    PTR(Val) if_value = if_part->interp(Env::emptyenv);
    
    if (if_value->equals(NEW(BoolVal)(true)))
        return then_part->haveVariable();
    else
        return else_part->haveVariable();
}

PTR(Expr) IfExpr::optimize() {
    PTR(Expr) if_part_optimized = if_part->optimize();
    
    if (if_part_optimized->equals(NEW(BoolExpr)(true))) {
        return then_part->optimize();
    } else if (if_part_optimized->equals(NEW(BoolExpr)(false))){
        return else_part->optimize();
    } else {
        return NEW(IfExpr)(if_part_optimized, then_part->optimize(), else_part->optimize());
    }
}

std::string IfExpr::to_string() {
    return "(_if " + if_part->to_string()
    + " _then " + then_part->to_string()
    + " _else " + else_part->to_string() + ")";
}

int IfExpr::size() {
    return sizeof(IfExpr);
}

void IfExpr::trace() {
    UPDATE(if_part);
    UPDATE(then_part);
    UPDATE(else_part);
}

CompExpr::CompExpr(PTR(Expr) lhs, PTR(Expr) rhs) {
    this->lhs = lhs;
    this->rhs = rhs;
}

bool CompExpr::equals(PTR(Expr) e) {
    PTR(CompExpr) ee = CAST(CompExpr)(e);
    
    if (ee == NULL)
        return false;
    else
        return lhs->equals(ee->lhs) && rhs->equals(ee->rhs);
}

// What if it's comparing two functions???
PTR(Val) CompExpr::interp(PTR(Env) env) {
    PTR(Val) lhs_value = lhs->interp(env);
    PTR(Val) rhs_value = rhs->interp(env);
    
    if (lhs_value->equals(rhs_value))
        return NEW(BoolVal)(true);
    else
        return NEW(BoolVal)(false);
}

void CompExpr::step_interp() {
    Step::mode = Step::interp_mode;
    Step::expr = lhs;
    Step::cont = NEW(RightThenCompCont)(rhs, Step::env, Step::cont);
}

PTR(Expr) CompExpr::subst(std::string var, PTR(Val) val) {
    return NEW(CompExpr)(lhs->subst(var, val), rhs->subst(var, val));
}

bool CompExpr::haveVariable() {
    return lhs->haveVariable() || rhs->haveVariable();
}

PTR(Expr) CompExpr::optimize() {
    PTR(Expr) lhs_optimized = lhs->optimize();
    PTR(Expr) rhs_optimized = rhs->optimize();
    
    if (!lhs_optimized->haveVariable() && !rhs_optimized->haveVariable())
        return NEW(BoolExpr)(lhs_optimized->equals(rhs_optimized));
    else
        return NEW(CompExpr)(lhs_optimized, rhs_optimized);
}

std::string CompExpr::to_string() {
    return "(" + lhs->to_string() + " == " + rhs->to_string() + ")";
}

int CompExpr::size() {
    return sizeof(CompExpr);
}

void CompExpr::trace() {
    UPDATE(lhs);
    UPDATE(rhs);
}

FunExpr::FunExpr(std::string formal_arg, PTR(Expr) body) {
    this->formal_arg = formal_arg;
    this->body = body;
}

bool FunExpr::equals(PTR(Expr) e) {
    PTR(FunExpr) f = CAST(FunExpr)(e);
    
    if (f == NULL)
        return false;
    else
        return formal_arg == f->formal_arg && body->equals(f->body);
}

PTR(Val) FunExpr::interp(PTR(Env) env) {
    return NEW(FunVal)(formal_arg, body, env);
}

void FunExpr::step_interp() {
    Step::mode = Step::continue_mode;
    Step::val = NEW(FunVal)(formal_arg, body, Step::env);
}

PTR(Expr) FunExpr::subst(std::string var, PTR(Val) val) {
    // If the variable to be substituted is the same as the formal_arg, don't substitute
    // e.g. (_fun(x) x + x)(2) ->substitute("x", 3)
    if (var == formal_arg)
        return NEW(FunExpr)(formal_arg, body);
    else
        return NEW(FunExpr)(formal_arg, body->subst(var, val));
}

// _fun(x) x + x doens't have a variable
// _fun(x) x + y have a variable
// So I just substitute the x with a random NumVal
// and check whether the body have a variable after the substitution
bool FunExpr::haveVariable() {
    return body->subst(formal_arg, NEW(NumVal)(0))->haveVariable();
}

PTR(Expr) FunExpr::optimize() {
    return NEW(FunExpr)(formal_arg, body->optimize());
}

std::string FunExpr::to_string() {
    return "(_fun(" + formal_arg + ") " + body->to_string() + ")";
}

int FunExpr::size() {
    return sizeof(FunExpr);
}

void FunExpr::trace() {
    UPDATE(body);
}

CallFunExpr::CallFunExpr(PTR(Expr) to_be_called, PTR(Expr) actual_arg) {
    this->to_be_called = to_be_called;
    this->actual_arg = actual_arg;
}

bool CallFunExpr::equals(PTR(Expr) e) {
    PTR(CallFunExpr) c = CAST(CallFunExpr)(e);
    
    if (c == NULL)
        return false;
    else
        return to_be_called->equals(c->to_be_called) && actual_arg->equals(c->actual_arg);
}

PTR(Val) CallFunExpr::interp(PTR(Env) env) {
    return to_be_called->interp(env)->call(actual_arg->interp(env));
}

void CallFunExpr::step_interp() {
    Step::mode = Step::interp_mode;
    Step::expr = to_be_called;
    Step::cont = NEW(ArgThenCallCont)(actual_arg, Step::env, Step::cont);
}

PTR(Expr) CallFunExpr::subst(std::string var, PTR(Val) val) {
    return NEW(CallFunExpr)(to_be_called->subst(var, val), actual_arg->subst(var, val));
}

bool CallFunExpr::haveVariable() {
    return actual_arg->haveVariable() || to_be_called->haveVariable();
}

PTR(Expr) CallFunExpr::optimize() {
    return NEW(CallFunExpr)(to_be_called->optimize(), actual_arg->optimize());
}

std::string CallFunExpr::to_string() {
    return to_be_called->to_string() + "(" + actual_arg->to_string() + ")";
}

int CallFunExpr::size() {
    return sizeof(CallFunExpr);
}

void CallFunExpr::trace() {
    UPDATE(actual_arg);
    UPDATE(to_be_called);
}

/* for tests */
static std::string evaluate_expr(PTR(Expr) expr) {
    try {
        (void)expr->interp(Env::emptyenv);
        return "";
    } catch (std::runtime_error exn) {
        return exn.what();
    }
}


TEST_CASE( "equals" ) {
    // equals method for NumExpr
    CHECK( (NEW(NumExpr)(1))->equals(NEW(NumExpr)(1)) );
    CHECK( ! (NEW(NumExpr)(1))->equals(NEW(NumExpr)(2)) );
    CHECK( ! (NEW(NumExpr)(0))->equals(NEW(AddExpr)(NEW(NumExpr)(0), NEW(NumExpr)(0))) );
    CHECK( ! (NEW(NumExpr)(1))->equals(NEW(MultExpr)(NEW(NumExpr)(2), NEW(NumExpr)(4))) );
    CHECK( ! (NEW(NumExpr)(-1))->equals(NEW(VarExpr)("x")) );
    CHECK( ! (NEW(NumExpr)(1))->equals(NEW(BoolExpr)(true)) );
    
    // equals method for AddExpr
    CHECK( (NEW(AddExpr)(NEW(NumExpr)(8), NEW(NumExpr)(9)))
          ->equals(NEW(AddExpr)(NEW(NumExpr)(8), NEW(NumExpr)(9))) );
    CHECK( ! (NEW(AddExpr)(NEW(NumExpr)(8), NEW(NumExpr)(9)))
          ->equals(NEW(AddExpr)(NEW(NumExpr)(8), NEW(NumExpr)(10))) );
    CHECK( ! (NEW(AddExpr)(NEW(NumExpr)(8), NEW(NumExpr)(9)))
          ->equals(NEW(AddExpr)(NEW(NumExpr)(10), NEW(NumExpr)(9))) );
    CHECK( ! (NEW(AddExpr)(NEW(NumExpr)(8), NEW(NumExpr)(9)))
          ->equals(NEW(NumExpr)(8)) );
    CHECK( ! (NEW(AddExpr)(NEW(NumExpr)(8), NEW(NumExpr)(9)))
          ->equals(NEW(AddExpr)(NEW(NumExpr)(9), NEW(NumExpr)(8))) );
    
    // equals method for MultExpr
    CHECK( (NEW(MultExpr)(NEW(NumExpr)(5), NEW(NumExpr)(7)))
          ->equals(NEW(MultExpr)(NEW(NumExpr)(5), NEW(NumExpr)(7))) );
    CHECK( ! (NEW(MultExpr)(NEW(NumExpr)(5), NEW(NumExpr)(7)))
          ->equals(NEW(MultExpr)(NEW(NumExpr)(5), NEW(NumExpr)(10))) );
    CHECK( ! (NEW(MultExpr)(NEW(NumExpr)(5), NEW(NumExpr)(7)))
          ->equals(NEW(MultExpr)(NEW(NumExpr)(10), NEW(NumExpr)(7))) );
    CHECK( ! (NEW(MultExpr)(NEW(NumExpr)(5), NEW(NumExpr)(7)))
          ->equals(NEW(NumExpr)(5)) );
    CHECK( ! (NEW(MultExpr)(NEW(NumExpr)(5), NEW(NumExpr)(7)))
          ->equals(NEW(MultExpr)(NEW(NumExpr)(7), NEW(NumExpr)(5))) );
    
    // equals method for VarExpr
    CHECK( (NEW(VarExpr)("x"))->equals(NEW(VarExpr)("x")) );
    CHECK( !(NEW(VarExpr)("x"))->equals(NEW(VarExpr)("y")) );
    CHECK( ! (NEW(VarExpr)("xyz"))->equals(NEW(VarExpr)("zyx")) );
    CHECK( ! (NEW(VarExpr)("z"))->equals(NEW(NumExpr)(5)) );
    
    // equals method for BoolExpr
    CHECK( (NEW(BoolExpr)(true))->equals(NEW(BoolExpr)(true)) );
    CHECK( !(NEW(BoolExpr)(true))->equals(NEW(BoolExpr)(false)) );
    CHECK( (NEW(BoolExpr)(false))->equals(NEW(BoolExpr)(false)) );
    CHECK( !(NEW(BoolExpr)(false))->equals(NEW(VarExpr)("false")) );
    CHECK( !(NEW(BoolExpr)(true))->equals(NEW(NumExpr)(1)) );
    CHECK( !(NEW(BoolExpr)(false))->equals(NEW(NumExpr)(0)) );
    
    // equals method for LetExpr
    CHECK( !(NEW(LetExpr)("x",
                          NEW(NumExpr)(1),
                          NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4))))
          ->equals(NEW(NumExpr)(5)) );
    CHECK( (NEW(LetExpr)("x",
                         NEW(NumExpr)(1),
                         NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4))))
          ->equals(NEW(LetExpr)("x",
                                NEW(NumExpr)(1),
                                NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4)))) );
    
    // equals method for IfExpr
    CHECK( !(NEW(IfExpr)(NEW(BoolExpr)(true),
                         NEW(NumExpr)(3),
                         NEW(NumExpr)(4)))
          ->equals(NEW(IfExpr)(NEW(BoolExpr)(true),
                               NEW(NumExpr)(3),
                               NEW(NumExpr)(5))) );
    
    CHECK( (NEW(IfExpr)(NEW(CompExpr)(NEW(NumExpr)(3), NEW(NumExpr)(3)),
                        NEW(BoolExpr)(true),
                        NEW(BoolExpr)(false)))
          ->equals((NEW(IfExpr)(NEW(CompExpr)(NEW(NumExpr)(3), NEW(NumExpr)(3)),
                                NEW(BoolExpr)(true),
                                NEW(BoolExpr)(false)))) );
    
    CHECK( !(NEW(IfExpr)(NEW(CompExpr)(NEW(NumExpr)(3), NEW(NumExpr)(3)),
                         NEW(BoolExpr)(true),
                         NEW(BoolExpr)(false)))
          ->equals((NEW(IfExpr)(NEW(BoolExpr)(true),
                                NEW(BoolExpr)(true),
                                NEW(BoolExpr)(false)))) );
    
    // equals method for CompExpr
    CHECK( !(NEW(CompExpr)(NEW(NumExpr)(3), NEW(NumExpr)(1)))
          ->equals(NEW(CompExpr)(NEW(NumExpr)(3), NEW(BoolExpr)(true))) );
    CHECK( (NEW(CompExpr)(NEW(NumExpr)(3), NEW(NumExpr)(4)))
          ->equals(NEW(CompExpr)(NEW(NumExpr)(3), NEW(NumExpr)(4))) );
    
    // equals method for FunExpr
    CHECK( (NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))))
          ->equals(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x")))) );
    CHECK( !(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))))
          ->equals(NEW(FunExpr)("x", NEW(MultExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x")))) );
    
    // equals method for CallFunExpr
    CHECK( (NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))),
                             NEW(NumExpr)(4)))
          ->equals(NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))),
                                    NEW(NumExpr)(4))) );
    CHECK( !(NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))),
                              NEW(NumExpr)(4)))
          ->equals(NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(MultExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))),
                                    NEW(NumExpr)(4))) );
}

TEST_CASE( "interp") {
    // interp method for NumExpr
    CHECK( (NEW(NumExpr)(10))->interp(Env::emptyenv)->equals(NEW(NumVal)(10)) );
    CHECK( !(NEW(NumExpr)(10))->interp(Env::emptyenv)->equals(NEW(NumVal)(100)) );
    
    CHECK( Step::interp_by_steps(NEW(NumExpr)(10))->equals(NEW(NumVal)(10)) );
    CHECK( !Step::interp_by_steps(NEW(NumExpr)(10))->equals(NEW(NumVal)(100)) );
    
    // interp method for AddExpr
    CHECK( (NEW(AddExpr)(NEW(NumExpr)(3), NEW(NumExpr)(2)))->interp(Env::emptyenv)
          ->equals(NEW(NumVal)(5)) );
    CHECK( !(NEW(AddExpr)(NEW(NumExpr)(0), NEW(NumExpr)(1)))->interp(Env::emptyenv)
          ->equals(NEW(BoolVal)(true)) );
    
    CHECK( Step::interp_by_steps(NEW(AddExpr)(NEW(NumExpr)(3), NEW(NumExpr)(2)))
          ->equals(NEW(NumVal)(5)) );
    CHECK( !Step::interp_by_steps(NEW(AddExpr)(NEW(NumExpr)(0), NEW(NumExpr)(1)))
          ->equals(NEW(BoolVal)(true)) );
    
    // interp method for MultExpr
    CHECK( (NEW(MultExpr)(NEW(NumExpr)(3), NEW(NumExpr)(2)))->interp(Env::emptyenv)
          ->equals(NEW(NumVal)(6)) );
    CHECK( !(NEW(MultExpr)(NEW(NumExpr)(0), NEW(NumExpr)(1)))->interp(Env::emptyenv)
          ->equals(NEW(BoolVal)(false)) );
    
    CHECK( Step::interp_by_steps(NEW(MultExpr)(NEW(NumExpr)(3), NEW(NumExpr)(2)))
          ->equals(NEW(NumVal)(6)) );
    CHECK( !Step::interp_by_steps(NEW(MultExpr)(NEW(NumExpr)(0), NEW(NumExpr)(1)))
          ->equals(NEW(BoolVal)(false)) );
    
    // interp method for VarExpr
    CHECK( evaluate_expr(NEW(VarExpr)("fish")) == "free variable: fish" );
    CHECK( evaluate_expr(NEW(VarExpr)("me")) == "free variable: me" );
    
    // interp method for BoolExpr
    CHECK( (NEW(BoolExpr)(true))->interp(Env::emptyenv)->equals(NEW(BoolVal)(true)) );
    CHECK( (NEW(BoolExpr)(false))->interp(Env::emptyenv)->equals(NEW(BoolVal)(false)) );
    CHECK( !(NEW(BoolExpr)(false))->interp(Env::emptyenv)->equals(NEW(BoolVal)(true)) );
    
    CHECK( Step::interp_by_steps(NEW(BoolExpr)(true))->equals(NEW(BoolVal)(true)) );
    CHECK( Step::interp_by_steps(NEW(BoolExpr)(false))->equals(NEW(BoolVal)(false)) );
    CHECK( !Step::interp_by_steps(NEW(BoolExpr)(false))->equals(NEW(BoolVal)(true)) );
    
    // interp method for LetExpr
    CHECK( (NEW(LetExpr)("x",
                         NEW(NumExpr)(1),
                         NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4))))
          ->interp(Env::emptyenv)->equals(NEW(NumVal)(5)));
    CHECK( evaluate_expr(NEW(LetExpr)("x",
                                      NEW(VarExpr)("y"),
                                      NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4))))
          == "free variable: y" );
    CHECK( (NEW(LetExpr)("x",
                         NEW(NumExpr)(1),
                         NEW(LetExpr)("x",
                                      NEW(NumExpr)(2),
                                      NEW(VarExpr)("x"))))
          ->interp(Env::emptyenv)->equals(NEW(NumVal)(2)) );
    
    CHECK( Step::interp_by_steps(NEW(LetExpr)("x",
                                              NEW(NumExpr)(1),
                                              NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4))))
          ->equals(NEW(NumVal)(5)));
    CHECK( Step::interp_by_steps(NEW(LetExpr)("x",
                                              NEW(NumExpr)(1),
                                              NEW(LetExpr)("x",
                                                           NEW(NumExpr)(2),
                                                           NEW(VarExpr)("x"))))
          ->equals(NEW(NumVal)(2)) );
    
    // interp for IfExpr
    CHECK( (NEW(IfExpr)(NEW(BoolExpr)(false),
                        NEW(NumExpr)(3),
                        NEW(NumExpr)(6)))
          ->interp(Env::emptyenv)->equals(NEW(NumVal)(6)) );
    CHECK( evaluate_expr(NEW(IfExpr)(NEW(CompExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4)),
                                     NEW(BoolExpr)(true),
                                     NEW(BoolExpr)(false)))
          == "free variable: x" );
    CHECK( !(NEW(IfExpr)(NEW(CompExpr)(NEW(NumExpr)(3), NEW(NumExpr)(4)),
                         NEW(NumExpr)(3),
                         NEW(NumExpr)(6)))
          ->interp(Env::emptyenv)->equals(NEW(NumVal)(3)) );
    
    CHECK( Step::interp_by_steps(NEW(IfExpr)(NEW(BoolExpr)(false),
                                             NEW(NumExpr)(3),
                                             NEW(NumExpr)(6)))
          ->equals(NEW(NumVal)(6)) );
    CHECK( !Step::interp_by_steps(NEW(IfExpr)(NEW(CompExpr)(NEW(NumExpr)(3), NEW(NumExpr)(4)),
                                              NEW(NumExpr)(3),
                                              NEW(NumExpr)(6)))
          ->equals(NEW(NumVal)(3)) );
    
    // interp for CompExpr
    CHECK((NEW(CompExpr)(NEW(BoolExpr)(true), NEW(BoolExpr)(true)))
          ->interp(Env::emptyenv)->equals((NEW(BoolVal)(true))) );
    CHECK((NEW(CompExpr)(NEW(AddExpr)(NEW(NumExpr)(6), NEW(NumExpr)(6)),
                         NEW(MultExpr)(NEW(NumExpr)(3), NEW(NumExpr)(4))))
          ->interp(Env::emptyenv)->equals((NEW(BoolVal)(true))) );
    CHECK(evaluate_expr(NEW(CompExpr)(NEW(VarExpr)("xyz"), NEW(NumExpr)(2)))
          == "free variable: xyz" );
    
    CHECK( Step::interp_by_steps(NEW(CompExpr)(NEW(BoolExpr)(true), NEW(BoolExpr)(true)))
          ->equals((NEW(BoolVal)(true))) );
    CHECK( Step::interp_by_steps(NEW(CompExpr)(NEW(AddExpr)(NEW(NumExpr)(6), NEW(NumExpr)(6)),
                                               NEW(MultExpr)(NEW(NumExpr)(3), NEW(NumExpr)(4))))
          ->equals((NEW(BoolVal)(true))) );
    
    // interp for FunExpr
    CHECK( (NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))))->interp(Env::emptyenv)
          ->to_string() == "[FUNCTION]" );
    CHECK( !((NEW(FunExpr)("y", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))))->interp(Env::emptyenv)
             ->to_string() == "FUNCTION]") );
    
    CHECK( Step::interp_by_steps(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))))
          ->to_string() == "[FUNCTION]" );
    CHECK( !(Step::interp_by_steps(NEW(FunExpr)("y", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))))
             ->to_string() == "FUNCTION]") );
    
    // interp for CallFunExpr
    CHECK( (NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))),
                             NEW(NumExpr)(4)))
          ->interp(Env::emptyenv)->equals(NEW(NumVal)(8)));
    CHECK( !(NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(MultExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))),
                              NEW(NumExpr)(4)))
          ->interp(Env::emptyenv)->equals(NEW(NumVal)(15)));
    
    CHECK( Step::interp_by_steps(NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))),
                             NEW(NumExpr)(4)))
          ->equals(NEW(NumVal)(8)));
    CHECK( !Step::interp_by_steps(NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(MultExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))),
                              NEW(NumExpr)(4)))
          ->equals(NEW(NumVal)(15)));
}


TEST_CASE( "subst") {
    // subst method for NumExpr
    CHECK( (NEW(NumExpr)(10))->subst("dog", NEW(NumVal)(3))
          ->equals(NEW(NumExpr)(10)) );
    CHECK( !(NEW(NumExpr)(10))->subst("dog", NEW(NumVal)(9))
          ->equals(NEW(NumExpr)(9)) );
    
    // subst method for AddExpr
    CHECK( (NEW(AddExpr)(NEW(NumExpr)(2), NEW(VarExpr)("dog")))->subst("dog", NEW(NumVal)(3))
          ->equals(NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3))) );
    CHECK( !(NEW(AddExpr)(NEW(VarExpr)("cat"), NEW(VarExpr)("dog")))->subst("dog", NEW(NumVal)(3))
          ->equals(NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3))) );
    
    // subst method for MultExpr
    CHECK( (NEW(MultExpr)(NEW(NumExpr)(2), NEW(VarExpr)("dog")))->subst("dog", NEW(NumVal)(3))
          ->equals(NEW(MultExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3))) );
    CHECK( !(NEW(MultExpr)(NEW(VarExpr)("cat"), NEW(VarExpr)("dog")))->subst("dog", NEW(NumVal)(3))
          ->equals(NEW(MultExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3))) );
    
    // subst method for VarExpr
    CHECK( (NEW(VarExpr)("fish"))->subst("dog", NEW(NumVal)(3))
          ->equals(NEW(VarExpr)("fish")) );
    CHECK( (NEW(VarExpr)("dog"))->subst("dog", NEW(NumVal)(3) )
          ->equals(NEW(NumExpr)(3)) );
    CHECK( (NEW(VarExpr)("dog"))->subst("dog", NEW(BoolVal)(true) )
          ->equals(NEW(BoolExpr)(true)) );
    CHECK( !(NEW(VarExpr)("cat"))->subst("dog", NEW(NumVal)(3) )
          ->equals(NEW(NumExpr)(3)) );
    
    // subst method for BoolExpr
    CHECK( (NEW(BoolExpr)(true))->subst("x", NEW(NumVal)(3))->equals(NEW(BoolExpr)(true)) );
    CHECK( !(NEW(BoolExpr)(false))->subst("false", NEW(NumVal)(3))->equals(NEW(NumExpr)(3)) );
    
    // subst method for LetExpr
    CHECK( (NEW(LetExpr)("x",
                         NEW(VarExpr)("y"),
                         NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4))))
          ->subst("y", NEW(NumVal)(3))
          ->equals(NEW(LetExpr)("x",
                                NEW(NumExpr)(3),
                                NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4)))) );
    CHECK( !(NEW(LetExpr)("x",
                          NEW(VarExpr)("z"),
                          NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4))))
          ->subst("y", NEW(NumVal)(3))
          ->equals(NEW(LetExpr)("x",
                                NEW(NumExpr)(3),
                                NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4)))) );
    
    CHECK( (NEW(LetExpr)("x",
                         NEW(NumExpr)(3),
                         NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4))))
          ->subst("x", NEW(NumVal)(7))
          ->equals(NEW(LetExpr)("x",
                                NEW(NumExpr)(3),
                                NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4)))) );
    
    // subst method for IfExpr
    CHECK( (NEW(IfExpr)(NEW(CompExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3)),
                        NEW(BoolExpr)(true),
                        NEW(BoolExpr)(false)))
          ->subst("x", NEW(NumVal)(3))
          ->equals(NEW(IfExpr)(NEW(CompExpr)(NEW(NumExpr)(3), NEW(NumExpr)(3)),
                               NEW(BoolExpr)(true),
                               NEW(BoolExpr)(false))) );
    
    CHECK( !(NEW(IfExpr)(NEW(CompExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3)),
                         NEW(BoolExpr)(true),
                         NEW(BoolExpr)(false)))
          ->subst("y", NEW(NumVal)(3))
          ->equals(NEW(IfExpr)(NEW(CompExpr)(NEW(NumExpr)(3), NEW(NumExpr)(3)),
                               NEW(BoolExpr)(true),
                               NEW(BoolExpr)(false))) );
    
    // subst method for CompExpr
    CHECK( (NEW(CompExpr)(NEW(VarExpr)("x"), NEW(BoolExpr)(true)))
          ->subst("x", NEW(NumVal)(124))
          ->equals(NEW(CompExpr)(NEW(NumExpr)(124), NEW(BoolExpr)(true))) );
    
    CHECK( !(NEW(CompExpr)(NEW(VarExpr)("x"), NEW(BoolExpr)(true)))
          ->subst("yx", NEW(NumVal)(124))
          ->equals(NEW(CompExpr)(NEW(NumExpr)(124), NEW(BoolExpr)(true))) );
    
    CHECK( (NEW(CompExpr)(NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4)),
                          NEW(MultExpr)(NEW(NumExpr)(12), NEW(VarExpr)("x"))))
          ->subst("x", NEW(NumVal)(124))
          ->equals(NEW(CompExpr)(NEW(AddExpr)(NEW(NumExpr)(124), NEW(NumExpr)(4)),
                                 NEW(MultExpr)(NEW(NumExpr)(12), NEW(NumExpr)(124)))) );
    
    // subst method for FunExpr
    CHECK( (NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))))->subst("x", NEW(NumVal)(3))
          ->equals( NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x")))) );
    CHECK( (NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("y"))))->subst("y", NEW(NumVal)(3))
          ->equals( NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3)))) );
    CHECK( !(NEW(FunExpr)("x", NEW(MultExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("y"))))->subst("x", NEW(NumVal)(3))
          ->equals( NEW(FunExpr)("x", NEW(AddExpr)(NEW(NumExpr)(3), NEW(VarExpr)("y")))) );
    
    // subst method for CallFunExpr
    CHECK( (NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))),
                             NEW(NumExpr)(4)))
          ->subst("x", NEW(NumVal)(3))
          ->equals((NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))),
                                     NEW(NumExpr)(4)))) );
    CHECK( (NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("y"))),
                             NEW(NumExpr)(4)))
          ->subst("y", NEW(NumVal)(3))
          ->equals((NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3))),
                                     NEW(NumExpr)(4)))) );
    CHECK( !(NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(MultExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))),
                              NEW(NumExpr)(4)))
          ->subst("x", NEW(NumVal)(3))
          ->equals((NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(NumExpr)(3), NEW(NumExpr)(3))),
                                     NEW(NumExpr)(4)))) );
}

TEST_CASE("haveVariable") {
    // haveVariable method for NumExpr
    CHECK( !(NEW(NumExpr)(3))->haveVariable() );
    CHECK( !(NEW(NumExpr)(0))->haveVariable() );
    
    // haveVariable method for AddExpr
    CHECK( !(NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3)))->haveVariable() );
    CHECK( (NEW(AddExpr)(NEW(NumExpr)(2), NEW(VarExpr)("x")))->haveVariable() );
    
    // haveVariable method for MultExpr
    CHECK( !(NEW(MultExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3)))->haveVariable() );
    CHECK( (NEW(MultExpr)(NEW(NumExpr)(2), NEW(VarExpr)("x")))->haveVariable() );
    
    // haveVariable method for VarExpr
    CHECK( (NEW(VarExpr)("x"))->haveVariable() ) ;
    CHECK( (NEW(VarExpr)("xyz"))->haveVariable() );
    
    // haveVariable method for BoolExpr
    CHECK( !(NEW(BoolExpr)(true))->haveVariable()) ;
    CHECK( !(NEW(BoolExpr)(false))->haveVariable() );
    
    // haveVariable method for LetExpr
    CHECK( (NEW(LetExpr)("x", NEW(VarExpr)("y"), NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4))))
          ->haveVariable() );
    CHECK( !(NEW(LetExpr)("x", NEW(NumExpr)(3), NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4))))
          ->haveVariable() );
    
    // haveVariable method for IfExpr
    CHECK( !(NEW(LetExpr)("x",
                          NEW(NumExpr)(1),
                          NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4))))
          ->haveVariable() );
    
    CHECK( (NEW(IfExpr)(NEW(CompExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3)),
                        NEW(BoolExpr)(true),
                        NEW(BoolExpr)(false)))
          ->haveVariable() );
    
    // haveVariable method for CompExpr
    CHECK( (NEW(CompExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3)))->haveVariable() );
    CHECK( !(NEW(CompExpr)(NEW(BoolExpr)(true), NEW(NumExpr)(3)))->haveVariable() );
    
    // haveVariable method for FunExpr
    CHECK( !(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))))->haveVariable() );
    CHECK( (NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("y"))))->haveVariable() );
    
    // haveVariable method for CallFunExpr
    CHECK( !(NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))),
                              NEW(NumExpr)(4)))->haveVariable() );
    CHECK( (NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("y"))),
                             NEW(NumExpr)(4)))->haveVariable() );
}

TEST_CASE("optimize") {
    // optimize method for NumExpr
    CHECK( (NEW(NumExpr)(3))->optimize()->equals(NEW(NumExpr)(3)) );
    CHECK( !(NEW(NumExpr)(3))->optimize()->equals(NEW(NumExpr)(8)) );
    
    // optimize method for AddExpr
    CHECK( (NEW(AddExpr)(NEW(NumExpr)(3), NEW(NumExpr)(4)))->optimize()->equals(NEW(NumExpr)(7)) );
    CHECK( (NEW(AddExpr)(NEW(NumExpr)(3), NEW(VarExpr)("x")))->optimize()
          ->equals(NEW(AddExpr)(NEW(NumExpr)(3), NEW(VarExpr)("x"))) );
    
    // optimize method for MultExpr
    CHECK( (NEW(MultExpr)(NEW(NumExpr)(3), NEW(NumExpr)(4)))->optimize()->equals(NEW(NumExpr)(12)) );
    CHECK( (NEW(MultExpr)(NEW(NumExpr)(3), NEW(VarExpr)("x")))->optimize()
          ->equals(NEW(MultExpr)(NEW(NumExpr)(3), NEW(VarExpr)("x"))) );
    
    // optimize method for VarExpr
    CHECK( (NEW(VarExpr)("x"))->optimize()->equals(NEW(VarExpr)("x")) );
    CHECK( !(NEW(VarExpr)("x"))->optimize()->equals(NEW(VarExpr)("y")) );
    
    // optimize method for BoolExpr
    CHECK( (NEW(BoolExpr)(true))->optimize()->equals(NEW(BoolExpr)(true)) );
    CHECK( !(NEW(BoolExpr)(true))->optimize()->equals(NEW(BoolExpr)(false)) );
    
    // optimize method for LetExpr
    CHECK( (NEW(LetExpr)("x",
                         NEW(VarExpr)("y"),
                         NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4))))
          ->optimize()
          ->equals(NEW(LetExpr)("x",
                                NEW(VarExpr)("y"),
                                NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4)))) );
    CHECK( (NEW(LetExpr)("x",
                         NEW(NumExpr)(3),
                         NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4))))
          ->optimize()->equals(NEW(NumExpr)(7)) );
    
    // optimize method for IfExpr
    CHECK( (NEW(IfExpr)(NEW(CompExpr)(NEW(AddExpr)(NEW(NumExpr)(3), NEW(NumExpr)(9)),
                                      NEW(MultExpr)(NEW(NumExpr)(3), NEW(NumExpr)(4))),
                        NEW(BoolExpr)(true),
                        NEW(BoolExpr)(false)))
          ->optimize()->equals(NEW(BoolExpr)(true)));
    
    CHECK( (NEW(IfExpr)(NEW(CompExpr)(NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(9)),
                                      NEW(MultExpr)(NEW(NumExpr)(3), NEW(NumExpr)(4))),
                        NEW(BoolExpr)(true),
                        NEW(BoolExpr)(false)))
          ->optimize()->equals(NEW(IfExpr)(NEW(CompExpr)(NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(9)),
                                                         NEW(NumExpr)(12)),
                                           NEW(BoolExpr)(true),
                                           NEW(BoolExpr)(false))));
    
    // optimize method for CompExpr
    CHECK( (NEW(CompExpr)(NEW(AddExpr)(NEW(NumExpr)(3), NEW(NumExpr)(9)),
                          NEW(MultExpr)(NEW(NumExpr)(3), NEW(NumExpr)(4))))
          ->optimize()->equals(NEW(BoolExpr)(true)));
    
    CHECK( (NEW(CompExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x")))
          ->optimize()->equals(NEW(CompExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))));
    
    CHECK( !(NEW(CompExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x")))
          ->optimize()->equals(NEW(BoolExpr)(true)));
    
    // optimize method for FunExpr
    CHECK( (NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))))->optimize()
          ->equals(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x")))) );
    CHECK( (NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"),
                                           NEW(AddExpr)(NEW(NumExpr)(3), NEW(NumExpr)(4)))))->optimize()
          ->equals(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(7)))) );
    CHECK( !(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"),
                                            NEW(MultExpr)(NEW(NumExpr)(3), NEW(NumExpr)(4)))))->optimize()
          ->equals(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(7)))) );
    
    
    // optimize method for CallFunExpr
    CHECK( (NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))),
                             NEW(NumExpr)(4)))
          ->optimize()->equals((NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))), NEW(NumExpr)(4)))) );
    
    CHECK( !(NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(MultExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))),
                              NEW(NumExpr)(4)))
          ->optimize()->equals(NEW(NumExpr)(16)) );
    
}
