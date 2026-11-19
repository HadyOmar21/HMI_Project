# LM35 Temperature Monitoring HMI — Detailed Project Design

| Document field | Value |
|---|---|
| Project | LM35 Temperature Monitoring HMI |
| Document revision | 1.0 — Final Submission |
| Date | 19 November 2026 |
| Controller | Arduino Uno compatible with ATmega328P |
| LCD | HD44780-compatible 16 × 2 character display, 4-bit interface |
| Temperature sensor | LM35 analog temperature sensor |

## 1. Project overview

The project is a small temperature-monitoring human-machine interface (HMI). An LM35 sensor provides an analog voltage proportional to temperature. The Arduino Uno samples that voltage through ADC channel A0, discards the first conversion after selecting the channel, averages 64 subsequent conversions, and converts the averaged ADC result into a temperature value with one decimal place.

The user configures a lower limit (`LL`) and an upper limit (`LH`) using two push buttons. The measured temperature is compared with these limits. A 16 × 2 LCD presents the current temperature, the `NORMAL` or `ALERT` state, and the two limits. An alert LED connected to Arduino D12 is also driven when the measurement is outside the configured range. The firmware sends a startup message and periodic diagnostic measurements over the hardware UART.

The firmware starts with `LL = 20 °C` and `LH = 25 °C`. These values are held in RAM; they are not stored in EEPROM and return to their defaults after reset or power loss.

## 2. System architecture

```text
               +---------------------+
               |       LM35          |
               | VCC / OUT / GND     |
               +----+-----+-----+----+
                    |     |     |
                  +5 V   A0   Separate Arduino GND pin
                          |
                          v
+-------------------+  ADC + averaging   +--------------------+
| MAX/LH pushbutton |--- PB1             |                    |
| + external        |                    |   Application      |
| pull-down         |                    |   - LL/LH logic    |
+-------------------+                    |   - alarm compare  |
                                         |   - scheduler      |
+-------------------+                    |                    |
| MIN/LL pushbutton |--- PB0             |                    |
| + external        |                    +---+------------+---+
| pull-down         |                        |            |
+-------------------+                        v            v
                                      +-------------+  +----------+
                                      | 16x2 LCD    |  | Alert LED|
                                      | 4-bit       |  | D12      |
                                      +-------------+  +----------+
                                             |
                                             v
                                      +-------------+
                                      | UART 9600   |
                                      | 8N1         |
                                      +-------------+
```

The push buttons are active-HIGH inputs with external pull-down resistors. The LM35 ground lead is connected to a **separate physical GND pin on the Arduino board**. This is a separate connection point for the sensor lead, not an electrically isolated ground: all Arduino GND pins are part of the same electrical ground reference.

## 3. Hardware pin assignment

| Function | Arduino pin | ATmega328P signal | Direction | Electrical behavior |
|---|---:|---|---|---|
| LM35 output | A0 | PC0 / ADC0 | Analog input | 0–AVCC analog measurement range |
| MAX / `LH` push button | D9 | PB1 | Input | Active HIGH; external pull-down resistor |
| MIN / `LL` push button | D8 | PB0 | Input | Active HIGH; external pull-down resistor |
| Alert LED | D12 | PB4 | Output | Active HIGH |
| LCD RS | D10 | PB2 | Output | LCD command/data select |
| LCD Enable | D11 | PB3 | Output | LCD enable strobe |
| LCD D4 | D4 | PD4 | Output | 4-bit LCD data bit 0 |
| LCD D5 | D5 | PD5 | Output | 4-bit LCD data bit 1 |
| LCD D6 | D6 | PD6 | Output | 4-bit LCD data bit 2 |
| LCD D7 | D7 | PD7 | Output | 4-bit LCD data bit 3 |
| LCD R/W | GND | — | — | Permanently write-only |
| UART RX | D0 | PD0 / RXD | Input | Hardware USART receive; no application command parser |
| UART TX | D1 | PD1 / TXD | Output | Hardware USART transmit |
| Supply | 5V / GND | VCC / GND | — | Common board supply and reference |

