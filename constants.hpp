#pragma once

#include "pico/stdlib.h"

#include <cstdint>

namespace pin
{
inline constexpr unsigned int led = PICO_DEFAULT_LED_PIN; // 25
inline constexpr unsigned int I2C0_SDA = 8;
inline constexpr unsigned int I2C0_SCL = 9;
} // namespace pin