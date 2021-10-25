#include "constants.hpp"
#include "hardware/gpio.h"
#include "pico/binary_info.h"
#include "pico/stdlib.h"
#include "pico/time.h"
#include "pum.hpp"
#include "tusb.h"

#include <cstdint>
#include <cstdio>

absolute_time_t startTime;
absolute_time_t currentTime;

// Variable that holds the current position in the sequence.
uint32_t note_pos = 0;

// Store example melody as an array of note values
uint8_t note_sequence[] = {
    74, 78, 81, 86,  90, 93, 98, 102, 57, 61,  66, 69, 73, 78, 81, 85,
    88, 92, 97, 100, 97, 92, 88, 85,  81, 78,  74, 69, 66, 62, 57, 62,
    66, 69, 74, 78,  81, 86, 90, 93,  97, 102, 97, 93, 90, 85, 81, 78,
    73, 68, 64, 61,  56, 61, 64, 68,  74, 78,  81, 86, 90, 93, 98, 102};

namespace midi {
constexpr uint8_t cable_num = 0; // MIDI jack associated with USB endpoint
constexpr uint8_t channel = 0;   // 0 for channel 1
} // namespace midi

void midi_task() {
  // The MIDI interface always creates input and output port/jack descriptors
  // regardless of these being used or not. Therefore incoming traffic should be
  // read (possibly just discarded) to avoid the sender blocking in IO
  uint8_t packet[4];
  while (tud_midi_available()) {
    tud_midi_packet_read(packet);
  }

  // send note every 1000 ms
  currentTime = get_absolute_time();
  if (us_to_ms(absolute_time_diff_us(startTime, currentTime)) < 1000) {
    return;
  }
  startTime = get_absolute_time();

  // Previous positions in the note sequence.
  int previous = note_pos - 1;

  // If we currently are at position 0, set the
  // previous position to the last note in the sequence.
  if (previous < 0) {
    previous = sizeof(note_sequence) - 1;
  }

  // Send Note On for current position at full velocity (127) on channel 1.
  uint8_t note_on[3] = {0x90 | midi::channel, note_sequence[note_pos], 127};
  tud_midi_stream_write(midi::cable_num, note_on, 3);

  // Send Note Off for previous note.
  uint8_t note_off[3] = {0x80 | midi::channel, note_sequence[previous], 0};
  tud_midi_stream_write(midi::cable_num, note_off, 3);

  // Increment position
  note_pos++;

  // If we are at the end of the sequence, start over.
  if (note_pos >= sizeof(note_sequence)) {
    note_pos = 0;
  }
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+
// Invoked when device is mounted
void tud_mount_cb(void) {}

// Invoked when device is unmounted
void tud_umount_cb(void) {}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en) { (void)remote_wakeup_en; }

// Invoked when usb bus is resumed
void tud_resume_cb(void) {}

//--------------------------------------------------------------------+
// main
//--------------------------------------------------------------------+
int main() {
  // Project discripsion.
  bi_decl(bi_program_description("Simple USB-MIDI."));

  stdio_init_all();
  tusb_init(); // Init tinyUSB

  // Init GPIO
  gpio_init(pin::led);
  gpio_set_dir(pin::led, GPIO_OUT);

  startTime = get_absolute_time();

  while (1) {
    std::printf("Hello\n");
    tud_task();
    midi_task();
  }
}