[bits 64]
[extern main]
[extern exit]
[extern _init]
[extern _libd_init]
section .text

global _start
_start:
	;fix end of stack frame
	mov rbp, 0
	push rbp  ;rip - 0
	push rbp  ; rbp=0
	mov rbp, rsp

	; pass args to main
	push rsi
	push rdi

	; init malloc etc 
	call _libd_init

	; global constructors.
	call _init

	; argc and argv.
	pop rdi
	pop rsi

	; enter our elf files "real" entry pt
	call main

	; call exit when main returns with ret val
	mov rdi, rax
	call exit
