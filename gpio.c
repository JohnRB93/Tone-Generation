#include"gpio.h"
#include"irq.h"

/***************** Private Helper Function Headers *************************************/
static uint8_t GPIO_BaseAddressToCode(GPIO_TypeDef *GPIOx);
//static uint8_t GPIO_PinNumberToExtiCode(uint8_t pinNum);


/*********************Peripheral Clock Setup***************************************/
/*
 * @fn			- GPIO_PeriClockControl
 *
 * @brief		- This function enables or disables peripheral clock for the given GPIO port.
 *
 * @param[in]	- Base address of the GPIO peripheral.
 * @param[in]	- ENABLE or DISABLE macros.
 *
 * @return		- None.
 *
 * @note		- None.
 */
void GPIO_PeriClockControl(GPIO_TypeDef *GPIOx, uint8_t enOrDis)
{
	if(enOrDis == ENABLE)
	{
		if(GPIOx == GPIOA)
			RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
		else if(GPIOx == GPIOB)
			RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
		else if(GPIOx == GPIOC)
			RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
		else if(GPIOx == GPIOD)
			RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
		else if(GPIOx == GPIOE)
			RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
		else if(GPIOx == GPIOF)
			RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;
		else if(GPIOx == GPIOG)
			RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;
		else if(GPIOx == GPIOH)
			RCC->AHB1ENR |= RCC_AHB1ENR_GPIOHEN;
		else if(GPIOx == GPIOI)
			RCC->AHB1ENR |= RCC_AHB1ENR_GPIOIEN;
	}
	else
	{
		if(GPIOx == GPIOA)
			RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOAEN;
		else if(GPIOx == GPIOB)
			RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOBEN;
		else if(GPIOx == GPIOC)
			RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOCEN;
		else if(GPIOx == GPIOD)
			RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIODEN;
		else if(GPIOx == GPIOE)
			RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOEEN;
		else if(GPIOx == GPIOF)
			RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOFEN;
		else if(GPIOx == GPIOG)
			RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOGEN;
		else if(GPIOx == GPIOH)
			RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOHEN;
		else if(GPIOx == GPIOI)
			RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOIEN;
	}
}


/*********************Initialization and De-initialization*************************/

/*
 * @fn			- GPIO_Init
 *
 * @brief		- This function initializes the GPIO Peripheral.
 *
 * @param[in]	- Base address of the GPIO Peripheral.
 *
 * @return		- None.
 *
 * @note		- None.
 */
void GPIO_Init(GPIO_TypeDef *GPIOx)
{
	//Enable the peripheral clock.
	GPIO_PeriClockControl(GPIOx, ENABLE);
}

/*
 * @fn			- GPIO_DeInit
 *
 * @brief		- This function de-initializes the GPIO Peripheral to its reset state.
 *
 * @param[in]	- Base address of the GPIO Peripheral.
 *
 * @return		- None.
 *
 * @note		- None.
 */
void GPIO_DeInit(GPIO_TypeDef *GPIOx)
{
	if(GPIOx == GPIOA)
	{
		RCC->AHB1RSTR |= RCC_AHB1RSTR_GPIOARST;
		RCC->AHB1RSTR &= ~RCC_AHB1RSTR_GPIOARST;
	}else if(GPIOx == GPIOB)
	{
		RCC->AHB1RSTR |= RCC_AHB1RSTR_GPIOBRST;
		RCC->AHB1RSTR &= ~RCC_AHB1RSTR_GPIOBRST;
	}else if(GPIOx == GPIOC)
	{
		RCC->AHB1RSTR |= RCC_AHB1RSTR_GPIOCRST;
		RCC->AHB1RSTR &= ~RCC_AHB1RSTR_GPIOCRST;
	}else if(GPIOx == GPIOD)
	{
		RCC->AHB1RSTR |= RCC_AHB1RSTR_GPIODRST;
		RCC->AHB1RSTR &= ~RCC_AHB1RSTR_GPIODRST;
	}else if(GPIOx == GPIOE)
	{
		RCC->AHB1RSTR |= RCC_AHB1RSTR_GPIOERST;
		RCC->AHB1RSTR &= ~RCC_AHB1RSTR_GPIOERST;
	}else if(GPIOx == GPIOF)
	{
		RCC->AHB1RSTR |= RCC_AHB1RSTR_GPIOFRST;
		RCC->AHB1RSTR &= ~RCC_AHB1RSTR_GPIOFRST;
	}else if(GPIOx == GPIOG)
	{
		RCC->AHB1RSTR |= RCC_AHB1RSTR_GPIOGRST;
		RCC->AHB1RSTR &= ~RCC_AHB1RSTR_GPIOGRST;
	}else if(GPIOx == GPIOH)
	{
		RCC->AHB1RSTR |= RCC_AHB1RSTR_GPIOHRST;
		RCC->AHB1RSTR &= ~RCC_AHB1RSTR_GPIOHRST;
	}else if(GPIOx == GPIOI)
	{
		RCC->AHB1RSTR |= RCC_AHB1RSTR_GPIOIRST;
		RCC->AHB1RSTR &= ~RCC_AHB1RSTR_GPIOIRST;
	}
}

