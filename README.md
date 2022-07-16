[![Documentation](https://img.shields.io/badge/docs-doxygen-blue.svg)](https://epoll-reactor.github.io/weak_compiler/index.html)

# Compiler

This is an implementation of simple (or not so simple, XD) compiler,
which uses LLVM.

## TODO
* Complete code generation;
* graph-based optimizations;
* meaningful warnings and error messages;
* API for external utilities (such as static analysis, code formatting tools).

## Example

The program
```c
int main() {
    int result = 0;
    for (int i = 0; i < 1000; ++i) {
        --result;
    }
    return result;
}
```
compiled to LLVM IR
```asm
define i32 @main() {
entry:
  %result = alloca i32, align 4
  store i32 0, i32* %result, align 4
  %i = alloca i32, align 4
  store i32 0, i32* %i, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.body, %entry
  %i1 = load i32, i32* %i, align 4
  %0 = icmp slt i32 %i1, 1000
  br i1 %0, label %for.body, label %for.end

for.body:                                         ; preds = %for.cond
  %result2 = load i32, i32* %result, align 4
  %1 = sub i32 %result2, 1
  store i32 %1, i32* %result, align 4
  %i3 = load i32, i32* %i, align 4
  %2 = add i32 %i3, 1
  store i32 %2, i32* %i, align 4
  br label %for.cond

for.end:                                          ; preds = %for.cond
  %result4 = load i32, i32* %result, align 4
  ret i32 %result4
}
```
and next can be compiled to target assembly
```asm
    .text
    .file     "ir.ll"
    .globl    main                      # -- Begin function main
    .p2align  4, 0x90
    .type     main,@function
main:                                   # @main
    .cfi_startproc
# %bb.0:                                # %entry
    movl      $0, -4(%rsp)
    movl      $0, -8(%rsp)
    cmpl      $999, -8(%rsp)            # imm = 0x3E7
    jg        .LBB0_3
    .p2align  4, 0x90
.LBB0_2:                                # %for.body
                                        # =>This Inner Loop Header: Depth=1
    decl      -4(%rsp)
    incl      -8(%rsp)
    cmpl      $999, -8(%rsp)            # imm = 0x3E7
    jle       .LBB0_2
.LBB0_3:                                # %for.end
    movl      -4(%rsp), %eax
    retq
.Lfunc_end0:
    .size     main, .Lfunc_end0-main
    .cfi_endproc
                                        # -- End function
    .section  ".note.GNU-stack","",@progbits
```
which successfully runs as executable file
```
$ strace ./binary
...
brk(NULL)                               = 0x55aa1373a000
brk(0x55aa1375b000)                     = 0x55aa1375b000
exit_group(-1000)                       = ?
+++ exited with 24 +++
```