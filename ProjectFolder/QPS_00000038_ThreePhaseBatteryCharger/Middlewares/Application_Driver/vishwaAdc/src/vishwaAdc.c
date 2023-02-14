/*
 * vishwaAdc.c
 *
 *  Created on: Mar 11, 2022
 *      Author: vishweshgm
 */
#define VISHWA_ADC_C 1
#if VISHWA_ADC_C == 1
#include "vishwaAdc.h"
#include "stateMachine.h"
/*extern variable*/
extern SM_MainApplParameters SM_ApplParam;

#define FINDPEAK_NUM_OF_SAMPLES_SHUNT_CURRENT 100
float VISHWA_V1_MAX_SENSE_VOLTS = 1480;/* calculated as, for voltage divider
to give 3.3V, it should have input as 1500*/
float VISHWA_I1_MAX_SENSE_AMPS = 306;
//float VISHWA_I1_MAX_SENSE_AMPS = 303; /*calculated as, for NonI Amplifier to give 3.3V,
//it should have input 3.3/100=33mV, and from shunt specification we know 75mV for 75A.
//So 33A for 33mV*/
float VISHWA_I2_MAX_SENSE_AMPS = 64.5; /*calculated as, for voltage divider to give 3.3V,
 it should have input 4.3V. We know for 12V supply, Hall
 effect sensor gives +/-90A measurement. So for 6V, +90A measurement handled.
 So for 4.3V, 64.5A handled*/

static uint32_t FindPeakBufferHallCurrent[FINDPEAK_NUM_OF_SAMPLES_SHUNT_CURRENT];
/*>>>>>>>>>ADC Peripheral variables<<<<<<<<<<<<<<<<*/
uint32_t Appl_u32AdcRawValueVoltage;
uint32_t Appl_u32pMovingAvgArrayVoltage[MOVING_AVERAGE_SIZE_VOLTAGE];
uint32_t Appl_u32MovingAvgVoltageSum;
uint8_t Appl_u8MovingAvgPositionVoltage;
float Appl_fltAdcRawValueVoltage;


uint32_t Appl_u32AdcRawValueHallCurrent;
uint32_t Appl_u32pMovingAvgArrayHallCurrent[MOVING_AVERAGE_SIZE_HallCURRENT];
uint32_t Appl_u32MovingAvgHallCurrentSum;
uint8_t Appl_u8MovingAvgPositionHallCurrent;
float Appl_fltAdcRawValueHallCurrent;
float Appl_fltConverted_ValueHallCurrent;

uint32_t Appl_u32AdcRawValueShuntCurrent;
uint32_t Appl_u32pMovingAvgArrayShuntCurrent[MOVING_AVERAGE_SIZE_ShuntCURRENT];
uint32_t Appl_u32MovingAvgShuntCurrentSum;
uint8_t Appl_u8MovingAvgPositionShuntCurrent;
float Appl_fltAdcRawValueShuntCurrent;
float Appl_fltConverted_ValueShuntCurrent;

uint16_t Appl_u16pAdcDmaValues[4] = {0};

vishwaAdc Sinev1;
vishwaAdc Sinev2;
vishwaAdc Sinei1;
vishwaAdc Sinei2;
/*-----------------------------------------------------------------------------
ROUTINE NAME : ADC_getdata_filter_noise
INPUT        : void
OUTPUT       : unsigned int, returns the value after averaging
DESCRIPTION  : The Routine waits for NUMBER_OF_CONVERSIONS,
               filters the noise, and relaunch the conversions in case
               an erroronous ADC conversion is seen.
COMMENTS     : User has to select suitable values for
                #define NUMBER_OF_CONVERSIONS
				#define ADC_THRESHOLD
------------------------------------------------------------------------------*/
#define ADC_TEST 				(0)
#define NUMBER_OF_CONVERSIONS	(10)
#define ADC_THRESHOLD			(7)

