//
//  env.cpp
//  Interpreter
//
//  Created by Xuefeng Xu on 3/3/20.
//  Copyright © 2020 Xuefeng Xu. All rights reserved.
//

#include "env.hpp"
#include "value.hpp"

PTR(Env) Env::emptyenv = NEW(EmptyEnv)();

PTR(Val) EmptyEnv::lookup(std::string file_name){
    throw std::runtime_error((std::string)"free variable" + file_name);
}

bool EmptyEnv::equals(PTR(Env) env){
    return CAST(EmptyEnv)(env) != nullptr;
}

ExtendedEnv::ExtendedEnv(std::string name, PTR(Val) val, PTR(Env) rest){
    this->name = name;
    this->val = val;
    this->rest = rest;
}

PTR(Val) ExtendedEnv::lookup(std::string file_name){
    if(file_name == name) return val;
    else return rest->lookup(file_name);
}

bool ExtendedEnv::equals(PTR(Env) env){
    PTR(ExtendedEnv) e = CAST(ExtendedEnv)(env);
    if(e == nullptr) return false;
    else return name == e->name && val->equals(e->val) && rest->equals(e->rest);
}
