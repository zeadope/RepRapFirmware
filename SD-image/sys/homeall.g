
G90
G1 X2000 F6000 S1 ;approach X Max quickly
G91
G1 X-5 ;back of 5
G90
G1 X2000 F300 S1 ;approach X Max slowly
G92 X194 ;X axis Max
G90
G1 Y2000 F6000 S1 ;approach Y Max quickly
G91
G1 Y-5 ;back of 5
G90
G1 Y2000 F300 S1 ;approach Y Max slowly
G92 Y199 ;Y axis Max
G90
G1 Z220 F200 S1 ;approach Z Max quickly(ish)
G91
G1 Z-1 F200;back of 1
G90
G1 Z220 F50 S1 ;approach Y Max slowly
G92 Z197.5 ;Y axis Max


