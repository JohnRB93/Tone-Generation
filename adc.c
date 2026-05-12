#include"adc.h"

/***************** Private Helper Function Headers *************************************/

static void ADC_HandleAWDIt(ADC_TypeDef *ADC_T);
static void ADC_HandleEOCIt(ADC_TypeDef *ADC_T);
static void ADC_HandleJEOCIt(ADC_TypeDef *ADC_T);
static void ADC_HandleOVRIt(ADC_TypeDef *ADC_T);

/***************************************************************************************/


/***************** User Application Exposed Function Definitions ***********************/

/*
 * @fn			- ADC_init
 *
 * @brief		- This function initializes the ADC peripheral.
 *
 * @param[in]	- ADC address structure.
 *
 * @return		- None.
 *
 * @note		- You will need to call the ADC_RegChannelSel() and/or ADC_InjChannelSel()
 * 				  function(s) to configure the conversion channels after calling this function.
 * 				  If neither continuous nor scan mode are activated, then single mode is used by
 * 				  default.
 */
void ADC_Init(ADC_TypeDef *ADC_T)
{
	ADC_PeriClockControl(ADC_T, ENABLE);
}

/*
 * @fn			- ADC_DeInit
 *
 * @brief		- This function de-initializes all ADC peripherals.
 *
 * @param[in]	- None.
 *
 * @return		- None.
 *
 * @note		- None.
 */
void ADC_DeInit(void)
{
	RCC->APB2ENR |= RCC_APB2RSTR_ADCRST;
	RCC->APB2ENR &= ~RCC_APB2RSTR_ADCRST;
}

/*
 * @fn			- ADC_PeriClockControl
 *
 * @brief		- This function enables or disables peripheral clock for the
 * 				  given ADC register.
 *
 * @param[in]	- Base address of the ADC register.
 * @param[in]	- ENABLE or DISABLE macros.
 *
 * @return		- None.
 *
 * @note		- None.
 */
void ADC_PeriClockControl(ADC_TypeDef *ADC_T, uint8_t EnOrDi)
{
	if(EnOrDi == ENABLE)
	{
		if(ADC_T == ADC1)
			RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
		else if(ADC_T == ADC2)
			RCC->APB2ENR |= RCC_APB2ENR_ADC2EN;
		else if(ADC_T == ADC3)
			RCC->APB2ENR |= RCC_APB2ENR_ADC3EN;
	}
	else
	{
		if(ADC_T == ADC1)
			RCC->APB2ENR &= ~RCC_APB2ENR_ADC1EN;
		else if(ADC_T == ADC2)
			RCC->APB2ENR &= ~RCC_APB2ENR_ADC2EN;
		else if(ADC_T == ADC3)
			RCC->APB2ENR &= ~RCC_APB2ENR_ADC3EN;
	}
}

/*
 * @fn			- ADC_EnableConverter
 *
 * @brief		- This function enables the ADC.
 *
 * @param[in]	- Base address of the ADC register.
 *
 * @return		- None.
 *
 * @note		- None.
 */
void ADC_EnableConverter(ADC_TypeDef *ADC_T)
{
	ADC_T->CR2 |= ADC_CR2_ADON;
}

/*
 * @fn			- ADC_SetBitResolution
 *
 * @brief		- This function enables or disables peripheral clock for the
 * 				  given ADC register.
 *
 * @param[in]	- Base address of the ADC register.
 * @param[in]	- Bit resolution for the ADC
 * 				  (ADC_12BIT_RESOLUTION, ADC_10BIT_RESOLUTION, ADC_08BIT_RESOLUTION, ADC_06BIT_RESOLUTION).
 *
 * @return		- None.
 *
 * @note		- None.
 */
void ADC_SetBitResolution(ADC_TypeDef *ADC_T, uint8_t resolution)
{
	switch(resolution)
	{
		case ADC_12BIT_RESOLUTION: ADC_T->CR1 &= ~ADC_CR1_RES; break;
		case ADC_10BIT_RESOLUTION:
		case ADC_08BIT_RESOLUTION:
		case ADC_06BIT_RESOLUTION: ADC_T->CR1 &= ~ADC_CR1_RES; ADC_T->CR1 |= (resolution << ADC_CR1_RES_Pos); break;
		default: break;
	}
}

/*
 * @fn			- ADC_RegChannelSel
 *
 * @brief		- This function configures the selection of the regular
 * 				  group of channels, how many channels in the group, and
 * 				  the order of the channels.
 *
 * @param[in]	- Base address of the ADC register.
 *
 * @param[in]	- Number of conversions for the selected group.
 * 				  (ADC_01_CONVERSIONS, ADC_02_CONVERSIONS, ... ADC_16_CONVERSIONS).
 * @param[in]	- Array containing which channels to be used, ordered from index 0
 * 				  to last index. ({ADC_IN0, ADC_IN1, ... ADC_IN15})
 *
 * @return		- None.
 *
 * @note		- Regular channels can have up to 16 conversions.
 * 				  ** Be sure to place the channel numbers in the array parameter in the
 * 				  order that you want to have them converted (index[0] will be first). **
 */
