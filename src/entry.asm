BITS 32

section .text
	align 4
	dd 0x1BADB002
	dd 0x00000003
	dd -(0x1BADB002 + 0x00000003)

global start
extern kernel_main

start:
	cli
	mov esp, stack_top
	push ebx
	push eax
	call kernel_main
	add esp, 8

HaltKernel:
	cli
	hlt
	jmp HaltKernel

section .bss
align 16
stack_bottom:
    resb 8192
stack_top:
