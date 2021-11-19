;	WAVES - The Third Creation - Round #10
;	Written by TyRaNiD


                               TARGET      BIN, R5900

                               ALIGN$$     16
code                           SEGMENT     400000h

ELF_Start:                      DB          7Fh, "ELF"                          ; indentifier
                                DB          1                                   ; File Class: 1=32bit, 2=64bit objects
                                DB          1                                   ; Data Encoding: 1=ELFDATA2LSB, 2=ELFDATA2MSB
                                DB          1                                   ; ELF header version (must be 1)
                                DB          0, 0, 'W', 'A', 'V', 'E', 'S', 0, 0
                                DH          2                                   ; ELF type: 0=NONE, 1=REL, 2=EXEC, 3=SHARED, 4=CORE
                                DH          8                                   ; Processor: 8=MIPS
                                DW          1                                   ; Version: 1=current
                                DW          main	                        ; Entry point address
                                DW          Program_Headers-ELF_Start           ; Start of program headers (offset from file start)
                                DW          0				        ; Start of section headers (offset from file start)
                                DW          20924001h                           ; Processor specific flags = 0x20924001 noreorder, mips
                                DH          52                                  ; ELF header size (0x34 = 52 bytes)
                                DH          20h                                 ; Program headers entry size

Program_Headers:		DW          1                                   ; Segment type:
                                                                                ; 0=Inactive
                                                                                ; 1=Load the segment into memory, no. of bytes specified by 0x10 and 0x14
                                                                                ; 2=Dynamic linking
                                                                                ; 3=Interpreter. The array element must specify a path name
                                                                                ; 4=Note. The array element must specify the location and size of aux. info
                                                                                ; 5=reserved
                                                                                ; 6=The array element must specify location and size of the program header table.
                                                                                ; 0x70000000 - 0x7fffffff processor specific semantics
                                DW          0                                   ; Offset from file start to program segment.
    			        DW          0x400000                            ; Virtual address of the segment
                                DW          0x400000                            ; Physical address of the segment
                                DW          Codeend-ELF_Start                   ; Number of bytes in the file image of the segment
                                DW          Codeend-ELF_Start                   ; Number of bytes in the memory image of the segment
                                DW          0h                                  ; Flags for segment
                                DW          0h                                  ; Alignment. The address of 0x08 and 0x0C must fit this alignment. 0=no alignment

 
; General Rules
; t0 -> t7 hardware base pointers
; s0 -> s7 program variables
; at + a0->3 function registers
; vX temporary storage

; Regsiters used
; s0, s2, s6, s7
; s1 = Value of DISPFB2
; s3 = base for GS Priv regs
; s4 = base for DMA Ch 2
; s5 = base for InitGif

; Variable locations
DISPLAY2	EQU	(_DISPLAY2-InitGif)	; DISPLAY2 location
RASTORE		EQU	(_RASTORE-InitGif)	; Double up to provide store for ra
RECTCOLOUR	EQU	(_RectColour-InitGif)	; Rectangle colour
RECTTXY		EQU	(_RectTXY-InitGif)	; Rectangle x,y
RECTBXY		EQU	(_RectBXY-InitGif)	; Rectangle bottom x,y
INITGIFSZ	EQU	((End_InitGif-InitGif) >> 4)
DRAWRECT	EQU	(DrawRect-InitGif)	
DRAWRECTSZ	EQU	((End_DrawRect-DrawRect) >> 4)
SINUSCOUNT	EQU	(SinusCount-InitGif)

BGCOLOUR	EQU	0x00ffffff


main				PROC
				lui	s3, 0x1200			; s3 is base for GS priv regs
				lui	s4, 0x1000			
				ori	s4, s4, 0xA000			; s4 is base for DMA 2
				lui	s5, (InitGif >> 16)		; s5 is location of InitGif
				ori	s5, s5, (InitGif & 0xFFFF)

				ori	v0, zero, (1 << 9)
				sd	v0, 0x1000(s3)		; Reset GS
				sync.p
				nop

				ori         a0, a0, 0xFF00  ; set imr
		                ld          v0, 0x1000(s3)
                		dsrl        v0, v0, 16
		                andi        v0, v0, 0xFF
		                ori         v1, zero, 0x71
		                syscall

