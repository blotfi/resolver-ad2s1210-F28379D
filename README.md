# resolver-ad2s1210-F28379D
resolver ad2s1210 code for Texas Instruments TMS F28379D

This code interface the F28379D to an AD2S1210 board using SPI (Serial Peripheral Interface)

![image](https://github.com/blotfi/resolver-ad2s1210-F28379D/assets/24873186/5845176b-20b5-4692-8104-f86653ddf8e3)
![image](https://github.com/blotfi/resolver-ad2s1210-F28379D/assets/24873186/d6ee8b42-6b69-4d23-beb2-9537433a9469)

The AD2S1210 create an excitation signal to be sent to the resolver and decode the sin/cos signals to an absolute position
and also gives the rotational speed

![top board](img/board2.png)
![bottom board](img/board1.png)
![image](https://github.com/blotfi/resolver-ad2s1210-F28379D/assets/24873186/b02a7d12-96eb-44f2-bc8e-8061546d66f6)
![image](https://github.com/blotfi/resolver-ad2s1210-F28379D/assets/24873186/7cd88ed2-6137-440f-8ba6-17634e4881cd)

Here are some tips of connection and configuration

PCS is not a positive Chip Select, it is an /CS so leave it NC or to GND

You need to set A0, A1, RESET at 3.3V

A0 and A1 logical high means that the device is in configuration mode, we can configure it (excitation frequency, resolution)
but also read the data

**Control Register**

![image](https://github.com/blotfi/resolver-ad2s1210-F28379D/assets/24873186/d590fbc4-58e5-4601-8568-f551a7fc39b0)

To set the resolution, we need to send on SPI :
```
SpiaRegs.SPITXBUF = AD2S1210_REG_CNTRL<<8;
SpiaRegs.SPITXBUF = 0x7F<<8; // 16 bits and 0x7E for 12 bit
```
this is done with the correct SPI /CS and timing:
```
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
```

Sample must be put Low than High in order to do the conversion
```
void AD2S1210_SAMPLING() {
    GPIO_WritePin(AD2S1210_SAMPLE, 1);
    GPIO_WritePin(AD2S1210_SAMPLE, 0);
    DELAY_US(2);            // t16 = 2 × tCK + 20 ns min = 2 us pour tclk=1us
    GPIO_WritePin(AD2S1210_SAMPLE, 1);
    DELAY_US(4);            // t17-t16 = (6-2) × tCK ns min
    // can be avoided if the read is performed long after
    // now we can read
}
 ```
Then we can read the position and/or the velocity and the error registers.

![image](https://github.com/blotfi/resolver-ad2s1210-F28379D/assets/24873186/15e36acd-991f-450b-bcc5-524aedd0725c)

For 8-bit data, you need to shift left (LSL) by 8 bits before sending it to SPITXBUF = data << 8;

While reading from SPIRXBUF is aligned to the least significant bit (LSB)

timing:
```
SCLK 1 us (fclk= 1 MHz)
t16=2*1us+20ns = 2.02 us
t17=6*1us+20ns = 6.02 us
```
![image](https://github.com/blotfi/resolver-ad2s1210-F28379D/assets/24873186/541c8f26-475d-431e-a8f9-18830cfbcd0c)

Even if it is 10 kHz, I noticed that we must configure it, see `void ad2s1210_conf(void);`
```#define  SDI_GPIO       58
#define  SDO_GPIO       59
#define  CLK_GPIO       60
#define  CLK_MUX        15
#define  SCS_GPIO        61 // /WR c'est le /CS de la SPI
#define  AD2S1210_WR     SCS_GPIO
#define  AD2S1210_SAMPLE 18
```
![image](https://github.com/blotfi/resolver-ad2s1210-F28379D/assets/24873186/0249365f-7da3-4aa3-bce9-ab87e8709618)
![image](https://github.com/blotfi/resolver-ad2s1210-F28379D/assets/24873186/dbed6a89-b4a4-4dad-a48c-0f6c866bd8ee)
![image](https://github.com/blotfi/resolver-ad2s1210-F28379D/assets/24873186/c6e20a08-2d93-4c9b-90fd-0fc4d7c3e1db)
![image](https://github.com/blotfi/resolver-ad2s1210-F28379D/assets/24873186/3db789f8-4f4b-4769-bd3f-909ac0e6714e)
![image](https://github.com/blotfi/resolver-ad2s1210-F28379D/assets/24873186/0b8555a2-06c1-4840-837f-bcc7921ded2f)

Faults than can occur

![image](https://github.com/blotfi/resolver-ad2s1210-F28379D/assets/24873186/3fbcafc7-7256-40a6-a3da-2432445f258f)

For example if the USB power is not enough, I started having:
```ad2s1210_fault
0x08 LOT
0x68 LOT LOS DOS
0x60 LOS DOS
0x00 No error
```

special thanks to https://github.com/arcoslab/omni-base/tree/master/baldor_control
for their code for STM
