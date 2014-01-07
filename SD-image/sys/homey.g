M120 ; Push
G90
G1 Y220 F6000 S1 ;approach Y Max quickly
G91
G1 Y-5 ;back of 5
G90
G1 Y220 F300 S1 ;approach Y Max slowly
G92 Y199 ;Y axis Max
M121

