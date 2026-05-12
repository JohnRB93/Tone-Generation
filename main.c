//#define _ANALOG_INTERRUPTS_SINGLE_CONV_UART_CONSOL
//#define _TIMER_BLINK
//#define _LCD_DISPLAY
//#define _ANALOG_MEASUREMENT_LCD
//#define _ANALOG_MEASUREMENT_LCD_UART
//#define _USER_PUSHBUTTON_INTERRUPT_LEDS
//#define _SPI_DATA_TX_ONLY
//#define _SPI_DATA_TX_RX
#define _TONE_GENERATION
//#define _TEST


/* x = value, y = bit to check */
#define CHECK_BIT(x, y) (((x) >> (y)) & 0x1)
/* x = value, y = bit to set */
#define BIT_SET(x, y) ((x) |= (0x1 << (y)))
/* x = value, y = bit to clear */
#define BIT_CLEAR(x, y) ((x) &= ~(0x1 << (y)))
/* x = value, y = bit to toggle */
#define BIT_TOGGLE(x, y) ((x) ^= (0x1 << (y)))


#ifdef _ANALOG_INTERRUPTS_SINGLE_CONV_UART_CONSOL

/*
 * This program uses the Analog to Digital Conversion peripheral to convert an analog
 * input to a digital value and display it on a computer console. A UART module is used
 * to enable serial communication between the microcontroller and the computer. The value
 * of the analog input is determined by the use of a potentiometer. The ADC peripheral in
 * this case uses interrupts each time a conversion is created.
 */

#include<stdio.h>
#include"uart.h"
#include"adc.h"
#include"irq.h"
#include"gpio.h"
#include"tim.h"

/*
 * UART -> PA2
 * ADC1 -> PC0
 */

GPIO_PinConfig_t gpioConfig;
ADC_Config_t adcConfig;

uint8_t adcChannels[] = {ADC_IN10};
uint16_t analogRead = 0;

void configGpioPinPC0_analog(void);
void initTim(void);

int main(void)
{
	uart_init();
	printf("%s", "Program Start\n");

	initTim();
	configGpioPinPC0_analog();
	ADC_StartConversion(ADC1, adcConfig.ADC_ConvGroup, ADC_SINL_CONV_MODE);

	while(1){}
}

void configGpioPinPC0_analog(void)
{
	// Setup for analog mode pin C0, ADC1
	gpioConfig.GPIO_PinNumber = GPIO_PIN_NO_0;
	gpioConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
	gpioConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GPIO_Init(GPIOC, gpioConfig);

	adcConfig.ADC_BitRes = ADC_12BIT_RESOLUTION;
	adcConfig.ADC_SampTime = ADC_003_CYCLES;
	adcConfig.ADC_EOCSelect = ADC_END_OF_EACH;
	adcConfig.ADC_ConvGroup = ADC_REGULAR_GROUP;
	adcConfig.ADC_DataAlign = ADC_DATA_ALIGNMENT_RIGHT;
	adcConfig.ADC_ScanMode = 0;
	adcConfig.ADC_ClkPreSclr = ADC_PCLK_DIV2;
	adcConfig.ADC_ItEnable = ADC_INTERRUPT_ENABLE;
	adcConfig.ADC_WtDgEnable = ADC_WATCHDOG_DISABLE;
	adcConfig.ADC_DMAEnable = ADC_DMA_DISABLE;
	ADC_Init(ADC1, adcConfig);
	ADC_RegChannelSel(ADC1, ADC_01_CONVERSIONS, adcChannels);
	ADC_ConfigSampRate(ADC1, adcChannels[0], adcConfig.ADC_SampTime);
	ADC_SelectEOCFlagTrigger(ADC1, adcConfig);
	IRQInterruptConfig(ADC_IRQn, ENABLE);
}

void initTim(void)
{
	TIM_Config_t timConfig;
	timConfig.arpEnable = TIMX_ARR_REG_NOT_BUFF;
	timConfig.centerAligneMode = COUNT_EDGE_ALIGNED_MODE;
	timConfig.clockSel = INTERNAL_CLOCK;
	timConfig.counterMode = COUNT_DOWN_MODE;
	timConfig.pulseMode = PULSE_MODE_NOT_ENABLE;
	timConfig.pwmMode = TIMX_PWM_MODE_NOT_ENABLE;
	TIM_Init(TIM2, timConfig);
}

void ADC_IRQHandler(void)
{
	ADC_IRQHandling(ADC1);
}

extern void ADC_ApplicationEventCallback(uint8_t appEv)
{
	if(appEv == ADC_END_OF_CONVERSION_REG || appEv == ADC_END_OF_CONVERSION_INJ)
	{
		analogRead = (((255.0 / 4094) * ADC_ReadRegDR(ADC1)) - (255.0 / 4094));
		printf("%d\n", analogRead);
		ADC_StartConversion(ADC1, adcConfig.ADC_ConvGroup, ADC_SINL_CONV_MODE);
	}
	else if(appEv == ADC_OVERRUN_SET)
		ADC_StartConversion(ADC1, adcConfig.ADC_ConvGroup, ADC_SINL_CONV_MODE);

	TIM_Delay_ms(TIM2, 50);
}

#endif /* _ANALOG_INTERRUPTS_SINGLE_CONV_UART_CONSOL */

