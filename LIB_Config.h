/**
  ******************************************************************************
  * @file    LIB_Config.h
  * @author  Waveshare Team
  * @version
  * @date    13-October-2014
  * @brief     This file provides configurations for low layer hardware libraries.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, WAVESHARE SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  ******************************************************************************
  *
  * Modified by Maksymilian Mruszczak 2021
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _USE_LIB_CONFIG_H_
#define _USE_LIB_CONFIG_H_
//Macro Definition

/* Includes ------------------------------------------------------------------*/
#include "MacroAndConst.h"
#include "spi.h"
#include "SSD1331.h"
#include "Fonts.h"

#define __SSD1331_RES_SET()           spi_res_high()
#define __SSD1331_RES_CLR()           spi_res_low()

#define __SSD1331_DC_SET()            spi_dc_high()
#define __SSD1331_DC_CLR()            spi_dc_low()

#define __SSD1331_CS_SET()            spi_cs_high()
#define __SSD1331_CS_CLR()            spi_cs_low()


#define __SSD1331_WRITE_BYTE(__DATA)  spi_write(__DATA)

/*------------------------------------------------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */

#endif

/*-------------------------------END OF FILE-------------------------------*/
