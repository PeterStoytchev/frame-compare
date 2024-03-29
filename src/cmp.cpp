#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "optick.h"

#include <cstdio>
#include <math.h>
#include "types.h"

#include <immintrin.h>

__m256i SetRGB(u8* frame);

bool cmp_frames_rgb(u64 image_size, u8* frame, u8* frame2, u8* diff)
{
    OPTICK_EVENT();

    u64 white_count = 0;
    u64 write_index = 0;

    #define VAL 22
    __m256i cmp = _mm256_set1_epi8(VAL);

    #define PIXELS_PER_ITER 10
    //#define PIXELS_PER_ITER 1
    for (u64 i = 0; i < image_size * 3; i += 3 * PIXELS_PER_ITER)
    {
        /*

        bool r = abs(frame[i] - frame2[i]) < VAL;
        bool g = abs(frame[i + 1] - frame2[i + 1]) < VAL;
        bool b = abs(frame[i + 2] - frame2[i + 2]) < VAL;
        
        bool eq = r && g && b;
        white_count += eq;
        diff[write_index] = eq * 225;
        write_index++;
        */

        u8* tashak = frame + i + 32;
        __m256i fr1 = _mm256_loadu_si256((const __m256i*)frame + i);
        __m256i fr2 = _mm256_loadu_si256((const __m256i*)frame2 + i);

        __m256i sub = _mm256_sub_epi8(fr1, fr2);

        __m256i absolutes = _mm256_abs_epi8(sub);

        __m256i res = _mm256_cmpgt_epi8(absolutes, cmp);
        
        u8 results[32];
        _mm256_storeu_si256((__m256i*)&results, res);

        for (u64 j = 0; j < PIXELS_PER_ITER * 3; j += 3)
        {
            bool eq = !results[j] && !results[j + 1] && !results[j + 2];
            white_count += eq;
            diff[write_index] = eq * 225;
            write_index++;
        }

    }

    return white_count > image_size * 0.93;
}

void process_frame_range(const char* path_template, const char* output_path_template, u64 start_frame, u64 end_frame)
{
    s32 x,y,n;

    char name_buffer[128];
    char name_buffer2[128];
    char name_buffer3[128];

    memset(name_buffer3, 0, sizeof(name_buffer3));
    sprintf(name_buffer3, path_template, start_frame);

    u8* data = stbi_load(name_buffer3, &x, &y, &n, 0);
    u8* diff = (u8*)malloc(x * y);

    for (u64 i = start_frame + 1; i <= end_frame; i++)
    {
        {
            OPTICK_EVENT();

            memset(name_buffer, 0, sizeof(name_buffer));
            memset(name_buffer2, 0, sizeof(name_buffer2));

            sprintf(name_buffer, path_template, i);

            u8 *data2;
            {
                OPTICK_EVENT("stbi_load");
                data2 = stbi_load(name_buffer, &x, &y, &n, 0);
            }

            sprintf(name_buffer2, output_path_template, i);
            cmp_frames_rgb(x * y, data, data2, diff);

            {
                OPTICK_EVENT("stbi_write");
                stbi_write_bmp(name_buffer2, x, y, 1, diff);
            }

            stbi_image_free(data);
            data = data2;

        }
    }

    stbi_image_free(data);
    free(diff);
}

void process_frame_range_thread(const char* path_template, const char* output_path_template, u64 start_frame, u64 end_frame)
{
    OPTICK_THREAD("Worker");
    process_frame_range(path_template, output_path_template, start_frame, end_frame);
}