/*********************Pin Configurations*******************************************/

/*
 * @fn			- GPIO_SetPinMode
 *
 * @brief		- This function sets the GPIO pin's pin mode.
 *
 * @param[in]	- Base address of the GPIO Peripheral.
 * @param[in]	- GPIO pin number(GPIO_PIN_NO_0, GPIO_PIN_NO_1, ... GPIO_PIN_NO_15).
 * @param[in]	- GPIO pin mode setting(Input Mode[GPIO_MODE_IN], Output Mode[GPIO_MODE_OUT],
 * 				  Alternate Function Mode[GPIO_MODE_ALTFN], or Analog Mode[GPIO_MODE_ANALOG]).
 *
 * @return		- None.
 *
 * @note		- None.
 */
void GPIO_SetPinMode(GPIO_TypeDef *GPIOx, uint8_t pinNumber, uint8_t pinMode)
{
	switch(pinMode)
	{
		case GPIO_MODE_IN:     GPIOx->MODER &= ~(0x3 << pinNumber * 2); break;
		case GPIO_MODE_OUT:    GPIOx->MODER &= ~(0x3 << pinNumber * 2); GPIOx->MODER |= (0x1 << pinNumber * 2); break;
		case GPIO_MODE_ALTFN:  GPIOx->MODER &= ~(0x3 << pinNumber * 2); GPIOx->MODER |= (0x2 << pinNumber * 2); break;
		case GPIO_MODE_ANALOG: GPIOx->MODER &= ~(0x3 << pinNumber * 2); GPIOx->MODER |= (0x3 << pinNumber * 2); break;
		default: return;
	}
}

/*
 * @fn			- GPIO_SetOutputType
 *
 * @brief		- This function sets the GPIO pin's output type if in output
 * 				  mode.
 *
 * @param[in]	- Base address of the GPIO Peripheral.
 * @param[in]	- GPIO pin number(GPIO_PIN_NO_0, GPIO_PIN_NO_1, ... GPIO_PIN_NO_15).
 * @param[in]	- GPIO output type setting(Output push-pull[GPIO_OP_TYPE_PP] or Output open-drain[GPIO_OP_TYPE_OD])
 *
 * @return		- None.
 *
 * @note		- None.
 */
void GPIO_SetOutputType(GPIO_TypeDef *GPIOx, uint8_t pinNumber, uint8_t outputType)
{
	switch(outputType)
	{
		case GPIO_OP_TYPE_PP: GPIOx->OTYPER &= ~(1 << pinNumber); break;
		case GPIO_OP_TYPE_OD: GPIOx->OTYPER |=  (1 << pinNumber); break;
		default: return;
	}
}

/*
 * @fn			- GPIO_SetPinSpeed
 *
 * @brief		- This function sets the speed of the GPIO pin according to the
 * 				  arguments passed.
 *
 * @param[in]	- Base address of the GPIO Peripheral.
 * @param[in]	- GPIO pin number(GPIO_PIN_NO_0, GPIO_PIN_NO_1, ... GPIO_PIN_NO_15).
 * @param[in]	- GPIO speed setting(GPIO_SPEED_LOW, GPIO_SPEED_MEDIUM, GPIO_SPEED_HIGH,
 * 				  or GPIO_SPEED_VERY_HIGH).
 *
 * @return		- None.
 *
 * @note		- None.
 */
