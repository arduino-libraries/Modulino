#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"

#include "Wire.h"
#include "fw.h"

// https://www.st.com/resource/en/application_note/an4221-i2c-protocol-used-in-the-stm32-bootloader-stmicroelectronics.pdf

int howmany;
int res;

ArduinoLEDMatrix matrix;


int command_write_page(uint8_t opcode, uint8_t* buf_cmd, size_t len_cmd, const uint8_t* buf_fw, size_t len_fw) {
  uint8_t cmd[2];
  cmd[0] = opcode;
  cmd[1] = 0xFF ^ opcode;
  Wire1.beginTransmission(100);
  Wire1.write(cmd, 2);
  if (len_cmd > 0) {
    buf_cmd[len_cmd - 1] = 0;
    for (int i = 0; i < len_cmd - 1; i++) {
      buf_cmd[len_cmd - 1] ^= buf_cmd[i];
    }
    Wire1.endTransmission(true);
    Wire1.requestFrom(100, 1);
    auto c = Wire1.read();
    if (c != 0x79) {
      Serial.print("error first ack: ");
      Serial.println(c, HEX);
      return -1;
    }
    Wire1.beginTransmission(100);
    Wire1.write(buf_cmd, len_cmd);
  }
  Wire1.endTransmission(true);
  Wire1.requestFrom(100, 1);
  auto c = Wire1.read();
  if (c != 0x79) {
    while (c == 0x76) {
      delay(10);
      Wire1.requestFrom(100, 1);
      c = Wire1.read();
    }
    if (c != 0x79) {
      Serial.print("error second ack: ");
      Serial.println(c, HEX);
      return -1;
    }
  }
  uint8_t tmpbuf[len_fw + 2] = { len_fw - 1 };
  memcpy(&tmpbuf[1], buf_fw, len_fw);
  for (int i = 0; i < len_fw + 1; i++) {
    tmpbuf[len_fw + 1] ^= tmpbuf[i];
  }
  Wire1.beginTransmission(100);
  Wire1.write(tmpbuf, len_fw + 2);
  Wire1.endTransmission(true);
  Wire1.requestFrom(100, 1);
  c = Wire1.read();
  if (c != 0x79) {
    while (c == 0x76) {
      delay(10);
      Wire1.requestFrom(100, 1);
      c = Wire1.read();
    }
    if (c != 0x79) {
      Serial.print("error: ");
      Serial.println(c, HEX);
      return -1;
    }
  }
final_ack:
  return howmany + 1;
}


int command(uint8_t opcode, uint8_t* buf_cmd, size_t len_cmd, uint8_t* buf_resp, size_t len_resp) {
  uint8_t cmd[2];
  cmd[0] = opcode;
  cmd[1] = 0xFF ^ opcode;
  Wire1.beginTransmission(100);
  Wire1.write(cmd, 2);
  if (len_cmd > 0) {
    Wire1.endTransmission(true);
    Wire1.requestFrom(100, 1);
    auto c = Wire1.read();
    if (c != 0x79) {
      Serial.print("error first ack: ");
      Serial.println(c, HEX);
      return -1;
    }
    Wire1.beginTransmission(100);
    Wire1.write(buf_cmd, len_cmd);
  }
  Wire1.endTransmission(true);
  Wire1.requestFrom(100, 1);
  auto c = Wire1.read();
  if (c != 0x79) {
    while (c == 0x76) {
      delay(100);
      Wire1.requestFrom(100, 1);
      c = Wire1.read();
      Serial.print("retry");
    }
    if (c != 0x79) {
      Serial.print("error second ack: ");
      Serial.println(c, HEX);
      return -1;
    }
  }
  int howmany = -1;
  if (len_resp == 0) {
    goto final_ack;
  }
  Wire1.requestFrom(100, len_resp);
  howmany = Wire1.read();
  for (int j = 0; j < howmany + 1; j++) {
    buf_resp[j] = Wire1.read();
  }

  Wire1.requestFrom(100, 1);
  c = Wire1.read();
  if (c != 0x79) {
    Serial.print("error: ");
    Serial.println(c, HEX);
    return -1;
  }
final_ack:
  return howmany + 1;
}

