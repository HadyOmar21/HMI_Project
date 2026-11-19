
*** Settings ***
Documentation    LM35 Temperature HMI Project Test Cases

*** Test Cases ***

Test Case 1: Startup Message, Defaults, and LCD
    [Documentation]    Verify the startup UART message, default thresholds, and normal live LCD screen.
    # Arrange
    Connect the Arduino Uno and open the Serial Monitor at 9600 baud.

    # Act
    Reset or power-cycle the Arduino Uno.
    Allow the system to complete initialization and the first temperature update.

    # Assert
    Verify that Temperature HMI Started is transmitted through UART.
    Verify that the LCD displays the temperature and NORMAL or ALERT on row 1.
    Verify that LL:20 LH:25 is displayed on row 2.
    Verify that the temporary startup banner is replaced after approximately one second.


Test Case 2: Button Pull-Downs and LM35 Separate Ground Pin
    [Documentation]    Verify the specified button and LM35 wiring before normal operation.
    # Arrange
    Turn OFF the Arduino power.
    Inspect the wiring of both normally-open buttons.
    Verify that each button connects +5 V to its signal pin when pressed.
    Verify that an external pull-down resistor connects each signal node to Arduino GND.
    Verify that MAX/LH is connected to A1 and MIN/LL is connected to A2.
    Verify that LM35 OUT is connected to A0.
    Verify that LM35 GND uses a separate physical Arduino GND header pin.

    # Act
    Inspect the button signal levels while both buttons are released.
    Check the LM35 ground connection.

    # Assert
    Verify that both button inputs are LOW when idle.
    Verify that the external pull-down resistors prevent floating button inputs.
    Verify that the LM35 has its own physical GND lead.
    Verify that the separate Arduino GND pins remain electrically common.


Test Case 3: UART Diagnostic Format and Conversion
    [Documentation]    Verify the periodic UART report fields and ADC-to-temperature calculation.
    # Arrange
    Connect the Arduino Uno to the PC.
    Configure the Serial Monitor to 9600 baud.

    # Act
    Reset the Arduino.
    Observe several consecutive UART diagnostic reports.

    # Assert
    Verify that each report follows the format ADC=n | V=n mV | TEMP=n.n C | MIN=n | MAX=n.
    Verify that the ADC value is between 0 and 1023.
    Verify that the voltage value is between 0 and 5000 mV.
    Verify that voltage approximately equals round(ADC * 5000 / 1023).
    Verify that temperature in tenths of a degree corresponds to voltage in mV.
    Verify that 350 mV corresponds to approximately 35.0 C.


Test Case 4: One-Second Report Scheduler
    [Documentation]    Verify that periodic UART reports continue at approximately one-second intervals.
    # Arrange
    Connect the Arduino Uno to the PC.
    Open the Serial Monitor at 9600 baud.

    # Act
    Observe at least six consecutive temperature reports.
    Compare the arrival times of consecutive reports.

    # Assert
    Verify that reports are transmitted in order at approximately one-second intervals.
    Allow for slightly longer intervals caused by processing.
    Verify that there are no unexplained multi-second gaps or unexpected bursts.
    Verify that periodic reporting does not stop.


Test Case 5: Stable Analog Input and Averaged ADC Reading
    [Documentation]    Verify repeatability with a stable analog test source without independently proving the sample count.
    # Arrange
    Disconnect LM35 OUT from A0.
    Connect a stable, low-noise DC source to A0 with its return connected to Arduino GND.
    Set the source voltage to approximately 0.35 V.

    # Act
    Observe at least five consecutive UART temperature reports.

    # Assert
    Verify that the reported temperature settles near 35 C.
    Verify that the reported temperature remains stable.
    Note that the firmware averages 64 ADC samples per reading.
    Do not consider this visual test independent proof of the sample count.
    Restore the LM35 wiring after the test.