void ADC_RegChannelSel(ADC_TypeDef *ADC_T, uint8_t conversions, uint8_t channels[])
{
	if(conversions == ADC_01_CONVERSIONS)
	{//Only one regular conversion will be done.(Most likely for single conversion mode.)
		ADC_T->SQR1 &= ~ADC_SQR1_L;
		ADC_T->SQR3 &= 0xC0000000;
		ADC_T->SQR3 |= (channels[0] << 0);
	}
	else if(conversions <= ADC_16_CONVERSIONS && conversions >= ADC_02_CONVERSIONS)
	{//More than one regular conversion will be done.
		ADC_T->SQR1 |= ((conversions) << 20U);
		for(uint8_t i = 0; i < conversions+1; i++)
		{
			if(i < 6)
				ADC_T->SQR3 |= (channels[i] << (i * 5));
			else if(i > 5 && i < 12)
				ADC_T->SQR2 |= (channels[i] << (i * 5));
			else
				ADC_T->SQR1 |= (channels[i] << (i * 5));
		}
	}
}


/*
 * @fn			- ADC_InjChannelSel
 *
 * @brief		- This function configures the selection of injected group of
 * 				  channels, how many channels in the group, and the order of
 * 				  the channels.
 *
 * @param[in]		- Base address of the ADC register.
 *
 * @param[in]		- Number of conversions for the selected group.
 * 					  (ADC_01_CONVERSIONS, ADC_02_CONVERSIONS, ... ADC_16_CONVERSIONS).
 * @param[in]		- Array containing which channels to be used, ordered from index 0
 * 					  to last index. ({ADC_IN0, ADC_IN1, ... ADC_IN15})
 *
 *
 * @return		- None.
 *
 * @note		- Injected channels can have up to 4 conversions.
 * 				  ** Be sure to place the channel numbers in the array parameter in the
 * 				  order that you want to have them converted (index[0] will be first). **
 */
void ADC_InjChannelSel(ADC_TypeDef *ADC_T, uint8_t conversions, uint8_t channels[])
{
	if(conversions <= ADC_04_CONVERSIONS && conversions > ADC_01_CONVERSIONS)
	{
		ADC_T->JSQR |= (conversions << 20U);
		if(conversions == ADC_01_CONVERSIONS)
			ADC_T->JSQR |= (channels[0] << 15);
		else if(conversions == ADC_02_CONVERSIONS)
		{
			ADC_T->JSQR |= (channels[0] << 10);
			ADC_T->JSQR |= (channels[1] << 15);
		}
		else if(conversions == ADC_03_CONVERSIONS)
		{
			ADC_T->JSQR |= (channels[0] << 5);
			ADC_T->JSQR |= (channels[1] << 10);
			ADC_T->JSQR |= (channels[2] << 15);
		}
		else if(conversions == ADC_04_CONVERSIONS)
		{
			ADC_T->JSQR |= (channels[0] << 0);
			ADC_T->JSQR |= (channels[1] << 5);
			ADC_T->JSQR |= (channels[2] << 10);
			ADC_T->JSQR |= (channels[3] << 15);
		}
	}
}

/*
 * @fn			- ADC_SelectEOCFlagTrigger
 *
 * @brief		- This function configures the End of Conversion
 * 				  Selection bit in the CR2 register.
 *
 * @param[in]	- Base address of the ADC Register.
 * @param[in]	- End of conversion flag trigger setting.
 * 				  (End of sequence[ADC_END_OF_SEQ] or End of each[ADC_END_OF_EACH])
 *
 * @return		- None.
 *
 * @note		- None.
 */
void ADC_SelectEOCFlagTrigger(ADC_TypeDef *ADC_T, uint8_t eocSelection)
{
	if(eocSelection == ADC_END_OF_EACH)
		ADC_T->CR2 |= ADC_CR2_EOCS;			/*End of each conversion.*/
	else
		ADC_T->CR2 &= ~ADC_CR2_EOCS;		/*End of each sequence of conversions.*/
}

/*
 * @fn			- ADC_SetDataAlignment
 *
 * @brief		- This function sets the data alignment of data stored in the
 * 				  data register after each conversion to either right-aligned or
 * 				  left-aligned.
 *
 * @param[in]	- Base address of the ADC Register.
 * @param[in]	- Alignment setting.
 * 				  (Right-Aligned[ADC_DATA_ALIGNMENT_RIGHT] or Left-Aligned[ADC_DATA_ALIGNMENT_LEFT])
 *
 * @return		- None.
 *
 * @note		- None.
 */
void ADC_SetDataAlignment(ADC_TypeDef *ADC_T, uint8_t dataAlignment)
{
	if(dataAlignment == ADC_DATA_ALIGNMENT_RIGHT)
		ADC_T->CR2 &= ~ADC_CR2_ALIGN;
	else
		ADC_T->CR2 |= ADC_CR2_ALIGN;
}

/*
 * @fn			- ADC_SetPrescaler
 *
 * @brief		- This function sets the division factor by which the peripheral
 * 				  clock of the ADC is divided by.
 *
 * @param[in]	- Base address of the ADC Register.
 * @param[in]	- Peripheral clock division.
 * 				  (Divided by 2[ADC_PCLK_DIV2],  Divided by 4[ADC_PCLK_DIV4],
 * 				  Divided by 6[ADC_PCLK_DIV6], or Divided by 8[ADC_PCLK_DIV8])
 *
 * @return		- None.
 *
 * @note		- None.
 */
