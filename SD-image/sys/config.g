; Think3dprint3d Mendel90 with Kraken Configuration
; Standard configuration G Codes
M111 S1; Debug on
M550 PMendel90; Set the machine's name
M551 Preprap; Set the password
M552 P192.168.1.14; Set the IP address
M553 P255.255.255.0; Set netmask
M554 P192.168.1.1; Set the gateway
M555 P2; Emulate Marlin USB output
G21 ; Work in mm
G90 ; Absolute positioning
M83 ; Extrusions relative
;M558 P1 ; Turn Z Probe on (not used on the T3P3 printer)
;G31 Z0.5 P500 ; Set Z probe height and threshold (not used on the T3P3 printer)

M906 X800 Y800 Z800 ; Motor currents (mA)

;configure each extruder
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

T0 ; Select extruder 0