Test Case 6: MAX/LH Short Press Increments Once
    [Documentation]    Verify that a short press of MAX/LH increments the high threshold exactly once.
    # Arrange
    Reset the Arduino.
    Verify that LL is 20 and LH is 25.

    # Act
    Press and release MAX/LH (A1) once.
    Ensure that the press duration is shorter than one second.

    # Assert
    Verify that LH changes from 25 to 26 exactly once.
    Verify that LL remains 20.
    Verify that releasing the button does not cause an additional change.


Test Case 7: MIN/LL Short Press Increments Once
    [Documentation]    Verify that a short press of MIN/LL increments the low threshold exactly once.
    # Arrange
    Reset the Arduino.
    Verify that LL is 20 and LH is 25.

    # Act
    Press and release MIN/LL (A2) once.
    Ensure that the press duration is shorter than one second.

    # Assert
    Verify that LL changes from 20 to 21 exactly once.
    Verify that LH remains 25.


Test Case 8: MAX/LH Long Press Decrements Once
    [Documentation]    Verify that a long press decrements LH without also performing a short-press increment on release.
    # Arrange
    Reset the Arduino.
    Verify that LL is 20 and LH is 25.

    # Act
    Hold MAX/LH (A1) for approximately 1.2 seconds.
    Release the button.

    # Assert
    Verify that LH changes from 25 to 24 exactly once.
    Verify that releasing the button does not increment LH back to 25.
    Verify that LL remains 20.


Test Case 9: MIN/LL Long Press Decrements Once
    [Documentation]    Verify that a long press decrements LL without also performing a short-press increment on release.
    # Arrange
    Reset the Arduino.
    Verify that LL is 20 and LH is 25.

    # Act
    Hold MIN/LL (A2) for approximately 1.2 seconds.
    Release the button.

    # Assert
    Verify that LL changes from 20 to 19 exactly once.
    Verify that releasing the button does not increment LL back to 20.
    Verify that LH remains 25.


Test Case 10: MAX/LH Long-Hold Repeat and Stop on Release
    [Documentation]    Verify repeated decrements while MAX/LH is held and that decrementing stops after release.
    # Arrange
    Reset the Arduino.
    Verify that LL is 20 and LH is 25.

    # Act
    Hold MAX/LH (A1) continuously for approximately 3.2 seconds.
    Release the button.
    Wait for another UART report.

    # Assert
    Verify that LH decreases approximately once per second.
    Verify that LH decreases approximately three times, reaching 22.
    Verify that LH remains at 22 after releasing the button.
    Verify that LL remains 20.


Test Case 11: MIN/LL Long-Hold Repeat and Stop on Release
    [Documentation]    Verify repeated decrements while MIN/LL is held and that decrementing stops after release.
    # Arrange
    Reset the Arduino.
    Verify that LL is 20 and LH is 25.

    # Act
    Hold MIN/LL (A2) continuously for approximately 3.2 seconds.
    Release the button.
    Wait for another UART report.

    # Assert
    Verify that LL decreases approximately once per second.
    Verify that LL decreases approximately three times, reaching 17.
    Verify that LL remains at 17 after releasing the button.
    Verify that LH remains 25.


Test Case 12: Press Just Below the Long-Press Threshold
    [Documentation]    Verify short-press behavior immediately below the one-second hold threshold.
    # Arrange
    Reset the Arduino.
    Verify that LH is 25.
    Prepare a timer or button test jig for accurate timing.

    # Act
    Press and release MAX/LH (A1) after approximately 0.8 seconds.

    # Assert
    Verify that the action is treated as a short press.
    Verify that LH changes from 25 to 26.
    Verify that LH does not decrease to 24.


Test Case 13: Press Just Above the Long-Press Threshold
    [Documentation]    Verify long-press behavior immediately above the one-second hold threshold.
    # Arrange
    Reset the Arduino.
    Verify that LH is 25.
    Prepare a timer or button test jig for accurate timing.

    # Act
    Hold MAX/LH (A1) for approximately 1.2 seconds.
    Release the button.

    # Assert
    Verify that the action is treated as a long press.
    Verify that LH changes from 25 to 24.
    Verify that LH does not increase to 26.


