rem Change path to SEGGER JLink install location if necessary
"C:\Program Files (x86)\SEGGER\JLink_V632g\JLink.exe" ^
 ^
 ^
 ^
-If SWD -Speed 5000 -CommanderScript EFM32_M0_unlock.jlink
pause
