#include <stdint.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <rtmidi_c.h>

#include "event.h"

static struct RtMidiWrapper *midi_in;
static struct RtMidiWrapper *midi_out;

static void midi_in_callback(double t, const uint8_t *message, size_t n, void *user_data) {
    union event_data *ev = event_data_new(EVENT_MIDI);

    ev->midi.nbytes = n;

    for (uint8_t i = 0; i < n; i++) {
        ev->midi.data[i] = message[i];
    }

    event_post(ev);
}

void midi_send(const uint8_t *message, size_t n) {
    rtmidi_out_send_message(midi_out, message, n);
}

void init_midi() {
    midi_in = rtmidi_in_create_default();
    rtmidi_open_virtual_port(midi_in, "isms");
    rtmidi_in_set_callback(midi_in, midi_in_callback, NULL);

    midi_out = rtmidi_out_create_default();
    rtmidi_open_virtual_port(midi_out, "isms");
}
