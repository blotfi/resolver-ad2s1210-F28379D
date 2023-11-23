//###########################################################################
//
// FILE:   F2837xD_Spi.c
//
// mon initialisation
//
// Included Files
//
#include "F2837xD_device.h"
#include "F2837xD_Examples.h"
#include "ad2s1210.h"
unsigned int dataH, dataL;

//
// Calculate BRR: 7-bit baud rate register value
// SPI CLK freq = 1000 kHz
// LSPCLK freq  = CPU freq / 4  (by default)
// BRR          = (LSPCLK freq / SPI CLK freq) - 1
//
#if CPU_FRQ_200MHZ
#define SPI_BRR        ((200E6 / 4) / 1000E3) - 1
#endif

#if CPU_FRQ_150MHZ
#define SPI_BRR        ((150E6 / 4) / 1000E3) - 1
#endif

#if CPU_FRQ_120MHZ
#define SPI_BRR        ((120E6 / 4) / 1000E3) - 1
#endif

//
// InitSPI - This function initializes the SPI to a known state
//
void InitSpi(void)
{
    // Initialize SPI-A
/*
    // Set reset low before configuration changes
    // Clock polarity (0 == rising, 1 == falling)
    // 16-bit character
    // Enable loop-back
    SpiaRegs.SPICCR.bit.SPISWRESET = 0;
    SpiaRegs.SPICCR.bit.CLKPOLARITY = 0;
    SpiaRegs.SPICCR.bit.SPICHAR = (16-1);
    SpiaRegs.SPICCR.bit.SPILBK = 1; //loopBack

    // Enable master (0 == slave, 1 == master)
    // Enable transmission (Talk)
    // Clock phase (0 == normal, 1 == delayed)
    // SPI interrupts are disabled
    SpiaRegs.SPICTL.bit.MASTER_SLAVE = 1;
    SpiaRegs.SPICTL.bit.TALK = 1;
    SpiaRegs.SPICTL.bit.CLK_PHASE = 0;
    SpiaRegs.SPICTL.bit.SPIINTENA = 0;
    // Set the baud rate
    SpiaRegs.SPIBRR.bit.SPI_BIT_RATE = SPI_BRR;
    // Set FREE bit
    // Halting on a breakpoint will not halt the SPI
    SpiaRegs.SPIPRI.bit.FREE = 1;
    // Release the SPI from reset
    SpiaRegs.SPICCR.bit.SPISWRESET = 1;
*/
    SpiaRegs.SPICCR.bit.SPISWRESET = 0;       // Put SPI in reset state
//    SpiaRegs.SPICCR.bit.SPICHAR = 0xF;        // 16-bit character
    SpiaRegs.SPICCR.bit.SPICHAR = 0x7;        // 8-bit character
    SpiaRegs.SPICCR.bit.SPILBK = 0;           // 1 => loopback On
    SpiaRegs.SPICCR.bit.CLKPOLARITY = 0;      // Rising edge without delay

    SpiaRegs.SPICTL.bit.SPIINTENA = 0;        // disable SPI interrupt
    SpiaRegs.SPICTL.bit.TALK = 1;             // enable transmission
    SpiaRegs.SPICTL.bit.MASTER_SLAVE = 1;     // master
    SpiaRegs.SPICTL.bit.CLK_PHASE = 0;        // Rising edge without delay
    SpiaRegs.SPICTL.bit.OVERRUNINTENA = 0;    // disable reciever overrun interrupt

    SpiaRegs.SPIBRR.bit.SPI_BIT_RATE = SPI_BRR;        // SPICLK = LSPCLK / 4 (max SPICLK)

    SpiaRegs.SPICCR.bit.SPISWRESET=1;         // Enable SPI
}

//
// InitSpiGpio - This function initializes GPIO pins to function as SPI pins.
//               Each GPIO pin can be configured as a GPIO pin or up to 3
//               different peripheral functional pins. By default all pins come
//               up as GPIO inputs after reset.
//
//               Caution:
//               For each SPI peripheral
//               Only one GPIO pin should be enabled for SPISOMO operation.
//               Only one GPIO pin should be enabled for SPISOMI operation.
//               Only one GPIO pin should be enabled for SPICLK  operation.
//               Only one GPIO pin should be enabled for SPISTE  operation.
//               Comment out other unwanted lines.

