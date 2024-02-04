;==================================================================================
;DRIVER IDE HDD for ORION-PRO
;бУНДМШЕ ОЮПЮЛЕРПШ НАЫХЕ:
;HL-ЮДПЕЯ ГЮЦПСГЙХ Б ОЮЛЪРЭ
;BCDE-32-У АХРМШИ МНЛЕП ЯЕЙРНПЮ
;A=1 РНКЭЙН, ВРЕМХЕ АНКЭЬЕ НДМНЦН ЯЕЙРНПЮ МЕ ОНДДЕПФХБЮЕРЯЪ PVV! ЙНКХВЕЯРБН АКНЙНБ (АКНЙ=512 АЮИР)
;
;==================================================================================
P_1F7   .EQU 05FH     ;пецхярп янярнъмхъ/пецхярп йнлюмд
P_1F6   .EQU 05EH     ;CHS-мнлеп цнкнбш х сярп/LBA юдпея 24-27
P_1F5   .EQU 05DH     ;CHS-жхкхмдп 8-15/LBA юдпея 16-23
P_1F4   .EQU 05CH     ;CHS-жхкхмдп 0-7/LBA юдпея 8-15
P_1F3   .EQU 05BH     ;CHS-мнлеп яейрнпю/LBA юдпея 0-7
P_1F2   .EQU 05AH     ;явервхй яейрнпнб
P_1F1   .EQU 059H     ;онпр ньханй/ябниярб
P_1F0   .EQU 058H     ;онпр лк. аюир дюммшу
P_3F6   .EQU 056H     ;пецхярп янярнъмхъ/сопюбкемхъ
P_HI    .EQU 057H     ;онпр ярюп. аюир дюммшу
PRT_RW    .EQU P_1F0*256+P_HI ;онпрш времхъ/гюохях ндмхл якнбнл

;мю бшунде:
;H-дкъ MASTER 0-HDD, 1-CDROM, 0XFF-NONE
;L-дкъ SLAVE  0-HDD, 1-CDROM, 0XFF-NONE
SD_INIT:
HDDINIT:   mvi A,0E0H ;A=E0-дкъ MASTER, A=F0-дкъ SLAVE
      PUSH H
      CALL ID_DEV
      POP H
      ana A
      cz INIT_91
      mov H,A
      mvi L,0FFH;A
      xra A

SD_OFF:
HDDOFF:   RET

#ifdef SD_DBG_PRINT1
LBA_OK: DB "IDE:LBA_OK",0DH,0ah,0
#endif
INIT_91:   PUSH H
      mvi L,49*2+1
      mov A,M
      ani 2
      jz INI_912
#ifdef SD_DBG_PRINT1
;  push H
  LXI H,LBA_OK
  CALL PRINT
;  pop H
#endif
      ;mvi C,P_1F2 ;05AH      ;явервхй яейрнпнб
      mvi L,00CH
      mov A,M
      out P_1F2 ;OUT (C),A
      mvi L,6
      ;mvi C,P_1F6 ;05EH      ;CHS-мнлеп цнкнбш х сярп/LBA юдпея 24-27
      mov A,M
      dcr A
      out P_1F6 ;OUT (C),A
      ;mvi C,P_1F7 ;05FH      ;пецхярп янярнъмхъ/пецхярп йнлюмд
      mvi A,091H
      out P_1F7 ;OUT (C),A
      lxi D,1000H
INI_911:   dcx D
      mov A,D
      ora E
      jz INI_912
      in P_1F7;IN A,(C)
      ani 80H
      jnz INI_911
      POP H
      RET

INI_912:   mvi A,0FFH
      POP H
      RET

FS_READ: CALL FS_RDWR
SD_READ:
;READ 1 SECTOR HDD
HDDRDS:   ; mvi A,1
;READ "A" SECTORS HDD
HDDRDM:    PUSH B
      PUSH D
