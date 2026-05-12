#include"tim.h"

uint32_t 			sysClkFrequency = 0;
uint32_t			apb1ClkFrequency = 0;
uint32_t			apb2ClkFrequency = 0;

/***************** Private Helper Function Headers *************************************/

static void TIM_periClkCtrl(TIM_TypeDef *TIM, uint8_t setting);
static uint32_t calcClkFrequency(uint8_t clkSource);
static uint32_t calcApbClk(uint8_t u);
static void configClkPsc(TIM_TypeDef *TIM);

/***************************************************************************************/


/***************** User Application Exposed Function Definitions ***********************/

/*
 * @fn						- TIM_Init.
 *
 * @brief					- This function initializes one of the timer peripherals
 * 				  			  Which ever timer typedef is passed as the first argument is
 * 				    		  the one that gets initialized.
 *
 * @param[TIM_TypeDef*]		- Base address of the timer peripheral.
 * @param[TIM_Config_t]		- Timer configuration structure.
 *
 * @return					- None.
 *
 * @note					- If interrupts are to be used, you must configure the NVIC IRQ
 * 							  and set IRQ priorities in your application.
 */
void TIM_Init(TIM_TypeDef *TIM)
{
	// Enable clock access to the appropriate timer.
	TIM_periClkCtrl(TIM, SET);
	// Determine clock source and system clock frequency, set timer prescaler.
	configClkPsc(TIM);
	// Make sure Update Disable is cleared  (0: UEV enabled.).
	TIM->CR1 &= ~TIM_CR1_UDIS;
	SCB->CPACR |= ((3UL << 10*2) | (3UL << 11*2));
}

/*
 * @fn						- TIM_Deinit.
 *
 * @brief					- This function deinitializes the timer peripheral.
 *
 * @param[TIM_TypeDef]		- Base address of the timer peripheral.
 *
 * @return					- None.
 *
 * @note					- None.
 */
void TIM_Deinit(TIM_TypeDef *TIM)
{
	TIM_periClkCtrl(TIM, RESET);
}

/*
 * @fn						- TIM_SetCounterMode.
 *
 * @brief					- This function sets the timer's counter in either up counting
 * 							  or down counting.
 *
 * @param[in]				- Base address of the timer peripheral.
 * @param[in]				- Counting mode (up or down).
 * 							  COUNT_UP_MODE, COUNT_DOWN_MODE.
 *
 * @return					- None.
 *
 * @note					- None.
 */
void TIM_SetCounterMode(TIM_TypeDef *TIM, uint8_t counterMode)
{
	switch(counterMode)
	{
		case COUNT_UP_MODE: TIM->CR1 &= ~TIM_CR1_DIR; break;
		case COUNT_DOWN_MODE: TIM->CR1 |= TIM_CR1_DIR; break;
		default: return;
	}
}

/*
 * @fn						- TIM_SetAutoReloadValue.
 *
 * @brief					- This function writes the 16-bit value of the second argument to
 * 							  the timer's Auto Reload Register.
 *
 * @param[in]				- Base address of the timer peripheral.
 * @param[in]				- Value to write to the auto reload register.
 *
 * @return					- None.
 *
 * @note					- None.
 */
void TIM_SetAutoReloadValue(TIM_TypeDef *TIM, uint16_t autoReloadValue)
{
	TIM->ARR = autoReloadValue;
}

/*
 * @fn						- TIM_SetAutoReloadPreload.
 *
 * @brief					- This function enables or disables the auto reload preload
 * 							  feature.
 *
 * @param[in]				- Base address of the timer peripheral.
 * @param[in]				- ENABLE or DISABLE.
 *
 * @return					- None.
 *
 * @note					- None.
 */
void TIM_SetAutoReloadPreload(TIM_TypeDef *TIM, uint8_t enableOrDisable)
{
	switch(enableOrDisable)
	{
		case ENABLE: TIM->CR1 |= TIM_CR1_ARPE; break;
		case DISABLE: TIM->CR1 &= ~TIM_CR1_ARPE; break;
		default: return;
	}
}

