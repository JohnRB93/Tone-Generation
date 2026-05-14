/* x = value, y = bit to check */
#define CHECK_BIT(x, y) (((x) >> (y)) & 0x1)
/* x = value, y = bit to set */
#define BIT_SET(x, y) ((x) |= (0x1 << (y)))
/* x = value, y = bit to clear */
#define BIT_CLEAR(x, y) ((x) &= ~(0x1 << (y)))
/* x = value, y = bit to toggle */
#define BIT_TOGGLE(x, y) ((x) ^= (0x1 << (y)))

/*
 * **** Program Summary ****
 *
 * The goal of this program is to use the STM32 microcontroller to simulate a keyboard
 * instrument with buttons for keys and a joystick module for pitch bending.
 *
 * This program demonstrates the use of timer PWM output, multichannel ADC conversion with
 * DMA, and EXTI interrupts to simulate a keyboard instrument with buttons, an 8 Ohm speaker,
 * and a joystick module.
 *
 * Eight push buttons are connected to eight different GPIO pins of the microcontroller
 * and will cause the speaker connected to the PWM output to play a specified note with
 * the use of EXTI interrupts. The interrupt service routine of each pin connected to a
 * push button will play a note by setting the appropiate frequency required for the note.
 *
 * The ADC will convert analog inputs from the joystick module and use those values to alter
 * the frequency and/or pulse width of a currently playing note. The DMA is used in order to
 * manage the transfer of data from two ADC channels from the ADC data register (ADC_DR) to
 * specified locations in memory (in this case, elements of an array).
 *
 * For debugging purposes, a UART module is connected to pin PA2 to display analog measurements
 * and frequencies.
 */

/*
 * **** Connections ****
 * 
 * **PWM Output Signal**
 * Tim3, Channel 1 ------> PA6
 *
 * **ADC Converter with Joystick Module**
 * ADC1, Channel 10 -----> PC0
 * ADC1, Channel 11 -----> PC1
 *
 * **UART Module**
 * UART2 Tx -------------> PA2
 *
 * **Push Buttons**
 * NOTE_C4 Push Button --> PE7
 * NOTE_D4 Push Button --> PE8
 * NOTE_E4 Push Button --> PE9
 * NOTE_F4 Push Button --> PE10
 * NOTE_G4 Push Button --> PE11
 * NOTE_A4 Push Button --> PE12
 * NOTE_B4 Push Button --> PE13
 * NOTE_C5 Push Button --> PE14
 * */

#include<stdio.h>
#include<string.h>
#include<math.h>
#include"gpio.h"
#include"adc.h"
#include"irq.h"
#include"tim.h"
#include"uart.h"

#define ADC_MAX 3438.0
#define ADC_CONV_MAX 201.0

#define ANALOG_PIN_0 GPIO_PIN_NO_0
#define ANALOG_PIN_1 GPIO_PIN_NO_1

#define TIMER_PIN GPIO_PIN_NO_6

#define NOTE_C4			261.63
#define NOTE_D4			293.99
#define NOTE_E4			329.63
#define NOTE_F4			349.23
#define NOTE_G4			392.00
#define NOTE_A4			440.00
#define NOTE_B4			493.88
#define NOTE_C5			523.25

#define PIN_C4 GPIO_PIN_NO_7
#define PIN_D4 GPIO_PIN_NO_8
#define PIN_E4 GPIO_PIN_NO_9
#define PIN_F4 GPIO_PIN_NO_10
#define PIN_G4 GPIO_PIN_NO_11
#define PIN_A4 GPIO_PIN_NO_12
#define PIN_B4 GPIO_PIN_NO_13
#define PIN_C5 GPIO_PIN_NO_14

#define C4_BIT 0U
#define D4_BIT 1U
#define E4_BIT 2U
#define F4_BIT 3U
#define G4_BIT 4U
#define A4_BIT 5U
#define B4_BIT 6U
#define C5_BIT 7U

#define C4_BEND_UP_RANGE 0.3268
#define D4_BEND_UP_RANGE 0.3670
#define E4_BEND_UP_RANGE 0.4118
#define F4_BEND_UP_RANGE 0.4364
#define G4_BEND_UP_RANGE 0.4897
#define A4_BEND_UP_RANGE 0.5498
#define B4_BEND_UP_RANGE 0.6172
#define C5_BEND_UP_RANGE 0.6539

#define C4_BEND_DOWN_RANGE 0.2025
#define D4_BEND_DOWN_RANGE 0.2271
#define E4_BEND_DOWN_RANGE 0.2551
#define F4_BEND_DOWN_RANGE 0.2702
#define G4_BEND_DOWN_RANGE 0.3033
#define A4_BEND_DOWN_RANGE 0.3404
#define B4_BEND_DOWN_RANGE 0.3821
#define C5_BEND_DOWN_RANGE 0.4049

#define NORMAL_DUTY_CYCLE 50

/************ Global Variables ********************************************************************************/
uint8_t adcChannels[] = {ADC_IN10, ADC_IN11};
uint16_t analogData[] = {0, 0};

