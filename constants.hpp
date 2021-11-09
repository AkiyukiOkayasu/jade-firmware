#pragma once

#include "pico/stdlib.h"

#include <cstdint>

namespace pin
{
inline constexpr unsigned int led = PICO_DEFAULT_LED_PIN; // 25
inline constexpr unsigned int I2C0_SDA = 8;
inline constexpr unsigned int I2C0_SCL = 9;
inline constexpr unsigned int gateIn1 = 10; //未定
} // namespace pin

namespace i2c
{
inline constexpr uint BAUDRATE = 100'000; //100kHz
}

namespace gpio
{
inline constexpr unsigned int lowlevel = 0b0000;
inline constexpr unsigned int highlevel = 0b0010;
inline constexpr unsigned int edgeLow = 0b0100;
inline constexpr unsigned int edgeHigh = 0b1000;
} // namespace gpio