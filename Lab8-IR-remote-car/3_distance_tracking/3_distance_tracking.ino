#include <IRremote.hpp> // include the library
#include <MPU9255.h>

#define IR_RECEIVE_PIN 3

#define L_DIR 2
#define L_EN 5
#define R_DIR 4
#define R_EN 6

MPU9255 mpu;

void setup() {
  pinMode(L_DIR, OUTPUT);
  pinMode(L_EN, OUTPUT);
  pinMode(R_DIR, OUTPUT);
  pinMode(R_EN, OUTPUT);


  Serial.begin(115200);
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  mpu.init();
  mpu.set_acc_scale(scale_2g);
}

char remoteDecoder(long int raw_ir_data) {
  char command = '\0';
  switch (raw_ir_data) {
    case 0xB946FF00:
      command = 'U';
      break;
    case 0xEA15FF00:
      command = 'D';
      break;
    case 0xBC43FF00:
      command = 'R';
      break;
    case 0xBB44FF00:
      command = 'L';
      break;
    case 0xBF40FF00:
      command = 'O';
      break;
    default:
      Serial.print("Unrecognized Command:");
      Serial.println(raw_ir_data, HEX);
  }
  return command;
}

void forward(int speed){
  digitalWrite(L_DIR, HIGH);
  analogWrite(L_EN,speed);
  digitalWrite(R_DIR, LOW);
  analogWrite(R_EN,speed);
}


void backwards(int speed){
  digitalWrite(L_DIR, LOW);
  analogWrite(L_EN,speed);
  digitalWrite(R_DIR, HIGH);
  analogWrite(R_EN,speed);
}


void stop(){
  analogWrite(L_EN,0);
  analogWrite(R_EN,0);
}


void left(int speed) {
  digitalWrite(L_DIR, LOW);
  analogWrite(L_EN,speed);
  digitalWrite(R_DIR, LOW);
  analogWrite(R_EN,speed);
}


void right(int speed){ 
  digitalWrite(L_DIR, HIGH); 
  analogWrite(L_EN,speed); 
  digitalWrite(R_DIR, HIGH); 
  analogWrite(R_EN,speed); 
}


void controlMotor(char command, int speed){ 
  // call the appropriate motor control function based on what command is recieved 
  switch(command){ 
    case 'F': 
      forward(speed); 
      break; 
    case 'B': 
      backwards(speed); 
      break; 
    case 'O': 
      stop(); 
      break; 
    case 'L': 
      left(speed); 
      break; 
    case 'R': 
      right(speed); 
      break; 
  } 

} 



void loop() {
  static long long current_time = micros();
  static long long prev_time = current_time;
  static long long elapsed_time = current_time - prev_time;

  prev_time = current_time;
  current_time = micros();
  elapsed_time = current_time - prev_time;

  mpu.read_acc();
  float ax = mpu.ax;
  float ay = mpu.ay;





  if (IrReceiver.decode()) {
    long int raw_ir_data = IrReceiver.decodedIRData.decodedRawData;
    if (raw_ir_data != 0) {
      Serial.println(raw_ir_data, HEX);
      Serial.println(raw_ir_data, BIN);

      char command = remoteDecoder(raw_ir_data);
      Serial.print("Command is: ");
      Serial.println(command);
    }

    IrReceiver.resume();
  }
}