void VishwaAdcInit(void){
	/*Vishwesh : 4 ADC configured, However only 2 being used
	 * ADC samples at rate of 10kHz. Good enough for 150 Hz pulses
	 * */

	memset(Appl_u16pAdcDmaValues,0,sizeof(Appl_u16pAdcDmaValues));
	if(HAL_OK != HAL_ADC_Start_DMA(&hadc1, (uint32_t*)Appl_u16pAdcDmaValues, 4)){
		Error_Handler();
	}
	RectifiedSineAnalyzer_Config(&Sinev1,10000,0.1);
	RectifiedSineAnalyzer_Config(&Sinev2,10000,0.1);
	RectifiedSineAnalyzer_Config(&Sinei1,10000,0.1);
	RectifiedSineAnalyzer_Config(&Sinei2,10000,0.1);
	Sinev1.Recalibrateflag = 0;
	Sinei1.Recalibrateflag = 0;
	Sinev2.Recalibrateflag = 0;
	Sinei2.Recalibrateflag = 0;
}



uint32_t ADC_normal_avg(uint32_t Appl_u32loaclvolt){
	static uint32_t total,count = 0;
	static uint32_t Appl_u32AdcAvg;
	count++;
	total+=Appl_u32loaclvolt;
	if(count >= 20){

		Appl_u32AdcAvg = total/count;
		count = 0;
		total = 0;
	}
	return Appl_u32AdcAvg;
}


uint32_t ADC_filter_noise(uint32_t Fnclocal_u32AdcVoltageValue)
{
    /* Declaration of the variables used in the application*/

	static uint8_t Fnclocal_u8Numofconversions;
    static uint32_t adc_data, Fnclocal_fltAdcAvg;
    static uint32_t adc_compare_value;
    static double total;

    Fnclocal_u8Numofconversions++;

	adc_data = Fnclocal_u32AdcVoltageValue;
      if (adc_compare_value == 0xffff)   /*** Check First time conversion ??  */
      {
      /* 1st conversion */
        adc_compare_value = adc_data;     /*** Take the value for comparision */
      }

         if (( adc_data < (adc_compare_value- ADC_THRESHOLD) )
             || ( adc_data > (adc_compare_value+ ADC_THRESHOLD) ))
          {
            Fnclocal_u8Numofconversions = 0;
            adc_compare_value = 0xffff;    /** Reset gain, for comparison  **/
            adc_data =0;               /**  Reset, Value will be overwritten **/
            total=0;                   /**  Reset for new calculation        **/
          }

      total += adc_data;               /**  Take the total for averaging     **/

      if(Fnclocal_u8Numofconversions == NUMBER_OF_CONVERSIONS){
    	  Fnclocal_u8Numofconversions = 0;
    	  Fnclocal_fltAdcAvg = total/NUMBER_OF_CONVERSIONS;
    	  total = 0;
    	  adc_compare_value = 0xffff;
      }
    return Fnclocal_fltAdcAvg;

}

float movingAvg(uint32_t *ptrArrNumbers, uint32_t *ptrSum, uint8_t pos, uint16_t len, uint32_t nextNum)
{
  //Subtract the oldest number from the prev sum, add the new number
  *ptrSum = *ptrSum - ptrArrNumbers[pos] + nextNum;
  //Assign the nextNum to the position in the array
  ptrArrNumbers[pos] = nextNum;
  //return the average
  return *ptrSum / len;
}

void Appl_fncCalculateFeedbackShuntCurrent(void){
	static uint16_t AdcCalibCounter;
	if(Sinei1.Recalibrateflag == 1){
		AdcCalibCounter++;
		Sinei1.SinemeasOffset = Appl_u16pAdcDmaValues[1] * ( VISHWA_ADC_4096_INVERSE);
		if(AdcCalibCounter > 25000){
			AdcCalibCounter = 0;
			Sinei1.Recalibrateflag = 0;
		}
	}
	else{
		Sinei1.SinemeasOffset = 0;
//		Sinei1.Sinemeaspu = (Appl_u16pAdcDmaValues[1] + Appl_u16pAdcDmaValues[3]) * VISHWA_ADC_4096_INVERSE * 0.5;
		Sinei1.Sinemeaspu = (Appl_u16pAdcDmaValues[1] ) * VISHWA_ADC_4096_INVERSE ;
		RectifiedSineAnalyzer(&Sinei1);
//		SM_ApplParam.RealtimeShuntCurrent = Sinei1.vRms * VISHWA_I1_MAX_SENSE_AMPS;
		SM_ApplParam.RealtimeShuntCurrent_vrms = Sinei1.vRms * VISHWA_I1_MAX_SENSE_AMPS;
		SM_ApplParam.RealtimeShuntCurrent_vavg = Sinei1.vAvg * VISHWA_I1_MAX_SENSE_AMPS;
	}
}

