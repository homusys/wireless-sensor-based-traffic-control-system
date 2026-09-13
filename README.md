# Wireless Sensor Based Traffic Control System
A sensor-driven traffic control system driven that uses multiple HC-SR04 ultrasonic sensors to detect cars and pedestrians, 
with the traffic light control logic basing on the sensor data.

## Relevant Hardware
- x9  Arduino R3 development boards (1 master board, 7 slave boards, 1 controller board)
- x10 HCSR04 Ultrasonic Sensors
- x9  nRF24L01 Low-power 2.4 GHz Wireless Transceiver Modules

## Project  Overview
I was hired as a freelance embedded programmer for a university thesis project from university students. 
While I don't have much of the details in front of me, the main takeaway is that the system will use
ultrasonic sensors to detect cars and pedestrians, the data is stored to a slave board, then sent to the 
master board using a transceiver. The master board will then process all the events from all the slave
boards and perform the traffic sequence appropriate to the current data. Then the events are sent to
the slave boards where the traffic lights are also connected and thus will perform the event. 
