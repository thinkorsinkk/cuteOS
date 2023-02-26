;extern kstart

;global _start

;section .bss
	;stack: resb 0x2000

;section .text
;_start:
	;lea rsp, [stack + 0x2000]
	;jmp kstart