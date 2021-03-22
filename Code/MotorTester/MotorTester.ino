#include <Servo.h>

Servo rollServo;
Servo pitchServo;
Servo yawServo;

const int potentiometerPin{ A2 };

void setup() {
  Serial.begin(115200); // opens serial port, sets data rate to 9600 bps
  rollServo.attach(3);
  pitchServo.attach(5);
  yawServo.attach(9);
}

void loop() {
  auto outputAngle = map(analogRead(potentiometerPin), 0, 1023, 0, 180);
  Serial.println(outputAngle);
  rollServo.write(89);
  pitchServo.write(outputAngle);
  yawServo.write(0);
  delay(100);
}