The source initializes LCD data pins in the order D4, D5, D6, D7. LCD D0–D3 are not connected in this 4-bit implementation.

## 4. Hardware connections

### 4.1 LM35 temperature sensor

Connect the sensor as follows:

| LM35 connection | Destination |
|---|---|
| VS / supply | Arduino 5V |
| VOUT | Arduino A0 |
| GND | A separate physical Arduino GND header pin |

Important wiring notes:

- Use the LM35 package datasheet to confirm the pin order for the exact package and viewing orientation before applying power. Do not infer pin order from the flat face or package shape alone.
- Route the LM35 GND lead directly to its own Arduino GND header connection, separate from the physical GND point used for the button pull-down resistors where practical. The two grounds remain electrically common through the Arduino board.
- Do not connect the sensor output to A0 through a pull-down resistor. A0 must receive the LM35 output signal directly (or, during a controlled test, the output of a calibrated test source instead of the sensor output).
- A 100 nF ceramic bypass capacitor between the LM35 supply and ground, placed close to the sensor, is recommended for supply decoupling. This is a hardware recommendation, not a component configured by the firmware.
- The LM35 output is nominally 10 mV per degree Celsius. The firmware's conversion assumes a 5,000 mV ADC reference; actual accuracy therefore depends on the real AVCC voltage and sensor characteristics.

### 4.2 Push buttons and external pull-down resistors

There are two independent, normally-open push-button circuits. Each button uses its own external pull-down resistor. A 10 kΩ resistor is shown as an illustrative typical value; the firmware does not specify the resistor value.

```text
                 +5 V                         +5 V
                  |                            |
             [MAX/LH switch]              [MIN/LL switch]
                  |                            |
                  +---- PB1                     +---- PB0
                  |                            |
               [10 kΩ]                      [10 kΩ]
                  |                            |
             Button GND                  Button GND
```

For each button:

1. Connect one side of the normally-open switch to +5 V.
2. Connect the other side to its input signal: PB1 for MAX/LH or PB0 for MIN/LL.
3. Connect one pull-down resistor from that signal node to GND.
4. Connect the pull-down resistors to the button-ground reference. Keep the sensor's GND lead on its separate physical Arduino GND pin as described in Section 4.1.

When the switch is released, its signal node is held LOW by the pull-down resistor. When pressed, the switch connects the signal node to +5 V and the input becomes HIGH. This matches the source configuration: both pins are inputs and the firmware does not enable the ATmega328P internal pull-up resistors.

A separate external pull-down is required for each input. Do not leave PB0 or PB1 floating, and do not wire the button so that pressing it connects the input to GND; that would invert the expected active-HIGH behavior.

### 4.3 Alert LED

Connect Arduino D12 through a 220 Ω series resistor to the LED anode. Connect the LED cathode to GND. Verify LED polarity and the resistor connection before powering the board. The firmware drives D12 HIGH when the alert condition is true and LOW otherwise.

### 4.4 16 × 2 LCD — 4-bit mode

| LCD pin | Connection |
|---|---|
| VSS | GND |
| VDD | +5 V |
| V0 / contrast | Wiper of a 10 kΩ contrast potentiometer |
| RS | Arduino D10 |
| R/W | GND |
| E | Arduino D11 |
| D0–D3 | Not connected |
| D4 | Arduino D4 |
| D5 | Arduino D5 |
| D6 | Arduino D6 |
| D7 | Arduino D7 |
| LED+ / LED− | Follow the LCD module datasheet; provide current limiting if the module does not include it |

Connect the contrast potentiometer outer terminals to +5 V and GND and its wiper to LCD V0. Adjust the potentiometer after power-up until the characters are visible. Check the particular LCD module's backlight requirements; some modules include a resistor and others require an external one.

### 4.5 UART

Use the Uno USB connection for power, programming, and serial monitoring. The firmware configures the hardware USART for 9600 baud, 8 data bits, no parity, and one stop bit (9600 8N1). Avoid connecting a second active transmitter to D0/RX while the onboard USB-to-serial interface is connected.

