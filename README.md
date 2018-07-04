# Table of Contents
- [Installation Instructions](#installation-instructions)
  * [Setting up the Software Environment](#setting-up-the-software-environment)
  * [Starting Eclipse](#starting-eclipse)
  * [Connect to the NI ELVIS III](#connect-to-the-ni-elvis-iii)
  * [Importing C Support File for NI ELVIS III to Eclipse](#importing-c-support-file-for-ni-elvis-iii-to-eclipse)
  * [Installing Bitfiles](#installing-bitfiles)
  * [Building and Installing Executable](#building-and-installing-executable)
- [Examples Overview](#examples-overview)
  * [AIO](#aio)
  * [AIO N Sample](#aio-n-sample)
  * [DIO](#dio)
  * [DIO N Sample](#dio-n-sample)
  * [PWM](#pwm)
  * [Encoder](#encoder)
  * [I2C](#i2c)
  * [SPI](#spi)
  * [UART](#uart)
  * [AIIRQ](#aiirq)
  * [DIIRQ](#diirq)
  * [ButtonIRQ](#buttonirq)
  * [TimerIRQ](#timerirq)
- [Function Select Register](#function-select-register)
- [NI ELVIS III Top Board Developer's Manual](#ni-elvis-iii-top-board-developer-s-manual)

# Installation Instructions

  This document guides you through configuring the development environment of Eclipse to debugging or running C example projects on the NI ELVIS III with Eclipse.
  
## Setting up the Software Environment

1.	Install Java SE Development Kit 6.0 or greater , which you can download at http://www.oracle.com/technetwork/java/javase/downloads/index.html.
2.	Install C/C++ Development Tools for NI Linux Real-Time, Eclipse Edition 2017-2018, which you can download at http://www.ni.com/download/labview-real-time-module-2017/6731/en/.

![](https://github.com/ni-kismet/ELVISIII_C_Examples/blob/master/docs/resources/picture.png)  

3.  Add the compiler path to the system environment variables
      
      a. In the Windows Control Panel, select **System and Security » System » Advanced system settings** to display the **System Properties** dialog box. <br>
      b. Click **Environment Variables** to display the **Environment Variables** dialog box.  <br>
      c. Select **PATH** in the **User variables** group box and click **Edit**. If **PATH** does not exist, click **New** to create one.  <br>
      d. Append the compiler path to **Variable value**. Delimit paths with semicolons.  <br>
      C:\build\17.0\arm\sysroots\i686-nilrtsdk-mingw32\usr\bin\arm-nilrt-linux-gnueabi <br>
      e. Click **OK** to close the dialog boxes and save changes. <br>
     
4. 	Add the library path to the system environment variables
      
      a. Click **New** in the **User variable** group box to display the **New User Variable** dialog box. <br>
      b. In the **Variable name**, enter **LIB_PATH**. <br>
      c. In the **Variable value** box, enter: C:\build\17.0\arm\sysroots\cortexa9-vfpv3-nilrt-linux-gnueabi <br>
      d. Click **OK** to close the dialog boxes and save changes. <br>
      
      e. Click **New** in the **User variable** group box to display the **New User Variable** dialog box. <br>
      f. In the **Variable name**, enter **C_INCLUDE_PATH**. <br>
      g. In the **Variable value** box, enter: %LIB_PATH%/usr/include <br>
      h. Click **OK** to close the dialog boxes and save changes. <br>



>NOTE : If you use other versions of Eclipse, update the paths accordingly.

## Starting Eclipse

1.	Run Eclipse from the location where it is installed.
2.	The **Select a Workspace** dialog shows. Click **OK**.
3.	On the Welcome screen, click the arrow to launch the Eclipse Workbench (C/C++ Editor)

![](https://github.com/ni-kismet/ELVISIII_C_Examples/blob/master/docs/resources/pic2.png)  

## Connect to the NI ELVIS III
1.	Select **Window > Open Perspective > Other** to display the **Open Perspective** dialog box.
2. Select **Remote System Explorer** and click **OK**.

![](https://github.com/ni-kismet/ELVISIII_C_Examples/blob/master/docs/resources/pic3.png)  

3.	On the **Remote Systems** pane's toolbar, click the **Define a new connection to a remote system** icon to launch the **New Connection** dialog box.

![](https://github.com/ni-kismet/ELVISIII_C_Examples/blob/master/docs/resources/pic4.png)  

4.	Select **Linux** and click **Next** to display the **Remote Linux System Connection** page.
5.	Enter the NI ELVIS III wireless IP address in the **Host name** textbox and click **Next** to display the **Files** page.

![](https://github.com/ni-kismet/ELVISIII_C_Examples/blob/master/docs/resources/pic5.png)

6. Enable the **ssh.files** checkbox and click **Next** to display the **Processes** page.

![](https://github.com/ni-kismet/ELVISIII_C_Examples/blob/master/docs/resources/pic6.png)  

7. Enable the **processes.shell.linux** checkbox and click **Next** to display the **Shells** page.
8. Select **ssh.shells** and click **Next** to display the **SSH Terminals** page.
9.	Click **Finish**. The remote system shows in the **Remote Systems** pane.
10. In the **Remote Systems** pane, right-click the target and select **Connect**. The **Enter Password** dialog box shows.

![](https://github.com/ni-kismet/ELVISIII_C_Examples/blob/master/docs/resources/pic7.png)  

11.	Enter the user ID and the password of the NI ELVIS III. Click **OK**.

>NOTE : Leave the **Password** textbox blank if you have not set one. There is no password by default.

12.	The **Info** dialog box shows. Click **OK**. You have successfully connected to the NI ELVIS III.

## Importing C Support File for NI ELVIS III to Eclipse
       Use C examples to try how to run a project on ELVIS III with Eclipse.
1. Download the C Support archive File from github depository. Click **Clone or download** button. Click **Download ZIP**.

![](https://github.com/ni-kismet/ELVISIII_C_Examples/blob/master/docs/resources/clone.png)  

2.	Select **Window > Open Perspective > Other** to display the **Open Perspective** dialog box.
3.	Select **C/C++ (default)** and click **OK**.
4.	Select **File > Import...** The **Import dialog** box shows.
5.	Select **General > Existing Projects** into Workspace and click **Next** to display the **Import Projects** page.
6.	Select **Select archive file**, click **Browse** and select the latest build C support archive file.
7.	Select the C Support file and the example to import and click **Finish**.

![](https://github.com/ni-kismet/ELVISIII_C_Examples/blob/master/docs/resources/pic8.png)  

>Note : After importing the C Support project, you need to check if the location of C_Support file is correct.<br>
>a. In the **Project Explore**r pane, right-click the example and select **Properties**. <br>
>b. On the left pane of the **Properties** dialog box, expand **Resource** and select **Linked Resources**. <br>
>c. On the right pane, click the **Linked Resources** tab, select the **C_Support** row, and click **Edit** to edit the path to the C support files. Usually the path is C:\Users<administrator name>\workspace\C Support for ELVISIII\source. Click **OK** to save your changes.  
 
![](https://github.com/ni-kismet/ELVISIII_C_Examples/blob/master/docs/resources/cSupportPath.png)  

## Installing Bitfiles
   Bitfiles contain information about the interfaces and registers of the hardware. Before using the NI ELVIS III, you need to install bitfiles  on the NI ELVIS III. You can download bitfiles from …
1.	Click **Window > Open Perspective > Other....** Select **Remote System Explorer**. Click **OK**.
2.	In the **Remote Systems** pane's toolbar, expand **Sftp Files**. Right-click **Root**, and select **New > Folder**. The **New Folder** dialog box shows.

![](https://github.com/ni-kismet/ELVISIII_C_Examples/blob/master/docs/resources/pic9.png)  

3.	Enter the folder name in the **New folder name** textbox: /var/local/natinst/bitfiles and click **Finish**. Ignore the error if the folder already exists.
4.	In the **Remote Systems** pane, right-click the newly created folder and select **Export From Project** from the shortcut menu to display the **Export** dialog box.
5.	Expand the project and select NiFpga_ELVISIIIv10FPGA.lvbitx (the bitfile name may vary depending on the hardware you are using). Click **Finish**.

![](https://github.com/ni-kismet/ELVISIII_C_Examples/blob/master/docs/resources/pic10.png)  

## Building and Installing Executable
1.	Click **Window > Open Perspective > Other....** Select **C/C++ (default)**. Click **OK**.

2.	In the **Project Explorer** pane, right-click an example and select **Build Project** from the shortcut menu to build the example. Wait until the project finishes building. 

>NOTE: if you select **Release** on the toolbar to build the project ,you need to configure the settings and tool chain of the project.

>![](https://github.com/ni-kismet/ELVISIII_C_Examples/blob/master/docs/resources/pic13.png)  

>   a.	Configuring the settings
>1.	In the **Project Explore**r pane, right-click the example and select **Properties**. 
>2.	On the left pane of the **Properties** dialog box, expand **C/C++ Build** and select **Settings**. 
>3.	On the right pane, select **Release** from the **Configuration** pull-down menu.
>4.	On the **Tool Settings** tab, Expand **Cross GCC Compiler** and configure **Symbols** and **Includes** according to configuration in Debug. Check your Debug configurations by selecting **Debug** from the **Configuration** pull-down menu. 
>5.	Expand **Cross GCC Linke**r and configure **Libraries** according to configuration in Debug.


>![](https://github.com/ni-kismet/ELVISIII_C_Examples/blob/master/docs/resources/pic14.png)

>   b.	Configuring the toolchain
>1.	On the left pane of the **Properties** dialog box, expand **C/C++ Build** and select **Tool Chain Editor**. 
>2.	On the right pane, select **CDT Internal Builder** from the **Current builder** pull-down menu.

3.	In the **Project Explorer** pane, select the project and select **Run -> Run Configurations** from the toolbar. 
4.	Right-click **C/C++ Remote Application** and select **New** to display the **Run Configurations** dialog box .

![](https://github.com/ni-kismet/ELVISIII_C_Examples/blob/master/docs/resources/pic11.png) 

5.	Select the IP address of the NI ELVIS III from the **Connection** pull-down menu.
6.	Click **Browse** to the right of the **Remote Absolute File Path for C/C++ Application** textbox.
7.	Right-click **My Home** and select **New -> Folder**. 
8.	Enter the new folder name and click **Finish**.
9.	Select the folder you have just created and click **OK**. In the textbox, append **/** after **/home/admin** and add the project name to the end of the path. 
10.	Click **Apply** to save the settings. 
  
  ![](https://github.com/ni-kismet/ELVISIII_C_Examples/blob/master/docs/resources/pic12.png) 
  
11.	Click **Run** to build and install the executables.

# Examples Overview
NI provides the following examples for using the NI ELVIS III. Refer to the *main.c* file in each example for more information.

## AIO
  Demonstrates using the analog input and output (AIO). This example writes an initial value to AO0 on bank A and reads this value from AI0 on bank A.This example also reads the difference of the initial values on AI1 and AI5 on bank B. This example prints the values to the console.
## AIO N Sample
  Demonstrates using th analog input and output of N Sample (AIO - N Sample). This example reads a group of values from AI0 on bank A and writes a group of initial values to AO0 on bank B.This example prints the group of read values to the console.
## DIO
  Demonstrates using the digital input and output (DIO). This example writes an initial value to DIO1 on bank A and reads this value from DIO0 on bank A.This example also reads an initial value from DIO0 on bank B.This example prints the values to the console.
## DIO N Sample
  Demonstrates using th digital input and output of N Sample (AIO - N Sample). This example reads a group of values from DIO0 on bank A and writes a group of initial values to DIO0 on bank B.This example prints the group of read values to the console.
## PWM
  Demonstrates using pulse-width modulation (PWM). This example generates a PWM signal from PWM 0 on bank A.
## Encoder
  Demonstrates using the encoder. This example reads a step and direction signal from the encoder on bank A and prints the values to the console.
## I2C
  Demonstrates using the I²C. This example reads the temperature from a connected TMP102 digital temperature sensor and writes the response to the console.
## SPI
  Demonstrates using the serial peripheral interface bus (SPI). This example writes a message to the SPI bus and then prints any returned bytes to the console.
## UART
  Demonstrates using the universal asynchronous receiver/transmitter (UART). This example writes a character to the UART bus and then prints any returned character to the console.
## AIIRQ
  Demonstrates using the analog input interrupt request (IRQ). This example registers an IRQ on analog input AI0 on bank A and creates a new thread that waits for the interrupt to occur.
## DIIRQ
  Demonstrates using the digital input interrupt request. This example registers an IRQ on digital input DIO0 on connector A and creates a new thread that waits for the interrupt to occur.
## ButtonIRQ
  Demonstrates using the button interrupt request. This example registers an IRQ on the user button of the NI ELVIS III and creates a new thread that waits for the interrupt to occur.
## TimerIRQ
  Demonstrates using the timer interrupt request. This example registers an IRQ on software timer and creates a new thread that waits for the interrupt to occur.

# Function Select Register

Some examples need extra devices to connect to NI ELVIS III.Here are the input and output interfaces of these examples:

- DIO:      DIO [0:19] on bank A and bank B
- PWM:      DIO [0:19] on bank A and bank B
- Encoder:  DIO [0:1], DIO [2:3], …, DIO [18:19] on bank A and bank B
- SPI:      DIO [5:7] on bank A and bank B
- I2C:      DIO [14:15] on bank A and bank B
- UART:     DIO [16:17]  on bank A and bank B

|**NI ELVIS III**| DIO example | PWM example | Encoder example | SPI example | I2C example              |UART example| 
|:--------------:|:-----------:|:-----------:|:---------------:|:-----------:|:------------------------:|:----------:| 
| **DIO 0**      | DIO 0       | PWM 0       | ENC.A 0         |             |                          |            | 
| **DIO 1**      | DIO 1       | PWM 1       | ENC.B 0         |             |                          |            | 
| **DIO 2**      | DIO 2       | PWM 2       | ENC.A 1         |             |                          |            | 
| **DIO 3**      | DIO 3       | PWM 3       | ENC.B 1         |             |                          |            | 
| **DIO 4**      | DIO 4       | PWM 4       | ENC.A 2         |             |                          |            | 
| **DIO 5**      | DIO 5       | PWM 5       | ENC.B 2         | SPI.CLK     |                          |            | 
| **DIO 6**      | DIO 6       | PWM 6       | ENC.A 3         | SPI.MISO    |                          |            | 
| **DIO 7**      | DIO 7       | PWM 7       | ENC.B 3         | SPI.MOSI    |                          |            | 
| **DIO 8**      | DIO 8       | PWM 8       | ENC.A 4         |             |                          |            | 
| **DIO 9**      | DIO 9       | PWM 9       | ENC.B 4         |             |                          |            | 
| **DIO 10**     | DIO 10      | PWM 10      | ENC.A 5         |             |                          |            | 
| **DIO 11**     | DIO 11      | PWM 11      | ENC.B 5         |             |                          |            | 
| **DIO 12**     | DIO 12      | PWM 12      | ENC.A 6         |             |                          |            | 
| **DIO 13**     | DIO 13      | PWM 13      | ENC.B 6         |             |                          |            | 
| **DIO 14**     | DIO 14      | PWM 14      | ENC.A 7         |             | I2C.SCL                  |            | 
| **DIO 15**     | DIO 15      | PWM 15      | ENC.B 7         |             | I2C.SDA                  |            | 
| **DIO 16**     | DIO 16      | PWM 16      | ENC.A 8         |             |                          | UART.RX    | 
| **DIO 17**     | DIO 17      | PWM 17      | ENC.B 8         |             |                          | UART.TX    | 
| **DIO 18**     | DIO 18      | PWM 18      | ENC.A 9         |             |                          |            | 
| **DIO 19**     | DIO 19      | PWM 19      | ENC.B 9         |             |                          |            | 

# NI ELVIS III Top Board Developer's Manual

This manual helps users of the National Instruments ELVIS III develop a custom application top board. The manual includes information on licensing, mechanical design, electrical design, basic software interfacing, and courseware support. For information on the use of ELVIS III or administering and provisioning ELVIS III in a laboratory setting refer to the product documentation.

You can find NI ELVIS III Top Board Developer's Manual [here](./docs/NI%20ELVIS%20III%20MDK.docx).

