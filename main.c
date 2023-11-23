// PWM 3-phase + ADC ISR EOC, synch GPIO32
//
// L. BAGHLI 08/11/2022

#include "F28x_Project.h"
#include "ad2s1210.h"

volatile int countPos = 0, OldPos = 0, tmrCount = 0;
volatile int raw_pos = 0, raw_pos_last = 0, vit = 0;
volatile int ad2s1210_fault = 0xFF;

Uint16 sdata;  // send data
Uint16 rdata;  // received data

void Gpio_setup();
void delay_loop(void);
Uint16 spi_xmit(Uint16 data);
void spi_fifo_init(void);
void InitSpiaGpio();

// Gpio_setup1 -Example 1: Basic Pinout
void Gpio_setup()
{
  EALLOW;
        // Configuration des broches de quadrature encoder
    GpioCtrlRegs.GPAMUX2.bit.GPIO20 = 1;    // Configure GPIO20 as EQEP1A
    GpioCtrlRegs.GPAMUX2.bit.GPIO21 = 1;    // Configure GPIO21 as EQEP1B
    // GpioCtrlRegs.GPAMUX2.bit.GPIO22 = 1;    // Configure GPIO22 as EQEP1S
    GpioCtrlRegs.GPAMUX2.bit.GPIO23 = 1;    // Configure GPIO23 as EQEP1I
    GpioCtrlRegs.GPAPUD.bit.GPIO20 = 1;    // Disable pull-up on GPIO20 (EQEP1A)
    GpioCtrlRegs.GPAPUD.bit.GPIO21 = 1;    // Disable pull-up on GPIO21 (EQEP1B)
    // GpioCtrlRegs.GPAPUD.bit.GPIO22 = 1;    // Disable pull-up on GPIO22 (EQEP1S)
    GpioCtrlRegs.GPAPUD.bit.GPIO23 = 1;    // Disable pull-up on GPIO23 (EQEP1I)
    GpioCtrlRegs.GPAQSEL2.bit.GPIO20 = 0;   // Sync GPIO20 to SYSCLK  (EQEP1A)  ou 2;  // Configure GPIO20 (EQEP1A) as a qualifying input 
    GpioCtrlRegs.GPAQSEL2.bit.GPIO21 = 0;   // Sync GPIO21 to SYSCLK  (EQEP1B)
    // GpioCtrlRegs.GPAQSEL2.bit.GPIO22 = 0;   // Sync GPIO22 to SYSCLK  (EQEP1S)
    GpioCtrlRegs.GPAQSEL2.bit.GPIO23 = 0;   // Sync GPIO23 to SYSCLK  (EQEP1I)

  // Enable PWM1-3 on GPIO0-GPIO5
  GpioCtrlRegs.GPAPUD.bit.GPIO0 = 1;    // Disable pull-up on GPIO0 (EPWM1A)
  GpioCtrlRegs.GPAPUD.bit.GPIO1 = 1;    // Disable pull-up on GPIO1 (EPWM1B)
  GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1;   // Configure GPIO0 as EPWM1A
  GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 1;   // Configure GPIO1 as EPWM1B

  GpioCtrlRegs.GPAPUD.bit.GPIO2 = 1;    // Disable pull-up on GPIO2 (EPWM2A)
  GpioCtrlRegs.GPAPUD.bit.GPIO3 = 1;    // Disable pull-up on GPIO3 (EPWM2B)
  GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 1;   // Configure GPIO2 as EPWM2A
  GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 1;   // Configure GPIO3 as EPWM2B

  GpioCtrlRegs.GPAPUD.bit.GPIO4 = 1;    // Disable pull-up on GPIO4 (EPWM3A)
  GpioCtrlRegs.GPAPUD.bit.GPIO5 = 1;    // Disable pull-up on GPIO5 (EPWM3B)
  GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 1;   // Configure GPIO4 as EPWM3A
  GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 1;   // Configure GPIO5 as EPWM3B

  GpioCtrlRegs.GPBPUD.bit.GPIO32 = 1;     // Disable pull-up on GPIO32
  GpioCtrlRegs.GPBMUX1.bit.GPIO32 = 0;    // Configure GPIO32 as GPIO
  GpioCtrlRegs.GPBDIR.bit.GPIO32 = 1;     // GPIO32 = output
  GpioDataRegs.GPBCLEAR.bit.GPIO32 = 1;   // Clear

// utilis√© par QEP1  GpioCtrlRegs.GPAPUD.bit.GPIO22 = 1;     // Disable pull-up on GPIO22
//   GpioCtrlRegs.GPAMUX2.bit.GPIO22 = 0;    // Configure GPIO22 as GPIO
//   GpioCtrlRegs.GPADIR.bit.GPIO22 = 1;     // GPIO22 = output
//   GpioDataRegs.GPACLEAR.bit.GPIO22 = 1;   // Clear

//  GpioCtrlRegs.GPDPUD.bit.GPIO124 = 1;     // Disable pull-up on GPI124 MOTOR1_EN_GATE_GPIO
//  GpioCtrlRegs.GPDMUX2.bit.GPIO124 = 0;    // Configure GPI124 as GPIO
//  GpioCtrlRegs.GPDDIR.bit.GPIO124 = 1;     // GPIO32 = output
//  GpioDataRegs.GPDCLEAR.bit.GPIO124 = 1;   // Clear // Disable EN_GATE
//
//  GpioCtrlRegs.GPDPUD.bit.GPIO125 = 1;     // Disable pull-up on GPI125 MOTOR1_WAKE_GPIO pour F28769D
//  GpioCtrlRegs.GPDMUX2.bit.GPIO125 = 0;    // Configure GPI124 as GPIO
//  GpioCtrlRegs.GPDDIR.bit.GPIO125 = 1;     // GPI32 = output
//  GpioDataRegs.GPDCLEAR.bit.GPIO125 = 1;   // Clear // Disable EN_GATE
//
//  GpioCtrlRegs.GPAPUD.bit.GPIO19 = 1;     // Disable pull-up on GPI19 FAULT pour F28769D
//  GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 0;    // Configure GPI124 as GPIO
//  GpioCtrlRegs.GPADIR.bit.GPIO19 = 0;     // GPIO19 = input
//  GpioDataRegs.GPACLEAR.bit.GPIO19 = 1;   // Clear // Disable EN_GATE

/*
    // Configure as Output : Motor 1 - EN-GATE
    GPIO_WritePin(MOTOR1_EN_GATE_GPIO, 0);  // Disable EN_GATE
    GPIO_SetupPinOptions(MOTOR1_EN_GATE_GPIO, GPIO_OUTPUT, GPIO_ASYNC);
    GPIO_SetupPinMux(MOTOR1_EN_GATE_GPIO, 0, MOTOR1_EN_GATE_MUX);

    // Configure as Output : Motor 1 - WAKE for DRV8305
    GPIO_WritePin(MOTOR1_WAKE_GPIO, 0);  // Set WAKE to 0 (default state)
    GPIO_SetupPinOptions(MOTOR1_WAKE_GPIO, GPIO_OUTPUT, GPIO_ASYNC);
    GPIO_SetupPinMux(MOTOR1_WAKE_GPIO, 0, MOTOR1_WAKE_MUX);

    // Configure as Input - Motor 1 - FAULT - (active low), trip PWM based on this
    GPIO_SetupPinOptions(MOTOR1_FAULT_GPIO, GPIO_INPUT, GPIO_INVERT);
    GPIO_SetupPinMux(MOTOR1_FAULT_GPIO, 0, MOTOR1_FAULT_MUX);

    // SPI GPIO MUX pins config
    // set up SDI pin
    GPIO_SetupPinOptions(MOTOR1_SDI_GPIO, GPIO_INPUT, GPIO_ASYNC);
    GPIO_SetupPinMux(MOTOR1_SDI_GPIO,0,MOTOR1_SDI_MUX);

    // set up SDO pin
    GPIO_SetupPinOptions(MOTOR1_SDO_GPIO, GPIO_INPUT, GPIO_ASYNC);
    GPIO_SetupPinMux(MOTOR1_SDO_GPIO,0,MOTOR1_SDO_MUX);

    // set up CLK pin
    GPIO_SetupPinOptions(MOTOR1_CLK_GPIO, GPIO_INPUT, GPIO_ASYNC);
    GPIO_SetupPinMux(MOTOR1_CLK_GPIO,0,MOTOR1_CLK_MUX);

    // set up SCS pin
    GPIO_SetupPinOptions(MOTOR1_SCS_GPIO, GPIO_OUTPUT, GPIO_ASYNC);
    GPIO_SetupPinMux(MOTOR1_SCS_GPIO,0,MOTOR1_SCS_MUX);
*/
  EDIS;
}

