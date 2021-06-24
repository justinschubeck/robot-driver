/**
 * Configure the MSP432 clock to run at 48 MHz
 * @param none
 * @return none
 * @note  Since the crystal is used, the bus clock will be very accurate
 * @see Clock_GetFreq()
 * @brief  Initialize clock to 48 MHz
 */
void Clock_Init48MHz(void);
 
/**
 * Return the current bus clock frequency
 * @param none
 * @return frequency of the system clock in Hz
 * @note  In this module, the return result will be 3000000 or 48000000
 * @see Clock_Init48MHz()
 * @brief Returns current clock bus frequency in Hz
 */
uint32_t Clock_GetFreq(void);

/**
 * Simple delay function which delays about n milliseconds.
 * It is implemented with a nested for-loop and is very approximate.
 * @param  n is the number of msec to wait
 * @return none
 * @note This function assumes a 48 MHz clock.
 * This implementation is not very accurate.
 * To improve accuracy, you could tune this function
 * by adjusting the constant within the implementation
 * found in the <b>Clock.c</b> file.
 * For a more accurate time delay, you could use the SysTick module.
 * @brief  Software implementation of a busy-wait delay
 */
void Clock_Delay1ms(uint32_t n);

/**
 * Simple delay function which delays about n microseconds.
 * It is implemented with a nested for-loop and is very approximate.
 * @param  n is the number of usec to wait
 * @return none
 * @note This function assumes a 48 MHz clock.
 * This implementation is not very accurate.
 * To improve accuracy, you could tune this function
 * by adjusting the constant within the implementation
 * found in the <b>Clock.c</b> file.
 * For a more accurate time delay, you could use the SysTick module.
 * @brief  Software implementation of a busy-wait delay
 */
void Clock_Delay1us(uint32_t n);