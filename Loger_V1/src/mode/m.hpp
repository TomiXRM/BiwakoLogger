#ifndef M_HPP
#define M_HPP

#include <Arduino.h>
#include <./setup/pinDefs.hpp>
#include <./setup/dataDefs.hpp>

static void m_before() {
}

static void m_body() {
}

static void m_after() {
}

static Mode_t mode_m = {
    "MainMode",
    'M',
    m_before,
    m_body,
    m_after,
};

#endif