void InitSpiaGpio()
{
   EALLOW;
    // Enable internal pull-up for the selected pins
    // Pull-ups can be enabled or disabled by the user.
    // This will enable the pullups for the specified pins.
//    GpioCtrlRegs.GPAPUD.bit.GPIO16 = 0;  // Enable pull-up on GPIO16 (SPISIMOA)
//    GpioCtrlRegs.GPAPUD.bit.GPIO17 = 0;  // Enable pull-up on GPIO17 (SPISOMIA)
//    GpioCtrlRegs.GPAPUD.bit.GPIO18 = 0;  // Enable pull-up on GPIO18 (SPICLKA)
//    GpioCtrlRegs.GPAPUD.bit.GPIO19 = 0;  // Enable pull-up on GPIO19 (SPISTEA)

   // Set qualification for selected pins to asynch only
    // This will select asynch (no qualification) for the selected pins.
//    GpioCtrlRegs.GPAQSEL2.bit.GPIO16 = 3; // Asynch input GPIO16 (SPISIMOA)
//    GpioCtrlRegs.GPAQSEL2.bit.GPIO17 = 3; // Asynch input GPIO17 (SPISOMIA)
//    GpioCtrlRegs.GPAQSEL2.bit.GPIO18 = 3; // Asynch input GPIO18 (SPICLKA)
//    GpioCtrlRegs.GPAQSEL2.bit.GPIO19 = 3; // Asynch input GPIO19 (SPISTEA)

    //Configure SPI-A pins using GPIO regs
    // This specifies which of the possible GPIO pins will be SPI functional
    // pins.
//    GpioCtrlRegs.GPAMUX2.bit.GPIO16 = 1; // Configure GPIO16 as SPISIMOA
//    GpioCtrlRegs.GPAMUX2.bit.GPIO17 = 1; // Configure GPIO17 as SPISOMIA
//    GpioCtrlRegs.GPAMUX2.bit.GPIO18 = 1; // Configure GPIO18 as SPICLKA
//    GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 1; // Configure GPIO19 as SPISTEA

    // SPI GPIO MUX pins config
    // set up SDI pin MOSI P58
    GPIO_SetupPinOptions(SDI_GPIO, GPIO_INPUT, GPIO_ASYNC);
    GPIO_SetupPinMux(SDI_GPIO,0,SDI_MUX);
    // set up SDO pin MISO P59
    GPIO_SetupPinOptions(SDO_GPIO, GPIO_INPUT, GPIO_ASYNC);
    GPIO_SetupPinMux(SDO_GPIO,0,SDO_MUX);
    // set up CLK pin P60
    GPIO_SetupPinOptions(CLK_GPIO, GPIO_INPUT, GPIO_ASYNC);
    GPIO_SetupPinMux(CLK_GPIO,0,CLK_MUX);
    // set up SCS pin /WR p61
    GPIO_SetupPinOptions(SCS_GPIO, GPIO_OUTPUT, GPIO_ASYNC);
    GPIO_SetupPinMux(SCS_GPIO,0,SCS_MUX);
    // set up /SAMPLE pin p18
    GPIO_SetupPinOptions(AD2S1210_SAMPLE, GPIO_OUTPUT, GPIO_ASYNC);
    GPIO_SetupPinMux(AD2S1210_SAMPLE,0,0);

    EDIS;
}

// spi_xmit - Transmit value via SPI
//void spi_xmit(Uint16 a)
//{
//    SpiaRegs.SPITXBUF = a;
//}

Uint16 spi_xmit(Uint16 data)
{
    GPIO_WritePin(SCS_GPIO, 0);               // Enable SCS or WR (the same pin)
    SpiaRegs.SPITXBUF = data;                     // send out the data
//    while(SpiaRegs.SPIFFRX.bit.RXFFST !=1) { }
    while(SpiaRegs.SPISTS.bit.INT_FLAG == 0);     // wait for the packet to complete
    data = SpiaRegs.SPIRXBUF;                     // data read to clear the INT_FLAG bit
    GPIO_WritePin(SCS_GPIO, 1);               // Disable SCS
    return(data);
}
//
// spi_fifo_init - Initialize SPIA FIFO
//
//void spi_fifo_init()
//{
//    // Initialize SPI FIFO registers
//    SpiaRegs.SPIFFTX.all = 0xE040;
//    SpiaRegs.SPIFFRX.all = 0x2044;
//    SpiaRegs.SPIFFCT.all = 0x0;
//    // Initialize core SPI registers
//    InitSpi();
//}

