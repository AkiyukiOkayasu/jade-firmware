/**
    @file midiCallback.hpp
    @author Akiyuki Okayasu
*/

#pragma once

#include "constants.hpp"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "pum.hpp"

#include <cassert>
#include <cstdio>

//==============================================================================
//Variables
uint8_t i2cDataBuffer[64] {};

//==============================================================================
//Functions
namespace
{
/** [0, 15]の範囲の2つの数をビット演算し[0, 255]にして返す.
    @param msb [0, 15]
    @param lsb [0, 15]
    @return constexpr uint8_t [0, 255]
    @note SysExの仕組み上128以上の数は送信できないのでI2Cの1byteを2byteに分割して送信している
*/
constexpr uint8_t compute8bitValue (const uint8_t msb, const uint8_t lsb)
{
    return (msb << 4) | (lsb & 0x0F);
}
} // namespace

//==============================================================================
inline void noteOnCallback (pum::Note note)
{
    std::printf ("noteOn: %d, %d, %dch\n", note.noteNumber, note.velocity, note.channel);
}

inline void noteOffCallback (pum::Note note)
{
    std::printf ("noteOff: %d, %dch\n", note.noteNumber, note.channel);
}

inline void controlChangeCallback (pum::ControlChange cc)
{
    std::printf ("cc: %d, %d, %dch\n", cc.controlNumber, cc.value, cc.channel);
}

inline void sysExCallback (const uint8_t data[], const uint8_t size)
{
    std::printf ("sysEx: size_%d\n", size);
    if (data[0] != pum::SysEx::ManufacturerID::NON_COMMERCIAL && data[1] != pum::SysEx::DeviceID::JADE)
    {
        return;
    }

    const uint8_t addr = compute8bitValue (data[2], data[3]);
    uint8_t i2cArrayLength = 0;
    for (uint8_t i = 4; i < size; i += 2)
    {
        i2cDataBuffer[i2cArrayLength] = compute8bitValue (data[i], data[i + 1]);
        i2cArrayLength++;
    }

    if (const int result = i2c_write_blocking (i2c0,
                                               addr,
                                               i2cDataBuffer,
                                               i2cArrayLength,
                                               false);
        result != PICO_OK)
    {
        std::printf ("I2C write error: %d\n", result);
    }
}