void GPIO_SetPinSpeed(GPIO_TypeDef *GPIOx, uint8_t pinNumber, uint8_t pinSpeed)
{
	switch(pinSpeed)
	{
		case GPIO_SPEED_LOW:       GPIOx->OSPEEDR &= ~(0x3 << pinNumber * 2); break;
		case GPIO_SPEED_MEDIUM:    GPIOx->OSPEEDR &= ~(0x3 << pinNumber * 2); GPIOx->OSPEEDR |= (0x1 << pinNumber * 2); break;
		case GPIO_SPEED_HIGH:      GPIOx->OSPEEDR &= ~(0x3 << pinNumber * 2); GPIOx->OSPEEDR |= (0x2 << pinNumber * 2); break;
		case GPIO_SPEED_VERY_HIGH: GPIOx->OSPEEDR &= ~(0x3 << pinNumber * 2); GPIOx->OSPEEDR |= (0x3 << pinNumber * 2); break;
		default: return;
	}
}

/*
 * @fn			- GPIO_SetPullUpPullDown
 *
 * @brief		- This function sets the GPIO pin's pull-up or pull-down resistor
 * 				  setting.
 *
 * @param[in]	- Base address of the GPIO Peripheral.
 * @param[in]	- GPIO pin number(GPIO_PIN_NO_0, GPIO_PIN_NO_1, ... GPIO_PIN_NO_15).
 * @param[in]	- GPIO pull-up or pull-down setting(No pull-up, pull-down[GPIO_NO_PUPD],
 * 				  Pull-up[GPIO_PIN_PU], or Pull-down[GPIO_PIN_PD]).
 *
 * @return		- None.
 *
 * @note		- None.
 */
void GPIO_SetPullUpPullDown(GPIO_TypeDef *GPIOx, uint8_t pinNumber, uint8_t pupd)
{
	switch(pupd)
	{
		case GPIO_NO_PUPD: GPIOx->PUPDR &= ~(0x3 << pinNumber * 2); break;
		case GPIO_PIN_PU:  GPIOx->PUPDR &= ~(0x3 << pinNumber * 2); GPIOx->PUPDR |= (0x1 << pinNumber * 2); break;
		case GPIO_PIN_PD:  GPIOx->PUPDR &= ~(0x3 << pinNumber * 2); GPIOx->PUPDR |= (0x2 << pinNumber * 2); break;
		default: return;
	}
}

/*
 * @fn			- GPIO_SetAlternateConfig
 *
 * @brief		- This function sets the alternate function configuration for
 * 				  the GPIO pin specified in the first two arguments.
 *
 * @param[in]	- Base address of the GPIO Peripheral.
 * @param[in]	- GPIO pin number(GPIO_PIN_NO_0, GPIO_PIN_NO_1, ... GPIO_PIN_NO_15).
 * @param[in]	- Alternate function number(GPIO_AF_00, GPIO_AF_01, ... GPIO_AF_15).
 *
 * @return		- None.
 *
 * @note		- None.
 */
void GPIO_SetAlternateConfig(GPIO_TypeDef *GPIOx, uint8_t pinNumber, uint8_t AF_config)
{
	if(pinNumber > GPIO_PIN_NO_7)
	{// Configure Alternate Functionality for pins 8 through 15.
		if(AF_config == GPIO_AF_00)
			GPIOx->AFR[1] &= ~(0xF << (pinNumber * 4));
		else if((AF_config > GPIO_AF_01) && (AF_config <= GPIO_AF_15))
			GPIOx->AFR[1] |= (AF_config << (pinNumber * 4));
		else
			return;
	}
	else
	{// Configure Alternate Functionality for pins 0 through 7.
		if(AF_config == GPIO_AF_00)
			GPIOx->AFR[0] &= ~(0xF << (pinNumber * 4));
		else if((AF_config > GPIO_AF_01) && (AF_config <= GPIO_AF_15))
			GPIOx->AFR[0] |= (AF_config << (pinNumber * 4));
		else
			return;
	}
}

