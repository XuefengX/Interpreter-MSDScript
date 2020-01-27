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

NumVal::NumVal(int rep){
    this->rep = rep;
}

bool NumVal::equals(Val *other_val){
    NumVal *nv = dynamic_cast<NumVal*>(other_val);
    if(nv == nullptr)
        return false;
    else
        return rep == nv->rep;
}

Val *NumVal::add_to(Val *other_val){
    NumVal *nv = dynamic_cast<NumVal*>(other_val);
    if(nv == nullptr)
        throw std::runtime_error((std::string)"Addend is not a number");
    else
        return new NumVal(rep + nv->rep);
}

Val *NumVal::mult_with(Val *other_val){
    NumVal *nv = dynamic_cast<NumVal*>(other_val);
    if(nv == nullptr)
        throw std::runtime_error((std::string)"Addend is not a number");
    else
        return new NumVal(rep * nv->rep);
}

Expr *NumVal::to_expr(){
    return new NumExpr(rep);
}

std::string NumVal::to_string(){
    return std::to_string(rep);
}

BoolVal::BoolVal(bool rep){
    this->rep = rep;
}

bool BoolVal::equals(Val *other_val){
    BoolVal *bv = dynamic_cast<BoolVal*>(other_val);
    if(bv == nullptr)
        return false;
    else
        return rep == bv->rep;
}

Val *BoolVal::add_to(Val *other_val){
    throw std::runtime_error((std::string)"No adding booleans");
}

Val *BoolVal::mult_with(Val *other_val){
    throw std::runtime_error((std::string)"No multiplying booleans");
}

Expr *BoolVal::to_expr(){
    return new BoolExpr(rep);
}

std::string BoolVal::to_string()
{
    if (rep)
        return "_true";
    else
        return "_false";
}

TEST_CASE( "values equals" ) {
    CHECK( (new NumVal(5))->equals(new NumVal(5)) );
    CHECK( ! (new NumVal(7))->equals(new NumVal(5)) );
    
    CHECK( (new BoolVal(true))->equals(new BoolVal(true)) );
    CHECK( ! (new BoolVal(true))->equals(new BoolVal(false)) );
    CHECK( ! (new BoolVal(false))->equals(new BoolVal(true)) );
    
    CHECK( ! (new NumVal(7))->equals(new BoolVal(false)) );
    CHECK( ! (new BoolVal(false))->equals(new NumVal(8)) );
}

TEST_CASE( "add_to" ) {
    
    CHECK ( (new NumVal(5))->add_to(new NumVal(8))->equals(new NumVal(13)) );
    
    CHECK_THROWS_WITH ( (new NumVal(5))->add_to(new BoolVal(false)), "not a number" );
    CHECK_THROWS_WITH ( (new BoolVal(false))->add_to(new BoolVal(false)),
                       "no adding booleans" );
}

TEST_CASE( "mult_with" ) {
    
    CHECK ( (new NumVal(5))->mult_with(new NumVal(8))->equals(new NumVal(40)) );
    
    CHECK_THROWS_WITH ( (new NumVal(5))->mult_with(new BoolVal(false)), "not a number" );
    CHECK_THROWS_WITH ( (new BoolVal(false))->mult_with(new BoolVal(false)),
                       "no multiplying booleans" );
}

TEST_CASE( "value to_expr" ) {
    CHECK( (new NumVal(5))->to_expr()->equals(new NumExpr(5)) );
    CHECK( (new BoolVal(true))->to_expr()->equals(new BoolExpr(true)) );
    CHECK( (new BoolVal(false))->to_expr()->equals(new BoolExpr(false)) );
}

TEST_CASE( "value to_string" ) {
    CHECK( (new NumVal(5))->to_string() == "5" );
    CHECK( (new BoolVal(true))->to_string() == "_true" );
    CHECK( (new BoolVal(false))->to_string() == "_false" );
}

