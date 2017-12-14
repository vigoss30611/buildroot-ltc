/*
 * audio.h
 *
 * Synopsys Inc.
 * SG DWC PT02
 */
/** @file
 * audio module, initialisation and configuration. 
 */

#ifndef AUDIO_H_
#define AUDIO_H_

#include "types.h"
#include "audioParams.h"
/**
 * Initial set up of package and prepare it to be configured. Set audio mute to on.
 * @param baseAddr base Address of controller
 * @return TRUE if successful
 */
int audio_Initialize(u16 baseAddr);
/**
 * Configure hardware modules corresponding to user requirements to start transmitting audio packets.
 * @param baseAddr base Address of controller
 * @param params: audio parameters
 * @param pixelClk: pixel clock [0.01 MHz]
 * @param ratioClk: ratio clock (TMDS / Pixel) [0.01]
 * @return TRUE if successful
 */
int audio_Configure(u16 baseAddr, audioParams_t *params, u16 pixelClk, unsigned int ratioClk);
/**
 * Mute audio.
 * Stop sending audio packets
 * @param baseAddr base Address of controller
 * @param state:  1 to enable/0 to disable the mute
 * @return TRUE if successful
 */
int audio_Mute(u16 baseAddr, u8 state);
/**
 * The module is implemented for testing reasons. The method has no effect in
 *  the real application.
 * @param baseAddr base Address of audio generator module
 * @param params: audio parameters
 * @return TRUE if successful
 */
int audio_AudioGenerator(u16 baseAddr, audioParams_t *params);
/**
 * Compute the value of N to be used in a clock divider to generate an
 * intermediate clock that is slower than the 128·fS clock by the factor N.
 * The N value relates the TMDS to the sampling frequency fS.
 * @param baseAddr base Address of controller
 * @param freq: audio sampling frequency [Hz]
 * @param pixelClk: pixel clock [0.01 MHz]
 * @param ratioClk: ratio clock (TMDS / Pixel) [0.01]
 * @return N
 */
u16 audio_ComputeN(u16 baseAddr, u32 freq, u16 pixelClk, u16 ratioClk);

/**
 * Calculate the CTS value, the denominator of the fractional relationship
 * between the TMDS clock and the audio reference clock used in ACR
 * @param baseAddr base Address of controller
 * @param freq: audio sampling frequency [Hz]
 * @param pixelClk: pixel clock [0.01 MHz]
 * @param ratioClk: ratio clock (TMDS / Pixel) [0.01]
 * @return CTS value
 */
u32 audio_ComputeCts(u16 baseAddr, u32 freq, u16 pixelClk, u16 ratioClk);

#endif /* AUDIO_H_ */