#ifdef _TIMER_BLINK

/*
 * This program uses Timer2 peripheral to blink one of the on-board leds (GPIOD pin 15)
 * at a predetermined interval. This program along with the use of either a logic analyzer or
 * an oscilloscope help to verify the accuracy of the timer's delay function.
 */


#include"gpio.h"
#include"tim.h"

#define LED_PIN GPIO_PIN_NO_15

void initLED(void);
void initTim(void);

int main(void)
{
	initLED();
	initTim();

	while(1)
	{
		GPIO_WriteToOutputPin(GPIOD, LED_PIN, SET);
		TIM_Delay_ms(TIM2, 1000);
		GPIO_WriteToOutputPin(GPIOD, LED_PIN, RESET);
		TIM_Delay_ms(TIM2, 1000);
	}
}

void initLED(void)
{
	GPIO_Init(GPIOD);
	GPIO_SetPinMode(GPIOD, LED_PIN, GPIO_MODE_OUT);
	GPIO_SetOutputType(GPIOD, LED_PIN, GPIO_OP_TYPE_PP);
	GPIO_SetPinSpeed(GPIOD, LED_PIN, GPIO_SPEED_HIGH);
	GPIO_SetPullUpPullDown(GPIOD, LED_PIN, GPIO_PIN_PU);
}

void initTim(void)
{
	TIM_Init(TIM2);
	TIM_SetCounterMode(TIM2, COUNT_UP_MODE);
}

#endif /* _TIMER_BLINK */

#ifdef _LCD_DISPLAY

/*
 * This program uses the LCD_Print() function to display variables onto a 2x16 LCD module.
 * The LCD module is used in 8-bit mode and requires 11 connections in total. Most of the GPIOD
 * peripheral is used to acheive the necessary connections. A potentiometer is used to alter the
 * analog input value.
 */

#include<stdio.h>
#include<string.h>
#include"lcd2_16.h"
#include"tim.h"

/*
 * GPIO_PIN_PD0  -> LCD_DB0
 * GPIO_PIN_PD1  -> LCD_DB1
 * GPIO_PIN_PD2  -> LCD_DB2
 * GPIO_PIN_PD3  -> LCD_DB3
 * GPIO_PIN_PD4  -> LCD_DB4
 * GPIO_PIN_PD5  -> LCD_DB5
 * GPIO_PIN_PD6  -> LCD_DB6
 * GPIO_PIN_PD7  -> LCD_DB7
 * GPIO_PIN_PD8  -> LCD_RW
 * GPIO_PIN_PD9  -> LCD_RS
 * GPIO_PIN_PD10 -> LCD_E
 */

int main(void)
{
	LCD_Init8Bit();
	uint32_t numA = 50;
	uint32_t numB = 5;
	uint32_t numC = numA * numB;
	char* myStr = "Let's do math: %d * %d = %d\n";
	LCD_Print(myStr, numA, numB, numC);
	myStr = "It's fun when it works!";
	LCD_Print(myStr);
	while(1)
	{
		if(LCD_GetScrollFlag())
		{
			LCD_ScrollDisplay();
		}
	}
}

#endif /* _LCD_DISPLAY */

#ifdef _ANALOG_MEASUREMENT_LCD

/*
 * This program uses the Analog to Digital Conversion peripheral to convert an analog
 * input to a digital value and display it on a 2x16 LCD module. The LCD module uses
 * 8-bit configuration as well as the Timer 2 peripheral. The value of the analog input
 * is determined by the use of a potentiometer.
 */

#include<stdio.h>
#include<string.h>
#include"lcd2_16.h"
#include"tim.h"
#include"adc.h"
#include"irq.h"
#include"gpio.h"

#define POT_MAX 10400U
#define ADC_MAX 4094.0

#define ANALOG_PIN GPIO_PIN_NO_0

#define RED_LED GPIO_PIN_NO_0
#define YELLOW_LED GPIO_PIN_NO_1
#define GREEN_LED GPIO_PIN_NO_2

/*
 * GPIO_PIN_PC0  -> ADC1
 *
 * GPIO_PIN_PD0  -> LCD_DB0
 * GPIO_PIN_PD1  -> LCD_DB1
 * GPIO_PIN_PD2  -> LCD_DB2
 * GPIO_PIN_PD3  -> LCD_DB3
 * GPIO_PIN_PD4  -> LCD_DB4
 * GPIO_PIN_PD5  -> LCD_DB5
 * GPIO_PIN_PD6  -> LCD_DB6
 * GPIO_PIN_PD7  -> LCD_DB7
 * GPIO_PIN_PD8  -> LCD_RW
 * GPIO_PIN_PD9  -> LCD_RS
 * GPIO_PIN_PD10 -> LCD_E
 *
 * GPIO_PIN_PB0  -> Red LED
 * GPIO_PIN_PB1  -> Yellow LED
 * GPIO_PIN_PB2  -> Green LED
 */

/************ Global Variables ********************************************************************************/
uint8_t adcChannels[] = {ADC_IN10};
uint16_t analogRead = 0;
/**************************************************************************************************************/

/************ Function Headers ********************************************************************************/
void configGpioPinPC0_analog(void);
void configGpioLEDs(void);
/**************************************************************************************************************/

