// Copyright (c) 2024 Arduino SA
// SPDX-License-Identifier: MPL-2.0

#include "Wire.h"
#include <vector>
#include <vl53l4cd_class.h>  // from stm32duino
#include "Arduino_LSM6DSOX.h"
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

class ModulinoButtons : public Module {
public:
  ModulinoButtons(uint8_t address = 0xFF)
    : Module(address, "BUTTONS") {}
  PinStatus isPressed(int index) {
    return last_status[index] ? HIGH : LOW;
  }
  bool update() {
    uint8_t buf[3];
    auto res = read((uint8_t*)buf, 3);
    auto ret = res && (buf[0] != last_status[0] || buf[1] != last_status[1] || buf[2] != last_status[2]);
    last_status[0] = buf[0];
    last_status[1] = buf[1];
    last_status[2] = buf[2];
    return ret;
  }
  void setLeds(bool a, bool b, bool c) {
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
  bool last_status[3];
protected:
  std::vector<uint8_t> match = { 0x7C };  // same as fw main.c
};

class ModulinoBuzzer : public Module {
public:
  ModulinoBuzzer(uint8_t address = 0xFF)
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

class ModulinoPixels : public Module {
public:
  ModulinoPixels(uint8_t address = 0xFF)
    : Module(address, "LEDS") {
    memset((uint8_t*)data, 0xE0, NUMLEDS * 4);
  }
  bool begin() {
    Module::begin();
  }
  void set(int idx, ModulinoColor rgb, uint8_t brightness = 25) {
    if (idx < NUMLEDS) {
      uint8_t _brightness = map(brightness, 0, 100, 0, 0x1F);
      data[idx] = (uint32_t)rgb | _brightness | 0xE0;
    }
  }
  void set(int idx, uint8_t r, uint8_t g, uint8_t b, uint8_t brightness = 5) {
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


class ModulinoKnob : public Module {
public:
  ModulinoKnob(uint8_t address = 0xFF)
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
  void set(int16_t value) {
    uint8_t buf[4];
    memcpy(buf, &value, 2);
    write(buf, 4);
  }
  bool isPressed() {
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

extern ModulinoColor RED;
extern ModulinoColor BLUE;
extern ModulinoColor GREEN;
extern ModulinoColor VIOLET;
extern ModulinoColor WHITE;

class ModulinoMovement : public Module {
public:
  bool begin() {
    if (_imu == nullptr) {
      _imu = new LSM6DSOXClass(*((TwoWire*)getWire()), 0x6A);
    }
    initialized = _imu->begin();
    return initialized != 0;
  }
  int update() {
    if (initialized) {
      return _imu->readAcceleration(x, y, z);
    }
    return 0;
  }
  float getX() {
    return x;
  }
  float getY() {
    return y;
  }
  float getZ() {
    return z;
  }
private:
  LSM6DSOXClass* _imu = nullptr;
  float x,y,z;
  int initialized = 0;
};

class ModulinoThermo: public Module {
public:
  bool begin() {
    if (_sensor == nullptr) {
      _sensor = new HS300xClass(*((TwoWire*)getWire()));
    }
    initialized = _sensor->begin();
    return initialized;
  }
  float getHumidity() {
    if (initialized) {
      return _sensor->readHumidity();
    }
    return 0;
  }
  float getTemperature() {
    if (initialized) {
      return _sensor->readTemperature();
    }
    return 0;
  }
private:
  HS300xClass* _sensor = nullptr;
  int initialized = 0;
};

class ModulinoPressure : public Module {
public:
  bool begin() {
    if (_barometer == nullptr) {
      _barometer = new LPS22HBClass(*((TwoWire*)getWire()));
    }
    initialized = _barometer->begin();
    if (initialized == 0) {
      // unfortunately LPS22HBClass calles Wire.end() on failure, restart it
      getWire()->begin();
    }
    return initialized != 0;
  }
  float getPressure() {
    if (initialized) {
      return _barometer->readPressure();
    }
    return 0;
  }
  float getTemperature() {
    if (initialized) {
      return _barometer->readTemperature();
    }
    return 0;
  }
private:
  LPS22HBClass* _barometer = nullptr;
  int initialized = 0;
};

class ModulinoLight : public Module {

};

class ModulinoDistance : public Module {
public:
  bool begin() {
    tof_sensor = new VL53L4CD((TwoWire*)getWire(), -1);
    tof_sensor->InitSensor();
    tof_sensor->VL53L4CD_SetRangeTiming(10, 0);
    tof_sensor->VL53L4CD_StartRanging();
  }
  float get() {
    VL53L4CD_Result_t results;
    uint8_t NewDataReady = 0;
    uint8_t status;
    do {
      status = tof_sensor->VL53L4CD_CheckForDataReady(&NewDataReady);
    } while (!NewDataReady);
    tof_sensor->VL53L4CD_ClearInterrupt();
    tof_sensor->VL53L4CD_GetResult(&results);
    return results.distance_mm;
  }
private:
  VL53L4CD* tof_sensor; ;
};