EESchema Schematic File Version 4
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L ESP32-DEVKITC-32D:ESP32-DEVKITC-32D U1
U 1 1 5DD574B9
P 4400 2750
F 0 "U1" H 4400 3917 50  0000 C CNN
F 1 "ESP32-DEVKITC-32D" H 4400 3826 50  0000 C CNN
F 2 "MODULE_ESP32-DEVKITC-32D" H 4400 2750 50  0001 L BNN
F 3 "None" H 4400 2750 50  0001 L BNN
F 4 "Espressif Systems" H 4400 2750 50  0001 L BNN "Champ4"
F 5 "Unavailable" H 4400 2750 50  0001 L BNN "Champ5"
F 6 "EVAL BOARD FOR ESP-WROOM-32" H 4400 2750 50  0001 L BNN "Champ6"
F 7 "ESP32-DEVKITC-32D" H 4400 2750 50  0001 L BNN "Champ7"
F 8 "None" H 4400 2750 50  0001 L BNN "Champ8"
	1    4400 2750
	1    0    0    -1  
$EndComp
Text GLabel 2950 2650 0    50   Input ~ 0
DAC1
Text GLabel 2950 2750 0    50   Input ~ 0
DAC2
Wire Wire Line
	2950 2650 3600 2650
Wire Wire Line
	2950 2750 3000 2750
Connection ~ 3000 2750
Wire Wire Line
	3000 2750 3600 2750
Wire Wire Line
	3800 4700 3000 4700
Wire Wire Line
	3000 2750 3000 4700
$Comp
L Device:R R2
U 1 1 5DD579AB
P 4000 5200
F 0 "R2" V 3793 5200 50  0000 C CNN
F 1 "R" V 3884 5200 50  0000 C CNN
F 2 "" V 3930 5200 50  0001 C CNN
F 3 "~" H 4000 5200 50  0001 C CNN
	1    4000 5200
	0    1    1    0   
$EndComp
$Comp
L Device:R R1
U 1 1 5DD57A3A
P 3550 5200
F 0 "R1" V 3343 5200 50  0000 C CNN
F 1 "R1" V 3434 5200 50  0000 C CNN
F 2 "" V 3480 5200 50  0001 C CNN
F 3 "~" H 3550 5200 50  0001 C CNN
	1    3550 5200
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5DD57A8D
P 3000 5350
F 0 "#PWR?" H 3000 5100 50  0001 C CNN
F 1 "GND" H 3005 5177 50  0000 C CNN
F 2 "" H 3000 5350 50  0001 C CNN
F 3 "" H 3000 5350 50  0001 C CNN
	1    3000 5350
	1    0    0    -1  
$EndComp
Wire Wire Line
	3000 5350 3000 5200
Wire Wire Line
	3000 5200 3400 5200
Wire Wire Line
	4150 5200 4400 5200
Wire Wire Line
	4400 5200 4400 4800
Wire Wire Line
	3800 4900 3800 5200
Wire Wire Line
	3700 5200 3800 5200
Connection ~ 3800 5200
Wire Wire Line
	3800 5200 3850 5200
$Comp
L Amplifier_Operational:L272M U2
U 1 1 5DD57FB9
P 4100 4800
F 0 "U2" H 4100 5167 50  0000 C CNN
F 1 "L272M" H 4100 5076 50  0000 C CNN
F 2 "Package_DIP:DIP-8_W7.62mm_LongPads" H 4100 4800 50  0001 C CNN
F 3 "www.st.com/resource/en/datasheet/l272.pdf" H 4100 4800 50  0001 C CNN
	1    4100 4800
	1    0    0    -1  
$EndComp
$EndSCHEMATC