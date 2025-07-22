#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <argp.h>
#include <unistd.h>

#include "hid.h"

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
        for (int i = 0; i < device_preset_pairs_size; i++)
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
            const device_preset_t *device_preset = device_search_preset(keyboard_presets, keyboard_presets_size, arguments.keyboard_preset);
            if (device_preset != NULL)
                device_set_preset(keyboard, device_preset);
            else
                printf("Unknown keyboard preset: %s\n", arguments.keyboard_preset);
        }

        if (arguments.aurora_preset != NULL)
        {
            const device_preset_t *device_preset = device_search_preset(aurora_presets, aurora_presets_size, arguments.aurora_preset);
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