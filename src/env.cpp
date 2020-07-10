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

PTR(Val) EmptyEnv::lookup(std::string find_name) {
    throw std::runtime_error("free variable: " + find_name);
}

bool EmptyEnv::equals(PTR(Env) env) {
    PTR(EmptyEnv) ee = CAST(EmptyEnv)(env);
    
    return ee != NULL;
}

int EmptyEnv::size() {
    return sizeof(EmptyEnv);
}

void EmptyEnv::trace() {
    
}

ExtendedEnv::ExtendedEnv(std::string name, PTR(Val) val, PTR(Env) env) {
    this->rest = env;
    this->name = name;
    this->val = val;
}

PTR(Val) ExtendedEnv::lookup(std::string find_name) {
    if (find_name == name)
        return val;
    else
        return rest->lookup(find_name);
}

bool ExtendedEnv::equals(PTR(Env) env) {
    PTR(ExtendedEnv) ee = CAST(ExtendedEnv)(env);
    
    if (ee == NULL)
        return false;
    else
        return name == ee->name && val->equals(ee->val) && rest->equals(ee->rest);
}

int ExtendedEnv::size() {
    return sizeof(ExtendedEnv);
}

void ExtendedEnv::trace() {
    UPDATE(val);
    UPDATE(rest);
}