/*
 * @fn			- GPIO_LockRegister
 *
 * @brief		- This function Locks the port pin configuration
 * 				  until the peripheral or the MCU is reset.
 *
 * @param[in]	- Base address of the GPIOx port register.
 * @param[in]	- An array of pins to be locked.
 * @param[in]	- Length of array.
 *
 * @return		- None.
 *
 * @note		- None.
 */
void GPIO_LockRegister(GPIO_TypeDef *GPIOx, uint8_t *PinNumber, uint8_t length)
{
	if(((GPIOx->LCKR >> 16) & 0x1) == RESET)
	{
		uint32_t pins = 0;
		for(uint8_t i = 0; i < length; i++)
		{
			pins |= (1 << *PinNumber);
			if(!(i + 1 == length))
				PinNumber++;
		}
		GPIOx->LCKR = pins;
		//Lock Sequence.
		GPIOx->LCKR = (uint32_t)((GPIOx->LCKR | (1 << 16)) + ((GPIOx->LCKR >> 15) & 0xf));
		GPIOx->LCKR = (uint32_t)((GPIOx->LCKR & ~(1 << 16)) + ((GPIOx->LCKR >> 15) & 0xf));
		GPIOx->LCKR = (uint32_t)((GPIOx->LCKR | (1 << 16)) + ((GPIOx->LCKR >> 15) & 0xf));
		GPIOx->LCKR;
		while(!((GPIOx->LCKR >> 16) & 0x1));
	}
}

/*********************Read Data and Write Data*************************************/
/*
 * @fn			- GPIO_ReadFromInputPin
 *
 * @brief		- This function reads data from the input pin.
 *
 * @param[in]	- Base address of the GPIOx register.
 * @param[in]	- GPIO pin number(GPIO_PIN_NO_0, GPIO_PIN_NO_1, ... GPIO_PIN_NO_15).
 *
 * @return		- uint8_t, 0 or 1.
 *
 * @note		- None.
 */
uint8_t GPIO_ReadFromInputPin(GPIO_TypeDef *GPIOx, uint8_t PinNumber)
{
	return (uint8_t)((GPIOx->IDR >> PinNumber) & 0x00000001);
}

/*
 * @fn			- GPIO_ReadFromInputPort
 *
 * @brief		- This function reads data from the input port.
 *
 * @param[in]	- Base address of the GPIOx port register.
 *
 * @return		- uint16_t.
 *
 * @note		- None.
 */
uint16_t GPIO_ReadFromInputPort(GPIO_TypeDef *GPIOx)
{
	return (uint16_t)(GPIOx->IDR);
}

/*
 * @fn			- GPIO_WriteToOutputPin
 *
 * @brief		- This function writes data to the GPIO output pin.
 *
 * @param[GPIO_TypeDef*]	- Base address of the GPIOx port register.
 * @param[uint8_t]			- GPIO pin number(GPIO_PIN_NO_0, GPIO_PIN_NO_1, ... GPIO_PIN_NO_15).
 * @param[uint8_t]			- Number (0 or 1) to write to the pin.
 *
 * @return		- None.
 *
 * @note		- None.
 */
void GPIO_WriteToOutputPin(GPIO_TypeDef *GPIOx, uint8_t PinNumber, uint8_t Value)
{
	if(Value == SET) //Write 1 to the output data register at the bit field corresponding to the pin.
		GPIOx->ODR |= (1 << PinNumber);
	else //Write 0 to the output data register at the bit field corresponding to the pin.
		GPIOx->ODR &= ~(1 << PinNumber);
}

/*
 * @fn			- GPIO_WriteToOutputPort
 *
 * @brief		- This function writes data to the GPIO output port.
 *
 * @param[in]	- Base address of the GPIOx port register.
 * @param[in]	- Number value to write to the pin.
 *
 * @return		- None.
 *
 * @note		- None.
 */