Test Case 14: Pulse Shorter Than the Debounce Check Is Ignored
    [Documentation]    Verify rejection of a pulse shorter than the firmware's 25 ms input recheck delay.
    # Arrange
    Reset the Arduino.
    Verify that LH is 25.
    Prepare a signal generator or button test jig.

    # Act
    Apply one HIGH pulse to A1 with a duration shorter than 25 ms.
    Return the signal to LOW.

    # Assert
    Verify that the short pulse is ignored.
    Verify that LH remains 25.
    Do not use a normal human finger press for this timing test.


Test Case 15: Five Short Presses Produce Five Increments
    [Documentation]    Verify that repeated short presses are counted exactly once per press.
    # Arrange
    Reset the Arduino.
    Verify that LH is 25.

    # Act
    Press and release MAX/LH (A1) exactly five times.
    Ensure that every press is shorter than one second.
    Allow the button to return LOW between consecutive presses.

    # Assert
    Verify that LH increases by exactly five.
    Verify that LH reaches 30.
    Verify that no increments are missed or duplicated.


Test Case 16: Simultaneous Short Presses
    [Documentation]    Verify that both button inputs are processed when pressed and released together.
    # Arrange
    Reset the Arduino.
    Verify that LL is 20 and LH is 25.

    # Act
    Press MAX/LH (A1) and MIN/LL (A2) simultaneously.
    Release both buttons before one second.

    # Assert
    Verify that LH changes from 25 to 26.
    Verify that LL changes from 20 to 21.
    Verify that each button press is counted exactly once.


Test Case 17: Both Buttons Held Together
    [Documentation]    Verify that both long-press actions are processed when both buttons are held simultaneously.
    # Arrange
    Reset the Arduino.
    Verify that LL is 20 and LH is 25.

    # Act
    Press and hold A1 and A2 simultaneously for approximately 1.2 seconds.
    Release both buttons.

    # Assert
    Verify that LH decreases once from 25 to 24.
    Verify that LL decreases once from 20 to 19.
    Verify that neither threshold increases when the buttons are released.


Test Case 18: Idle Pull-Down Prevents False Button Events
    [Documentation]    Verify that idle button inputs remain inactive and thresholds do not change spontaneously.
    # Arrange
    Reset the Arduino.
    Verify that LL is 20 and LH is 25.
    Ensure that both buttons are released.

    # Act
    Leave both buttons untouched for at least 60 seconds.
    Do not touch the button wires or signal nodes.

    # Assert
    Verify that LL remains 20.
    Verify that LH remains 25.
    Verify that no spontaneous threshold changes occur.
    Verify that both active-HIGH inputs remain LOW through their external pull-down resistors.


Test Case 19: MAX/LH Increment Saturates at 99
    [Documentation]    Verify the upper limit for LH and ensure additional presses do not cause wrapping or overflow.
    # Arrange
    Reset the Arduino.
    Verify that LH is 25.

    # Act
    Short-press MAX/LH (A1) exactly 74 times.
    Verify that LH reaches 99.
    Perform three additional short presses.

    # Assert
    Verify that LH reaches 99.
    Verify that LH remains 99 after the additional presses.
    Verify that LH does not wrap to zero or exceed 99.


Test Case 20: MIN/LL Increment Saturates at 99
    [Documentation]    Verify the upper limit for LL and ensure additional presses do not cause wrapping or overflow.
    # Arrange
    Reset the Arduino.
    Verify that LL is 20.

    # Act
    Short-press MIN/LL (A2) exactly 79 times.
    Verify that LL reaches 99.
    Perform three additional short presses.

    # Assert
    Verify that LL reaches 99.
    Verify that LL remains 99 after the additional presses.
    Verify that LL does not wrap to zero or exceed 99.


Test Case 21: MAX/LH Decrement Saturates at Zero
    [Documentation]    Verify that long-hold decrements stop at zero without unsigned underflow.
    # Arrange
    Reset the Arduino.
    Verify that LH is 25.

    # Act
    Hold MAX/LH (A1) continuously for approximately 25.5 seconds.
    Release the button.
    Perform additional long holds after reaching zero.

    # Assert
    Verify that LH reaches zero.
    Verify that LH never wraps to 255 or 99.
    Verify that additional long holds at zero leave LH unchanged.
    Verify that LL remains 20.


