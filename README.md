# spna

This is code for single-port network analyzer (SPNA) using
an Arduino Mega, graphical display shield, AD9850 DDS signal 
generator, Mini-Circuits PDC-20-3BD directional coupler, and 
AD8302 RF gain phase detector. 

http://pages.cs.wisc.edu/~timc/e/spna/

There are two different versions -- v3 and v4.  

v3 uses UTFT and UTouch -- which is problematic as the touch function
with this display is problematic at best.

v4 uses https://github.com/neu-rah/ArduinoMenu.git

v3 uses the following libraries:

UTFT: http://www.rinkydinkelectronics.com/library.php?id=51

various fonts: http://www.rinkydinkelectronics.com/r_fonts.php

URTouch: http://www.rinkydinkelectronics.com/library.php?id=92

UTFT_Buttons: http://www.rinkydinkelectronics.com/library.php?id=61

DDS: https://github.com/m0xpd/DDS

##
Tue Jan  2 23:33:32 CST 2018

Picking up project after a long hiatus.  Lots of cleanup as the 
compiler gets pickier.  Now have a mouse cursor on the screen,
need to action that to find screen color select/replace/update
cycle and actually 'click' a button.