;				lui	at, 0x1FC8 		; Get PAL/NTSC
;				lbu	t0, -0xAE(at)		; Load machine area
;				ori	t1, zero, 'E'		; Load E
				ori	a1, zero, 2
;				beql    t0, t1, Pal
;				addi	a1, a1, 1		; Set a1 to PAL
;Pal:
				
				ori	a0, zero, 1
				ori	a2, zero, 0	
				ori 	v1, zero, 0x02				
				syscall				; call SetGsCrt
				;;;;;;;;;;;;;;;;;;;;;;;;;

								; Init other GS Priv regs
				ori	v0, zero, 0xFF46	; Set PMODE
				sd	v0, 0x0(s3)		; ""
				ori	s1, zero, 0x1000	; Set DISPFB2
					
				ld	v0, DISPLAY2(s5)	; Set DISPLAY2
				sd 	v0, 0xA0(s3)		; ""

				move 	a1, s5			; Set a1 to initgif data
				jal	dmasend
				ori	a2, zero, INITGIFSZ	; Size of initgif / 8				

				ori	s6, zero, 200

;;;; Initialise the hsync handler

				ori	a0, zero, 10
				lui	a1, (hsync_inth >> 16)
				ori	a1, a1, (hsync_inth & 0xFFFF)
				move	a2, zero
				move 	a3, zero
				ori	v1, zero, 0x10		
				syscall				; Install hsync interrupt

				ori	a0, zero, 10
				ori	v1, zero, 0x14
				syscall				; Enable timer 0 intc

				lui	t1, 0x1000		; Load 0x1000 0000 to t1
				sw	zero, 0x810(t1)
				ori	t2, zero, 1		; 
				sw 	t2, 0x820(t1)		; Set T0_COMPARE to 1
				sw	zero, 0x800(t1)		; Clear the count
				ori	t2, zero, 0xD83		;
				sw	t2, 0x810(t1)		; Setup timer and start

			
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
								; Initialise variables
_demo_loop:                
				lui	v0, (BGCOLOUR >> 16)
				ori	v0, v0, (BGCOLOUR & 0xFFFF)
				sw	v0, RECTCOLOUR(s5)
				ori	v0, zero, (1024 << 4)		; Clear screen
				ori	v1, zero, (1024 << 4)
				ori	a0, zero, ((1024 + 512) << 4)
				jal	drawrect
				ori	a1, zero, ((1024 + 512) << 4)

				ori 	s2, zero, 6			; s2 y counter
_checkboard_y:
				ori	s0, zero, 7			; s0 x counter
_checkboard_x:
				lui	t0, 0xFF
				sw	t0, RECTCOLOUR(s5)
				sll	v0, s0, 11
				andi	t0, s2, 1
				bgtzl	t0, _check_offset
				addi	v0, v0, 0x400
_check_offset:
				addi	v0, v0, 0x4000
				sll	v1, s2, 10
				addi	v1, v1, 0x4000
				addi	a0, v0, 0x400
				jal	drawrect
				addi	a1, v1, 0x400

				bgtz	s0, _checkboard_x
				addi	s0, s0, -1

				bgtz	s2, _checkboard_y
				addi	s2, s2, -1

;				sw	zero, RECTCOLOUR(s5) 
;				ori	v0, zero, ((1024 + 100) << 4)
;				ori	v1, zero, ((1024 + 100) << 4)
;				ori	a0, zero, ((1024 + 200) << 4)
;				jal	drawrect
;				ori	a1, zero, ((1024 + 200) << 4)

				ld	v0, 0x1000(s3)		; load csr
				andi	v0, v0, 8		; set bit 3
				sd	v0, 0x1000(s3)		; store back in csr
_vsync:				
				ld	v0, 0x1000(s3)		; load
				andi	v0, v0, 0x8		
				beq	v0, zero, _vsync	; if bit 0 then wait
				move 	a1, s5			; Set a1 to initgif data
				
				xori	s1, s1, 0x80
				sd	s1, 0x90(s3)		; Write to DISPFB2			

				ld	v0, (FRAME_1-InitGif)(s5) ; Toggle FRAME_1 location
				xori	v0, v0, 0x80
				sw	v0, (FRAME_1-InitGif)(s5)
				
				jal	dmasend
				ori	a2, zero, ((End_InitGif-InitGif) >> 4)	; Size of initgif / 16

