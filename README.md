[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](/LICENSE)

# Team Çakatech - WRO® 2026 Future Engineers

<table border="0">
  <tr border="0">
    <td width="70%"  border="0">
      <img src="t-photos/TeamOfficial.jpg" alt="Official Photo" width="45%">
      <img src="t-photos/TeamFunny.jpg" alt="Funny Photo" width="45%">
    </td>
    <td valign="top" style="padding-left: 0px;" border="0">
        This repository details team <em>Çakatech</em>'s building and programming process in the 2026 WRO Future Engineers Competition. 
      We are team <em>Çakatech</em>, three students who are passionate about technology and robotics. For this competition we built an autonomous vehicle. We worked after school and on weekends. Through this competition, we learned how to work as a team and solve problems related to robotics.
       </p>
        <strong>Team Members:</strong>
      <ul>
        <li>Batuhan Pekcan, 16</li>
        <li>Deniz Türker, 16</li>
        <li>Ali Aslan İşkol, 15</li>
      </ul>
      </p>
    </td>
  </tr>
</table>

[![Youtube](https://img.shields.io/badge/Youtube-%23FF0000.svg?style=for-the-badge&logo=Youtube&logoColor=white)](https://www.youtube.com/@Saybrone/shorts)


# Table of Contents
- [Folder Contents](#folder-contents)
- [The Challenge](#the-challenge)
- [Vehicle Overview](#vehicle-overview)
  - [V-Photos](#v-photos)
  - [Hardware Components](#hardware-components)
  - [Electronic Components](#electronic-components)
  - [Mobility Management](#mobility-management)
    - [Powertrain](#powertrain)
    - [Steering](#steering)
    - [Chassis](#chassis)
- [Power and Sense Management](#power-and-sense-management)
  - [Li-ion Battery](#li-ion-battery)
  - [IMU Sensor](#imu-sensor)
  - [OpenMV Cam RT1062](#camera)
  - [EVN Alpha](#evn-alpha)
- [Software Components](#software-components)
  - [EVN Alpha Side](#evnalpha-side)
  - [OpenMV Camera Side](#openmv-side)
- [License](#license)

# Folder Contents <a class="anchor" id="folder-contents"></a>
* `models` is for the 3D files we used to print our parts
* `other` includes other files which can be used to understand how to prepare the vehicle for the competition. It includes documentations, datasets, hardware specifications, communication protocols,  descriptions etc.
* `schemes` contains schematic diagrams of the electromechanical components illustrating all the elements (electronic components and motors) used in the vehicle and how they connect to each other.
* `src` contains code of control software for all components which were programmed to participate in the competition
* `t-photos` contains photos of the team and logos
* `v-photos` contains 6 photos of the vehicle from various angles
* `video` contains the video.md file with the link to our YouTube channel and the respective videos

# The Challenge <a class="anchor" id="the-challenge"></a>

In the **[WRO 2026 Future Engineers – Self-Driving Cars](https://wro-association.org/)** category, teams are tasked with creating a robotic vehicle that can autonomously navigate a changing racetrack. Each round introduces new track layouts, requiring vehicles to adapt in real time.
The competition highlights the full engineering process:

- **Vehicle Design**: Building a functional robot with electromechanical components and advanced steering or motion systems.
- **Obstacle Management**: Applying computer vision, sensor fusion, and motion planning to make real-time decisions.
- **Project Documentation**: Maintaining an engineering journal and sharing designs in a public GitHub repository.

Teams are judged on performance, innovation, reliability, and the clarity of their engineering process, encouraging creativity, teamwork, and STEM skills.

Learn more about the challenge [here](https://wro-association.org/wp-content/uploads/WRO-2026-Future-Engineers-Self-Driving-Cars-General-Rules.pdf).

# Vehicle <a class="anchor" id="vehicle-overview"></a>




## V-Photos <b class="anchor" id="v-photos"></a>
| <img src="v-photos/front.jpg" width="40%" /> | <img src="v-photos/back.jpg" width="40%" /> | 
| :--: | :--: | 
| *Front* | *Back* |
| <img src="v-photos/left.jpg" width="40%" /> | <img src="v-photos/right.jpg" width="40%" /> | 
| *Left* | *Right* |
| <img src="v-photos/top.jpg" width="40%" /> | <img src="v-photos/bottom.jpg" width="40%" /> | 
| *Top* | *Bottom* |

<br>

# Hardware Components <a class="anchor" id="hardware-components"></a>
This section covers all the parts utilized in the vehicle, such as motors, sensors, controllers, chassis, mechanical systems, and other components.
## Electronic Components <a class="anchor" id="electronic-components"></a>
<table border="1" cellpadding="12" cellspacing="0">
  <thead>
    <tr>
      <th>Component</th>
      <th>Description / Link</th>
      <th>Image</th>
      <th>Price</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>Motor</td>
      <td>EV3 Medium Motor</td>
      <td><img src = "other/motor/motor.png" alt= "Motor" width="200"</td>
      <td>$125x2</td>
    </tr>
    <tr>
      <td>Servo Motor</td>
      <td>GeekServo (360 degrees)</a></td>
      <td><img src = "other/servo_motor/geek.PNG"  alt = "Servo motor" width ="135"</td>
      <td>$10</td>
    </tr>
  <tr>
    <tr>
      <td>Motor Controller and Processor</td>
      <td>EVN Alpha</a></td>
      <td><img src="other/evn/evn2.png" alt="evn" width="150"></td>
      <td>$168</td>
    </tr>
      <td>IMU</td>
      <td>MPU 6500 Gyro Sensor</td>
      <td><img src ="other/IMU/IMU.PNG" alt="IMU" width="150"</td>
      <td>$1.88</td> 
    <tr>
      <td>Battery</td>
      <td>Molicel 35A Li-ion battery</td>
      <td><img src="other/battery/Battery.png" alt="battery" width="150"</td>
      <td>$10x2</td>
    </tr>
    </tr>
    <tr>
      <td>Camera</td>
      <td>OpenMV Cam RT1062</a></td>
      <td><img src= "other/camera/cam.PNG" alt= "Camera" width="143"</td>
      <td>$120</td>
    </tr>
    <tr>
      <td>Time of Flight Sensors</td>
      <td>VL53L0X ToF Distance Sensor</a></td>
      <td><img src= "other/tof/tof.PNG" alt= "Time of Flight" width="143"</td>
      <td>$2.66x2</td>
    </tr>
    <tr>
      <td>Lego Differential</td>
      <td>Lego Differential</td>
      <td><img src ="/other/Lego Differential/Lego Differantial.png" alt="Differential" width="150"</td>
      <td>$2.96</td> 
    <tr>
      <td colspan="3"><strong>Total Cost</strong></td>
      <td><strong>$878.06</strong></td>
    </tr>
  </tbody>
</table>

## Mobility Management <a class="anchor" id="mobility-management"></a>

Our robot’s mobility depends on the coordination of its powertrain, steering system, and chassis. Together, these components provide stability, control, and efficiency, enabling smooth and reliable movement.

### Powertrain <a class="anchor" id="powertrain"></a>
The powertrain converts electrical energy into mechanical motion, driving the robot’s wheels for movement and obstacle navigation.

#### Motor
<table>
<tr>
<td>
<img src="other/motor/motor.png" alt="EV3 Medium Motor" width="900">
</td>

<td valign="top" style="padding-left: 15px;">

<b>Specifications (per motor):</b><br>
Rated Voltage: 9V<br>
Weight: 80g<br>
No-Load Speed: ~240 RPM<br>
Stall Torque: ~8 N·cm<br>
Integrated Rotation Sensor: 1° Resolution<br>
Connector: LEGO EV3/RJ12<br>
<br>
This project uses <b>two LEGO EV3 Medium Motors</b> as the primary actuators. These motors are designed for applications requiring <b>precise control</b> and <b>fast response</b>, making them ideal for robotic mechanisms and mobile platforms.
<br><br>
The built-in <b>high-resolution rotation sensor</b> enables accurate position and speed feedback, allowing for reliable closed-loop control. This feature is particularly useful for tasks that require precise movements, such as navigation and steering.
<br><br>
</td>
</tr>
</table>

  
#### Differential
<table> <tr><td> <img src = "/other/Lego Differential/Lego Differantial.png" alt="Differential" width = "400" </td><td valign="top" style="padding-left: 15px;"> <b>Specifications:</b><br> Dimensions: 23 x 16 x 11 cm <br> Weight: 18 g <br><br>This Lego Differential is fully compatible with 3D shapes and Lego parts, allowing for seamless integration. It also features a 28 Teeth Differential Gear with a round axle design, providing smooth rotational movement for our robot. </td> </tr> </table>

### Steering <a class="anchor" id="steering"></a>
Our robot uses a **Ackermann steering** where the wheels are turned with linkage geometry. This method makes the robot easier to turn and keeps the overall design compact. It is highly effective for lightweight and fast-moving prototypes where simplicity and space efficiency are key.

#### Servo Motor
<table> <tr><td><img src = "other/servo motor/geek.PNG" alt="Servo motor" width = "800" </td> <td valign="top" style="padding-left: 15px;"> <b>Specifications:</b><br> Operating Voltage: 3.3V~6V<br> Rated Voltage: 4.8V<br> Rotational range: 360°<br> Maximum Torque: 1.6kg±0.2kg/cm (4.8V)<br> Maximum Speed: 45rpm (3V)<br> Weight: 20g<br><br> For steering we selected the <b>GeekServo</b>. This motor is compatible with Lego Technic parts and offers a higher speed compared to 9g motors. The output shaft features a Lego Technic axle connector, making it ideal for applications that require a high-power drive. </td> </tr> </table> 

### Chassis <a class="anchor" id="chassis"></a>
Our chassis combines LEGO components with custom 3D-printed parts, creating a reliable and durable structure. The chassis provides mounting points for all motors, controllers, and sensors, ensuring stable alignment and easy integration. Below, 3D models of the parts are included.


#### Axle Adapter
</p> <img src="/models/AxleAdapter/AxleAdapter.jpeg" alt = "axle adapter" width = "500" display:block; margin:0 auto; width:400px;" />

[3D Model](/models/AxleAdapter/AxleAdapter.stl)

       
## Power and Sense Management <a class="anchor" id="power-and-sense-management"></a>


### Li-ion Battery <a class="anchor" id="li-ion-battery"></a>
<table> <tr><td> <img src = "other/battery/Battery.png" alt = "Battery" width = 100 </td><td valign="top" style="padding-left: 15px;"> <b>Specifications:</b><br> Voltage: 3.7V <br>Capacity:2800mAh<br> Diameter: 18mm <br> Length: 65mm <br></td> </tr> </table> 

### IMU <a class="anchor" id="imu-sensor"></a>
<table> <tr><td> <img src ="other/IMU/IMU.PNG" alt="IMU" width="150" alt = "IMU" width = "300" <td valign="top" style="padding-left: 15px;"> <b>Specifications:</b><br> Gyroscope Range: ±250, ±500, ±1000, ±2000 °/s<br>Accelerometer Range: ±2g, ±4g, ±8g, ±16g<br> Interface : I2C <br>Power Supply: 3.5V  <br> </td> </tr> </table> The MPU-6500 is a 6-axis MotionTracking sensor that combines a 3-axis gyroscope and a 3-axis accelerometer in a compact 3x3x0.9 mm package. This integration allows reliable motion detection and orientation tracking with reduced size and complexity. We selected the MPU-6500 because it provides reliable motion tracking, low power consumption, and small form factor, making it well-suited for our vehicle’s navigation and stability control.

### OpenMV Cam RT1062 <a class="anchor" id="camera"></a>
<table> <tr> <td><img src="other/camera/cam.PNG" alt = "Camera " width = "800" width:800px; height:auto; display:block; margin:0 auto;" />
 </td> <td valign="top" style="padding-left: 15px;"> <b>Specifications:</b><br> Microcontroller: ARM Cortex M7 (RT1062)<br>Frequency: 600 MHz<br> RAM: 32 MB SDRAM + 1 MB SRAM <br> Flash Memory: 16 MB program/storage flash<br>Camera Resolution: 2592 × 1944 (5 MP) <br> Frame Rate:~40 FPS on QVGA (320 × 240) <br><br> This <b>OpenMV Cam -RT1062</b> is the one we used in our robot. The OpenMV Cam is a small, low-power microcontroller board that we used in our robot to implement machine vision applications. We program the OpenMV Cam in high-level Python scripts (via the MicroPython Operating System) instead of C/C++, which makes it much easier to handle the complex outputs of machine vision algorithms and work with high-level data structures. At the same time, we retain full control over the OpenMV Cam and its I/O pins in Python. This allows our robot to locate obstacles, lines and walls, enabling intelligent, autonomous behaviors. </td> </tr> </table>


### EVN Alpha <a class="anchor" id="evn-alpha"></a>
<table> <tr> <td><img src="other/evn/evn alpha2.jpg" alt = "evn alpha" width = "1100"  width:800px; height:auto; display:block; margin:0 auto;" />
 </td> <td valign="top" style="padding-left: 15px;"> <b>The EVN ALPHA is a compact robot controller based on the RP2040, housed in a LEGO Technic-compatible shell. It provides 26 I/O channels for controlling brushed DC motors, servos, and connecting UART or I2C peripherals. The board also integrates a 2-cell Lithium-Ion power management system, offering charging, cell balancing, and voltage regulation, making it ideal for safely powering and controlling our robot’s motors and sensors.</td> </tr> </table>
   
## Software Components <a class="anchor" id="software-components"></a>
### EVN Alpha Side <a class="anchor" id="evnalpha-side"></a>
---
### OpenMV Camera Side <a class="anchor" id="openmv-side"></a>

#### 1. Color Detection
- **Orange / Blue line**:  
  - Detects blobs in bottom ROI.  
  - Returns `True` if blob is large enough.  
- **Red / Green box**:  
  - Detects rectangular blobs.  
  - Filters by size and shape.  
  - Returns `[area, error]` for EVN Alpha.  

#### 2. Brightness Detection
- Reads a central ROI.  
- Computes average brightness.  
- Low brightness → wall detected.  

#### 3. UART Control
- Waits for EVN Alpha command.  
- Runs appropriate detection function.  
- Sends results back wrapped in `<...*>`.  

Example:  
- EVN Alpha sends `1`.  
- Camera detects green box.  
- Camera replies: `<500,20*>` (area=500, error=20px right).
```py
import sensor, time
from machine import UART

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.set_auto_gain(False)
sensor.set_auto_whitebal(False)
#---Flips sensor data because it is upside down---
sensor.set_vflip(True)
sensor.set_hmirror(True)

sensor.skip_frames(time=2000)
uart1 = UART(1, baudrate=9600) #Starts Uart Communication in pin "P4" and "P5"

clock = time.clock()
img_width = sensor.width()
img_height = sensor.height()

#---Finds brightness in middle to detect wall---
def Find_MiddleBrightness(img):
    #---Defining a rectangle roi for middle---
    rect_x = int(img_width * 0.3)
    rect_y = int(img_height * 0.35)
    rect_w = int(img_width * 0.4)
    rect_h = int(img_height * 0.3)
    middle_roi = (rect_x, rect_y, rect_w, rect_h)

    #---Gets the average brightness of the pixels in roi---
    stats = img.get_statistics(roi=middle_roi)
    brightness = stats.mean()
    return brightness


#---Detects Orange Line to determine the direction---
def Find_Orange_Line(img):
    orange_threshold = (40, 51, -115, 68, 16, 29) #Color threshold used for orange

    #---Defines a rectangle roi for the floor---
    rect_x = int(0)
    rect_y = int(img_height * 0.3)
    rect_w = int(img_width)
    rect_h = int(img_height)
    lines_roi = (rect_x, rect_y, rect_w, rect_h)

    #---Finds similar colored pixel blocks---
    orange_blobs = img.find_blobs([orange_threshold], roi=lines_roi, area_threshold=3000)
    for blob in orange_blobs:
        img.draw_rectangle(blob.rect(),color=(0, 255, 0))
        return True #Returns True if sees an orange blob
    return False #Returns False if it doesn't see an orange blob


#---Detects Blue Line to determine the direction---
def Find_Blue_Line(img):
    blue_threshold = (35, 8, -128, 16, -128, -6) #Color threshold used for blue

    #---Defines a rectangle roi for the floor---
    rect_x = int(0)
    rect_y = int(img_height * 0.3)
    rect_w = int(img_width)
    rect_h = int(img_height)
    lines_roi = (rect_x, rect_y, rect_w, rect_h)
    img.draw_rectangle(lines_roi,color=(255, 0, 0))

    #---Finds similar colored pixel blocks---
    blue_blobs = img.find_blobs([blue_threshold], roi=lines_roi, area_threshold=3000)
    for blob in blue_blobs:
        img.draw_rectangle(blob.rect(),color=(0, 255, 0))
        print(blob.area())
        return True #Returns True if sees a blue blob

    return False #Returns False if it doesn't see a blue blob


#---Detects Red Rectangles (used for object tracking or direction)---
def Find_Red_Rect(img):
    red_threshold = (9, 72, 57, 22, -12, 57) #Color threshold used for red
    blobs_red = img.find_blobs([red_threshold], area_threshold=1000)

    rect = None
    center = None
    error = None
    area = None
    returnSTR = ""

    #---Draws cross at image center---
    img_center_x = img.width() // 2
    img_center_y = img.height() // 2
    img.draw_cross(img_center_x, img_center_y, color=(255, 0, 0))

    #---Finds blobs that match red threshold---
    for a in blobs_red:
        img.draw_rectangle(a.rect())
        x, y, w, h = a.rect()
        img.draw_cross(x + w // 2, y + h // 2)

        #---Check shape (elongation close to rectangle)---
        if a.elongation() < 1.2:
            area = a.area()
            rect = a.rect()
            x, y, w, h = rect
            center = (x + w // 2, y + h // 2)
            error = center[0] - img_center_x
            returnSTR = returnSTR + str([area, error])+"?"

    return returnSTR


#---Detects Green Rectangles (used for object tracking or direction)---
def Find_Green_Rect(img):
    green_threshold = (0, 52, -35, -15, -55, 17) #Color threshold used for green
    blobs_green = img.find_blobs([green_threshold], area_threshold=1000)

    rect = None
    center = None
    returnSTR = ""
    error = 0
    area = 0

    #---Draws cross at image center---
    img_center_x = img.width() // 2
    img_center_y = img.height() // 2
    img.draw_cross(img_center_x, img_center_y, color=(255, 0, 0))

    #---Finds blobs that match green threshold---
    for a in blobs_green:
        img.draw_rectangle(a.rect())
        x, y, w, h = a.rect()
        img.draw_cross(x + w // 2, y + h // 2)

        #---Check shape (elongation close to rectangle)---
        if a.elongation() < 1.2:
            area = a.area()
            rect = a.rect()
            x, y, w, h = rect
            center = (x + w // 2, y + h // 2)
            error = center[0] - img_center_x
            returnSTR = returnSTR + str([area, error])+"?"

    return returnSTR


#---Main Loop---
while True:
    clock.tick()  #Updates the FPS clock
    img = sensor.snapshot()  #Takes a picture from the camera

    #---Reads 1 byte from UART to decide which function to run---
    a = uart1.read(1)

    if a == b'1':
        #---Detects Green Rectangle and sends area + error via UART---
        uart1.write("<" + str(Find_Green_Rect(img)) + "*")
    elif a == b'2':
        #---Detects Red Rectangle and sends area + error via UART---
        uart1.write("<" + str(Find_Red_Rect(img)) + "*")
    elif a == b'3':
        #---Measures brightness in the middle ROI and sends value via UART---
        uart1.write("<" + str(Find_MiddleBrightness(img)) + "*")
    elif a == b'4':
        #---Detects Orange Line and sends True/False via UART---
        uart1.write("<" + str(Find_Orange_Line(img)) + "*")
    elif a == b'5':
        #---Detects Blue Line and sends True/False via UART---
        uart1.write("<" + str(Find_Blue_Line(img)) + "*")

    #---Debugging: print the received UART command and FPS---
    print(a)
    print(clock.fps())
    time.sleep(0.01)  #Small delay for stability
```

## License <a class="anchor" id="license"></a>

```
GNU General Public License v3.0

Copyright (C) 2007 Free Software Foundation, Inc. <https://fsf.org/>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published
by the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.

