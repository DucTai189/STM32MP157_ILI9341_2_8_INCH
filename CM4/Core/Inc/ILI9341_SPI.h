#ifndef __ILI9341_SPI_H__
#define __ILI9341_SPI_H__
#include "stm32mp1xx_hal.h"

#define ILI9341_NOP               0x00u
#define ILI9341_SWRESET           0x01u
#define ILI9341_RDDID             0x04u
#define ILI9341_RDDST             0x09u
#define ILI9341_RDDPM             0x0Au
#define ILI9341_RDDMAD            0x0Bu
#define ILI9341_RDDCOL            0x0Cu
#define ILI9341_RDDIM             0x0Du
#define ILI9341_RDDSM             0x0Eu
#define ILI9341_RDDSDR            0x0Fu

// Power & Display Mode Control (0x10 - 0x21)
#define ILI9341_SLPIN             0x10u
#define ILI9341_SLPOUT            0x11u
#define ILI9341_PTLON             0x12u
#define ILI9341_NORON             0x13u
#define ILI9341_INVOFF            0x20u
#define ILI9341_INVON             0x21u

//Display Control & Window Setting (0x26 - 0x2E)
#define ILI9341_GAMSET            0x26u
#define ILI9341_DISPOFF           0x28u
#define ILI9341_DISPON            0x29u
#define ILI9341_CASET             0x2Au
#define ILI9341_PASET             0x2Bu
#define ILI9341_RAMWR             0x2Cu
#define ILI9341_CLSET             0x2Du
#define ILI9341_RAMRD             0x2Eu

//Area & Format Settings (0x30 - 0x3F)
#define ILI9341_PLAREA            0x30u
#define ILI9341_VSCRDEF           0x33u
#define ILI9341_TEOFF             0x34u
#define ILI9341_TEON              0x35u
#define ILI9341_MADCTL            0x36u
#define ILI9341_VSCRSADD          0x37u
#define ILI9341_IDMOFF            0x38u
#define ILI9341_IDMON             0x39u
#define ILI9341_PIXFMT            0x3Au
#define ILI9341_RAMWRC            0x3Cu
#define ILI9341_RAMRDC            0x3Eu

//Brightness & Identification (0x44 - 0xDC)
#define ILI9341_TESL              0x44u
#define ILI9341_GSL               0x45u
#define ILI9341_WRDB              0x51u
#define ILI9341_RDDB              0x52u
#define ILI9341_WRCD              0x53u
#define ILI9341_RDCD              0x54u
#define ILI9341_WRCABC            0x55u
#define ILI9341_RDCABC            0x56u
#define ILI9341_WRCABCMB          0x5Eu
#define ILI9341_RDCABCMB          0x5Fu
#define ILI9341_RDID1             0xDAu
#define ILI9341_RDID2             0xDBu
#define ILI9341_RDID3             0xDCh

#define ILI9341_SPI_HEIGHT  240u
#define ILI9341_SPI_WEIGHT 320u
#define ILI9341_SPI_MAXIMUM_COLUMN   (ILI9341_SPI_HEIGHT - 1u)
#define ILI9341_SPI_MAXIMUM_ROW   	 (ILI9341_SPI_WEIGHT - 1u)
#define ILI9341_MAXIMUN_TEXTPERLINE   (ILI9341_SPI_HEIGHT / 8) // based on the fornt 16x8

#define ILI9341_RESET_Pin GPIO_PIN_4
#define ILI9341_RESET_GPIO_Port GPIOF
#define ILI9341_DC_Pin GPIO_PIN_5
#define ILI9341_DC_GPIO_Port GPIOI
#define ILI9341_CS_Pin GPIO_PIN_3
#define ILI9341_CS_GPIO_Port GPIOF
#define SPI5_MOSI_Pin GPIO_PIN_9
#define SPI5_MOSI_GPIO_Port GPIOF
#define SPI5_SCK_Pin GPIO_PIN_7
#define SPI5_SCK_GPIO_Port GPIOF
#define SPI5_MISO_Pin GPIO_PIN_8
#define SPI5_MISO_GPIO_Port GPIOF

//COLOR define
#define   BLACK        0x0000                    
#define   NAVY         0x000F                    
#define   DGREEN       0x03E0                    
#define   DCYAN        0x03EF                   
#define   MAROON       0x7800                    
#define   PURPLE       0x780F                    
#define   OLIVE        0x7BE0                    
#define   LGRAY        0xC618                    
#define   DGRAY        0x7BEF                   
#define   BLUE         0x001F                    
#define   GREEN        0x07E0                 	 
#define   CYAN         0x07FF                    
#define   RED          0xF800                    
#define   MAGENTA      0xF81F                    
#define   YELLOW       0xFFE0                    
#define   WHITE        (uint16_t)0xFFFF
typedef enum
{
	FONT_12x6 = 12,
	FONT_16x8 = 16,
}font_size_st;
typedef struct
{
	char 		text[ILI9341_MAXIMUN_TEXTPERLINE];
	uint16_t 	background_color_u16;
	uint16_t 	textcolor_u16;
	uint8_t 	Font_Size_u16;
}ILI9341_character_st;

void ILI9341_Init(SPI_HandleTypeDef *hspi);
void ILI9341_FillScreen(SPI_HandleTypeDef *hspi, uint32_t color);
void ILI9341_SHOW_CHAR_X(uint8_t X,uint16_t Y,uint8_t chars, ILI9341_character_st character);
void ILI9341_SHOW_STRING_X(uint8_t X,uint16_t Y,const char *p, ILI9341_character_st character);

#endif
