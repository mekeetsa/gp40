
;LAST UPDATE: 06.02.2011 savelij

	.org 0000H

	LD A,0C0H
	OUT (0FBH),A
	
	LD HL,1000H

	CALL COMHDDN
	.DB 0
	LD A,H
	CALL 0F815H
	LD A,L
	CALL 0F815H
	
	LD BC,0E000H
	LD DE,0000H
	LD HL,2000H
	LD A,40H
	CALL COMHDDN
	.DB 3
M1	JP M1
	
	
;==================================================================================
;DRIVER IDE HDD	for ORION-PRO
;бУНДМШЕ ОЮПЮЛЕРПШ НАЫХЕ:
;HL-ЮДПЕЯ ГЮЦПСГЙХ Б ОЮЛЪРЭ
;BCDE-32-У АХРМШИ МНЛЕП ЯЕЙРНПЮ
;A-ЙНКХВЕЯРБН АКНЙНБ (АКНЙ=512 АЮИР)
;РНКЭЙН ДКЪ ЛМНЦНАКНВМНИ ГЮОХЯХ/ВРЕМХХ
;	CALL COMHDDN
;   .DB МНЛЕП ТСМЙЖХХ (0-5)
;
;==================================================================================
Hddinit		.EQU 0   ;INIT HDD
Hddoff		.EQU 1
Hddrds		.EQU 2   ;READ SINGLE SECTOR
Hddrdm		.EQU 3   ;READ MULTI SECTOR
Hddwrs		.EQU 4   ;SAVE SINGLE SECTOR
Hddwrm		.EQU 5   ;SAVE MULTI SECTOR

;наыюъ рнвйю бундю дкъ пюанрш я HDD NEMO
COMHDDN		EX AF,AF'
			EX (SP),HL
			LD A,(HL)
			INC HL
			EX (SP),HL
			ADD A,A
			PUSH HL
			LD HL,TBLHDDN
			ADD A,L
			LD L,A
			LD A,H
			ADC A,0
			LD H,A
			LD A,(HL)
			INC HL
			LD H,(HL)
			LD L,A
			EX AF,AF'
			EX (SP),HL
			RET

TBLHDDN    .DW HDDINIT	;INIT HDD
		   .DW HDDOFF
		   .DW HDDRDS	;READ SINGLE SECTOR
		   .DW HDDRDM	;READ MULTI SECTOR
		   .DW HDDWRS	;SAVE SINGLE SECTOR
		   .DW HDDWRM	;SAVE MULTI SECTOR


P_1F7		.EQU 05FH			;пецхярп янярнъмхъ/пецхярп йнлюмд
P_1F6		.EQU 05EH			;CHS-мнлеп цнкнбш х сярп/LBA юдпея 24-27
P_1F5		.EQU 05DH			;CHS-жхкхмдп 8-15/LBA юдпея 16-23
P_1F4		.EQU 05CH			;CHS-жхкхмдп 0-7/LBA юдпея 8-15
P_1F3		.EQU 05BH			;CHS-мнлеп яейрнпю/LBA юдпея 0-7
P_1F2		.EQU 05AH			;явервхй яейрнпнб
P_1F1		.EQU 059H			;онпр ньханй/ябниярб
P_1F0		.EQU 058H			;онпр лк. аюир дюммшу
P_3F6		.EQU 056H			;пецхярп янярнъмхъ/сопюбкемхъ
P_HI		.EQU 057H			;онпр ярюп. аюир дюммшу
PRT_RW		.EQU P_1F0*256+P_HI	;онпрш времхъ/гюохях ндмхл якнбнл

;мю бшунде:
;H-дкъ MASTER 0-HDD, 1-CDROM, 0XFF-NONE
;L-дкъ SLAVE  0-HDD, 1-CDROM, 0XFF-NONE
HDDINIT		LD A,0E0H
			PUSH HL
			CALL ID_DEV
			POP HL
			AND A
			CALL Z,INIT_91
;			LD D,A
;			LD A,0F0H
;			PUSH DE
;			PUSH HL
;			CALL ID_DEV
;			POP HL	
;			AND A
;			CALL Z,INIT_91
;			POP HL
			LD H,A
			LD L,0FFH;A
			XOR A

