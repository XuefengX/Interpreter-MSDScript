//
//  env.hpp
//  Interpreter
//
//  Created by Xuefeng Xu on 3/3/20.
//  Copyright © 2020 Xuefeng Xu. All rights reserved.
//

#ifndef env_hpp
#define env_hpp

#include "pointer.hpp"
#include <string>

class Val;

class Env ENABLE_THIS(Env){
public:
    static PTR(Env) emptyenv;
    virtual PTR(Val) lookup(std::string file_name) = 0;
    virtual bool equals(PTR(Env) env) = 0;
};

class EmptyEnv : public Env{
public:
    PTR(Val) lookup(std::string file_name);
    bool equals(PTR(Env) env);
};

class ExtendedEnv : public Env{
public:
    std::string name;
    PTR(Val) val;
    PTR(Env) rest;
    
    ExtendedEnv(std::string name, PTR(Val) val, PTR(Env) rest);
    PTR(Val) lookup(std::string file_name);
    bool equals(PTR(Env) env);
};

#endif /* env_hpp */
