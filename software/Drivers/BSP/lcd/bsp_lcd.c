/**
  ******************************************************************************
  * @file    bsp_sdram.c
  * @author  fire
  * @version V1.0
  * @date    2016-xx-xx
  * @brief   LCD??????,??ARGB888/RGB888/RGB565/ARGB4444/ARGB1555 (??????)
  ******************************************************************************
  * @attention
  *
  * ????:??  STM32 F767 ???  
  * ??    :http://www.firebbs.cn
  * ??    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
/* Includes ------------------------------------------------------------------*/
#include "bsp_lcd.h"
#include "font24.c"
#include "font20.c"
#include "font16.c"
#include "font12.c"
#include "font8.c"
#include "bsp_qspi_flash.h"

#define POLY_X(Z)              ((int32_t)((Points + Z)->X))
#define POLY_Y(Z)              ((int32_t)((Points + Z)->Y)) 

#define ABS(X)  ((X) > 0 ? (X) : -(X))

static LTDC_HandleTypeDef  Ltdc_Handler;
static DMA2D_HandleTypeDef Dma2d_Handler;

/* Default LCD configuration with LCD Layer 1 */
static uint32_t            ActiveLayer = 0;
static LCD_DrawPropTypeDef DrawProp[MAX_LAYER_NUMBER];
/**
 * @brief  Initializes the LCD.
 * @param  None
 * @retval None
 */


/*¸ù¾ÝÒº¾§Êý¾ÝÊÖ²áµÄ²ÎÊýÅäÖÃ*/
#define HBP  140		//HSYNCºóµÄÎÞÐ§ÏñËØ
#define VBP  20		//VSYNCºóµÄÎÞÐ§ÐÐÊý

#define HSW   20		//HSYNC¿í¶È
#define VSW   3		//VSYNC¿í¶È

#define HFP  160		//HSYNCÇ°µÄÎÞÐ§ÏñËØ
#define VFP  12		//VSYNCÇ°µÄÎÞÐ§ÐÐÊý

static void DrawChar(uint16_t Xpos, uint16_t Ypos, const uint8_t *c);
static void FillTriangle(uint16_t x1, uint16_t x2, uint16_t x3, uint16_t y1, uint16_t y2, uint16_t y3);
static void LL_FillBuffer(uint32_t LayerIndex, void *pDst, uint32_t xSize, uint32_t ySize, uint32_t OffLine, uint32_t ColorIndex);
static void LL_ConvertLineToARGB8888(void * pSrc, void *pDst, uint32_t xSize, uint32_t ColorMode);
 /**
  * @brief  ?????LCD?IO
  * @param  ?
  * @retval ?
  */