HDDOFF		RET

INIT_91		PUSH HL
			LD L,49*2+1
			LD A,(HL)
			AND 2
			JR Z,INI_912
			LD C,P_1F2 ;05AH			;явервхй яейрнпнб
			LD L,00CH
			LD A,(HL)
			OUT (C),A
			LD L,6
			LD C,P_1F6 ;05EH			;CHS-мнлеп цнкнбш х сярп/LBA юдпея 24-27
			LD A,(HL)
			DEC A
			OUT (C),A
			LD C,P_1F7 ;05FH			;пецхярп янярнъмхъ/пецхярп йнлюмд
			LD A,091H
			OUT (C),A
			LD DE,1000H
INI_911		DEC DE
			LD A,D
			OR E
			JR Z,INI_912
			IN A,(C)
			AND 80H
			JR NZ,INI_911
			POP HL
			RET

INI_912		LD A,0FFH
			POP HL
			RET

;READ 1 SECTOR HDD
HDDRDS		LD A,1
;READ "A" SECTORS HDD
HDDRDM		PUSH BC
			PUSH DE
			CALL SETHREG
			EX AF,AF'
			LD C,P_1F7 ;05FH			;пецхярп янярнъмхъ/пецхярп йнлюмд
			LD A,20H
			OUT (C),A
			LD C,P_1F7 ;05FH			;пецхярп янярнъмхъ/пецхярп йнлюмд
HDDRD1		IN A,(C)
			AND 88H
			CP 8
			JR NZ,HDDRD1
			EX AF,AF'
HDDRD2		EX AF,AF'
			CALL READSEC
			LD C,P_1F7 ;05FH			;пецхярп янярнъмхъ/пецхярп йнлюмд
HDDRD3		IN A,(C)
			AND 80H
			JR NZ,HDDRD3
			EX AF,AF'
			DEC A
			JR NZ,HDDRD2
			JR EXITNHD

;WRITE 1 SECTOR HDD
HDDWRS		LD A,1
;WRITE "A" SECTORS HDD
HDDWRM		PUSH BC
			PUSH DE
			CALL SETHREG
			EX AF,AF'
			LD C,P_1F7 ;05FH			;пецхярп янярнъмхъ/пецхярп йнлюмд
			LD A,30H
			OUT (C),A
			LD C,P_1F7 ;05FH			;пецхярп янярнъмхъ/пецхярп йнлюмд
HDDWR1		IN A,(C)
			AND 88H
			CP 8
			JR NZ,HDDWR1
			EX AF,AF'
HDDWR2		EX AF,AF'
			CALL WRITSEC
;		    inc h                  ??????????????????????????? МЮ ТНПСЛЕ nedopc ОХЯЮКХ (БПНДЕ DimkaM) , ВРН ЩРН НЬХАЙХ, ОНЩРНЛС ГЮЙНЛЛЕМРХПНБЮМШ
;		    inc h			??????????????????????????????????
			LD C,P_1F7 ;05FH			;пецхярп янярнъмхъ/пецхярп йнлюмд
HDDWR3		IN A,(C)
			AND 80H
			JR NZ,HDDWR3
			EX AF,AF'
			DEC A
			JR NZ,HDDWR2
EXITNHD		POP DE
			POP BC
			LD A,B
			AND 0FH
			LD B,A
			XOR A
			RET

;READ SECTOR (512 BYTES)
READSEC		LD A,40H
			LD C,P_1F0	;HI 058H			;онпр дюммшу
READSC1		
;			REPT 4

			IN E,(C)
			DEC C      ;INC C
			IN D,(C)
			INC C      ;DEC C
			LD (HL),E
			INC HL
			LD (HL),D
			INC HL
			
			IN E,(C)
			DEC C      ;INC C
			IN D,(C)
			INC C      ;DEC C
			LD (HL),E
			INC HL
			LD (HL),D
			INC HL		
			
			IN E,(C)
			DEC C      ;INC C
			IN D,(C)
			INC C      ;DEC C
			LD (HL),E
			INC HL
			LD (HL),D
			INC HL				
			
			IN E,(C)
			DEC C      ;INC C
			IN D,(C)
			INC C      ;DEC C
			LD (HL),E
			INC HL
			LD (HL),D
			INC HL				
			