/************ Main Function ***********************************************************************************/
int main(void)
{
	LCD_Init8Bit();
	configGpioPinPC0_analog();
	configGpioLEDs();
	ADC_StartConversion(ADC1, ADC_REGULAR_GROUP);

	while(1){}
}
/**************************************************************************************************************/

/************ Function Definitions ****************************************************************************/
void configGpioPinPC0_analog(void)
{
	// Setup for analog mode pin C0, ADC1
	GPIO_Init(GPIOC);
	GPIO_SetPinMode(GPIOC, ANALOG_PIN, GPIO_MODE_ANALOG);
	GPIO_SetPinSpeed(GPIOC, ANALOG_PIN, GPIO_SPEED_HIGH);

	ADC_Init(ADC1);
	ADC_SetBitResolution(ADC1, ADC_12BIT_RESOLUTION);
	ADC_RegChannelSel(ADC1, ADC_01_CONVERSIONS, adcChannels);
	ADC_SelectEOCFlagTrigger(ADC1, ADC_END_OF_EACH);
	ADC_SetDataAlignment(ADC1, ADC_DATA_ALIGNMENT_RIGHT);
	ADC_SetPrescaler(ADC1, ADC_PCLK_DIV2);
	ADC_ConfigSampRate(ADC1, adcChannels[0], ADC_003_CYCLES);
	ADC_EnableInterrupts(ADC1, ADC_EOC_INTERRUPTS);
	IRQInterruptConfig(ADC_IRQn, ENABLE);
}

void configGpioLEDs(void)
{
	GPIO_Init(GPIOB);
	// Configure Red LED
	GPIO_SetPinMode(GPIOB, RED_LED, GPIO_MODE_OUT);
	GPIO_SetOutputType(GPIOB, RED_LED, GPIO_OP_TYPE_PP);
	GPIO_SetPinSpeed(GPIOB, RED_LED, GPIO_SPEED_HIGH);
	GPIO_SetPullUpPullDown(GPIOB, RED_LED, GPIO_PIN_PU);
	// Configure Yellow LED
	GPIO_SetPinMode(GPIOB, YELLOW_LED, GPIO_MODE_OUT);
	GPIO_SetOutputType(GPIOB, YELLOW_LED, GPIO_OP_TYPE_PP);
	GPIO_SetPinSpeed(GPIOB, YELLOW_LED, GPIO_SPEED_HIGH);
	GPIO_SetPullUpPullDown(GPIOB, YELLOW_LED, GPIO_PIN_PU);
	// Configure Green LED
	GPIO_SetPinMode(GPIOB, GREEN_LED, GPIO_MODE_OUT);
	GPIO_SetOutputType(GPIOB, GREEN_LED, GPIO_OP_TYPE_PP);
	GPIO_SetPinSpeed(GPIOB, GREEN_LED, GPIO_SPEED_HIGH);
	GPIO_SetPullUpPullDown(GPIOB, GREEN_LED, GPIO_PIN_PU);

	GPIOB->MODER = 0x15;
}
/**************************************************************************************************************/

/************ IRQ Handling and Application Event Callbacks ****************************************************/
void ADC_IRQHandler(void)
{
	ADC_IRQHandling(ADC1);
}

extern void ADC_ApplicationEventCallback(uint8_t appEv)
{
	if(appEv == ADC_END_OF_CONVERSION_REG || appEv == ADC_END_OF_CONVERSION_INJ)
	{
		analogRead = (((255.0 / ADC_MAX) * ADC_ReadRegDR(ADC1)) - (255.0 / ADC_MAX));
		LCD_Print("Value: %u   ", analogRead);
		LCD_SetRowBottom();

		if(analogRead > 245)
		{
			LCD_Print("%s%c", "Red Light     ", 'R');
			GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_0, SET);
			GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_1, RESET);
			GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_2, RESET);
		}
		else if(analogRead <= 245 && analogRead > 55)
		{
			LCD_Print("%s%c", "Yellow Light  ", 'Y');
			GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_0, RESET);
			GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_1, SET);
			GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_2, RESET);
		}
		else
		{
			LCD_Print("%s%c", "Green Light   ", 'G');
			GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_0, RESET);
			GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_1, RESET);
			GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_2, SET);
		}

		ADC_StartConversion(ADC1, ADC_REGULAR_GROUP);
	}
	else if(appEv == ADC_OVERRUN_SET)
		ADC_StartConversion(ADC1, ADC_REGULAR_GROUP);

	LCD_SetRowTop();
	TIM_Delay_ms(TIM2, 25);
}

#endif /* _ANALOG_MEASUREMENT_LCD */

#ifdef _ANALOG_MEASUREMENT_LCD_UART

/*
 * This program uses the Analog to Digital Conversion peripheral to convert an analog
 * input to a digital value and display it on a 2x16 LCD module. The LCD module uses
 * 8-bit configuration as well as the Timer 2 peripheral. The value of the analog input
 * is determined by the use of a potentiometer. Functionallity of the LCD_Print() function
 * is demonstrated by printing the ADC converted value on the LCD module. Three LEDs will
 * light up on a bread board depending on the value. The contents printed on the LCD module
 * are also printed on a console via a UART USB module.
 */

#include<stdio.h>
#include<string.h>
#include"lcd2_16.h"
#include"tim.h"
#include"adc.h"
#include"irq.h"
#include"gpio.h"
#include"uart.h"

