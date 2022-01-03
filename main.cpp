#include "SEGGER_RTT.h"
#include "constants.hpp"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "midiCallback.hpp"
#include "pico/binary_info.h"
#include "pico/stdlib.h"
#include "pico/time.h"
#include "pum.hpp"
#include "tusb.h"

#include <cstdint>
#include <cstdio>

//==============================================================================
//Constants
namespace
{
// Store example melody as an array of note values
inline constexpr uint8_t note_sequence[] = {
    74,
    78,
    81,
    86,
    90,
    93,
    98,
    102,
    57,
    61,
    66,
    69,
    73,
    78,
    81,
    85,
    88,
    92,
    97,
    100,
    97,
    92,
    88,
    85,
    81,
    78,
    74,
    69,
    66,
    62,
    57,
    62,
    66,
    69,
    74,
    78,
    81,
    86,
    90,
    93,
    97,
    102,
    97,
    93,
    90,
    85,
    81,
    78,
    73,
    68,
    64,
    61,
    56,
    61,
    64,
    68,
    74,
    78,
    81,
    86,
    90,
    93,
    98,
    102
};
} // namespace

//==============================================================================
//Variables
namespace
{
absolute_time_t startTime;
absolute_time_t currentTime;
uint32_t note_pos = 0; // Variable that holds the current position in the sequence.

namespace midi
{
    constexpr uint8_t cableNum = 0; // MIDI jack associated with USB endpoint
    constexpr uint8_t channel = 0;  // 0 for channel 1
    pum::Parser parser {};
    pum::Generator generator { channel, cableNum };
} // namespace midi
} // namespace

//==============================================================================
//Functions
void midi_task()
{
    // The MIDI interface always creates input and output port/jack descriptors
    // regardless of these being used or not. Therefore incoming traffic should be
    // read (possibly just discarded) to avoid the sender blocking in IO
    uint8_t packet[4];
    while (tud_midi_available())
    {
        tud_midi_packet_read (packet);
        midi::parser.parse (packet);
    }

    // send note every 1000 ms
    currentTime = get_absolute_time();
    if (us_to_ms (absolute_time_diff_us (startTime, currentTime)) < 1000)
    {
        return;
    }
    startTime = get_absolute_time();

    // Previous positions in the note sequence.
    int previous = note_pos - 1;

    // If we currently are at position 0, set the
    // previous position to the last note in the sequence.
    if (previous < 0)
    {
        previous = sizeof (note_sequence) - 1;
    }

    // Send Note On for current position at full velocity (127) on channel 1.
    midi::generator.makeNoteOn (packet, note_sequence[note_pos], 127);
    tud_midi_packet_write (packet);

    // Send Note Off for previous note.
    midi::generator.makeNoteOff (packet, note_sequence[previous]);
    tud_midi_packet_write (packet);

    // Increment position
    note_pos++;

    // If we are at the end of the sequence, start over.
    if (note_pos >= sizeof (note_sequence))
    {
        note_pos = 0;
    }
}

//==============================================================================
//USB callback functions
// Invoked when device is mounted
void tud_mount_cb (void) {}

// Invoked when device is unmounted
void tud_umount_cb (void) {}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb (bool remote_wakeup_en) { (void) remote_wakeup_en; }

// Invoked when usb bus is resumed
void tud_resume_cb (void) {}

//==============================================================================
//GPIO callback
void callback_gate1 (uint gpio, uint32_t events)
{
    //TODO 実装追加
}

//==============================================================================
void initI2C()
{
    i2c_init (i2c0, i2c::BAUDRATE);
    gpio_set_function (pin::I2C0_SCL, GPIO_FUNC_I2C);
    gpio_set_function (pin::I2C0_SDA, GPIO_FUNC_I2C);
    gpio_pull_up (pin::I2C0_SCL);
    gpio_pull_up (pin::I2C0_SDA);
}

void initGPIO()
{
    // Init GPIO
    gpio_init (pin::led);
    gpio_set_dir (pin::led, GPIO_OUT);

    //Gate inputs
    gpio_init (pin::gateIn1);
    gpio_set_dir (pin::gateIn1, GPIO_IN);
    gpio_pull_down (pin::gateIn1);
    gpio_set_irq_enabled_with_callback (pin::gateIn1, gpio::edgeHigh, true, callback_gate1);

    //PWM output
    gpio_set_function (pin::pwm0, GPIO_FUNC_PWM);
    gpio_set_function (pin::pwm1, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num (pin::pwm0);
    pwm_set_wrap (slice_num, 4095); //12bit 125MHz/4096=30.51758kHz
}

void initPeripherals()
{
    initGPIO();
    initI2C();
}

//--------------------------------------------------------------------+
// main
//--------------------------------------------------------------------+
int main()
{
    bi_decl (bi_program_description ("Simple USB-MIDI."));                      // Project discripsion.
    bi_decl (bi_2pins_with_func (pin::I2C0_SDA, pin::I2C0_SCL, GPIO_FUNC_I2C)); //I2C pins

    initPeripherals();

    midi::parser.setNoteOnCallback (noteOnCallback);
    midi::parser.setNoteOffCallback (noteOffCallback);
    midi::parser.setControlChangeCallback (controlChangeCallback);
    midi::parser.setSysExCallback (sysExCallback);

    stdio_init_all();
    tusb_init(); // Init tinyUSB

    startTime = get_absolute_time();
    uint32_t i = 0;

    while (1)
    {
        tud_task();
        midi_task();
        // SEGGER_RTT_WriteString (0, "Hello World from SEGGER!\r\n");
        SEGGER_RTT_printf (0, "RTT: %d\n", i);
        i++;
        if (i >= 256)
        {
            i = 0;
        }
    }
}