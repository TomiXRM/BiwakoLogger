#ifndef DATADEFS_HPP
#define DATADEFS_HPP
#include <Arduino.h>

typedef struct {
    char *modeName;
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
    } value;
    int id;
    const char *name;
    const char *unit;

    sensor1_t &operator=(float val) {
        this->value.f = val;
        return *this;
    }
    operator float() {
        return this->value.f;
    }
};

class sensors3_t {
  public:
    sensors3_t(int id, const char *name, const char *unit)
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
};

class sensors4_t {
  public:
    sensors4_t(int id, const char *name, const char *unit)
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
};

// print関数
// void printXYZValues(sensors3_t &sensor) {
//     printf("%s%s:%.2f[%s] \t id:%d\n", sensor.name, sensor.x.name, (float)sensor.x, sensor.x.unit, sensor.x.id);
//     printf("%s%s:%.2f[%s] \t id:%d\n", sensor.name, sensor.y.name, (float)sensor.y, sensor.y.unit, sensor.y.id);
//     printf("%s%s:%.2f[%s] \t id:%d\n", sensor.name, sensor.z.name, (float)sensor.z, sensor.z.unit, sensor.z.id);
// }
// void printValue(sensor1_t &sensor) {
//     printf("%s%s:%.2f[%s] \t id:%d\n", sensor.name, (float)sensor, sensor.unit, sensor.id);
// }

// void printBytes(sensor1_t &sensor) {
//     printf("%d %d %d %d\n", sensor.value.u8[0], sensor.value.u8[1], sensor.value.u8[2], sensor.value.u8[3]);
// }

#endif