Test Case 22: MIN/LL Decrement Saturates at Zero
    [Documentation]    Verify that long-hold decrements stop at zero without unsigned underflow.
    # Arrange
    Reset the Arduino.
    Verify that LL is 20.

    # Act
    Hold MIN/LL (A2) continuously for approximately 20.5 seconds.
    Release the button.
    Perform additional long holds after reaching zero.

    # Assert
    Verify that LL reaches zero.
    Verify that LL never wraps to 255 or 99.
    Verify that additional long holds at zero leave LL unchanged.
    Verify that LH remains 25.


Test Case 23: LL Can Cross Above LH
    [Documentation]    Verify that crossed thresholds are accepted and characterize the strict outside-range alarm comparison.
    # Arrange
    Reset the Arduino.
    Verify that LL is 20 and LH is 25.
    Disconnect LM35 OUT from A0.
    Connect a stable test source to A0 with its return connected to Arduino GND.

    # Act
    Short-press MIN/LL (A2) six times to set LL to 26.
    Verify that LH remains 25.
    Set A0 to approximately 0.255 V.
    Observe the UART temperature report, LCD, and alert LED.

    # Assert
    Verify that LL=26 and LH=25 are accepted.
    Verify that the firmware does not reorder or reject crossed thresholds.
    Verify that a measured temperature between 25.0 C and 26.0 C produces ALERT.
    Verify that the alert LED turns ON.
    Restore the LM35 wiring after the test.


Test Case 24: Temperature Below LL Turns Alert On
    [Documentation]    Verify the low-temperature alarm condition.
    # Arrange
    Reset the Arduino.
    Verify that LL is 20 and LH is 25.
    Disconnect LM35 OUT from A0.
    Connect a test source to A0 with its return connected to Arduino GND.

    # Act
    Set A0 to approximately 0.18 V.
    Observe the UART report, LCD, and alert LED.

    # Assert
    Verify that the reported temperature is below 20.0 C.
    Verify that the LCD displays ALERT.
    Verify that the D12 alert LED is ON.
    Restore the LM35 wiring after the test.


Test Case 25: Exact Equality at LL Is Normal
    [Documentation]    Verify that a temperature equal to the low threshold does not trigger an alarm.
    # Arrange
    Reset the Arduino.
    Verify that LL is 20 and LH is 25.
    Disconnect LM35 OUT from A0.
    Connect an adjustable low-voltage source to A0 with its return connected to Arduino GND.

    # Act
    Adjust the source voltage near 0.201 V.
    Continue adjusting until UART reports exactly 20.0 C.

    # Assert
    Verify that the reported temperature is exactly 20.0 C.
    Verify that the LCD displays NORMAL.
    Verify that the alert LED is OFF.
    Verify that equality with LL does not trigger an alarm.
    Restore the LM35 wiring after the test.


Test Case 26: Temperature Strictly Inside the Limits Is Normal
    [Documentation]    Verify the normal operating region between the low and high thresholds.
    # Arrange
    Reset the Arduino.
    Verify that LL is 20 and LH is 25.
    Disconnect LM35 OUT from A0.
    Connect a test source to A0 with its return connected to Arduino GND.

    # Act
    Set A0 to approximately 0.225 V.
    Observe the UART report, LCD, and alert LED.

    # Assert
    Verify that the reported temperature is strictly above 20.0 C and below 25.0 C.
    Verify that the LCD displays NORMAL.
    Verify that the alert LED is OFF.
    Restore the LM35 wiring after the test.


