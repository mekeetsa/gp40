;**********************************************
RUNFILE: ;MX
  CALL PRINT_NEWLINE
  LXI H,LINEBUF
  MVI C,'.'
  CALL STRCHR
  JNC RUNF1
  LXI D,DOTRK
  CALL STRCPY
RUNF1:  LXI D,LINEBUF
  CALL F_OPEN
  JNC RD_4B
  LXI H,LINEBUF
  MVI C,'.'
  CALL STRCHR
  LXI D,DOTRKS
  CALL STRCPY
  LXI D,LINEBUF
  CALL F_OPEN ;������ ������� RKS ����
  JC C_DIRNO ; no file
  LXI H,LINEBUF+24
  MVI M,'S'
RD_4B:
  LXI B,4 ; ������ �� RK? ����� ����� ������ ���������/���� ���������� � ������ � ����� �� �������� ���������� ������ � ������
  LXI H,LINEBUF
  push H
  CALL F_READ
  pop H
  MOV e,M
  INX H
  MOV d,M
  INX H
  MOV A,M
  INX H
  MOV b,M
  SUB E
  MOV c,A
  MOV A,b
  SBB D
  MOV b,A
  INX B  ;������ �����
  XCHG
  PUSH H ;��������� ����� ������, � ret ����� F_READ ������� ������� �� ���� �����
  mov A,H
  call PRHEX
  mov A,L
  call PRHEX
  push B
  MVI C,','
  CALL PUTC
  pop B
  mov A,B
  call PRHEX
  mov A,C
  call PRHEX
  push H
  LXI H,LINEBUF+24
  MOV A,M
  CPI 'S'
  JNZ RD_RKX ;�������� RKX, ������� �� ������� �� 0xE6 � ����������
  pop H
  CALL F_READ
  LXI D,M2MON
  CALL F_OPEN ;������ ������� ���� ��������
  JNC RUNF3
  pop H
  JMP C_DIRNO ; no file
RUNF3:
  LXI H,0C000H
  LXI B,4096
  CALL F_READ
  JMP 0C000H
RD_RKX:
  push B
RUNF4:
  LXI H,LINEBUF+26
  LXI B,1
  CALL F_READ
  CPI 0E6H ; � � ��������� ��������� ����
  JZ  RUNF6
RUNF5:
  LXI B,1
  INX H
  CALL F_READ
  CPI 0E6H ;
  JNZ RUNF5
RUNF6:
  pop B
  pop H
  CALL F_READ
  jmp SD_OFF

;#ifdef MX2
C_LRD:
  LXI D,LINEBUF+2
  CALL FS_FNDF
  JC C_DIRNO ; no file
   ; � HL ������ FAT � ����������� ������
  LXI D,LINEBUF+25H
  mvi c,7
C_LRD_LOOP1:
  MOV A,M
  STAX D
  INX H
  INX D
  DCR C
  JP C_LRD_LOOP1
  mvi A,20H
  STAX D
  INX D
  mvi c,2
C_LRD_LOOP2:
  MOV A,M
  STAX D
  INX H
  INX D
  DCR C
  JP C_LRD_LOOP2
  LXI D,LINEBUF+2
  CALL F_OPEN
  LXI B,5 ; ������ �� RKX ����� ����� ������ � ����� �����, ������� �����������(������� �����)
  LXI H,LINEBUF+20H
  CALL F_READ
  LXI H,LINEBUF+20H
  MOV e,M
  INX H
  MOV d,M
  INX H
  MOV A,M
  INX H
  MOV b,M
  SUB E
  MOV c,A
  MOV A,b
  SBB D
  MOV b,A
  INX B
  PUSH D ; ����� ������
  PUSH B ; ������ �����, ������� ������
  LXI H,LINEBUF+24H
  MOV A,M
  CPI 0E6H ; � � ��������� ��������� ����, �������� ������� �����
  JZ NOFNAME
  LXI B,12 ; ������ �� RKX ����� ��� �����
  LXI H,LINEBUF+25H
  CALL F_READ
