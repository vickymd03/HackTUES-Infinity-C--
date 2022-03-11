/**
  ******************************************************************************
  *                                 GreenBo         
  *                           The Embedded Experts
  ******************************************************************************
  * @file    BMP.c
  * @brief   This file provides code for the configuration
  *          of all the requested RAW to BMP transfers.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 GreenBo.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by GreenBo under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  **/

/* Includes ------------------------------------------------------------------*/
#include "BMP.h"
#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
uint8_t BitMapFileHeader[80];
/* Private function prototypes -----------------------------------------------*/


/* Private user code ---------------------------------------------------------*/
void BitMapFileHeader_init(void)
{
  uint32_t bfSize;
  uint32_t Offset;
  uint32_t biWidth;
  uint32_t biHeight;
  uint32_t SizeImage;
  uint32_t biClrUsed;

  bfSize = sizeof(sendBuf);
  Offset = 80;
  biWidth = FrameWidth;
  biHeight = FrameHeight;
  SizeImage = FrameWidth * FrameHeight * 2;
  biClrUsed = 0;  //Colours -> 0 (All)

  /** Bitmap file header 14 bytes **/
  BitMapFileHeader[0] = 'B';
  BitMapFileHeader[1] = 'M';
  BitMapFileHeader[2] = bfSize & 0xff;  // Size of File
  BitMapFileHeader[3] = (bfSize >> 8) & 0xff;
  BitMapFileHeader[4] = (bfSize >> 16) & 0xff;
  BitMapFileHeader[5] = (bfSize >> 24) & 0xff;
  BitMapFileHeader[6] = 0;  // Reserved
  BitMapFileHeader[7] = 0;
  BitMapFileHeader[8] = 0;  // Reserved
  BitMapFileHeader[9] = 0;
  BitMapFileHeader[10] = Offset & 0xff;  // Offset to images data ((uint16_t *)&pic[1][0])
  BitMapFileHeader[11] = (Offset >> 8) & 0xff;
  BitMapFileHeader[12] = (Offset >> 16) & 0xff;
  BitMapFileHeader[13] = (Offset >> 24) & 0xff;

  /** bitmap information header 40 bytes + mask 16 bytes**/
  BitMapFileHeader[14] = 56;  // Size of Info -> 40 + 16 = 56
  BitMapFileHeader[15] = 0;
  BitMapFileHeader[16] = 0;
  BitMapFileHeader[17] = 0;
  BitMapFileHeader[18] = biWidth & 0xff;  // Image Width
  BitMapFileHeader[19] = (biWidth >> 8) & 0xff;
  BitMapFileHeader[20] = (biWidth >> 16) & 0xff;
  BitMapFileHeader[21] = (biWidth >> 24) & 0xff;
  BitMapFileHeader[22] = biHeight & 0xff;  // Image Height
  BitMapFileHeader[23] = (biHeight >> 8) & 0xff;
  BitMapFileHeader[24] = (biHeight >> 16) & 0xff;
  BitMapFileHeader[25] = (biHeight >> 24) & 0xff;
  BitMapFileHeader[26] = 1; // Planes
  BitMapFileHeader[27] = 0;
  BitMapFileHeader[28] = 16;  // RGB 565 -> 16 (bits)
  BitMapFileHeader[29] = 0;
  BitMapFileHeader[30] = 3; // Compression -> 3 (RGB565)
  BitMapFileHeader[31] = 0;
  BitMapFileHeader[32] = 0;
  BitMapFileHeader[33] = 0;
  BitMapFileHeader[34] = SizeImage & 0xff; // SizeImage
  BitMapFileHeader[35] = (SizeImage >> 8) & 0xff;
  BitMapFileHeader[36] = (SizeImage >> 16) & 0xff;
  BitMapFileHeader[37] = (SizeImage >> 24) & 0xff;
  BitMapFileHeader[38] = 0; // XPerlPerMeter
  BitMapFileHeader[39] = 0;
  BitMapFileHeader[40] = 0;
  BitMapFileHeader[41] = 0;
  BitMapFileHeader[42] = 0; // YPerlPerMeter
  BitMapFileHeader[43] = 0;
  BitMapFileHeader[44] = 0;
  BitMapFileHeader[45] = 0;
  BitMapFileHeader[46] = biClrUsed & 0xff;  // Colours used -> 0 (All)
  BitMapFileHeader[47] = (biClrUsed >> 8) & 0xff;
  BitMapFileHeader[48] = (biClrUsed >> 16) & 0xff;
  BitMapFileHeader[49] = (biClrUsed >> 24) & 0xff;
  BitMapFileHeader[50] = 0; // ColorImportant -> 0 (All)
  BitMapFileHeader[51] = 0;
  BitMapFileHeader[52] = 0;
  BitMapFileHeader[53] = 0;

  /** extra bit mask **/
  BitMapFileHeader[54] = 0x00; // 0x0000001F B
  BitMapFileHeader[55] = 0xf8;
  BitMapFileHeader[56] = 0x00;
  BitMapFileHeader[57] = 0x00;
  BitMapFileHeader[58] = 0xe0; // 0x000007E0 G
  BitMapFileHeader[59] = 0x07;
  BitMapFileHeader[60] = 0x00;
  BitMapFileHeader[61] = 0x00;
  BitMapFileHeader[62] = 0x1f; // 0x000000F8 R
  BitMapFileHeader[63] = 0x00;
  BitMapFileHeader[64] = 0x00;
  BitMapFileHeader[65] = 0x00;
  BitMapFileHeader[66] = 0; // 0
  BitMapFileHeader[67] = 0;
  BitMapFileHeader[68] = 0;
  BitMapFileHeader[69] = 0;

  BitMapFileHeader[70] = 0x77; // ov7725 camera user ID
  //BitMapFileHeader[70] = 0x90; // mlx90640 camera user ID
  BitMapFileHeader[71] = 0; // CRC
  BitMapFileHeader[72] = 0; // Reserved
  BitMapFileHeader[73] = 0; // Reserved
  BitMapFileHeader[74] = 0; // Reserved
  BitMapFileHeader[75] = 0; // Reserved
  BitMapFileHeader[76] = 0; // Reserved
  BitMapFileHeader[77] = 0; // Reserved
  BitMapFileHeader[78] = 0; // Reserved
  BitMapFileHeader[79] = 0; // Reserved

  for(int i = 0; i < 80; i++)
  {
    sendBuf[i] = BitMapFileHeader[i];
  }

}

uint8_t Calculate_CRC(void)
{
}

void Prepare_RGBdata(void)
{
}


/************************ (C) COPYRIGHT GreenBo *****END OF FILE****/
