<table border="0">
  <tr>
    <td align="left" valign="middle">
	  <h1>EFM32 and EFR32<br/>32-bit MCU Peripheral Examples</h1>
	</td>
	<td align="left" valign="middle">
	  <a href="https://www.silabs.com/products/mcu/32-bit">
	    <img src="http://pages.silabs.com/rs/634-SLU-379/images/WGX-transparent.png"  title="Silicon Labs Gecko and Wireless Gecko MCUs" alt="EFM32 32-bit Microcontrollers" width="250"/>
	  </a>
	</td>
  </tr>
</table>

This repo contains simple peripheral examples based on emlib for Series 0, Series 1, and Series 2 devices.

## Supported Series 0 Devices ##
- EFM32ZG
- EFM32HG
- EFM32TG
- EFM32G
- EFM32LG
- EFM32GG
- EFM32WG

## Supported Series 1 Devices ##
- EFM32PG1
- EFR32MG1
- EFR32BG1
- EFR32FG1
- EFM32PG12
- EFR32MG12
- EFR32BG12
- EFR32FG12
- EFR32MG13
- EFR32BG13
- EFR32FG13
- EFR32MG14
- EFR32FG14
- EFM32GG11
- EFM32TG11

## Supported Series 2 Devices ##
- EFR32BG21
- EFR32MG21
- EFR32BG22
- EFR32FG22
- EFR32MG22

## Requirements ##
1. Desired Silicon Labs Starter Kit
2. Simplicity Studio 5 or IAR Workbech IDE
3. Gecko SDK Suite 3.1 and 32-bit MCU SDK 6.0.0.0, available via Simplicity Studio
4. This repo, cloned to to `C:\SiliconLabs\SimplicityStudio\v5\developer\sdks\gecko_sdk_suite\v#.#\`, where #.# is the Gecko SDK suite version number

## How to import to Simplicity Studio IDE ##
1. In Simplicity Studio: 
	- File -> Import, or 
	- Project -> Import -> MCU Project
2. Navigate to the desired .slsproj file

## How to import to IAR Embedded Workbench IDE ##
1. Navigate to the desired .eww file and double click

## Porting to Another Board
Peripheral examples make use of the BSP headers to provide portable support for different development boards. To change the target board, navigate to Project -> Properties -> C/C++ Build -> Board/Part/SDK. Start typing in the Boards search box and locate the desired development board, then click Apply to change the project settings. Ensure that the board specific include paths, found in Project -> Properties -> C/C++ General -> Paths and Symbols, correctly match the target board.

e.g. ```${StudioSdkPath}/hardware/kit/EFR32MG21_BRD4181B/config``` is the correct include path for the brd4181b radio board.

## Reporting Bugs/Issues and Posting Questions and Comments ##

All examples in this repo is are considered EVALUATION QUALITY, meaning this code has been minimally tested to ensure that it builds with the specified dependencies and is suitable as a demonstration for evaluation purposes only. This code will be maintained at the sole discretion of Silicon Labs.

To report bugs in the peripheral example projects, please create a new "Issue" in the "Issues" section of this repo.  Please reference the board, project, and source files associated with the bug, and reference line numbers.  If you are proposing a fix, also include information on the proposed fix.  Silicon Labs engineers will address bugs and push them to the public repository periodically.

Questions and comments related to the peripheral examples should be made by creating a new "Issue" in the "Issues" section of this repo.
