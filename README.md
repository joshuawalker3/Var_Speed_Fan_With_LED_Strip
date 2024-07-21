# **Variable Speed Desk for Use While Soldering with RBG LED**

## **Project Overview**
An annoyance I’ve had while soldering is that the fumes from the flux burning off tends to drift directly in my face and up my nose. To solve this issue, I made this desktop fan. The fan is actually a PC chassis fan and the LED strip is a cheap LED strip I had lying around from an old PC build. 

## **Code**
### *Overview*
The code is written in C. No IDE was used, only sublime text editor (because it’s the superior text editor).

Overall, the code is pretty straightforward. After initializing structs representing a motor (the fan) and the LED strip, they are used to initialize their respective functions then the drivers for each are started as their own task so they can run in “parallel”. The drivers and related functions are contained in components so the code header can be linked when the ELF is built by the toolchain.

The fan is represented in the code as a Pulse Width Modulated (PWM) DC-motor. The struct is used to set up the PWM module and the GPIO pins for controlling motor speed. A 2-bit resolution for the PWM was chosen since I only desired to have three speeds for the fan: off, half-power, and full-power. The motor driver polls the GPIO pins and, when an input is detected, changes the motor speed accordingly.

The LED strip is controlled via signals from 3 GPIO pins, 1 for each color. The LED has two modes. The static mode where a chosen color is displayed indefinitely and the cycle mode where every color combination is cycled through sequentially. The LED driver polls two input pins. The first input pin cycles through colors sequentially and can cycle power to the LED if pressed for 1 second. The second input changes the LED mode.
### *Challenges and Lessons Learned*
The biggest challenge with the code was figuring out the toolchain and how the build system works. A good chunk of time was spent trying to figure out how to integrate the header into the main code. The solution to this was obviously to RTFM. The biggest lesson learned was the value of structs. By putting all needed variables into pretty little structs, the code became easier to read as well as to write.

## **Chassis Design and Manufacture**
### *Overview*
The chassis was designed in FreeCAD, sliced using Ankermake default slicer, and printed on an Ankermake M5. It was printed as one solid piece. The chassis is simply a plastic base connected to a slot for the fan and slots for the LED strip and PCB. It was designed to be as compact as possible while maintaining stability.
### *Challenges and Lessons Learned*
The main challenge with the chassis was getting it to print properly. Since that are parts that are “floating” while the print is in process, supports were required for the fan slot and the circular vertical hole. The print failed twice for what is suspected to be the same reason. I had the standard print speed set, so while doing the infill the print bed can vibrate rapidly as the print head moves. This caused breaks in the supports. The solution was to simply to slow the speeds down across the board. This caused the print time to inflate from around 11 hours to 30 hours.

## **PCB Design**
### *Overview*
The PCB was designed using KiCAD and manufactured through PCBWay. I changed PCB manufacturers out of curiousity but could find no signifigance between PCBWay and Seeed Fusion.

Since the fan and LED strip require 12 VDC and the ESP32 requires between 3.3 – 6 VDC, the circuit is split by a buck converter that transforms 12VDC to8 around 5 VDC (I used 4.5 VDC). The PWM signal is sent directly from the ESP board to the fan. The LED strip interfaces to the ESP board via PN-2222A transistors.

I also added some components to aid in troubleshooting such as LED’s that indicate that power is being applied to the board when off, an indication the model is on, LED’s connected in parallel to each LED GPIO to show proper response, and an LED in parallel to the fan that shows the status of the fan since the LED will change brightness as the PWM signal, and therefore fan speed, changes.
### *Challenges and Lessons Learned*
This was a big step for me in PCB design since it had more components than I had ever used before. This meant that I had to be creative with routing traces. To further remove trace clutter on the PCB, I added via’s so I could have traces on both the top and bottom of the PCB.