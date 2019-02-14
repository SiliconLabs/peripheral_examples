I2C_Master_Slave

This project demonstrates both master and slave configurations of the 
EFM32 I2C peripheral. Two EFM32 starter kits are connected via a single I2C
bus, and each EFM32 I2C peripheral is setup to both transmit (master mode)
and receive (slave mode). This readme is for the Zero Gecko and Happy Gecko
boards, the Happy Gecko uses the odd GPIO interrupt handler and the Zero 
Gecko uses the even.

How To Test:
1. Build the project(s) and download to both Starter Kits
2. Connect the SDA and SCL lines between the two kits via the EXP	
   header
3. Press BTN0 of a kit to send data from the respective kit to the other
4. Observe the receive buffer in the IDE variables window to ensure the 
   receive data is correct

Peripherals Used:
HFRCO  - 14 MHz
I2C0 - 392157 Hz

Board:  Silicon Labs EFM32HG Starter Kit (SLSTK3400A)
Device: EFM32HG322F64
PF4 - LED0
PF5 - LED1
PC9 - Push Button PB0
PD6(Expansion header 16) - I2C0 SDA
PD7(Expansion header 15) - I2C0 SCL

Board:  Silicon Labs EFM32ZG Starter Kit (STK3200)
Device: EFM32ZG222F32
PC10 - LED0
PC11 - LED1
PC8 - Push Button PB0
PD6(Expansion header 6) - I2C0 SDA
PD7(Expansion header 4) - I2C0 SCL

