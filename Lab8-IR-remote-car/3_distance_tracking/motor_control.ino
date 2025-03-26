/*
 * This file contains functions for controlling the car motors
 * for common driving operations
 */


void forward(int speed) {
  digitalWrite(L_DIR, HIGH);
  analogWrite(L_EN,speed);
  digitalWrite(R_DIR, LOW);
  analogWrite(R_EN,speed);
}

void backward(int speed) {
  digitalWrite(L_DIR, LOW);
  analogWrite(L_EN,speed);
  digitalWrite(R_DIR, HIGH);
  analogWrite(R_EN,speed);
}

void right(int speed) { 
  digitalWrite(L_DIR, HIGH); 
  analogWrite(L_EN,speed); 
  digitalWrite(R_DIR, HIGH); 
  analogWrite(R_EN,speed); 
}

void left(int speed) {
  digitalWrite(L_DIR, LOW);
  analogWrite(L_EN,speed);
  digitalWrite(R_DIR, LOW);
  analogWrite(R_EN,speed);
}

void stop() {
  digitalWrite(L_EN,LOW);
  digitalWrite(R_EN,LOW);
}