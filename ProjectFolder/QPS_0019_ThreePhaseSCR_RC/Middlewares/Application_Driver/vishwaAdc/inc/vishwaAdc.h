/*
 * vishwaAdc.h
 *
 *  Created on: Mar 11, 2022
 *      Author: vishweshgm
 */

#ifndef APPLICATION_DRIVER_VISHWAADC_INC_VISHWAADC_H_
#define APPLICATION_DRIVER_VISHWAADC_INC_VISHWAADC_H_

#include "main.h"
typedef float float32_t;
typedef struct vishwaAdc_Tag{
	bool Recalibrateflag;
	float SinemeasOffset;
	float Sinemeaspu;
    float32_t sampleFreq;  //!< Input: Signal Sampling Freq
    float32_t threshold;   //!< Input: Voltage level corresponding to zero i/p
    float32_t vRms;        //!< Output: RMS Value
    float32_t vAvg;        //!< Output: Average Value
    float32_t vEma;        //!< Output: Exponential Moving Average Value
    float32_t vSum;        //!< Internal : running sum for vac calculation over one sine cycles
    float32_t vSqrSum;     //!< Internal : running sum for vacc square calculation over one sine cycle
    float32_t acFreqSum;   //!< Internal : running sum of acFreq
    int32_t  nSamples;     //!< Internal: No of samples in one cycle of the sine wave
    float32_t inverse_nSamples; //!< Internal: 1/( No of samples in one cycle of the sine wave)
    float32_t sqrt_inverse_nSamples; //!< Internal: sqrt(1/( No of samples in one cycle of the sine wave))
	float32_t emaFilterMultiplier;  //!< Internal: multiplier value used for the exponential moving average filter
}vishwaAdc;

#define VISHWA_ADC_4096_INVERSE  	((float)0.000244140625)

uint32_t findpeak(uint32_t rawval, uint32_t num_of_samples_to_collect);
void VishwaAdcInit(void);
void RectifiedSineAnalyzer_Config(vishwaAdc* v,float isrFrequency, float threshold);
void RectifiedSineAnalyzer(vishwaAdc* v);

extern ADC_HandleTypeDef hadc1;

#endif /* APPLICATION_DRIVER_VISHWAADC_INC_VISHWAADC_H_ */
