#ifndef __ADC_H_
#define __ADC_H_

#include<stdint.h>
#include"stm32f4xx.h"


/***************** Macro Definitions ***************************************************/

//@BitResolution
#define ADC_12BIT_RESOLUTION		0
#define ADC_10BIT_RESOLUTION		1
#define ADC_08BIT_RESOLUTION		2
#define ADC_06BIT_RESOLUTION		3

//@SamplingTime
#define ADC_003_CYCLES				0
#define ADC_015_CYCLES				1
#define ADC_028_CYCLES				2
#define ADC_056_CYCLES				3
#define ADC_084_CYCLES				4
#define ADC_112_CYCLES				5
#define ADC_144_CYCLES				6
#define ADC_480_CYCLES				7

//@EOC_Selection
#define ADC_END_OF_SEQ				0		/*End of each sequence of conversions.*/
#define ADC_END_OF_EACH				1		/*End of each conversion.*/

//@ConversionGroups
#define ADC_REGULAR_GROUP			0
#define ADC_INJECTED_GROUP			1

//@DataAlignment
#define ADC_DATA_ALIGNMENT_RIGHT	0
#define ADC_DATA_ALIGNMENT_LEFT		1

//@ClockPreScaler
#define ADC_PCLK_DIV2				0
#define ADC_PCLK_DIV4				1
#define ADC_PCLK_DIV6				2
#define ADC_PCLK_DIV8				3

//Analog Conversion Channels
#define ADC_IN0					0
#define ADC_IN1					1
#define ADC_IN2					2
#define ADC_IN3					3
#define ADC_IN4					4
#define ADC_IN5					5
#define ADC_IN6					6
#define ADC_IN7					7
#define ADC_IN8					8
#define ADC_IN9					9
#define ADC_IN10				10
#define ADC_IN11				11
#define ADC_IN12				12
#define ADC_IN13				13
#define ADC_IN14				14
#define ADC_IN15				15
#define ADC_IN16				16
#define ADC_IN17				17
#define ADC_IN18				18

//Number of Conversions
#define ADC_01_CONVERSIONS		0
#define ADC_02_CONVERSIONS		1
#define ADC_03_CONVERSIONS		2
#define ADC_04_CONVERSIONS		3
#define ADC_05_CONVERSIONS		4
#define ADC_06_CONVERSIONS		5
#define ADC_07_CONVERSIONS		6
#define ADC_08_CONVERSIONS		7
#define ADC_09_CONVERSIONS		8
#define ADC_10_CONVERSIONS		9
#define ADC_11_CONVERSIONS		10
#define ADC_12_CONVERSIONS		11
#define ADC_13_CONVERSIONS		12
#define ADC_14_CONVERSIONS		13
#define ADC_15_CONVERSIONS		14
#define ADC_16_CONVERSIONS		15

//Number of Discontinuous Conversions
#define ADC_DISC_NUM_1			0
#define ADC_DISC_NUM_2			1
#define ADC_DISC_NUM_3			2
#define ADC_DISC_NUM_4			3
#define ADC_DISC_NUM_5			4
#define ADC_DISC_NUM_6			5
#define ADC_DISC_NUM_7			6
#define ADC_DISC_NUM_8			7

//Analog Watchdog Channel Selection
#define ADC_WD_ALL_INJ_CH				0
#define ADC_WD_ALL_REG_CH				1
#define ADC_WD_ALL_REG_AND_INJ_CH		2
#define ADC_WD_SINGLE_INJ_CH			3
#define ADC_WD_SINGLE_REG_CH			4
#define ADC_WD_SINGLE_REG_OR_INJ_CH		5

//External Event Trigger Detection for both regular and injected groups
#define ADC_DETECTION_DISABLED		0
#define ADC_RISING_EDGE				1
#define ADC_FALLING_EDGE			2
#define ADC_RIS_FALL_EDGE			3

//External Event Selection for regular group
#define ADC_TIM1_CC1_EVENT			0
#define ADC_TIM1_CC2_EVENT			1
#define ADC_TIM1_CC3_EVENT			2
#define ADC_TIM2_CC2_EVENT			3
#define ADC_TIM2_CC3_EVENT			4
#define ADC_TIM2_CC4_EVENT			5
#define ADC_TIM2_TRGO_REG_EVENT		6
#define ADC_TIM3_CC1_EVENT			7
#define ADC_TIM3_TRGO_EVENT			8
#define ADC_TIM4_CC4_EVENT			9
#define ADC_TIM5_CC1_EVENT			10
#define ADC_TIM5_CC2_EVENT			11
#define ADC_TIM5_CC3_EVENT			12
#define ADC_TIM8_CC1_EVENT			13
#define ADC_TIM8_TRGO_EVENT			14
#define ADC_EXTI_LINE_11			15