Test Case 27: Temperature Above LH Turns Alert On
    [Documentation]    Verify the high-temperature alarm condition.
    # Arrange
    Reset the Arduino.
    Verify that LL is 20 and LH is 25.
    Disconnect LM35 OUT from A0.
    Connect a test source to A0 with its return connected to Arduino GND.

    # Act
    Set A0 to approximately 0.275 V.
    Observe the UART report, LCD, and alert LED.

    # Assert
    Verify that the reported temperature is above 25.0 C.
    Verify that the LCD displays ALERT.
    Verify that the D12 alert LED is ON.
    Restore the LM35 wiring after the test.


Test Case 28: Exact Equality at LH Is Normal
    [Documentation]    Verify that temperature equal to a reachable high threshold does not trigger an alarm.
    # Arrange
    Reset the Arduino.
    Verify that LL is 20 and LH is 25.
    Disconnect LM35 OUT from A0.
    Connect an adjustable test source to A0 with its return connected to Arduino GND.

    # Act
    Hold MAX/LH (A1) for approximately 3.2 seconds to set LH to 22.
    Adjust A0 near 0.220 V until UART reports exactly 22.0 C.

    # Assert
    Verify that the reported temperature is exactly 22.0 C.
    Verify that the LCD displays NORMAL.
    Verify that the alert LED is OFF.
    Verify that equality with LH does not trigger an alarm.
    Restore the LM35 wiring after the test.


Test Case 29: ADC Zero and Near-Full-Scale Input
    [Documentation]    Characterize ADC readings near both ends of the permitted input range.
    # Arrange
    Disconnect LM35 OUT from A0.
    Connect an adjustable test source to A0 with its return connected to Arduino GND.

    # Act
    Set A0 to 0.00 V and observe the UART ADC value.
    Set A0 to approximately 4.90 V and observe the UART ADC value.
    Never exceed 5.00 V or the Arduino board's AVCC rail.

    # Assert
    Verify that the ADC reading is close to zero at 0.00 V.
    Verify that the ADC reading is near full scale (1023) at approximately 4.90 V.
    Verify that voltage and temperature remain within the firmware's representable range.
    Restore the LM35 wiring after the test.


Test Case 30: Sensor Disconnect Has No Dedicated Fault Indication
    [Documentation]    Characterize firmware behavior with A0 disconnected without expecting a specific floating reading.
    # Arrange
    Ensure that the LM35 is powered and its ground connection remains connected.

    # Act
    Disconnect only LM35 OUT from A0.
    Leave A0 floating.
    Observe the LCD and UART reports for several measurement cycles.

    # Assert
    Verify that no dedicated SENSOR ERROR or sensor-disconnected message is displayed.
    Do not expect a specific temperature or LED state because the floating A0 reading is unspecified.
    Reconnect LM35 OUT after the test.


Test Case 31: Sensor Reconnection Restores Telemetry
    [Documentation]    Verify that temperature reporting resumes correctly after reconnecting the LM35 output.
    # Arrange
    Ensure that the LM35 supply is connected.
    Verify that the LM35 ground lead uses its separate physical Arduino GND pin.

    # Act
    Reconnect LM35 OUT to A0.
    Wait for the next temperature measurement.

    # Assert
    Verify that UART temperature reports continue.
    Verify that the LCD displays a numeric temperature.
    Verify that the corresponding NORMAL or ALERT status is displayed.
    Verify that no manual reset is required.


Test Case 32: UART Status Command Is Not Implemented
    [Documentation]    Verify that the firmware does not implement an application-level status command parser.
    # Arrange
    Connect the Arduino Uno to the PC.
    Open the Serial Monitor at 9600 baud.
    Disable local echo.

    # Act
    Send status followed by Enter.
    Observe the subsequent UART output.

    # Assert
    Verify that the firmware does not send a special status-command reply.
    Verify that periodic temperature diagnostic reports continue normally.


Test Case 33: Reset Restores Default Thresholds
    [Documentation]    Verify that threshold settings are volatile and return to their default values after reset.
    # Arrange
    Reset the Arduino.
    Verify that LL is 20 and LH is 25.

    # Act
    Change at least one threshold using its corresponding button.
    Press the Arduino RESET button or power-cycle the board.

    # Assert
    Verify that Temperature HMI Started is transmitted through UART.
    Verify that LL returns to 20.
    Verify that LH returns to 25.
    Verify that the LCD and subsequent UART reports show the default thresholds.


