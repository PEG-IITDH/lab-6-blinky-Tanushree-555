#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"

/* SysTick memory-mapped registers */

#define STCTRL *((volatile long *) 0xE000E010)                                 // control and status
#define STRELOAD *((volatile long *) 0xE000E014)                                                          // reload value
#define STCURRENT *((volatile long *) 0xE000E018)                                          // current value

#define MASK_BITS   0x11                                                                //  mask bits for user switch 1 and 2
#define ENABLE      (1 << 0)                                                                 // bit 0 of CSR to enable the timer
#define CLK_INT      (1 << 2)                                                              // bit 2 of CSR to specify CPU clock
#define EN_INT       (1 << 1)                                                                               // enable systick interrupt

void SysTick_Handler(void)
       {
            GPIO_PORTF_DATA_R  = 0x00;                                                                    // turn off Blue LED
            GPIO_PORTF_ICR_R = MASK_BITS;                                                  // clear interrupt for port F
            GPIO_PORTF_IM_R |= MASK_BITS;                                   // unmask interrupts for sw1 and sw2
      }

void PORTF_Handler(void)
    {
    if(GPIO_PORTF_MIS_R & 0X01)
        {
        GPIO_PORTF_ICR_R = MASK_BITS;                                                       // clear interrupt for port F
        GPIO_PORTF_IM_R &= ~MASK_BITS;                                       // mask interrupts for sw1 and sw2
        GPIO_PORTF_DATA_R = 0x04;                                                                           // turn on Blue LED
        STRELOAD = 1*16000000;                                                                                   // for delay of 1 sec
        STCURRENT = 0;
        STCTRL |= (CLK_INT | ENABLE | EN_INT);                                      // set internal clock, enable the
                     //timer,enable interrupt
          }
    }

int main(void)
{
        SYSCTL_RCGC2_R |= 0x00000020;                                                        /* enable clock to GPIOF */
        GPIO_PORTF_LOCK_R = 0x4C4F434B;                                                  /* unlock commit register */
        GPIO_PORTF_CR_R = 0x01;                                                           /* make PORTF0 configurable */
        GPIO_PORTF_DEN_R = 0x1F;                                                                  /* set PORTF pins 4 pin */
        GPIO_PORTF_DIR_R = 0x0E;                                      /* set PORTF4 pin as input user switch pin */
        GPIO_PORTF_PUR_R = 0x11;                                                                    /* PORTF4 is pulled up */
        GPIO_PORTF_IM_R &= ~MASK_BITS;                                        // mask interrupts for sw1 and sw2
        GPIO_PORTF_IS_R &= ~MASK_BITS;                                                     // edge sensitive interrupts
        GPIO_PORTF_IEV_R  &= ~MASK_BITS;                                     // falling edge triggers the interrupt
        GPIO_PORTF_IBE_R &= ~MASK_BITS;                                   // Interrupt is controlled by GPIOIEV

        NVIC_PRI7_R = (NVIC_PRI7_R & 0xFF1FFFFF) | (2 << 21);         // 2 priority of interrupt 30(port F)
        NVIC_EN0_R |= (1 << 30);                                                                        // enable interrupt on port F
        GPIO_PORTF_ICR_R = MASK_BITS;                   // clears RIS and MIS for edge sensitive interrupt
        GPIO_PORTF_IM_R |= MASK_BITS;                                       // unmask interrupts for sw1 and sw2
        while(1)
        {
            ;                                                                                                                                               //wait
        }
}