void AD2S1210_WR(Uint16 reg, Uint16 data) {
Uint16 dummy;
    GPIO_WritePin(SCS_GPIO, 0);               // Enable SCS or WR (the same pin)
    SpiaRegs.SPITXBUF = reg<<8;                     // send out the data
    while(SpiaRegs.SPISTS.bit.INT_FLAG == 0);    // wait for the packet to complete
    dummy = SpiaRegs.SPIRXBUF;                          // data read to clear the INT_FLAG bit
    GPIO_WritePin(SCS_GPIO, 1);               // Disable SCS
    //DELAY_US(0.01); //10ns
    GPIO_WritePin(SCS_GPIO, 0);
    SpiaRegs.SPITXBUF = data<<8;
    while(SpiaRegs.SPISTS.bit.INT_FLAG == 0);
    dummy = SpiaRegs.SPIRXBUF;
    GPIO_WritePin(SCS_GPIO, 1);
//  gpio_clear(AD2S1210_WRPIN_PORT, AD2S1210_WRPIN);
//  spi_write(AD2S1210_SPI, reg);
//  spi_read(AD2S1210_SPI);
//  gpio_set(AD2S1210_WRPIN_PORT, AD2S1210_WRPIN);
//  gpio_clear(AD2S1210_WRPIN_PORT, AD2S1210_WRPIN);
//  spi_write(AD2S1210_SPI, data);
//  spi_read(AD2S1210_SPI);
//  gpio_set(AD2S1210_WRPIN_PORT, AD2S1210_WRPIN);
}

Uint16 AD2S1210_RD(Uint16 reg) {
Uint16 dummy, data;
  GPIO_WritePin(SCS_GPIO, 0);               // Enable SCS or WR (the same pin)
  SpiaRegs.SPITXBUF = reg<<8;                     // send out the data
  while(SpiaRegs.SPISTS.bit.INT_FLAG == 0);    // wait for the packet to complete
  dummy = SpiaRegs.SPIRXBUF;                          // data read to clear the INT_FLAG bit
  GPIO_WritePin(SCS_GPIO, 1);               // Disable SCS
  //DELAY_US(0.01); //10ns
  GPIO_WritePin(SCS_GPIO, 0);
  SpiaRegs.SPITXBUF = 0x00;
  while(SpiaRegs.SPISTS.bit.INT_FLAG == 0);
  data = SpiaRegs.SPIRXBUF;
  GPIO_WritePin(SCS_GPIO, 1);
//  gpio_clear(AD2S1210_WRPIN_PORT, AD2S1210_WRPIN);
//  spi_write(AD2S1210_SPI, reg);
//  spi_read(AD2S1210_SPI);
//  gpio_set(SCS_GPIOPIN_PORT, AD2S1210_WRPIN);
//  gpio_clear(AD2S1210_WRPIN_PORT, AD2S1210_WRPIN);
//  spi_write(AD2S1210_SPI, 0x00);
//  data = (int)spi_read(AD2S1210_SPI); // Casting to int
//  gpio_set(AD2S1210_WRPIN_PORT, AD2S1210_WRPIN);
  return data;
}

void AD2S1210_SAMPLING() {
    GPIO_WritePin(AD2S1210_SAMPLE, 1);
    GPIO_WritePin(AD2S1210_SAMPLE, 0);
    DELAY_US(2);            // t16 = 2 × tCK + 20 ns min = 2 us pour tclk=1us
    GPIO_WritePin(AD2S1210_SAMPLE, 1);
    DELAY_US(4);            // t17-t16 = (6-2) × tCK ns min
    // can be avoided if the read is performed long after
    // now we can read

//  gpio_set(AD2S1210_SAMPLEPIN_PORT, AD2S1210_SAMPLEPIN);
//  gpio_clear(AD2S1210_SAMPLEPIN_PORT, AD2S1210_SAMPLEPIN);
//  DELAY_US(0.4);
//  gpio_set(AD2S1210_SAMPLEPIN_PORT, AD2S1210_SAMPLEPIN);
//  DELAY_US(0.8);
}

