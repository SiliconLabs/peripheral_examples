
================================================================================

This project uses the USB module to implement a USB CDC device (Communications
Device Class) that uses the driver code in Drivers/cdc.c to act as a USB to UART
bridge. Input that is received on the USB device's USART RX pin gets processed
and then sent over USB to the USB host. Input that is received from the USB host
is then processed and sent to the USB device's USART TX pin. In this case, the
USB host is the computer and the USB device is the EFM32 board. This project
operates in EM1.

Note: the Drivers/cdc.c file uses the DMA and therefore is not compatible with
the GG11's LDMA. Therefore, this repo has a src/cdc_gg11.c file that is simply a
port of the Drivers/cdc.c file to use the LDMA instead.

The src/descriptors.c file defines what kind of device is seen by the USB host.
It defines the device as a CDC device, the vendor ID, product ID, etc.

The inc/usbconfig.h file has #defines that can be modified to configure the USB
stack. These options include the clock source for low power mode, number of
endpoints used, the number of interfaces, as well as DMA and USART
configuration, etc.

The Drivers/cdc.c file contains the CDC callback functions for handling device
state changes and USB host setup commands. It also contains RX/TX callback
functions that define the flow of data transfers. Once the device is in the
configured state, CDC_StateChangeEvent() initializes all of the globals to the
proper state, configures the device to be able to read from the USB host's OUT
endpoint, and activates the DMA to be able to read from the USART RX pin. If the
device receives data from the USART RX pin, the DMA transfer will occur. At this
point, if CDC_USB_TX_BUF_SIZ (127 by default) bytes are not received within
CDC_RX_TIMEOUT (10 ms by default), then UartRxTimeout() will be called by the
timer. This function then calls the DmaRxComplete() function to write the data
over USB to the host computer (USBD_Write()) and also restart the DMA to read
again from the USART RX pin upon receiving data. If the timeout did not occur,
then DmaRxComplete() would be called anyway. After USBD_Write() finishes,
UsbDataTransmitted() will be called. This function sets up a write to the USB
host's IN endpoint and again refreshes the DMA read. A similar process would
occur if data was received over USB instead of USART to begin with.
UsbDataReceived() would be called, which then activates a DMA transfer, after
the transfer, DmaTxComplete() would be called.

Note: The callback functions in Drivers/cdc.c are named with respect to the usb
device (in this case the EFM32 board). For example, DmaRxComplete() gets called
when the board receives data from the USART_RX pin. UsbDataTransmitted() gets
called when the board transmits data over USB to the host (in this case the
computer).

Note: Endpoints are named with respect to the USB host (which conforms to the
USB standard). For example, CDC_EP_DATA_IN is the USB host's IN endpoint and
therefore the USB device's OUT endpoint. CDC_EP_DATA_OUT is the USB host's OUT
endpoint and therefore the USB device's IN endpoint.

================================================================================

Note: This project only works on Windows 10 currently since we have not made a
signed driver yet (it has yet to be approved by Microsoft). As a result, this
project uses Windows 10's default CDC driver.

================================================================================

Project directory structure:
 - Note: The following only applies to the Github's directory structure. The
   Simplicity Studio project was generated in a way such that only the relevant
   files for the project were included and thus the directory structure of the
   Simplicity Studio project does not look like the following.

usbd_cdc_uart_bridge/

  - inc

      inc_gg11/
        usbconfig.h

      inc_hg/
        usbconfig.h

      inc_series0/
        usbconfig.h

      cdc.h
      descriptors.h

  - src
      cdc_gg11.c
      descriptors.c
      main_gg11.c
      main_s0.c

There are two main folders: inc/ and src/. The inc/ folder has all of the
releveant header files. Every project shares the cdc.h and descriptors.h file
since they don't have much other than function prototypes and a list of global
variables. Each project has its own usbconfig.h file because of different pin
mappings for the USART, DMA/LDMA, etc. Series 0 projects (i.e. GG, LG, WG, HG)
have two source files: main_s0.c and descriptors.c. The GG11 project has three
source files: main_s1.c, descriptors.c, and cdc_gg11.c.

================================================================================

How To Test:
1. Put the power source switch in the DBG/AEM position.
2. Plug the USB type B mini into the board, build the project, and download
   it to the Starter Kit.
3. Plug the USB type B micro into the bottom USB port of the board. For the
   GG11, this USB port is on the upper right side of the starter kit.
4. Make sure the board is listed under "Ports" in Device Manager. Since we are
   using a default Windows 10 driver, it should be listed as
   "USB Serial Device." If the power switch is in DBG/AEM mode and the debugger
   USB cable is plugged in, then the user might also see an additional COM port
   called "JLink CDC UART Port." This JLink port is for the debugger and is not
   the port that we care about in this example.
5. Use a USB to serial device (such as a Silicon Labs's CP210x device). Connect
   the CP210x's RX pin to the board's TX pin. Connect the CP210x's TX pin to the
   board's RX pin. The port and pin mappings are listed below. Make sure the
   CP210x is listed under "Ports" in Device Manager.
6. Use a serial terminal device such as Termite and open up two connections.
   One connection will be to the CP210x serial device. The other will be to the
   USB CDC virtual com port.
7. Start typing in one of the terminal devices and press enter. If the output
   appears in the other terminal device then the project is working.

Note: If the program does not look like it is working, it might be because the
serial terminals' outputs are not being updated. To fix this, simply reconnect
to each the COM ports.

Note: After the project has been downloaded to the board (which can only be done
with the power switch in the DBG/AEM position), the board's power switch can
then be switched to the USB position and the USB type B mini can be unplugged.
However, the board can only be debugged with both USBs plugged in and the power
switch in the DBG/AEM position.

================================================================================

Peripherals Used:
HFXO - 48 MHz
USHFRCO - 48 MHz (used by the GG11 board instead of the HFXO by default)
LFXO - 32 kHz (used for low power mode)
USB

Note: the clock source selected for the USB must be 48 MHz.

================================================================================

Listed below are the boards that have a USB module
 - Giant Gecko (GG)
 - Leopard Gecko (LG)
 - Wonder Gecko (WG)
 - Happy Gecko (HG)
 - Giant Gecko 11 (GG11)

================================================================================

Listed below are the port and pin mappings for working with this example.

Board:  Silicon Labs EFM32GG Starter Kit (STK3700)
Device: EFM32GG990F1024
PD0 - USART1_TX (Expansion Header pin 4)
PD1 - USART1_RX (Expansion Header pin 6)

Board:  Silicon Labs EFM32LG Starter Kit (STK3600)
Device: EFM32LG990F256
PD0 - USART1_TX (Expansion Header pin 4)
PD1 - USART1_RX (Expansion Header pin 6)

Board:  Silicon Labs EFM32WG Starter Kit (STK3800)
Device: EFM32WG990F256
PD0 - USART1_TX (Expansion Header pin 4)
PD1 - USART1_RX (Expansion Header pin 6)

Board:  Silicon Labs EFM32HG Starter Kit (SLSTK3400A)
Device: EFM32HG322F64
PC0 - USART1_TX (Expansion Header pin 3)
PC1 - USART1_RX (Expansion Header pin 5)

Board:  Silicon Labs EFM32GG11 Starter Kit (SLSTK3701A)
Device: EFM32GG11B820F2048GL192
PE10 - USART0_TX (Expansion Header pin 4)
PE11 - USART0_RX (Expansion Header pin 6)

