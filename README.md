# Interpreter-MSDScript
A basic interpreter for a new programming language-MSDScript. 

* The Syntax of the MSDScript is like this:  
![](img/1.png)

* Functions of MSDScript include: arithmetic/boolean parser, variable binding, functions, explicit continuations.

* Grammar: 
![](img/2.png)

* Parsing Expression:
![](img/3.png)


Working log:
* History progress:
    1. Write a *parser* for arithmethic that supports  
        * numbers (non-empty sequence of digits 0-9)
         * plus
         * times
         * parenthesized expressions
         * variables (non-empty sequence of letters a-zA-Z)
    2. Add a method to `Expr` to to report whether the expression contains any variables. For example, `1+x` contains a variable,
      but `1+2` does not.
    3. Add a method to `Expr` to compute the value of an expression assuming that each variable's value is 0.


* Jan 20, 2020   
    1. Separate tests from the main
    2.  Make parser allow whitespace, and add a way to read an expression that is terminated with an end-of-file. Make a program that reads such an expression from terminal input and print its value followed by a newline, with nothing (including whitespace) before the value. The following shows an example.  
            Input:
        
        ```
        (3+4 + x)
            * 5
        ```

        Output:

        ```
        35
        ```

        > Note: to create an end-of-file in a terminal, hit Ctl-D on a fresh line.
    3. Add optimizer to simply the entered expression
    4. Add a `_let` \<*variable*\> `=` \<*expr*\> `_in` \<*expr*\> expression form
      to parser and expression, where the value of a `_let` form
      is the value of the second \<*expr*\> with each \<*variable*\> in that \<*expr*\>
      replaced by the value of the first \<*expr*\>. For example,

       ```
       _let x = 5
       _in x + x
       ```

* Jan 25, 2020   
    1. Keep working the optimizer program that simplifies an expression (possibly nested in other expression) when it the expression doesn't have a variable. It should substitute a `_let`-bound variable if and only if the variable can be replaced by a number, and then the `_let` should go away. The following example shows input and a possible output, although your printed might choose different whitespace or parenthesization for the output, as long as the output fed back into your simplifier would produce the same output.  
        Input:

        ```
        _let x = 5
        _in _let y = z + 2
        _in x + y + (2 * 3)
        ```

        Output:

        ```
        _let y = z + 2 _in 5 + y + 6
        ```

* Jan 30, 2020
    1. Add support for booleans and comparisons to interpreter andoptimizer. That includes `_true`, `_false`, `_if`, `==`, and converting interpreter to work with number and boolean values instead of just number values. Also support negative numbers, and make attempting to get the value of a variable (one that hasn't been replaced) report an error instead of 0.
    2. Change value method to interpreter, simplify method to optimizer. Make optimizer logic more robust.
    3. Try to add random tests

* Feb 5, 2020
    1. Improve the performance of the interpreter and fix bugs.