#define POT_MAX 10400U
#define ADC_MAX 4094.0

#define ANALOG_PIN GPIO_PIN_NO_0

#define RED_LED GPIO_PIN_NO_0
#define YELLOW_LED GPIO_PIN_NO_1
#define GREEN_LED GPIO_PIN_NO_2

/*
 * GPIO_PIN_PC0  -> ADC1
 *
 * GPIO_PIN_PA2  -> UART2
 *
 * GPIO_PIN_PD0  -> LCD_DB0
 * GPIO_PIN_PD1  -> LCD_DB1
 * GPIO_PIN_PD2  -> LCD_DB2
 * GPIO_PIN_PD3  -> LCD_DB3
 * GPIO_PIN_PD4  -> LCD_DB4
 * GPIO_PIN_PD5  -> LCD_DB5
 * GPIO_PIN_PD6  -> LCD_DB6
 * GPIO_PIN_PD7  -> LCD_DB7
 * GPIO_PIN_PD8  -> LCD_RW
 * GPIO_PIN_PD9  -> LCD_RS
 * GPIO_PIN_PD10 -> LCD_E
 *
 * GPIO_PIN_PB0  -> Red LED
 * GPIO_PIN_PB1  -> Yellow LED
 * GPIO_PIN_PB2  -> Green LED
 */

/************ Global Variables ********************************************************************************/
uint8_t adcChannels[] = {ADC_IN10};
uint16_t analogRead = 0;
/**************************************************************************************************************/

/************ Function Headers ********************************************************************************/
void configGpioPinPC0_analog(void);
void configGpioLEDs(void);
/**************************************************************************************************************/

/************ Main Function ***********************************************************************************/
int main(void)
{
	LCD_Init8Bit();
	configGpioPinPC0_analog();
	uart_init();
	printf("Progam Start\n\n");
	configGpioLEDs();
	ADC_StartConversion(ADC1, ADC_REGULAR_GROUP);

	while(1){}
}
/**************************************************************************************************************/

/************ Function Definitions ****************************************************************************/
void configGpioPinPC0_analog(void)
{
	// Setup for analog mode pin C0, ADC1
	GPIO_Init(GPIOC);
	GPIO_SetPinMode(GPIOC, ANALOG_PIN, GPIO_MODE_ANALOG);
	GPIO_SetPinSpeed(GPIOC, ANALOG_PIN, GPIO_SPEED_HIGH);

	ADC_Init(ADC1);
	ADC_SetBitResolution(ADC1, ADC_12BIT_RESOLUTION);
	ADC_RegChannelSel(ADC1, ADC_01_CONVERSIONS, adcChannels);
	ADC_SelectEOCFlagTrigger(ADC1, ADC_END_OF_EACH);
	ADC_SetDataAlignment(ADC1, ADC_DATA_ALIGNMENT_RIGHT);
	ADC_SetPrescaler(ADC1, ADC_PCLK_DIV2);
	ADC_ConfigSampRate(ADC1, adcChannels[0], ADC_003_CYCLES);
	ADC_EnableInterrupts(ADC1, ADC_EOC_INTERRUPTS);
	IRQInterruptConfig(ADC_IRQn, ENABLE);
}

void configGpioLEDs(void)
{
	GPIO_Init(GPIOB);
	// Configure Red LED
	GPIO_SetPinMode(GPIOB, RED_LED, GPIO_MODE_OUT);
	GPIO_SetOutputType(GPIOB, RED_LED, GPIO_OP_TYPE_PP);
	GPIO_SetPinSpeed(GPIOB, RED_LED, GPIO_SPEED_HIGH);
	GPIO_SetPullUpPullDown(GPIOB, RED_LED, GPIO_PIN_PU);
	// Configure Yellow LED
	GPIO_SetPinMode(GPIOB, YELLOW_LED, GPIO_MODE_OUT);
	GPIO_SetOutputType(GPIOB, YELLOW_LED, GPIO_OP_TYPE_PP);
	GPIO_SetPinSpeed(GPIOB, YELLOW_LED, GPIO_SPEED_HIGH);
	GPIO_SetPullUpPullDown(GPIOB, YELLOW_LED, GPIO_PIN_PU);
	// Configure Green LED
	GPIO_SetPinMode(GPIOB, GREEN_LED, GPIO_MODE_OUT);
	GPIO_SetOutputType(GPIOB, GREEN_LED, GPIO_OP_TYPE_PP);
	GPIO_SetPinSpeed(GPIOB, GREEN_LED, GPIO_SPEED_HIGH);
	GPIO_SetPullUpPullDown(GPIOB, GREEN_LED, GPIO_PIN_PU);

	GPIOB->MODER = 0x15;
}
/**************************************************************************************************************/

/************ IRQ Handling and Application Event Callbacks ****************************************************/
void ADC_IRQHandler(void)
{
	ADC_IRQHandling(ADC1);
}