;      push psw
      CALL SETHREG
      ;mvi C,P_1F7 ;05FH      ;пецхярп янярнъмхъ/пецхярп йнлюмд
      mvi A,20H
      out P_1F7 ;OUT (C),A
      ;mvi C,P_1F7 ;05FH      ;пецхярп янярнъмхъ/пецхярп йнлюмд
HDDRD1:    in P_1F7;IN A,(C)
      ani 88H
      cpi 8
      jnz HDDRD1
      ;pop psw ;      EX AF,AF'
HDDRD2:    ;push psw ;EX AF,AF'
      CALL READSEC
      ;mvi C,P_1F7 ;05FH      ;пецхярп янярнъмхъ/пецхярп йнлюмд
HDDRD3:    in P_1F7 ;IN A,(C)
      ani 80H
      jnz HDDRD3
;      pop psw
;      dcr A
;      jnz HDDRD2
      jmp EXITNHD

FS_WRITE: CALL FS_RDWR
SD_WRITE:
;WRITE 1 SECTOR HDD
HDDWRS:   ; mvi A,1
;WRITE "A" SECTORS HDD
HDDWRM:    PUSH B
      PUSH D
      CALL SETHREG
      ;mvi C,P_1F7 ;05FH      ;пецхярп янярнъмхъ/пецхярп йнлюмд
      mvi A,30H
      out P_1F7 ; OUT (C),A
      ;mvi C,P_1F7 ;05FH      ;пецхярп янярнъмхъ/пецхярп йнлюмд
HDDWR1:    in P_1F7; IN A,(C)
      ani 88H
      cpi 8
      jnz HDDWR1
HDDWR2:
      CALL WRITSEC
;       inc h                  ??????????????????????????? МЮ ТНПСЛЕ nedopc ОХЯЮКХ (БПНДЕ DimkaM) , ВРН ЩРН НЬХАЙХ, ОНЩРНЛС ГЮЙНЛЛЕМРХПНБЮМШ
;       inc h     ??????????????????????????????????
      ;mvi C,P_1F7 ;05FH      ;пецхярп янярнъмхъ/пецхярп йнлюмд
HDDWR3:    in P_1F7 ;IN A,(C)
      ani 80H
      jnz HDDWR3
;      pop psw
;      dcr A
;      jnz HDDWR2
EXITNHD:   POP D
      POP B
      mov A,B
      ani 0FH
      mov B,A
      xra A
      RET

;READ SECTOR (512 BYTES)
READSEC:   mvi c,40H
      ;mvi C,P_1F0  ;HI 058H      ;онпр дюммшу
READSC1:
       in P_1F0
       mov M,a
       inx H
;      IN E,(C)
;      DEC C      ;INC C
;      IN D,(C)
;      INC C      ;DEC C
       in (P_1F0-1)
       mov M,a
       inx H
;      LD (HL),E
;      INC HL
;      LD (HL),D
;      INC HL

       in P_1F0
       mov M,a
       inx H
       in (P_1F0-1)
       mov M,a
       inx H

       in P_1F0
       mov M,a
       inx H
       in (P_1F0-1)
       mov M,a
       inx H

       in P_1F0
       mov M,a
       inx H
       in (P_1F0-1)
       mov M,a
       inx H
      dcr C
      jnz READSC1
      RET

;SAVE SECTOR (512 BYTES)
WRITSEC:   ;  EXX
;      PUSH H
;      lxi H,0
;      dad SP
; ;     EXX
;      sphl
      mvi A,40H
;      lxi H,PRT_RW  ;5857
WR_SEC1:   ;POP D
;      LD C,L
;      OUT (C),D
       mov a,M
       out (P_1F0-1)
       inx h
;      LD C,H
;      OUT (C),E
       mov a,M
       out P_1F0
       inx h
;      POP D
;      LD C,L
;      OUT (C),D
       mov a,M
       out (P_1F0-1)
       inx h
;      LD C,H
;      OUT (C),E
       mov a,M
       out P_1F0
       inx h
