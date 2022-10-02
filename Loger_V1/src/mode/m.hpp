#ifndef M_HPP
#define M_HPP

#include <Arduino.h>
#include <./setup/pinDefs.hpp>
#include <./setup/dataDefs.hpp>

void m_before() {
}

void m_body() {
}

void m_after() {
}

Mode_t mode_m = {
    'M',
    m_before,
    m_body,
    m_after,
};

#endif