extern void ADC_ApplicationEventCallback(uint8_t appEv)
{
	if(appEv == ADC_END_OF_CONVERSION_REG || appEv == ADC_END_OF_CONVERSION_INJ)
	{
		analogRead = (((255.0 / ADC_MAX) * ADC_ReadRegDR(ADC1)) - (255.0 / ADC_MAX));
		LCD_Print("Value: %u   ", analogRead);
		printf("Value: %u\n", analogRead);
		LCD_SetRowBottom();

		if(analogRead > 245)
		{
			LCD_Print("%s%c", "Red Light     ", 'R');
			printf("%s%c\n\n", "Red Light     ", 'R');
			GPIO_WriteToOutputPin(GPIOB, RED_LED, SET);
			GPIO_WriteToOutputPin(GPIOB, YELLOW_LED, RESET);
			GPIO_WriteToOutputPin(GPIOB, GREEN_LED, RESET);
		}
		else if(analogRead <= 245 && analogRead > 55)
		{
			LCD_Print("%s%c", "Yellow Light  ", 'Y');
			printf("%s%c\n\n", "Yellow Light  ", 'Y');
			GPIO_WriteToOutputPin(GPIOB, RED_LED, RESET);
			GPIO_WriteToOutputPin(GPIOB, YELLOW_LED, SET);
			GPIO_WriteToOutputPin(GPIOB, GREEN_LED, RESET);
		}
		else
		{
			LCD_Print("%s%c", "Green Light   ", 'G');
			printf("%s%c\n\n", "Green Light   ", 'G');
			GPIO_WriteToOutputPin(GPIOB, RED_LED, RESET);
			GPIO_WriteToOutputPin(GPIOB, YELLOW_LED, RESET);
			GPIO_WriteToOutputPin(GPIOB, GREEN_LED, SET);
		}

		ADC_StartConversion(ADC1, ADC_REGULAR_GROUP);
	}
	else if(appEv == ADC_OVERRUN_SET)
		ADC_StartConversion(ADC1, ADC_REGULAR_GROUP);

	LCD_SetRowTop();
	TIM_Delay_ms(TIM2, 25);
}

#endif /* _ANALOG_MEASUREMENT_LCD_UART */

#ifdef _USER_PUSHBUTTON_INTERRUPT_LEDS

/*
 * This program uses the four LED lights on board and the user button
 * to demonstrate the functionality of External Interrupts. Upon stating,
 * the program lights up the on-board LEDs in clockwise order. If the
 * user button is pressed, an external interrupt occurs and changes
 * direction of the LED cycle.
 */

/*
 * GPIO_PIN_PA00 -> User Push Button
 *
 * GPIO_PIN_PD12 -> Green LED
 * GPIO_PIN_PD13 -> Orange LED
 * GPIO_PIN_PD14 -> Red LED
 * GPIO_PIN_PD15 -> Blue LED
 */

#include<stdint.h>
#include"gpio.h"
#include"irq.h"
#include"tim.h"

/************ Global Variables and Macros *********************************************************************/
#define CLOCKWISE 0U
#define COUNTER_CLOCKWISE 1U

#define GREEN_LED GPIO_PIN_NO_12
#define ORANGE_LED GPIO_PIN_NO_13
#define RED_LED GPIO_PIN_NO_14
#define BLUE_LED GPIO_PIN_NO_15

#define PUSH_BTN GPIO_PIN_NO_0

volatile uint32_t delay_ms = 500U;
volatile uint8_t ledPosition = 0b0001U;
volatile uint8_t direction = CLOCKWISE;
/**************************************************************************************************************/

/************ Function Headers ********************************************************************************/
void gpio_leds_init(void);
void gpio_userBtn_init(void);
void initTim(void);
uint8_t rotateBitsClockwise(uint8_t value);
uint8_t rotateBitsCounterClockwise(uint8_t value);
/**************************************************************************************************************/

/************ Main Function ***********************************************************************************/
int main(void)
{
	gpio_leds_init();
	gpio_userBtn_init();
	initTim();

	while(1)
	{
		if(direction == CLOCKWISE)
		{
			for(uint8_t i = 0; i < 4; i++)
				GPIO_WriteToOutputPin(GPIOD, GREEN_LED + i, ((ledPosition >> i) & 0b0001U));

			ledPosition = rotateBitsClockwise(ledPosition);
		}
		else
		{
			for(uint8_t i = 0; i < 4; i++)
				GPIO_WriteToOutputPin(GPIOD, GREEN_LED + i, ((ledPosition >> i) & 0b0001U));

			ledPosition = rotateBitsCounterClockwise(ledPosition);
		}

		TIM_Delay_ms(TIM2, delay_ms);
	}
}
/**************************************************************************************************************/