void ad2s1210_conf(void) {
//  default excitation frequency to 10 KHz on powerup
// Setting excitation frequency to 4.5KHz
Uint16 dummy;
    GPIO_WritePin(SCS_GPIO, 0);
    SpiaRegs.SPITXBUF = AD2S1210_REG_EXC_FREQ<<8;
    while(SpiaRegs.SPISTS.bit.INT_FLAG == 0);
    dummy = SpiaRegs.SPIRXBUF;
    GPIO_WritePin(SCS_GPIO, 1);
    DELAY_US(0.01);
    GPIO_WritePin(SCS_GPIO, 0);
//    SpiaRegs.SPITXBUF = 0x12<<8;    // 4.5KHz
    SpiaRegs.SPITXBUF = 0x28<<8;    // 10KHz
    while(SpiaRegs.SPISTS.bit.INT_FLAG == 0);
    dummy = SpiaRegs.SPIRXBUF;
    GPIO_WritePin(SCS_GPIO, 1);
    DELAY_US(0.01);

 // Setting resolution to 16bits
    GPIO_WritePin(SCS_GPIO, 0);
    SpiaRegs.SPITXBUF = AD2S1210_REG_CNTRL<<8;
    while(SpiaRegs.SPISTS.bit.INT_FLAG == 0);
    dummy = SpiaRegs.SPIRXBUF;
    GPIO_WritePin(SCS_GPIO, 1);
    //DELAY_US(0.01); //10ns
    GPIO_WritePin(SCS_GPIO, 0);
    SpiaRegs.SPITXBUF = 0x7F<<8; // 16 bits and 0x7E for 12 bit
    while(SpiaRegs.SPISTS.bit.INT_FLAG == 0);
    dummy = SpiaRegs.SPIRXBUF;
    GPIO_WritePin(SCS_GPIO, 1);
}


Uint16 AD2S1210_Read_Pos()
{
Uint16 dummy;
    GPIO_WritePin(SCS_GPIO, 0);
    SpiaRegs.SPITXBUF = AD2S1210_REG_POS_H<<8;     // ask High
    while(SpiaRegs.SPISTS.bit.INT_FLAG == 0);
    dummy = SpiaRegs.SPIRXBUF;                          // dummy read to clear the INT_FLAG bit
    GPIO_WritePin(SCS_GPIO, 1);
    //DELAY_US(0.01); //10ns
    GPIO_WritePin(SCS_GPIO, 0);
    SpiaRegs.SPITXBUF = AD2S1210_REG_POS_L<<8;
    while(SpiaRegs.SPISTS.bit.INT_FLAG == 0);
    dataH = SpiaRegs.SPIRXBUF;
    GPIO_WritePin(SCS_GPIO, 1);
    //DELAY_US(0.01); //10ns
    GPIO_WritePin(SCS_GPIO, 0);
    SpiaRegs.SPITXBUF = 0x00;
    while(SpiaRegs.SPISTS.bit.INT_FLAG == 0);
    dataL = SpiaRegs.SPIRXBUF;
    GPIO_WritePin(SCS_GPIO, 1);
    return (dataH << 8) | dataL;
}

int AD2S1210_Read_Vit()
{
Uint16 dummy;
    GPIO_WritePin(SCS_GPIO, 0);
    SpiaRegs.SPITXBUF = AD2S1210_REG_VEL_H<<8;     // ask High
    while(SpiaRegs.SPISTS.bit.INT_FLAG == 0);
    dummy = SpiaRegs.SPIRXBUF;                          // dummy read to clear the INT_FLAG bit
    GPIO_WritePin(SCS_GPIO, 1);
    //DELAY_US(0.01); //10ns
    GPIO_WritePin(SCS_GPIO, 0);
    SpiaRegs.SPITXBUF = AD2S1210_REG_VEL_L<<8;
    while(SpiaRegs.SPISTS.bit.INT_FLAG == 0);
    dataH = SpiaRegs.SPIRXBUF;
    GPIO_WritePin(SCS_GPIO, 1);
    //DELAY_US(0.01); //10ns
    GPIO_WritePin(SCS_GPIO, 0);
    SpiaRegs.SPITXBUF = 0x00;
    while(SpiaRegs.SPISTS.bit.INT_FLAG == 0);
    dataL = SpiaRegs.SPIRXBUF;
    GPIO_WritePin(SCS_GPIO, 1);
    return (dataH << 8) | dataL;
}
