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

Expr *addend(std::istream &in);
Expr *number_or_paren(std::istream &in);
Expr *parse_number(std::istream &in);
Expr *parse_variable(std::istream &in);
Expr *let(std::istream &in);
std::string parse_keyword(std::istream &in);
std::string parse_alphabetic(std::istream &in, std::string prefix);

/**
 Return an expression (Expr *) accroding to grammar:
 <expr> = <addend>
 | <addend> + <expr>
 */
Expr *expr(std::istream &in) {
    Expr *num = addend(in);
    if(num == nullptr) return nullptr;
    char c = peek_next(in);
    if (c == '+') {
        get_next(in);
        Expr *temp = expr(in);
        if(temp == nullptr) return nullptr;
        return new AddExpr(num, temp);
    }
    return num;
}

/**
 Return an expresion according to grammar:
 <addend> = <number-or-paren>
 | <number-or-paren> * <addend>
 */
Expr *addend(std::istream &in) {
    Expr *num = number_or_paren(in);
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
 Return an expression according to grammar:
 <number-or-paren> = <number>
 | (<expr>)
 | <variable>
 */
Expr *number_or_paren(std::istream &in){
    char c = peek_next(in);
    if (c == '(') { // if is a parenthesis
        get_next(in);
        Expr *num = expr(in);
        if(num == nullptr || peek_next(in) != ')') return nullptr;
        get_next(in);
        return num;
    } else if (isdigit(c)) { // if is a number
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
        else
            throw std::runtime_error((std::string)"unexpected keyword " + keyword);
    } else
        return nullptr;
}

Expr *let(std::istream &in){
    VarExpr *ve = dynamic_cast<VarExpr*>(number_or_paren(in));
    char c = peek_next(in);
    if(c != '=')
        throw std::runtime_error((std::string)"Should have = keyword");
    in.get();
    Expr *fe = expr(in);
    std::string in_string = parse_keyword(in);
    if(in_string != "_in")
        throw std::runtime_error((std::string)"Should have _in keyword");
    Expr *se = expr(in);
    se = se->subst(ve->name, fe->value());
    return new LetExpr(ve, fe, se);
}


// Parses a number, assuming that `in` starts with a digit.
Expr *parse_number(std::istream &in) {
    int num = 0;
    in >> num;
    return new NumExpr(num);
}

// Parses an expression, assuming that `in` starts with a
// letter.
Expr *parse_variable(std::istream &in) {
    return new VarExpr(parse_alphabetic(in, ""));
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

TEST_CASE( "equals" ) {
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

