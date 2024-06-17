# NXP Application Code Hub
[<img src="https://mcuxpresso.nxp.com/static/icon/nxp-logo-color.svg" width="100"/>](https://www.nxp.com)

## Nafe Industrial Application Examples using API Libraries

This is a collection of MCUXpresso projects, in this collection several fundamental voltage/current/sensor readout applications via NXP analog front-end are exposed.

The examples contained in the collection are 7, below is a brief description:
* **Current Sensing**: Reading a current across 0.25ohm shunt between AI4P-AI4N using *Single Channel Single Reading* conversion.
* **Voltage Sensing**: Reading multiple voltages (Single Ended and Differential) on different Analog Input using *Single Channel Single Reading* conversion.
* **Voltage Sensing *(MCMR)***: Reading multiple voltages (Single Ended and Differential) on different Analog Input using *Multi Channels Multi Readings* conversion.
* **RTD 2/3/4 Wires**: Reading a voltage across an RTD Thermal Sensor excited by the internal VIEX, the conversion is performed using *Single Channel Continuous Readings* conversion.
* **Weight Scale *(Load Cell)***: Calibrate procedure + Reading procedure of a load cell, the load cell excitation is generated thanks to the NAFE integrated VIEX, the conversion is performed using Single Channel Continuous Readings conversion.

#### Boards: LPCXpresso54628, LPCXpresso54S018
#### Categories: Sensor, Industrial
#### Peripherals: GPIO, SPI, TIMER, UART, SENSOR, DMA
#### Toolchains: MCUXpresso IDE

## Table of Contents
1. [Software](#step1)
2. [Hardware](#step2)
3. [Setup](#step3)
4. [Results](#step4)
5. [FAQs](#step5) 
6. [Support](#step6)
7. [Release Notes](#step7)

## 1. Software<a name="step1"></a>
This demo is built based on NXP MCUXpresso SDK. All required source files to run the demo are contained in this repository.

The required softwares are:
* [MCUXpresso IDE v11.7.0](https://nxp.com/mcuxpresso)

## 2. Hardware<a name="step2"></a>
The needed Hardware are:
* Power supply of 15V,-15V and 3.3V or 24V power adapter.
* USB Cable.
* [LPCXpresso54628](https://www.nxp.com/design/design-center/software/development-software/mcuxpresso-software-and-tools-/lpcxpresso-boards/lpcxpresso54628-development-board:OM13098) or [LPCXpresso54s018](https://www.nxp.com/products/processors-and-microcontrollers/arm-microcontrollers/general-purpose-mcus/lpc54000-arm-cortex-m4-/lpcxpresso54s018-development-board:LPC54S018-EVK) Development Boards.
	- <img src="/Image/LPCxpresso54s018.png" alt="drawing" width="300"/>
* [NXP_NAFExxx388_EVB](https://www.nxp.com/design/design-center/development-boards-and-designs/analog-toolbox/nafexx388-evb-8-channels-universal-input-afe-evaluation-board:NAFExx388-EVB)
	- <img src="/Image/NAFE_EVK.png" alt="drawing" width="300"/>
* RTD Sensor (Example *RTD 2/3/4 Wires*).
* 250 ohm resistor (Example *Current Sensing*).
* Load Cell (Example *Weight Scale*).
* External sources in order to try different voltage readings (Example *Voltage Sensing*).

## 3. Setup<a name="step3"></a>
In order to have a complete view on the project code and sensor reading please refer on the follow Application Note:
* [AN14102](https://www.nxp.com/docs/en/application-note/AN14102.pdf) Industrial application measurements using NXP AFE.
* [AN14103](https://www.nxp.com/docs/en/application-note/AN14103.pdf) NAFE applications with MCUXpresso.

Common steps to follow for all the examples are described as follows.

### 3.1 Step 1
The hardware setup (NXP_NAFExxx388_EVB) changes depending on the power supply mode chosen:

* Power Adaptor -> J78A, J78B, J78C jumpers position 2-3
* External Voltage Source -> J78A, J78B, J78C jumpers position 1-2

Instead the jumpers J78 and J79 must be closed, in all configuration.

### 3.2 Step 2
Start debugging the choosen project.

### 3.3 Step 3
Configure the integrate terminal console with the follow configuration:

* Baud rate = 115200
* Data size = 8
* Parity = None
* Stop bits = 1

## 4. Results<a name="step4"></a>
For all results, for each of the examples in this repository please refer to [AN14103](https://www.nxp.com/docs/en/application-note/AN14103.pdf).

Here an example of console output for Voltage Sensing Example.

<img src="/Image/Console_output.png" alt="drawing" width="600"/>

Where is reported the time elapsed and results for each channels, in order to request a new conversion it's needed to click any button.


## 5. FAQs<a name="step5"></a>
No FAQs have been identified for this project.

## 6. Support<a name="step6"></a>
* [NAFEx3388 Highly Configurable 8 Channel ±25 V Universal Input Analog Front-End with Excitation Sources](https://www.nxp.com/products/analog-and-mixed-signal/analog-front-end/highly-configurable-8-channel-25-v-universal-input-analog-front-end-with-excitation-sources:NAFEx3388)
#### Project Metadata
<!----- Boards ----->
[![Board badge](https://img.shields.io/badge/Board-LPCXPRESSO54628-blue)](https://github.com/search?q=org%3Anxp-appcodehub+LPCXpresso54628+in%3Areadme&type=Repositories) [![Board badge](https://img.shields.io/badge/Board-LPCXPRESSO54S018-blue)](https://github.com/search?q=org%3Anxp-appcodehub+LPCXpresso54S018+in%3Areadme&type=Repositories)

<!----- Categories ----->
[![Category badge](https://img.shields.io/badge/Category-SENSOR-yellowgreen)](https://github.com/search?q=org%3Anxp-appcodehub+sensor+in%3Areadme&type=Repositories) [![Category badge](https://img.shields.io/badge/Category-INDUSTRIAL-yellowgreen)](https://github.com/search?q=org%3Anxp-appcodehub+industrial+in%3Areadme&type=Repositories)

<!----- Peripherals ----->
[![Peripheral badge](https://img.shields.io/badge/Peripheral-GPIO-yellow)](https://github.com/search?q=org%3Anxp-appcodehub+gpio+in%3Areadme&type=Repositories) [![Peripheral badge](https://img.shields.io/badge/Peripheral-SPI-yellow)](https://github.com/search?q=org%3Anxp-appcodehub+spi+in%3Areadme&type=Repositories) [![Peripheral badge](https://img.shields.io/badge/Peripheral-TIMER-yellow)](https://github.com/search?q=org%3Anxp-appcodehub+timer+in%3Areadme&type=Repositories) [![Peripheral badge](https://img.shields.io/badge/Peripheral-UART-yellow)](https://github.com/search?q=org%3Anxp-appcodehub+uart+in%3Areadme&type=Repositories) [![Peripheral badge](https://img.shields.io/badge/Peripheral-SENSOR-yellow)](https://github.com/search?q=org%3Anxp-appcodehub+sensor+in%3Areadme&type=Repositories) [![Peripheral badge](https://img.shields.io/badge/Peripheral-DMA-yellow)](https://github.com/search?q=org%3Anxp-appcodehub+dma+in%3Areadme&type=Repositories)

<!----- Toolchains ----->
[![Toolchain badge](https://img.shields.io/badge/Toolchain-MCUXPRESSO%20IDE-orange)](https://github.com/search?q=org%3Anxp-appcodehub+mcux+in%3Areadme&type=Repositories)

Questions regarding the content/correctness of this example can be entered as Issues within this GitHub repository.

>**Warning**: For more general technical questions regarding NXP Microcontrollers and the difference in expected funcionality, enter your questions on the [NXP Community Forum](https://community.nxp.com/)

[![Follow us on Youtube](https://img.shields.io/badge/Youtube-Follow%20us%20on%20Youtube-red.svg)](https://www.youtube.com/@NXP_Semiconductors)
[![Follow us on LinkedIn](https://img.shields.io/badge/LinkedIn-Follow%20us%20on%20LinkedIn-blue.svg)](https://www.linkedin.com/company/nxp-semiconductors)
[![Follow us on Facebook](https://img.shields.io/badge/Facebook-Follow%20us%20on%20Facebook-blue.svg)](https://www.facebook.com/nxpsemi/)
[![Follow us on Twitter](https://img.shields.io/badge/Twitter-Follow%20us%20on%20Twitter-white.svg)](https://twitter.com/NXP)

## 7. Release Notes<a name="step7"></a>
| Version | Description / Update                           | Date                        |
|:-------:|------------------------------------------------|----------------------------:|
| 1.0     | Initial release on Application Code Hub        | June 13<sup>th</sup> 2024 |

