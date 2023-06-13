#include <stdint.h>
#include <rtmidi_c.h>

static struct RtMidiWrapper *midi_in;
static struct RtMidiWrapper *midi_out;

static void midi_in_callback(double timeStamp, const unsigned char *message, size_t messageSize, void *userData) {
    // TODO: lua callback (handle_midi in interface.c?)
    // TODO: clock callback
}

void midi_send(const uint8_t *data, size_t n) {
    rtmidi_out_send_message(midi_out, data, n);
}

void init_midi() {
    midi_in = rtmidi_in_create_default();
    rtmidi_open_virtual_port(midi_in, "isms");
    rtmidi_in_set_callback(midi_in, midi_in_callback, NULL);

    midi_out = rtmidi_out_create_default();
    rtmidi_open_virtual_port(midi_out, "isms");     
}