/************ Function Definitions ****************************************************************************/
void gpio_leds_init(void)
{
	GPIO_Init(GPIOD);
	// Green LED
	GPIO_SetPinMode(GPIOD, GREEN_LED, GPIO_MODE_OUT);
	GPIO_SetOutputType(GPIOD, GREEN_LED, GPIO_OP_TYPE_PP);
	GPIO_SetPinSpeed(GPIOD, GREEN_LED, GPIO_SPEED_HIGH);
	GPIO_SetPullUpPullDown(GPIOD, GREEN_LED, GPIO_NO_PUPD);
	// Orange LED
	GPIO_SetPinMode(GPIOD, ORANGE_LED, GPIO_MODE_OUT);
	GPIO_SetOutputType(GPIOD, ORANGE_LED, GPIO_OP_TYPE_PP);
	GPIO_SetPinSpeed(GPIOD, ORANGE_LED, GPIO_SPEED_HIGH);
	GPIO_SetPullUpPullDown(GPIOD, ORANGE_LED, GPIO_NO_PUPD);
	// Red LED
	GPIO_SetPinMode(GPIOD, RED_LED, GPIO_MODE_OUT);
	GPIO_SetOutputType(GPIOD, RED_LED, GPIO_OP_TYPE_PP);
	GPIO_SetPinSpeed(GPIOD, RED_LED, GPIO_SPEED_HIGH);
	GPIO_SetPullUpPullDown(GPIOD, RED_LED, GPIO_NO_PUPD);
	// Blue LED
	GPIO_SetPinMode(GPIOD, BLUE_LED, GPIO_MODE_OUT);
	GPIO_SetOutputType(GPIOD, BLUE_LED, GPIO_OP_TYPE_PP);
	GPIO_SetPinSpeed(GPIOD, BLUE_LED, GPIO_SPEED_HIGH);
	GPIO_SetPullUpPullDown(GPIOD, BLUE_LED, GPIO_NO_PUPD);
}

void gpio_userBtn_init(void)
{
	GPIO_Init(GPIOA);
	GPIO_EnableInterrupts(GPIOA, PUSH_BTN, GPIO_MODE_IT_FT);
}

void initTim(void)
{
	TIM_Init(TIM2);
	TIM_SetCounterMode(TIM2, COUNT_UP_MODE);
}

uint8_t rotateBitsClockwise(uint8_t value)
{
	return ((value << 1) | (value >> (4 - 1)));
}

uint8_t rotateBitsCounterClockwise(uint8_t value)
{
	return ((value >> 1) | (value << (4 - 1)));
}
/**************************************************************************************************************/

/************ IRQ Handling and Application Event Callbacks ****************************************************/
void EXTI0_IRQHandler(void)
{
	EXTI0_IRQHandling();
}

extern void GPIO_ApplicationEventCallBack(void)
{
	if(direction == CLOCKWISE)
		direction = COUNTER_CLOCKWISE;
	else
		direction = CLOCKWISE;
}
/**************************************************************************************************************/

#endif /* _USER_PUSHBUTTON_INTERRUPT_LEDS */

#ifdef _SPI_DATA_TX_ONLY

/*
 * This program demonstrates SPI communication with an arduino board; STM32 board as master and the
 * arduino as slave. The STM32 board is connected to four push buttons and the arduino through a
 * voltage level converter module. The arduino is connected to four LEDs that correspond to the STM's
 * push buttons. When a push button is pressed, the STM32 sends data to the arduino via SPI peripheral
 * to turn on the corresponding LED. The press of the button causes an Interrupt Service Routine to send
 * the data. The baud rate for the SPI communication is 500kHz.
 */

/*
 * SPI2 Pin Configurations:
 * 	PB12 -> NSS
 * 	PB13 -> SCK
 * 	PB14 -> MISO
 * 	PB15 -> MOSI
 *
 * Push Button Pins:
 *  PC00 -> Red Push Button
 *  PC01 -> Yellow Push Button
 *  PC02 -> Green Push Button
 *  PC03 -> Blue Push Button
 */

#include<stdint.h>
#include<string.h>
#include"spi.h"
#include"gpio.h"

/************ Global Variables and Macros *********************************************************************/
#define NSS				GPIO_PIN_NO_12
#define CLK				GPIO_PIN_NO_13
#define MISO			GPIO_PIN_NO_14
#define MOSI			GPIO_PIN_NO_15

#define RED_BTN			GPIO_PIN_NO_4
#define YELLOW_BTN		GPIO_PIN_NO_1
#define GREEN_BTN		GPIO_PIN_NO_2
#define BLUE_BTN		GPIO_PIN_NO_3

#define RED_BTN_PRESSED			10
#define YELLOW_BTN_PRESSED		15
#define GREEN_BTN_PRESSED		20
#define BLUE_BTN_PRESSED		25

uint8_t btnPressed;
/**************************************************************************************************************/

/************ Function Headers ********************************************************************************/
void setUpSpi(void);
void setUpButtons(void);
/**************************************************************************************************************/

/************ Main Function ***********************************************************************************/
int main(void)
{
	setUpButtons();
	setUpSpi();
	btnPressed = 0;
	uint8_t dataTx[1];
	while(1)
	{
		if(btnPressed == RED_BTN_PRESSED)
		{
			dataTx[0] = 'r';
			GPIO_WriteToOutputPin(GPIOB, NSS, RESET);
			SPI_MasterTransmissionStartTx(SPI2, dataTx);
			GPIO_WriteToOutputPin(GPIOB, NSS, SET);
			btnPressed = 0;
		}
		else if(btnPressed == YELLOW_BTN_PRESSED)
		{
			dataTx[0] = 'y';
			GPIO_WriteToOutputPin(GPIOB, NSS, RESET);
			SPI_MasterTransmissionStartTx(SPI2, dataTx);
			GPIO_WriteToOutputPin(GPIOB, NSS, SET);
			btnPressed = 0;
		}
		else if(btnPressed == GREEN_BTN_PRESSED)
		{
			dataTx[0] = 'g';
			GPIO_WriteToOutputPin(GPIOB, NSS, RESET);
			SPI_MasterTransmissionStartTx(SPI2, dataTx);
			GPIO_WriteToOutputPin(GPIOB, NSS, SET);
			btnPressed = 0;
		}
		else if(btnPressed == BLUE_BTN_PRESSED)
		{
			dataTx[0] = 'b';
			GPIO_WriteToOutputPin(GPIOB, NSS, RESET);
			SPI_MasterTransmissionStartTx(SPI2, dataTx);
			GPIO_WriteToOutputPin(GPIOB, NSS, SET);
			btnPressed = 0;
		}
	}
}
/**************************************************************************************************************/

