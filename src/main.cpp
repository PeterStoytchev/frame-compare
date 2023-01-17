#include "types.h"
#include <cstdio>
#include <chrono>
#include <ctime>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


#include "optick.h"

void cmp_frames(u64 image_size, u64 stride, u8* frame, u8* frame2, u8* diff)
{
    OPTICK_EVENT();

    u64 write_index = 0;
    for (u64 i = 0; i < image_size * stride; i += stride)
    {
        u8 counter = 0;
        for (s32 j = 0; j < stride; j++)
        {
            counter += frame[i + j] == frame2[i + j];
        }

        diff[write_index] = (counter == stride) * 255;
        write_index++;
    }
}

void process_frame_range(const char* path_template, const char* output_path_template, u64 start_frame, u64 end_frame)
{
    s32 x,y,n;

    char name_buffer[32];
    char name_buffer2[32];
    char name_buffer3[32];

    memset(name_buffer3, 0, sizeof(name_buffer3));
    sprintf(name_buffer3, path_template, start_frame);

    printf("Initial frame path: %s\n", name_buffer3);

    u8* data = stbi_load(name_buffer3, &x, &y, &n, 0);
    u8* diff = (u8*)malloc(x * y);

    for (u64 i = start_frame + 1; i <= end_frame; i++)
    {
        OPTICK_FRAME("MainThread");

        memset(name_buffer, 0, sizeof(name_buffer));
        memset(name_buffer2, 0, sizeof(name_buffer2));

        sprintf(name_buffer, path_template, i);

        u8 *data2;
        
        {
            OPTICK_EVENT("stbi_load");
            data2 = stbi_load(name_buffer, &x, &y, &n, 0);
        }

        printf("New frame path: %s\n", name_buffer);

        cmp_frames(x * y, n, data, data2, diff);

        stbi_image_free(data);

        data = data2;

        sprintf(name_buffer2, output_path_template, i);

        {
            OPTICK_EVENT("stbi_write");
            stbi_write_bmp(name_buffer2, x, y, 1, diff);
        }
    }

    stbi_image_free(data);
    free(diff);
}

int main()
{
    OPTICK_START_CAPTURE();
    process_frame_range("../data/source/frame%i.bmp", "../data/output/frame%i.bmp", 1, 5719);
    OPTICK_STOP_CAPTURE();
    
    OPTICK_SAVE_CAPTURE("fc-debug-prof");
    OPTICK_SHUTDOWN();
}