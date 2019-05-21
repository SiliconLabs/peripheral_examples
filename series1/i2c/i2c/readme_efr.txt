I2C_Master_Slave

This project demonstrates both master and slave configurations of the 
EFM32 I2C peripheral. Two EFM32 starter kits are connected via a single I2C
bus, and each EFM32 I2C peripheral is setup to both transmit (master mode)
and receive (slave mode).

How To Test:
1. Build the project(s) and download to both Starter Kits
2. Connect the SDA and SCL lines between the two kits via the EXP	
   header
3. Press BTN0 of a kit to send data from the respective kit to the other
4. Observe the receive buffer in the IDE variables window to ensure the 
   receive data is correct

Peripherals Used:
HFRCO  - 19 MHz
I2C0 - 392157 Hz

Board:  Silicon Labs EFR32BG1P Starter Kit (BRD4100A)
Device: EFR32BG1P232F256GM48
PC10(EXP header 16) - I2C0 SDA
PC11(EXP header 15) - I2C0 SCL
PF4 - LED0
PF5 - LED1
PF6 - Push Button PB0

Board:  Silicon Labs EFR32BG12P Starter Kit (BRD4103A)
Device: EFR32BG12P332F1024GL125
PC10(EXP header 16) - I2C0 SDA
PC11(EXP header 15) - I2C0 SCL
PF4 - LED0
PF5 - LED1
PF6 - Push Button PB0

Board:  Silicon Labs EFR32BG13P Starter Kit (BRD4104A)
Device: EFR32BG13P632F512GM48
PC10(EXP header 16) - I2C0 SDA
PC11(EXP header 15) - I2C0 SCL
PF4 - LED0
PF5 - LED1
PF6 - Push Button PB0

Board:  Silicon Labs EFR32BG14P Starter Kit (BRD4105A)
Device: EFR32BG14P732F256GM48
PC10(EXP header 16) - I2C0 SDA
PC11(EXP header 15) - I2C0 SCL
PF4 - LED0
PF5 - LED1
PF6 - Push Button PB0

Board:  Silicon Labs EFR32FG1P Starter Kit (BRD4250A)
Device: EFR32FG1P133F256GM48
PC10(EXP header 16) - I2C0 SDA
PC11(EXP header 15) - I2C0 SCL
PF4 - LED0
PF5 - LED1
PF6 - Push Button PB0

Board:  Silicon Labs EFR32FG12P Starter Kit (BRD4253A)
Device: EFR32FG12P433F1024GL125
PC10(EXP header 16) - I2C0 SDA
PC11(EXP header 15) - I2C0 SCL
PF4 - LED0
PF5 - LED1
PF6 - Push Button PB0

Board:  Silicon Labs EFR32FG13P Starter Kit (BRD4256A)
Device: EFR32FG13P233F512GM48
PC10(EXP header 16) - I2C0 SDA
PC11(EXP header 15) - I2C0 SCL
PF4 - LED0
PF5 - LED1
PF6 - Push Button PB0

Board:  Silicon Labs EFR32FG14P Starter Kit (BRD4257A)
Device: EFR32FG14P233F256GM48
PC10(EXP header 16) - I2C0 SDA
PC11(EXP header 15) - I2C0 SCL
PF4 - LED0
PF5 - LED1
PF6 - Push Button PB0

Board:  Silicon Labs EFR32MG1P Starter Kit (BRD4151A)
Device: EFR32MG1P232F256GM48
PF4 - LED0
PF5 - LED1
PF6 - Push Button PB0
PC10(EXP header 16) - I2C0 SDA
PC11(EXP header 15) - I2C0 SCL

Board:  Silicon Labs EFR32MG12P Starter Kit (BRD4161A)
Device: EFR32MG1P432F1024GL125
PF4 - LED0
PF5 - LED1
PF6 - Push Button PB0
PC10(EXP header 16) - I2C0 SDA
PC11(EXP header 15) - I2C0 SCL

Board:  Silicon Labs EFR32MG13P Starter Kit (BRD4159A)
Device: EFR32MG13P632F512GM48
PF4 - LED0
PF5 - LED1
PF6 - Push Button PB0
PC10(EXP header 16) - I2C0 SDA
PC11(EXP header 15) - I2C0 SCL

Board:  Silicon Labs EFR32MG14P Starter Kit (BRD4169B)
Device: EFR32MG14P733F256GM48
PF4 - LED0
PF5 - LED1
PF6 - Push Button PB0
PC10(EXP header 16) - I2C0 SDA
PC11(EXP header 15) - I2C0 SCL
