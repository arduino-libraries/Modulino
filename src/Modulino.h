#include "Wire.h"
#include <vector>
#include <VL53L1X.h>  // from Poulou
#include <Arduino_APDS9960.h>
#include "Arduino_BMI270_BMM150.h"
#include <Arduino_LPS22HB.h>
#include <Arduino_HS300x.h>
//#include <SE05X.h>  // need to provide a way to change Wire object

class ModulinoClass {
public:
  void begin(HardwareI2C& wire = Wire1) {
#ifdef ARDUINO_UNOR4_WIFI
    // unlock Wire1 bus at begin since we don't know the state of the system
    pinMode(WIRE1_SCL_PIN, OUTPUT);
    for (int i = 0; i < 20; i++) {
      digitalWrite(WIRE1_SCL_PIN, HIGH);
      digitalWrite(WIRE1_SCL_PIN, LOW);
    }
#endif
    _wire = &wire;
    _wire->begin();
    _wire->setClock(100000);
  }
  friend class Module;
  friend class Distance;
protected:
  HardwareI2C* _wire;
};

extern ModulinoClass Modulino;

class Module : public Printable {
public:
  Module(uint8_t address = 0xFF, char* name = "")
    : address(address), name(name) {}
  bool begin() {
    if (address == 0xFF) {
      address = discover() / 2;  // divide by 2 to match address in fw main.c
    }
  }
  virtual uint8_t discover() {
    return 0xFF;
  }
  operator bool() {
    return address != 0xFF;
  }
  static HardwareI2C* getWire() {
    return Modulino._wire;
  }
  bool read(uint8_t* buf, int howmany) {
    if (address == 0xFF) {
      return false;
    }
    Modulino._wire->requestFrom(address, howmany + 1);
    auto start = millis();
    while ((Modulino._wire->available() == 0) && (millis() - start < 100)) {
      delay(1);
    }
    if (Modulino._wire->available() < howmany) {
      return false;
    }
    pinstrap_address = Modulino._wire->read();
    for (int i = 0; i < howmany; i++) {
      buf[i] = Modulino._wire->read();
    }
    while (Modulino._wire->available()) {
      Modulino._wire->read();
    }
    return true;
  }
  bool write(uint8_t* buf, int howmany) {
    if (address == 0xFF) {
      return false;
    }
    Modulino._wire->beginTransmission(address);
    for (int i = 0; i < howmany; i++) {
      Modulino._wire->write(buf[i]);
    }
    Modulino._wire->endTransmission();
    return true;
  }
  bool nonDefaultAddress() {
    return (pinstrap_address != address);
  }
  virtual size_t printTo(Print& p) const {
    return p.print(name);
  }
  bool scan(uint8_t addr) {
    Modulino._wire->beginTransmission(addr / 2);  // multply by 2 to match address in fw main.c
    auto ret = Modulino._wire->endTransmission();
    if (ret == 0) {
      // could also ask for 1 byte and check if it's truely a modulino of that kind
      return true;
    }
    return false;
  }
private:
  uint8_t address;
  uint8_t pinstrap_address;
  char* name;
};

class Buttons : public Module {
public:
  Buttons(uint8_t address = 0xFF)
    : Module(address, "BUTTONS") {}
  bool get(bool& a, bool& b, bool& c) {
    uint8_t buf[3];
    auto res = read((uint8_t*)buf, 3);
    a = buf[0];
    b = buf[1];
    c = buf[2];
    auto ret = res && (a != last_a || b != last_b || c != last_c);
    last_a = a;
    last_b = b;
    last_c = c;
    return ret;
  }
  void set(bool a, bool b, bool c) {
    uint8_t buf[3];
    buf[0] = a;
    buf[1] = b;
    buf[2] = c;
    write((uint8_t*)buf, 3);
    return;
  }
  virtual uint8_t discover() {
    for (int i = 0; i < match.size(); i++) {
      if (scan(match[i])) {
        return match[i];
      }
    }
  }
private:
  bool last_a, last_b, last_c;
protected:
  std::vector<uint8_t> match = { 0x7C };  // same as fw main.c
};

class Tone : public Module {
public:
  Tone(uint8_t address = 0xFF)
    : Module(address, "BUZZER") {}
  void tone(size_t freq, size_t len_ms) {
    uint8_t buf[8];
    memcpy(&buf[0], &freq, 4);
    memcpy(&buf[4], &len_ms, 4);
    write(buf, 8);
  }
  void noTone() {
    uint8_t buf[8];
    memset(&buf[0], 0, 8);
    write(buf, 8);
  }
  virtual uint8_t discover() {
    for (int i = 0; i < match.size(); i++) {
      if (scan(match[i])) {
        return match[i];
      }
    }
  }
protected:
  std::vector<uint8_t> match = { 0x3C };  // same as fw main.c
};

class ModulinoColor {
public:
  ModulinoColor(uint8_t r, uint8_t g, uint8_t b)
    : r(r), g(g), b(b) {}
  operator uint32_t() {
    return (b << 8 | g << 16 | r << 24);
  }
private:
  uint8_t r, g, b;
};