void Appl_fncCalculateFeedbackHallCurrent(void){
#if 0
	static uint16_t AdcCalibCounter;
#if 0
	Appl_u32AdcRawValueHallCurrent = (uint32_t)Appl_u16pAdcDmaValues[1];
	Appl_fltAdcRawValueHallCurrent = movingAvg(Appl_u32pMovingAvgArrayHallCurrent,&Appl_u32MovingAvgHallCurrentSum,Appl_u8MovingAvgPositionHallCurrent,MOVING_AVERAGE_SIZE_HallCURRENT, Appl_u32AdcRawValueHallCurrent);
	Appl_u8MovingAvgPositionHallCurrent++;
	if(Appl_u8MovingAvgPositionHallCurrent>=MOVING_AVERAGE_SIZE_HallCURRENT)Appl_u8MovingAvgPositionHallCurrent=0;
	SM_ApplParam.RealtimeHallCurrent = (Appl_fltAdcRawValueHallCurrent * 3300.0)/(4096.0);
//	SM_ApplParam.RealtimeHallCurrent = -.0008*SM_ApplParam.RealtimeHallCurrent*SM_ApplParam.RealtimeHallCurrent + .0416*SM_ApplParam.RealtimeHallCurrent + 1.1065;
//	SM_ApplParam.RealtimeHallCurrent = Appl_fltAdcRawValueHallCurrent * 3.3/(4096.0);
#endif
	if(Sinei2.Recalibrateflag == 1){
		AdcCalibCounter++;
		Sinei2.SinemeasOffset = 0.998 * (Sinei2.SinemeasOffset) +  0.001999 * Appl_u16pAdcDmaValues[1] * ( VISHWA_ADC_4096_INVERSE);
		if(AdcCalibCounter > 25000){
			AdcCalibCounter = 0;
			Sinei2.Recalibrateflag = 0;
		}
	}
	else{
		Sinei2.Sinemeaspu = Appl_u16pAdcDmaValues[1] * VISHWA_ADC_4096_INVERSE - Sinei2.SinemeasOffset;
		RectifiedSineAnalyzer(&Sinei2);
		SM_ApplParam.RealtimeHallCurrent = Sinei2.vRms * VISHWA_I2_MAX_SENSE_AMPS;
		SM_ApplParam.RealtimeHallCurrent_vrms = Sinei2.vRms * VISHWA_I2_MAX_SENSE_AMPS;
		SM_ApplParam.RealtimeHallCurrent_vavg = Sinei2.vAvg * VISHWA_I2_MAX_SENSE_AMPS;
	}
#endif
}

uint32_t findpeak(uint32_t rawval, uint32_t num_of_samples_to_collect){
	static uint32_t fnclocal_findpeakcounter = 0;
	static uint32_t peakValue = 0;
	FindPeakBufferHallCurrent[fnclocal_findpeakcounter] = rawval;
	fnclocal_findpeakcounter++;
	if(fnclocal_findpeakcounter >=num_of_samples_to_collect){
		fnclocal_findpeakcounter = 0;
		peakValue = FindPeakBufferHallCurrent[0];
		for(int i=0;i<num_of_samples_to_collect;i++){
			if(FindPeakBufferHallCurrent[i]> peakValue)peakValue = FindPeakBufferHallCurrent[i];
		}
	}
	return peakValue;
}
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
	Appl_fncCalculateFeedbackVoltage();
