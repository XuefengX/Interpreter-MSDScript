//
//  cont.cpp
//  Interpreter
//
//  Created by Xuefeng Xu on 3/25/20.
//  Copyright © 2020 Xuefeng Xu. All rights reserved.
//

#include "cont.hpp"
#include "step.hpp"
#include "value.hpp"
#include "env.hpp"
#include "expr.hpp"


PTR(Cont) Cont::done = NEW(DoneCont)();

DoneCont::DoneCont() { }

void DoneCont::step_continue() {
    throw std::runtime_error("can't continue done");
}


RightThenAddCont::RightThenAddCont(PTR(Expr) rhs, PTR(Env) env, PTR(Cont) rest) {
    this->rhs = rhs;
    this->env = env;
    this->rest = rest;
}

void RightThenAddCont::step_continue() {
    PTR(Val) lhs_val = Step::val;
    Step::mode = Step::interp_mode;
    Step::expr = rhs;
    Step::env = env;
    Step::cont = NEW(AddCont)(lhs_val, rest);
}

AddCont::AddCont(PTR(Val) lhs_val, PTR(Cont) rest) {
    this->lhs_val = lhs_val;
    this->rest = rest;
}

void AddCont::step_continue() {
    PTR(Val) rhs_val = Step::val;
    Step::mode = Step::continue_mode;
    Step::val = lhs_val->add_to(rhs_val);
    Step::cont = rest;
}

RightThenMultCont::RightThenMultCont(PTR(Expr) rhs, PTR(Env) env, PTR(Cont) rest) {
    this->rhs = rhs;
    this->env = env;
    this->rest = rest;
}

void RightThenMultCont::step_continue() {
    PTR(Val) lhs_val = Step::val;
    Step::mode = Step::interp_mode;
    Step::expr = rhs;
    Step::env = env;
    Step::cont = NEW(MultCont)(lhs_val, rest);
}

MultCont::MultCont(PTR(Val) lhs_val, PTR(Cont) rest) {
    this->lhs_val = lhs_val;
    this->rest = rest;
}

void MultCont::step_continue() {
    PTR(Val) rhs_val = Step::val;
    Step::mode = Step::continue_mode;
    Step::val = lhs_val->mult_with(rhs_val);
    Step::cont = rest;
}

RightThenCompCont::RightThenCompCont(PTR(Expr) rhs, PTR(Env) env, PTR(Cont) rest) {
    this->rhs = rhs;
    this->env = env;
    this->rest = rest;
}

void RightThenCompCont::step_continue() {
    PTR(Val) lhs_val = Step::val;
    Step::mode = Step::interp_mode;
    Step::expr = rhs;
    Step::env = env;
    Step::cont = NEW(CompCont)(lhs_val, rest);
}

CompCont::CompCont(PTR(Val) lhs_val, PTR(Cont) rest) {
    this->lhs_val = lhs_val;
    this->rest = rest;
}

void CompCont::step_continue() {
    PTR(Val) rhs_val = Step::val;
    Step::mode = Step::continue_mode;
    if (lhs_val->equals(rhs_val))
        Step::val = NEW(BoolVal)(true);
    else
        Step::val = NEW(BoolVal)(false);
    Step::cont = rest;
}

ArgThenCallCont::ArgThenCallCont(PTR(Expr) actual_arg, PTR(Env) env, PTR(Cont) rest) {
    this->actual_arg = actual_arg;
    this->env = env;
    this->rest = rest;
}

void ArgThenCallCont::step_continue() {
    PTR(Val) to_be_called = Step::val;
    Step::mode = Step::interp_mode;
    Step::expr = actual_arg;
    Step::env = env;
    Step::cont = NEW(CallCont)(to_be_called, rest);
}

CallCont::CallCont(PTR(Val) to_be_called, PTR(Cont) rest) {
    this->to_be_called = to_be_called;
    this->rest = rest;
}

void CallCont::step_continue() {
    to_be_called->call_step(Step::val, rest);
}

IfBranchCont::IfBranchCont(PTR(Expr) then_part, PTR(Expr) else_part, PTR(Env) env, PTR(Cont) rest) {
    this->then_part = then_part;
    this->else_part = else_part;
    this->env = env;
    this->rest = rest;
}

void IfBranchCont::step_continue() {
    PTR(BoolVal) if_val = CAST(BoolVal)(Step::val);
    
    if (if_val == NULL)
        throw std::runtime_error("if part doesn't evaluate to a bool val!");
    else if (if_val->rep == true)
        Step::expr = then_part;
    else
        Step::expr = else_part;
    Step::env = env;
    Step::mode = Step::interp_mode;
    Step::cont = rest;
}

LetBodyCont::LetBodyCont(std::string var, PTR(Expr) body, PTR(Env) env, PTR(Cont) rest) {
    this->var = var;
    this->body = body;
    this->env = env;
    this->rest = rest;
}

void LetBodyCont::step_continue() {
    PTR(Val) rhs_val = Step::val;
    Step::mode = Step::interp_mode;
    Step::env = NEW(ExtendedEnv)(var, rhs_val, Step::env);
    Step::expr = body;
    Step::cont = rest;
}