static void LCD_GPIO_Config(void)
{ 
	GPIO_InitTypeDef GPIO_InitStruct;
  
  /* ??LCD???????? */
                          //?????
  LTDC_R0_GPIO_CLK_ENABLE();LTDC_R1_GPIO_CLK_ENABLE();LTDC_R2_GPIO_CLK_ENABLE();\
  LTDC_R3_GPIO_CLK_ENABLE();LTDC_R4_GPIO_CLK_ENABLE();LTDC_R5_GPIO_CLK_ENABLE();\
  LTDC_R6_GPIO_CLK_ENABLE();LTDC_R7_GPIO_CLK_ENABLE();LTDC_G0_GPIO_CLK_ENABLE();\
  LTDC_G1_GPIO_CLK_ENABLE();LTDC_G2_GPIO_CLK_ENABLE();LTDC_G3_GPIO_CLK_ENABLE();\
  LTDC_G3_GPIO_CLK_ENABLE();LTDC_G5_GPIO_CLK_ENABLE();LTDC_G6_GPIO_CLK_ENABLE();\
  LTDC_G7_GPIO_CLK_ENABLE();LTDC_B0_GPIO_CLK_ENABLE();LTDC_B1_GPIO_CLK_ENABLE();\
  LTDC_B2_GPIO_CLK_ENABLE();LTDC_B3_GPIO_CLK_ENABLE();LTDC_B4_GPIO_CLK_ENABLE();\
  LTDC_B5_GPIO_CLK_ENABLE();LTDC_B6_GPIO_CLK_ENABLE();LTDC_B7_GPIO_CLK_ENABLE();\
  LTDC_CLK_GPIO_CLK_ENABLE();LTDC_HSYNC_GPIO_CLK_ENABLE();LTDC_VSYNC_GPIO_CLK_ENABLE();\
  LTDC_DE_GPIO_CLK_ENABLE();LTDC_DISP_GPIO_CLK_ENABLE();LTDC_BL_GPIO_CLK_ENABLE();
/* GPIO?? */

 /* ????? */                        
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Mode  = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull  = GPIO_PULLUP;
  
  GPIO_InitStruct.Pin =   LTDC_R0_GPIO_PIN; 
  GPIO_InitStruct.Alternate = LTDC_R0_AF;
  HAL_GPIO_Init(LTDC_R0_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin =   LTDC_R1_GPIO_PIN; 
  GPIO_InitStruct.Alternate = LTDC_R1_AF;
  HAL_GPIO_Init(LTDC_R1_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_R2_GPIO_PIN; 
  GPIO_InitStruct.Alternate = LTDC_R2_AF;
  HAL_GPIO_Init(LTDC_R2_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_R3_GPIO_PIN; 
  GPIO_InitStruct.Alternate = LTDC_R3_AF;
  HAL_GPIO_Init(LTDC_R3_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_R4_GPIO_PIN; 
  GPIO_InitStruct.Alternate = LTDC_R4_AF;
  HAL_GPIO_Init(LTDC_R4_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_R5_GPIO_PIN; 
  GPIO_InitStruct.Alternate = LTDC_R5_AF;
  HAL_GPIO_Init(LTDC_R5_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_R6_GPIO_PIN; 
  GPIO_InitStruct.Alternate = LTDC_R6_AF;
  HAL_GPIO_Init(LTDC_R6_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_R7_GPIO_PIN; 
  GPIO_InitStruct.Alternate = LTDC_R7_AF;
  HAL_GPIO_Init(LTDC_R7_GPIO_PORT, &GPIO_InitStruct);
  
  //?????
  GPIO_InitStruct.Pin =   LTDC_G0_GPIO_PIN; 
  GPIO_InitStruct.Alternate = LTDC_G0_AF;
  HAL_GPIO_Init(LTDC_G0_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin =   LTDC_G1_GPIO_PIN; 
  GPIO_InitStruct.Alternate = LTDC_G1_AF;
  HAL_GPIO_Init(LTDC_G1_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_G2_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_G2_AF;
  HAL_GPIO_Init(LTDC_G2_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_G3_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_G3_AF;
  HAL_GPIO_Init(LTDC_G3_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_G4_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_G4_AF;
  HAL_GPIO_Init(LTDC_G4_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_G5_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_G5_AF;
  HAL_GPIO_Init(LTDC_G5_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_G6_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_G6_AF;
  HAL_GPIO_Init(LTDC_G6_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_G7_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_G7_AF;
  HAL_GPIO_Init(LTDC_G7_GPIO_PORT, &GPIO_InitStruct);
  
  //?????
  GPIO_InitStruct.Pin =   LTDC_B0_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_B0_AF;
  HAL_GPIO_Init(LTDC_B0_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin =   LTDC_B1_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_B1_AF;
  HAL_GPIO_Init(LTDC_B1_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_B2_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_B2_AF;
  HAL_GPIO_Init(LTDC_B2_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_B3_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_B3_AF;
  HAL_GPIO_Init(LTDC_B3_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_B4_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_B4_AF;
  HAL_GPIO_Init(LTDC_B4_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_B5_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_B5_AF;
  HAL_GPIO_Init(LTDC_B5_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_B6_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_B6_AF;
  HAL_GPIO_Init(LTDC_B6_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_B7_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_B7_AF;
  HAL_GPIO_Init(LTDC_B7_GPIO_PORT, &GPIO_InitStruct);
  
  //?????
  GPIO_InitStruct.Pin = LTDC_CLK_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_CLK_AF;
  HAL_GPIO_Init(LTDC_CLK_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin = LTDC_HSYNC_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_HSYNC_AF;
  HAL_GPIO_Init(LTDC_HSYNC_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin = LTDC_VSYNC_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_VSYNC_AF;
  HAL_GPIO_Init(LTDC_VSYNC_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin = LTDC_DE_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_DE_AF;
  HAL_GPIO_Init(LTDC_DE_GPIO_PORT, &GPIO_InitStruct);
  
  //??BL ???????DISP
  GPIO_InitStruct.Pin = LTDC_DISP_GPIO_PIN;                             
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  
  HAL_GPIO_Init(LTDC_DISP_GPIO_PORT, &GPIO_InitStruct);
  
  
  GPIO_InitStruct.Pin = LTDC_BL_GPIO_PIN; 
  HAL_GPIO_Init(LTDC_BL_GPIO_PORT, &GPIO_InitStruct);
  
}

void LCD_Init(void)
{
    RCC_PeriphCLKInitTypeDef  periph_clk_init_struct;  
    /* ??LTDC?? */
    __HAL_RCC_LTDC_CLK_ENABLE();
    /* ??DMA2D?? */
    __HAL_RCC_DMA2D_CLK_ENABLE();
    /* ???LCD?? */
    LCD_GPIO_Config();
    /* ???SDRAM ??LCD ??*/
    SDRAM_Init();
    /* ??LTDC?? */
    Ltdc_Handler.Instance = LTDC;  
    /* ?????????(HSW-1) */
    Ltdc_Handler.Init.HorizontalSync =HSW-1;
    /* ??????????(VSW-1) */
    Ltdc_Handler.Init.VerticalSync = VSW-1;
    /* ??(HSW+HBP-1) */
    Ltdc_Handler.Init.AccumulatedHBP = HSW+HBP-1;
    /* ??(VSW+VBP-1) */
    Ltdc_Handler.Init.AccumulatedVBP = VSW+VBP-1;
    /* ??(HSW+HBP+??????-1) */
    Ltdc_Handler.Init.AccumulatedActiveW = HSW+HBP+LCD_PIXEL_WIDTH-1;
    /* ??(VSW+VBP+??????-1) */
    Ltdc_Handler.Init.AccumulatedActiveH = VSW+VBP+LCD_PIXEL_HEIGHT-1;
    /* ?????(HSW+HBP+??????+HFP-1) */
    Ltdc_Handler.Init.TotalWidth =HSW+ HBP+LCD_PIXEL_WIDTH + HFP-1; 
    /* ?????(VSW+VBP+??????+VFP-1) */
    Ltdc_Handler.Init.TotalHeigh =VSW+ VBP+LCD_PIXEL_HEIGHT + VFP-1;
    /* ??????? */
    /* PLLSAI_VCO Input = HSE_VALUE/PLL_M = 1 Mhz */
    /* PLLSAI_VCO Output = PLLSAI_VCO Input * PLLSAIN = 192 Mhz */
    /* PLLLCDCLK = PLLSAI_VCO Output/PLLSAIR = 192/5 = 38.4 Mhz */
    /* LTDC clock frequency = PLLLCDCLK / LTDC_PLLSAI_DIVR_4 = 38.4/4 = 9.6Mhz */
    periph_clk_init_struct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
    periph_clk_init_struct.PLLSAI.PLLSAIN = 192;
    periph_clk_init_struct.PLLSAI.PLLSAIR = 5;
    periph_clk_init_struct.PLLSAIDivR = RCC_PLLSAIDIVR_4;
    HAL_RCCEx_PeriphCLKConfig(&periph_clk_init_struct);
    /* ???LCD???????? */
    Ltdc_Handler.LayerCfg->ImageWidth  = LCD_PIXEL_WIDTH;
    Ltdc_Handler.LayerCfg->ImageHeight = LCD_PIXEL_HEIGHT;
    /* ??LCD??????,???? */
    Ltdc_Handler.Init.Backcolor.Red = 0;
    Ltdc_Handler.Init.Backcolor.Green = 0;
    Ltdc_Handler.Init.Backcolor.Blue = 0;
    /* ???? */
    /* ????????,????? */
    Ltdc_Handler.Init.HSPolarity = LTDC_HSPOLARITY_AL;
    /* ????????,????? */
    Ltdc_Handler.Init.VSPolarity = LTDC_VSPOLARITY_AL;
    /* ?????????,????? */
    Ltdc_Handler.Init.DEPolarity = LTDC_DEPOLARITY_AL;
    /* ??????????,????? */
    Ltdc_Handler.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
    HAL_LTDC_Init(&Ltdc_Handler);
    /* ????? */
    LCD_SetFont(&LCD_DEFAULT_FONT);
}

/**
  * @brief  ??LCD?????X????
  * @retval X????
  */
uint32_t LCD_GetXSize(void)
{
  return Ltdc_Handler.LayerCfg[ActiveLayer].ImageWidth;
}

/**
  * @brief  ??LCD?????Y????
  * @retval Y????
  */
uint32_t LCD_GetYSize(void)
{
  return Ltdc_Handler.LayerCfg[ActiveLayer].ImageHeight;
}

/**
  * @brief  ??LCD?????Y????
  * @param  imageWidthPixels:????????
  * @retval ?
  */
void LCD_SetXSize(uint32_t imageWidthPixels)
{
  Ltdc_Handler.LayerCfg[ActiveLayer].ImageWidth = imageWidthPixels;
}

/**
  * @brief  ??LCD?????Y????
  * @param  imageHeightPixels:????????
  * @retval None
  */
void LCD_SetYSize(uint32_t imageHeightPixels)
{
  Ltdc_Handler.LayerCfg[ActiveLayer].ImageHeight = imageHeightPixels;
}

/**
  * @brief  ???LCD? 
  * @param  LayerIndex:  ???(?1)?????(?0)
  * @param  FB_Address:  ?????????
  * @param  PixelFormat: ??????
  * @retval ?
  */
void LCD_LayerInit(uint16_t LayerIndex, uint32_t FB_Address, uint32_t PixelFormat)
{     
  LTDC_LayerCfgTypeDef  layer_cfg;

  /* ???? */
  layer_cfg.WindowX0        = 0;
  layer_cfg.WindowX1        = LCD_GetXSize();
  
  layer_cfg.WindowY0        = 0;
  layer_cfg.WindowY1        = LCD_GetYSize();
  
  layer_cfg.FBStartAdress   = FB_Address;
  layer_cfg.PixelFormat     = PixelFormat;
  
  layer_cfg.Alpha           = 255;
  layer_cfg.Alpha0          = 0;
  layer_cfg.Backcolor.Red   = 0;
  layer_cfg.Backcolor.Blue  = 0;
  layer_cfg.Backcolor.Green = 0;
  
  layer_cfg.ImageWidth      = LCD_GetXSize();
  layer_cfg.ImageHeight     = LCD_GetYSize();
  
  layer_cfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
  layer_cfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
  
  HAL_LTDC_ConfigLayer(&Ltdc_Handler, &layer_cfg, LayerIndex);

  DrawProp[LayerIndex].BackColor = LCD_COLOR_WHITE;
  DrawProp[LayerIndex].TextColor = LCD_COLOR_BLACK;
  DrawProp[LayerIndex].pFont     = &LCD_DEFAULT_FONT;
  
  __HAL_LTDC_RELOAD_CONFIG(&Ltdc_Handler);
}
/**
  * @brief  ??LCD?
  * @param  LayerIndex: ???(?1)?????(?0)
  * @retval ?
  */
void LCD_SelectLayer(uint32_t LayerIndex)
{
  ActiveLayer = LayerIndex;
} 

/**
  * @brief  ??LCD?????
  * @param  LayerIndex: ???(?1)?????(?0)
  * @param  State: ??????
  * @retval ?
  */
void LCD_SetLayerVisible(uint32_t LayerIndex, FunctionalState State)
{
  if(State == ENABLE)
  {
    __HAL_LTDC_LAYER_ENABLE(&Ltdc_Handler, LayerIndex);
  }
  else
  {
    __HAL_LTDC_LAYER_DISABLE(&Ltdc_Handler, LayerIndex);
  }
  __HAL_LTDC_RELOAD_CONFIG(&Ltdc_Handler);
} 

/**
  * @brief  ??LCD??????
  * @param  LayerIndex: ???(?1)?????(?0)
  * @param  Transparency: ???,??(0—255)0?????
  * @retval ?
  */
void LCD_SetTransparency(uint32_t LayerIndex, uint8_t Transparency)
{    
  HAL_LTDC_SetAlpha(&Ltdc_Handler, Transparency, LayerIndex);
}

/**
  * @brief  ??LCD??????? 
  * @param  LayerIndex: ???(?1)?????(?0)
  * @param  Address: LCD???????     
  * @retval ?
  */
void LCD_SetLayerAddress(uint32_t LayerIndex, uint32_t Address)
{
  HAL_LTDC_SetAddress(&Ltdc_Handler, Address, LayerIndex);
}

/**
  * @brief  ??????
  * @param  LayerIndex: ???(?1)?????(?0)
  * @param  Xpos: LCD 	X?????
  * @param  Ypos: LCD 	Y?????
  * @param  Width: LCD  ????
  * @param  Height: LCD ????  
  * @retval None
  */
void LCD_SetLayerWindow(uint16_t LayerIndex, uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
  /* ???????? */
  HAL_LTDC_SetWindowSize(&Ltdc_Handler, Width, Height, LayerIndex);
  
  /* ??????????? */
  HAL_LTDC_SetWindowPosition(&Ltdc_Handler, Xpos, Ypos, LayerIndex); 
}


/**
  * @brief  ??LCD???????
  * @param  Color: ????
  * @retval ?
  */
void LCD_SetTextColor(uint32_t Color)
{
  DrawProp[ActiveLayer].TextColor = Color;
}

/**
  * @brief  ??LCD???????
  * @retval ????
  */
uint32_t LCD_GetTextColor(void)
{
  return DrawProp[ActiveLayer].TextColor;
}

/**
  * @brief  ??LCD??????????
  * @param  Color: ??????
  * @retval ?
  */
void LCD_SetBackColor(uint32_t Color)
{
  DrawProp[ActiveLayer].BackColor = Color;
}

/**
  * @brief  ??LCD??????????
  * @retval ??????
  */
uint32_t LCD_GetBackColor(void)
{
  return DrawProp[ActiveLayer].BackColor;
}

/**
 * @brief  ??LCD???????????
 * @param  TextColor: ??????
 * @param  BackColor: ??????
 * @retval ?
 */
void LCD_SetColors(uint32_t TextColor, uint32_t BackColor)
{
     LCD_SetTextColor (TextColor);
     LCD_SetBackColor (BackColor);
}
/**
  * @brief  ??LCD????????
  * @param  fonts: ????
  * @retval None
  */
void LCD_SetFont(sFONT *fonts)
{
  DrawProp[ActiveLayer].pFont = fonts;
}

/**
  * @brief  ??LCD????????
  * @retval ????
  */
sFONT *LCD_GetFont(void)
{
  return DrawProp[ActiveLayer].pFont;
}

/**
  * @brief  ?LCD????
  * @param  Xpos: X ?????
  * @param  Ypos: Y ?????
  * @retval RGB???
  */
uint32_t LCD_ReadPixel(uint16_t Xpos, uint16_t Ypos)
{
  uint32_t ret = 0;
  
  if(Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_ARGB8888)
  {
    /* ?SDRAM????????? */
    ret = *(__IO uint32_t*) (Ltdc_Handler.LayerCfg[ActiveLayer].FBStartAdress + (2*(Ypos*LCD_GetXSize() + Xpos)));
  }
  else if(Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB888)
  {
    /* ?SDRAM????????? */
    ret  = (*(__IO uint8_t*) (Ltdc_Handler.LayerCfg[ActiveLayer].FBStartAdress + (3*(Ypos*LCD_GetXSize() + Xpos))+2) & 0x00FFFFFF);
	ret |= (*(__IO uint8_t*) (Ltdc_Handler.LayerCfg[ActiveLayer].FBStartAdress + (3*(Ypos*LCD_GetXSize() + Xpos))+1) & 0x00FFFFFF);
	ret |= (*(__IO uint8_t*) (Ltdc_Handler.LayerCfg[ActiveLayer].FBStartAdress + (3*(Ypos*LCD_GetXSize() + Xpos))) & 0x00FFFFFF);
  }
  else if((Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB565) || \
          (Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_ARGB4444) || \
          (Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_AL88))  
  {
    /* ?SDRAM????????? */
    ret = *(__IO uint16_t*) (Ltdc_Handler.LayerCfg[ActiveLayer].FBStartAdress + (2*(Ypos*LCD_GetXSize() + Xpos)));    
  }
  else
  {
    /* ?SDRAM????????? */
    ret = *(__IO uint8_t*) (Ltdc_Handler.LayerCfg[ActiveLayer].FBStartAdress + (2*(Ypos*LCD_GetXSize() + Xpos)));    
  }
  
  return ret;
}

/**
  * @brief  LCD?????
  * @param  Color: ????
  * @retval None
  */
void LCD_Clear(uint32_t Color)
{ 
  /* ?? */ 
  LL_FillBuffer(ActiveLayer, (uint32_t *)(Ltdc_Handler.LayerCfg[ActiveLayer].FBStartAdress), LCD_GetXSize(), LCD_GetYSize(), 0, Color);
}

/**
  * @brief  ????
  * @param  Line: ?
  * @retval None
  */
void LCD_ClearLine(uint32_t Line)
{
  uint32_t color_backup = DrawProp[ActiveLayer].TextColor;
  DrawProp[ActiveLayer].TextColor = DrawProp[ActiveLayer].BackColor;
  
  /* ???????????????? */
  LCD_FillRect(0, (Line * DrawProp[ActiveLayer].pFont->Height), LCD_GetXSize(), DrawProp[ActiveLayer].pFont->Height);
  
  DrawProp[ActiveLayer].TextColor = color_backup;
  LCD_SetTextColor(DrawProp[ActiveLayer].TextColor);  
}

/**
  * @brief  ??????
  * @param  Xpos: X?????
  * @param  Ypos: Y?????
  * @param  Ascii: ?? ascii ?,??( 0x20 —0x7E )
  * @retval ?
  */
void LCD_DisplayChar(uint16_t Xpos, uint16_t Ypos, uint8_t Ascii)
{
    DrawChar(Xpos, Ypos, &DrawProp[ActiveLayer].pFont->table[(Ascii-' ') *\
    DrawProp[ActiveLayer].pFont->Height * ((DrawProp[ActiveLayer].pFont->Width + 7) / 8)]);
}

/**
  * @brief  ?????
  * @param  Xpos: X?????
  * @param  Ypos: Y????? 
  * @param  Text: ?????
  * @param  Mode: ??????,???CENTER_MODE?RIGHT_MODE?LEFT_MODE
  * @retval None
  */
void LCD_DisplayStringAt(uint16_t Xpos, uint16_t Ypos, uint8_t *Text, Text_AlignModeTypdef Mode)
{
  uint16_t ref_column = 1, i = 0;
  uint32_t size = 0, xsize = 0; 
  uint8_t  *ptr = Text;
  
  /* ??????? */
  while (*ptr++) size ++ ;
  
  /* ???????????? */
  xsize = (LCD_GetXSize()/DrawProp[ActiveLayer].pFont->Width);
  
  switch (Mode)
  {
  case CENTER_MODE:
    {
      ref_column = Xpos + ((xsize - size)* DrawProp[ActiveLayer].pFont->Width) / 2;
      break;
    }
  case LEFT_MODE:
    {
      ref_column = Xpos;
      break;
    }
  case RIGHT_MODE:
    {
      ref_column = - Xpos + ((xsize - size)*DrawProp[ActiveLayer].pFont->Width);
      break;
    }    
  default:
    {
      ref_column = Xpos;
      break;
    }
  }
  
  /*????????????? */
  if ((ref_column < 1) || (ref_column >= 0x8000))
  {
    ref_column = 1;
  }

  /* ???????????????*/
  while ((*Text != 0) & (((LCD_GetXSize() - (i*DrawProp[ActiveLayer].pFont->Width)) & 0xFFFF)\
			>= DrawProp[ActiveLayer].pFont->Width))
  {
    /* ?????? */
    LCD_DisplayChar(ref_column, Ypos, *Text);
    /* ??????????????? */
    ref_column += DrawProp[ActiveLayer].pFont->Width;
    /* ????????? */
    Text++;
    i++;
  }  
}

/**
  * @brief  ?????????(??60?)
  * @param  Line: ????
  * @param  ptr: ?????
  * @retval ?
  */
void LCD_DisplayStringLine(uint16_t Line, uint8_t *ptr)
{  
  LCD_DisplayStringAt(0, LINE(Line), ptr, LEFT_MODE);
}
/**
  * @brief  ?????
  * @param  Xpos: X?????
  * @param  Ypos: Y?????
  * @param  Length: ????
  * @retval ?
  */
void LCD_DrawHLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length)
{
  uint32_t  Xaddress = 0;

  if(Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_ARGB8888)
  {
    Xaddress = (Ltdc_Handler.LayerCfg[ActiveLayer].FBStartAdress) + 4*(LCD_GetXSize()*Ypos + Xpos);
  }
  else if(Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB888)
  {
    Xaddress = (Ltdc_Handler.LayerCfg[ActiveLayer].FBStartAdress) + 3*(LCD_GetXSize()*Ypos + Xpos);
  }
  else if((Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB565) || \
          (Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_ARGB4444) || \
          (Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_AL88))  
  {
    Xaddress = (Ltdc_Handler.LayerCfg[ActiveLayer].FBStartAdress) + 2*(LCD_GetXSize()*Ypos + Xpos);   
  }
  else
  {
    Xaddress = (Ltdc_Handler.LayerCfg[ActiveLayer].FBStartAdress) + 2*(LCD_GetXSize()*Ypos + Xpos);   
  }	
  /* ???? */
  LL_FillBuffer(ActiveLayer, (uint32_t *)Xaddress, Length, 1, 0, DrawProp[ActiveLayer].TextColor);
}

/**
  * @brief  ?????
  * @param  Xpos: X?????
  * @param  Ypos: Y?????
  * @param  Length: ????
  * @retval ?
  */
void LCD_DrawVLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length)
{
  uint32_t  Xaddress = 0;
  
  if(Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_ARGB8888)
  {
    Xaddress = (Ltdc_Handler.LayerCfg[ActiveLayer].FBStartAdress) + 4*(LCD_GetXSize()*Ypos + Xpos);
  }
  else if(Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB888)
  {
    Xaddress = (Ltdc_Handler.LayerCfg[ActiveLayer].FBStartAdress) + 3*(LCD_GetXSize()*Ypos + Xpos);
  }
  else if((Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB565) || \
          (Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_ARGB4444) || \
          (Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_AL88))  
  {
    Xaddress = (Ltdc_Handler.LayerCfg[ActiveLayer].FBStartAdress) + 2*(LCD_GetXSize()*Ypos + Xpos);   
  }
  else
  {
    Xaddress = (Ltdc_Handler.LayerCfg[ActiveLayer].FBStartAdress) + 2*(LCD_GetXSize()*Ypos + Xpos);   
  }	
  
  /* ???? */
  LL_FillBuffer(ActiveLayer, (uint32_t *)Xaddress, 1, Length, (LCD_GetXSize() - 1), DrawProp[ActiveLayer].TextColor);
}

/**
  * @brief  ????????
  * @param  x1: ???X???
  * @param  y1: ???Y???
  * @param  x2: ???X???
  * @param  y2: ???Y???
  * @retval ?
  */
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
  int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, 
  yinc1 = 0, yinc2 = 0, den = 0, num = 0, num_add = 0, num_pixels = 0, 
  curpixel = 0;
  
  deltax = ABS(x2 - x1);        /* ?x????? */
  deltay = ABS(y2 - y1);        /* ?y????? */
  x = x1;                       /* ??????x????? */
  y = y1;                       /* ??????y????? */
  
  if (x2 >= x1)                 /* x?????? */
  {
    xinc1 = 1;
    xinc2 = 1;
  }
  else                          /* x?????? */
  {
    xinc1 = -1;
    xinc2 = -1;
  }
  
  if (y2 >= y1)                 /* y?????? */
  {
    yinc1 = 1;
    yinc2 = 1;
  }
  else                          /* y?????? */
  {
    yinc1 = -1;
    yinc2 = -1;
  }
  
  if (deltax >= deltay)         /* ?? y ????????x???*/
  {
    xinc1 = 0;                  /* ??????????????? x */
    yinc2 = 0;                  /* ????????? y */
    den = deltax;
    num = deltax / 2;
    num_add = deltay;
    num_pixels = deltax;         /* x?y??? */
  }
  else                          /* ?? x ????????y??? */
  {
    xinc2 = 0;                  /* ????????? x */
    yinc1 = 0;                  /* ??????????????? y */
    den = deltay;
    num = deltay / 2;
    num_add = deltax;
    num_pixels = deltay;         /* y?x??? */
  }
  
  for (curpixel = 0; curpixel <= num_pixels; curpixel++)
  {
    LCD_DrawPixel(x, y, DrawProp[ActiveLayer].TextColor);   /* ??????? */
    num += num_add;                            /* ??????????? */
    if (num >= den)                           /* ??????????? */
    {
      num -= den;                             /* ??????? */
      x += xinc1;                             /* x??? */
      y += yinc1;                             /* y??? */
    }
    x += xinc2;                               /* y??? */
    y += yinc2;                               /* y??? */
  }
}

/**
  * @brief  ??????
  * @param  Xpos:   X???
  * @param  Ypos:   Y???
  * @param  Width:  ????  
  * @param  Height: ????
  * @retval ?
  */
void LCD_DrawRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
  /* ????? */
  LCD_DrawHLine(Xpos, Ypos, Width);
  LCD_DrawHLine(Xpos, (Ypos+ Height), Width);
  
  /* ????? */
  LCD_DrawVLine(Xpos, Ypos, Height);
  LCD_DrawVLine((Xpos + Width), Ypos, Height);
}

/**
  * @brief  ??????
  * @param  Xpos:   X???
  * @param  Ypos:   Y???
  * @param  Radius: ????
  * @retval ?
  */
void LCD_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
  int32_t   decision;    /* ???? */ 
  uint32_t  current_x;   /* ??x??? */
  uint32_t  current_y;   /* ??y??? */
  
  decision = 3 - (Radius << 1);
  current_x = 0;
  current_y = Radius;
  
  while (current_x <= current_y)
  {
    LCD_DrawPixel((Xpos + current_x), (Ypos - current_y), DrawProp[ActiveLayer].TextColor);
    
    LCD_DrawPixel((Xpos - current_x), (Ypos - current_y), DrawProp[ActiveLayer].TextColor);
    
    LCD_DrawPixel((Xpos + current_y), (Ypos - current_x), DrawProp[ActiveLayer].TextColor);
    
    LCD_DrawPixel((Xpos - current_y), (Ypos - current_x), DrawProp[ActiveLayer].TextColor);
    
    LCD_DrawPixel((Xpos + current_x), (Ypos + current_y), DrawProp[ActiveLayer].TextColor);
    
    LCD_DrawPixel((Xpos - current_x), (Ypos + current_y), DrawProp[ActiveLayer].TextColor);
    
    LCD_DrawPixel((Xpos + current_y), (Ypos + current_x), DrawProp[ActiveLayer].TextColor);
    
    LCD_DrawPixel((Xpos - current_y), (Ypos + current_x), DrawProp[ActiveLayer].TextColor);
    
    if (decision < 0)
    { 
      decision += (current_x << 2) + 6;
    }
    else
    {
      decision += ((current_x - current_y) << 2) + 10;
      current_y--;
    }
    current_x++;
  } 
}

/**
  * @brief  ??????
  * @param  Points: ????????
  * @param  PointCount: ??
  * @retval ?
  */
void LCD_DrawPolygon(pPoint Points, uint16_t PointCount)
{
  int16_t x = 0, y = 0;
  
  if(PointCount < 2)
  {
    return;
  }
  
  LCD_DrawLine(Points->X, Points->Y, (Points+PointCount-1)->X, (Points+PointCount-1)->Y);
  
  while(--PointCount)
  {
    x = Points->X;
    y = Points->Y;
    Points++;
    LCD_DrawLine(x, y, Points->X, Points->Y);
  }
}

/**
  * @brief  ??????
  * @param  Xpos:   X???
  * @param  Ypos:   Y???
  * @param  XRadius: ??X???
  * @param  YRadius: ??Y???
  * @retval ?
  */
void LCD_DrawEllipse(int Xpos, int Ypos, int XRadius, int YRadius)
{
  int x = 0, y = -YRadius, err = 2-2*XRadius, e2;
  float k = 0, rad1 = 0, rad2 = 0;
  
  rad1 = XRadius;
  rad2 = YRadius;
  
  k = (float)(rad2/rad1);  
  
  do { 
    LCD_DrawPixel((Xpos-(uint16_t)(x/k)), (Ypos+y), DrawProp[ActiveLayer].TextColor);
    LCD_DrawPixel((Xpos+(uint16_t)(x/k)), (Ypos+y), DrawProp[ActiveLayer].TextColor);
    LCD_DrawPixel((Xpos+(uint16_t)(x/k)), (Ypos-y), DrawProp[ActiveLayer].TextColor);
    LCD_DrawPixel((Xpos-(uint16_t)(x/k)), (Ypos-y), DrawProp[ActiveLayer].TextColor);      
    
    e2 = err;
    if (e2 <= x) {
      err += ++x*2+1;
      if (-y == x && e2 <= y) e2 = 0;
    }
    if (e2 > y) err += ++y*2+1;     
  }
  while (y <= 0);
}

/**
  * @brief  ?????
  * @param  Xpos:   X???
  * @param  Ypos:   Y???
  * @param  RGB_Code: ?????
  * @retval ?
  */
void LCD_DrawPixel(uint16_t Xpos, uint16_t Ypos, uint32_t RGB_Code)
{

  if(Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_ARGB8888)
  {
    *(__IO uint32_t*) (Ltdc_Handler.LayerCfg[ActiveLayer].FBStartAdress + (4*(Ypos*LCD_GetXSize() + Xpos))) = RGB_Code;
  }
  else if(Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB888)
  {
    *(__IO uint8_t*) (Ltdc_Handler.LayerCfg[ActiveLayer].FBStartAdress + (3*(Ypos*LCD_GetXSize() + Xpos))+2) = 0xFF&(RGB_Code>>16);
	*(__IO uint8_t*) (Ltdc_Handler.LayerCfg[ActiveLayer].FBStartAdress + (3*(Ypos*LCD_GetXSize() + Xpos))+1) = 0xFF&(RGB_Code>>8);
	*(__IO uint8_t*) (Ltdc_Handler.LayerCfg[ActiveLayer].FBStartAdress + (3*(Ypos*LCD_GetXSize() + Xpos))) = 0xFF&RGB_Code;
  }
  else if((Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB565) || \
          (Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_ARGB4444) || \
          (Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_AL88))  
  {
    *(__IO uint16_t*) (Ltdc_Handler.LayerCfg[ActiveLayer].FBStartAdress + (2*(Ypos*LCD_GetXSize() + Xpos))) = (uint16_t)RGB_Code;   
  }
  else
  {
    *(__IO uint16_t*) (Ltdc_Handler.LayerCfg[ActiveLayer].FBStartAdress + (2*(Ypos*LCD_GetXSize() + Xpos))) = (uint16_t)RGB_Code;   
  }

}

/**
  * @brief  ???????flash???ARGB888(32 bits per pixel)?????
  * @param  Xpos: Bmp????X ???
  * @param  Ypos: Bmp????Y ???
  * @param  pbmp: ????????flash?Bmp??????
  * @retval ?
  */
void LCD_DrawBitmap(uint32_t Xpos, uint32_t Ypos, uint8_t *pbmp)
{
  uint32_t index = 0, width = 0, height = 0, bit_pixel = 0;
  uint32_t address;
  uint32_t input_color_mode = 0;
  
  /* ?????????? */
  index = *(__IO uint16_t *) (pbmp + 10);
  index |= (*(__IO uint16_t *) (pbmp + 12)) << 16;
  
  /* ?????? */
  width = *(uint16_t *) (pbmp + 18);
  width |= (*(uint16_t *) (pbmp + 20)) << 16;
  
  /* ?????? */
  height = *(uint16_t *) (pbmp + 22);
  height |= (*(uint16_t *) (pbmp + 24)) << 16; 
  
  /* ?????? */
  bit_pixel = *(uint16_t *) (pbmp + 28);   
  
  /* ???? */
  address = Ltdc_Handler.LayerCfg[ActiveLayer].FBStartAdress + (((LCD_GetXSize()*Ypos) + Xpos)*(4));
  
  /*????????? */    
  if ((bit_pixel/8) == 4)
  {
    input_color_mode = CM_ARGB8888;
  }
  else if ((bit_pixel/8) == 2)
  {
    input_color_mode = CM_RGB565;   
  }
  else 
  {
    input_color_mode = CM_RGB888;
  }
  
  /* ??????? */
  pbmp += (index + (width * (height - 1) * (bit_pixel/8)));  
  
  /* ?????? ARGB8888 ???? */
  for(index=0; index < height; index++)
  {
    /* ?????? */
    LL_ConvertLineToARGB8888((uint32_t *)pbmp, (uint32_t *)address, width, input_color_mode);
    
    /* ????????? */
    address+=  (LCD_GetXSize()*4);
    pbmp -= width*(bit_pixel/8);
  } 
}

/**
  * @brief  ????????
  * @param  Xpos: X???
  * @param  Ypos: Y???
  * @param  Width:  ???? 
  * @param  Height: ????
  * @retval ?
  */
void LCD_FillRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
  uint32_t  x_address = 0;
  
  /* ?????? */
  LCD_SetTextColor(DrawProp[ActiveLayer].TextColor);
  
  /* ???????? */
    if(Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_ARGB8888)
  {
    x_address = (Ltdc_Handler.LayerCfg[ActiveLayer].FBStartAdress) + 4*(LCD_GetXSize()*Ypos + Xpos);
  }
  else if(Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB888)
  {
    x_address = (Ltdc_Handler.LayerCfg[ActiveLayer].FBStartAdress) + 3*(LCD_GetXSize()*Ypos + Xpos);
  }
  else if((Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB565) || \
          (Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_ARGB4444) || \
          (Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_AL88))  
  {
    x_address = (Ltdc_Handler.LayerCfg[ActiveLayer].FBStartAdress) + 2*(LCD_GetXSize()*Ypos + Xpos);   
  }
  else
  {
    x_address = (Ltdc_Handler.LayerCfg[ActiveLayer].FBStartAdress) + 2*(LCD_GetXSize()*Ypos + Xpos);
  }	
  /* ???? */
  LL_FillBuffer(ActiveLayer, (uint32_t *)x_address, Width, Height, (LCD_GetXSize() - Width), DrawProp[ActiveLayer].TextColor);
}

/**
  * @brief  ???????
  * @param  Xpos: X???
  * @param  Ypos: Y???
  * @param  Radius: ????
  * @retval ?
  */
void LCD_FillCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
  int32_t   decision;    /* ???? */ 
  uint32_t  current_x;   /* ??x??? */
  uint32_t  current_y;   /* ??y??? */
  
  decision = 3 - (Radius << 1);
  
  current_x = 0;
  current_y = Radius;
  
  LCD_SetTextColor(DrawProp[ActiveLayer].TextColor);
  
  while (current_x <= current_y)
  {
    if(current_y > 0) 
    {
      LCD_DrawHLine(Xpos - current_y, Ypos + current_x, 2*current_y);
      LCD_DrawHLine(Xpos - current_y, Ypos - current_x, 2*current_y);
    }
    
    if(current_x > 0) 
    {
      LCD_DrawHLine(Xpos - current_x, Ypos - current_y, 2*current_x);
      LCD_DrawHLine(Xpos - current_x, Ypos + current_y, 2*current_x);
    }
    if (decision < 0)
    { 
      decision += (current_x << 2) + 6;
    }
    else
    {
      decision += ((current_x - current_y) << 2) + 10;
      current_y--;
    }
    current_x++;
  }
  
  LCD_SetTextColor(DrawProp[ActiveLayer].TextColor);
  LCD_DrawCircle(Xpos, Ypos, Radius);
}

/**
  * @brief  ?????
  * @param  Points: ????????
  * @param  PointCount: ??
  * @retval ?
  */
void LCD_FillPolygon(pPoint Points, uint16_t PointCount)
{
  int16_t X = 0, Y = 0, X2 = 0, Y2 = 0, X_center = 0, Y_center = 0, X_first = 0, Y_first = 0, pixelX = 0, pixelY = 0, counter = 0;
  uint16_t  image_left = 0, image_right = 0, image_top = 0, image_bottom = 0;
  
  image_left = image_right = Points->X;
  image_top= image_bottom = Points->Y;
  
  for(counter = 1; counter < PointCount; counter++)
  {
    pixelX = POLY_X(counter);
    if(pixelX < image_left)
    {
      image_left = pixelX;
    }
    if(pixelX > image_right)
    {
      image_right = pixelX;
    }
    
    pixelY = POLY_Y(counter);
    if(pixelY < image_top)
    { 
      image_top = pixelY;
    }
    if(pixelY > image_bottom)
    {
      image_bottom = pixelY;
    }
  }  
  
  if(PointCount < 2)
  {
    return;
  }
  
  X_center = (image_left + image_right)/2;
  Y_center = (image_bottom + image_top)/2;
  
  X_first = Points->X;
  Y_first = Points->Y;
  
  while(--PointCount)
  {
    X = Points->X;
    Y = Points->Y;
    Points++;
    X2 = Points->X;
    Y2 = Points->Y;    
    
    FillTriangle(X, X2, X_center, Y, Y2, Y_center);
    FillTriangle(X, X_center, X2, Y, Y_center, Y2);
    FillTriangle(X_center, X2, X, Y_center, Y2, Y);   
  }
  
  FillTriangle(X_first, X2, X_center, Y_first, Y2, Y_center);
  FillTriangle(X_first, X_center, X2, Y_first, Y_center, Y2);
  FillTriangle(X_center, X2, X_first, Y_center, Y2, Y_first);   
}

/**
  * @brief  ????????
  * @param  Xpos:   X???
  * @param  Ypos:   Y???
  * @param  XRadius: ??X???
  * @param  YRadius: ??Y???
  * @retval ?
  */
void LCD_FillEllipse(int Xpos, int Ypos, int XRadius, int YRadius)
{
  int x = 0, y = -YRadius, err = 2-2*XRadius, e2;
  float k = 0, rad1 = 0, rad2 = 0;
  
  rad1 = XRadius;
  rad2 = YRadius;
  
  k = (float)(rad2/rad1);
  
  do 
  {       
    LCD_DrawHLine((Xpos-(uint16_t)(x/k)), (Ypos+y), (2*(uint16_t)(x/k) + 1));
    LCD_DrawHLine((Xpos-(uint16_t)(x/k)), (Ypos-y), (2*(uint16_t)(x/k) + 1));
    
    e2 = err;
    if (e2 <= x) 
    {
      err += ++x*2+1;
      if (-y == x && e2 <= y) e2 = 0;
    }
    if (e2 > y) err += ++y*2+1;
  }
  while (y <= 0);
}

/**
  * @brief  ????
  * @retval ?
  */
void LCD_DisplayOn(void)
{
  /* ??? */
  __HAL_LTDC_ENABLE(&Ltdc_Handler);
  HAL_GPIO_WritePin(LTDC_DISP_GPIO_PORT, LTDC_DISP_GPIO_PIN, GPIO_PIN_SET);/* LCD_DISP??*/
  HAL_GPIO_WritePin(LTDC_BL_GPIO_PORT, LTDC_BL_GPIO_PIN, GPIO_PIN_SET);  /* ???*/
}

/**
  * @brief  ????
  * @retval ?
  */
void LCD_DisplayOff(void)
{
  /* ??? */
  __HAL_LTDC_DISABLE(&Ltdc_Handler);
  HAL_GPIO_WritePin(LTDC_DISP_GPIO_PORT, LTDC_DISP_GPIO_PIN, GPIO_PIN_RESET); /* LCD_DISP??*/
  HAL_GPIO_WritePin(LTDC_BL_GPIO_PORT, LTDC_BL_GPIO_PIN, GPIO_PIN_RESET);/*???*/
}

/**
  * @brief  ????
  * @param  hltdc: LTDC??
  * @param  Params
  * @retval ?
  */
void LCD_ClockConfig(LTDC_HandleTypeDef *hltdc, void *Params)
{
  static RCC_PeriphCLKInitTypeDef  periph_clk_init_struct;

  /* LCD???? */
  /* PLLSAI_VCO Input = HSE_VALUE/PLL_M = 1 Mhz */
  /* PLLSAI_VCO Output = PLLSAI_VCO Input * PLLSAIN = 192 Mhz */
  /* PLLLCDCLK = PLLSAI_VCO Output/PLLSAIR = 192/5 = 38.4 Mhz */
  /* LTDC???? = PLLLCDCLK / LTDC_PLLSAI_DIVR_4 = 38.4/4 = 9.6Mhz */
  periph_clk_init_struct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
  periph_clk_init_struct.PLLSAI.PLLSAIN = 400;
  periph_clk_init_struct.PLLSAI.PLLSAIR = 4;
  periph_clk_init_struct.PLLSAIDivR = RCC_PLLSAIDIVR_2;
  HAL_RCCEx_PeriphCLKConfig(&periph_clk_init_struct);
}


/*******************************************************************************
                            ????
*******************************************************************************/

/**
  * @brief  ??????
  * @param  Xpos: ????????
  * @param  Ypos: ?????
  * @param  c: ?????????
  * @retval ?
  */
static void DrawChar(uint16_t Xpos, uint16_t Ypos, const uint8_t *c)
{
  uint32_t i = 0, j = 0;
  uint16_t height, width;
  uint8_t  offset;
  uint8_t  *pchar;
  uint32_t line;
  
  height = DrawProp[ActiveLayer].pFont->Height;//??????????
  width  = DrawProp[ActiveLayer].pFont->Width; //??????????
  
  offset =  8 *((width + 7)/8) -  width ;//??????????????,??????-????
  
  for(i = height-3; i < height; i++)//????????
  {
    pchar = ((uint8_t *)c + (width + 7)/8 * i);//???????????????
    for (j = 0; j < width; j++)//????????
    {
        LCD_DrawPixel((Xpos + j), Ypos, DrawProp[ActiveLayer].BackColor);
    }
    Ypos++;
  }
  for(i = 0; i < height-3; i++)//????????
  {
    pchar = ((uint8_t *)c + (width + 7)/8 * i);//???????????????
    
    switch(((width + 7)/8))//???????????????????
    {
      
    case 1:
      line =  pchar[0];      //????????8???????
      break;
      
    case 2:
      line =  (pchar[0]<< 8) | pchar[1]; //????????8??16???????     
      break;
      
    case 3:
    default:
      line =  (pchar[0]<< 16) | (pchar[1]<< 8) | pchar[2]; //????????16??24???????     
      break;
    } 
    
    for (j = 0; j < width; j++)//????????
    {
      if(line & (1 << (width- j + offset- 1))) //??????????????????????????
      {
        LCD_DrawPixel((Xpos + j), Ypos, DrawProp[ActiveLayer].TextColor);
      }
      else//????????????????????
      {
        LCD_DrawPixel((Xpos + j), Ypos, DrawProp[ActiveLayer].BackColor);
      } 
    }
    Ypos++;
  }
}

/**
 * @brief  ?????????????
 * @param  usX :?????????????X??
 * @param  usY :?????????????Y??
 * @param  usChar :????????(???)
 * @retval ?
 */ 
static void LCD_DispChar_CH (uint16_t Xpos, uint16_t Ypos, uint16_t Text)
{
  uint32_t i = 0, j = 0;
  uint16_t height, width;
  uint8_t  offset;
  uint8_t  *pchar;
  uint8_t  Buffer[HEIGHT_CH_CHAR*3];
  uint32_t line;
	
  GetGBKCode (Buffer, Text );
  
  height = 	HEIGHT_CH_CHAR;//?????//??????????
  width  =  WIDTH_CH_CHAR; //??????????
  
  offset =  8 *((width + 7)/8) -  width ;//??????????????,??????-????
  
  for(i = 0; i < height; i++)//????????
  {
    pchar = ((uint8_t *)Buffer + (width + 7)/8 * i);//???????????????
    
    switch(((width + 7)/8))//???????????????????
    {
      
    case 1:
      line =  pchar[0];      //????????8???????
      break;
      
    case 2:
      line =  (pchar[0]<< 8) | pchar[1]; //????????8??16???????     
      break;
      
    case 3:
    default:
      line =  (pchar[0]<< 16) | (pchar[1]<< 8) | pchar[2]; //????????16??24???????     
      break;
    } 
    
    for (j = 0; j < width; j++)//????????
    {
      if(line & (1 << (width- j + offset- 1))) //??????????????????????????
      {
        LCD_DrawPixel((Xpos + j), Ypos, DrawProp[ActiveLayer].TextColor);
      }
      else//????????????????????
      {
        LCD_DrawPixel((Xpos + j), Ypos, DrawProp[ActiveLayer].BackColor);
      } 
    }
    Ypos++;
  }
}

/**
  * @brief  ??????,???????,??????
			?????,?????????Font24??
  * @param  Line: ???????LINE(0) - LINE(N)
  * @param  *ptr: ?????????
  * @retval None
  */
void LCD_DisplayStringLine_EN_CH(uint16_t Line, uint8_t *ptr)
{  
  uint16_t refcolumn = 0;
  /* ??????????????? */
  while ((refcolumn < LCD_PIXEL_WIDTH) && ((*ptr != 0) & (((refcolumn + DrawProp[ActiveLayer].pFont->Width) & 0xFFFF) >= DrawProp[ActiveLayer].pFont->Width)))
  {
	/* ??LCD?????? */
	if ( * ptr <= 126 )	           	//????
	{
				
		LCD_DisplayChar(refcolumn, LINE(Line), *ptr);
		/* ??????????? */
		refcolumn += DrawProp[ActiveLayer].pFont->Width;
		/* ???????????? */
		ptr++;
	}
	
	else	                            //????
	{	
		uint16_t usCh;
		
		/*???????*/
		usCh = * ( uint16_t * ) ptr;	
		/*??????*/
		usCh = ( usCh << 8 ) + ( usCh >> 8 );		
		
		/*????*/
		LCD_DispChar_CH ( refcolumn,LINE(Line) , usCh );
		/*??????*/
		refcolumn += WIDTH_CH_CHAR;
		/* ???????????? */
		ptr += 2; 		
    }		
  }
}
/**
  * @brief  ?????(????)
  * @param  x1: ????X???
  * @param  y1: ????Y???
  * @param  x2: ????X???
  * @param  y2: ????Y???
  * @param  x3: ????X???
  * @param  y3: ????Y???
  * @retval ?
  */
static void FillTriangle(uint16_t x1, uint16_t x2, uint16_t x3, uint16_t y1, uint16_t y2, uint16_t y3)
{ 
  int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, 
  yinc1 = 0, yinc2 = 0, den = 0, num = 0, num_add = 0, num_pixels = 0,
  curpixel = 0;
  
  deltax = ABS(x2 - x1);        /* ?x????? */
  deltay = ABS(y2 - y1);        /* ?y????? */
  x = x1;                       /* ??????x????? */
  y = y1;                       /* ??????y????? */
  
  if (x2 >= x1)                 /* x??????*/
  {
    xinc1 = 1;
    xinc2 = 1;
  }
  else                          /* x?????? */
  {
    xinc1 = -1;
    xinc2 = -1;
  }
  
  if (y2 >= y1)                 /* y??????*/
  {
    yinc1 = 1;
    yinc2 = 1;
  }
  else                          /* y?????? */
  {
    yinc1 = -1;
    yinc2 = -1;
  }
  
  if (deltax >= deltay)         /* ?? y ????????x???*/
  {
    xinc1 = 0;                  /* ??????????????? x */
    yinc2 = 0;                  /* ????????? y */
    den = deltax;
    num = deltax / 2;
    num_add = deltay;
    num_pixels = deltax;         /* x?y??? */
  }
  else                          /* ?? x ????????y??? */
  {
    xinc2 = 0;                  /* ????????? x */
    yinc1 = 0;                  /* ??????????????? y */
    den = deltay;
    num = deltay / 2;
    num_add = deltax;
    num_pixels = deltay;         /* y?x??? */
  }
  
  for (curpixel = 0; curpixel <= num_pixels; curpixel++)
  {
    LCD_DrawLine(x, y, x3, y3);
    
    num += num_add;              /* ??????????? */
    if (num >= den)             /* ????????????? */
    {
      num -= den;               /* ??????? */
      x += xinc1;               /* x??? */
      y += yinc1;               /* y??? */
    }
    x += xinc2;                 /* x??? */
    y += yinc2;                 /* y??? */
  } 
}

/**
  * @brief  ???????
  * @param  LayerIndex: ???
  * @param  pDst: ?????????
  * @param  xSize: ?????
  * @param  ySize: ?????
  * @param  OffLine: ???
  * @param  ColorIndex: ????
  * @retval None
  */
static void LL_FillBuffer(uint32_t LayerIndex, void *pDst, uint32_t xSize, uint32_t ySize, uint32_t OffLine, uint32_t ColorIndex) 
{

  Dma2d_Handler.Init.Mode         = DMA2D_R2M;
  if(Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB565)
  { 
    Dma2d_Handler.Init.ColorMode    = DMA2D_RGB565;
  }
  else if(Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_ARGB8888)
  { 
    Dma2d_Handler.Init.ColorMode    = DMA2D_ARGB8888;
  }
  else if(Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB888)
  { 
    Dma2d_Handler.Init.ColorMode    = DMA2D_RGB888;
  }
  else if(Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_ARGB1555)
  { 
    Dma2d_Handler.Init.ColorMode    = DMA2D_ARGB1555;
  }
  else if(Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_ARGB4444)
  { 
    Dma2d_Handler.Init.ColorMode    = DMA2D_ARGB4444;
  }
  Dma2d_Handler.Init.OutputOffset = OffLine;      
  
  Dma2d_Handler.Instance = DMA2D;
  
  /* DMA2D ??? */
  if(HAL_DMA2D_Init(&Dma2d_Handler) == HAL_OK) 
  {
    if(HAL_DMA2D_ConfigLayer(&Dma2d_Handler, LayerIndex) == HAL_OK) 
    {
      if (HAL_DMA2D_Start(&Dma2d_Handler, ColorIndex, (uint32_t)pDst, xSize, ySize) == HAL_OK)
      {
        /* DMA???? */  
        HAL_DMA2D_PollForTransfer(&Dma2d_Handler, 100);
      }
    }
  } 
}

/**
  * @brief  ?????ARGB8888????
  * @param  pSrc: ?????????
  * @param  pDst: ????
  * @param  xSize: ?????
  * @param  ColorMode: ??????   
  * @retval ?
  */
static void LL_ConvertLineToARGB8888(void *pSrc, void *pDst, uint32_t xSize, uint32_t ColorMode)
{    
  /* ??DMA2D??,????????? */
  Dma2d_Handler.Init.Mode         = DMA2D_M2M_PFC;
  Dma2d_Handler.Init.ColorMode    = DMA2D_ARGB8888;
  Dma2d_Handler.Init.OutputOffset = 0;     
  
  /* Foreground Configuration */
  Dma2d_Handler.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
  Dma2d_Handler.LayerCfg[1].InputAlpha = 0xFF;
  Dma2d_Handler.LayerCfg[1].InputColorMode = ColorMode;
  Dma2d_Handler.LayerCfg[1].InputOffset = 0;
  
  Dma2d_Handler.Instance = DMA2D; 
  
  /* DMA2D ??? */
  if(HAL_DMA2D_Init(&Dma2d_Handler) == HAL_OK) 
  {
    if(HAL_DMA2D_ConfigLayer(&Dma2d_Handler, 1) == HAL_OK) 
    {
      if (HAL_DMA2D_Start(&Dma2d_Handler, (uint32_t)pSrc, (uint32_t)pDst, xSize, 1) == HAL_OK)
      {
        /* DMA????*/  
        HAL_DMA2D_PollForTransfer(&Dma2d_Handler, 10);
      }
    }
  } 
}

#if GBKCODE_FLASH

/*??FLASH??*/

//???????FLASH????? :
//GBKCODE_START_ADDRESS ?fonts.h????
/**
  * @brief  ??FLASH????????
	* @param  pBuffer:??????????
	* @param  c : ??????
  * @retval None.
  */
int GetGBKCode_from_EXFlash( uint8_t * pBuffer, uint16_t c)
{ 
	unsigned char High8bit,Low8bit;
	unsigned int pos;

	static uint8_t everRead=0;

	/*?????,???FLASH*/
	if(everRead == 0)
	{
		QSPI_FLASH_Init();
		everRead = 1;
	}

	High8bit= c >> 8;     /* ??8??? */
	Low8bit= c & 0x00FF;  /* ??8??? */		

	/*GB2312 ??*/
	pos = ((High8bit-0xa1)*94+Low8bit-0xa1)*24*24/8;
	BSP_QSPI_Read(pBuffer,GBKCODE_START_ADDRESS+pos,24*24/8); //??????  
	//	  printf ( "%02x %02x %02x %02x\n", pBuffer[0],pBuffer[1],pBuffer[2],pBuffer[3]);

	return 0;  

}

#else

/*??SD??*/

static FIL fnew;													/* file objects */
static FATFS fs;													/* Work area (file system object) for logical drives */
static FRESULT res_sd; 
static UINT br;            					/* File R/W count */

//????????,fonts.h???:
//#define GBKCODE_FILE_NAME			"0:/Font/GB2312_H2424.FON"

/**
  * @brief  ??SD?????????
	* @param  pBuffer:??????????
	* @param  c : ??????
  * @retval None.
  */
int GetGBKCode_from_sd ( uint8_t * pBuffer, uint16_t c)
{ 
    unsigned char High8bit,Low8bit;
    unsigned int pos;
		
		static uint8_t everRead = 0;
	
    High8bit= c >> 8;     /* ??8??? */
    Low8bit= c & 0x00FF;  /* ??8??? */
		
    pos = ((High8bit-0xa1)*94+Low8bit-0xa1)*24*24/8;
	
	/*?????,??????,???sd*/
	if(everRead == 0)
	{
		res_sd = f_mount(&fs,"0:",1);
		everRead = 1;

	}
		
    res_sd = f_open(&fnew , GBKCODE_FILE_NAME, FA_OPEN_EXISTING | FA_READ);
    
    if ( res_sd == FR_OK ) 
    {
        f_lseek (&fnew, pos);		//????
        res_sd = f_read( &fnew, pBuffer, 24*24/8, &br );		 //24*24????? ??? ??24*24/8???
        
        f_close(&fnew);
        
        return 0;  
    }    
    else
        return -1;    
}

#endif
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
