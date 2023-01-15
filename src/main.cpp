#include "types.h"
#include <cstdio>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

void cmp_frames(u64 image_size, u64 stride, unsigned char* frame, unsigned char* frame2, unsigned char* diff)
{
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

int main()
{
    int x,y,n;

    u64 start_frame = 1;
    u64 end_frame = 5719;

    char name_buffer[30];
    char name_buffer2[30];

    unsigned char* data = stbi_load("../data/source/frame1.bmp", &x, &y, &n, 0);
    unsigned char* diff = (unsigned char*)malloc(x * y);

    for (u64 i = start_frame + 1; i <= end_frame; i++)
    {
        memset(name_buffer, 0, sizeof(name_buffer));
        memset(name_buffer2, 0, sizeof(name_buffer2));

        sprintf(name_buffer, "../data/source/frame%i.bmp", i);
        unsigned char *data2 = stbi_load(name_buffer, &x, &y, &n, 0);

        cmp_frames(x * y, n, data, data2, diff);

        stbi_image_free(data);

        data = data2;

        sprintf(name_buffer2, "../data/output/frame%i.bmp", i);
        stbi_write_bmp(name_buffer2, x, y, 1, diff);
    }

    stbi_image_free(data);
    free(diff);
}