int sendReset() {
  uint8_t buf[3] = { 'D', 'I', 'E' };
  int ret;
  for (int i = 30; i < 100; i++) {
    Wire1.beginTransmission(i);
    ret = Wire1.endTransmission();
    if (ret != 2) {
      Wire1.beginTransmission(i);
      Wire1.write(buf, 40);
      ret = Wire1.endTransmission();
      return ret;
    }
  }
  return ret;
}

void setup() {
  Serial.begin(115200);
  Wire1.begin();
  Wire1.setClock(400000);

  if (sendReset() != 0) {
    Serial.println("Send reset failed");
  }

  uint8_t resp_buf[255];
  int resp;
  Serial.println("GET_COMMAND");
  resp = command(0, nullptr, 0, resp_buf, 20);

  if (resp < 0) {
    Serial.println("Failed :(");
    matrix.begin();
    matrix.beginDraw();

    matrix.stroke(0xFFFFFFFF);
    matrix.textScrollSpeed(100);

    const char text[] = "FAIL";
    matrix.textFont(Font_4x6);
    matrix.beginText(0, 1, 0xFFFFFF);
    matrix.println(text);
    matrix.endText();

    matrix.endDraw();
    while (1)
      ;
  }

  for (int i = 0; i < resp; i++) {
    Serial.println(resp_buf[i], HEX);
  }

  Serial.println("GET_ID");
  resp = command(2, nullptr, 0, resp_buf, 3);
  for (int i = 0; i < resp; i++) {
    Serial.println(resp_buf[i], HEX);
  }

  Serial.println("GET_ID");
  resp = command(2, nullptr, 0, resp_buf, 3);
  for (int i = 0; i < resp; i++) {
    Serial.println(resp_buf[i], HEX);
  }

  Serial.println("MASS_ERASE");
  uint8_t erase_buf[3] = { 0xFF, 0xFF, 0x0 };
  resp = command(0x44, erase_buf, 3, nullptr, 0);
  for (int i = 0; i < resp; i++) {
    Serial.println(resp_buf[i], HEX);
  }

  for (int i = 0; i < node_base_bin_len; i += 128) {
    Serial.print("WRITE_PAGE ");
    Serial.println(i, HEX);
    uint8_t write_buf[5] = { 8, 0, i / 256, i % 256 };
    resp = command_write_page(0x32, write_buf, 5, &node_base_bin[i], 128);
    for (int i = 0; i < resp; i++) {
      Serial.println(resp_buf[i], HEX);
    }
    delay(10);
  }

  matrix.begin();
  matrix.beginDraw();

  matrix.stroke(0xFFFFFFFF);
  matrix.textScrollSpeed(100);

  const char text[] = "PASS";
  matrix.textFont(Font_4x6);
  matrix.beginText(0, 1, 0xFFFFFF);
  matrix.println(text);
  matrix.endText();

  matrix.endDraw();

  //Wire1.beginTransmission(100);
  //if (Wire1.endTransmission() == 0) {
  //  Serial.println("Device found");
  //}
  /*

    uint8_t buf[2] = {0, 0xFF};
    Wire1.beginTransmission(100);
    Wire1.write(buf, 2);
    Wire1.endTransmission(true);
    Wire1.requestFrom(100, 1);
    while (!Wire1.available()) {
      delay(1);
    }
    uint8_t res = Wire1.read();
    Serial.println(res, HEX);

    Wire1.requestFrom(100, 20);
    uint8_t howmany = Wire1.read();
    Serial.println(howmany);
    for (int j = 0; j < howmany+1; j++) {
      Serial.println(Wire1.read(), HEX);
    }
    delay(100);
    buf[0] = 0x2; buf[1] = 0xFD;
    Wire1.beginTransmission(100);
    Wire1.write(buf, 2);
    Wire1.endTransmission(true);
    Wire1.requestFrom(100, 1);
    res = Wire1.read();
    Serial.println(res, HEX);
    if (res == 255)  {
    Serial.println("error");
    while (1);
    }

    Wire1.requestFrom(100, 3);
    howmany = Wire1.read();
    Serial.println(howmany);
    for (int j = 0; j < howmany + 1; j++) {
    Serial.println(Wire1.read(), HEX);
    }
  */
}

void loop() {
  // put your main code here, to run repeatedly:
}
