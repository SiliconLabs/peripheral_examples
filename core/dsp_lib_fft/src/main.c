/***************************************************************************//**
 * @file main.c
 * @brief This project demonstrates use of the CMSIS DSP_lib to perform an FFT
 * on a sample cosine wave. See readme for information about setting up
 * projects to use DSP_lib.
 * @version 0.0.1
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/
#include "em_system.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "arm_math.h"
#include <math.h>

// Defines size of FFT, test data is 128 samples
// Supported arm_fft_f32 lengths are 128, 512, 2048
#define FFTSIZE 128

// Defines sample frequency of input data
// Use for calculating FFT resolution:
// Resolution = SAMPLEFREQ / FFTSIZE
#define SAMPLEFREQ 48000

// Instance structures for float32_t RFFT
static arm_rfft_instance_f32 rfft_instance;
// Instance structure for float32_t CFFT used by the RFFT
static arm_cfft_radix4_instance_f32 cfft_instance;

// Buffers to store complex frequency, and magnitude response data
float32_t freqBuffer[FFTSIZE * 2];
float32_t magnitudeResponse[FFTSIZE];

// 128 point Hamming window
// To window real time data, Hamming function is:
// w = .54 - (1 - .54)*cos(2*pi*SAMPLEINDEX /(FFTSIZE - 1))
float32_t hammingWindow[128] =
{
    0.0800,    0.0806,    0.0823,    0.0851,    0.0890,    0.0940,    0.1001,
    0.1073,    0.1156,    0.1249,    0.1352,    0.1465,    0.1587,    0.1719,
    0.1860,    0.2010,    0.2167,    0.2333,    0.2507,    0.2687,    0.2874,
    0.3067,    0.3266,    0.3470,    0.3679,    0.3892,    0.4109,    0.4329,
    0.4551,    0.4776,    0.5002,    0.5229,    0.5457,    0.5684,    0.5911,
    0.6136,    0.6360,    0.6581,    0.6800,    0.7015,    0.7226,    0.7432,
    0.7634,    0.7830,    0.8020,    0.8204,    0.8381,    0.8551,    0.8712,
    0.8866,    0.9012,    0.9148,    0.9275,    0.9393,    0.9501,    0.9599,
    0.9687,    0.9764,    0.9831,    0.9886,    0.9931,    0.9965,    0.9987,
    0.9999,    0.9999,    0.9987,    0.9965,    0.9931,    0.9886,    0.9831,
    0.9764,    0.9687,    0.9599,    0.9501,    0.9393,    0.9275,    0.9148,
    0.9012,    0.8866,    0.8712,    0.8551,    0.8381,    0.8204,    0.8020,
    0.7830,    0.7634,    0.7432,    0.7226,    0.7015,    0.6800,    0.6581,
    0.6360,    0.6136,    0.5911,    0.5684,    0.5457,    0.5229,    0.5002,
    0.4776,    0.4551,    0.4329,    0.4109,    0.3892,    0.3679,    0.3470,
    0.3266,    0.3067,    0.2874,    0.2687,    0.2507,    0.2333,    0.2167,
    0.2010,    0.1860,    0.1719,    0.1587,    0.1465,    0.1352,    0.1249,
    0.1156,    0.1073,    0.1001,    0.0940,    0.0890,    0.0851,    0.0823,
    0.0806,    0.0800
};

// Test data of a 10kHz cosine wave
float32_t testData[128] =
{
  0,9.6592579,5,-7.0710678,-8.6602535,2.5881906,10,2.5881906,-8.6602535,
  -7.0710678,5,9.6592579,6.1232342e-15,-9.6592579,-5,7.0710678,8.6602535,
  -2.5881906,-10,-2.5881906,8.6602535,7.0710678,-5,-9.6592579,-1.2246468e-14,
  9.6592579,5,-7.0710678,-8.6602535,2.5881906,10,2.5881906,-8.6602535,
  -7.0710678,5,9.6592579,-1.7157435e-14,-9.6592579,-5,7.0710678,8.6602535,
  -2.5881906,-10,-2.5881906,8.6602535,7.0710678,-5,-9.6592579,-2.4492937e-14,
  9.6592579,5,-7.0710678,-8.6602535,2.5881906,10,2.5881906,-8.6602535,
  -7.0710678,5,9.6592579,-4.9109666e-15,-9.6592579,-5,7.0710678,8.6602535,
  -2.5881906,-10,-2.5881906,8.6602535,7.0710678,-5,-9.6592579,3.4314870e-14,
  9.6592579,5,-7.0710678,-8.6602535,2.5881906,10,2.5881906,-8.6602535,
  -7.0710678,5,9.6592579,-6.3718772e-14,-9.6592579,-5,7.0710678,8.6602535,
  -2.5881906,-10,-2.5881906,8.6602535,7.0710678,-5,-9.6592579,-4.8985874e-14,
  9.6592579,5,-7.0710678,-8.6602535,2.5881906,10,2.5881906,-8.6602535,
  -7.0710678,5,9.6592579,-2.6463513e-13,-9.6592579,-5,7.0710678,8.6602535,
  -2.5881906,-10,-2.5881906,8.6602535,7.0710678,-5,-9.6592579,9.8219332e-15,
  9.6592579,5,-7.0710678,-8.6602535,2.5881906,10,2.5881906
};

/**************************************************************************//**
 * @brief Perform FFT and extract signal frequency content
 *****************************************************************************/
int main()
{
  // Initialize FFTSIZE point rfft configuration.
  // Note valid FFTSIZE values are 128, 512, and 2048
  arm_rfft_init_f32(&rfft_instance, &cfft_instance, FFTSIZE, 0, 1);

  // Window time domain data
  // Windowing removes discontinuities between first and last time-domain sample
  // The Hamming window function also reduces spectral leakage
  for(int i = 0; i < FFTSIZE; i++)
  {
    testData[i] = testData[i] * hammingWindow[i];
  }

  // Perform FFT and calculate magnitude
  // Uses test waveform as time domain data
  arm_rfft_f32(&rfft_instance, testData, freqBuffer);
  arm_cmplx_mag_f32(freqBuffer, magnitudeResponse, FFTSIZE);

  while(1)
  {
    EMU_EnterEM1();
  }
}

