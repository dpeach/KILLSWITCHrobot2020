/**
 * Two Channel Receiver
 * Author: Shawn Hymel (SparkFun Electronics)
 * Date: Aug 24, 2017
 * 
 * Connect a TB6612FNG and RC (PWM) receiver to the Arduino.
 * Mixes two channels for arcade drive.
 * 
 * This code is beerware; if you see me (or any other SparkFun 
 * employee) at the local, and you've found our code helpful, 
 * please buy us a round! 
 * Distributed as-is; no warranty is given.
 * 
 * (dpeach) Much of this code's modifications came from another
 * SparkFun project written by Nick Poole.
 * 
 * The main difference for my code and Shawn's is that I am
 * using a different h-bridge. I have the L9110S which only has
 * VCC, GND and then 2 control pins for each motor (A1/A2,
 * B1/B2).
 * 
 */

// Controller pins
const int CH_1_PIN = 2;
const int CH_2_PIN = 3;
const int CH_3_PIN = 4;
const int CH_4_PIN = 5;

// Motor driver pins
const int motorA1 = 7; // Motor A leg 1
const int motorA2 = 8; // Motor A leg 2
const int motorB1 = 9; // Motor B leg 1
const int motorB2 = 10; // Motor B leg 2

// Parameters
const int deadzone = 15;  // Anything between -20 and 20 is stop

void setup() {

  // Configure pins
  pinMode(motorA1, OUTPUT);
  pinMode(motorA2, OUTPUT);
  pinMode(motorB1, OUTPUT);
  pinMode(motorB2, OUTPUT);

  // Input pins; dpeach added this based on other code
  pinMode(CH_1_PIN, INPUT);
  pinMode(CH_2_PIN, INPUT);
  pinMode(CH_3_PIN, INPUT);
  pinMode(CH_4_PIN, INPUT);

  Serial.begin(9600);
}

void loop() {

  // Read pulse width from receiver
  int y = pulseIn(CH_2_PIN, HIGH, 25000);
  int x = pulseIn(CH_4_PIN, HIGH, 25000);

  // Convert to PWM value (-255 to 255)
  y = pulseToPWM(y);
  x = pulseToPWM(x);
  Serial.println(x);
  Serial.println(y);

  // Mix for arcade drive
  int left = y + x;
  int right = y - x;

  // Drive motor
  drive(left, right);

  delay(5);
}

// Positive for forward, negative for reverse
void drive(int speed_a, int speed_b) {

  // Limit speed between -255 and 255
  speed_a = constrain(speed_a, -255, 255);
  speed_b = constrain(speed_b, -255, 255);

  // Set direction for motor A
  if ( speed_a == 0 ) {
    digitalWrite(motorA1, LOW);
    digitalWrite(motorA2, LOW);
  } else if ( speed_a > 0 ) {
    digitalWrite(motorA1, HIGH);
    digitalWrite(motorA2, LOW);
  } else {
    digitalWrite(motorA1, LOW);
    digitalWrite(motorA2, HIGH);
  }

  // Set direction for motor B
  if ( speed_b == 0 ) {
    digitalWrite(motorB1, LOW);
    digitalWrite(motorB2, LOW);
  } else if ( speed_b > 0 ) {
    digitalWrite(motorB1, HIGH);
    digitalWrite(motorB2, LOW);
  } else {
    digitalWrite(motorB1, LOW);
    digitalWrite(motorB2, HIGH);
  }

  // Set speed
  analogWrite(motorA1, abs(speed_a));
  analogWrite(motorB1, abs(speed_b));
  
}

// Convert RC pulse value to motor PWM value
int pulseToPWM(int pulseIn) {
  
  // If we're receiving numbers, convert them to motor PWM
  if ( pulseIn > 1000 ) {
    pulseIn = map(pulseIn, 1000, 2000, -500, 500);
    pulseIn = constrain(pulseIn, -255, 255);
  } else {
    pulseIn = 0;
  }

  // Anything in deadzone should stop the motor
  if ( abs(pulseIn) <= deadzone ) {
    pulseIn = 0;
  }

  return pulseIn;
}