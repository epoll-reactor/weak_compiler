# This is a compiler created in educational purposes

## What's already been done?
* Lexical, syntactic analysis, building of CFG;
* IR;

## What's left
* Code analysis and optimisation;
* SSA form;
* platform-dependent code generation.

## Simple example
The program
```
void f() {
  for (int i = 0; i < 10; ++i) {
    int a = 1 + 2;
    continue;
    int b = 3 + 4;
  }
  int c = 5 + 6;
}
```
can be transformed to
```
         t0 4 bytes  =   0
L0:
         if             t0 < 10 goto L1
         goto L2
L1:
         t1 4 bytes  =   1 +  2
         goto L0
         t2 4 bytes  =   3 +  4
         t0 4 bytes  =  t0 +  1
         goto L0
L2:
         t4 4 bytes  =   5 +  6
```