RD_E6_LOOP:
  LXI H,LINEBUF+31H
  LXI B,1
  CALL F_READ
  CPI 0E6H ; � � ��������� ��������� ����
  JNZ RD_E6_LOOP
NOFNAME:
  LXI H,LINEBUF+25H ; ��� � ������� ������
  call 0F848H ; ���������� ��� ����� ��� ������ � RAM Disk
  pop B ; ������ �����, ������� ������
  push B
  LXI H,0 ; �������� � ����
  CALL F_READ ; reading
  LXI H,0 ; ��������� ����� ����
  pop D ; �������� ����� ����� �����
  call 0F83FH ; ������ ����� � RAM Disk
  pop H ; � HL ��������� �����
  call 0F842H ; ���������� ����� ��������
  LXI H,RWR_OK
  JMP PRINT

#ifdef RWR ;!!!!!!!!!!!!!!!!!!!!!!!!!!!
C_SRD: 
  LXI D,LINEBUF+2
  CALL FS_FNDF
  JC C_DIRNO ; no file
  push H ; � HL ������ FAT � ����������� ������
  CALL F_OPEN 
  LXI H,LINEBUF+2
C_SRR1: 
  MOV A,M
  INX H
  CPI 0
  JZ C_SRR2 ;��� �� SD � RAM ����� ���������
  CPI 2CH
  JNZ C_SRR1
  pop B     ;����������� ����� � ����� ��� �� �����, ����� �� SD � RAM ����� ������
  LXI D,LINEBUF+20H
  mvi c,7  ;�� ��������� ������ � ������ �� RAM ����� ����� ��� � ������� RAMFOS
C_SRD_LOOP3:
  MOV A,M
  CPI 2EH
  JNZ C_SRD_LOOP4
  MVI A,20H
  DCX H
C_SRD_LOOP4:
  STAX D
  INX D
  INX H
  DCR C
  JP C_SRD_LOOP3
  mvi A,20H
  STAX D
  INX D
  INX H
  mvi c,2
C_SRD_LOOP5:
  MOV A,M
  STAX D
  INX H
  INX D
  DCR C
  JP C_SRD_LOOP5
  JMP C_SRD_LOOP2
C_SRR2: 
  pop H ; � HL ������ FAT � ����������� ������
  LXI D,LINEBUF+20H
  mvi c,7 ;�� ��������� ������ � ������ �� SD ����� ��� � ������� RAMFOS
C_SRD_LOOP1:
  MOV A,M
  STAX D
  INX H
  INX D
  DCR C
  JP C_SRD_LOOP1
  mvi A,20H
  STAX D
  INX D
  mvi c,2 ; ����������� ����������
C_SRD_LOOP11:
  MOV A,M
  STAX D
  INX H
  INX D
  DCR C
  JP C_SRD_LOOP11
C_SRD_LOOP2:
  LXI H,0H
C_RD_E6_LOOP: ;������� ������� RKX �����
  LXI B,1
  CALL F_READ
  CPI 0E6H ; � � ��������� ��������� ����
  JNZ C_RD_E6_LOOP
  PUSH H      ;���������� ������ ������� RKX �����
  LXI H,LINEBUF+20H ; ��� � ������� ������
  call 0F848H ; ���������� ��� ����� ��� ������ �� RAM Disk
  call 0F84BH ; ����� ����� �� ����� �� 'call 0F848H'
  pop H ; ������ ������ ����� ����� �������
  JNZ C_DIRNO ; no file
  call 0F84EH ; �������� ����� � ����� �� HL
  push D ; ��������� �� ����� ���������� ������
  LXI D,LINEBUF+2 ; ��������� ������������� � ����� �����
  CALL FS_FNDF
  CALL F_OPEN 
  LXI H,0
  pop B ; ������� �������� (����� ������)
  CALL F_WRITE
  LXI H,RWR_OK
  JMP PRINT
#endif ;!!!!!!!!!!!!!!!!!!!!!!!!!!!
