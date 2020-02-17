# CE228938 - PWM GPIO
This example demonstrates the configuration of PWM (pulse width modulation) to control the brightness of an LED, and configuration of GPIO to toggle an LED ON and OFF using ModusToolbox™ integrated development environment (IDE). This application also demonstrates how to invert PWM signals which would be helpful for applications like motor control. 

## Requirements
- **Tool**: [ModusToolbox™ IDE](https://www.cypress.com/products/modustoolbox-software-environment) v2.x
- **Programming Language**: C
- **Associated Parts**: [CYW20819](https://www.cypress.com/datasheet/CYW20819), [CYW20820](https://www.cypress.com/datasheet/CYW20820), [CYW20719](https://www.cypress.com/documentation/datasheets/cyw20719-enhanced-low-power-bredrble-bluetooth-50-soc)


## Supported Kits
-  [CYW920819EVB-02 Evaluation Kit](http://www.cypress.com/CYW920819EVB-02) 
-  [CYW920820EVB-02 Evaluation kit](http://www.cypress.com/CYW920820EVB-02)  
-  [CYW920719B2Q40EVB-01 Evaluation kit](https://community.cypress.com/docs/DOC-17736)

## Hardware Setup
This example uses the board's default configuration. Refer to the kit user guide to ensure that the board is configured correctly.


## Software Setup
Install a terminal emulator if you don't have one. Instructions in this document use [Tera Term](https://ttssh2.osdn.jp/index.html.en).
This example requires no additional software or tools.

## Using the Code Example

### In ModusToolbox IDE:

1. Install ModusToolbox 2.x.

2. In the ModusToolbox IDE, click the **New Application** link in the Quick Panel (or, use **File** > **New** > **ModusToolbox IDE Application**).

3. Select the target board supported by the code example from the list shown in the **IDE Application** dialog (Board Support Package) and click **Next**.

4. Select **wiced\_btsdk**. This project contains the SDK, which is used by all BTSDK applications. You will need to create this project just once in the working directory (i.e., Eclipse workspace). Ignore if you already created this project earlier.
    
    **Note**: Do not change the name of this project. All BTSDK apps use this project name in application makefiles.

5. After the 'wiced\_btsdk' project is created, click the **New Application** link again, and select the board and click **Next**.

6. Select the 'PWM GPIO' application and click **Next**. Click **Finish** to create the application.

7. Select the application in the IDE. In the Quick Panel, click the **Build** link to build the application.

8. To program the board (download the application), plug in the board and select 'PWM GPIO', and then click **Program** in the Quick Panel.

### In Command-Line Interface (CLI):

1. Install ModusToolbox 2.x.

2. Download and unzip this repository onto your local machine, or clone the repository as follows: 
    
    1. Clone the 'wiced_btsdk' repo first with the `git clone` command. You will need to create this project just once in the working directory. For example:
    ```
   > git clone https://github.com/cypresssemiconductorco/wiced_btsdk
    ```
    2. Clone the 'mtb-example-btsdk-pwm_gpio' application repo with the `git clone` command. The application repo directory should be at the same folder level as 'wiced_btsdk'. For example:
   ``` 
   > git clone https://github.com/cypresssemiconductorco/mtb-example-btsdk-pwm_gpio
   ```

3. Open a CLI terminal and navigate to the *wiced_btsdk* folder. The 'wiced_btsdk' repo contains references to other repos. To download all the required collateral, change to the root folder and use the `make getlibs` command.
   ```
   > cd wiced_btsdk
   > make getlibs
   ```

    **Note**: On Windows, use Cygwin from *\ModusToolbox\tools_2.0\modus-shell\Cygwin.bat* as the command line interface.

4. Open a CLI terminal and navigate to the application folder (*mtb-example-btsdk-pwm_gpio*), and run the `make build` command.

5. Connect the kit. 

6. To program (download to) the board, run the `make qprogram` command:
    ```
    > make qprogram
    ```

7. Unplug the kit.

8. To build and program the board, run the `make program` command from respective app paths:
    ```
    > make program
    ```

   **Note**: `make program` = `make build` + `make qprogram`

## Operation

1. Connect the board to your PC using the provided USB cable.

   The USB Serial interface on the kit provides access to the two UART interfaces of the CYW208xx/CYW207xx device – WICED HCI UART and WICED Peripheral UART (PUART). The HCI UART is used only for downloading the application code in this code example, while the PUART interface is used for printing the Bluetooth stack and application trace messages. 

2. Open your terminal software and select the PUART COM port, with a baud rate  of 115200 bps.

3. Program the board with 'PWM GPIO' application.

4. After programming, the application boots up automatically. See Figure 1 for the output on PUART immediately after boot up.

    ##### Figure 1. PUART Terminal Output After Booting Up
    ![bootupimage](images/bootup-log.png)

5. Press the User Button (SW3) to initiate the PWM. On pressing the button SW3, the PWM duty cycle on LED 2, pin D6, and pin D7 will start changing. This results in the brightness changing in a breathing fashion on LED 2. In addition, LED 1 toggles from ON to OFF. 

6. Press the button again. LED 2 will hold it brightness level at which the button was pressed. LED 1 toggles from OFF to ON. The UART Terminal shows the duty cycle at which the PWM stopped. Pin D6 and D7 will also hold the same duty cycle at which the button was pressed.

    **Note**: The PWM output on D6 and D7 will be inverted with respect to each other.

    ##### Figure 2. PUART Terminal Output
    ![bootupimage](images/working-log.png)

## Design and Implementation

### Introduction
Objective of PWM_GPIO application is to demonstrate how PWM (Pulse Width Modulation) and GPIOs can be configured in an application using ModusToolbox IDE. It also shows how PWM signal and GPIO toggling are controlled using an onboard button. Once the device is powered ON, it boots up, initializes the PUART, and initializes the BT stack. The BT stack callback function (`bt_cback`) does the device configuration - GPIO configurations, register interrupts, and start a thread to control the PWM signal.

`application_start()` is the entry point to this application. It initializes PUART and route all the debug traces to the PUART. By default the PUART baud rate is configured as 115200 bps. This you can change by using the API `wiced_hal_puart_configuration()`. application_start also initializes the BT Stack (`wiced_bt_stack_init()`).  The BT Stack callback funtion (`bt_cback()`) performs below operations when it detects BTM_ENABLED_EVT event.  
1. Configure GPIOs (Pin D6 and pin D7) and LED2 for PWM output. 
2. Configures LED 1 and sets it's output to 0 by default. (Note: Pins are active low)
3. Configure on-board button (SW3) and Register for button interrupt.
3. Create a thread to control the PWM signal. 

Whenever the User Button (SW3) on the board is pressed and released, application detects a button interrupt in the interrupt callback function (`button_cb()`) and then LED 1 and the PWM flag will get toggled. Button is configured to detect the falling edge of the interrupt signal (Eg:GPIO_EN_INT_FALLING_EDGE). If PWM flag is set, then the duty cycle of PWM signal will start changing between 0% to 100% back and forth in the thread function (`PWM_control`). This continous change in the pwm signal will create a breathing effect on LED 2 brightness. The duty_cycle is incremented or decremented with a step size which is set in the application (`DUTY_STEP_SIZE`). When PWM flag is FALSE, LED 2 will hold it brightness level at which the button was pressed. Also LED1 toggles from OFF to ON state.  

PWM output on pin D6 and pin D7 are set inverted with respect to each other as shown below. This is useful for motor control application.

##### Figure 3. Inverted PWM in pin D6 and pin D7
![bootupimage](images/inverted-pwm.png)

This is done by setting the initial states of the pins (D6 and D7) as below, and then configuring it for the PWM signal.
```c
    wiced_hal_gpio_configure_pin(CYBSP_D6, GPIO_OUTPUT_ENABLE, GPIO_PIN_OUTPUT_HIGH);
    wiced_hal_gpio_configure_pin(CYBSP_D7, GPIO_OUTPUT_ENABLE, GPIO_PIN_OUTPUT_LOW);
```
The initial states (Immediately after programming) of LED1, LED2 and pin D6, D7 are as shown below. 

##### Table 1. Initial States of LEDs and Pins

|Pin    |State          |
|-------|---------------|
|LED1   |ON             |
|LED2   |OFF            |
|pin D6 |LOGIC_HIGH     |
|pin D7 |LOGIC_LOW      |

**Note**: Initial value of pwm_flag is 0.
	
This application also provides an option to use Auxiliary clock for generating PWM signal which will be using 24 MHz base frequency. Note that the desired GPIO pin must have already been configured to output for aclk. Please check `wiced_hal_aclk.h` for more details. 
To enable it please uncomment the following lines from the code.

```c
 clk=PMU_CLK;
 #if (defined(CYW20819A1) || defined(CYW20820A1))
    wiced_hal_aclk_enable(PWM_INP_CLK_IN_HZ, ACLK1, ACLK_FREQ_24_MHZ);
 #else
    wiced_hal_aclk_enable(PWM_INP_CLK_IN_HZ, WICED_ACLK1, WICED_ACLK_FREQ_24_MHZ);
 #endif
```

## Resources and Settings
This section explains the ModusToolbox resources and their configuration as used in this code example. Note that all the configuration explained in this section has already been done in the code example. The ModusToolbox IDE stores the configuration settings of the application in the *design.modus* file. This file is used by the graphical configurators, which generate the configuration firmware. This firmware is stored in the application’s *GeneratedSource* folder.

* **Device Configurator**: The Device Configurator is used to enable/configure the peripherals and pins used in the application. See the 
[Device Configurator Guide](https://www.cypress.com/ModusToolboxDeviceConfig).

* **Bluetooth Configurator**: The Bluetooth Configurator is used for generating/modifying the BLE GATT database. See the 
[Bluetooth Configurator Guide](https://www.cypress.com/ModusToolboxBLEConfig).

## Related Resources

| **Application Notes**                                            |                                                              |
| :----------------------------------------------------------- | :----------------------------------------------------------- |
|[AN225684 – Getting Started with CYW208XX](http://www.cypress.com/an225684) | Describes CYW20819, CYW20820 Bluetooth SoC, software/ hardware development ecosystem, and how to build your first BLE application using the device in ModusToolbox IDE.
| **Code Examples**  | Visit the BTSDK code examples repository in Cypress GitHub portal for a comprehensive collection of code examples using ModusToolbox IDE|
|**Device Documentation**|
|[CYW20819 Device Datasheet](https://www.cypress.com/datasheet/CYW20819)|
|[CYW20820 Device Datasheet](https://www.cypress.com/datasheet/CYW20820)|
|[CYW20719 Device Datasheet](https://www.cypress.com/documentation/datasheets/cyw20719-enhanced-low-power-bredrble-bluetooth-50-soc)|
|**Development Kits** |
|[CYW920819EVB-02 Evaluation Kit](http://www.cypress.com/CYW920819EVB-02)|
|[CYW920820EVB-02 Evaluation Kit](http://www.cypress.com/CYW920820EVB-02)|
|[CYW920719B2Q40EVB-01 Evaluation kit](https://community.cypress.com/docs/DOC-17736)|
|**Tool Documentation** |
|[ModusToolbox IDE](http://www.cypress.com/modustoolbox)| The Cypress IDE for IoT designers|
--------------------------------------------------------------------------------------------

## Document History

Document Title: PWM GPIO

| Version | Description of Change |
| ------- | --------------------- |
| 1.0.0   | New code example      |

------

![Banner](images/Banner.png)

-------------------------------------------------------------------------------

© Cypress Semiconductor Corporation, 2020. This document is the property of Cypress Semiconductor Corporation and its subsidiaries (“Cypress”).  This document, including any software or firmware included or referenced in this document (“Software”), is owned by Cypress under the intellectual property laws and treaties of the United States and other countries worldwide.  Cypress reserves all rights under such laws and treaties and does not, except as specifically stated in this paragraph, grant any license under its patents, copyrights, trademarks, or other intellectual property rights.  If the Software is not accompanied by a license agreement and you do not otherwise have a written agreement with Cypress governing the use of the Software, then Cypress hereby grants you a personal, non-exclusive, nontransferable license (without the right to sublicense) (1) under its copyright rights in the Software (a) for Software provided in source code form, to modify and reproduce the Software solely for use with Cypress hardware products, only internally within your organization, and (b) to distribute the Software in binary code form externally to end users (either directly or indirectly through resellers and distributors), solely for use on Cypress hardware product units, and (2) under those claims of Cypress’s patents that are infringed by the Software (as provided by Cypress, unmodified) to make, use, distribute, and import the Software solely for use with Cypress hardware products.  Any other use, reproduction, modification, translation, or compilation of the Software is prohibited.
TO THE EXTENT PERMITTED BY APPLICABLE LAW, CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH REGARD TO THIS DOCUMENT OR ANY SOFTWARE OR ACCOMPANYING HARDWARE, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  No computing device can be absolutely secure.  Therefore, despite security measures implemented in Cypress hardware or software products, Cypress shall have no liability arising out of any security breach, such as unauthorized access to or use of a Cypress product.  CYPRESS DOES NOT REPRESENT, WARRANT, OR GUARANTEE THAT CYPRESS PRODUCTS, OR SYSTEMS CREATED USING CYPRESS PRODUCTS, WILL BE FREE FROM CORRUPTION, ATTACK, VIRUSES, INTERFERENCE, HACKING, DATA LOSS OR THEFT, OR OTHER SECURITY INTRUSION (collectively, “Security Breach”).  Cypress disclaims any liability relating to any Security Breach, and you shall and hereby do release Cypress from any claim, damage, or other liability arising from any Security Breach.  In addition, the products described in these materials may contain design defects or errors known as errata which may cause the product to deviate from published specifications.  To the extent permitted by applicable law, Cypress reserves the right to make changes to this document without further notice. Cypress does not assume any liability arising out of the application or use of any product or circuit described in this document.  Any information provided in this document, including any sample design information or programming code, is provided only for reference purposes.  It is the responsibility of the user of this document to properly design, program, and test the functionality and safety of any application made of this information and any resulting product.  “High-Risk Device” means any device or system whose failure could cause personal injury, death, or property damage.  Examples of High-Risk Devices are weapons, nuclear installations, surgical implants, and other medical devices.  “Critical Component” means any component of a High-Risk Device whose failure to perform can be reasonably expected to cause, directly or indirectly, the failure of the High-Risk Device, or to affect its safety or effectiveness.  Cypress is not liable, in whole or in part, and you shall and hereby do release Cypress from any claim, damage, or other liability arising from any use of a Cypress product as a Critical Component in a High-Risk Device.  You shall indemnify and hold Cypress, its directors, officers, employees, agents, affiliates, distributors, and assigns harmless from and against all claims, costs, damages, and expenses, arising out of any claim, including claims for product liability, personal injury or death, or property damage arising from any use of a Cypress product as a Critical Component in a High-Risk Device.  Cypress products are not intended or authorized for use as a Critical Component in any High-Risk Device except to the limited extent that (i) Cypress’s published data sheet for the product explicitly states Cypress has qualified the product for use in a specific High-Risk Device, or (ii) Cypress has given you advance written authorization to use the product as a Critical Component in the specific High-Risk Device and you have signed a separate indemnification agreement.
Cypress, the Cypress logo, Spansion, the Spansion logo, and combinations thereof, WICED, PSoC, CapSense, EZ-USB, F-RAM, and Traveo are trademarks or registered trademarks of Cypress in the United States and other countries.  For a more complete list of Cypress trademarks, visit cypress.com.  Other names and brands may be claimed as property of their respective owners.
