// FILE:   ad2s1210.h

#ifndef ad2s1210_H_
#define ad2s1210_H_

#define  SDI_GPIO       58
#define  SDI_MUX        15
#define  SDO_GPIO       59
#define  SDO_MUX        15
#define  CLK_GPIO       60
#define  CLK_MUX        15

#define  SCS_GPIO        61 // /WR c'est le /CS de la SPI
#define  SCS_MUX         0
//#define  AD2S1210_WR_PIN     SCS_GPIO
#define  AD2S1210_SAMPLE 18

#define AD2S1210_REG_EXC_FREQ 0x91
#define AD2S1210_REG_CNTRL 0x92
#define AD2S1210_REG_FAULT 0xff
#define AD2S1210_REG_POS_H 0x80
#define AD2S1210_REG_POS_L 0x81
#define AD2S1210_REG_VEL_H 0x82
#define AD2S1210_REG_VEL_L 0x83

void InitSpi();
void InitSpiaGpio();
Uint16 spi_xmit(Uint16 data);

void AD2S1210_WR(Uint16 reg, Uint16 data);
Uint16 AD2S1210_RD(Uint16 reg);
void AD2S1210_SAMPLING();
void ad2s1210_conf(void);
Uint16 AD2S1210_Read_Pos();
int AD2S1210_Read_Vit();

#endif