/*
 * @fn						- TIM_SetRepititionCount.
 *
 * @brief					- This function
 *
 * @param[in]				- Base address of the timer peripheral.
 * @param[in]				-
 *
 * @return					- None.
 *
 * @note					- None.
 */
void TIM_SetRepititionCount(TIM_TypeDef *TIM, uint8_t repCnt)
{

}

/*
 * @fn						- TIM_SetPWM_Mode.
 *
 * @brief					- This function turns on the timer's PWM mode.
 *
 * @param[in]				- Base address of the timer peripheral.
 * @param[in]				- Mode of the PWM.
 *
 * @return					- None.
 *
 * @note					- None.
 */
void TIM_SetPWM_Mode(TIM_TypeDef *TIM, uint8_t pdmMode)
{
	TIM->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1; // PWM mode 1
	TIM->CCER |= TIM_CCER_CC1E;                        // Enable output
	TIM->CR1 |= TIM_CR1_CEN;                           // Start timer
}

/*
 * @fn						- TIM_SetPWM_Frequency.
 *
 * @brief					- This function turns on the timer's PWM mode.
 *
 * @param[in]				- Base address of the timer peripheral.
 * @param[in]				- Desired frequency.
 *
 * @return					- None.
 *
 * @note					- None.
 */
void TIM_SetPWM_Frequency(TIM_TypeDef *TIM, float freq)
{
	TIM->ARR = (uint16_t)(sysClkFrequency / ((freq * 1.05946) * TIM->PSC));
}

/*
 * @fn						- TIM_SetPWM_DutyCycle.
 *
 * @brief					- This function causes a delay in the program.
 * 							  The length of the delay is determined by the argument passed.
 *
 * @param[in]				- Base address of the timer peripheral.
 * @param[in]				- Time in microseconds(us) to delay.
 * @param[in]				- Time in microseconds(us) to delay.
 *
 * @return					- None.
 *
 * @note					- None.
 */
void TIM_SetPWM_DutyCycle(TIM_TypeDef *TIM, float freq, uint8_t percentage)
{
	TIM->CCR1 = (uint16_t)((sysClkFrequency / ((freq * 1.05946) * TIM->PSC))) * (percentage / 100.0);
	//TIM->CCR1 = (uint16_t)(freq * (percentage / 100.0));
}

/*
 * @fn						- TIM_SetCounterMode.
 *
 * @brief					- This function causes a delay in the program.
 * 							  The length of the delay is determined by the argument passed.
 *
 * @param[in]				- Base address of the timer peripheral.
 * @param[in]				- Time in microseconds(us) to delay.
 *
 * @return					- None.
 *
 * @note					- None.
 */
void TIM_SetCenterAlignMode(TIM_TypeDef *TIM, uint8_t alignMode)
{

}

/*
 * @fn						- TIM_Delay_us.
 *
 * @brief					- This function causes a delay in the program.
 * 							  The length of the delay is determined by the argument passed.
 *
 * @param[in]				- Base address of the timer peripheral.
 * @param[in]				- Time in microseconds(us) to delay.
 *
 * @return					- None.
 *
 * @note					- None.
 */
void TIM_Delay_us(TIM_TypeDef *TIM, uint16_t us)
{
	TIM->ARR = us - 1;
	TIM->CR1 |= TIM_CR1_CEN;
	while(((TIM->SR >> TIM_SR_UIF_Pos) & 0x1) == RESET){}
	TIM->SR &= ~TIM_SR_UIF;
}

/*
 * @fn						- TIM_Delay_ms.
 *
 * @brief					- This function causes a delay in the program.
 * 							  The length of the delay is determined by the argument passed.
 *
 * @param[in]				- Base address of the timer peripheral.
 * @param[in]				- Time in milliseconds(ms) to delay.
 *
 * @return					- None.
 *
 * @note					- None.
 */