void ADC_SetPrescaler(ADC_TypeDef *ADC_T, uint8_t prescaler)
{
	switch(prescaler)
	{
		case ADC_PCLK_DIV2: ADC->CCR &= ADC_CCR_ADCPRE; break;
		case ADC_PCLK_DIV4:
		case ADC_PCLK_DIV6:
		case ADC_PCLK_DIV8: ADC->CCR &= ADC_CCR_ADCPRE; ADC->CCR |= (prescaler << ADC_CCR_ADCPRE_Pos); break;
		default: break;
	}
}

/*
 * @fn			- ADC_ConfigSampRate
 *
 * @brief		- This function configures the sample rate for the given
 * 				  analog channel.
 *
 * @param[in]	- Base address of the ADC Register.
 * @param[in]	- ADC channel (ADC_IN0, ... ADC_IN18).
 * @param[in]	- Sampling time (ADC_003_CYCLES, ... ADC_480_CYCLES).
 *
 * @return		- None.
 *
 * @note		- None.
 */
void ADC_ConfigSampRate(ADC_TypeDef *ADC_T, uint8_t channel, uint8_t cycles)
{
	if(cycles == ADC_003_CYCLES)
	{
		switch(channel)
		{
			case ADC_IN0 : ADC_T->SMPR2 &= ~ADC_SMPR2_SMP0;  break;
			case ADC_IN1 : ADC_T->SMPR2 &= ~ADC_SMPR2_SMP1;  break;
			case ADC_IN2 : ADC_T->SMPR2 &= ~ADC_SMPR2_SMP2;  break;
			case ADC_IN3 : ADC_T->SMPR2 &= ~ADC_SMPR2_SMP3;  break;
			case ADC_IN4 : ADC_T->SMPR2 &= ~ADC_SMPR2_SMP4;  break;
			case ADC_IN5 : ADC_T->SMPR2 &= ~ADC_SMPR2_SMP5;  break;
			case ADC_IN6 : ADC_T->SMPR2 &= ~ADC_SMPR2_SMP6;  break;
			case ADC_IN7 : ADC_T->SMPR2 &= ~ADC_SMPR2_SMP7;  break;
			case ADC_IN8 : ADC_T->SMPR2 &= ~ADC_SMPR2_SMP8;  break;
			case ADC_IN9 : ADC_T->SMPR2 &= ~ADC_SMPR2_SMP9;  break;
			case ADC_IN10: ADC_T->SMPR1 &= ~ADC_SMPR1_SMP10; break;
			case ADC_IN11: ADC_T->SMPR1 &= ~ADC_SMPR1_SMP11; break;
			case ADC_IN12: ADC_T->SMPR1 &= ~ADC_SMPR1_SMP12; break;
			case ADC_IN13: ADC_T->SMPR1 &= ~ADC_SMPR1_SMP13; break;
			case ADC_IN14: ADC_T->SMPR1 &= ~ADC_SMPR1_SMP14; break;
			case ADC_IN15: ADC_T->SMPR1 &= ~ADC_SMPR1_SMP15; break;
			case ADC_IN16: ADC_T->SMPR1 &= ~ADC_SMPR1_SMP16; break;
			case ADC_IN17: ADC_T->SMPR1 &= ~ADC_SMPR1_SMP17; break;
			case ADC_IN18: ADC_T->SMPR1 &= ~ADC_SMPR1_SMP18; break;
		}
	}
	else
	{
		switch(channel)
		{
			case ADC_IN0 : ADC_T->SMPR2 |= (cycles << 0);  break;
			case ADC_IN1 : ADC_T->SMPR2 |= (cycles << 3);  break;
			case ADC_IN2 : ADC_T->SMPR2 |= (cycles << 6);  break;
			case ADC_IN3 : ADC_T->SMPR2 |= (cycles << 9);  break;
			case ADC_IN4 : ADC_T->SMPR2 |= (cycles << 12); break;
			case ADC_IN5 : ADC_T->SMPR2 |= (cycles << 15); break;
			case ADC_IN6 : ADC_T->SMPR2 |= (cycles << 18); break;
			case ADC_IN7 : ADC_T->SMPR2 |= (cycles << 21); break;
			case ADC_IN8 : ADC_T->SMPR2 |= (cycles << 24); break;
			case ADC_IN9 : ADC_T->SMPR2 |= (cycles << 27); break;
			case ADC_IN10: ADC_T->SMPR1 |= (cycles << 0);  break;
			case ADC_IN11: ADC_T->SMPR1 |= (cycles << 3);  break;
			case ADC_IN12: ADC_T->SMPR1 |= (cycles << 6);  break;
			case ADC_IN13: ADC_T->SMPR1 |= (cycles << 9);  break;
			case ADC_IN14: ADC_T->SMPR1 |= (cycles << 12); break;
			case ADC_IN15: ADC_T->SMPR1 |= (cycles << 15); break;
			case ADC_IN16: ADC_T->SMPR1 |= (cycles << 18); break;
			case ADC_IN17: ADC_T->SMPR1 |= (cycles << 21); break;
			case ADC_IN18: ADC_T->SMPR1 |= (cycles << 24); break;
		}
	}
}

