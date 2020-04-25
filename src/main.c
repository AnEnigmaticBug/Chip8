#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "chip8.h"
#include "viewer.h"

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        fprintf(stderr, "Usage: chip8 <scale> <delay> <rom>\n");
        return -1;
    }

    srand(time(NULL));

    int video_scale = atoi(argv[1]);
    int cycle_delay = atoi(argv[2]);

    FILE *rom_fp = fopen(argv[3], "r");

    Viewer viewer;
    viewer_init(&viewer, SCR_WD * video_scale, SCR_HT * video_scale, SCR_WD, SCR_HT);

    Chip8 chip8;
    chip8_init(&chip8);
    chip8_load_rom(&chip8, rom_fp);

    struct timeval prev_time, curr_time;
    gettimeofday(&prev_time, NULL);

    while (true)
    {
        if (viewer_process_input(&viewer, chip8.keypad))
        {
            break;
        }

        gettimeofday(&curr_time, NULL);
        int dt = (curr_time.tv_usec - prev_time.tv_usec) / 1000;

        if (dt > cycle_delay)
        {
            chip8_cycle(&chip8);
            viewer_update(&viewer, chip8.video_memory);

            prev_time = curr_time;
        }
    }

    viewer_free(&viewer);
    fclose(rom_fp);
}
