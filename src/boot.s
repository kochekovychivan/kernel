
.section .multiboot
.align 4
.long 0x1BADB002
.long ((1<<0) | (1<<1))
.long -(0x1BADB002 + ((1<<0) | (1<<1)))

.section .bss
.align 16
stack_bottom:
.skip 16384
stack_top:

.section .text
.global start
.type start, @function
start:
	mov $stack_top, %esp
	push %ebx
	push %eax
	call kernel_main	
	cli
1:	hlt
	jmp 1b

.size start, . - start