/*
 * @fn			- ADC_EnableContinuousMode
 *
 * @brief		- This function
 *
 * @param[in]	- Base address of the ADC register.
 *
 * @return		- None.
 *
 * @note		- None.
 */
void ADC_EnableContinuousMode(ADC_TypeDef *ADC_T)
{
	ADC_T->CR2 |= ADC_CR2_CONT;
}

/*
 * @fn			- ADC_DisableContinuousMode
 *
 * @brief		- This function clears the CONT bit in the CR2
 * 				  register, disabling continuous conversion mode.
 *
 * @param[in]	- Base address of the ADC register.
 *
 * @return		- None.
 *
 * @note		- None.
 */
void ADC_DisableContinuousMode(ADC_TypeDef *ADC_T)
{
	ADC_T->CR2 &= ~ADC_CR2_CONT;
}

/*
 * @fn			- ADC_EnableScanMode
 *
 * @brief		- This function enables scan mode.
 *
 * @param[in]	- Base address of the ADC register.
 *
 * @return		- None.
 *
 * @note		- None.
 */
void ADC_EnableScanMode(ADC_TypeDef *ADC_T)
{
	ADC_T->CR1 |= ADC_CR1_SCAN;
}

/*
 * @fn			- ADC_DisableScanMode
 *
 * @brief		- This function disables scan mode.
 *
 * @param[in]	- Base address of the ADC register.
 *
 * @return		- None.
 *
 * @note		- None.
 */
void ADC_DisableScanMode(ADC_TypeDef *ADC_T)
{
	ADC_T->CR1 &= ~ADC_CR1_SCAN;
}

/*
 * @fn			- ADC_EnableDisContMode
 *
 * @brief		- This function enables discontinuous mode.
 *
 * @param[in]	- Base address of the ADC register.
 *
 * @return		- None.
 *
 * @note		- None.
 */
void ADC_EnableDisContMode(ADC_TypeDef *ADC_T)
{
	ADC_T->CR1 |= ADC_CR1_DISCEN;
}

/*
 * @fn			- ADC_DisableDisContMode
 *
 * @brief		- This function disables discontinuous mode.
 *
 * @param[in]	- Base address of the ADC register.
 *
 * @return		- None.
 *
 * @note		- None.
 */
void ADC_DisableDisContMode(ADC_TypeDef *ADC_T)
{
	ADC_T->CR1 &= ~ADC_CR1_DISCEN;
}

/*
 * @fn			- ADC_SetDisContNumber
 *
 * @brief		- This function configures the number of conversions
 * 				  that will take place for DisContinuous Conversion
 * 				  Mode.
 *
 * @param[in]	- Base address of the ADC Register.
 * @param[in]	- Number of dicontinuous conversions
 * 							  (ADC_DISC_NUM_1, ... ADC_DISC_NUM_8).
 *
 * @return		- None.
 *
 * @note		- None.
 */
void ADC_SetDisContNumber(ADC_TypeDef *ADC_T, uint8_t numDiscConversions)
{
	if(numDiscConversions == ADC_DISC_NUM_1)
		ADC_T->CR1 &= ~ADC_CR1_DISCNUM;
	else if(numDiscConversions >= ADC_DISC_NUM_1 && numDiscConversions <= ADC_DISC_NUM_8)
		ADC_T->CR1 |= (numDiscConversions << 13);
}

/*
 * @fn			- ADC_StartConversion
 *
 * @brief		- This function starts ADC conversion according to the
 * 				  configurations defined in the ADC_Config Structure.
 * 				  Supported conversion modes are single, continuous,
 * 				  and discontinuous modes.
 *
 * @param[in]	- Base address of the ADC register.
 * @param[in]	- Regular group or Injected group ex.
 * 				  (ADC_REGULAR_GROUP, ADC_INJECTED_GROUP).
 * @param[in]	- Conversion Mode. ex.(ADC_SINL_CONV_MODE,
 * 				  ADC_CONT_CONV_MODE, ADC_DISCONT_CONV_MODE).
 *
 * @return		- None.
 *
 * @note		- Injected channels cannot be converted continuously. The
 * 				  only exception is when an injected channel is configured
 * 				  to be converted automatically after regular channels in
 * 				  continuous mode (using JAUTO bit).
 */
void ADC_StartConversion(ADC_TypeDef *ADC_T, uint8_t group)
{
	if(group == ADC_REGULAR_GROUP) //Regular Group will be converted.
		ADC_T->CR2 |= ADC_CR2_SWSTART;
	else
	{	//Injected Group will be converted.
		ADC_T->CR2 |= ADC_CR2_JSWSTART;
		ADC_T->SR &= ~ADC_SR_JSTRT;
	}
}

/*
 * @fn			- ADC_StopConversion
 *
 * @brief		- This function stops the ADC conversion.
 *
 * @param[in]	- Base address of the ADC register.
 *
 * @return		- None.
 *
 * @note		- None.
 */
void ADC_StopConversion(ADC_TypeDef *ADC_T)
{
	ADC_T->CR2 &= ~ADC_CR2_ADON;
}

