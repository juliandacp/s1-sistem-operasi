; kernel.asm

global _putInMemory
global _interrupt

; void putInMemory(int segment, int address, char character)
_putInMemory:
	push bp
	mov bp,sp
	push ds
	mov ax,[bp+4]
	mov si,[bp+6]
	mov cl,[bp+8]
	mov ds,ax
	mov [si],cl
	pop ds
	pop bp
	ret

; int interrupt(int number, int AX, int BX, int CX, int DX)
_interrupt:
	push bp
	mov bp,sp
	mov ax,[bp+4]
	push ds
	mov bx,cs
	mov ds,bx
	mov si,intr
	mov [si+1],al
	pop ds
	mov ax,[bp+6]
	mov bx,[bp+8]
	mov cx,[bp+10]
	mov dx,[bp+12]

intr:	int 0x00

	mov ah,0
	pop bp
	ret

global _getRTCTime
global _getRTCDate
global _getTickCount

; void getRTCTime(byte* hours, byte* minutes, byte* seconds)
; https://www.ctyme.com/intr/rb-2273.htm
_getRTCTime:
	push bp
	mov bp, sp
	mov ah, 0x02
	int 0x1A
	mov si, [bp+4]
	mov [si], ch
	mov si, [bp+6]
	mov [si], cl
	mov si, [bp+8]
	mov [si], dh
	pop bp
	ret

; void getRTCDate(byte* century, byte* year, byte* month, byte* day)
; https://www.ctyme.com/intr/rb-2277.htm
_getRTCDate:
	push bp
	mov bp, sp
	mov ah, 0x04
	int 0x1A
	mov si, [bp+4]
	mov [si], ch
	mov si, [bp+6]
	mov [si], cl
	mov si, [bp+8]
	mov [si], dh
	mov si, [bp+10]
	mov [si], dl
	pop bp
	ret

; int getTickCount(void)
; https://www.ctyme.com/intr/rb-2271.htm
_getTickCount:
	push bp
	mov bp, sp
	mov ah, 0x00
	int 0x1A
	mov ax, dx
	pop bp
	ret

; void setVGAPalette(byte* palette)
; Reference: https://www.ctyme.com/intr/rb-0135.htm
global _setVGAPalette
_setVGAPalette:
	push bp
	mov bp, sp
	push es
	mov ax, ds
	mov es, ax
	mov bx, 0
	mov cx, 256
	mov dx, [bp+4]
	mov ax, 0x1012
	int 0x10
	pop es
	pop bp
	ret

