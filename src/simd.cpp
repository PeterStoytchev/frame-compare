#include <immintrin.h>

#include "types.h"

#define TOMBSTONE 255

__m256i SetRGB(u8* frame)
{
    return _mm256_setr_epi8(frame[0], frame[1], frame[2], frame[3], frame[4], frame[5], frame[6], frame[7], frame[8], frame[9], frame[10], frame[11], frame[12], frame[13], frame[14], frame[15], frame[16], frame[17], frame[18], frame[19], frame[20], frame[21], frame[22], frame[23], frame[24], frame[25], frame[26], frame[27], frame[28], frame[29], frame[30], frame[31]);
}