uint16_t CurrentAnalogReadX = 0;
uint16_t CurrentAnalogReadY = 0;
uint16_t PreviousAnalogReadX = 0;
uint16_t PreviousAnalogReadY = 0;

uint8_t butnsPressed = 0;
uint8_t toneFlag = 0;

float toneFrequency = 0.0;
float toneFrequencies[] = {NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_C5};

float pitchBendUpRanges[] = {
		C4_BEND_UP_RANGE, D4_BEND_UP_RANGE, E4_BEND_UP_RANGE, F4_BEND_UP_RANGE,
		G4_BEND_UP_RANGE, A4_BEND_UP_RANGE, B4_BEND_UP_RANGE, C5_BEND_UP_RANGE
};
float pitchBendUpRange = 0.0;

float pitchBendDownRanges[] = {
		C4_BEND_DOWN_RANGE, D4_BEND_DOWN_RANGE, E4_BEND_DOWN_RANGE, F4_BEND_DOWN_RANGE,
		G4_BEND_DOWN_RANGE, A4_BEND_DOWN_RANGE, B4_BEND_DOWN_RANGE, C5_BEND_DOWN_RANGE
};
float pitchBendDownRange = 0.0;
/**************************************************************************************************************/

/************ Function Headers ********************************************************************************/
void initPWM(void);
void setupAnalogPins(void);
void setupButtons(void);
/**************************************************************************************************************/

/************ Main Function ***********************************************************************************/
int main(void)
{
	uart_init();
	initPWM();
	setupButtons();
	setupAnalogPins();
	ADC_StartConversion(ADC1, ADC_REGULAR_GROUP);
	while(1){}
}
/**************************************************************************************************************/

/************ Function Definitions ****************************************************************************/
void initPWM(void) {
	// Enable and configure GPIOA for TIM3, channel 1
	GPIO_Init(GPIOA);
	GPIO_SetPinMode(GPIOA, TIMER_PIN, GPIO_MODE_ALTFN);
	GPIO_SetAlternateConfig(GPIOA, TIMER_PIN, GPIO_AF_02);
    // Enable and configure TIM3, channel 1
	TIM_Init(TIM3);
	TIM_SetPWM_Mode(TIM3, PWM_MODE_1);
}

void setupAnalogPins(void)
{
	//Setup for analog mode pins C0[ADC_IN10] and C1[ADC_IN11], ADC1.
	GPIO_Init(GPIOC);
	GPIO_SetPinMode(GPIOC, ANALOG_PIN_0, GPIO_MODE_ANALOG);
	GPIO_SetPinMode(GPIOC, ANALOG_PIN_1, GPIO_MODE_ANALOG);
	//Initialize the ADC1 peripheral and configure settings.
	ADC_Init(ADC1);
	ADC_SetBitResolution(ADC1, ADC_12BIT_RESOLUTION);
	ADC_RegChannelSel(ADC1, ADC_02_CONVERSIONS, adcChannels);
	ADC_SelectEOCFlagTrigger(ADC1, ADC_END_OF_SEQ);
	ADC_SetDataAlignment(ADC1, ADC_DATA_ALIGNMENT_RIGHT);
	ADC_SetPrescaler(ADC1, ADC_PCLK_DIV2);
	ADC_ConfigSampRate(ADC1, adcChannels[0], ADC_003_CYCLES);
	ADC_ConfigSampRate(ADC1, adcChannels[1], ADC_003_CYCLES);
	ADC_EnableContinuousMode(ADC1);
	ADC_EnableScanMode(ADC1);
	ADC_EnableConverter(ADC1);
	ADC_EnableDMA(ADC1, DMA2_Stream0, 0, 2, analogData);
	IRQInterruptConfig(DMA2_Stream0_IRQn, ENABLE);
}

void setupButtons(void)
{
	GPIO_Init(GPIOE);
	GPIO_EnableInterrupts(GPIOE, PIN_C4, GPIO_MODE_IT_RFT);
	GPIO_EnableInterrupts(GPIOE, PIN_D4, GPIO_MODE_IT_RFT);
	GPIO_EnableInterrupts(GPIOE, PIN_E4, GPIO_MODE_IT_RFT);
	GPIO_EnableInterrupts(GPIOE, PIN_F4, GPIO_MODE_IT_RFT);
	GPIO_EnableInterrupts(GPIOE, PIN_G4, GPIO_MODE_IT_RFT);
	GPIO_EnableInterrupts(GPIOE, PIN_A4, GPIO_MODE_IT_RFT);
	GPIO_EnableInterrupts(GPIOE, PIN_B4, GPIO_MODE_IT_RFT);
	GPIO_EnableInterrupts(GPIOE, PIN_C5, GPIO_MODE_IT_RFT);
	IRQInterruptConfig(EXTI9_5_IRQn, ENABLE);
	IRQInterruptConfig(EXTI15_10_IRQn, ENABLE);
}
/**************************************************************************************************************/

/************ IRQ Handling and Application Event Callbacks ****************************************************/