;			ENDM

			DEC A
			JR NZ,READSC1
			RET

;SAVE SECTOR (512 BYTES)
WRITSEC		EXX
			PUSH HL
			LD HL,0
			ADD HL,SP
			EXX
			LD SP,HL
			LD A,40H
			LD HL,PRT_RW  ;5857
WR_SEC1		POP DE
			LD C,L
			OUT (C),D
			LD C,H
			OUT (C),E
			POP DE
			LD C,L
			OUT (C),D
			LD C,H
			OUT (C),E
			POP DE
			LD C,L
			OUT (C),D
			LD C,H
			OUT (C),E
			POP DE
			LD C,L
			OUT (C),D
			LD C,H
			OUT (C),E
			DEC A
			JR NZ,WR_SEC1
			LD HL,0
			ADD HL,SP
			EXX
			LD SP,HL
			POP HL
			EXX
			RET

;SET HDD PORTS
SETHREG		PUSH DE
			LD D,B
			LD E,C
			LD C,P_1F6 ;05EH			;CHS-мнлеп цнкнбш х сярп/LBA юдпея 24-27
			OUT (C),D
			EX AF,AF'
			LD C,P_1F7 ;05FH			;пецхярп янярнъмхъ/пецхярп йнлюмд
SETHRE1		IN A,(C)
			AND 80H
			JR NZ,SETHRE1
			LD C,P_1F5 ;05DH			;CHS-жхкхмдп 8-15/LBA юдпея 16-23
			OUT (C),E
			POP DE
			LD C,P_1F4 ;05CH			;CHS-жхкхмдп 0-7/LBA юдпея 8-15
			OUT (C),D
			LD C,P_1F3 ;05BH			;CHS-мнлеп яейрнпю/LBA юдпея 0-7
			OUT (C),E
			LD C,P_1F2 ;05AH			;явервхй яейрнпнб
			EX AF,AF'
			OUT (C),A
			RET

;HL-юдпея астепю яейрнпю хдемрхтхйюжхх
;A=E0-дкъ MASTER, A=F0-дкъ SLAVE
ID_DEV		LD C,P_1F6 ;05EH			;CHS-мнлеп цнкнбш х сярп/LBA юдпея 24-27
			OUT (C),A
			LD C,P_1F7 ;05FH			;пецхярп янярнъмхъ/пецхярп йнлюмд
			LD D,26
ID_DEV3		EI
			HALT
			DI
			DEC D
			JR Z,NO_DEV
			IN A,(C)
			BIT 7,A
			JR NZ,ID_DEV3
			AND A
			JR Z,NO_DEV
			INC A
			JR Z,NO_DEV
			XOR A
			LD C,P_1F5 ;05DH			;CHS-жхкхмдп 8-15/LBA юдпея 16-23
			OUT (C),A
			LD C,P_1F4 ;05CH			;CHS-жхкхмдп 0-7/LBA юдпея 8-15
			OUT (C),A
			LD A,0ECH
			LD C,P_1F7 ;05FH			;пецхярп янярнъмхъ/пецхярп йнлюмд
			OUT (C),A
			LD C,P_1F7 ;05FH			;пецхярп янярнъмхъ/пецхярп йнлюмд
ID_DEV1		IN A,(C)
			AND A
			JR Z,NO_DEV
			INC A
			JR Z,NO_DEV
			DEC A
			RRCA
			JR C,ID_DEV2
			RLCA
			AND 88H
			CP 8
			JR NZ,ID_DEV1
ID_DEV2		LD C,P_1F4 ;05CH			;CHS-жхкхмдп 0-7/LBA юдпея 8-15
			IN E,(C)
			LD C,P_1F5 ;05DH			;CHS-жхкхмдп 8-15/LBA юдпея 16-23
			IN D,(C)
			LD A,D
			OR E
			JP Z,READSEC
			LD HL,0EB14H
			SBC HL,DE
			LD A,1
			RET Z
NO_DEV		LD A,0FFH
			RET

			.END