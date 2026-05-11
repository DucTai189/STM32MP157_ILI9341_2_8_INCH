#include "ILI9341_SPI.h"
#include "TV_FONT.h"

static uint16_t RGB565(uint32_t color);
static void ILI9341_Write_Cmd(SPI_HandleTypeDef *hspi, uint8_t cmd) ;
static void ILI9341_Write_Data(SPI_HandleTypeDef *hspi, uint8_t data) ;
static void ILI9341_Write_Data16(SPI_HandleTypeDef *hspi, uint16_t data);
static void ILI9341_WRITE_REG(uint8_t cmd, uint8_t data);
static void ILI9341_WRITE_REG_32(uint8_t cmd, uint16_t data1, uint16_t data2);
static  SPI_HandleTypeDef hspi_ili9341;

static void ILI9341_Write_Cmd(SPI_HandleTypeDef *hspi, uint8_t cmd) 
{
    HAL_GPIO_WritePin(ILI9341_DC_GPIO_Port, ILI9341_DC_Pin, GPIO_PIN_RESET); // DC Low for Command
    HAL_GPIO_WritePin(ILI9341_CS_GPIO_Port, ILI9341_CS_Pin, GPIO_PIN_RESET); // CS Low
    HAL_SPI_Transmit(hspi, &cmd, 1, 100);  								 // timeout 100ticks counter of systick
    HAL_GPIO_WritePin(ILI9341_CS_GPIO_Port, ILI9341_CS_Pin, GPIO_PIN_SET);   // CS High
}
static void ILI9341_Write_Data(SPI_HandleTypeDef *hspi, uint8_t data) 
{
    HAL_GPIO_WritePin(ILI9341_DC_GPIO_Port, ILI9341_DC_Pin, GPIO_PIN_SET);   // DC High for Data
    HAL_GPIO_WritePin(ILI9341_CS_GPIO_Port, ILI9341_CS_Pin, GPIO_PIN_RESET); // CS Low
    HAL_SPI_Transmit(hspi, &data, 1, 100);								 // timeout 100ticks counter of systick
    HAL_GPIO_WritePin(ILI9341_CS_GPIO_Port, ILI9341_CS_Pin, GPIO_PIN_SET);   // CS High
}
static void ILI9341_Write_Data16(SPI_HandleTypeDef *hspi, uint16_t data)
{
    uint8_t buffer[2];
    buffer[0] = data >> 8;   // High byte first (MSB)
    buffer[1] = data & 0xFF; // Low byte

    HAL_GPIO_WritePin(ILI9341_DC_GPIO_Port, ILI9341_DC_Pin, GPIO_PIN_SET);   // Data mode
    HAL_GPIO_WritePin(ILI9341_CS_GPIO_Port, ILI9341_CS_Pin, GPIO_PIN_RESET); // CS low
    HAL_SPI_Transmit(hspi, buffer, 2, 100);								// timeout 100ticks counter of systick
    HAL_GPIO_WritePin(ILI9341_CS_GPIO_Port, ILI9341_CS_Pin, GPIO_PIN_SET);   // CS high
}

void ILI9341_Init(SPI_HandleTypeDef *hspi) 
{
    // assign the spi chanel
    hspi_ili9341 = *hspi;

    HAL_GPIO_WritePin(ILI9341_RESET_GPIO_Port, ILI9341_RESET_Pin, GPIO_PIN_RESET);
    HAL_Delay(50);                          // 50ms minimum
    HAL_GPIO_WritePin(ILI9341_RESET_GPIO_Port, ILI9341_RESET_Pin, GPIO_PIN_SET);
    HAL_Delay(150);                         // 120ms minimum after power-up

	// Software reset
	ILI9341_Write_Cmd(hspi, ILI9341_SWRESET);    // Software reset
	HAL_Delay(150);                         // Wait 120ms minimum

	// Exit sleep
	ILI9341_Write_Cmd(hspi, ILI9341_SLPOUT);     // Sleep out
	HAL_Delay(150);                         // Wait 120ms minimum

	// Memory access control (set BGR and orientation)
    ILI9341_WRITE_REG(ILI9341_MADCTL, 0x48);  // MX=1, BGR=1 (for correct colors)
           

	// Pixel format (16-bit color)
    ILI9341_WRITE_REG(ILI9341_PIXFMT, 0x55); // 16-bit/pixel (RGB565) . 0x55 is 16-bit, 0x66 is 18bit


	// Display on
	ILI9341_Write_Cmd(hspi, ILI9341_DISPON);     // Display ON
	HAL_Delay(50);
}

