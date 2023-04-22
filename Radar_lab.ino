#include <Ultrasonic.h>
#include <Wire.h>
#include <Servo.h>
#include <math.h>
#define MAX_SERVO_ANGLE 180
#define MIN_SERVO 0

Servo myServo;
int pin_servo = A1;
int pin_ultra = 7;
Ultrasonic ultrasonic(pin_ultra);

#define MAX_RESULTS 180

struct reading_angle {
  int angle;
  int reading;
};

reading_angle value_pair(long (*test_func)(), int test) {
  reading_angle result;//value pair reading/angle
  result.angle = test;
  result.reading = test_func();
  return result;
}

reading_angle results1[MAX_RESULTS];
reading_angle results2[MAX_RESULTS];

long read_ultra(){
  long RangeInCentimeters;
  RangeInCentimeters = ultrasonic.MeasureInCentimeters();
  return RangeInCentimeters;
}

void servo_sweep_read() {
  int result_count = 0;
  for (int i = 0; i <= 179; i++) { 
    myServo.write(i);
    delay(80);
    results1[result_count] = value_pair(read_ultra, i);
    int x = map(results1[result_count].angle,44,244,0,180);
    Serial.print(x + ',');
    Serial.print(",");
    Serial.print(results1[result_count].reading + '.');
    Serial.print(".");
    result_count++;
  }
  for (int j = 180; j >= 0; j--) {
    myServo.write(j);
    delay(80);
    results2[result_count] = value_pair(read_ultra, j); 
    int y = map(results2[result_count].angle,244,44,180,0);
    Serial.print(y + ',');
    Serial.print(",");
    Serial.print(results2[result_count].reading + '.');
    Serial.print(".");
    result_count++;
  }
}


void setup(){
    Serial.begin(9600); 
    myServo.attach(pin_servo);
    myServo.write(0);
}

void loop(){ 
   servo_sweep_read();
}