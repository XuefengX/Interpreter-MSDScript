//
//  parser.cpp
//  Arithmetic_Parser
//
//  Created by Xuefeng Xu on 1/13/20.
//  Copyright © 2020 Xuefeng Xu. All rights reserved.
//

#include <sstream>
#include "parse.hpp"
#include "catch.hpp"
#include "expr.hpp"
#include "env.hpp"
#include "value.hpp"
#include "step.hpp"

/**
 Ignore all the blankspace and return the next character in the input stream without change it
 Param: istream in - input stream
 Return: first char in the stream without space
 */
char peek_next(std::istream &in){
    while(in.peek() == ' ' || in.peek() == '\n')
        in.get();
    return in.peek();
}

/**
 Get the next non-space character
 */
char get_next(std::istream &in){
    peek_next(in);
    return in.get();
}

PTR(Expr) parse_if(std::istream &in);
PTR(Expr) parse_comparg(std::istream &in);
PTR(Expr) parse_addend(std::istream &in);
PTR(Expr) parse_multicand(std::istream &in);
PTR(Expr) parse_inner(std::istream &in);
PTR(Expr) parse_number(std::istream &in);
PTR(Expr) parse_variable(std::istream &in);
PTR(Expr) parse_let(std::istream &in);
PTR(Expr) parse_fun(std::istream &in);
std::string parse_keyword(std::istream &in);
std::string parse_alphabetic(std::istream &in, std::string prefix);

/**
 Return an expression (PTR(Expr) ) accroding to grammar:
 <expr> = <comparg>
        | <comparg> + <expr>
 */
PTR(Expr) parse_expr(std::istream &in) {
    PTR(Expr) num = parse_comparg(in);
    if(num == nullptr) return nullptr;
    char c = peek_next(in);
    if (c == '=') {
        get_next(in);
        if(get_next(in) != '=') throw std::runtime_error((std::string)"should be double equal");
        PTR(Expr) temp = parse_expr(in);
        if(temp == nullptr) return nullptr;
        return NEW(EquExpr)(num, temp);
    }
    return num;
}

/**
 <conparg> = <addend>
           | <addend> + <comparg>
 */
PTR(Expr) parse_comparg(std::istream &in){
    PTR(Expr) add = parse_addend(in);
    if(add == nullptr) return nullptr;
    char c = peek_next(in);
    if(c == '+'){
        get_next(in);
        PTR(Expr) comp = parse_comparg(in);
        if(comp == nullptr) return nullptr;
        return NEW(AddExpr)(add, comp);
    }
    return add;
}

/**
 Return an expresion according to grammar:
 <addend> = <multicand>
          | <multicand> * <addend>
 */
PTR(Expr) parse_addend(std::istream &in) {
    PTR(Expr) num = parse_multicand(in);
    if(num == nullptr) return nullptr;
    char c = peek_next(in);
    if (c == '*') {
        get_next(in);
        PTR(Expr) temp = parse_addend(in);
        if(temp == nullptr) return nullptr;
        return NEW(MultExpr)(num, temp);
    }
    return num;
}

/**
 <multicand> = <inner>
             | <multicand> ( <expr> )
 */
PTR(Expr) parse_multicand(std::istream &in) {
    PTR(Expr) temp = parse_inner(in);
    while (peek_next(in) == '(') {
        get_next(in);
        PTR(Expr) actual_arg = parse_expr(in);
        if(get_next(in) != ')')
            throw std::runtime_error((std::string)"bad format");
        temp = NEW(CallExpr)(temp, actual_arg);
    }
    return temp;
}

/**
 Return an expression according to grammar:
 <number-or-paren> = <number>
                   | (<expr>)
                   | <variable>
                   | _let <variable> = <expr> _in <expr>
                   | _true/_false
                   | _if <expr> _then <expr> _else <expr>
                   | _fun ( <variable> ) <expr>
 */
PTR(Expr) parse_inner(std::istream &in){
    char c = peek_next(in);
    if (c == '(') { // if is a parenthesis
        get_next(in);
        PTR(Expr) num = parse_expr(in);
        if(num == nullptr || peek_next(in) != ')') return nullptr;
        get_next(in);
        return num;
    } else if (c == '-' || isdigit(c)) { // if is a number(or negative number)
        return parse_number(in);
    } else if (isalpha(c)){ // if is a variable
        return parse_variable(in);
    } else if (c == '_'){
        std::string keyword = parse_keyword(in);
        if (keyword == "_true")
            return NEW(BoolExpr)(true);
        else if (keyword == "_false")
            return NEW(BoolExpr)(false);
        else if (keyword == "_let")
            return parse_let(in);
        else if (keyword == "_if")
            return parse_if(in);
        else if (keyword == "_fun")
            return parse_fun(in);
        else
            throw std::runtime_error((std::string)"unexpected keyword " + keyword);
    } else
        return nullptr;
}

