; Configuration file for RepRap Ormerod
; RepRapPro Ltd
;
M111 S0                             ; Debug off
M550 PMendel90                      ; Machine name (can be anything you like)
M551 Preprap                        ; Machine password (currently not used)
M540 P0xBE:0xEF:0xDE:0xAD:0xFE:0xED ; MAC Address
M552 P192.168.1.14                  ; IP address
M553 P255.255.255.0                 ; Netmask
M554 P192.168.1.1                   ; Gateway
M555 P2                             ; Set output to look like Marlin
G21                                 ; Work in millimetres
G90                                 ; Send absolute corrdinates...
M83                                 ; ...but relative extruder moves
M906 X800 Y800 Z800                 ; Motor currents (mA)
T0 ; Select extruder 0
M92 E650; Set extruder steps/mm
M906 .E800 ; Motor current (mA)
T1 ; Select extruder 1
M92 E650; Set extruder steps/mm
M906 .E800 ; Motor current (mA)
T2 ; Select extruder 2
M92 E642; Set extruder steps/mm
M906 .E800 ; Motor current (mA)
T3 ; Select extruder 3
M92 E625; Set extruder steps/mm
M906 .E800 ; Motor current (mA)
T4 ; Select extruder 4
M92 E650; Set extruder steps/mm
M906 .E800 ; Motor current (mA)
; extruder config done
M563 P1 D0 H1                       ; Define tool 1
G10 P1 S0 R0                        ; Set tool 1 operating and standby temperatures
M563 P2 D1 H2                       ; Define tool 1
G10 P2 S0 R0                        ; Set tool 1 operating and standby temperatures
M563 P3 D2 H3                       ; Define tool 1
G10 P3 S0 R0                        ; Set tool 1 operating and standby temperatures
M563 P4 D3 H4                       ; Define tool 1
G10 P4 S0 R0                        ; Set tool 1 operating and standby temperatures


M201 X800 Y800 Z15 E1000            ; Accelerations (mm/s^2)
M203 X15000 Y15000 Z180 E3600       ; Maximum speeds (mm/min)
M566 X600 Y600 Z30 E20              ; Minimum speeds mm/minute

T0 ; Select extruder 0