void ILI9341_FillScreen(SPI_HandleTypeDef *hspi, uint32_t color)
{
	uint16_t color_16bit_u16;

	color_16bit_u16 = RGB565(color);
    ILI9341_WRITE_REG_32(ILI9341_CASET, 0, ILI9341_SPI_MAXIMUM_COLUMN); // Column address set. 0 is start column

    ILI9341_WRITE_REG_32(ILI9341_PASET, 0, ILI9341_SPI_MAXIMUM_ROW); // Row address set. 0 is start row

    ILI9341_Write_Cmd(hspi, ILI9341_RAMWR); // Memory write

    // Fill screen with color
    for(uint32_t i = 0; i < ILI9341_SPI_HEIGHT * ILI9341_SPI_WEIGHT; i++)
    {
        ILI9341_Write_Data16(hspi,color_16bit_u16);
    }
}
// 16-bit color converter (RGB565)
static uint16_t RGB565(uint32_t color)
{
	uint16_t color_16bit_u16;
	color_16bit_u16 = (((color & 0xFC00)>>8) << 3) | ((color & 0xF8) >>3)  ;
	color_16bit_u16 = (((color & 0xF80000)>>16) << 8) | color_16bit_u16;

    return color_16bit_u16;
}
void ILI9341_SHOW_STRING_X(uint8_t X,uint16_t Y, const char *p, ILI9341_character_st character)
{         
    while(*p!='\0')
    {       
        if(X > ILI9341_SPI_MAXIMUM_COLUMN)
        {  
            X = character.Font_Size_u16;
        }
        if(Y > ILI9341_SPI_MAXIMUM_ROW)
        {
            Y = X = 0u;
            ILI9341_FillScreen(&hspi_ili9341,WHITE);
        }
        ILI9341_SHOW_CHAR_X(X,Y,*p,character);
        X += character.Font_Size_u16/2;
        p++;
    }  
}

/**********************************************************

***********************************************************/
void ILI9341_CLEAR_STRING_X(uint8_t X,uint16_t Y, uint8_t *p, ILI9341_character_st character)
{         
    while(*p!='\0')
    {       
        if(X > ILI9341_SPI_MAXIMUM_COLUMN)
        {
            X = character.Font_Size_u16;
        }
        if(Y > ILI9341_SPI_MAXIMUM_ROW)
        {
            Y = X = 0u;
            ILI9341_FillScreen(&hspi_ili9341,WHITE);
        }
        ILI9341_SHOW_CHAR_X(X,Y,' ',character);
        X+=8;
        p++;
    }  
}


static void ILI9341_SET_DISPLAY_WINDOW(uint8_t X, uint16_t Y, uint8_t X_END, uint16_t Y_END)
{
    ILI9341_Write_Cmd(&hspi_ili9341, ILI9341_CASET); // Column address set
    ILI9341_Write_Data16(&hspi_ili9341,X);     // Start column
    ILI9341_Write_Data16(&hspi_ili9341, X + X_END -1);   // End column 

    ILI9341_Write_Cmd(&hspi_ili9341, ILI9341_PASET); // Page address set
    ILI9341_Write_Data16(&hspi_ili9341,Y);     // Start row
    ILI9341_Write_Data16(&hspi_ili9341, Y+ Y_END -1);   // End row
}

/**********************************************************

***********************************************************/
void ILI9341_SHOW_CHAR_X(uint8_t X,uint16_t Y,uint8_t chars, ILI9341_character_st character)
{
	uint16_t temp;
    uint8_t pos,t;
    uint8_t char_width = character.Font_Size_u16 / 2;   // 8 for 16pt font, 6 for 12pt font
    uint8_t char_height = character.Font_Size_u16;      // 16 for 16pt font, 12 for 12pt font

    if( X > ILI9341_SPI_MAXIMUM_COLUMN || Y > ILI9341_SPI_MAXIMUM_ROW) 
    {
        return;
    }
    
    ILI9341_SET_DISPLAY_WINDOW(X,Y,char_width,char_height);
    ILI9341_Write_Cmd(&hspi_ili9341, ILI9341_RAMWR); // Memory write


    for(pos=0; pos < char_height; pos++) // For each row in character
    {
        if(character.Font_Size_u16 == 12)	
            temp=ASCII_1206[chars-0x20][pos];
        else if(character.Font_Size_u16 == 16)	
            temp=ASCII_1608[chars-0x20][pos];
        else 					
            temp=ASCII_1608[chars-0x20][pos];

        for(t=0; t<char_width; t++)	// For each column in character
        {
            if(temp & (0x80 >> t))
            {
                ILI9341_Write_Data16(&hspi_ili9341, character.textcolor_u16);
            }

            else
            {
                ILI9341_Write_Data16(&hspi_ili9341, character.background_color_u16);
            }
        }
    }


}
static void ILI9341_WRITE_REG_32(uint8_t cmd, uint16_t data1, uint16_t data2)
{
    ILI9341_Write_Cmd(&hspi_ili9341,cmd);
    ILI9341_Write_Data16(&hspi_ili9341, data1);
    ILI9341_Write_Data16(&hspi_ili9341, data2);
}

static void ILI9341_WRITE_REG(uint8_t cmd, uint8_t data)
{
    ILI9341_Write_Cmd(&hspi_ili9341,cmd);
    ILI9341_Write_Data(&hspi_ili9341, data);
}
