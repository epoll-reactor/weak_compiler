# Compiler

## What's already been done?
* Lexical, syntactic analysis;
* CFG;
* SSA form.

## What's left?
* Code analysis and optimisations;
* interprocedural CFG;
* platform-dependent code generation.

## Simple example
The program
```
void f() {
  int a = 1;
  int b = 2;
  for (int i = 0; i < 10; i = i + 1) {
    do {
      a = 7;
      b = 8;
      while (9 + 10) {
        a = 11;
        b = 12;
      }
    } while (5 + 6);
    a = a + i;
    b = b - i;
  }
  a = b;
  b = a;
}
```
reincarnated to this graph
![alt_text](https://github.com/epoll-reactor/weak_compiler/blob/dev/images/CFG.jpg?raw=true)
