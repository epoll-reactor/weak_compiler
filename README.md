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
int fact(int n) {
    if ((n == 0) || (n == 1)) {
        return 1;
    }
    return n * fact(n - 1);
}

int main() {
    return fact(5);
}
```
compiled to LLVM IR
```asm
define i32 @fact(i32 %n) {
entry:
  %0 = icmp eq i32 %n, 0
  %1 = icmp eq i32 %n, 1
  %2 = select i1 %0, i1 true, i1 %1
  %condition = icmp ne i1 %2, false
  br i1 %condition, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret i32 1
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %4 = sub i32 %n, 1
  %5 = call i32 @fact(i32 %4)
  %6 = mul i32 %n, %5
  ret i32 %6
}

define i32 @main() {
entry:
  %0 = call i32 @fact(i32 5)
  ret i32 %0
}

```
and next can be compiled to target assembly
```asm
	.text
	.file	"factorial.ll"
	.globl	fact                    # -- Begin function fact
	.p2align	4, 0x90
	.type	fact,@function
fact:                                   # @fact
	.cfi_startproc
# %bb.0:                                # %entry
	testl	$-2, %edi
	jne	.LBB0_2
# %bb.1:                                # %if.then
	movl	$1, %eax
	retq
.LBB0_2:                                # %if.end
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset %rbx, -16
	movl	%edi, %ebx
	leal	-1(%rbx), %edi
	callq	fact@PLT
	imull	%ebx, %eax
	popq	%rbx
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end0:
	.size	fact, .Lfunc_end0-fact
	.cfi_endproc
                                        # -- End function
	.globl	main                    # -- Begin function main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:                                # %entry
	pushq	%rax
	.cfi_def_cfa_offset 16
	movl	$5, %edi
	callq	fact@PLT
	popq	%rcx
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end1:
	.size	main, .Lfunc_end1-main
	.cfi_endproc
                                        # -- End function
	.section	".note.GNU-stack","",@progbits
```
which successfully runs as executable file
```
$ strace ./binary
...
brk(NULL)                               = 0x55e086f32000
brk(0x55e086f53000)                     = 0x55e086f53000
exit_group(120)                         = ?
+++ exited with 120 +++
```