void EXTI9_5_IRQHandler(void)
{
	TIM_Delay_ms(TIM3, 50);
	if(GPIO_CheckInterruptPending(PIN_C4))
		EXTI9_5_IRQHandling(PIN_C4);
	else if(GPIO_CheckInterruptPending(PIN_D4))
		EXTI9_5_IRQHandling(PIN_D4);
	else if(GPIO_CheckInterruptPending(PIN_E4))
		EXTI9_5_IRQHandling(PIN_E4);
	else if(GPIO_CheckInterruptPending(PIN_F4))
		EXTI9_5_IRQHandling(PIN_F4);
}

void EXTI15_10_IRQHandler(void)
{
	TIM_Delay_ms(TIM3, 50);
	if(GPIO_CheckInterruptPending(PIN_F4))
		EXTI15_10_IRQHandling(PIN_F4);
	else if(GPIO_CheckInterruptPending(PIN_G4))
		EXTI15_10_IRQHandling(PIN_G4);
	else if(GPIO_CheckInterruptPending(PIN_A4))
		EXTI15_10_IRQHandling(PIN_A4);
	else if(GPIO_CheckInterruptPending(PIN_B4))
		EXTI15_10_IRQHandling(PIN_B4);
	else if(GPIO_CheckInterruptPending(PIN_C5))
		EXTI15_10_IRQHandling(PIN_C5);
}

void DMA2_Stream0_IRQHandler(void)
{
	ADC_HandleDMA_TransferFlag();
	CurrentAnalogReadY = (uint16_t)(((ADC_CONV_MAX / ADC_MAX) * analogData[0]) - (ADC_CONV_MAX / ADC_MAX));
	printf("Raw Analog Value:       %d\n", analogData[0]);
	printf("Converted Analog Value: %d\n", CurrentAnalogReadY);
	printf("Button Frequency:       %f\n", toneFrequency);
	//TIM_Delay_ms(TIM3, 50);
	if(toneFlag == 1)
	{

		if(CurrentAnalogReadY > 140) // Pitch is being bent higher than nornal level.
		{
			int diff = CurrentAnalogReadY - PreviousAnalogReadY;
			if(diff >= 2)
			{
				toneFrequency += (pitchBendUpRange * diff);
				printf("Frequency Output:       %f\n\n", toneFrequency);
				TIM_SetPWM_Frequency(TIM3, toneFrequency);
				TIM_SetPWM_DutyCycle(TIM3, toneFrequency, NORMAL_DUTY_CYCLE);
				PreviousAnalogReadY = CurrentAnalogReadY;
			}
			else if(diff <= -2)
			{
				toneFrequency += (pitchBendUpRange * diff);
				printf("Frequency Output:       %f\n\n", toneFrequency);
				TIM_SetPWM_Frequency(TIM3, toneFrequency);
				TIM_SetPWM_DutyCycle(TIM3, toneFrequency, NORMAL_DUTY_CYCLE);
				PreviousAnalogReadY = CurrentAnalogReadY;
			}
		}
		else if(CurrentAnalogReadY < 138) // Pitch is being bent lower than nornal level.
		{
			int diff = CurrentAnalogReadY - PreviousAnalogReadY;
			if(diff >= 2)
			{
				toneFrequency += (pitchBendDownRange * diff);
				printf("Frequency Output:       %f\n\n", toneFrequency);
				TIM_SetPWM_Frequency(TIM3, toneFrequency);
				TIM_SetPWM_DutyCycle(TIM3, toneFrequency, NORMAL_DUTY_CYCLE);
				PreviousAnalogReadY = CurrentAnalogReadY;
			}
			else if(diff <= -2)
			{
				toneFrequency += (pitchBendDownRange * diff);
				printf("Frequency Output:       %f\n\n", toneFrequency);
				TIM_SetPWM_Frequency(TIM3, toneFrequency);
				TIM_SetPWM_DutyCycle(TIM3, toneFrequency, NORMAL_DUTY_CYCLE);
				PreviousAnalogReadY = CurrentAnalogReadY;
			}
		}
	}
}

extern void GPIO_ApplicationEventCallBack(uint8_t pinNumber)
{
	for(uint8_t i = 0; i < 8; i++)
	{
		if(pinNumber == i + PIN_C4) // Chech witch EXTI line generated an interrupt.
		{
			if(!CHECK_BIT(butnsPressed, i)) // Check whether the button was pressed or released.
			{
				TIM_SartTimer(TIM3);
				toneFrequency = toneFrequencies[i];
				TIM_SetPWM_Frequency(TIM3, toneFrequency);
				TIM_SetPWM_DutyCycle(TIM3, toneFrequency, NORMAL_DUTY_CYCLE);
				toneFlag = 1;
				pitchBendUpRange = pitchBendUpRanges[i];
				pitchBendDownRange = pitchBendDownRanges[i];
				BIT_SET(butnsPressed, i);
			}
			else
			{
				TIM_StopTimer(TIM3);
				toneFrequency = 0;
				toneFlag = 0;
				pitchBendUpRange = 0.0;
				butnsPressed = 0;
			}
		}
	}
}

/**************************************************************************************************************/

/**************************************************************************************************************/
