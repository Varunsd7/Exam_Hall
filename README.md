# Exam_Hall
📌 Overview

The Exam Hall Monitoring System is an IoT-based smart solution designed to enhance transparency, reduce malpractice, and improve monitoring efficiency during examinations. The system uses sensors, microcontrollers, and wireless communication to track student activity and environmental conditions in real-time.

This project aims to assist invigilators by automating monitoring tasks and providing alerts for suspicious behavior.

🎯 Objectives
Prevent cheating and malpractice in exam halls
Automate student and environment monitoring
Provide real-time alerts to invigilators
Reduce manual workload
Improve exam security and fairness
⚙️ Features
📡 Real-time monitoring using IoT
🚨 Alert system for suspicious activity
🌡️ Temperature & humidity tracking
👀 Motion / activity detection
📲 Wireless communication (WiFi-based)
📊 Data logging for analysis
🛠️ Technologies Used
Microcontroller: ESP32 / Arduino
Sensors:
Motion Sensor (PIR)
Temperature Sensor (DHT11/DHT22)
Communication: WiFi
Programming Language: C / C++ (Arduino IDE)
Platform: Embedded Systems + IoT
🧩 System Architecture
Sensors collect real-time data (motion, temperature, etc.)
ESP32 processes the data
Data is transmitted via WiFi
Alerts are generated if abnormal activity is detected
Monitoring system displays results
🔌 Components Required
ESP32 / Arduino Board
PIR Motion Sensor
DHT11 / DHT22 Sensor
Buzzer / Alarm Module
Power Supply
Connecting Wires
🧠 Working Principle
The system continuously monitors the exam hall environment.
If unusual movement is detected (e.g., frequent motion), an alert is triggered.
Environmental data like temperature is also tracked.
Alerts are sent to the invigilator via notification or buzzer.
📊 Applications
Schools and Colleges
Competitive Exam Centers
Online / Hybrid Exam Monitoring
Smart Classroom Systems
🚧 Limitations
Limited accuracy in crowded environments
Sensor range constraints
Dependence on stable WiFi connection
Cannot fully replace human invigilators
🔮 Future Scope
AI-based cheating detection using cameras
Face recognition for attendance
Cloud-based monitoring dashboard
Integration with mobile applications
Data analytics for behavior tracking
🧪 Testing
Verified sensor accuracy under different conditions
Tested alert system for motion detection
Checked WiFi connectivity and response time
📁 Project Structure
Exam-Hall-Monitoring/
│── code/
│   ├── main.ino
│── docs/
│   ├── report.pdf
│── circuit/
│   ├── diagram.png
│── README.md
📜 Conclusion

The Exam Hall Monitoring System provides a reliable and efficient way to enhance exam security using IoT technology. While it cannot fully replace manual supervision, it significantly reduces the chances of malpractice and improves monitoring efficiency.
