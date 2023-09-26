#include <Servo.h>
#include <Grove_LED_Bar.h>
#define MAX_SERVO_ANGLE 165
#define MIN_SERVO 30
Servo myservo;


Grove_LED_Bar bar(7, 6, 0, LED_BAR_10);


struct Command {
  bool isPosition;
  bool isAngle;
  int* positionValue;
  int* angleValue;
};

String inputString = "";
bool inputComplete = false;
int current_Angle=0;//
int current_Position = 0;
int previous_Angle = 0;
int previous_Postition = 0;

// Function to parse the input string into the Command struct
//takes 5 args, 4 pointers to global variables
// returns boolean values and postionvalue and angleValue being set based on input.
// It also sets the current position vaule i.e if mid postion = 5; The ledlvel.
//  and returns the modded struct. 
// tldr - the function takes inputstring and returns a modded struct and sets currentpostion and current angle;

Command parseInput(String input, int* current_angle, int* current_position,int* previous_angle,int* previous_postition) {
  Command command;
  command.isPosition = false;
  command.isAngle = false;
  command.positionValue = current_position;
  command.angleValue = current_angle;
  previous_angle = &previous_Angle;
  input.toUpperCase(); // case folding.
  //checking input.
  if (input.indexOf("UP") != -1) {
    command.isPosition = true;
    command.positionValue = &current_Position;
    // previous_angle = &previous_Angle;
    *command.positionValue = 10;
    current_Position = *command.positionValue;
    Serial.println("Blinds Position: UP");
  }
  else if (input.indexOf("MID") != -1) {
    command.isPosition = true;
    *command.positionValue = 5;
    current_Position = *command.positionValue;
    Serial.println("Blinds Position: MID");
  }
  else if (input.indexOf("DOWN") != -1) {
    command.isPosition = true;
    // command.positionValue = &current_Position;
    *command.positionValue = 0;
    current_Position = *command.positionValue;
    Serial.println("Blinds Position: DOWN");
  }//eror msg


  if (input.indexOf("SLAT") != -1) {
    command.isAngle = true;
    int startIndex = input.indexOf("SLAT") + 5;//pos of char after space is set as startIndex val.
    int endIndex = input.indexOf(",", startIndex);
    if (endIndex == -1) {
      endIndex = input.length();
    }
    int x = input.substring(startIndex, endIndex).toInt();
    if(x>=0 && x<=100){
      *command.angleValue = x;
      // previous_Angle = current_Angle;
      current_Angle = map(*command.angleValue,0,100,MIN_SERVO,MAX_SERVO_ANGLE);
    }
    else if (x > 100 || x < 0) { // if angle value is greater than 100 or less than 0
    // if out of bounds, go back to old angle and wait for input/
    Serial.println("Invalid angle input");
  }
  }
  return command; //ret modded struct
}


//set andle function, for redundancy ive mapped the values again. only called once thrice in main.
void setAngle(int angle) {
  // int servoAngle = map(angle, 0, 100, 30, 165);
  myservo.write(angle);
  delay(1000);
}


void setup() {
  Serial.begin(9600);
  bar.begin();
  myservo.attach(A0);
  bar.setLevel(1);
  Serial.println("Blinds Position: DOWN");//defaults. execs once.
  Serial.println("Slat Position:");
  Serial.println("30");
  setAngle(63.75);
}

void loop() {
  // Read serial input until a newline character is received
  
  
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    inputString += inChar; // this is arg of parse input
    if (inChar == '\n') {
      inputComplete = true; // set bool input completee once it hits new line char/
    }
  }

  // If a complete input string is received, parse it 
  if (inputComplete) {
    Command command = parseInput(inputString, &current_Position, &current_Position, &previous_Angle, &previous_Postition); //initiate and set struct via funct.

    if (command.isPosition) { // based on struct exec. eihter t/f and corresponding values,
      Serial.print("Setting blind position to :");
      Serial.println(*command.positionValue);
      bar.setLevel(0);//edits for prev leds.
      delay(1000);
      setAngle(97);// mid angle set
      delay(1000);
      bar.setLed(current_Position,255);
      delay(1000);// store ca as temp
      //curent as previous
      //prev gets temp 
      // int er = map(*command.angleValue, 0, 100, 30, 165);
      setAngle(current_Angle);
      Serial.println("Its now set to Postion");
      Serial.println(*command.positionValue);
    }
    if (command.isAngle) {
    int angle = *command.angleValue;
    if (angle < 0) {
      Serial.println("Error: Angle value cannot be negative.");
    }
    else if (angle > 100) {
      Serial.println("Error: Angle value cannot be greater than 100.");
    }
    else {
      Serial.print("Setting angle to: ");
      Serial.println(angle);
      Serial.println("In degrees:");
      Serial.println(current_Angle);
      setAngle(current_Angle);// updated in the parse input func.
    }
    }
    if(!command.isPosition && !command.isAngle){
      Serial.println("Invalid Command, retry.!");
    }
    inputString = "";
    inputComplete = false;
    // command.isPosition = false;
    // command.isAngle = false;
    // // command.angleValue = 0;
    // // command.positionValue = 0;
  }
}