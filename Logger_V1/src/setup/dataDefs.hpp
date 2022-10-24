#ifndef DATADEFS_HPP
#define DATADEFS_HPP
#include <Arduino.h>

typedef struct {
    const char *modeName;
    char modeLetter;
    void (*before)();
    void (*body)();
    void (*after)();
} Mode_t;

class Sensor1_t {
  public:
    Sensor1_t(int id, const char *name, const char *unit) {
        this->id = id;
        this->name = name;
        this->unit = unit;
    }
    union {
        float f;
        uint32_t i;
        uint8_t u8[4];
    };
    int id;
    const char *name;
    const char *unit;

    Sensor1_t &operator=(float val) {
        this->f = val;
        return *this;
    }
    operator float() {
        return this->f;
    }

    void print() {
        Serial.printf("%s:%.2f[%s] \t id:%d\n", this->name, this->f, this->unit, this->id);
        // x:0.00[deg]  id:0x200
    }

    void printByts() {
        Serial.printf("%d %d %d %d\n", this->u8[0], this->u8[1], this->u8[2], this->u8[3]);
    }
};

class Sensor3_t {
  public:
    Sensor3_t(int id, const char *name, const char *unit)
        : x(id + 1, "X", unit),
          y(id + 2, "Y", unit),
          z(id + 3, "Z", unit) {
        this->id = id;
        this->name = name;
    }
    int id;
    const char *name;
    Sensor1_t x;
    Sensor1_t y;
    Sensor1_t z;

    void print() {
        Serial.printf("%sX:%.2f[%s] \t id:%d\n", this->name, this->x.f, this->x.unit, this->x.id);
        Serial.printf("%sY:%.2f[%s] \t id:%d\n", this->name, this->y.f, this->y.unit, this->y.id);
        Serial.printf("%sZ:%.2f[%s] \t id:%d\n", this->name, this->z.f, this->z.unit, this->z.id);
    }
};

class Sensor4_t {
  public:
    Sensor4_t(int id, const char *name, const char *unit)
        : w(id + 1, "W", unit),
          x(id + 2, "X", unit),
          y(id + 3, "Y", unit),
          z(id + 4, "Z", unit) {
        this->id = id;
        this->name = name;
    }
    int id;
    const char *name;
    Sensor1_t w;
    Sensor1_t x;
    Sensor1_t y;
    Sensor1_t z;

    void print() {
        Serial.printf("%sW:%.2f[%s] \t id:%d\n", this->name, this->w.f, this->w.unit, this->w.id);
        Serial.printf("%sX:%.2f[%s] \t id:%d\n", this->name, this->x.f, this->x.unit, this->x.id);
        Serial.printf("%sY:%.2f[%s] \t id:%d\n", this->name, this->y.f, this->y.unit, this->y.id);
        Serial.printf("%sZ:%.2f[%s] \t id:%d\n", this->name, this->z.f, this->z.unit, this->z.id);
    }
};

#endif