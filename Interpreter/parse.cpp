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

Expr *parse_if(std::istream &in);
Expr *comparg(std::istream &in);
Expr *addend(std::istream &in);
Expr *multicand(std::istream &in);
Expr *inner(std::istream &in);
Expr *parse_number(std::istream &in);
Expr *parse_variable(std::istream &in);
Expr *let(std::istream &in);
Expr *parse_fun(std::istream &in);
std::string parse_keyword(std::istream &in);
std::string parse_alphabetic(std::istream &in, std::string prefix);

/**
 Return an expression (Expr *) accroding to grammar:
 <expr> = <comparg>
        | <comparg> + <expr>
 */
Expr *expr(std::istream &in) {
    Expr *num = comparg(in);
    if(num == nullptr) return nullptr;
    char c = peek_next(in);
    if (c == '=') {
        get_next(in);
        if(get_next(in) != '=') throw std::runtime_error((std::string)"should be double equal");
        Expr *temp = expr(in);
        if(temp == nullptr) return nullptr;
        return new EquExpr(num, temp);
    }
    return num;
}

/**
 <conparg> = <addend>
           | <addend> + <comparg>
 */
Expr *comparg(std::istream &in){
    Expr *add = addend(in);
    if(add == nullptr) return nullptr;
    char c = peek_next(in);
    if(c == '+'){
        get_next(in);
        Expr *comp = comparg(in);
        if(comp == nullptr) return nullptr;
        return new AddExpr(add, comp);
    }
    return add;
}

/**
 Return an expresion according to grammar:
 <addend> = <multicand>
          | <multicand> * <addend>
 */
Expr *addend(std::istream &in) {
    Expr *num = multicand(in);
    if(num == nullptr) return nullptr;
    char c = peek_next(in);
    if (c == '*') {
        get_next(in);
        Expr *temp = addend(in);
        if(temp == nullptr) return nullptr;
        return new MultExpr(num, temp);
    }
    return num;
}

/**
 <multicand> = <inner>
             | <multicand> ( <expr> )
 */
