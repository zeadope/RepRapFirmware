M120 ; Push
G90
G1 Z220 F200 S1 ;approach Z Max quickly(ish)
G91
G1 Z-1 F200 ;back of 1
G90
G1 Z220 F50 S1 ;approach Y Max slowly
G92 Z198.3 ;Y axis Max
M121

