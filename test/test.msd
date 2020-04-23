_let fib = _fun (fib)
              _fun (x)
                 _if x == 0
                 _then 1
                 _else _if x == 2 + -1
                 _then 1
                 _else fib(fib)(x + -1)
                     + fib(fib)(x + -2)_in fib(fib)(10)