PTR(Expr) parse_if(std::istream &in){
    PTR(Expr) test_part = parse_expr(in);
    if(parse_keyword(in) != "_then") throw std::runtime_error((std::string)"unexpected keyword");
    PTR(Expr) then_part = parse_expr(in);
    if(parse_keyword(in) != "_else") throw std::runtime_error((std::string)"unexpected keyword");
    PTR(Expr) else_part = parse_expr(in);
    return NEW(IfExpr)(test_part, then_part, else_part);
}

PTR(Expr) parse_let(std::istream &in){
    peek_next(in);
    std::string variable = parse_alphabetic(in, "");
    char c = peek_next(in);
    if(c != '=')
        throw std::runtime_error((std::string)"Should have = keyword");
    in.get();
    PTR(Expr) fe = parse_expr(in);
    std::string in_string = parse_keyword(in);
    if(in_string != "_in")
        throw std::runtime_error((std::string)"Should have _in keyword");
    PTR(Expr) se = parse_expr(in);
    return NEW(LetExpr)(variable, fe, se);
}


// Parses a number, assuming that `in` starts with a digit.
PTR(Expr) parse_number(std::istream &in) {
    char c = peek_next(in);
    int flag = 1;
    if(c == '-'){
        flag = -1;
        get_next(in);
    }
    if(!isdigit(peek_next(in))) throw std::runtime_error((std::string)"Unexpected number");
    int num = 0;
    in >> num;
    return NEW(NumExpr)(flag * num);
}

// Parses an expression, assuming that `in` starts with a
// letter.
PTR(Expr) parse_variable(std::istream &in) {
    return NEW(VarExpr)(parse_alphabetic(in, ""));
}

// Parse a function
PTR(Expr) parse_fun(std::istream &in){
    char c = peek_next(in);
    if(c == '('){
        get_next(in);
        peek_next(in);
        std::string formal_var = parse_alphabetic(in, "");
        c = peek_next(in);
        if(c != ')')
            throw std::runtime_error((std::string)"not a function format");
        get_next(in);
        PTR(Expr) body = parse_expr(in);
        return NEW(FuncExpr)(formal_var, body);
    } else
        throw std::runtime_error((std::string)"not a function format");
}

// Parses an expression, assuming that `in` starts with a
// letter.
std::string parse_keyword(std::istream &in) {
    get_next(in); // consume `_`
    return parse_alphabetic(in, "_");
}

// Parses an expression, assuming that `in` starts with a
// letter.
std::string parse_alphabetic(std::istream &in, std::string prefix) {
    std::string name = prefix;
    while (1) {
        char c = in.peek();
        if (!isalpha(c))
            break;
        name += in.get();
    }
    return name;
}

/**
 Parse input stream into an Expr object and return
 */
PTR(Expr) parse(std::istream &in){
    return parse_expr(in);
}

/**
 Convert the string into an istream and all parse to read the stream and
 parse the string into a Expr object
 */
PTR(Expr) parse_str(std::string s){
    std::istringstream in(s);
    return parse(in);
}


TEST_CASE( "parse" ) {
    CHECK((NEW(NumExpr)(1))->equals(NEW(NumExpr)(1)));
    CHECK(!(NEW(NumExpr)(1))->equals(NEW(NumExpr)(2)));
    CHECK(!(NEW(NumExpr)(1))->equals(NEW(MultExpr)(NEW(NumExpr)(2), NEW(NumExpr)(4))));
    CHECK((NEW(VarExpr)("hello"))->equals(NEW(VarExpr)("hello")));
    CHECK(!(NEW(VarExpr)("hello"))->equals(NEW(VarExpr)("ello")));
    CHECK(peek_next(*(NEW(std::istringstream)("hello"))) == 'h');
    CHECK(peek_next(*(NEW(std::istringstream)("   hello"))) == 'h');
    CHECK(get_next(*(NEW(std::istringstream)("   hello"))) == 'h');
    CHECK(parse_str("1")->equals(NEW(NumExpr)(1)));
    CHECK(parse_str("  1")->equals(NEW(NumExpr)(1)));
    CHECK(parse_str("4+2")->equals(NEW(AddExpr)(NEW(NumExpr)(4), NEW(NumExpr)(2))));
    CHECK(parse_str("   45 +    23")->equals(NEW(AddExpr)(NEW(NumExpr)(45), NEW(NumExpr)(23))));
    CHECK(parse_str("   45   *  23   ")->equals(NEW(MultExpr)(NEW(NumExpr)(45), NEW(NumExpr)(23))));
    CHECK(parse_str("(90)")->equals(NEW(NumExpr)(90)));
    CHECK(parse_str("3*(2+43)")->equals(NEW(MultExpr)(NEW(NumExpr)(3),NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(43)))));
    CHECK(parse_str("Hello")->equals(NEW(VarExpr)("Hello")));
    CHECK(parse_str("3*(2+width)")->equals(NEW(MultExpr)(NEW(NumExpr)(3),NEW(AddExpr)(NEW(NumExpr)(2), NEW(VarExpr)("width")))));
}


