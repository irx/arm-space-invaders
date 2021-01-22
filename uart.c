/**
 * 2021, Maksymilian Mruszczak <u at one u x dot o r g>
 *
 * UART communication for user input handling
 */

#include "MKL05Z4.h"
#include "uart.h"

enum Clock {
	CLK_DIS = 0x00,
	MCGFLLCLK = 0x01,
	OSCERCLK = 0x02,
	MCGIRCLK = 0x03
};

void		handle_input(char);


void
uart_init(void)
{
	SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;
	SIM->SOPT2 |= SIM_SOPT2_UART0SRC(MCGFLLCLK);
	PORTB->PCR[1] = PORT_PCR_MUX(2); /* Tx */
	PORTB->PCR[2] = PORT_PCR_MUX(2); /* Rx */

	UART0->C2 &= ~(UART0_C2_TE_MASK | UART0_C2_RE_MASK ); /* disable */
	UART0->BDH = 0x01;
	UART0->BDL = 0x11; /* for SBR=273(0x111) BR=9600 */
	UART0->C4 &= ~UART0_C4_OSR_MASK;
	UART0->C4 |= UART0_C4_OSR(15);
	UART0->C5 |= UART0_C5_BOTHEDGE_MASK; /* Both edges sampling */
	UART0->C2 |= UART0_C2_RIE_MASK; /* Enable IRQ from Rx */
	UART0->C2 |= (UART0_C2_TE_MASK | UART0_C2_RE_MASK); /* enable */

	NVIC_EnableIRQ(UART0_IRQn);
	NVIC_ClearPendingIRQ(UART0_IRQn);
}

void
UART0_IRQHandler(void)
{
	char c;
	if (UART0->S1 & UART0_S1_RDRF_MASK) {
		inp = UART0->D;
		handle_input(c);
		NVIC_EnableIRQ(UART0_IRQn);
	}
}
