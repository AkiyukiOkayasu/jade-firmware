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