//	Appl_fncCalculateFeedbackHallCurrent();
	Appl_fncCalculateFeedbackShuntCurrent();
	SM_ApplParam.RealtimeCurrent = SM_ApplParam.RealtimeShuntCurrent_vavg;
	if((CAPCONTACTOR_STATE() == GPIO_PIN_RESET) || (DC_CONTACTOR_STATE() == GPIO_PIN_RESET)){
		SM_ApplParam.RealtimeVoltage = SM_ApplParam.Realtime_floatingVoltage_vavg;
		if(SM_ApplParam.RealtimeVoltage < 5) SM_ApplParam.RealtimeVoltage = 0;
		Appl_AdcPhaseAngleDetermination();
	}
	else{
		SM_ApplParam.RealtimeVoltage = 0;
	}

}


void Appl_fncCalculateFeedbackVoltage(void){
	static uint16_t AdcCalibCounter;
#if 0
	Appl_u32AdcRawValueVoltage = (uint32_t)Appl_u16pAdcDmaValues[0];
	Appl_u32AdcRawValueVoltage = ADC_filter_noise((uint32_t)Appl_u16pAdcDmaValues[0]);
	Appl_fltAdcRawValueVoltage = movingAvg(Appl_u32pMovingAvgArrayVoltage,&Appl_u32MovingAvgVoltageSum,Appl_u8MovingAvgPositionVoltage,MOVING_AVERAGE_SIZE_VOLTAGE, Appl_u32AdcRawValueVoltage);
	Appl_u8MovingAvgPositionVoltage++;
	if(Appl_u8MovingAvgPositionVoltage>=MOVING_AVERAGE_SIZE_VOLTAGE)Appl_u8MovingAvgPositionVoltage=0;
	SM_ApplParam.Realtime_floatingVoltage = ((Appl_fltAdcRawValueVoltage * 3.3)*(1)/((4096.0)*(0.002123142))) - 0.005;
#endif
#if 1
	if(Sinev1.Recalibrateflag == 1){
		AdcCalibCounter++;
		Sinev1.SinemeasOffset = Appl_u16pAdcDmaValues[0] * ( VISHWA_ADC_4096_INVERSE);
		if(AdcCalibCounter > 25000){
			AdcCalibCounter = 0;
			Sinev1.Recalibrateflag = 0;
		}
	}
	else{
		Sinev1.Sinemeaspu = Appl_u16pAdcDmaValues[0] * VISHWA_ADC_4096_INVERSE - Sinev1.SinemeasOffset;
		RectifiedSineAnalyzer(&Sinev1);
		SM_ApplParam.Realtime_floatingVoltage_vrms = Sinev1.vRms * VISHWA_V1_MAX_SENSE_VOLTS;
		SM_ApplParam.Realtime_floatingVoltage_vavg = Sinev1.vAvg * VISHWA_V1_MAX_SENSE_VOLTS;
	}
#endif
}

uint32_t noofsamples=200;

void RectifiedSineAnalyzer(vishwaAdc* v){
	v->nSamples++;
	v->vSum = v->vSum+v->Sinemeaspu;
    v->vSqrSum = v->vSqrSum+(v->Sinemeaspu*v->Sinemeaspu);
    v->vEma = v->vEma+(v->emaFilterMultiplier*(v->Sinemeaspu - v->vEma));
    if(v->nSamples >= noofsamples)
    {
        v->inverse_nSamples = (1.0f)/(v->nSamples);
        v->sqrt_inverse_nSamples = sqrtf(v->inverse_nSamples);
        v->vAvg = (v->vSum*v->inverse_nSamples);
        v->vRms = sqrtf(v->vSqrSum)*v->sqrt_inverse_nSamples;
        v->nSamples=0;
        v->vSum=0;
        v->vSqrSum=0;
    }
}

void RectifiedSineAnalyzer_Config(vishwaAdc* v,float isrFrequency, float threshold){
    v->sampleFreq = (float)(isrFrequency);
    v->threshold = (float)(threshold);
    v->emaFilterMultiplier=2.0f/isrFrequency;
}

#endif
