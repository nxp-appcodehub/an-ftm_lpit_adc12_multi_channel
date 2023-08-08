# NXP Application Code Hub
[<img src="https://mcuxpresso.nxp.com/static/icon/nxp-logo-color.svg" width="100"/>](https://www.nxp.com)

## AN13437: Using KE17Z LPIT to Trigger ADC Multi-channel Conversion
<div><div style="--tw-border-spacing-x: 0; --tw-border-spacing-y: 0; --tw-translate-x: 0; --tw-translate-y: 0; --tw-rotate: 0; --tw-skew-x: 0; --tw-skew-y: 0; --tw-scale-x: 1; --tw-scale-y: 1; --tw-pan-x: ; --tw-pan-y: ; --tw-pinch-zoom: ; --tw-scroll-snap-strictness: proximity; --tw-ordinal: ; --tw-slashed-zero: ; --tw-numeric-figure: ; --tw-numeric-spacing: ; --tw-numeric-fraction: ; --tw-ring-inset: ; --tw-ring-offset-width: 0px; --tw-ring-offset-color: #fff; --tw-ring-color: rgba(59,130,246,.5); --tw-ring-offset-shadow: 0 0 #0000; --tw-ring-shadow: 0 0 #0000; --tw-shadow: 0 0 #0000; --tw-shadow-colored: 0 0 #0000; --tw-blur: ; --tw-brightness: ; --tw-contrast: ; --tw-grayscale: ; --tw-hue-rotate: ; --tw-invert: ; --tw-saturate: ; --tw-sepia: ; --tw-drop-shadow: ; --tw-backdrop-blur: ; --tw-backdrop-brightness: ; --tw-backdrop-contrast: ; --tw-backdrop-grayscale: ; --tw-backdrop-hue-rotate: ; --tw-backdrop-invert: ; --tw-backdrop-opacity: ; --tw-backdrop-saturate: ; --tw-backdrop-sepia: ; color: rgba(48, 48, 48, 0.8);">This demo shows the process of using LPIT to achieve FTM triggering the alternate conversion of four ADC channels in one PWM cycle.</div></div>

Please refer to AN13437 for complete instructions on how to use this software.

 https://www.nxp.com.cn/docs/en/application-note/AN13437.pdf.

In BLDC motor control application, users always use FTM to trigger ADC multiple channels convert alternately. 

However, the ADC of KE17Z does not support sampling in sequence, so it needs LPIT to provide alternate ADC hardware trigger source. TRGMUX implementation enables to realize ADC sampling in sequence in one PWM period. LPIT can also provide precise timing interval as the trigger delay between the conversion of ADC multi-channels.

The figure below shows the process of using LPIT to achieve FTM triggering the alternate conversion of four ADC channels in one PWM cycle:

![picture1](images/1.png)

#### Boards: FRDM-KE17Z
#### Categories: Industrial
#### Peripherals: ADC, PWM, LPIT
#### Toolchains: IAR

## Table of Contents
1. [Software](#step1)
2. [Hardware](#step2)
3. [Setup](#step3)
4. [Result](#result)
5. [Support](#step6)
6. [Release Notes](#step7)

## 1. Software<a name="step3"></a>

The development environment of example code is development on IAR embedded Workbench 9.10.2, based on SDK_2_10_1_FRDM-KE17Z.

The software flowchart of the using  LPIT to trigger ADC multi-channel conversion  is shown below:

![picture1](images/2.png)

The timing of LPIT0 output trigger/pre-trigger to ADC0:

![picture1](images/3.png)

## 2. Hardware<a name="step3"></a>

- Micro USB cable

- FRDM-KE17Z board.

- Personal Computer

  ![picture1](images/4.png)

​                                                                           FRDM-KE17Z board

## 3. Setup<a name="step3"></a>

1. Connect a USB cable between the host PC and the OpenSDA USB port on the target board.
2. Open a serial terminal with the following settings:
   - 115200 baud rate
   - 8 data bits
   - No parity
   - One stop bit
   - No flow control
3. Download the program to the target board.
4. Either press the reset button on your board or launch the debugger in your IDE to begin running the demo.

## 4. Result<a name="step6"></a> 

When the example runs successfully, you can see the similar information from the terminal as below:

*FTM0 trigger LPIT0, LPIT0 trigger four ADC channels conversion*

*ADC12 Conversion Completed, result value: 835,10,0,3015*



## 5. Support<a name="step6"></a>

For more details, please refer to AN13437 on NXP web.

#### Project Metadata
<!----- Boards ----->
[![Board badge](https://img.shields.io/badge/Board-FRDM&ndash;KE17Z-blue)](https://github.com/search?q=org%3Anxp-appcodehub+FRDM-KE17Z+in%3Areadme&type=Repositories)

<!----- Categories ----->
[![Category badge](https://img.shields.io/badge/Category-INDUSTRIAL-yellowgreen)](https://github.com/search?q=org%3Anxp-appcodehub+industrial+in%3Areadme&type=Repositories)

<!----- Peripherals ----->
[![Peripheral badge](https://img.shields.io/badge/Peripheral-ADC-yellow)](https://github.com/search?q=org%3Anxp-appcodehub+adc+in%3Areadme&type=Repositories) [![Peripheral badge](https://img.shields.io/badge/Peripheral-PWM-yellow)](https://github.com/search?q=org%3Anxp-appcodehub+pwm+in%3Areadme&type=Repositories) [![Peripheral badge](https://img.shields.io/badge/Peripheral-TIMER-yellow)](https://github.com/search?q=org%3Anxp-appcodehub+timer+in%3Areadme&type=Repositories)

<!----- Toolchains ----->
[![Toolchain badge](https://img.shields.io/badge/Toolchain-IAR-orange)](https://github.com/search?q=org%3Anxp-appcodehub+iar+in%3Areadme&type=Repositories)

Questions regarding the content/correctness of this example can be entered as Issues within this GitHub repository.

>**Warning**: For more general technical questions regarding NXP Microcontrollers and the difference in expected funcionality, enter your questions on the [NXP Community Forum](https://community.nxp.com/)

[![Follow us on Youtube](https://img.shields.io/badge/Youtube-Follow%20us%20on%20Youtube-red.svg)](https://www.youtube.com/@NXP_Semiconductors)
[![Follow us on LinkedIn](https://img.shields.io/badge/LinkedIn-Follow%20us%20on%20LinkedIn-blue.svg)](https://www.linkedin.com/company/nxp-semiconductors)
[![Follow us on Facebook](https://img.shields.io/badge/Facebook-Follow%20us%20on%20Facebook-blue.svg)](https://www.facebook.com/nxpsemi/)
[![Follow us on Twitter](https://img.shields.io/badge/Twitter-Follow%20us%20on%20Twitter-white.svg)](https://twitter.com/NXP)

## 6. Release Notes<a name="step7"></a>
| Version | Description / Update                           | Date                        |
|:-------:|------------------------------------------------|----------------------------:|
| 1.0     | Initial release on Application Code HUb        | June 5<sup>th</sup> 2023 |

