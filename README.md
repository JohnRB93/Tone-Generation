# Tone-Generation
* The goal of this program is to use the STM32 microcontroller to simulate a keyboard instrument with buttons for keys and a joystick module for pitch bending.
* This program demonstrates the use of timer PWM output, multichannel ADC conversion with DMA, and EXTI interrupts to simulate a keyboard instrument with buttons, an 8 Ohm speaker, and a joystick module.
* Eight push buttons are connected to eight different GPIO pins of the microcontroller and will cause the speaker connected to the PWM output to play a specified note with the use of EXTI interrupts. The interrupt service routine of each pin connected to a push button will play a note by setting the appropiate frequency required for the note.
* The ADC will convert analog inputs from the joystick module and use those values to alter the frequency and/or pulse width of a currently playing note. The DMA is used in order to manage the transfer of data from two ADC channels from the ADC data register (ADC_DR) to specified locations in memory (in this case, elements of an array).
* For debugging purposes, a UART module is connected to pin PA2 to display analog measurements and frequencies.