TEST_CASE("interpreter"){
    CHECK(parse_str("_let x = (_let y = 7 _in y) _in x")->interp(Env::emptyenv)->equals(NEW(NumVal)(7)));
    CHECK(parse_str("_let x = 5 _in _let y = x _in y + y")->interp(Env::emptyenv)->equals(NEW(NumVal)(10)));
    CHECK(parse_str("_let x = 6 _in _let x = 19 _in x")->interp(Env::emptyenv)->equals(NEW(NumVal)(19)));
    CHECK(parse_str("_if 5 == 3 _then 2 _else 89")->interp(Env::emptyenv)->equals(NEW(NumVal)(89)));
    CHECK(parse_str("-8 + 3")->interp(Env::emptyenv)->equals(NEW(NumVal)(-5)));
}

TEST_CASE("function"){
    CHECK(parse_str("_fun (x) x + 1")->equals(NEW(FuncExpr)("x", NEW(AddExpr)( NEW(VarExpr)("x"), NEW(NumExpr)(1)))));
    CHECK(parse_str("_let f = _fun (x) x + 1 _in f(10)")->interp(Env::emptyenv)->equals(NEW(NumVal)(11)));
    CHECK(parse_str("_let y = 8 _in _let f = _fun (x) x * y _in f(2)")->interp(Env::emptyenv)->equals(NEW(NumVal)(16)));
    CHECK(parse_str("(_fun (x) x + 2)(1)")->interp(Env::emptyenv)->equals(NEW(NumVal)(3)));
    CHECK(parse_str("_let f = _fun (x) _fun (y) x*x + y*y _in (f(2))(3)")->interp(Env::emptyenv)->equals(NEW(NumVal)(13)));
    CHECK(parse_str("_let f = _fun (x) _fun (y) x*x + y*y _in f(2)(3)")->interp(Env::emptyenv)->equals(NEW(NumVal)(13)));
    CHECK(parse_str("_let add = _fun (x) _fun (y) x + y _in _let addFive = add(5) _in addFive(10)")->interp(Env::emptyenv)->equals(NEW(NumVal)(15)));
}

TEST_CASE("recursive functions"){
    CHECK( parse_str("_let factrl = _fun(factrl)"
                     "                _fun(x)"
                     "                  _if x == 1"
                     "                  _then 1"
                     "                  _else x * factrl(factrl)(x + -1)"
                     "_in factrl(factrl)(5)")
          ->interp(Env::emptyenv)->to_string() == "120" );
    CHECK( parse_str("_let fib = _fun (fib)"
                     "              _fun (x)"
                     "                 _if x == 0"
                     "                 _then 1"
                     "                 _else _if x == 2 + -1"
                     "                 _then 1"
                     "                 _else fib(fib)(x + -1)"
                     "                       + fib(fib)(x + -2)"
                     "_in fib(fib)(10)")->interp(Env::emptyenv)->to_string() == "89");
}

TEST_CASE("continuation"){
    CHECK( Step::interp_by_steps(parse_str("_let factrl = _fun(factrl)"
                                           "                _fun(x)"
                                           "                  _if x == 1"
                                           "                  _then 1"
                                           "                  _else x * factrl(factrl)(x + -1)"
                                           "_in factrl(factrl)(5)"))
          ->to_string() == "120" );
    CHECK( (Step::interp_by_steps)(parse_str("_let fib = _fun (fib)"
                                           "              _fun (x)"
                                           "                 _if x == 0"
                                           "                 _then 1"
                                           "                 _else _if x == 2 + -1"
                                           "                 _then 1"
                                           "                 _else fib(fib)(x + -1)"
                                           "                       + fib(fib)(x + -2)"
                                           "_in fib(fib)(10)"))->to_string() == "89");
    
    CHECK( Step::interp_by_steps(parse_str("_let countdown = _fun(countdown) _fun(n) _if n == 0 _then 0 _else countdown(countdown)(n + -1) _in countdown(countdown)(1000000)")));
}
