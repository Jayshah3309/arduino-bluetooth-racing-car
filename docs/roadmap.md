# Current limits and roadmap

## Implemented reference scope

Manual Bluetooth byte commands, PWM levels, arm/disarm, timeout, stop input, reversal guard and serial telemetry. No voice recognition, obstacle avoidance, line following, encoder feedback, speed PID, autonomous mode, app, chassis CAD or verified four-motor design is included.

## Next steps

1. Confirm original components, GPIOs and controller app; reconcile reference assumptions.
2. Validate power, motor polarity, enable control, protocol and timeout with wheels raised.
3. Add actual photos/video, confirmed contributions and observed test results.
4. Measure current/temperature, stopping distance, latency, radio behaviour and track performance.
5. Add whole-loop software tests and a simulation that distinguishes virtual signals from motor mechanics.
6. If justified: acceleration ramps, speed trim/encoders, framed commands and connection-state handling.
7. Voice/obstacle modes require additional hardware/software, explicit mode transitions and their own tests; avoid presenting them as current functionality.

A future battery design must specify cells, protection, charging, current capability and mounting. A real hardware emergency cutoff must be engineered separately from D2's software-monitored input. Do not call a release hardware-tested until evidence exists.