## 5. Firmware behavior

### 5.1 Initialization and defaults

The supplied application calls Arduino `init()`, initializes UART, configures PB0 and PB1 as inputs, initializes the alert LED, configures ADC and LM35 handling, and initializes the LCD in 4-bit mode. It then displays:

- LCD row 1: `Temperature HMI`
- LCD row 2: `LL:20 LH:25`
- UART startup line: `Temperature HMI Started` followed by CRLF

The starting threshold values are `LL = 20` and `LH = 25`.

### 5.2 Temperature measurement and conversion

The LM35 driver selects ADC channel A0 and discards the first conversion following channel selection. It then reads 64 samples, adds them, and calculates the rounded average. A 200 µs delay follows each of the 64 retained samples.

The exact integer expression in the source is:

```c
voltage_mv = ((unsigned int)adc_value * sensor->reference_voltage_mv + 511U)
             / ADC_MAX_VALUE;
```

The sensor's nominal sensitivity is 10 mV/°C. Because the temperature is stored in tenths of a degree Celsius, the numerical millivolt value is copied directly into `temperature_x10`.

For example, a calculated voltage of 253 mV is represented as 25.3 °C. The source defines `LM35_DEFAULT_REFERENCE_MV` as 2500 mV and `ADC_MAX_VALUE` as 1023. The actual AVCC/reference voltage is not measured at runtime. A difference between the assumed and actual reference affects the reported voltage and temperature.

The value is unsigned. The current implementation does not support representing negative temperatures.

### 5.3 Button interaction

Both buttons are sampled in the main loop. A detected transition to HIGH is rechecked after a 50 ms delay. If it remains HIGH, the firmware records the press time and initializes the repeat timer.

| Button | Short press — released before 1 s | Long hold — 1 s and continued hold | Bounds |
|---|---|---|---|
| MAX/LH — PB1| Increase `LH` by 1 °C on release | Decrease `LH` once after 1 s, then once per additional 1 s while held | Increment stops at 99; decrement stops at 0 |
| MIN/LL — PB0| Increase `LL` by 1 °C on release | Decrease `LL` once after 1 s, then once per additional 1 s while held | Increment stops at 99; decrement stops at 0 |

A press that reaches the long-hold condition is not also treated as a short press when released. The firmware's long-press flag suppresses the release-time increment. The first decrement occurs at approximately 1 second after the debounced press is recognized. Each subsequent decrement is scheduled one second after the previous decrement.

The source independently bounds each threshold between 0 and 99 for its implemented increment/decrement operations. It does **not** enforce `LL <= LH`; users can configure crossed limits. This behavior is documented for testing and is not silently treated as an accepted operating constraint.

### 5.4 Alarm decision and LED

At each scheduled measurement, the firmware evaluates:

- If the measured temperature is strictly below LL or strictly above LH, D12 is driven HIGH and the alarm state is `ALERT`.
- If the temperature is equal to either limit or lies between the limits, D12 is driven LOW and the state is `NORMAL`.
- If `LL > LH`, there is no temperature that can satisfy both the normal lower and upper conditions. The comparison therefore evaluates to ALERT for every temperature. The code does not repair or swap crossed limits.

### 5.5 LCD output

The firmware refreshes both LCD rows after each temperature update:

| Row | Source format | Purpose |
|---|---|---|
| 1 | `T:<whole>.<decimal>C <NORMAL or ALERT>` | Current temperature and state |
| 2 | `LL:<minimum> LH:<maximum>` | Configured lower and upper limits |

The application provides a 16-character display width and writes only through column 16. The actual row strings include spacing to overwrite older characters when a value or state becomes shorter. `LL` is the low/minimum threshold; `LH` is the high/maximum threshold.

### 5.6 UART diagnostic output

The firmware initializes UART to 9600 8N1 and sends a diagnostic line after each temperature update. The message format is:

```text
ADC=<raw average> | V=<millivolts> mV | TEMP=<whole>.<decimal> C | MIN=<LL> | MAX=<LH>
```

