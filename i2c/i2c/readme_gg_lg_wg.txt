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

Board:  Silicon Labs EFM32GG Starter Kit (STK3700)
Device: EFM32GG990F1024
PE2 - LED0
PE3 - LED1
PB9 - Push Button PB0
PD6(Expansion header 16) - I2C0 SDA
PD7(Expansion header 17) - I2C0 SCL


Board:  Silicon Labs EFM32LG Starter Kit (STK3600)
Device: EFM32LG990F256
PE2 - LED0
PE3 - LED1
PB9 - Push Button PB0
PD6(Expansion header 16) - I2C0 SDA
PD7(Expansion header 17) - I2C0 SCL

Board:  Silicon Labs EFM32WG Starter Kit (STK3800)
Device: EFM32WG990F256
PE2 - LED0
PE3 - LED1
PB9 - Push Button PB0
PD6(Expansion header 16) - I2C0 SDA
PD7(Expansion header 17) - I2C0 SCL