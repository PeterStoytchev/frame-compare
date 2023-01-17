#include "types.h"
#include "optick.h"

void process_frame_range(const char* path_template, const char* output_path_template, u64 start_frame, u64 end_frame);

int main()
{
    OPTICK_START_CAPTURE();
    process_frame_range("../data/source/frame%i.bmp", "../data/output/frame%i.bmp", 1, 5719);
    OPTICK_STOP_CAPTURE();
    
    OPTICK_SAVE_CAPTURE("fc-debug-prof");
    OPTICK_SHUTDOWN();
}