Install WinCUPL from:

    https://www.microchip.com/en-us/products/fpgas-and-plds/spld-cplds/pld-design-resources

into:

    C:\Wincupl

The graphical interface does not work on Win 10. Use cupl.exe instead or wine under Linux.


Related Mnemonics:  g16v8, g16v8as, g16v8ma, g16v8ms, g16v8s

       Registered  Complex   Simple    Auto Mode Select
CUPL   G16V8MS     G16V8MA   G16V8AS   G16V8

In registered mode pin 1 and pin 11 are permanently configured
as clock and output enable, respectively. These pins cannot be configured
as dedicated inputs in the registered mode.

In complex mode pin 1 and pin 11 become dedicated inputs and
use the feedback paths of pin 19 and pin 12 respectively. Because
of this feedback path usage, pin 19 and pin 12 do not have the
feedback option in this mode.

In simple mode all feedback paths of the output pins are routed
via the adjacent pins. In doing so, the two inner most pins (pins
15 and 16) will not have the feedback option as these pins are
always configured as dedicated combinatorial output.

            ______________
           |    GP_U2     |
 !MREQ x---|1           20|---x Vcc  
   A15 x---|2           19|---x A14R 
   A14 x---|3           18|---x A7R  
    A7 x---|4           17|---x !RAM 
   A13 x---|5           16|---x A13R 
       x---|6           15|---x !CSE 
   A11 x---|7           14|---x !KEY 
   A12 x---|8           13|---x !ROM 
   D13 x---|9           12|---x !CSSD
   GND x---|10          11|---x      
           |______________|
           
=============================================
      !                             !        
      M                     ! ! ! ! C  A A   
      R  A A A  A A    D A  R R C K S  1 1   
      E  1 1 1  1 1  A 1 7  O A S E S  4 3   
      Q  5 4 3  2 1  7 3 R  M M E Y D  R R      
=============================================
0001: X  X X X  X X  X X X  X X X X X  X X
0002: X  X X X  X X  0 1 L  X X X X X  X X
0003: X  X X X  X X  0 0 H  X X X X X  X X
0004: X  X X X  X X  1 0 H  X X X X X  X X
0005: X  X X X  X X  1 1 H  X X X X X  X X
0006: 1  X X X  X X  X X X  H H H H H  X X
0007: 0  0 0 0  X X  X X X  L H H H H  L L
0008: 0  0 0 1  0 0  X X X  H H H L H  L H
0009: 0  0 0 1  0 1  X X X  H L H H H  L H
0010: 0  0 0 1  1 0  X X X  H L H H H  L H
0011: 0  0 0 1  1 1  X X X  H L H H H  L H
0012: 0  0 1 0  X X  X X X  H L H H H  H L
0013: 0  0 1 1  X X  X X X  H L H H H  H H
0014: 0  1 0 0  X X  X X X  H L H H H  L L
0015: 0  1 0 1  X X  X X X  H H L H H  L H
0016: 0  1 1 0  0 0  X X X  H L H H H  L H
0017: 0  1 1 0  0 1  X X X  H H H H L  L H
0018: 0  1 1 0  1 0  X X X  H H H H L  L H
0019: 0  1 1 0  1 1  X X X  H H H H L  L H
0020: 0  1 1 1  X X  X X X  L H H H H  H H
