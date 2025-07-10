# Interconnected General Purpose Microcontroller with Dual Core Design CH32H417
EN | [中文](README_zh.md)



### Overview
CH32H417 is an interconnected general-purpose microcontroller designed based on barley RISC-V5F and RISC-V3F dual cores. CH32H417 integrates USB 3.2 Gen1 controller and transceiver, 100 Gigabit Ethernet MAC and PHY, SerDes high-speed isolation transceiver, Type-C/PD controller and PHY, provides SD/EMMC controller, 500MBytes universal high-speed interface UHSIF, DVP digital image interface, single-wire protocol master interface SWPMI, flexible storage controller FIOC, programmable protocol I/O controller PIOC. Controller, 500MBytes Universal High Speed Interface UHSIF, DVP Digital Video Interface, Single Wire Protocol Master Interface SWPMI, Programmable Protocol I/O Controller PIOC, Flexible Memory Controller FMC, DFSDM, LTDC, GPHA, DMA Controller, Multiple Timers, 8 Serial, I3C, 4 I2C, 2 QSPI, 4 SPI, 2 I2S, 4 QSPI, 4 SPI, 4 QSPI, 4 I2S, 2 I2S, 4 I2S, 4 I2S, 4 I2S, 4 I2S, 4 I2S, 4 I2S, 4 I2S, 4 I2S, 4 I2S. SPI, 2-group I2S, 3-group CAN and other peripheral resources, built-in dual 12-bit ADC units with 5M sampling rate, 10-bit high-speed HSADC units with 20M sampling rate, 16-channel Touchkey, dual DAC units, 3-group op-amps (OPA), voltage comparator (CMP), and other analog resources, support 10M/100M Ethernet communication, support USB 2.0 and USB 3.0, support USB Host Host and USB Device, support USB 2.0 and USB 3.0, support USB 2.0 and USB 3.0, and support USB Host and USB Device. It supports USB 2.0 and USB 3.0, USB Host and USB Device, Type-C and PDUSB fast charging, SerDes high-speed isolation and long-distance transmission, and dual-core division of labor to improve the efficiency of network protocol processing and communication response speed.


### System Block Diagram
![Alt](image/frame.png)
### Features

- Dual Core Architecture: Barley RISC-V5F and RISC-V3F

- Fast programmable interrupt controller + hardware interrupt stack

- V5F up to 400MHz, V3F up to 150MHz

- 896KB volatile data storage area SRAM (including 128KB ITCM and 256KB DTCM)
- 960KB program storage area CodeFlash
- 56KB system boot program storage area BootLoader
- 256B user-defined information storage area

- System power supply VDD33 rated: 3.3V
- Regular GPIO power supply VDDIO, rated 3.3V, supports 1.8V
- High-speed GPIO power supply VIO18, selectable 1.2/1.8/2.5/3.3V
- VBAT power supply independently powering RTC, LSE
- Low power consumption modes: sleep, stop

- Built-in factory-tuned 25MHz RC oscillator
- Built-in RC oscillator of about 40kHz
- High-speed oscillator supports external 25MHz crystal.
- Low-speed oscillator supports external 32kHz crystal
- Power-up/down reset, programmable voltage monitor

- 2 groups of 16 DMA channels: support ring buffer management

- 2 sets of 12-bit analog-to-digital ADC: analog input range: VSS~VDDIO, 
  16 external signal channels + 2 internal signal channels, 
  Sampling rate up to 5Msps, support dual ADC conversion mode

- 16-channel TouchKey channel detection

- 1 group 10-bit high-speed analog-to-digital HSADC, analog input range: VSS ~ VDDIO 
  7 external signal channels, sampling rate up to 20Msps

- 2 groups of 12-bit digital to analog DACs

- 32-bit width 125MHz general-purpose high-speed interface UHSIF

- 150MHz digital video interface DVP

- 200MHz dual-edge SD/EMMC controller (SDMMC)

- SDIO master/slave interface: supports SD/SDIO/MMC port

- Single-wire protocol master interface SWPMI

- Programmable Protocol I/O Controller PIOC, programmable, support a variety of single-wire interfaces, two-wire interfaces 
  Support single-wire RGB chip multi-level serial connection

- Ethernet controller MAC and 10M/100M PHY: 
  MAC and 100M PHY fully integrated, peripheral only need capacitors, 
  support Auto-MDIX line automatic conversion and polarity self-adaptation, 
  provide RGMII interface, can be connected to external 1000M PHY, 
  built-in pre-assigned globally unique MAC address

- 5Gbps SuperSpeed USB 3.0 controller and PHY: 
  supports SuperSpeed Host and Device modes, 
  supports driving USB 3.0 HUB, 
  high-speed integrated design, measured 450Mbytes per second.

- 480Mbps Hi-Speed USB 2.0 Controller and PHY: 
  Supports high speed/full speed Host and Device modes, 
  Supports 1024 byte packets, 
  Supports USART serial port or I2C pin mapping.

- Full Speed USB 2.0 Controller and PHY: 
  Supports full speed/low speed Host and Device modes, 
 Supports OTG function.

- Remote SerDes Controller and PHY: 
  supports high voltage signal isolation transmission at kilovolt level, 
  supports 100 meters differential network cable remote transmission.

- Random Number Generator RNG

- USB PD and Type-C controllers and PHYs: 
  supports DRP, Sink and Source applications, PDUSB, 
  supports PD3.2 and EPR, 100W or 240W fast charging.

- Analog Voltage Comparator CMP: 
  2 input channels, output to peripherals or I/Os.

- 3 Op Amps OPA/PGA/Voltage Comparator: 
  Multiple input channels and output channels, 
  Low dropout voltage, multiple gain steps, support high speed mode.

- Multiple timers: 
  2 16-bit advanced timers, 
  4 16-bit and 4 32-bit general-purpose timers, 
  2 16-bit basic timers, 
  2 16-bit low-power timers, 
  2 watchdog timers: stand-alone and windowed, 
  2 32-bit system time base timers.

- Real-time clock RTC: 32-bit independent timer

- 8 groups of USART serial interface: support LIN

- 4 groups of I2C interface

- I3C interface

- 4 SPI interface (SPI2, SPI3 for I2S1, I2S2)

- 2 QuadSPI interfaces

- 3 CAN interfaces (2.0B active)

- Digital filter for sigma delta modulator DFSDM

- Serial audio interface SAI

- LCD-TFT display controller LTDC

- Graphics processing hardware gas pedal GPHA

- Flexible Memory Controller FMC: 
  Supports FSMC interface and SDRAM interface, 
  Supports external low cost PSRAM

- Fast GPIO ports: 
  6 sets of GPIO ports, 95 I/O ports, 
  maps 16 external interrupts.

- ECDC encryption module: 
  supports AES128/192/256 algorithm, 
  supports SM4 algorithm.

- Debug Mode: 
  supports both single-wire (default) and dual-wire debug modes.

- Package: QFN