void GPIO_WriteToOutputPort(GPIO_TypeDef *GPIOx, uint16_t Value)
{
	GPIOx->ODR = Value;
}

/*
 * @fn			- GPIO_ToggleOutputPin
 *
 * @brief		- This function toggles the GPIO pin between high(1) and low(0).
 *
 * @param[in]	- Base address of the GPIOx port register.
 * @param[in]	- GPIO pin number(GPIO_PIN_NO_0, GPIO_PIN_NO_1, ... GPIO_PIN_NO_15).
 *
 * @return		- None.
 *
 * @note		- None.
 */
void GPIO_ToggleOutputPin(GPIO_TypeDef *GPIOx, uint8_t PinNumber)
{
	GPIOx->ODR ^= (1 << PinNumber);
}


/*********************EXTI Configuration*************************************/

/*
 * @fn			- GPIO_SetInterrupts
 *
 * @brief		- This function enables the EXTI interrupts for the GPIO pin
 * 				  specified in the first two arguments.
 *
 * @param[in]	- Base address of the GPIOx port register.
 * @param[in]	- GPIO pin number(GPIO_PIN_NO_0, GPIO_PIN_NO_1, ... GPIO_PIN_NO_15).
 * @param[in]	- Which edge to trigger on (Rising Edge[GPIO_MODE_IT_FT],
 * 				  Falling Edge[GPIO_MODE_IT_RT], or Rising and Falling Edge
 * 				  [GPIO_MODE_IT_RFT]).
 *
 * @return		- None.
 *
 * @note		- None.
 */
void GPIO_EnableInterrupts(GPIO_TypeDef *GPIOx, uint8_t pinNumber, uint8_t edgeTrigger)
{
	//Configure GPIOx port mode to input mode.
	GPIOx->MODER &= ~(0x3U << (2 * pinNumber));
	GPIOx->PUPDR |= (0x2U << (2 * pinNumber));

	//Enable clock access to SYSCFG and select PORTx for EXTIx.
	uint8_t pinNumDiv4, pinNumMod4, portCode;
	pinNumDiv4 = pinNumber / 4;
	pinNumMod4 = pinNumber % 4;
	portCode = GPIO_BaseAddressToCode(GPIOx);
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	SYSCFG->EXTICR[pinNumDiv4] |= (portCode << (pinNumMod4 * 4));

	//Unmask the EXTIx using Interrupt Mask Register (IMR).
	EXTI->IMR |= (1 << pinNumber);

	//Configure Rising Edge, Falling Edge, or both.
	if(edgeTrigger == GPIO_MODE_IT_FT)
	{//Configure for Falling Edge Trigger.
		EXTI->FTSR |= (1 << pinNumber);
		//Clear the corresponding RTSR bit.
		EXTI->RTSR &= ~(1 << pinNumber);
	}
	else if(edgeTrigger == GPIO_MODE_IT_RT)
	{//Configure for Rising Edge Trigger.
		EXTI->RTSR |= (1 << pinNumber);
		//Clear the corresponding FTSR bit.
		EXTI->FTSR &= ~(1 << pinNumber);
	}
	else if(edgeTrigger == GPIO_MODE_IT_RFT)
	{//Configure both FTSR and RTSR.
		EXTI->RTSR |= (1 << pinNumber);
		EXTI->FTSR |= (1 << pinNumber);
	}

	// Enable EXTI Line
	//IRQInterruptConfig(GPIO_PinNumberToExtiCode(pinNumber), ENABLE);
}

/*
 * @fn			- GPIO_DisableInterrupts
 *
 * @brief		- This function disables the EXTI interrupts for the GPIO pin
 * 				  specified in the first two arguments.
 *
 * @param[in]	- Base address of the GPIOx port register.
 * @param[in]	- GPIO pin number(GPIO_PIN_NO_0, GPIO_PIN_NO_1, ... GPIO_PIN_NO_15).
 *
 * @return		- None.
 *
 * @note		- None.
 */
void GPIO_DisableInterrupts(GPIO_TypeDef *GPIOx, uint8_t pinNumber)
{//TODO: Implement.

}