/************ Function Definitions ****************************************************************************/
void setUpSpi(void)
{
	SPI_Config_t spiConfig;
	GPIO_PinConfig_t pins;

	//Configure GPIO pins for SPI2
	pins.GPIO_PinNumber = NSS;
	pins.GPIO_PinMode = GPIO_MODE_OUT;
	GPIO_Init(GPIOB, pins); //NSS
	GPIO_WriteToOutputPin(GPIOB, NSS, SET);

	pins.GPIO_PinMode = GPIO_MODE_ALTFN;
	pins.GPIO_PinAltFunMode = GPIO_AF_05;
	pins.GPIO_PinNumber = CLK;
	GPIO_Init(GPIOB, pins); //CLK

	//pins.GPIO_PinNumber = MISO;
	//GPIO_Init(GPIOB, pins); //MISO

	pins.GPIO_PinNumber = MOSI;
	pins.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	GPIO_Init(GPIOB, pins); //MOSI

	//Configure SPI Peripheral
	spiConfig.SPI_bidirectionalMode = SPI_1LINE_BIDRECTIONAL_TX_ONLY_MODE;
	spiConfig.SPI_baudRateDiv = SPI_BAUD_DIV_32; //Baud Rate = 16MHz/32 = 500kHz
	spiConfig.SPI_bitOrder = SPI_MSB_TRANSMIT_FIRST;
	spiConfig.SPI_dataBitMode = SPI_8BIT_DATA_FRAME;
	spiConfig.SPI_clkRelationship = SPI_CLOCK_POL_PHA_1;

	SPI_InitMaster(SPI2, spiConfig);
}

void setUpButtons(void)
{
	GPIO_PinConfig_t btnConfig;
	btnConfig.GPIO_PinMode = GPIO_MODE_IT_RT;
	btnConfig.GPIO_PinPuPdControl = GPIO_PIN_PD;
	btnConfig.GPIO_PinNumber = RED_BTN;
	GPIO_Init(GPIOC, btnConfig);	//Red Push Button

	btnConfig.GPIO_PinNumber = YELLOW_BTN;
	GPIO_Init(GPIOC, btnConfig);	// Yellow Push Button

	btnConfig.GPIO_PinNumber = GREEN_BTN;
	GPIO_Init(GPIOC, btnConfig);	// Green Push Button

	btnConfig.GPIO_PinNumber = BLUE_BTN;
	GPIO_Init(GPIOC, btnConfig);	// Blue Push Button

	GPIOC->PUPDR |= (0x2A8 << 0);
}
/**************************************************************************************************************/

/************ IRQ Handling and Application Event Callbacks ****************************************************/

void EXTI1_IRQHandler(void)
{
	EXTI1_IRQHandling();
}

void EXTI2_IRQHandler(void)
{
	EXTI2_IRQHandling();
}

void EXTI3_IRQHandler(void)
{
	EXTI3_IRQHandling();
}

void EXTI4_IRQHandler(void)
{
	EXTI4_IRQHandling();
}

extern void GPIO_ApplicationEventCallBack(uint8_t pinNumber)
{
	if(pinNumber == RED_BTN)
		btnPressed = RED_BTN_PRESSED;
	else if(pinNumber == YELLOW_BTN)
		btnPressed = YELLOW_BTN_PRESSED;
	else if(pinNumber == GREEN_BTN)
		btnPressed = GREEN_BTN_PRESSED;
	else if(pinNumber == BLUE_BTN)
		btnPressed = BLUE_BTN_PRESSED;
}
/**************************************************************************************************************/

#endif /* _SPI_DATA_TX_ONLY */

#ifdef _SPI_DATA_TX_RX

/* **** This Program is not finished ******
 * This program demonstrates SPI communication with an arduino board; STM32 board as master and the
 * arduino as slave. The STM32 board is connected to four push buttons and the arduino through a
 * voltage level converter module. The arduino is connected to four LEDs that correspond to the STM's
 * push buttons. When a push button is pressed, the STM32 sends data to the arduino via SPI peripheral
 * to turn on the corresponding LED. The press of the button causes an Interrupt Service Routine to send
 * the data. The baud rate for the SPI communication is 250kHz, any faster and the Arduino cannot keep up.
 */

/*
 * SPI2 Pin Configurations:
 * 	PB12 -> NSS
 * 	PB13 -> SCK
 * 	PB14 -> MISO
 * 	PB15 -> MOSI
 *
 * Push Button Pins:
 *  PC04 -> Push Button
 */

#include<stdint.h>
#include<string.h>
#include"spi.h"
#include"gpio.h"

/************ Global Variables and Macros *********************************************************************/
#define NSS				GPIO_PIN_NO_12
#define CLK				GPIO_PIN_NO_13
#define MISO			GPIO_PIN_NO_14
#define MOSI			GPIO_PIN_NO_15

