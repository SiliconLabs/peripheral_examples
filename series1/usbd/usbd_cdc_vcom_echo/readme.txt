
================================================================================

This project uses the USB module to implement a USB CDC device (Communications
Device Class) that acts as a VCOM port. This example uses the "VCOM port" to
implement a basic echo application. Data that is received from the USB host is
processed by the USB device and then sent back to the USB host. In this case,
the USB host is the computer and the USB device is the EFM32 board. This project
operates in EM1.

The src/descriptors.c file defines what kind of device is seen by the USB host.
It defines the device as a CDC device, the vendor ID, product ID, etc.

The inc/usbconfig.h file has #defines that can be modified to configure the USB
stack. These options include the clock source for low power mode, number of
endpoints used, the number of interfaces, etc.

The src/cdc.c file contains the CDC callback functions for handling device
state changes and USB host setup commands. It also contains RX/TX callback
functions that define the flow of data transfers. Once the device is in the
configured state, cdcStateChangeEvent() sets the usbTxActive global variable to
false to indicate that the TX line is not busy (i.e. we are waiting to receive
data from the USB host). The function then configures the device to be able to
read from the USB host's OUT endpoint. Once data is received from the USB host,
the callback function usbDataReceived() will be called (the callback was set
using the fourth argument to USBD_Read()). In usbDataReceived(), we check that
the status is OK and that we received more than 0 bytes. Then, we check to make
sure that the TX line is not busy before sending back the data and setting up
the endpoint for another read. Once the data has been transmitted,
usbDataTransmitted() gets called. Here, we simply update the global variable
usbTxActive to indicate that the tranmission has completed.

Note: The callback functions in src/cdc.c are named with respect to the USB
device (in this case the EFM32 board). For example, usbDataTransmitted() gets
called when the USB device transmits data over USB to the host.

Note: Endpoints are named with respect to the USB host (which conforms to the
USB standard). For example, CDC_EP_DATA_IN is the USB host's IN endpoint and
therefore the USB device's OUT endpoint. CDC_EP_DATA_OUT is the USB host's OUT
endpoint and therefore the USB device's IN endpoint.

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
5. Use a serial terminal device such as Termite and open up a connection to the
   USB CDC virtual com port (the COM port labeled "USB Serial Device").
6. Start typing and press enter. If successful, the data entered will be echoed
   back.

Note: If the program does not look like it is working, it might be because the
serial terminals' outputs are not being updated. To fix this, simply reconnect
to the COM port.

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

