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

class sensor1_t {
  public:
    sensor1_t(int id, const char *name, const char *unit) {
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

    sensor1_t &operator=(float val) {
        this->f = val;
        return *this;
    }
    operator float() {
        return this->f;
    }

    void print() {
        Serial.printf("%s:%.2f[%s] \t id:%d\n", this->name, this->f, this->unit, this->id);
    }

    void printByts() {
        Serial.printf("%d %d %d %d\n", this->u8[0], this->u8[1], this->u8[2], this->u8[3]);
    }
};

class sensor3_t {
  public:
    sensor3_t(int id, const char *name, const char *unit)
        : x(id + 1, "X", unit),
          y(id + 2, "Y", unit),
          z(id + 3, "Z", unit) {
        this->id = id;
        this->name = name;
    }
    int id;
    const char *name;
    sensor1_t x;
    sensor1_t y;
    sensor1_t z;

    void print() {
        Serial.printf("%sX:%.2f[%s] \t id:%d\n", this->name, this->x.f, this->x.unit, this->x.id);
        Serial.printf("%sY:%.2f[%s] \t id:%d\n", this->name, this->y.f, this->y.unit, this->y.id);
        Serial.printf("%sZ:%.2f[%s] \t id:%d\n", this->name, this->z.f, this->z.unit, this->z.id);
    }
};

class sensor4_t {
  public:
    sensor4_t(int id, const char *name, const char *unit)
        : w(id + 1, "W", unit),
          x(id + 2, "X", unit),
          y(id + 3, "Y", unit),
          z(id + 4, "Z", unit) {
        this->id = id;
        this->name = name;
    }
    int id;
    const char *name;
    sensor1_t w;
    sensor1_t x;
    sensor1_t y;
    sensor1_t z;

    void print() {
        Serial.printf("%sW:%.2f[%s] \t id:%d\n", this->name, this->w.f, this->w.unit, this->w.id);
        Serial.printf("%sX:%.2f[%s] \t id:%d\n", this->name, this->x.f, this->x.unit, this->x.id);
        Serial.printf("%sY:%.2f[%s] \t id:%d\n", this->name, this->y.f, this->y.unit, this->y.id);
        Serial.printf("%sZ:%.2f[%s] \t id:%d\n", this->name, this->z.f, this->z.unit, this->z.id);
    }
};

#endif