;      POP D
;      LD C,L
;      OUT (C),D
       mov a,M
       out (P_1F0-1)
       inx h
;      LD C,H
;      OUT (C),E
       mov a,M
       out P_1F0
       inx h
;      POP D
;      LD C,L
;      OUT (C),D
       mov a,M
       out (P_1F0-1)
       inx h
;      LD C,H
;      OUT (C),E
      dcr A
      jnz WR_SEC1
;      lxi H,0
;      dad SP
;;      EXX
;      sphl
;      POP H
;;      EXX
      RET

;SET HDD PORTS
SETHREG:
;      mvi C,P_1F6 ;05EH      ;CHS-мнлеп цнкнбш х сярп/LBA юдпея 24-27
      mvi a,0E0h   ; 11100000b - LBA mode, Master, head 0 
      out P_1F6
      ;mvi C,P_1F7 ;05FH      ;пецхярп янярнъмхъ/пецхярп йнлюмд
SETHRE1:   in P_1F7 ;IN A,(C)
      ani 80H
      jnz SETHRE1
;      mvi C,P_1F5 ;05DH      ;CHS-жхкхмдп 8-15/LBA юдпея 16-23
      mov a,c
      out P_1F5
;      mvi C,P_1F4 ;05CH      ;CHS-жхкхмдп 0-7/LBA юдпея 8-15
      mov a,d
      out P_1F4
;      mvi C,P_1F3 ;05BH      ;CHS-мнлеп яейрнпю/LBA юдпея 0-7
      mov a,e
      out P_1F3
      mvi a,01h
;      mvi C,P_1F2 ;05AH      ;явервхй яейрнпнб
      out P_1F2

      RET

;HL-юдпея астепю яейрнпю хдемрхтхйюжхх
;A=E0-дкъ MASTER, A=F0-дкъ SLAVE
ID_DEV:    ;mvi C,P_1F6 ;05EH      ;CHS-мнлеп цнкнбш х сярп/LBA юдпея 24-27
      out P_1F6 ;OUT (C),A
      ;mvi C,P_1F7 ;05FH      ;пецхярп янярнъмхъ/пецхярп йнлюмд
      mvi D,26
ID_DEV3:   nop ;EI
      nop ;HALT
      nop ;DI
      dcr D
      jz NO_DEV
      in P_1F7 ;IN A,(C)
      mov e,a
      ani 080h ;BIT 7,A
      jnz ID_DEV3
      mov a,e
      ana A
      jz NO_DEV
      inr A
      jz NO_DEV
      xra A
      ;mvi C,P_1F5 ;05DH      ;CHS-жхкхмдп 8-15/LBA юдпея 16-23
      out P_1F5 ;OUT (C),A
      ;mvi C,P_1F4 ;05CH      ;CHS-жхкхмдп 0-7/LBA юдпея 8-15
      out P_1F4 ;OUT (C),A
      mvi A,0ECH
      ;mvi C,P_1F7 ;05FH      ;пецхярп янярнъмхъ/пецхярп йнлюмд
      out P_1F7 ;OUT (C),A
      ;mvi C,P_1F7 ;05FH      ;пецхярп янярнъмхъ/пецхярп йнлюмд
ID_DEV1:   in P_1F7 ;IN A,(C)
      ana A
      jz NO_DEV
      inr A
      jz NO_DEV
      dcr A
      rrc
      jc ID_DEV2
      rlc
      ani 88H
      cpi 8
      jnz ID_DEV1
ID_DEV2:   ;mvi C,P_1F4 ;05CH      ;CHS-жхкхмдп 0-7/LBA юдпея 8-15
      ;mvi C,P_1F5 ;05DH      ;CHS-жхкхмдп 8-15/LBA юдпея 16-23
      in P_1F4
      mov E,A
      in P_1F5
      mov D,A
      ora E
      jz READSEC
;      lxi H,0EB14H
;      SBC HL,DE
;      mvi A,1
;      rz
NO_DEV:    mvi A,0FFH
      RET

      .END