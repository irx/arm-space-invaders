/*
 * SPI module handling for LED display driver
 * 2021 Maksymilian Mruszczak <u at one u x dot o r g>
 */

#define CS_MASK		(1 << 5)
#define DC_MASK		(1 << 7)
#define RES_MASK	(1 << 6)

void		spi_init(void);
void		spi_cs_high(void);
void		spi_cs_low(void);
void		spi_dc_high(void);
void		spi_dc_low(void);
void		spi_res_high(void);
void		spi_res_low(void);
void		spi_write(const uint8_t *);
