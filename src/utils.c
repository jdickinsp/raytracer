#include <utils.h>

float rand_range(float low, float high) {
    float r = rand() / (RAND_MAX + 1.0);
    return low + r * (high - low);
}

float clamp_range(float x, float low, float high) {
    if (x < low) return low;
    if (x > high) return high;
    return x;
}
