/* New Arduino UNO + HC-05 + L298N reference firmware.
 * Proposed wiring only; hardware verification pending. No autonomous driving.
 * Requires A to arm, repeated direction commands and closed stop-input loop.
 */
#include <SoftwareSerial.h>
#include "control.h"
constexpr uint8_t BT_RX = 10; // UNO receives HC-05 TX
constexpr uint8_t BT_TX = 11; // UNO TX through 1k/2k divider to HC-05 RX
constexpr uint8_t EN_LEFT = 5;
constexpr uint8_t EN_RIGHT = 6;
constexpr uint8_t IN_LEFT_A = 4;
constexpr uint8_t IN_LEFT_B = 7;
constexpr uint8_t IN_RIGHT_A = 8;
constexpr uint8_t IN_RIGHT_B = 9;
constexpr uint8_t STOP_INPUT = 2; // Normally closed contact to GND
constexpr bool INVERT_LEFT = false;
constexpr bool INVERT_RIGHT = false;
SoftwareSerial bluetooth(BT_RX, BT_TX);
racing::Controller controller;
racing::ReversalGuard leftGuard;
racing::ReversalGuard rightGuard;
int16_t appliedLeft = 32767;
int16_t appliedRight = 32767;
uint32_t lastLog = 0;
void writeMotor(uint8_t en, uint8_t a, uint8_t b, int16_t value) {
  analogWrite(en, 0); // Disable before changing direction
  digitalWrite(a, value > 0 ? HIGH : LOW);
  digitalWrite(b, value < 0 ? HIGH : LOW);
  analogWrite(en, uint8_t(value < 0 ? -value : value));
}
void setup() {
  pinMode(EN_LEFT, OUTPUT); pinMode(EN_RIGHT, OUTPUT);
  analogWrite(EN_LEFT, 0); analogWrite(EN_RIGHT, 0);
  pinMode(IN_LEFT_A, OUTPUT); pinMode(IN_LEFT_B, OUTPUT);
  pinMode(IN_RIGHT_A, OUTPUT); pinMode(IN_RIGHT_B, OUTPUT);
  digitalWrite(IN_LEFT_A, LOW); digitalWrite(IN_LEFT_B, LOW);
  digitalWrite(IN_RIGHT_A, LOW); digitalWrite(IN_RIGHT_B, LOW);
  pinMode(STOP_INPUT, INPUT_PULLUP);
  Serial.begin(115200); bluetooth.begin(9600);
  Serial.println(F("DISARMED reference firmware; wheels raised for first test."));
  Serial.println(F("ms,armed,stop_closed,speed,left,right"));
}
void loop() {
  const uint32_t now = millis();
  const bool stopClosed = digitalRead(STOP_INPUT) == LOW;
  if (!stopClosed) controller.disarm();
  // Bound parsing work so the stop input and watchdog are revisited frequently.
  for (uint8_t count = 0; count < 16 && bluetooth.available(); ++count)
    controller.receive(char(bluetooth.read()), now, stopClosed);
  const racing::Pair target = controller.output(now, stopClosed);
  const int16_t left = leftGuard.update(INVERT_LEFT ? -target.left : target.left, now);
  const int16_t right = rightGuard.update(INVERT_RIGHT ? -target.right : target.right, now);
  if (left != appliedLeft) {
    writeMotor(EN_LEFT, IN_LEFT_A, IN_LEFT_B, left); appliedLeft = left;
  }
  if (right != appliedRight) {
    writeMotor(EN_RIGHT, IN_RIGHT_A, IN_RIGHT_B, right); appliedRight = right;
  }
  if (racing::elapsed(now, lastLog, 500)) {
    lastLog = now;
    Serial.print(now); Serial.print(','); Serial.print(controller.armed());
    Serial.print(','); Serial.print(stopClosed);
    Serial.print(','); Serial.print(controller.speed());
    Serial.print(','); Serial.print(left); Serial.print(','); Serial.println(right);
  }
}