/*
 * @fn			- ADC_ReadRegDR
 *
 * @brief		- This function reads the data register.
 *
 * @param[in]	- Base address of the ADC register.
 *
 * @return		- uint16_t Data read from the data register.
 *
 * @note		- Once the DR is read, if the EOC flag was set, it will
 * 				  be automatically cleared by hardware.
 */
uint16_t ADC_ReadRegDR(ADC_TypeDef *ADC_T)
{
	return (uint16_t)ADC_T->DR;
}

/*
 * @fn			- ADC_ReadInjDR
 *
 * @brief		- This function reads the injected data register 1 of
 * 				  injected channel.
 *
 * @param[in]	- Base address of the ADC register.
 *
 * @return		- uint16_t Data read from the injected data register.
 *
 * @note		- None.
 */
uint16_t ADC_ReadInjDR(ADC_TypeDef *ADC_T)
{
	return (uint16_t)ADC_T->JDR1;
}

/*
 * @fn			- ADC_ExtTrigDetect
 *
 * @brief		- This function configures the trigger detection for
 * 				  falling edge, rising edge, both, or none.
 *
 * @param[in]	- Base address of the ADC register.
 * @param[in]	- Conversion Group.
 * 				  (ADC_REGULAR_GROUP, ADC_INJECTED_GROUP)
 * @param[in]	- Trigger detection.
 * 				  (ADC_DETECTION_DISABLED, ADC_RISING_EDGE,
 * 			      ADC_FALLING_EDGE, ADC_RIS_FALL_EDGE)
 *
 * @return		- None.
 *
 * @note		- None.
 */
void ADC_ExtTrigDetect(ADC_TypeDef *ADC_T, uint8_t group, uint8_t detection)
{
	if(group == ADC_REGULAR_GROUP)
	{//External trigger enable for regular channels.
		if(detection == ADC_DETECTION_DISABLED)
			ADC_T->CR2 &= ~ADC_CR2_EXTEN;
		else
			ADC_T->CR2 |= (detection << 28);
	}else
	{//External trigger enable for injected channels.
		if(detection == ADC_DETECTION_DISABLED)
			ADC_T->CR2 &= ~ADC_CR2_JEXTEN;
		else
			ADC_T->CR2 |= (detection << 20);
	}
}

/*
 * @fn			- ADC_SelectExtEvReg
 *
 * @brief		- This function configures the selection of which
 * 				  external events will be used to generate external
 * 				  interrupts.
 *
 * @param[in]	- Base address of the ADC register.
 * @param[in]	- Conversion Group.
 * 				  (ADC_REGULAR_GROUP, ADC_INJECTED_GROUP)
 * @param[in]	- External event.
 * 				  (ADC_TIM1_CC1_EVENT, ... ADC_EXTI_LINE_15)
 *
 * @return		- None.
 *
 * @note		- None.
 */
void ADC_SelectExtEvReg(ADC_TypeDef *ADC_T, uint8_t group, uint8_t event)
{
	if(group == ADC_REGULAR_GROUP)
	{//External event selection for regular channels.
		if(event == ADC_TIM1_CC1_EVENT)
			ADC_T->CR2 &= ~ADC_CR2_EXTSEL;
		else
			ADC_T->CR2 |= (event << 24);
	}else
	{//External event selection for injected channels.
		if(event == ADC_TIM1_CC4_EVENT)
			ADC_T->CR2 &= ~ADC_CR2_JEXTSEL;
		else
			ADC_T->CR2 |= (event << 16);
	}
}

/*
 * @fn			- ADC_EnableWatchdog
 *
 * @brief		- This function enables the analog watchdog for the channels
 * 				  passed in the second argument. The AWD analog watchdog status
 * 				  bit is set if the analog voltage converted by the ADC is below
 * 				  a lower threshold or above a higher threshold. These thresholds
 * 				  are programmed in the 12 least significant bits of the ADC_HTR
 * 				  and ADC_LTR 16-bit registers. An interrupt can be enabled by
 * 				  using the AWDIE bit in the ADC_CR1 register.
 *
 * @param[in]	- Base address of the ADC register.
 * @param[in]	- Analog watchdog selection.
 *
 * @return		- None.
 *
 * @note		- All Injected Channels ---------------> ADC_WD_ALL_INJ_CH
 * 				  All Regular Channels ----------------> ADC_WD_ALL_REG_CH
 * 				  All Regular and Injected Channels ---> ADC_WD_ALL_REG_AND_INJ_CH
 * 				  Single Injected Channel -------------> ADC_WD_SINGLE_INJ_CH
 * 				  Single Regular Channel --------------> ADC_WD_SINGLE_REG_CH
 * 				  Single Regular or Injected Channel --> ADC_WD_SINGLE_REG_OR_INJ_CH
 */
