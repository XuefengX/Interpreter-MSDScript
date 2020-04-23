//
//  cont.hpp
//  Interpreter
//
//  Created by Xuefeng Xu on 3/25/20.
//  Copyright © 2020 Xuefeng Xu. All rights reserved.
//

#ifndef cont_hpp
#define cont_hpp

#include <iostream>
#include "pointer.hpp"

class Expr;
class Val;
class Env;

class Cont ENABLE_THIS(Cont) {
public:
    /* To take one step in the computation starting
     with this continuation, reading from the registers
     in `Step` and updating them to indicate the next
     step. The `Step::cont` register will contain
     this continuation (so it's uninteresting), and
     the `Step::val` register will contain the value
     that this continuaion was waiting form.
     The `Step::expr` register is unspecified
     (i.e., must not be used by this method). */
    virtual void step_continue() = 0;
    
    static PTR(Cont) done;
};

class DoneCont : public Cont {
public:
    DoneCont();
    void step_continue();
};

class RightThenAddCont : public Cont {
public:
    PTR(Expr) rhs;
    PTR(Env) env;
    PTR(Cont) rest;
    
    RightThenAddCont(PTR(Expr) rhs, PTR(Env) env, PTR(Cont) rest);
    void step_continue();
};

class AddCont : public Cont {
public:
    PTR(Val) lhs_val;
    PTR(Cont) rest;
    
    AddCont(PTR(Val) lhs_val, PTR(Cont) rest);
    void step_continue();
};

class RightThenMultCont : public Cont {
public:
    PTR(Expr) rhs;
    PTR(Env) env;
    PTR(Cont) rest;
    
    RightThenMultCont(PTR(Expr) rhs, PTR(Env) env, PTR(Cont) rest);
    void step_continue();
};

class MultCont : public Cont {
public:
    PTR(Val) lhs_val;
    PTR(Cont) rest;
    
    MultCont(PTR(Val) lhs_val, PTR(Cont) rest);
    void step_continue();
};

class RightThenCompCont : public Cont {
public:
    PTR(Expr) rhs;
    PTR(Env) env;
    PTR(Cont) rest;
    
    RightThenCompCont(PTR(Expr) rhs, PTR(Env) env, PTR(Cont) rest);
    void step_continue();
};

class CompCont : public Cont {
public:
    PTR(Val) lhs_val;
    PTR(Cont) rest;
    
    CompCont(PTR(Val) lhs_val, PTR(Cont) rest);
    void step_continue();
};

class ArgThenCallCont : public Cont {
public:
    PTR(Expr) actual_arg;
    PTR(Env) env;
    PTR(Cont) rest;
    
    ArgThenCallCont(PTR(Expr) actual_arg, PTR(Env) env, PTR(Cont) rest);
    void step_continue();
};

class CallCont : public Cont {
public:
    PTR(Val) to_be_called;
    PTR(Cont) rest;
    
    CallCont(PTR(Val) to_be_called, PTR(Cont) rest);
    void step_continue();
};

class IfBranchCont : public Cont {
public:
    PTR(Expr) then_part;
    PTR(Expr) else_part;
    PTR(Env) env;
    PTR(Cont) rest;
    
    IfBranchCont(PTR(Expr) then_part, PTR(Expr) else_part, PTR(Env) env, PTR(Cont) rest);
    void step_continue();
};

class LetBodyCont : public Cont {
public:
    std::string var;
    PTR(Expr) body;
    PTR(Env) env;
    PTR(Cont) rest;
    
    LetBodyCont(std::string var, PTR(Expr) body, PTR(Env) env, PTR(Cont) rest);
    void step_continue();
};

#endif /* cont_hpp */
