#include <texture.h>

Vec3 CHECKER_BOARD_EVEN = {.2, .3, .1};
Vec3 CHECKER_BOARD_ODD = {.9, .9, .9};

Vec3 texture_checkboard(float u, float v, float scale, Vec3 *even_color) {
    float inv_scale = 1.f / scale;
    int x = floor(inv_scale * u);
    int y = floor(inv_scale * v);
    int is_even = (x + y) % 2 == 0;

    return is_even ? *even_color : CHECKER_BOARD_ODD;
}