void ADC_EnableWatchdog(ADC_TypeDef *ADC_T, uint8_t channelSelection)
{
	if(channelSelection == ADC_WD_ALL_INJ_CH)
	{// All Injected Channels
		ADC_T->CR1 &= ~ADC_CR1_AWDSGL;
		ADC_T->CR1 &= ~ADC_CR1_AWDEN;
		ADC_T->CR1 |= ADC_CR1_JAWDEN;
	}
	else if(channelSelection == ADC_WD_ALL_REG_CH)
	{// All Regular Channels
		ADC_T->CR1 &= ~ADC_CR1_AWDSGL;
		ADC_T->CR1 |= ADC_CR1_AWDEN;
		ADC_T->CR1 &= ~ADC_CR1_JAWDEN;
	}
	else if(channelSelection == ADC_WD_ALL_REG_AND_INJ_CH)
	{// All Regular and Injected Channels
		ADC_T->CR1 &= ~ADC_CR1_AWDSGL;
		ADC_T->CR1 |= ADC_CR1_AWDEN;
		ADC_T->CR1 |= ADC_CR1_JAWDEN;
	}
	else if(channelSelection == ADC_WD_SINGLE_INJ_CH)
	{// Single Injected Channel
		ADC_T->CR1 |= ADC_CR1_AWDSGL;
		ADC_T->CR1 &= ~ADC_CR1_AWDEN;
		ADC_T->CR1 |= ADC_CR1_JAWDEN;
	}
	else if(channelSelection == ADC_WD_SINGLE_REG_CH)
	{// Single Regular Channel
		ADC_T->CR1 |= ADC_CR1_AWDSGL;
		ADC_T->CR1 |= ADC_CR1_AWDEN;
		ADC_T->CR1 &= ~ADC_CR1_JAWDEN;
	}
	else if(channelSelection == ADC_WD_SINGLE_REG_OR_INJ_CH)
	{// Single Regular or Injected Channel
		ADC_T->CR1 |= ADC_CR1_AWDSGL;
		ADC_T->CR1 |= ADC_CR1_AWDEN;
		ADC_T->CR1 |= ADC_CR1_JAWDEN;
	}
}

/*
 * @fn			- ADC_DisableWatchdog
 *
 * @brief		- This function disables the analog watchdog feature.
 *
 * @param[in]	- Base address of the ADC register.
 *
 * @return		- None.
 *
 * @note		- None.
 */
void ADC_DisableWatchdog(ADC_TypeDef *ADC_T)
{
	ADC_T->CR1 &= ~ADC_CR1_AWDSGL;
	ADC_T->CR1 &= ~ADC_CR1_AWDEN;
	ADC_T->CR1 &= ~ADC_CR1_JAWDEN;
}

/*
 * @fn			- ADC_SelectWatchdogChannel
 *
 * @brief		- This function configures which regular channel
 * 				  will be guarded by the watch dog feature.
 *
 * @param[in]	- Base address of the ADC register.
 * @param[in]	- The channel to be guarded.(ADC_IN0 ... ADC_IN15)
 *
 * @return		- None.
 *
 * @note		- None.
 */
void ADC_SelectWatchdogChannel(ADC_TypeDef *ADC_T, uint8_t channel)
{
	ADC_T->CR1 &= ~ADC_CR1_AWDCH;
	ADC_T->CR1 |= (channel << ADC_CR1_AWDCH_Pos);
}

/*
 * @fn			- ADC_SetWatchdogThresholds
 *
 * @brief		- This function sets the high and low threshold values for the analog
 * 				  watchdog. If the watchdog detects values above the high threshold or
 * 				  values below the low threshold, an interrupt can be generated.
 *
 * @param[in]	- Base address of the ADC register.
 * @param[in]	- High threshold for the analog watchdog.
 * @param[in]	- Low threshold for the analog watchdog.
 *
 * @return		- None.
 *
 * @note		- None.
 */
void ADC_SetWatchdogThresholds(ADC_TypeDef *ADC_T, uint16_t highThreshold, uint16_t lowThreshold)
{
	uint16_t val = (0xFFF & lowThreshold);
	ADC_T->LTR &= ~(0xFFF << 0);
	ADC_T->LTR |= (val << 0);

	val = 0;
	val = (0xFFF & highThreshold);
	ADC_T->HTR &= ~(0xFFF << 0);
	ADC_T->HTR |= (val << 0);
}

/*
 * @fn			- ADC_EnableInterrupts
 *
 * @brief		- This function enables interrupts based on the second
 * 				  argument passed.
 *
 * @param[in]	- Base address of the ADC register.
 * @param[in]	- Type of interrupt to enable.
 *
 * @return		- None.
 *
 * @note		- Interrupt Events:
 * 				  End of conversion of a regular group ----> ADC_EOC_INTERRUPTS
 * 				  End of conversion of an injected group --> ADC_JEOC_INTERRUPTS
 * 				  Analog watchdog status bit is set -------> ADC_AWD_INTERRUPTS
 * 				  Overrun ---------------------------------> ADC_OVR_INTERRUPTS
 */
void ADC_EnableInterrupts(ADC_TypeDef *ADC_T, uint8_t interruptSelection)
{
	switch(interruptSelection)
	{
		case ADC_EOC_INTERRUPTS:  ADC_T->CR1 |= ADC_CR1_EOCIE;  break;
		case ADC_JEOC_INTERRUPTS: ADC_T->CR1 |= ADC_CR1_JEOCIE; break;
		case ADC_AWD_INTERRUPTS:  ADC_T->CR1 |= ADC_CR1_AWDIE;  break;
		case ADC_OVR_INTERRUPTS:  ADC_T->CR1 |= ADC_CR1_OVRIE;  break;
		default: break;
	}
}