Each line ends with CRLF. The application sends the startup line once and then periodic measurement lines. Although UART receive helper functions exist in `uart.ino`, the application does not read incoming data and does not implement the earlier `status` command. Sending `status` therefore does not produce a command-specific response.

## 6. Scheduling and timing

The main loop continuously checks both buttons. It performs the temperature acquisition, alarm comparison, LCD refresh, and UART transmission when:

```c
millis() - previous_sensor_update >= 1000
```

The scheduler sets `previous_sensor_update` to the current `millis()` value before performing the measurement and output operations. Since acquisition averages 64 ADC values and the LCD/UART routines take additional time, the observed report interval can be longer than one second. The firmware does not promise an exact one-second wall-clock update.

## 7. Software module responsibilities

| Source file | Responsibility |
|---|---|
| `LM35_LCD_HMI_Clean.ino` | Application initialization, button timing, threshold updates, scheduling, alarm logic, LCD formatting, and UART reporting |
| `adc.h` / `adc.ino` | ADC reference selection, channel configuration, digital-input disable, and conversion functions |
| `lm35.h` / `lm35.ino` | First-conversion discard, 64-sample averaging, and voltage-to-temperature mapping |
| `hal_gpio.h` / `hal_gpio.ino` | Register-level GPIO direction, read, write, toggle, and initialization |
| `led.h` / `led.ino` | Alert LED interface using the GPIO HAL |
| `Chr_LCD.h` / `Chr_LCD.ino` | HD44780-compatible 4-bit initialization, nibble/byte transfers, cursor positioning, and text output |
| `uart.h` / `uart.ino` | USART setup and blocking transmit/receive primitives; the application uses transmit only |

## 8. Test strategy and coverage

The Robot Framework suite is in `test/assignment_test_cases.robot`; its serial and operator-interaction keywords are implemented in `test/LM35HmiLibrary.py`. Install dependencies with `python -m pip install -r test/requirements.txt`. See `test/README.md` for port configuration and execution commands.

The suite uses a hardware-in-the-loop approach:

- **Automated checks:** startup UART message, telemetry-line parsing, ADC range, integer conversion arithmetic, LL/LH values, periodic report timing, and absence of an application response to the unsupported `status` command.
- **Operator-assisted checks:** physical button presses/holds, simultaneous button operation, wiring inspection, LCD text, and LED state. The firmware has no host command interface to drive buttons or read LCD/LED state, so these actions require a person at the hardware.
- **Analog fixture checks:** a safe, calibrated low-voltage source may temporarily replace the LM35 output at A0 to check temperatures below, at, and above limits. Disconnect the sensor output before connecting the test source, keep its return common with Arduino GND, and never exceed the board's permitted analog input range.

The suite includes short/long press behavior, debounce characterization, repeat timing, button independence, simultaneous operation, idle input stability, a button held across reset, upper/lower saturation, crossed and equal thresholds, below/inside/above temperature cases, threshold equality cases, ADC edge characterization, sensor disconnect/reconnect characterization, reset defaults, and extended operation. No case is pre-marked as passed; Robot output files record the result of each execution.

### 8.1 Equality test reachability

The firmware's comparator treats exact equality as NORMAL. The test suite checks equality only when the integer ADC conversion can produce the required tenths-degree value under the documented 5,000 mV assumption. For example, ADC code 41 maps to 200 mV (20.0 °C), and ADC code 86 maps to 420 mV (42.0 °C). A 250 mV / 25.0 °C value is not produced by an integer ADC code using this exact rounding formula; therefore the upper-equality test uses the reachable test threshold `LH = 42 °C` rather than pretending a 25.0 °C equality measurement is guaranteed to be representable.

## 9. Conclusion

The supplied firmware implements a 4-bit LCD temperature HMI with 64-sample LM35 acquisition, independent short-press/long-hold threshold controls, one-second-or-longer reporting, a threshold-based LED alert, and UART diagnostics. This document now describes the actual final source and its wiring, including the external button pull-down resistors and the LM35's separate physical ground connection. The firmware itself remains unchanged; compile/link validation and physical test execution are separate acceptance activities.
