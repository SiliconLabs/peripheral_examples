msc_page_lock

This project demonstrates the use of the MSC page lock register to
lock flash pages. A 4 byte data array is stored in the last page(127) 
of the flash which verifies access to the flash page. Then MSC page lock 
register locks the flash page 127 and flash erase/write to corresponding 
flash page fails. Flash page can be monitored in the memory window 
of the debugger.


How To Test:
1. Build the project and download to the Starter Kit
2. Run in the debugger and add a memory window to monitor last page of flash
3. Verify the flash erase and write before the flash page is locked
4. After the flash page is locked, confirm flash erase/write 
   to the corresponding flash page fails.
   
Peripherals Used:
HFRCO  - 19 MHz
MSC

Board:  Silicon Labs EFR32xG21 Radio Board (BRD4181A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG21A010F1024IM32