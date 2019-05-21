debug_lock

This project demonstrates the device's ability to lock the debug
interface by clearing bits in the lock bits page. The lower four
bits in the Debug Lock Word are cleared. After a device reset, no
debugging can be done until a device mass erase is completed, 
unlocking the device.

Prerequisites:
1. The J-Link Software and Documentation Pack from https://www.segger.com/downloads/jlink/

How To Test:
1. Build the project and and download to the Starter Kit. The Debug
	lock word will be cleared.
2. Disconnect the debugger from the device.
3. Perform either a pin reset (by pressing the reset button) or a
	power on reset (by physically disconnecting and reconnecting the
	device).
4. Edit the second line of test/test_M0.bat (for EFM32ZG and EFM32HG) 
	or test/test_M3-M4.bat (for all other boards) to point to the install
	location of JLink.exe.
5. Run test/test_M0.bat (for EFM32ZG and EFM32HG) or test/test_M3-M4.bat 
	(for all other boards) from the command prompt.
6. The output should display "Read DP register 3 = 0x16E60001" if the device
	is locked.
	
NOTE: This is the most reliable way to verify a debug lock. Many IDEs will
automatically unlock the device when attempting to flash, making it appear
as if the debug lock has failed.

7. To recover the device, open Simplicity Commander, connect to the device, 
	and select "Unlock Debug Access" under "Flash". This erases the entire
	device and unlocks the debug interface.
8. Run test/test_M0.bat or test/test_M3-M4.bat again. The final 
	"Read DP register 3" should no longer output 0x16E60001, but 
	should output some other value.

Peripherals Used:
HFRCO  - 19 MHz

Board: Silicon Labs EFM32ZG Starter Kit (STK3200)
Device: EFM32ZG222f32

Board: Silicon Labs EFM32HG Starter Kit (SLSTK3400A)
Device: EFM32HG322F64

Board: Silicon Labs EFM32G Starter Kit (STKXXX)
Device: EFM32G890F128

Board: Silicon Labs EFM32GG Starter Kit (STK3700)
Device: EFM32GG990F1024

Board: Silicon Labs EFM32LG Starter Kit (STK3600)
Device: EFM32LG990F256

Board: Silicon Labs EFM32WG Starter Kit (STK3800)
Device: EFM32WG990F256

Board: Silicon Labs EFM32TG Starter Kit (STK3300)
Device: EFM32TG840F32