/*
 * @fn			- GPIO_CheckInterruptPending
 *
 * @brief		- This function checks the EXTI Pending Register for pending
 * 				  interrupts.
 *
 * @param[in]	- GPIO pin number(GPIO_PIN_NO_0, GPIO_PIN_NO_1, ... GPIO_PIN_NO_15).
 *
 * @return		- None.
 *
 * @note		- None.
 */
uint8_t GPIO_CheckInterruptPending(uint8_t pinNumber)
{
	return ((EXTI->PR >> pinNumber) & 0x1);
}


/***************** Private Helper Function Definitions *************************************/
static uint8_t GPIO_BaseAddressToCode(GPIO_TypeDef *GPIOx)
{
	if(GPIOx == GPIOA)
		return 0;
	else if(GPIOx == GPIOB)
		return 1;
	else if(GPIOx == GPIOC)
		return 2;
	else if(GPIOx == GPIOD)
		return 3;
	else if(GPIOx == GPIOE)
		return 4;
	else if(GPIOx == GPIOF)
		return 5;
	else if(GPIOx == GPIOG)
		return 6;
	else if(GPIOx == GPIOH)
		return 7;
	else if(GPIOx == GPIOI)
		return 8;
	else
		return 0;
}

/*static uint8_t GPIO_PinNumberToExtiCode(uint8_t pinNum)
{
	switch(pinNum)
	{
	case 0: return EXTI0_IRQn;
	case 1: return EXTI1_IRQn;
	case 2: return EXTI2_IRQn;
	case 3: return EXTI3_IRQn;
	case 4: return EXTI4_IRQn;
	case 5:
	case 6:
	case 7:
	case 8:
	case 9: return EXTI9_5_IRQn;
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15: return EXTI15_10_IRQn;
	}
	return 0;
}*/


/*********************IRQ Configuration and ISR handling***************************/

/* Weak function that can be implemented in user application. */
void __attribute__((weak)) GPIO_ApplicationEventCallBack(uint8_t pinNumber) {}


void EXTI0_IRQHandling(void)
{
	//Clear the EXTI PR register corresponding to the pin number.
	EXTI->PR |= (1 << GPIO_PIN_NO_0);// Writting a 1 clears the EXTI PR
	GPIO_ApplicationEventCallBack(GPIO_PIN_NO_0);
}

void EXTI1_IRQHandling(void)
{
	//Clear the EXTI PR register corresponding to the pin number.
	EXTI->PR |= (1 << GPIO_PIN_NO_1);// Writting a 1 clears the EXTI PR
	GPIO_ApplicationEventCallBack(GPIO_PIN_NO_1);
}

void EXTI2_IRQHandling(void)
{
	//Clear the EXTI PR register corresponding to the pin number.
	EXTI->PR |= (1 << GPIO_PIN_NO_2);// Writting a 1 clears the EXTI PR
	GPIO_ApplicationEventCallBack(GPIO_PIN_NO_2);
}

void EXTI3_IRQHandling(void)
{
	//Clear the EXTI PR register corresponding to the pin number.
	EXTI->PR |= (1 << GPIO_PIN_NO_3);// Writting a 1 clears the EXTI PR
	GPIO_ApplicationEventCallBack(GPIO_PIN_NO_3);
}

void EXTI4_IRQHandling(void)
{
	//Clear the EXTI PR register corresponding to the pin number.
	EXTI->PR |= (1 << GPIO_PIN_NO_4);// Writting a 1 clears the EXTI PR
	GPIO_ApplicationEventCallBack(GPIO_PIN_NO_4);
}

void EXTI9_5_IRQHandling(uint8_t pinNum)
{
	//Clear the EXTI PR register corresponding to the pin number.
	EXTI->PR |= (1 << pinNum);// Writting a 1 clears the EXTI PR
	GPIO_ApplicationEventCallBack(pinNum);
}

void EXTI15_10_IRQHandling(uint8_t pinNum)
{
	//Clear the EXTI PR register corresponding to the pin number.
	EXTI->PR |= (1 << pinNum);// Writting a 1 clears the EXTI PR
	GPIO_ApplicationEventCallBack(pinNum);
}


