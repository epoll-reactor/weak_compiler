# This is a compiler created in educational purposes

## What's already been done?

Transformation source text to pseudo-asm representation (feature branch). For example, the code

```
void Function() {
  for (1 == 2; 3 != 4; 5 + 6) {
    7 + 8;
    for (9 == 10; 11 != 12; 13 + 14) {
      15 - 16;
    }
    17 - 18;
  }
  19 - 20;
}
```

transformed to

```
Function:
cmpeq 1, 2
L1:
cmpneq 3, 4
jz L2
add 5, 6
add 7, 8
cmpeq 9, 10
L4:
cmpneq 11, 12
jz L5
add 13, 14
sub 15, 16
jmp L4
L5:
sub 17, 18
jmp L1
L2:
sub 19, 20
```

## What's left
* Code analysis and optimisation based on CFG;
* SSA form;
* target code generation.