//
// delay_loop - Loop for a brief delay
//
void delay_loop()
{
    long i;
    for (i = 0; i < 1000000; i++) {}
}

__interrupt void timerISR(void)
{
    tmrCount++;
    // Lire la position actuelle du codeur ‡ intervalles rÈguliers
    uint32_t position = raw_pos;
    countPos += position - OldPos;
    OldPos = position;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

void main(void)
{

   InitSysCtrl();
// Step 2. Initialize GPIO:
// This example function is found in the F2837xD_Gpio.c file and
// illustrates how to set the GPIO to it's default state.
// Setup only the GP I/O only for SPI-A functionality
// This function is found in F2837xD_Spi.c
//
   InitSpiaGpio();

//
   DINT;
   InitPieCtrl();
   IER = 0x0000;
   IFR = 0x0000;
   InitPieVectTable();
   // Step 4. Initialize the Device Peripherals:
   //spi_fifo_init();     // Initialize the SPI FIFO
   InitSpi();

   EALLOW;
   PieVectTable.TIMER0_INT = &timerISR;  // Pointer vers la fonction timerISR
   PieCtrlRegs.PIEIER1.bit.INTx7 = 1;    // Activer l'interruption TIMER0_INT
   EDIS;
   IER |= M_INT1;
   ConfigCpuTimer(&CpuTimer0, 200, 100000);
   CpuTimer0Regs.TCR.all = 0x4000;
   EINT;   // Enable Global __interrupt INTM
   ERTM;   // Enable Global realtime __interrupt DBGM

   sdata = 0x0000;
   while(1)
       {
//       rdata = spi_xmit(sdata<<8);
//       delay_loop();
//       sdata++;
       AD2S1210_SAMPLING();
       raw_pos=AD2S1210_Read_Pos();
       vit=AD2S1210_Read_Vit();
       ad2s1210_fault=AD2S1210_RD(AD2S1210_REG_FAULT);
       DELAY_US(100);
       }
}
