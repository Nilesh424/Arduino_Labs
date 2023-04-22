#include <Wire.h>
#include <rgb_lcd.h>
#include <Ultrasonic.h>
#include <math.h>

#define Light_Sensor (A2)
#define Temperature_Sensor (A0)
int Sound_Sensor= A1;
int batman =	2;
#define Touch_Sensor (3)
int Vibration_Motor	=	5;
int Led_Pin	=	6;
int Buzzer=7;

rgb_lcd lcd;
Ultrasonic ultrasonic(2);

// int lightCalibrationValue = 0;
// int soundCalibrationValue = 0;
// int distanceCalibrationValue = 0;
// int visibleLuxThreshold = 0;
float temp_sum = 0;
float light_sum = 0;
float sound_sum = 0;
float distance_sum = 0;
int num_readings = 0;
float temp_avg = 0;
// float light_avg = 0;
// float sound_avg = 0;
// float distance_avg = 0;
unsigned long touchDuration;

// // define constants for calibration time and delay
// const unsigned long CALIBRATION_TIME = 5000; // 5 seconds
// const unsigned long PASSIVE_DELAY_TIME = 2000; // 2 seconds
// const unsigned long ACTIVE_DELAY_TIME = 30; // 30 milliseconds

// define variables to store calibrated values
float calibratedTemp;
float calibratedLight;
float calibratedSound;
float calibratedDistance; 

// define variable to store current mode (0 for passive, 1 for active)
int currentMode = 0;

//funcs


float getTemperature(int t){
  const int B = 4275;         // B value of the thermistor
  const int R0 = 100000;
  delay(100);      // R0 = 100k
  int a = analogRead(t);    // Integer: 0-1023
  float R = 1023.0/a-1.0;
  R = R0*R;
  float temperature = 1.0/(log(R/R0)/B+1/298.15)-273.15;  // convert to temperature via datasheet
  return temperature;
}



float getLight() {
  float op = analogRead(Light_Sensor);
  // float po = map(op, 0, 800, 0, 10);
  delay(1000);
  return op;
}// gets light reads


int readSoundLevel() {
    // read sound sensor value
    int soundValue = analogRead(A1);

    // map sound sensor value to a more user-friendly range (0-100)
    int soundLevel = map(soundValue, 0, 1023, 0, 100);

    return soundLevel;
}



bool isCalibrationSuccessful() {
  return (calibratedLight != 0 && calibratedSound != 0 && calibratedDistance != 0 && calibratedTemp != 0);
}

unsigned long readTouchDuration() {
    // check if touch sensor is pressed
    if (digitalRead(4) == HIGH) {
        // record start time of touch
        unsigned long touchStartTime = millis();

        // wait until touch sensor is released
        while (digitalRead(4) == HIGH);

        // caltouchculate duration of touch in milliseconds
        touchDuration = millis() - touchStartTime;

        // return touchDuration;
    } else {
        return 0;
    }
}


void touch_func(){
    // unsigned long touchDuration = readTouchDuration();
    if (touchDuration > 0) {
        Serial.print("Touch duration: ");
        Serial.print(touchDuration);
        Serial.println(" ms");
    }
    
    if (touchDuration >= 3000) {
        vibrateMotor(200,1500);
        Serial.println("Active Mode Active");
        currentMode = 1;
        lcd.setRGB(255, 0,0);
    }
    else if (touchDuration>0 && touchDuration<=2000){
      vibrateMotor(200,500);
      delay(1000);
      vibrateMotor(200,500);
      Serial.println("Passive Mode");
      currentMode = 0;
      lcd.setColorWhite();
      }
    // else if(currentMode=1 && touchDuration=2000){ 
    //   vibrateMotor(255,100);
    //   delay(100);
    //   vibrateMotor(255,100);
    //   Serial.println("Passive Mode v2");
    //   currentMode=0;
    //   lcd.setColorWhite();
    // }
}

long batman_ear(){
  long RangeInCentimeters;
  RangeInCentimeters = ultrasonic.MeasureInCentimeters();
  delay(150);
  return RangeInCentimeters;
}

void vibrateMotor(int speed, int duration) {
  analogWrite(Vibration_Motor, speed); // set the motor speed
  delay(duration); // vibrate the motor for the specified duration in milliseconds
  analogWrite(Vibration_Motor, 0); // stop the motor
}


void triggerAlarm(String ee) {
  // turn on the red LED bulb and generate a buzzer tone
  for (int i = 0; i < 5; i++) {
    digitalWrite(6, HIGH); // turn on the LED bulb
    digitalWrite(7,HIGH);   // generate a tone for 500 ms
    delay(300); // wait for 300 ms
    digitalWrite(6, LOW); // turn off the LED bulb
    digitalWrite(7,LOW); 
    delay(300); // wait for 300 ms
    lcd.clear();
    lcd.print(ee);
  }
}


