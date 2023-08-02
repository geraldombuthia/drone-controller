# Drone Controller built on the ESP32 Microcontroller

This is a drone controller project that I did for my 5th year project.
The project was aimed at developing a drone with the capacity to be used 
in the fight against bandits in the central rift region of Kenya. This was 
a joint interdisciplinary project that inolved electrical and mechanical students.

My task was to develop a drone controller that allowed for communication between the drone
and the pilot.
<div style="display: flex; flex-direction: row; flex-wrap: wrap;">
  <figure style="margin-right: 20px; margin-bottom: 20px; flex: 1 1 300px;">
    <img src="/Images/Drone Controller Hardware.jpg" alt="Drone Controller Hardware" style="width: 100%; height: auto;">
    <figcaption>Tafiti Ground Terminal 3D Model complete</figcaption>
  </figure>
  <figure style="margin-right: 20px; margin-bottom: 20px; flex: 1 1 300px;">
    <img src="/Images/Drone Controller Front-view.jpg" alt="Drone Controller Front-view" style="width: 100%; height: auto;">
    <figcaption>Drone Controller Front-view.jpg</figcaption>
  </figure>
  <figure style="flex: 1 1 300px;">
    <img src="/Images/Drone Controller Receiver Hardware.jpg" alt="Drone Controller Receiver Hardware.jpg" style="width: 100%; height: auto;">
    <figcaption>Drone Controller Receiver Hardware</figcaption>
  </figure>
</div>

## Main Objective
To design, develop and test systems for a drone as a security solution with the capacity to achieve flight and perfom surveillance and reconnaisancce security missions.

## Specific Objectives
1. To design and implement an effective, secure and stable communication system that will enable transfer of flight data and logs from the UAV as well as provide a control channel to fly the UAV over ling distances.
2. To design and implement a ground control system that will generate control signals to fly the UAV.
3. To design and implement an interface to display operation status and assist in drone piloting.
4. To design and implement a good power supply system to provide the right amount of power for the ground station system.
5. To design a printed circuit for the ground and UAV system implementation.
6. To design a storage mechanismto store flight data and logs.
7. To design a fast and effecient software to run the processes.

## Block Diagram of the Full Electronic System
Only shows my area of work. The Microcontroller in the drone system segment is connected to driver circuits to enable flight.

![alt text](/Images/Circuit%20Block%20Diagram.drawio.png 'title text')

## Programming Language
C++
## Software used
- Arduino software - An Integrated Design Environment allowing for code development and compiling.
- Proteus - For general component simulation
- KiCAD - For circuit design and Schematic
- MatLab - For antenna design simulation
- FreeRTOS -  A Real-time Operating System for Embedded microcontrollers. Allows for multitasking capabilities