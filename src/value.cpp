//
//  value.cpp
//  Arithmetic Calculator and Optimizer
//
//  Created by Xuefeng Xu on 1/21/20.
//  Copyright © 2020 Xuefeng Xu. All rights reserved.
//

#include "value.hpp"
#include "expr.hpp"
#include "catch.hpp"
#include "env.hpp"
#include "step.hpp"

NumVal::NumVal(int rep){
    this->rep = rep;
}

bool NumVal::equals(PTR(Val) other_val){
    PTR(NumVal) nv = CAST(NumVal)(other_val);
    if(nv == nullptr)
        return false;
    else
        return rep == nv->rep;
}

PTR(Val) NumVal::add_to(PTR(Val) other_val){
    PTR(NumVal) nv = CAST(NumVal)(other_val);
    if(nv == nullptr)
        throw std::runtime_error((std::string)"Addend is not a number");
    else
        return NEW(NumVal)(rep + nv->rep);
}

PTR(Val) NumVal::mult_with(PTR(Val) other_val){
    PTR(NumVal) nv = CAST(NumVal)(other_val);
    if(nv == nullptr)
        throw std::runtime_error((std::string)"Mult is not a number");
    else
        return NEW(NumVal)(rep * nv->rep);
}

bool NumVal::is_ture(){
    throw std::runtime_error((std::string)"evaluate non-boolean");
}

PTR(Expr) NumVal::to_expr(){
    return NEW(NumExpr)(rep);
}

PTR(Val) NumVal::call(PTR(Val) actual_arg){
    throw std::runtime_error("not call a num");
}

void NumVal::call_step(PTR(Val) actual_arg_val, PTR(Cont) rest){
    throw std::runtime_error("not call a num");
}

std::string NumVal::to_string(){
    return std::to_string(rep);
}

BoolVal::BoolVal(bool rep){
    this->rep = rep;
}

bool BoolVal::equals(PTR(Val) other_val){
    PTR(BoolVal) bv = CAST(BoolVal)(other_val);
    if(bv == nullptr)
        return false;
    else
        return rep == bv->rep;
}

PTR(Val) BoolVal::add_to(PTR(Val) other_val){
    throw std::runtime_error((std::string)"No adding booleans");
}

PTR(Val) BoolVal::mult_with(PTR(Val) other_val){
    throw std::runtime_error((std::string)"No multiplying booleans");
}

PTR(Expr) BoolVal::to_expr(){
    return NEW(BoolExpr)(rep);
}

bool BoolVal::is_ture(){
    return rep;
}

PTR(Val) BoolVal::call(PTR(Val) actual_arg){
    throw std::runtime_error("not call a bool");
}

void BoolVal::call_step(PTR(Val) actual_arg_val, PTR(Cont) rest){
    throw std::runtime_error("not call a bool");
}

std::string BoolVal::to_string()
{
    if (rep)
        return "_true";
    else
        return "_false";
}

FuncVal::FuncVal(std::string formal_arg, PTR(Expr) body, PTR(Env) env){
    this->formal_arg = formal_arg;
    this->body = body;
    this->env = env;
}

bool FuncVal::equals(PTR(Val) other_val){
    PTR(FuncVal) fv = CAST(FuncVal)(other_val);
    return formal_arg == fv->formal_arg && body->equals(fv->body);
}

PTR(Val) FuncVal::add_to(PTR(Val) other_val){
    throw std::runtime_error((std::string)"No adding functions");
}

PTR(Val) FuncVal::mult_with(PTR(Val) other_val){
    throw std::runtime_error((std::string)"No multiplying functions");
}

bool FuncVal::is_ture(){
    throw std::runtime_error((std::string)"evaluate non-boolean");
}

PTR(Expr) FuncVal::to_expr(){
    return NEW(FuncExpr)(formal_arg, body);
}

PTR(Val) FuncVal::call(PTR(Val) actual_arg){
    return body->interp(NEW(ExtendedEnv)(formal_arg, actual_arg, env));
}

void FuncVal::call_step(PTR(Val) actual_arg_val, PTR(Cont) rest){
    Step::mode = Step::interp_mode;
    Step::expr = body;
    Step::env = NEW(ExtendedEnv)(formal_arg, actual_arg_val, env);
    Step::cont = rest;
}

std::string FuncVal::to_string(){
    return "_fun (" + formal_arg + ") " + body->to_string();
}

TEST_CASE( "values equals" ) {
    CHECK( (NEW(NumVal)(5))->equals(NEW(NumVal)(5)) );
    CHECK( ! (NEW(NumVal)(7))->equals(NEW(NumVal)(5)) );
    
    CHECK( (NEW(BoolVal)(true))->equals(NEW(BoolVal)(true)) );
    CHECK( ! (NEW(BoolVal)(true))->equals(NEW(BoolVal)(false)) );
    CHECK( ! (NEW(BoolVal)(false))->equals(NEW(BoolVal)(true)) );
    
    CHECK( ! (NEW(NumVal)(7))->equals(NEW(BoolVal)(false)) );
    CHECK( ! (NEW(BoolVal)(false))->equals(NEW(NumVal)(8)) );
}

TEST_CASE( "add_to" ) {
    
    CHECK ( (NEW(NumVal)(5))->add_to(NEW(NumVal)(8))->equals(NEW(NumVal)(13)) );
    
    CHECK_THROWS_WITH ( (NEW(NumVal)(5))->add_to(NEW(BoolVal)(false)), "Addend is not a number" );
    CHECK_THROWS_WITH ( (NEW(BoolVal)(false))->add_to(NEW(BoolVal)(false)),
                       "No adding booleans" );
}

TEST_CASE( "mult_with" ) {
    
    CHECK ( (NEW(NumVal)(5))->mult_with(NEW(NumVal)(8))->equals(NEW(NumVal)(40)) );
    
    CHECK_THROWS_WITH ( (NEW(NumVal)(5))->mult_with(NEW(BoolVal)(false)), "Mult is not a number" );
    CHECK_THROWS_WITH ( (NEW(BoolVal)(false))->mult_with(NEW(BoolVal)(false)),
                       "No multiplying booleans" );
}

TEST_CASE( "value to_expr" ) {
    CHECK( (NEW(NumVal)(5))->to_expr()->equals(NEW(NumExpr)(5)) );
    CHECK( (NEW(BoolVal)(true))->to_expr()->equals(NEW(BoolExpr)(true)) );
    CHECK( (NEW(BoolVal)(false))->to_expr()->equals(NEW(BoolExpr)(false)) );
}

TEST_CASE( "value to_string" ) {
    CHECK( (NEW(NumVal)(5))->to_string() == "5" );
    CHECK( (NEW(BoolVal)(true))->to_string() == "_true" );
    CHECK( (NEW(BoolVal)(false))->to_string() == "_false" );
}