void calib_sensor(){
  lcd.print("Calibrating...");
  for (int i = 0; i < 6; i++) {
    // Read the sensor values
    float temp = getTemperature(A0);
    float light = getLight();
    float sound = readSoundLevel();
    float distance = batman_ear();

    // Add the values to the running total
    temp_sum += temp;
    light_sum += light;
    sound_sum += sound;
    distance_sum += distance;
    num_readings++;

    // Delay between readings
    delay(1000);
  }

  // Compute the average values
  calibratedTemp = temp_sum / num_readings;
  calibratedLight = light_sum / num_readings;
  calibratedSound = sound_sum / num_readings;
  calibratedDistance = distance_sum / num_readings;
  lcd.clear();
  lcd.print("Calibration Successful");
}


void calib_values_check(){ 
  if (isCalibrationSuccessful()) {
    lcd.clear();
    lcd.print("Calibration");
    lcd.setCursor(0, 1);
    lcd.print("Successful");
  } 
  else {
    lcd.clear();
    lcd.print("Calibration");
    lcd.setCursor(0, 1);
    lcd.print("Failure, adjust");
    delay(1000);
    lcd.clear();
    lcd.print("Arduino position");
  }}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(Temperature_Sensor, INPUT);
  pinMode(Light_Sensor, INPUT);
  pinMode(3, INPUT);
  pinMode(Led_Pin, OUTPUT);
  pinMode(Vibration_Motor, OUTPUT);
  lcd.begin(16, 2);
  lcd.setRGB(255, 0, 0); // Red
  lcd.print("Passive Mode");
  currentMode=0;
  delay(2000);
  lcd.setRGB(255, 255, 255);
  lcd.clear();
  calib_sensor();
}



void loop() {
  // put your main code here, to run repeatedly:
  readTouchDuration();
  touch_func();
  if (currentMode==0){ 
    // Passive monitoring mode
    delay(2000);  // Delay of 2 seconds between readings
    float current_temperature = getTemperature(A0);
    int current_light_intensity = getLight();
    int current_sound_level = readSoundLevel();

    // Check if the current temperature is within the optimal range
    if (current_temperature < 10 || current_temperature > 35) {
      // Temperature is outside the optimal range
      // Alert the user by flashing the backlight display between red and white
      int color = WHITE;
      for (int i = 0; i < 5; i++) {
        lcd.setColor(color);        // Set the color of the backlight display
        delay(750);                              // Delay of 750 ms between color changes
        color = (color == WHITE) ? RED : WHITE;  // Toggle between red and white
      }
    }

    // Check if the current sound level is high
    if (current_sound_level > 25) {
      // Sound level is high
      // Alert the user by flashing the backlight display between red and white
      int color = WHITE;
      for (int i = 0; i < 5; i++) {
        lcd.setColor(color);       // Set the color of the backlight display
        delay(750);     // Delay of 750 ms between color changes
        color = (color == WHITE) ? RED : WHITE;  // Toggle between red and white
      }
    }

    // Display the current sensor values on the backlight display
    lcd.clear();
    lcd.setCursor(0, 0);  // set the cursor to the first line
    lcd.print("T: ");
    lcd.print(current_temperature);
    lcd.print(" C    ");
    lcd.setCursor(0, 1);  // set the cursor to the second line
    lcd.print("L: ");
    lcd.print(current_light_intensity);
    lcd.print(" lx    ");
    lcd.setCursor(10, 1);  // move the cursor to the 11th column on the second line
    lcd.print("S: ");
    lcd.print(current_sound_level);
    lcd.print(" dB");
  }
  else if (currentMode==1) {
    //active monitoring
    float current_temperature2 = getTemperature(A0);
    int current_light_intensity2 = getLight();
    int current_distance2 = batman_ear();
    // Check for any abrupt changes in the environment
    if (current_temperature2 - calibratedTemp > 10) {
       // Abrupt change in temperature
      // Trigger the alarm
      // trigger_alarm("Temperature change");
      triggerAlarm("alarm temp");
      // Serial.println("alarm temp");
    } 
    else if (current_light_intensity2 - calibratedLight > 500) {
      // Abrupt change in light intensity
      // Trigger the alarm
      // trigger_alarm("Light change");
      triggerAlarm("alarm light");

    } else if (calibratedDistance - current_distance2 > 10) {
      // Abrupt change in distance
      // Trigger the alarm
      // trigger_alarm("Movement detected");
      triggerAlarm("alarm move");
    }

    // Display the current sensor values on the backlight display
    lcd.clear();
    lcd.setCursor(0, 0);  // set the cursor to the first line
    lcd.print("T: ");
    lcd.print(current_temperature2);
    lcd.print(" C    ");
    lcd.setCursor(0, 1);  // set the cursor to the second line
    lcd.print("L: ");
    lcd.print(current_light_intensity2);
    lcd.print(" lx    ");
    lcd.setCursor(10, 1);  // move the cursor to the 11th column on the second line
    lcd.print("D: ");
    lcd.print(current_distance2);
    lcd.print("cm");
    Serial.println(calibratedDistance);
  }
  else{
    Serial.print(":Flops");
  }
}
