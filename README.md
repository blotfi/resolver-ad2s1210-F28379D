# resolver-ad2s1210-F28379D
resolver ad2s1210 code for Texas Instruments TMS F28379D
These code interface the F28379D to an AD2S1210 board using SPI (Serial Peripheral Interface)



![top board](img/board2.png)
![bottom board](img/board1.png)
![image](https://github.com/blotfi/resolver-ad2s1210-F28379D/assets/24873186/b02a7d12-96eb-44f2-bc8e-8061546d66f6)
![image](https://github.com/blotfi/resolver-ad2s1210-F28379D/assets/24873186/7cd88ed2-6137-440f-8ba6-17634e4881cd)



For 8-bit data, you need to shift left (LSL) by 8 bits before sending it to SPITXBUF = data << 8;
While reading from SPIRXBUF is aligned to the least significant bit (LSB)
timing
SCLK 1 us donc fclk= 1 MHz, donc 
t16=2*1us+20ns = 2.02 us
t17=6*1us+20ns = 6.02 us

![image](https://github.com/blotfi/resolver-ad2s1210-F28379D/assets/24873186/541c8f26-475d-431e-a8f9-18830cfbcd0c)
Even if it is 10 kHz, I noticed that we must configure it (see '''void ad2s1210_conf(void)''')
#define  SDI_GPIO       58
#define  SDO_GPIO       59
#define  CLK_GPIO       60
#define  CLK_MUX        15
#define  SCS_GPIO        61 // /WR c'est le /CS de la SPI
#define  AD2S1210_WR     SCS_GPIO
#define  AD2S1210_SAMPLE 18
![image](https://github.com/blotfi/resolver-ad2s1210-F28379D/assets/24873186/0249365f-7da3-4aa3-bce9-ab87e8709618)
![image](https://github.com/blotfi/resolver-ad2s1210-F28379D/assets/24873186/dbed6a89-b4a4-4dad-a48c-0f6c866bd8ee)
![image](https://github.com/blotfi/resolver-ad2s1210-F28379D/assets/24873186/c6e20a08-2d93-4c9b-90fd-0fc4d7c3e1db)
![image](https://github.com/blotfi/resolver-ad2s1210-F28379D/assets/24873186/3db789f8-4f4b-4769-bd3f-909ac0e6714e)
![image](https://github.com/blotfi/resolver-ad2s1210-F28379D/assets/24873186/0b8555a2-06c1-4840-837f-bcc7921ded2f)

Faults than can occur
![image](https://github.com/blotfi/resolver-ad2s1210-F28379D/assets/24873186/3fbcafc7-7256-40a6-a3da-2432445f258f)
For example if the USB power is not enough, I started having:
ad2s1210_fault
0x08 LOT
0x68 LOT LOS DOS
0x60 LOS DOS
0x00 No error


special thanks to https://github.com/arcoslab/omni-base/tree/master/baldor_control
for their code for STM
