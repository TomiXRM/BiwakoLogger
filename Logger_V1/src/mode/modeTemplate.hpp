#ifndef TEMP_HPP
#define TEMP_HPP

#include <Arduino.h>
#include <./setup/pinDefs.hpp>
#include <./setup/dataDefs.hpp>

static void t_before() {
}

static void t_body() {
}

static void t_after() {
}

static Mode_t mode_t = {
    "MainMode",
    'M',
    t_before,
    t_body,
    t_after,
};

#endif