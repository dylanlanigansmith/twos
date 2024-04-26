[bits 64]
[extern main]
[extern exit]
[extern _init]
[extern _libd_init]
section .text

global _start
_start:
	; Set up end of the stack frame linked list.
	mov rbp, 0
	push rbp  ;rip - 0
	push rbp  ; rbp=0
	mov rbp, rsp

	; We need those in a moment when we call main.
	push rsi
	push rdi

	; Prepare signals, memory allocation, stdio and such.
	call _libd_init

	; Run the global constructors.
	call _init

	; Restore argc and argv.
	pop rdi
	pop rsi

	; Run main
	call main

	; Terminate the process with the exit code.
	mov rdi, rax
	call exit
