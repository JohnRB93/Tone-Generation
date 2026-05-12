#ifndef TIM_H_
#define TIM_H_

#include"stm32f4xx.h"

/***************** Macro Definitions ***************************************************/

// Counter Direction Mode
#define COUNT_UP_MODE					0				// Counter used as upcounter
#define COUNT_DOWN_MODE					1				// Counter used as downcounter

// Counter Center-Aligned Mode
#define COUNT_EDGE_ALIGNED_MODE			0				// The counter counts up or down depending on the direction bit (DIR).
#define COUNT_CENTER_ALIGNED_MODE1		1				// The counter counts up and down alternatively.
														// 	Output compare interrupt flags of channels configured in output
														// 	(CCxS=00 in TIMx_CCMRx register) are set only when the counter is counting down.
#define COUNT_CENTER_ALIGNED_MODE2		2				// The counter counts up and down alternatively.
														// 	Output compare interrupt flags of channels configured in output
														// 	(CCxS=00 in TIMx_CCMRx register) are set only when the counter is counting up.
#define COUNT_CENTER_ALIGNED_MODE3		3				// The counter counts up and down alternatively.
														// 	Output compare interrupt flags of channels configured in output
														// 	(CCxS=00 in TIMx_CCMRx register) are set both when the counter is counting up or down.

// One-Pulse Mode Option
#define PULSE_MODE_NOT_ENABLE			0				// Counter is not stopped at update event
#define PULSE_MODE_ENABLE				1				// Counter stops counting at the next update event (clearing the bit CEN)

// Clock Selection
#define INTERNAL_CLOCK					0
#define EXTERNAL_CLK_MODE_1				1
#define EXTERNAL_CLK_MODE_2				2
#define INTERNAL_TRG_INPUTS				3

// Auto-reload preload enable
#define TIMX_ARR_REG_NOT_BUFF			0				// TIMx_ARR register is not buffered
#define TIMX_ARR_REG_BUFF				1				// TIMx_ARR register is buffered

// TIMx Pulse Width Modulation Counting Direction Mode
#define PWM_UP_COUNTING					0
#define PWM_DOWN_COUNTING				1
#define PWM_CENTER_ALIGNED_COUNTING		2

// TIMx PWM Mode
#define PWM_MODE_1						0
#define PWM_MODE_2						1


// TIMx bus macros
#define TIM_APB1(x) ((x == TIM2) || (x == TIM3) || (x == TIM4) || (x == TIM5) || (x == TIM12) || \
						(x == TIM13) || (x == TIM14) || (x == TIM6) || (x == TIM7))
#define TIM_APB2(x) ((x == TIM1) || (x == TIM8) || (x == TIM9) || (x == TIM10) || (x == TIM11))

/***************************************************************************************/
/*                         APIs supported by this driver                               */
/***************************************************************************************/

void TIM_Init(TIM_TypeDef *TIM);
void TIM_Deinit(TIM_TypeDef *TIM);

void TIM_SetCounterMode(TIM_TypeDef *TIM, uint8_t counterMode);
void TIM_SetAutoReloadValue(TIM_TypeDef *TIM, uint16_t reloadValue);
void TIM_SetAutoReloadPreload(TIM_TypeDef *TIM, uint8_t autoReloadEnableOrDisable);
void TIM_SetRepititionCount(TIM_TypeDef *TIM, uint8_t repCnt);

void TIM_SetPWM_Mode(TIM_TypeDef *TIM, uint8_t pdmMode);
void TIM_SetPWM_Frequency(TIM_TypeDef *TIM, float freq);
void TIM_SetPWM_DutyCycle(TIM_TypeDef *TIM, float freq, uint8_t percentage);
void TIM_SetCenterAlignMode(TIM_TypeDef *TIM, uint8_t alignMode);

void TIM_Delay_us(TIM_TypeDef *TIM, uint16_t us);
void TIM_Delay_ms(TIM_TypeDef *TIM, uint32_t ms);
void TIM_SartTimer(TIM_TypeDef *TIM);
void TIM_StopTimer(TIM_TypeDef *TIM);

void TIM1_BRK_TIM9_IRQHandling(TIM_TypeDef *TIM);
void TIM1_UP_TIM10_IRQHandling(TIM_TypeDef *TIM);
void TIM1_TRG_COM_TIM11_IRQHandling(TIM_TypeDef *TIM);
void TIM1_CC_IRQHandling(TIM_TypeDef *TIM);
void TIM2_IRQHandling(TIM_TypeDef *TIM);
void TIM3_IRQHandling(TIM_TypeDef *TIM);
void TIM4_IRQHandling(TIM_TypeDef *TIM);
void TIM8_BRK_TIM12_IRQHandling(TIM_TypeDef *TIM);
void TIM8_UP_TIM13_IRQHandling(TIM_TypeDef *TIM);
void TIM8_TRG_COM_TIM14_IRQHandling(TIM_TypeDef *TIM);
void TIM8_CC_IRQHandling(TIM_TypeDef *TIM);
void TIM5_IRQHandling(TIM_TypeDef *TIM);
void TIM6_DAC_IRQHandling(TIM_TypeDef *TIM);
void TIM7_IRQHandling(TIM_TypeDef *TIM);

#endif /* TIM_H_ */
