#include <Grove_LED_Bar.h>
Grove_LED_Bar bar(7, 6, 0, LED_BAR_10); // Clock pin, Data pin, Orientation

String COM_VOLCANO_LEVEL = "COM_VOLCANO_LEVEL";
String ACK_VOLCANO_LEVEL = "ACK_VOLCANO_LEVEL";

void setup(){
  Serial.begin( 9600 );
  bar.begin();
}

void loop(){
  if (Serial.available() > 0){  // Check if there is data available to read from the Serial port.
    String s_com = (Serial.readStringUntil('\n'));
    if(s_com == COM_VOLCANO_LEVEL){
      Serial.println(ACK_VOLCANO_LEVEL);
      while(1){
        if (Serial.available() > 0){  // Check if there is data available to read from the Serial port.
          String s_level = (Serial.readStringUntil('\n'));
          int level = s_level.toInt();
          int mapped_value = map(level, 0, 5, 10, 1);
          bar.setLevel(mapped_value);
          delay(100);
          Serial.println(ACK_VOLCANO_LEVEL);
          break; 
        }
      }
    }
  }
}



// #include <Grove_LED_Bar.h>
// #include <ArduinoJson.h>

// Grove_LED_Bar bar(7, 6, 0,LED_BAR_10); // Clock pin, Data pin, Orientation

// String COM_VOLCANO_LEVEL = "COM_VOLCANO_LEVEL";
// String ACK_VOLCANO_LEVEL = "ACK_VOLCANO_LEVEL";

// void setup() {
//   Serial.begin(9600);
//   bar.begin();
// }

// void loop() {
//   if (Serial.available() > 0) {  // Check if there is data available to read from the Serial port.
//     String s_com = (Serial.readStringUntil('\n'));
//     if (s_com == COM_VOLCANO_LEVEL) {
//       Serial.println(ACK_VOLCANO_LEVEL);
//       while (1) {
//         if (Serial.available() > 0) {  // Check if there is data available to read from the Serial port.
//           String s_json = Serial.readStringUntil('\n');
//           DynamicJsonDocument doc(1024);
//           DeserializationError error = deserializeJson(doc, s_json);
//           if (error) {
//             Serial.print(F("deserializeJson() failed: "));
//             Serial.println(error.c_str());
//           } else {
//             int level = doc["level"];
//             int mapped_value = map(level, 5, 0, 1, 10);
//             bar.setLevel(mapped_value);
//             delay(100);
//             Serial.println(ACK_VOLCANO_LEVEL);
//           }
//           break;
//         }
//       }
//     }
//   }
// }
