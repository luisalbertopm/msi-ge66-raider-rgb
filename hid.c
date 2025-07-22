#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include "hid.h"

#include "keyboard.inc"
#include "aurora.inc"

const device_preset_t device_preset_pairs[][2] = {{keyboard_preset_default, aurora_preset_default}, {keyboard_preset_dragon_shield, aurora_preset_dragon_shield}, {keyboard_preset_chakra, aurora_preset_chakra}, {keyboard_preset_disco, aurora_preset_disco}, {keyboard_preset_free_way, aurora_preset_macaw}, {keyboard_preset_drain, aurora_preset_rainbow}, {keyboard_preset_aqua, aurora_preset_blue_flash}, {keyboard_preset_rainbow_split, aurora_preset_rainbow}, {keyboard_preset_plain, aurora_preset_plain}, {keyboard_preset_disable, aurora_preset_disable}};
int device_preset_pairs_size = sizeof(device_preset_pairs) / sizeof(device_preset_pairs[0]);

const device_preset_t *device_search_preset(const device_preset_t *device_presets[], int size, char *name)
{
    for (int i = 0; i < size; i++)
    {
        const device_preset_t *device_preset = device_presets[i];
        if (strcmp(device_preset->name, name) == 0)
            return device_preset;
    }
    return NULL;
}

void device_set_preset(hid_device *device, const device_preset_t *preset)
{
    int error;
    int preset_packets = preset->size / sizeof(const unsigned char *);
    for (int i = 0; i < preset_packets; i++)
    {
        const unsigned char *data = preset->data[i];
        if (i < preset_packets - 1)
        {
            int data_length = 524;
            error = hid_send_feature_report(device, data + 36, data_length);
        }
        else
        {
            int data_length = 64;
            error = hid_write(device, data + 36, data_length);
        }
        if (error < 0)
            wprintf(L"%ls\n", hid_error(device));
        usleep(10000);
    }
}
