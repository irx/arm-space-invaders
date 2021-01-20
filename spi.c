/*
 * SPI module handling for LED display driver
 * 2021 Maksymilian Mruszczak <u at one u x dot o r g>
 */

#include "MKL05Z4.h"
#include "spi.h"

void
spi_init(void)
{
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTB_MASK; /* enable clock */

	PORTA->PCR[5] |= PORT_PCR_MUX(1); /* CS */
	PORTB->PCR[0] |= PORT_PCR_MUX(3); /* CLK */
	PORTA->PCR[7] |= PORT_PCR_MUX(3); /* Master Output */
	PORTA->PCR[6] |= PORT_PCR_MUX(3); /* Slave Output; not used atm */

	PTA->PDDR |= (1<<5); /* set dir to output */
	PTA->PSOR |= (1<<5); /* set CS to high */

	SIM->SCGC4 |= SIM_SCGC4_SPI0_MASK; /* enable CLK for SPI0 */

	SPI0->C1 = SPI_C1_SPE_MASK | SPI_C1_MSTR_MASK; /* enable spi & set as master */
	SPI0->C1 &= ~(SPI_C1_CPOL_MASK | SPI_C1_SPIE_MASK | SPI_C1_CPHA_MASK); /* disable interrupt */
	SPI0->C2 &= ~(SPI_C2_SPMIE_MASK); /* 8 BIT | USING POLLING */
	SPI0->BR = 34; /* baud rate */

	PORTB->PCR[7] |= PORT_PCR_MUX(1); /* D/C */
	PORTB->PCR[6] |= PORT_PCR_MUX(1); /* RES */
	/* set dirs as output */
	PTB->PDDR |= (DC_MASK | RES_MASK);
	PTB->PSOR |= (DC_MASK | RES_MASK);
}

void
spi_cs_low(void)
{
	PTA->PCOR |= CS_MASK;
}

void
spi_cs_high(void)
{
	PTA->PSOR |= CS_MASK;
}

void
spi_dc_high(void)
{
	PTB->PSOR |= DC_MASK;
}

void
spi_dc_low(void)
{
	PTB->PCOR |= DC_MASK;
}

void
spi_res_high(void)
{
	PTB->PSOR |= RES_MASK;
}

void
spi_res_low(void)
{
	PTB->PCOR |= RES_MASK;
}

void
spi_write(uint8_t *data)
{
	/* send data through the SPI module */
	while ((SPI0->S & SPI_S_SPTEF_MASK) != SPI_S_SPTEF_MASK);
	SPI0->D = *data;
	while ((SPI0->S & SPI_S_SPRF_MASK) != SPI_S_SPRF_MASK);
	SPI0->D;
}