//External Event Selection for injected group
#define ADC_TIM1_CC4_EVENT			0
#define ADC_TIM1_TRGO_EVENT			1
#define ADC_TIM2_CC1_EVENT			2
#define ADC_TIM2_TRGO_INJ_EVENT		3
#define ADC_TIM3_CC2_EVENT			4
#define ADC_TIM3_CC4_EVENT			5
#define ADC_TIM4_CC1_EVENT			6
#define ADC_TIM4_CC2_EVENT			7
#define ADC_TIM4_CC3_EVENT			8
#define ADC_TIM4_TRGO_EVENT			9
#define ADC_TIM5_CC4_EVENT			10
#define ADC_TIM5_TRGO_EVENT			11
#define ADC_TIM8_CC2_EVENT			12
#define ADC_TIM8_CC3_EVENT			13
#define ADC_TIM8_CC4_EVENT			14
#define ADC_EXTI_LINE_15			15

//ADC Interrupt Enable Selection
#define ADC_EOC_INTERRUPTS			0
#define ADC_JEOC_INTERRUPTS			1
#define ADC_AWD_INTERRUPTS			2
#define ADC_OVR_INTERRUPTS			3

//ADC Application Status  @ADC_State
#define ADC_OK							0
#define ADC_END_OF_CONVERSION_REG		1
#define ADC_END_OF_CONVERSION_INJ		2
#define ADC_WATCHDOG_SET				3
#define ADC_OVERRUN_SET					4


/***************************************************************************************/
/*                         APIs supported by this driver                               */
/***************************************************************************************/

void ADC_Init(ADC_TypeDef *ADC_T);
void ADC_DeInit(void);
void ADC_PeriClockControl(ADC_TypeDef *ADC_T, uint8_t EnOrDi);
void ADC_EnableConverter(ADC_TypeDef *ADC_T);

void ADC_SetBitResolution(ADC_TypeDef *ADC_T, uint8_t resolution);
void ADC_RegChannelSel(ADC_TypeDef *ADC_T, uint8_t conversions, uint8_t channels[]);
void ADC_InjChannelSel(ADC_TypeDef *ADC_T, uint8_t conversions, uint8_t channels[]);
void ADC_SelectEOCFlagTrigger(ADC_TypeDef *ADC_T, uint8_t eocSelection);
void ADC_SetDataAlignment(ADC_TypeDef *ADC_T, uint8_t dataAlignment);
void ADC_SetPrescaler(ADC_TypeDef *ADC_T, uint8_t prescaler);
void ADC_ConfigSampRate(ADC_TypeDef *ADC_T, uint8_t channel, uint8_t cycles);

void ADC_EnableContinuousMode(ADC_TypeDef *ADC_T);
void ADC_DisableContinuousMode(ADC_TypeDef *ADC_T);

void ADC_EnableScanMode(ADC_TypeDef *ADC_T);
void ADC_DisableScanMode(ADC_TypeDef *ADC_T);

void ADC_EnableDisContMode(ADC_TypeDef *ADC_T);
void ADC_DisableDisContMode(ADC_TypeDef *ADC_T);
void ADC_SetDisContNumber(ADC_TypeDef *ADC_T, uint8_t numDiscConversions);

void ADC_StartConversion(ADC_TypeDef *ADC_T, uint8_t group);
void ADC_StopConversion(ADC_TypeDef *ADC_T);

uint16_t ADC_ReadRegDR(ADC_TypeDef *ADC_T);
uint16_t ADC_ReadInjDR(ADC_TypeDef *ADC_T);

void ADC_ExtTrigDetect(ADC_TypeDef *ADC_T, uint8_t group, uint8_t detection);
void ADC_SelectExtEvReg(ADC_TypeDef *ADC_T, uint8_t group, uint8_t event);

void ADC_EnableWatchdog(ADC_TypeDef *ADC_T, uint8_t channelSelection);
void ADC_DisableWatchdog(ADC_TypeDef *ADC_T);
void ADC_SelectWatchdogChannel(ADC_TypeDef *ADC_T, uint8_t channel);
void ADC_SetWatchdogThresholds(ADC_TypeDef *ADC_T, uint16_t highThreshold, uint16_t lowThreshold);

void ADC_EnableInterrupts(ADC_TypeDef *ADC_T, uint8_t interruptSelection);
void ADC_DisableInterrupts(ADC_TypeDef *ADC_T, uint8_t interruptSelection);

void ADC_EnableDMA(ADC_TypeDef *ADC_T, DMA_Stream_TypeDef *DMASx, uint8_t dmaChannel, uint8_t numChannels, uint16_t *dest);
void ADC_DisableDMA(ADC_TypeDef *ADC_T, DMA_Stream_TypeDef *DMASx);
void ADC_HandleDMA_TransferFlag(void);

void ADC_IRQHandling(ADC_TypeDef *ADC_T);

/***************************************************************************************/


#endif /* __ADC_H_ */
