	.file	"race-condition.c"
	.text
	.globl	mails
	.bss
	.align 4
	.type	mails, @object
	.size	mails, 4
mails:
	.zero	4
	.text
	.globl	routine
	.type	routine, @function
routine:
.LFB6:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	$0, -4(%rbp)
	jmp	.L2
.L3:
	movl	mails(%rip), %eax
	addl	$1, %eax
	movl	%eax, mails(%rip)
	addl	$1, -4(%rbp)
.L2:
	cmpl	$999999, -4(%rbp)
	jle	.L3
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
	.size	routine, .-routine
	.section	.rodata
.LC0:
	.string	"Number of mails: %d\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB7:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	%edi, -20(%rbp)
	movq	%rsi, -32(%rbp)
	leaq	-8(%rbp), %rax
	movl	$0, %ecx
	leaq	routine(%rip), %rdx
	movl	$0, %esi
	movq	%rax, %rdi
	call	pthread_create@PLT
	testl	%eax, %eax
	je	.L5
	movl	$1, %eax
	jmp	.L10
.L5:
	leaq	-16(%rbp), %rax
	movl	$0, %ecx
	leaq	routine(%rip), %rdx
	movl	$0, %esi
	movq	%rax, %rdi
	call	pthread_create@PLT
	testl	%eax, %eax
	je	.L7
	movl	$2, %eax
	jmp	.L10
.L7:
	movq	-8(%rbp), %rax
	movl	$0, %esi
	movq	%rax, %rdi
	call	pthread_join@PLT
	testl	%eax, %eax
	je	.L8
	movl	$3, %eax
	jmp	.L10
.L8:
	movq	-16(%rbp), %rax
	movl	$0, %esi
	movq	%rax, %rdi
	call	pthread_join@PLT
	testl	%eax, %eax
	je	.L9
	movl	$4, %eax
	jmp	.L10
.L9:
	movl	mails(%rip), %eax
	movl	%eax, %esi
	leaq	.LC0(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$0, %eax
.L10:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7:
	.size	main, .-main
	.ident	"GCC: (Debian 12.2.0-14) 12.2.0"
	.section	.note.GNU-stack,"",@progbits
