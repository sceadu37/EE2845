#include <IRremote.h> // include the library

#define IR_RECEIVE_PIN 3

void setup() {
    Serial.begin(115200);
    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
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

void loop() {
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
