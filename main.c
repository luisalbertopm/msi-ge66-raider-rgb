#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <argp.h>
#include <unistd.h>

#include <hidapi/hidapi.h>

typedef struct device_preset
{
    const char *name;
    const unsigned char **data;
    const unsigned int size;
} device_preset_t;

#include "aurora.inc"
#include "keyboard.inc"

#define KEYBOARD_VENDOR_ID 0x1038  // SteelSeries
#define KEYBOARD_PRODUCT_ID 0x1122 // KLC
#define AURORA_VENDOR_ID 0x1038    // SteelSeries
#define AURORA_PRODUCT_ID 0x1132   // ALC

static const device_preset_t device_preset_pairs[][2] = {{keyboard_preset_default, aurora_preset_default}, {keyboard_preset_dragon_shield, aurora_preset_dragon_shield}, {keyboard_preset_chakra, aurora_preset_chakra}, {keyboard_preset_disco, aurora_preset_disco}, {keyboard_preset_free_way, aurora_preset_macaw}, {keyboard_preset_drain, aurora_preset_rainbow}, {keyboard_preset_aqua, aurora_preset_blue_flash}, {keyboard_preset_rainbow_split, aurora_preset_rainbow}, {keyboard_preset_plain, aurora_preset_plain}, {keyboard_preset_disable, aurora_preset_disable}};

const device_preset_t *search_preset(const device_preset_t *device_presets[], int size, char *name)
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

const char *argp_program_version = "MSI GE66 Raider RGB 0.1";
const char *argp_program_bug_address = "<https://github.com/luisalbertopm/msi-ge66-raider-rgb/issues>";

static char doc[] = "MSI GE66 Raider RGB - A tool to control the RGB lighting of the MSI GE66 Raider laptop";

static char args_doc[] = "";

static struct argp_option options[] = {{"keyboard", 'k', "KEYBOARD_PRESET", 0, "Set RGB preset for the keyboard (aqua|chakra|default|disable|disco|dragon_shield|drain|free_way|plain|rainbow_split)", 0}, {"aurora", 'a', "AURORA_PRESET", 0, "Set RGB preset for the aurora (aurora|blue_flash|casino|chakra|default|disable|disco|dragon_shield|flux|macaw|plain|rainbow)", 1}, {"demo", 'd', 0, 0, "Show demo", 2}, {0}};

struct arguments
{
    char *keyboard_preset;
    char *aurora_preset;
    int demo;
};

static error_t parse_option(int key, char *arg, struct argp_state *state)
{
    struct arguments *arguments = state->input;

    switch (key)
    {
    case 'k':
        arguments->keyboard_preset = arg;
        break;
    case 'a':
        arguments->aurora_preset = arg;
        break;
    case 'd':
        arguments->demo = 1;
        break;
    default:
        return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = {options, parse_option, args_doc, doc};

int main(int argc, char *argv[])
{
    struct arguments arguments;
    arguments.keyboard_preset = NULL;
    arguments.aurora_preset = NULL;
    arguments.demo = 0;

    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    int error = hid_init();
    if (error)
    {
        wprintf(L"%ls\n", hid_error(NULL));
        return EXIT_FAILURE;
    }

    hid_device *keyboard = hid_open(KEYBOARD_VENDOR_ID, KEYBOARD_PRODUCT_ID, NULL);
    if (!keyboard)
    {
        wprintf(L"%ls\n", hid_error(NULL));
        return EXIT_FAILURE;
    }

    hid_device *aurora = hid_open(AURORA_VENDOR_ID, AURORA_PRODUCT_ID, NULL);
    if (!aurora)
    {
        wprintf(L"%ls\n", hid_error(NULL));
        return EXIT_FAILURE;
    }

    if (arguments.demo)
    {
        for (int i = 0; i < sizeof(device_preset_pairs) / sizeof(device_preset_pairs[0]); i++)
        {
            const device_preset_t *keyboard_preset = &device_preset_pairs[i][0];
            const device_preset_t *aurora_preset = &device_preset_pairs[i][1];

            device_set_preset(keyboard, keyboard_preset);

            usleep(10000);

            device_set_preset(aurora, aurora_preset);

            printf("--keyboard %s --aurora %s\n", keyboard_preset->name, aurora_preset->name);

            sleep(3);
        }
    }
    else
    {
        if (arguments.keyboard_preset == NULL && arguments.aurora_preset == NULL)
            printf("Please specify at least one preset to apply.\n");

        if (arguments.keyboard_preset != NULL)
        {
            const device_preset_t *device_preset = search_preset(keyboard_presets, sizeof(keyboard_presets) / sizeof(const device_preset_t *), arguments.keyboard_preset);
            if (device_preset != NULL)
                device_set_preset(keyboard, device_preset);
            else
                printf("Unknown keyboard preset: %s\n", arguments.keyboard_preset);
        }

        if (arguments.aurora_preset != NULL)
        {
            const device_preset_t *device_preset = search_preset(aurora_presets, sizeof(aurora_presets) / sizeof(const device_preset_t *), arguments.aurora_preset);
            if (device_preset != NULL)
                device_set_preset(aurora, device_preset);
            else
                printf("Unknown aurora preset: %s\n", arguments.aurora_preset);
        }
    }

    hid_close(keyboard);
    hid_close(aurora);
    hid_exit();

    return EXIT_SUCCESS;
}