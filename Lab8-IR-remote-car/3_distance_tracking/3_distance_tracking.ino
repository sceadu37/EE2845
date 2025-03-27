#include <IRremote.hpp> // include the library
#include <MPU9255.h>
#include <LiquidCrystal_I2C.h>
#include <debug_print.h>

#define TO_METERS_PER_SEC ((1 << 14) / 9.81)
#define SZ_DAT_BUF 16

#define IR_RECEIVE_PIN 3
#define L_DIR 2
#define L_EN 5
#define R_DIR 4
#define R_EN 6

MPU9255 mpu;
LiquidCrystal_I2C lcd(0x27, 16, 2);

char remoteDecoder(long int raw_ir_data) {
  char command = '\0';
  switch (raw_ir_data) {
    case 0xB946FF00:
      command = 'F';
      break;
    case 0xEA15FF00:
      command = 'B';
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


/*
 * dt:  pointer to buffer of elapsed times
 * acc: pointer to buffer of instantaneous accelerations
 * a:   pointer to double storing average acceleration
 * v:   pointer to double storing velocity
 * s:   pointer to double storing displacement
 */
double calc_displacement(uint16_t* dt, double* acc, /*double* a, */double* v) {
  double a = 0;
  double v_prev = *v;
  double T = 0;
  double s = 0;
  for (byte i = 0; i < SZ_DAT_BUF; i++) {
    a += acc[i] * dt[i] / 1000.0;
    T += dt[i] / 1000.0;
  }
  *v = a;
  a /= T;
  s = v_prev * T + 0.5 * a * T*T;
  return s;
}


void setup() {
  pinMode(L_DIR, OUTPUT);
  pinMode(L_EN, OUTPUT);
  pinMode(R_DIR, OUTPUT);
  pinMode(R_EN, OUTPUT);

  Serial.begin(115200);
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  mpu.init();
  mpu.set_acc_scale(scale_2g);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Hello World!");
}


void loop() {
  static long long current_time = millis();
  static long long prev_time = current_time;
  static uint16_t elapsed_time = current_time - prev_time;
  static uint16_t dt[SZ_DAT_BUF];
  static uint16_t* ptr_dt = dt;
  static double acc[SZ_DAT_BUF];
  static double* ptr_acc = acc;
  static char line_buffer[17];
  static char float_buffer1[12];
  static char float_buffer2[12];
  static double a = 0;
  static double v = 0;
  static double s = 0;

  current_time = millis();
  elapsed_time = current_time - prev_time;

  if (elapsed_time > 100) {
    prev_time = current_time;
    
    mpu.read_acc();
    double ax = double(mpu.ax) / TO_METERS_PER_SEC;
    double ay = double(mpu.ay) / TO_METERS_PER_SEC;
    double az = double(mpu.az) / TO_METERS_PER_SEC;
    PRINT_VAR(ax);
    PRINT_VAR(ay);
    PRINT_VAR(az);
    Serial.println();

    if ((ptr_dt - dt) % 3 == 0) {
      dtostrf(ax, 1, 2, float_buffer1);
      sprintf(line_buffer, "x:%s y:%s", float_buffer1, dtostrf(ay, 1, 2, float_buffer2));
      lcd.setCursor(0, 0);
      lcd.printstr(line_buffer);
    }

    // write data to buffers and increment pointers to next element
    *ptr_dt = elapsed_time;
    ptr_dt++;
    *ptr_acc = ay;
    ptr_acc++;
    // once data buffers are full, compute displacement and clear buffers
    if (ptr_dt - dt >= SZ_DAT_BUF) {
      s += calc_displacement(dt, acc, &v);
      dtostrf(s, 1, 2, float_buffer1);
      sprintf(line_buffer, "s:%s v:%s", float_buffer1, dtostrf(v, 1, 2, float_buffer2));
      lcd.setCursor(0, 1);
      lcd.printstr(line_buffer);
      ptr_dt = dt;
      ptr_acc = acc;
    }
  }

  if (IrReceiver.decode()) {
    long int raw_ir_data = IrReceiver.decodedIRData.decodedRawData;
    if (raw_ir_data != 0) {
      Serial.println(raw_ir_data, HEX);
      Serial.println(raw_ir_data, BIN);

      char command = remoteDecoder(raw_ir_data);
      Serial.print("Command is: ");
      Serial.println(command);
      controlMotor(command, 25);
    }
    IrReceiver.resume();
  }
}
