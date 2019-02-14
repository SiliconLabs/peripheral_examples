Generates XML and project files for ALL boards for mcu_peripheral_examples. Reads selected main.c files and includes necessary libraries and drivers for each kit.

Steps to use:
1. edit gen.bat, and modify the 'projpath' to point to the correct project directory

2. run gen.bat. If this is the first time you are running the script for a particular project, select yes to modify and define the main files used for each kit

3. When defining the main file to be used, be sure to type the full file name, but omit the file extension. If you would like the xml to not include a particular kit, type none
ie:

  EFR32FG12P=main

  or 

  EFM32G=none

  or 

  EFM32LG=main_BURTC


*****
To regenerate project files only, edit regen.bat, and modify the 'projpath' to point to the correct project directory, then run

To generate the XML file only, run buildxml.py and specify the file path

