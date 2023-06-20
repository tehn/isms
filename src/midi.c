#include <stdint.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <rtmidi_c.h>

#define MAX_MIDI_DEVICES 32
#define MAX_PORT_NAME_LEN 256

static pthread_t p;

static struct RtMidiWrapper *poll_midi_in;
static struct RtMidiWrapper *poll_midi_out;

typedef struct {
  bool connected;
  bool add_event_sent;
  char name[MAX_PORT_NAME_LEN];
  int midi_in_port;
  struct RtMidiWrapper *midi_in;
  int midi_out_port;
  struct RtMidiWrapper *midi_out;
} midi_device_t;

static midi_device_t midi_devices[MAX_MIDI_DEVICES];

int find_connected_device(const char* name) {
    for (int i = 0; i < MAX_MIDI_DEVICES; i++) {
        if (strcmp(midi_devices[i].name, name) == 0) {
            return i;
        }
    }

    return -1;
}

int find_free_device() {
    for (int i = 0; i < MAX_MIDI_DEVICES; i++) {
        if (midi_devices[i].connected == false) {
            return i;
        }
    }

    return -1;
}

bool isms_port(const char* name) {
    return strstr(name, "isms") != NULL;
}

void *midi_poll_loop(void *x) {
    (void)x;

    char port_name[MAX_PORT_NAME_LEN];
    int port_name_len = MAX_PORT_NAME_LEN;

    char isms_port_name[MAX_PORT_NAME_LEN];

    while (true) {
        // scan connected devices
        for (int i = 0; i < MAX_MIDI_DEVICES; i++) {
            if (midi_devices[i].connected) {
                if (midi_devices[i].midi_in != NULL) {
                    rtmidi_get_port_name(poll_midi_in, midi_devices[i].midi_in_port, port_name, &port_name_len);
                } else {
                    rtmidi_get_port_name(poll_midi_out, midi_devices[i].midi_out_port, port_name, &port_name_len);
                }

                // clear a device if disconnected
                if (strcmp("", port_name) == 0 || strcmp(midi_devices[i].name, port_name) != 0) {
                    if (midi_devices[i].midi_in != NULL) {
                        rtmidi_close_port(midi_devices[i].midi_in);
                        rtmidi_in_free(midi_devices[i].midi_in);
                        midi_devices[i].midi_in_port = -1;
                        midi_devices[i].midi_in = NULL;
                    }
                    if (midi_devices[i].midi_out != NULL) {
                        rtmidi_close_port(midi_devices[i].midi_out);
                        rtmidi_in_free(midi_devices[i].midi_out);
                        midi_devices[i].midi_out_port = -1;
                        midi_devices[i].midi_out = NULL;
                    }

                    // TODO: send remove event

                    midi_devices[i].name[0] = 0;
                    midi_devices[i].connected = false;
                    midi_devices[i].add_event_sent = false;
                }
            }
        }

        // scan midi input ports

        int in_port_count = rtmidi_get_port_count(poll_midi_in);

        for (int i = 0; i < in_port_count; i++) {
            rtmidi_get_port_name(poll_midi_in, i, port_name, &port_name_len);

            if (isms_port(port_name)) {
                continue;
            }

            int device_index = find_connected_device(port_name);

            if (device_index < 0) {
                device_index = find_free_device();
            }

            if (midi_devices[device_index].midi_in == NULL) {
                midi_devices[device_index].midi_in = rtmidi_in_create_default();
                strcpy(midi_devices[device_index].name, port_name);

                snprintf(isms_port_name, sizeof(isms_port_name), "isms %d in", device_index);
                rtmidi_open_port(midi_devices[device_index].midi_in, i, isms_port_name);
                midi_devices[device_index].midi_in_port = i;
                midi_devices[device_index].connected = true;
            }
        }

        // scan midi output ports

        int out_port_count = rtmidi_get_port_count(poll_midi_out);

        for (int i = 0; i < out_port_count; i++) {
            rtmidi_get_port_name(poll_midi_out, i, port_name, &port_name_len);

            if (isms_port(port_name)) {
                continue;
            }

            int device_index = find_connected_device(port_name);

            if (device_index < 0) {
                device_index = find_free_device();
            }

            if (midi_devices[device_index].midi_out == NULL) {
                midi_devices[device_index].midi_out = rtmidi_out_create_default();
                strcpy(midi_devices[device_index].name, port_name);

                snprintf(isms_port_name, sizeof(isms_port_name), "isms %d out", device_index);
                rtmidi_open_port(midi_devices[device_index].midi_out, i, isms_port_name);
                midi_devices[device_index].midi_out_port = i;
                midi_devices[device_index].connected = true;
            }
        }

        // send device added events

        for (int i = 0; i < MAX_MIDI_DEVICES; i++) {
            if (midi_devices[i].connected && !midi_devices[i].add_event_sent) {
                // TODO: send add event
            }
        }

        usleep(1000000);
    }
}

void init_midi() {
    for (int i = 0; i < MAX_MIDI_DEVICES; i++) {
        midi_devices[i].connected = false;
        midi_devices[i].add_event_sent = false;
    }

    poll_midi_in = rtmidi_in_create_default();
    poll_midi_out = rtmidi_out_create_default();

    if (pthread_create(&p, NULL, midi_poll_loop, 0)) {
        fprintf(stderr, "midi: init fail pthread\n");
        exit(1);
    }

    pthread_setname_np(p, "midi");
}

struct RtMidiWrapper* midi_in_open(unsigned int port_number) {
    return NULL;
}
