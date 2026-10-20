# Upload and controlled bring-up

## Firmware

Use Arduino IDE 2.x, Arduino AVR Boards core 1.8.6 and Arduino Uno. Open `firmware/racing_car/racing_car.ino` and keep `control.h` beside it. Select the actual port and upload with motor power OFF. No third-party libraries are required. USB Serial Monitor: 115200 baud. Record tool versions, commit and board in the test report.

## Bluetooth

Confirm HC-05 breakout supply, RX logic level, data UART baud and host compatibility from its exact documentation. The sketch uses 9600 baud in normal data mode, not AT configuration mode. Pair using the module's documented process; do not assume a default PIN or universal phone support. Use an SPP controller/terminal that sends exact uppercase ASCII bytes. No particular app is installed or included.

The app must repeat F/B/L/R every 100–200 ms while a button is held, send S on release, and support A to arm and X to disarm. A one-shot direction app will stop after the 500 ms timeout; this is expected, not a reason to disable the timeout. Extra text, lowercase commands or unsupported button-release bytes disarm.

## Wheels-raised test

1. Inspect power rails, driver jumpers, flyback diodes, enable pulldowns, divider and common grounds with power disconnected.
2. Raise/secure the chassis so wheels cannot move it. Keep an accessible motor-power switch.
3. Verify D2's normally closed loop. Boot should remain disarmed; no motion before A and a direction command.
4. Select low speed with `3` (PWM 85), send `A`, then repeated `F`. Verify wheel polarity. Use `S` to stop, `X` to disarm.
5. Test B/L/R separately. Reversal should include an approximately 80 ms disable interval, which must be measured physically.
6. Stop command repetition/disconnect Bluetooth: motion should time out, disable and disarm. Reconnection alone must not be treated as a fresh arming decision; ensure the app does not automatically send A plus motion.
7. Open the D2 loop: outputs should disable. Closing it alone must not restart motion; A and a fresh direction are required.
8. Only after stationary checks, evaluate low-speed operation in a clear controlled area. Measure stopping distance rather than assuming bridge disable instantly stops the car.

## Diagnostics

CSV serial columns: `ms,armed,stop_closed,speed,left,right`.

Left/right are signed commanded PWM after reversal guards, not encoder readings. Negative means reverse, positive forward, zero disabled. Speed digits set integer PWM `digit*255/9`. Digit 0 selects zero. Speed selection also stops the current motion until a new direction arrives. Serial input is diagnostics only; USB commands do not drive the car.

## Troubleshooting

| Symptom | Check |
|---|---|
| No motion | Stop loop, A arming, repeated command, nonzero speed, EN jumper removal, motor power |
| Stops after half a second | App sends only once; enable command repetition |
| Unexpected disarm | Extra/lowercase protocol bytes, open stop loop, expired command stream |
| One wheel reversed | Motor polarity or inversion constants |
| Low torque / hot driver | Motor stall demand, L298 drop, supply sag, thermal limits |
| UNO resets on acceleration | Supply/ground noise, motor current, protection and decoupling |
| Bluetooth paired but no commands | Data baud, RX/TX crossover, divider, host SPP support |

The software watchdog monitors receipt of motion bytes, not HC-05 STATE, pairing status or an authenticated operator. Queued/replayed valid commands are not distinguished from new commands. Do not claim loss-of-link certification.
