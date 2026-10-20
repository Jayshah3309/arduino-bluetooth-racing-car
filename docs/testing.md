# Validation and evidence

## Status

New reference design. Actual motors, battery, driver module, Bluetooth app, physical speed/range and racing results are unverified. No original car photo/video has been supplied.

Host tests cover disarmed startup, arming, directions, speed selection, timeout, unsupported commands, stop input, rearming and reversal timing including clock wraparound. GitHub Actions runs these tests and compiles for UNO. Both jobs passed in [verified run #1](https://github.com/Jayshah3309/arduino-bluetooth-racing-car/actions/runs/39404990908), commit `793f129`, on 20 October 2026. The Arduino setup action emitted a runtime deprecation warning; it did not fail the build. A successful build does not establish hardware performance.

## Acceptance checklist

Record date, tester, board, firmware commit, AVR core, motor ratings/stall current, driver model/jumpers, power source, HC-05 baud and controller app/protocol.

| ID | Procedure | Expected | Actual |
|---|---|---|---|
| R1 | Boot with raised wheels | No motion; disarmed | Not run |
| R2 | Send F without A | Remains disabled | Not run |
| R3 | Send 3, A, then repeated F | Both motors forward at commanded PWM 85 | Not run |
| R4 | Repeat B/L/R separately | Expected reverse/pivot polarity | Not run |
| R5 | Send S / X | Stop / stop and disarm | Not run |
| R6 | Stop motion stream | Disable/disarm at about 500 ms plus loop latency | Not run |
| R7 | Disconnect/reconnect Bluetooth | No autonomous resumption without newly delivered A and direction | Not run |
| R8 | Open stop loop, then close | Stop/disarm; closing alone cannot restart | Not run |
| R9 | Request opposite wheel direction | Measure disabled interval >=80 ms before opposite output | Not run |
| R10 | Send unsupported/lowercase byte | Stop/disarm | Not run |
| R11 | Select speed while moving | Stop until new motion byte; selection does not renew watchdog | Not run |
| R12 | Controlled low-speed floor tests | Record traction, stopping distance and behaviour | Not run |
| R13 | Motor start/stall-current assessment using suitable equipment | No exceeded motor/driver/supply/thermal ratings | Not run |

Test electrical direction-change timing with suitable instruments; wheel motion has inertia. Do not infer braking distance from the timeout constant. Test timeout with an actually stopped command stream; buffered serial bytes may continue arriving after RF interruption. Diagnose whether the app auto-sends commands on reconnection.

Use `measurements.csv` for measured latency, current, stopping distance and range, specifying conditions and trial count. Invalid/missed commands, resets and overheated parts are results to report, not discard. No measured maximum speed, radio range, runtime or competition claim is made.