;				bgtz	s6, _dump
;				lw	t0, SINUSCOUNT(s5)
;				sd	zero, 0(zero)
;_dump:
;				addi	s6, s6, -1

				j 	_demo_loop
				nop
main				ENDP

; _dmasend a1 = source, a2 qwc
dmasend				PROC
				

waitdma:
	                        lw          v0, 0(s4)
                                andi        v0, v0, 0x100
                                bnez        v0, waitdma		; Wait for DMA channel
                                add	    a0, zero, zero	
				ori 	    v1, zero, 0x64	; Call FlushCache
				syscall

                                sw          a2, 0x20(s4)	; Set QWC
                                sw          a1, 0x10(s4)	; Set source to MADR
                                addiu       v0, zero, 0x101	; Start DMA
								
                                jr          ra
				sw          v0, 0x0(s4)
dmasend				ENDP

drawrect			PROC
				sd	ra, RASTORE(s5)		; Store return address
				sh	v0, RECTTXY(s5)
				sh	v1, (RECTTXY+2)(s5)
				sh	a0, RECTBXY(s5)
				sh	a1, (RECTBXY+2)(s5)

				addiu 	a1, s5, DRAWRECT ; Set a1 to drawpoint data
				jal	dmasend
				ori	a2, zero, DRAWRECTSZ	; Size of initgif / 8
				ld	ra, RASTORE(s5)		; Recover return address
				jr 	ra
				nop
drawrect			ENDP

hsync_inth			PROC
				lui	t0, (SinusCount >> 16)
				ori	t0, t0, (SinusCount & 0xFFFF)
				lwu	t1, 0(t0)

				lui	t4, (Sinus >> 16)
				ori	t4, t4, (Sinus & 0xFFFF)

				srl	t6, t1, 1
				andi	t6, t6, 0x7F
				add	t4, t4, t6
				lb	t5, 0(t4)

				lui	t2, (_DISPLAY2 >> 16)
				ori	t2, t2, (_DISPLAY2 & 0xFFFF)
				ld	t3, 0(t2)

				dadd	t3, t3, t5

				lui	at, 0x1200
				sw	t3, 0xA0(at)

				addi	t1, t1, 1
				sw	t1, 0(t0)

				lui	t1, 0x1000		; Load 0x1000 0000 to t1
				sw	zero, 0x810(t1)
				ori	t2, zero, 1		; 
				sw 	t2, 0x820(t1)		; Set T0_COMPARE to 1
				sw	zero, 0x800(t1)		; Clear the count
				ori	t2, zero, 0xD83		;
				sw	t2, 0x810(t1)		; Setup timer and start

				jr ra
				nop
hsync_inth			ENDP

				ALIGN$$ 16
DataStart:
InitGif:			DW	8005h		; NLOOP of 5 
				DW	10000000h	; 1 Registers, Packed mode 
				DW	0Eh		; A+D register 
				DW	0h	
				DW	1h		; PRMODECONT
				DW	0h		
				DW	1Ah		
				DW	0h		
FRAME_1:			DW	080000h		; FRAME_1
				DW	0h		
				DW	4Ch		
				DW	0		
				DW	4000h		; XYOFFSET_1
				DW	4000h		 
				DW	18h		
				DW	0h		
				DW	1FF0000h	; SCISSOR_1
				DW	1FF0000h	
				DW	40h
				DW	0h
				DW	00FFFh		; TEST_1
				DW	0h
				DW	47h
				DW	0h
End_InitGif:
DrawRect:			DW	8001h		; GIFTag
				DW	44000000h
				DW	5410h
				DW	0h
				DW	6h		; PRIM
				DW	0h
_RectColour:			DW	023000h		; RGBAQ
				DW	0h
_RectTXY:			DW	40004000h	; XYZ2
				DW	0h		
_RectBXY:			DW	60006000h	; XYZ2
				DW	0h		
End_DrawRect:

_DISPLAY2:			DW	0x02032290
				DW	0x001BF9FF
_RASTORE:			DW	0x0
				DW	0x0

SinusCount:			DW	0x0

Sinus:
				INCLUDE "sinus.asm"

Codeend:

code                            ENDS

                                END