Test Case 34: LCD Refresh With Single- and Two-Digit Thresholds
    [Documentation]    Verify LCD formatting and clearing of old characters at different threshold widths.
    # Arrange
    Connect the Arduino and allow the LCD to initialize.

    # Act
    Use the buttons to display threshold values 0, 9, 10, and 99 in both LL and LH positions.
    Observe the LCD after each change.
    Reset the board between value groups if necessary.

    # Assert
    Verify that LL and LH labels remain in their correct positions.
    Verify that all threshold digits are readable.
    Verify that old trailing characters are cleared when the number of digits changes.
    Verify that the display remains within the 16-character row width.


Test Case 35: Contact Bounce Does Not Duplicate a Press
    [Documentation]    Verify one press with simulated switch bounce around the debounce recheck interval.
    # Arrange
    Reset the Arduino.
    Verify that LH is 25.
    Prepare a controlled button test jig.

    # Act
    Generate one MAX/LH press with brief HIGH/LOW bounce transitions shorter than the 25 ms recheck period.
    Follow the bounce transitions with a stable HIGH signal lasting less than one second.
    Generate a similar release bounce.

    # Assert
    Verify that one physical press produces exactly one increment.
    Verify that LH changes from 25 to 26.
    Record any multiple increments as a test failure.
    Treat this as a characterization of the implemented 25 ms recheck, not as a guarantee of hardware-independent debouncing.


Test Case 36: Thirty-Minute Endurance and Button Responsiveness
    [Documentation]    Verify continued operation, reporting, display, LED, and button response during extended operation.
    # Arrange
    Connect the LM35 sensor and Arduino Uno.
    Open the Serial Monitor.
    Ensure that the LCD and alert LED are visible.
    Provide a stable LM35 input.

    # Act
    Run the system continuously for at least 30 minutes.
    Periodically press and release both buttons.
    Observe UART reports, LCD updates, and LED behavior throughout the test.

    # Assert
    Verify that no unexpected reset occurs.
    Verify that the system does not freeze or become unresponsive.
    Verify that UART reports continue without unexplained interruptions.
    Verify that the LCD remains readable.
    Verify that the alert LED does not become stuck.
    Verify that both buttons remain responsive.
    Verify that threshold changes remain consistent with the corresponding button actions.


Test Case 37: Button Held During Reset
    [Documentation]    Verify the behavior of a button that is already held when the firmware starts.
    # Arrange
    Verify that LH is 25.

    # Act
    Hold MAX/LH (A1).
    Press and release the Arduino RESET button while continuing to hold A1.
    Keep A1 held for approximately 1.2 seconds after restart.
    Release the button.

    # Assert
    Verify that the held input is recognized as one long press.
    Verify that LH changes from 25 to 24.
    Verify that the action is not treated as a short press when the button is released.


Test Case 38: Equal LL and LH Limits
    [Documentation]    Verify behavior below, at, and above equal low and high thresholds.
    # Arrange
    Reset the Arduino.
    Verify that LL is 20 and LH is 25.
    Disconnect LM35 OUT from A0.
    Connect an adjustable test source to A0 with its return connected to Arduino GND.

    # Act
    Short-press MIN/LL (A2) twice to set LL to 22.
    Hold MAX/LH (A1) for approximately 3.2 seconds to set LH to 22.
    Verify that both thresholds are 22.
    Adjust A0 to produce a temperature below 22.0 C.
    Adjust A0 until UART reports exactly 22.0 C.
    Finally, adjust A0 to produce a temperature above 22.0 C.

    # Assert
    Verify that LL=LH=22 is accepted.
    Verify that a temperature below 22.0 C produces ALERT and turns the LED ON.
    Verify that a temperature exactly equal to 22.0 C produces NORMAL and turns the LED OFF.
    Verify that a temperature above 22.0 C produces ALERT and turns the LED ON.
    Restore the LM35 wiring after the test.
