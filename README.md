# Arduino Bluetooth Racing Car

A manual differential-drive racing-car reference design using Arduino UNO R3, an HC-05 serial Bluetooth module and an L298N motor-driver module.

**Status:** newly authored reference code. The original repository was empty. Wiring, components and behaviour below are proposed, not recovered from your original car. Hardware, Bluetooth control, motor current and physical stopping distance remain unverified.

![Proposed reference connections](hardware/circuit.svg)

## Project purpose

Demonstrate wireless control of a small two-motor robot with explicit arming, speed selection, direction commands and automatic motor disable when fresh motion commands stop arriving. It is a manual-control prototype, not an autonomous vehicle or a validated competition configuration.

## Reference features

- Forward, reverse and pivot-left/right control.
- Nine nonzero PWM levels plus zero speed.
- Starts disarmed; an explicit `A` arms without moving.
- 500 ms command timeout that stops and disarms.
- Stop/disarm commands and a normally closed stop-input loop.
- 80 ms disabled interval before reversing each motor's direction.
- Serial diagnostics; no third-party Arduino libraries required.

PWM is a duty-cycle setting, not measured wheel speed. Stop disables the bridges (coast); it does not guarantee immediate physical stopping. The stop input is software monitored, not an independent emergency power cutoff.

## Proposed components

| Component | Quantity | Requirement |
|---|---:|---|
| Arduino UNO R3 | 1 | ATmega328P, 5 V logic |
| HC-05 breakout | 1 | Verify supply and 9600-baud data mode |
| L298N module | 1 | Exposes ENA/ENB, with flyback protection verified |
| Geared DC motors + wheels | 2 | Rated for chosen supply; stall current within driver limits |
| Chassis + caster | 1 set | Differential drive reference |
| Motor power source and switch | 1 | Voltage/current matched to actual motors and module |
| Regulated logic supply | As required | Follow wiring guide; no rail backfeeding |
| Normally closed stop switch | 1 | GPIO2 to GND loop |
| 1 kΩ + 2 kΩ resistors | 1 each | UNO TX to HC-05 RX divider |
| 10 kΩ resistors | 2 | ENA/ENB pulldowns after jumper removal |
| Wire, mounting hardware, USB cable | As needed | Secure power and common GND |

Two motors are the baseline. Four motors require verifying combined stall current per bridge, wiring and thermal performance; do not assume two motors can be paralleled safely on each channel.

## Protocol

| Byte | Action |
|---|---|
| `A` | Arm, remain stopped; stop loop must be closed |
| `F` / `B` | Forward / reverse |
| `L` / `R` | Pivot left / right |
| `S` | Stop; remain armed |
| `X` | Stop and disarm |
| `0`–`9` | Select PWM = digit ×255/9; stop until fresh direction command |
| Newline, carriage return, space | Ignore; do not renew timeout |
| Any other byte | Stop and disarm |

Send a direction command at least every **100–200 ms while held**. At 500 ms without one, the car stops and disarms; send `A` again before moving. Speed/arm/stop bytes are not keepalives. Configure your app for exact uppercase single-byte commands; a generic phone app is not automatically compatible. Bluetooth SPP host compatibility must be checked; no phone app or voice-recognition implementation is included.

## Setup and documentation

- [Exact wiring and power](hardware/wiring.md)
- [Upload, pairing and wheels-raised tests](docs/setup.md)
- [Architecture and timing](docs/architecture.md)
- [Test plan and current evidence](docs/testing.md)
- [Measurement worksheet](docs/measurements.csv)
- [Limitations and next steps](docs/roadmap.md)
- [Contributors](docs/contributors.md) and [media instructions](media/README.md)

Arduino IDE: select Arduino Uno, open `firmware/racing_car/racing_car.ino` with `control.h` beside it, select the correct port and upload. SoftwareSerial is included in Arduino AVR Boards. USB diagnostics use 115200 baud; HC-05 data UART is configured as 9600 baud.

CLI:

```sh
arduino-cli core update-index
arduino-cli core install arduino:avr@1.8.6
arduino-cli compile --fqbn arduino:avr:uno firmware/racing_car
```

Host tests:

```sh
g++ -std=c++11 -Wall -Wextra -Werror tests/control_test.cpp -o /tmp/racing-car-test
/tmp/racing-car-test
```

GitHub Actions runs host tests and builds for UNO. These checks do not validate the physical car, RF link, power delivery, traction or stopping distance.

## Results and future scope

No measured speed, range, current, race time or braking performance is claimed. Original project photos/video and exact hardware need to be supplied. Voice control, obstacle avoidance, line following, encoders, closed-loop speed control and autonomous modes are not implemented. See the roadmap for staged improvements.

## Credits and license

Owner: [Jayshah3309](https://github.com/Jayshah3309). Reference package prepared with AI assistance; actual project contributions and hardware evidence remain to be confirmed. License selection is pending; [LICENSE](LICENSE) is only a status notice.

## Technical references

- [Arduino UNO R3](https://docs.arduino.cc/hardware/uno-rev3/)
- [SoftwareSerial](https://docs.arduino.cc/learn/built-in-libraries/software-serial/)
- [Arduino PWM guide](https://support.arduino.cc/hc/en-us/articles/9350537961500-Use-PWM-output-with-Arduino)
- [ST L298 datasheet](https://www.st.com/resource/en/datasheet/l298.pdf)

The L298 chip datasheet does not define every third-party breakout's regulator, jumpers or ratings. Inspect your exact module and HC-05 documentation before wiring.