/*
 * @fn			- ADC_DisableInterrupts
 *
 * @brief		- This function disables interrupts based on the second
 * 				  argument passed.
 *
 * @param[in]	- Base address of the ADC register.
 *
 * @return		- None.
 *
 * @note		- Interrupt Events:
 * 				  End of conversion of a regular group ----> ADC_EOC_INTERRUPTS
 * 				  End of conversion of an injected group --> ADC_JEOC_INTERRUPTS
 * 				  Analog watchdog status bit is set -------> ADC_AWD_INTERRUPTS
 * 				  Overrun ---------------------------------> ADC_OVR_INTERRUPTS
 */
void ADC_DisableInterrupts(ADC_TypeDef *ADC_T, uint8_t interruptSelection)
{
	switch(interruptSelection)
	{
		case ADC_EOC_INTERRUPTS:  ADC_T->CR1 &= ~ADC_CR1_EOCIE;  break;
		case ADC_JEOC_INTERRUPTS: ADC_T->CR1 &= ~ADC_CR1_JEOCIE; break;
		case ADC_AWD_INTERRUPTS:  ADC_T->CR1 &= ~ADC_CR1_AWDIE;  break;
		case ADC_OVR_INTERRUPTS:  ADC_T->CR1 &= ~ADC_CR1_OVRIE;  break;
		default: break;
	}
}

/*
 * @fn			- ADC_EnableDMA
 *
 * @brief		- This function enables the DMA to allow for transfer of data
 * 				  from peripheral to memory when converting in scan mode.
 *
 * @param[in]	- Base address of the ADC register.
 * @param[in]	- Base address of the DMA Stream.
 * @param[in]	- DMA Channel.
 * @param[in]	- Number of ADC channels to be transferred.
 * @param[in]	- Destination memory address for the DMA to transfer data to.
 *
 * @return		- None.
 *
 * @note		- ADC Peripheral ->| DMA Stream, Channel
 * 				  ____________________________________________
 * 				  ADC1 ----------->| DMA2, Stream 0, Channel 0
 * 				  ADC1 ----------->| DMA2, Stream 4, Channel 0
 * 				  ADC2 ----------->| DMA2, Stream 2, Channel 1
 * 				  ADC2 ----------->| DMA2, Stream 3, Channel 1
 * 				  ADC3 ----------->| DMA2, Stream 0, Channel 2
 * 				  ADC3 ----------->| DMA2, Stream 1, Channel 2
 * 				  ____________________________________________
 *
 * 				  Source        | Destination       | Flow Controller     | Circular Mode
 * 				  --------------+-------------------+---------------------+--------------
 * 				  AHB Peri Port | AHB Mem Port      | DMA                 | Yes
 */
void ADC_EnableDMA(ADC_TypeDef *ADC_T, DMA_Stream_TypeDef *DMASx, uint8_t dmaChannel, uint8_t numChannels, uint16_t *dest)
{
	ADC_T->CR2 |= ADC_CR2_DMA;
	ADC_T->CR2 |= ADC_CR2_DDS;
	/*Enable the clock for DMA2 in the AHB1 RCC register.*/
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
	/*Clear the DMA_SxCR_EN bit to ensure there is no on-going transmission.*/
	while(((DMASx->CR >> DMA_SxCR_EN_Pos) & 0x1) == SET)
		DMASx->CR &= ~DMA_SxCR_EN;
	/*Set the peripheral port register address in the DMA_SxPAR register. The data will be
	moved from/to this address to/from the peripheral port after the peripheral event.*/
	DMASx->PAR = (uint32_t)&(ADC_T->DR);
	/*Set the memory address in the DMA_SxMA0R register (and in the DMA_SxMA1R
	register in the case of a double buffer mode). The data will be written to or read from
	this memory after the peripheral event.*/
	DMASx->M0AR = (uint32_t)dest;
	/*Configure the total number of data items to be transferred in the DMA_SxNDTR
	register. After each peripheral event or each beat of the burst, this value is
	decremented.*/
	DMASx->NDTR = numChannels;
	/*Select the DMA channel (request) using CHSEL[2:0] in the DMA_SxCR register.*/
	DMASx->CR &= ~DMA_SxCR_CHSEL;
	DMASx->CR |= (dmaChannel << DMA_SxCR_CHSEL_Pos);
	/*If the peripheral is intended to be the flow controller and if it supports this feature, set
	the PFCTRL bit in the DMA_SxCR register.*/
	DMASx->CR &= ~DMA_SxCR_PFCTRL;
	/*Configure the stream priority using the PL[1:0] bits in the DMA_SxCR register.*/
	DMASx->CR &= ~DMA_SxCR_PL;
	DMASx->CR |= (0x2 << DMA_SxCR_PL_Pos); //High Priority Level.
	/*Configure the FIFO usage (enable or disable, threshold in transmission and reception)*/
	DMASx->FCR &= ~DMA_SxFCR_DMDIS; //Disable FIFO to use Direct Mode.
	/*Configure the data transfer direction, peripheral and memory incremented/fixed mode,
	single or burst transactions, peripheral and memory data widths, Circular mode,
	Double buffer mode and interrupts after half and/or full transfer, and/or errors in the
	DMA_SxCR register.*/
	DMASx->CR &= ~DMA_SxCR_DIR;
	DMASx->CR &= ~DMA_SxCR_PINC;
	DMASx->CR |= DMA_SxCR_MINC;
	DMASx->CR |= (0x1 << DMA_SxCR_PSIZE_Pos);
	DMASx->CR |= (0x1 << DMA_SxCR_MSIZE_Pos);
	DMASx->CR |= DMA_SxCR_CIRC;
	DMASx->CR &= ~DMA_SxCR_DBM;
	DMA2->LIFCR |= DMA_LIFCR_CTCIF0;
	DMASx->CR |= DMA_SxCR_TCIE;
	/*Activate the stream by setting the EN bit in the DMA_SxCR register.*/
	DMASx->CR |= DMA_SxCR_EN;
}

