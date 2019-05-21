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

Board:  Silicon Labs EFR32TG11 Starter Kit (SLSTK3301A)
Device: EFM32TG11B520F128GM80
PD6(EXP header 16) - I2C0 SDA
PD7(EXP header 15) - I2C0 SCL
PD2 - LED0
PC2 - LED1
PD5 - Push Button PB0