#define PUSH_BTN		GPIO_PIN_NO_4

#define RX_LEN	4

/*uint8_t dataTx[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E,
                        0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
                        0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C,
                        0x1D, 0x1E, 0x1F, 0x20};*/


volatile uint8_t btnPressed;
/**************************************************************************************************************/

/************ Function Headers ********************************************************************************/
void setUpSpi(void);
void setUpButtons(void);
/**************************************************************************************************************/

/************ Main Function ***********************************************************************************/
int main(void)
{
	setUpButtons();
	setUpSpi();
	btnPressed = 0;
	while(1)
	{
		if(btnPressed)
		{
			uint8_t dataTx[] = {0x00, 0x10, 0x2E, 0xF1};
			uint8_t dataRx[RX_LEN];
			GPIO_WriteToOutputPin(GPIOB, NSS, RESET);
			SPI_MasterTransmissionStartTxRx(SPI2, dataTx, dataRx, RX_LEN);
			GPIO_WriteToOutputPin(GPIOB, NSS, SET);
			btnPressed = 0;
		}
	}
}
/**************************************************************************************************************/

/************ Function Definitions ****************************************************************************/
void setUpSpi(void)
{
	SPI_Config_t spiConfig;
	GPIO_PinConfig_t pins;

	//Configure GPIO pins for SPI2
	pins.GPIO_PinNumber = NSS;
	pins.GPIO_PinMode = GPIO_MODE_OUT;
	GPIO_Init(GPIOB, pins); //NSS
	GPIO_WriteToOutputPin(GPIOB, NSS, SET);

	pins.GPIO_PinMode = GPIO_MODE_ALTFN;
	pins.GPIO_PinAltFunMode = GPIO_AF_05;
	pins.GPIO_PinNumber = CLK;
	GPIO_Init(GPIOB, pins); //CLK

	pins.GPIO_PinNumber = MISO;
	GPIO_Init(GPIOB, pins); //MISO

	pins.GPIO_PinNumber = MOSI;
	pins.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	GPIO_Init(GPIOB, pins); //MOSI

	//Configure APB1 Prescaler
	RCC->CFGR |= (0b100 << RCC_CFGR_PPRE1_Pos);

	//Configure SPI Peripheral
	spiConfig.SPI_bidirectionalMode = SPI_2LINE_UNIDIRECTIONAL_RX_TX_MODE;
	spiConfig.SPI_baudRateDiv = SPI_BAUD_DIV_256; //Baud Rate = 8MHz/256 = 31,250 Hz
	spiConfig.SPI_bitOrder = SPI_MSB_TRANSMIT_FIRST;
	spiConfig.SPI_dataBitMode = SPI_8BIT_DATA_FRAME;
	spiConfig.SPI_clkRelationship = SPI_CLOCK_POL_PHA_1;
	spiConfig.SPI_SlaveMgmt = SPI_SWS_MGNT;

	//Initialize the SPI2 peripheral.
	SPI_InitMaster(SPI2, spiConfig);
}

void setUpButtons(void)
{
	GPIO_PinConfig_t btnConfig;
	btnConfig.GPIO_PinMode = GPIO_MODE_IT_RT;
	btnConfig.GPIO_PinPuPdControl = GPIO_PIN_PD;
	btnConfig.GPIO_PinNumber = PUSH_BTN;
	GPIO_Init(GPIOC, btnConfig);

	GPIOC->PUPDR |= (0b1000000000 << 0);
}
/**************************************************************************************************************/

/************ IRQ Handling and Application Event Callbacks ****************************************************/

void EXTI4_IRQHandler(void)
{
	EXTI4_IRQHandling();
}

extern void GPIO_ApplicationEventCallBack(uint8_t pinNumber)
{
	if(pinNumber == PUSH_BTN)
		btnPressed = 1;
}
/**************************************************************************************************************/

#endif /* _SPI_DATA_TX_RX */

#ifdef _TONE_GENERATION

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
	//TIM_Delay_ms(TIM3, 20);
	if(toneFlag == 1)
	{
		int diff = CurrentAnalogReadY - PreviousAnalogReadY;
		if(diff >= 2)
		{
			toneFrequency += (0.3268 * diff);
			printf("Frequency Output:       %f\n\n", toneFrequency);
			TIM_SetPWM_Frequency(TIM3, toneFrequency);
			TIM_SetPWM_DutyCycle(TIM3, toneFrequency, NORMAL_DUTY_CYCLE);
			PreviousAnalogReadY = CurrentAnalogReadY;
		}
		else if(diff <= -2)
		{
			toneFrequency += (0.3268 * diff);
			printf("Frequency Output:       %f\n\n", toneFrequency);
			TIM_SetPWM_Frequency(TIM3, toneFrequency);
			TIM_SetPWM_DutyCycle(TIM3, toneFrequency, NORMAL_DUTY_CYCLE);
			PreviousAnalogReadY = CurrentAnalogReadY;
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
				BIT_SET(butnsPressed, i);
			}
			else
			{
				TIM_StopTimer(TIM3);
				toneFrequency = 0;
				toneFlag = 0;
				butnsPressed = 0;
			}
		}
	}
}

/**************************************************************************************************************/

#endif /* _TONE_GENERATION */


#ifdef _TEST

#endif /* _TEST */