void TIM_Delay_ms(TIM_TypeDef *TIM, uint32_t ms)
{
	for(uint32_t i = 0; i < 1000; i++)
		TIM_Delay_us(TIM, ms);
}

/*
 * @fn						- TIM_SartTimer.
 *
 * @brief					- This function causes the timer to start.
 *
 * @param[in]				- Base address of the timer peripheral.
 *
 * @return					- None.
 *
 * @note					- None.
 */
void TIM_SartTimer(TIM_TypeDef *TIM)
{
	TIM->CR1 |= TIM_CR1_CEN;
}

/*
 * @fn						- TIM_StopTimer.
 *
 * @brief					- This function causes the timer to stop.
 *
 * @param[in]				- Base address of the timer peripheral.
 *
 * @return					- None.
 *
 * @note					- None.
 */
void TIM_StopTimer(TIM_TypeDef *TIM)
{
	TIM->CR1 &= ~TIM_CR1_CEN;
}

/***************************************************************************************/


/***************** TIMx IRQ Handling ***************************************************/

/* Weak function that can be implemented in user application. */
void __attribute__((weak)) TIM_ApplicationEventCallback(void){}

void TIM1_BRK_TIM9_IRQHandling(TIM_TypeDef *TIM)
{

}

void TIM1_UP_TIM10_IRQHandling(TIM_TypeDef *TIM)
{

}

void TIM1_TRG_COM_TIM11_IRQHandling(TIM_TypeDef *TIM)
{

}

void TIM1_CC_IRQHandling(TIM_TypeDef *TIM)
{

}

void TIM2_IRQHandling(TIM_TypeDef *TIM)
{
	if(((TIM->SR >> 0) & 0x1) == 1)
	{
		// Clear update interrupt flag
		TIM->SR &= ~TIM_SR_UIF;
		TIM_ApplicationEventCallback();
	}
}

void TIM3_IRQHandling(TIM_TypeDef *TIM)
{

}

void TIM4_IRQHandling(TIM_TypeDef *TIM)
{

}

void TIM8_BRK_TIM12_IRQHandling(TIM_TypeDef *TIM)
{

}

void TIM8_UP_TIM13_IRQHandling(TIM_TypeDef *TIM)
{

}

void TIM8_TRG_COM_TIM14_IRQHandling(TIM_TypeDef *TIM)
{

}

void TIM8_CC_IRQHandling(TIM_TypeDef *TIM)
{

}

void TIM5_IRQHandling(TIM_TypeDef *TIM)
{

}

void TIM6_DAC_IRQHandling(TIM_TypeDef *TIM)
{

}

void TIM7_IRQHandling(TIM_TypeDef *TIM)
{

}

/***************************************************************************************/


/***************** Private Helper Function Definitions *********************************/

static void TIM_periClkCtrl(TIM_TypeDef *TIM, uint8_t setting)
{
	if(setting == SET)		// Enabling Peripheral Clocks
	{
		if(TIM == TIM1)								// Advanced Timers
			RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
		else if(TIM == TIM8)
			RCC->APB2ENR |= RCC_APB2ENR_TIM8EN;
		else if(TIM == TIM2)						// Basic Timers
			RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
		else if(TIM == TIM3)
			RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
		else if(TIM == TIM4)
			RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
		else if(TIM == TIM5)
			RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
	}
	else					// Disabling Peripheral Clocks
	{
		if(TIM == TIM1)								// Advanced Timers
			RCC->APB2ENR &= ~RCC_APB2ENR_TIM1EN;
		else if(TIM == TIM8)
			RCC->APB2ENR &= ~RCC_APB2ENR_TIM8EN;
		else if(TIM == TIM2)						// Basic Timers
			RCC->APB1ENR &= ~RCC_APB1ENR_TIM2EN;
		else if(TIM == TIM3)
			RCC->APB1ENR &= ~RCC_APB1ENR_TIM3EN;
		else if(TIM == TIM4)
			RCC->APB1ENR &= ~RCC_APB1ENR_TIM4EN;
		else if(TIM == TIM5)
			RCC->APB1ENR &= ~RCC_APB1ENR_TIM5EN;
	}
}

