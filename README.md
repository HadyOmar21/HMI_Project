## Embedded Systems Project Report: LM35 Temperature Monitoring HMI

## 1. Introduction

This project implements an LM35-based temperature monitoring and alarm system using an Arduino Uno and Embedded C.

The system reads the ambient temperature using an LM35 temperature sensor connected to an analog input of the Arduino Uno. The measured temperature is processed and displayed on an LCD, while the current temperature limits are used to determine the system's alarm status.

The application allows the user to configure the Lower Limit (LL) and Upper Limit (LH) through push buttons. The default lower and upper temperature limits are 20°C and 25°C, respectively. The system compares the measured temperature against these limits and indicates whether the temperature is within the acceptable range or has crossed either threshold.

The project also implements UART communication to transmit temperature readings and system information to a PC. This interface supports monitoring and verification of the system's behavior.

The project demonstrates analog sensor interfacing, ADC configuration, temperature conversion, LCD interfacing, GPIO control, push-button handling, UART communication, threshold configuration, and alarm management.

## 2. Project Code

**GitHub Repository:**
[submission_repo](https://github.com/HadyOmar21/HMI_Project/tree/main/code)

**Local Root Directory**
[link_to_local_root_directory](./code/HMI_Code/)

## 3. Test Cases
[robot_tests](./test/test_cases.robot)

## 4. Bill of Materials (BOM)

**BOM**
[bom_file](./bom/bom.csv)

**Optimized BOM**
[optimized_bom_file](./bom/optimized_bom.csv)

## 5. Video Demonstration
[recorded_video](./video/Video_Test.mp4)


## 6. Design
[design_document](./design/project_design.md)