class LEDS : public Module {
public:
  LEDS(uint8_t address = 0xFF)
    : Module(address, "LEDS") {
    memset((uint8_t*)data, 0xE0, NUMLEDS * 4);
  }
  bool begin() {
    Module::begin();
  }
  void set(int idx, ModulinoColor rgb, uint8_t brightness = 128) {
    data[idx] = (uint32_t)rgb | brightness | 0xE0;
  }
  void set(int idx, uint8_t r, uint8_t g, uint8_t b, uint8_t brightness = 128) {
    set(idx, ModulinoColor(r,g,b), brightness);
  }
  void clear(int idx) {
    set(idx, ModulinoColor(0,0,0), 0);
  }
  void clear() {
    memset((uint8_t*)data, 0xE0, NUMLEDS * 4);
  }
  void show() {
    write((uint8_t*)data, NUMLEDS * 4);
  }
  virtual uint8_t discover() {
    for (int i = 0; i < match.size(); i++) {
      if (scan(match[i])) {
        return match[i];
      }
    }
  }
private:
  static const int NUMLEDS = 8;
  uint32_t data[NUMLEDS];
protected:
  std::vector<uint8_t> match = { 0x6C };
};


class Encoder : public Module {
public:
  Encoder(uint8_t address = 0xFF)
    : Module(address, "ENCODER") {}
  int16_t get() {
    uint8_t buf[3];
    auto res = read(buf, 3);
    if (res == false) {
      return 0;
    }
    _pressed = (buf[2] != 0);
    int16_t ret = buf[0] | (buf[1] << 8);
    return ret;
  }
  bool pressed() {
    get();
    return _pressed;
  }
  virtual uint8_t discover() {
    for (int i = 0; i < match.size(); i++) {
      if (scan(match[i])) {
        return match[i];
      }
    }
  }
private:
  bool _pressed = false;
protected:
  std::vector<uint8_t> match = { 0x74, 0x76 };
};

#define RETURN_IF_NOT_ZERO(func) do { int ret = func; if (ret != 0) { return 0; } } while (0)

class APDS9999 {
public:
  APDS9999(TwoWire& wire)
    : _wire(&wire) {}
  int begin() {
    RETURN_IF_NOT_ZERO(writeRegister(0x0, 0x6));
    RETURN_IF_NOT_ZERO(writeRegister(0x1, 0x73));
    RETURN_IF_NOT_ZERO(writeRegister(0x2, 0x0));
    RETURN_IF_NOT_ZERO(writeRegister(0x5, 0x4));
    return 1;
  }
  int available() {
    uint8_t buf[1] = { 0x7 };
    _wire->beginTransmission(address);
    _wire->write(buf, sizeof(buf));
    auto ret = _wire->endTransmission(false);
    if (ret == 0) {
      _wire->requestFrom(address, 1);
      auto res = (_wire->read() & (1 | (1 << 3)));
      return res;
    }
    return 0;
  }
  int read(int& r, int& g, int& b) {
    int uv;
    return read(r, g, b, uv);
  }
  int read(int& r, int& g, int& b, int& uv) {
    uint8_t buf[1] = { 0xA };
    _wire->beginTransmission(address);
    _wire->write(buf, sizeof(buf));
    auto ret = _wire->endTransmission(false);
    if (ret == 0) {
      _wire->requestFrom(address, 4 * 3);  // 4 registers * 3 bytes each
      uint8_t resp_buf[12];
      for (int i = 0; i < sizeof(resp_buf); i++) {
        resp_buf[i] = _wire->read();
      }
      // resultion is 18bit, compress to 8
      uv = (resp_buf[0] | (resp_buf[1] << 8) | (resp_buf[2] << 16)) >> 10;
      g = (resp_buf[3] | (resp_buf[4] << 8) | (resp_buf[5] << 16)) >> 10;
      b = (resp_buf[6] | (resp_buf[7] << 8) | (resp_buf[8] << 16)) >> 10;
      r = (resp_buf[9] | (resp_buf[10] << 8) | (resp_buf[11] << 16)) >> 10;
    }
  }
  int read(int& light) {
    uint8_t buf[1] = { 0x8 };
    _wire->beginTransmission(address);
    _wire->write(buf, sizeof(buf));
    auto ret = _wire->endTransmission(false);
    if (ret == 0) {
      _wire->requestFrom(address, 2);  // 4 registers * 3 bytes each
      return _wire->read() | ((_wire->read() & 0x3) << 8);
    }
    return 0;
  }
private:
  TwoWire* _wire;
  const uint8_t address = 0x52;
  int writeRegister(uint8_t reg, uint8_t value) {
    _wire->beginTransmission(address);
    _wire->write(reg);
    _wire->write(value);
    auto ret = _wire->endTransmission();
    return ret;
  }
};


extern ModulinoColor RED;
extern ModulinoColor BLUE;
extern ModulinoColor GREEN;
extern ModulinoColor VIOLET;
extern ModulinoColor WHITE;

extern ModulinoClass Modulino;

/*
extern Buttons buttons;
extern Tone buzzer;
extern LEDS leds;
extern Encoder encoder;
*/
extern BoschSensorClass imu;
extern VL53L1X tof_sensor;
extern APDS9999 color;  // TODO: need to change to APDS9999 https://docs.broadcom.com/doc/APDS-9999-DS
extern LPS22HBClass barometer;
extern HS300xClass humidity;

class Distance : public Module {
public:
  bool begin() {
    tof_sensor.setBus((TwoWire*)getWire());
    tof_sensor.init();
    tof_sensor.setDistanceMode(VL53L1X::Short);
    tof_sensor.setMeasurementTimingBudget(50000);
    tof_sensor.startContinuous(50);
  }
  float get() {
    return tof_sensor.read();
  }
};