static uint32_t calcClkFrequency(uint8_t clkSource)
{
    uint32_t sysclk;
    uint8_t pllSrc, pllm, plln, pllp;

    if (clkSource == 0x2) {  // PLL is the source
        pllSrc = ((RCC->PLLCFGR >> RCC_PLLCFGR_PLLSRC_Pos) & 0x1);  // Check PLLSRC bit field
        if (pllSrc == 0x01) {  // HSE as PLL source
            sysclk = 8000000;  // Assume 8 MHz HSE
        } else { 				// HSE as PLL source
            sysclk = 16000000;  // Assume 16 MHz HSI (default)
        }

        pllm = ((RCC->PLLCFGR >> RCC_PLLCFGR_PLLM_Pos) & 0x3F);
        plln = ((RCC->PLLCFGR >> RCC_PLLCFGR_PLLN_Pos) & 0x1FF);
        pllp = (((RCC->PLLCFGR >> RCC_PLLCFGR_PLLP_Pos) & 0x3) + 1);  // Divide by (2n+1)
        pllp *= 2;

        sysclk = (sysclk / pllm) * plln / pllp;
    } else if (clkSource == 0x1) {  // HSE is the source
        sysclk = 8000000;  // Assume 8 MHz HSE
    } else if (clkSource == 0x0){
        sysclk = 16000000;  // Assume 16 MHz HSI (default)
    }

    // Apply AHB prescaler
    uint8_t ahbPresc = ((RCC->CFGR >> RCC_CFGR_HPRE_Pos) & 0xF);
    switch(ahbPresc)
    {
		case 0x8: sysclk /= 2U;   break;
		case 0x9: sysclk /= 4U;   break;
		case 0xA: sysclk /= 8U;   break;
		case 0xB: sysclk /= 16U;  break;
		case 0xC: sysclk /= 64U;  break;
		case 0xD: sysclk /= 128U; break;
		case 0xE: sysclk /= 256U; break;
		case 0xF: sysclk /= 512U; break;
    }
    return sysclk;
}

static uint32_t calcApbClk(uint8_t u)
{
	switch(u)
	{
	case 0x4: return sysClkFrequency / 2; break;
	case 0x5: return sysClkFrequency / 4; break;
	case 0x6: return sysClkFrequency / 8; break;
	case 0x7: return sysClkFrequency / 16; break;
	}
	return sysClkFrequency;
}

static void configClkPsc(TIM_TypeDef *TIM)
{
	uint8_t clkSource = 0;
	clkSource = ((RCC->CFGR >> RCC_CFGR_SWS_Pos) & 0x3);
	sysClkFrequency = calcClkFrequency(clkSource);
	apb1ClkFrequency = calcApbClk((RCC->CFGR >> RCC_CFGR_PPRE1_Pos) & 0x7);
	apb2ClkFrequency = calcApbClk((RCC->CFGR >> RCC_CFGR_PPRE2_Pos) & 0x7);
	uint16_t apb1Psc, apb2Psc = 0;
	if(TIM_APB1(TIM))
	{
		if(apb1ClkFrequency % 1000000 == 0)
		{
			apb1Psc = apb1ClkFrequency / 1000000;
			TIM->PSC = apb1Psc - 1;
		}
	}
	else
	{
		if(apb1ClkFrequency % 1000000 == 0)
		{
			apb2Psc = apb1ClkFrequency / 1000000;
			TIM->PSC = apb2Psc - 1;
		}
	}
}

/***************************************************************************************/
