;**********************************************
RUNFILE: ;std
  CALL PRINT_NEWLINE
  LXI H,LINEBUF
  MVI C,'.'
  CALL STRCHR
  JNC RUNF1
  LXI D,DOTRK
  CALL STRCPY
RUNF1:  LXI D,LINEBUF
  CALL F_OPEN
  JC C_DIRNO ; no file
RUNF2:
  LXI B,3 ; читаю из TAP файла
  LXI H,LINEBUF
  CALL F_READ
  LXI B,10 ; читаю из TAP файла
  LXI H,LINEBUF+3
  CALL F_READ
  LXI H,LINEBUF+3
  LXI B,4
  push H
  CALL F_READ
  pop H
  MOV C,M
  INX H
  MOV B,M
  INX B  ;размер файла
  INX H
  MOV E,M
  INX H
  MOV D,M
  XCHG
  PUSH H ;сохраняем адрес старта, а ret после F_READ сделает переход на этот адрес
  push B
  push B
  mov A,H
  call PRHEX
  mov A,L
  call PRHEX
  MVI C,','
  CALL PUTC
  mov A,B
  call PRHEX
  POP B
  mov A,C
  call PRHEX
  push H
  LXI H,LINEBUF+3
  LXI B,02h
  CALL F_READ
  LXI H,03c31h
  LXI B,08h
  CALL F_READ
  LXI H,LINEBUF+2+3
  LXI B,03h
  CALL F_READ

  pop H
  POP B
  CALL F_READ
  LXI H,LINEBUF+2
  mov A,M
  CPI 20H
  JZ RUNF3
  LXI H,LINEBUF+3
  mov A,M
  mov E,A
  INX H
  mov A,M
  mov D,A
  LXI H,3c4ch
  mov A,E
  mov M,A
  INX H
  mov A,D
  mov M,A
  POP H
  jmp C_X
RUNF3:
  CALL SD_OFF

;  POP H
;  JMP 4000H 
  ret

GETC:   ; get keys 
 push H
get1:
 lxi H,03c27h
 mov A,M
 cpi 01eh
 jnz get1
get2:
 mov A,M
 cpi 01eh
 jz get2
 lxi H,03c26h
 mov A,M
 cpi 00h
 jz get1
 pop H
 ret

PUTC:   ; prints a character in C to the screen 
 mov A,C
 call 08h
 ret

PRINT:    ; prints a string pointed by HL
; push PSW
; push B
PR_LOOP:
 mov A,M
 inx H
 ora A
 JZ PREXT
 push H
 call 08h
 pop H
 jmp PR_LOOP
PREXT:
; pop B
; pop PSW
 ret

;prints a 16-bit number pointed
;by DE to the screen at the current cursor location.
PRHEX:
;#ifdef RWR ;!!!!!!!!!!!!!!!!!!!!!!!!!!!
 push PSW
 RAR
 RAR
 RAR
 RAR
 ani 0FH
 sui 0AH
 jm  prh1
 adi 07H
prh1:
 adi 3AH
 call 08h
 pop  PSW
 ani 0FH
 sui 0AH
 jm  prh2
 adi 07H
prh2:
 adi 3AH
 call 08h
;#endif ;!!!!!!!!!!!!!!!!!!!!!!!!!!!
 ret

IfKeyPress:
; push H
; lxi H,02000h+31 ; test if SPACE is pressed
; mov A,M
; push PSW
; pop PSW
; pop H
; ret
  push H
  push B
  push D
IfKE:
  call 0336h ; Scan keyboard and return key in the Accumulator
;  call 02Bh
  cpi 20h
  jz IfKE
  mvi A,0ffh
  pop D
  pop B
  pop H
 ret
;IfKE:
;  call 0336h ; Scan keyboard and return key in the Accumulator
;  cpi 20h
;  jz IfKE
; ret
