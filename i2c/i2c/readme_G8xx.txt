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

Board:  Silicon Labs EFM32G Starter Kit (Gxxx_STK)
Device: EFM32G890F128
PC0 - LED0
PC1 - LED1
PB9 - Push Button PB0
PD6(EXP16) - I2C0 SDA
PD7(EXP15) - I2C0 SCL