/*
 * @fn			- ADC_DisableDMA
 *
 * @brief		- This function
 *
 * @param[in]	- Base address of the ADC register.
 *
 * @return		- None.
 *
 * @note		- None.
 */
void ADC_DisableDMA(ADC_TypeDef *ADC_T, DMA_Stream_TypeDef *DMASx)
{
	DMASx->CR &= ~DMA_SxCR_EN;
	ADC_T->CR2 &= ~ADC_CR2_DMA;
}


void ADC_HandleDMA_TransferFlag(void)
{
    while (((DMA2->LISR >> DMA_LISR_TCIF0_Pos) & 0x1) == 0);  // Check if transfer complete flag is set
    DMA2->LIFCR |= DMA_LIFCR_CTCIF0;              // Clear transfer complete flag
}


/***************************************************************************************/


/***************** ADCx IRQ Handling ***************************************************/

/* Weak function that can be implemented in user application. */
void __attribute__((weak)) ADC_ApplicationEventCallback(uint8_t){}

/*
 * @fn			- ADC_IRQHandling
 *
 * @brief		- This function handles an ADC interrupt.
 *
 * @param[ADC_TypeDef]	- Base Address of the ADC Handle.
 *
 * @return		- None.
 *
 * @note		- The interrupt bits(OVRIE, EOCIE, JEOCIE, AWDIE) must
 * 				  be set in order for an interrupt to be triggered.
 */
void ADC_IRQHandling(ADC_TypeDef *ADC_T)
{
	if(((ADC_T->SR >> 1) & 0x1) == SET)  //End of Conversion flag.
	{
		ADC_HandleEOCIt(ADC_T);
		ADC_ApplicationEventCallback(ADC_END_OF_CONVERSION_REG);
	}
	if(((ADC_T->SR >> 2) & 0x1) == SET)  //End of Injected Conversion flag.
	{
		ADC_HandleJEOCIt(ADC_T);
		ADC_ApplicationEventCallback(ADC_END_OF_CONVERSION_INJ);
	}
	if(((ADC_T->SR >> 0) & 0x1) == SET)  //Analog Watchdog flag.
	{
		ADC_HandleAWDIt(ADC_T);
		ADC_ApplicationEventCallback(ADC_WATCHDOG_SET);
	}
	if(((ADC_T->SR >> 5) & 0x1) == SET)  //Overrun flag.
	{
		ADC_HandleOVRIt(ADC_T);
		ADC_ApplicationEventCallback(ADC_OVERRUN_SET);
	}
}



/***************************************************************************************/


/***************** Private Helper Function Definitions *********************************/

/*
 * @fn			- ADC_HandleAWDIt
 *
 * @brief		- This function handles the Analog Watchdog
 * 				  Interrupt.
 *
 * @param[ADC_TypeDef]	- Base Address of the ADC Handle.
 *
 * @return		- None.
 *
 * @note		- Private Helper Function.
 */
static void ADC_HandleAWDIt(ADC_TypeDef *ADC_T)
{
	ADC_T->SR &= ~ADC_SR_AWD;
}

/*
 * @fn			- ADC_HandleEOCIt
 *
 * @brief		- This function handles the End of Conversion
 * 				  Interrupt.
 *
 * @param[ADC_TypeDef]	- Base Address of the ADC Handle.
 *
 * @return		- None.
 *
 * @note		- Private Helper Function.
 */
static void ADC_HandleEOCIt(ADC_TypeDef *ADC_T)
{
	ADC_T->SR &= ~ADC_SR_EOC;
}

/*
 * @fn			- ADC_HandleJEOCIt
 *
 * @brief		- This function handles the Injection End of
 * 				  Conversion Interrupt.
 *
 * @param[ADC_TypeDef]	- Base Address of the ADC Handle.
 *
 * @return		- None.
 *
 * @note		- Private Helper Function.
 */
static void ADC_HandleJEOCIt(ADC_TypeDef *ADC_T)
{
	ADC_T->SR &= ~ADC_SR_JEOC;
}

/*
 * @fn			- ADC_HandleOVRIt
 *
 * @brief		- This function handles the Overrun
 * 				  Interrupt.
 *
 * @param[ADC_TypeDef*]	- Base Address of the ADC Handle.
 *
 * @return		- None.
 *
 * @note		- Private Helper Function.
 */
static void ADC_HandleOVRIt(ADC_TypeDef *ADC_T)
{
	ADC_T->SR &= ~ADC_SR_OVR;
}

/***************************************************************************************/
