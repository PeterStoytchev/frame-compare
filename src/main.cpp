#include "types.h"
#include "optick.h"

#include <thread>

void process_frame_range(const char *path_template, const char *output_path_template, u64 start_frame, u64 end_frame);
void process_frame_range_thread(const char *path_template, const char *output_path_template, u64 start_frame, u64 end_frame);

int main()
{
    OPTICK_START_CAPTURE();
    
    for (int j = 0; j < 2; j++)
    {
        OPTICK_FRAME("MainThread");

        if (j == 0)
        {
            u32 start_frame = 1;
            u32 end_frame = 4220;
            //u32 end_frame = 5719;

            //u32 thread_count = std::thread::hardware_concurrency();
            u32 thread_count = 1;
            std::thread** threads = (std::thread **)malloc(sizeof(std::thread*) * thread_count);

            u32 frame_count = end_frame - start_frame + 1;
            u32 leftovers = frame_count % thread_count;
            u32 frames_per_thread = (frame_count - leftovers) / thread_count;

            printf("frame_count: %u\n", frame_count);
            printf("thread_count: %u\n", thread_count);
            printf("frames_per_thread: %u\n", frames_per_thread);
            printf("leftovers: %u\n", leftovers);

            for (s32 i = 0; i < thread_count; i++)
            {
                u32 start_frame_local = (i * frames_per_thread) + (i == 0);
                printf("starting new thread to process from %u to %u\n", start_frame_local, (i * frames_per_thread) + frames_per_thread);

                std::thread *th = new std::thread(process_frame_range_thread, "G:/source/frame%i.bmp", "G:/target2/frame%i.bmp", start_frame_local, (i * frames_per_thread) + frames_per_thread);
                //std::thread *th = new std::thread(process_frame_range_thread, "C:/Users/Peter/Desktop/frame-compare/data/source/frame%i.bmp", "C:/Users/Peter/Desktop/frame-compare/data/target/frame%i.bmp", start_frame_local, (i * frames_per_thread) + frames_per_thread);

                threads[i] = th;
            }

            for (s32 i = 0; i < thread_count; i++)
            {
                threads[i]->join();
            }
            free(threads);

            if (leftovers != 0)
            {
                u32 last_start = thread_count * frames_per_thread;
                process_frame_range("G:/source/frame%i.bmp", "G:/target2/frame%i.bmp", last_start, last_start + leftovers);
                //process_frame_range("C:/Users/Peter/Desktop/frame-compare/data/source/frame%i.bmp", "C:/Users/Peter/Desktop/frame-compare/data/target/frame%i.bmp", last_start, last_start + leftovers);
            }
        }
    }

    OPTICK_STOP_CAPTURE();

    OPTICK_SAVE_CAPTURE("fc-debug-prof");
    OPTICK_SHUTDOWN();
}