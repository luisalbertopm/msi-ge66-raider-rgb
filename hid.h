#ifndef HID_H
#define HID_H

#include <hidapi/hidapi.h>

#define KEYBOARD_VENDOR_ID 0x1038  // SteelSeries
#define KEYBOARD_PRODUCT_ID 0x1122 // KLC
#define AURORA_VENDOR_ID 0x1038    // SteelSeries
#define AURORA_PRODUCT_ID 0x1132   // ALC

typedef struct device_preset
{
    const char *name;
    const unsigned char **data;
    const unsigned int size;
} device_preset_t;

extern const device_preset_t *keyboard_presets[];
extern int keyboard_presets_size;

extern const device_preset_t *aurora_presets[];
extern int aurora_presets_size;

extern const device_preset_t device_preset_pairs[][2];
extern int device_preset_pairs_size;

const device_preset_t *device_search_preset(const device_preset_t *device_presets[], int size, char *name);
void device_set_preset(hid_device *device, const device_preset_t *preset);

#endif