dsp_lib_fft

This example is compatible with all boards using cortex M4 cores. This includes
Pearl, Blue, Flex, Mighty, and Giant 11 Gecko boards.

This project demonstrates performing windowing and FFT on a time domain signal
using the ARM CMSIS DSP_lib. A test 10kHz cosine is used for time domain data.
The signal is first windowed by a hamming window function. This prevents
artifacts in the FFT caused by discontinuity between the first and last 
sample. The hamming window also reduces spectral leakage compared to lower 
complexity windowing functions. The FFT is performed on the windowed signal,
and the magnitude response is calculated and stored in a seperate buffer.

How To Test:
1. Build the project and download to the Starter Kit
2. View globally declared complex frequency and magnitude response buffers

NOTE: To use CMSIS DSP_lib functions in your own projects, perform the
following steps.

For GNU ARM Compiler: 
  1. Include path to the CMSIS/Include directory
  2. Set compiler flags:
    __FPU_PRESENT = 1
    ARM_MATH_CM4 = 1
  3. add the "m" lib to Libraries
  4. In your source code, ensure you have included:
    #include "arm_math.h"
    #include <math.h>

For IAR
  1. Navigate to Project->Options->General Options->Library Configuration
  2. Check the "Use CMSIS" and "DSP Library" boxes.
  3. In your source code, ensure you have included:
    #include "arm_math.h"
    #include <math.h>

Peripherals used:
Board:  Silicon Labs EFM32PG1 Starter Kit (SLSTK3401A)
Device: EFM32PG1B200F256GM48

Board:  Silicon Labs EFM32PG12 Starter Kit (SLSTK3402A)
Device: EFM32PG12B500F1024GL125

Board:  Silicon Labs EFR32BG1P Starter Kit (BRD4100A)
Device: EFR32BG1P232F256GM48

Board:  Silicon Labs EFR32BG12P Starter Kit (BRD4103A)
Device: EFR32BG12P332F1024GL125

Board:  Silicon Labs EFR32FG1P Starter Kit (BRD4250A)
Device: EFR32FG1P133F256GM48

Board:  Silicon Labs EFR32FG12P Starter Kit (BRD4253A)
Device: EFR32FG12P433F1024GL125

Board:  Silicon Labs EFR32MG1P Starter Kit (BRD4151A)
Device: EFR32MG1P232F256GM48

Board:  Silicon Labs EFR32MG12P Starter Kit (BRD4161A)
Device: EFR32MG1P432F1024GL125

Board:  Silicon Labs EFM32WG Starter Kit (STK3800)
Device: EFM32WG990F256