Expr *multicand(std::istream &in) {
    Expr *temp = inner(in);
    while (peek_next(in) == '(') {
        Expr *actual_arg = expr(in);
        temp = new CallExpr(temp, actual_arg);
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
Expr *inner(std::istream &in){
    char c = peek_next(in);
    if (c == '(') { // if is a parenthesis
        get_next(in);
        Expr *num = expr(in);
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
            return new BoolExpr(true);
        else if (keyword == "_false")
            return new BoolExpr(false);
        else if (keyword == "_let")
            return let(in);
        else if (keyword == "_if")
            return parse_if(in);
        else if (keyword == "_fun")
            return parse_fun(in);
        else
            throw std::runtime_error((std::string)"unexpected keyword " + keyword);
    } else
        return nullptr;
}

Expr *parse_if(std::istream &in){
    Expr *test_part = expr(in);
    if(parse_keyword(in) != "_then") throw std::runtime_error((std::string)"unexpected keyword");
    Expr *then_part = expr(in);
    if(parse_keyword(in) != "_else") throw std::runtime_error((std::string)"unexpected keyword");
    Expr *else_part = expr(in);
    return new IfExpr(test_part, then_part, else_part);
}

Expr *let(std::istream &in){
    VarExpr *ve = dynamic_cast<VarExpr*>(inner(in));
    char c = peek_next(in);
    if(c != '=')
        throw std::runtime_error((std::string)"Should have = keyword");
    in.get();
    Expr *fe = expr(in);
    std::string in_string = parse_keyword(in);
    if(in_string != "_in")
        throw std::runtime_error((std::string)"Should have _in keyword");
    Expr *se = expr(in);
    return new LetExpr(ve->name, fe, se);
}


// Parses a number, assuming that `in` starts with a digit.
Expr *parse_number(std::istream &in) {
    char c = peek_next(in);
    int flag = 1;
    if(c == '-'){
        flag = -1;
        get_next(in);
    }
    if(!isdigit(peek_next(in))) throw std::runtime_error((std::string)"Unexpected number");
    int num = 0;
    in >> num;
    return new NumExpr(flag * num);
}

// Parses an expression, assuming that `in` starts with a
// letter.
Expr *parse_variable(std::istream &in) {
    return new VarExpr(parse_alphabetic(in, ""));
}

// Parse a function
Expr *parse_fun(std::istream &in){
    char c = peek_next(in);
    if(c == '('){
        get_next(in);
        peek_next(in);
        std::string formal_var = parse_alphabetic(in, "");
        c = peek_next(in);
        if(c != ')')
            throw std::runtime_error((std::string)"not a function format");
        get_next(in);
        Expr *body = expr(in);
        return new FuncExpr(formal_var, body);
    } else
        throw std::runtime_error((std::string)"not a function format");
}

// Parses an expression, assuming that `in` starts with a
// letter.
std::string parse_keyword(std::istream &in) {
    in.get(); // consume `_`
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
Expr *parse(std::istream &in){
    return expr(in);
}

/**
 Convert the string into an istream and all parse to read the stream and
 parse the string into a Expr object
 */
Expr *parse_str(std::string s){
    std::istringstream in(s);
    return parse(in);
}


TEST_CASE( "parse" ) {
    CHECK((new NumExpr(1))->equals(new NumExpr(1)));
    CHECK(!(new NumExpr(1))->equals(new NumExpr(2)));
    CHECK(!(new NumExpr(1))->equals(new MultExpr(new NumExpr(2), new NumExpr(4))));
    CHECK((new VarExpr("hello"))->equals(new VarExpr("hello")));
    CHECK(!(new VarExpr("hello"))->equals(new VarExpr("ello")));
    CHECK(peek_next(*(new std::istringstream("hello"))) == 'h');
    CHECK(peek_next(*(new std::istringstream("   hello"))) == 'h');
    CHECK(get_next(*(new std::istringstream("   hello"))) == 'h');
    CHECK(parse_str("1")->equals(new NumExpr(1)));
    CHECK(parse_str("  1")->equals(new NumExpr(1)));
    CHECK(parse_str("4+2")->equals(new AddExpr(new NumExpr(4), new NumExpr(2))));
    CHECK(parse_str("   45 +    23")->equals(new AddExpr(new NumExpr(45), new NumExpr(23))));
    CHECK(parse_str("   45   *  23   ")->equals(new MultExpr(new NumExpr(45), new NumExpr(23))));
    CHECK(parse_str("(90)")->equals(new NumExpr(90)));
    CHECK(parse_str("3*(2+43)")->equals(new MultExpr(new NumExpr(3),new AddExpr(new NumExpr(2), new NumExpr(43)))));
    CHECK(parse_str("Hello")->equals(new VarExpr("Hello")));
    CHECK(parse_str("3*(2+width)")->equals(new MultExpr(new NumExpr(3),new AddExpr(new NumExpr(2), new VarExpr("width")))));
}

TEST_CASE("optimize"){
    CHECK(parse_str("_let x = 5 _in _let y = z + 2 _in x + y + (2 * 3)")->optimize()->equals(parse_str("_let y = z + 2 _in 5 + y + 6")));
    CHECK(parse_str("_let x = (_let y = 7 _in y) _in x")->optimize()->equals(new NumExpr(7)));
    CHECK(parse_str("_let x = 5 _in _let y = x _in y + y")->optimize()->equals(new NumExpr(10)));
    CHECK(parse_str("_let x = 5 _in _let x = 12 _in x + y")->optimize()->equals(parse_str("12 + y")));
    CHECK(parse_str("_if 5 == x _then x + 7 _else 67")->optimize()->equals(parse_str("_if 5 == x _then x + 7 _else 67")));
}

TEST_CASE("interpreter"){
    CHECK_THROWS_WITH(parse_str("_let x = 5 _in _let y = z + 2 _in x + y + (2 * 3)")->interp(), "cannot interpret a variable");
    CHECK(parse_str("_let x = (_let y = 7 _in y) _in x")->interp()->equals(new NumVal(7)));
    CHECK(parse_str("_let x = 5 _in _let y = x _in y + y")->interp()->equals(new NumVal(10)));
    CHECK(parse_str("_let x = 6 _in _let x = 19 _in x")->interp()->equals(new NumVal(19)));
    CHECK(parse_str("_if 5 == 3 _then 2 _else 89")->interp()->equals(new NumVal(89)));
    CHECK(parse_str("-8 + 3")->interp()->equals(new NumVal(-5)));
}

TEST_CASE("function"){
    CHECK(parse_str("_fun (x) x + 1")->equals(new FuncExpr("x", new AddExpr( new VarExpr("x"), new NumExpr(1)))));
    CHECK(parse_str("_let f = _fun (x) x + 1 _in f(10)")->interp()->equals(new NumVal(11)));
}
