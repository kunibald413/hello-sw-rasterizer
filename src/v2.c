#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef uint8_t u8;
typedef uint32_t u32;

typedef struct v3 {
    float x,y,z;
} v3;

v3 ndc_to_screen(v3 v, int w, int h) {
    return (v3) {
        (v.x + 1.0f) * 0.5 * w,
        (v.y + 1.0f) * 0.5 * h,
        (v.z + 1.0f) * 0.5
    };
}

// for the vectors (p - o) and (p2 - o)
// returns signed area of parallelogram(!)
// (area of triange is half of this value)
float determinant(v3 p, v3 p2, v3 o) {
    // [a b]
    // [c d]
    // ad - bc
    return (p.x - o.x) * (p2.y - o.y) - (p2.x - o.x) * (p.y - o.y);
}

bool is_in_triangle(v3 p, v3 v1, v3 v2, v3 v3) {

    float d1 = determinant(p, v2, v1);
    float d2 = determinant(p, v3, v2);
    float d3 = determinant(p, v1, v3);

    if (d1 <= 0.f && d2 <= 0.f && d3 <= 0.f) {
        return true;
    }

    return false;
}

int main (int argc, char *argv[]) {

    const int width = 680;
    const int height = 480;

    // define triangle in ndc with 3 vertices
    v3 v_ndc[3];
    v_ndc[0] = (v3) {  0.0f, -0.5f, 1.0f };
    v_ndc[1] = (v3) {  0.5f,  0.5f, 1.0f };
    v_ndc[2] = (v3) { -0.5f,  0.5f, 1.0f };
    
    // convert ndc to screen
    v3 v_s[3];
    v_s[0] = ndc_to_screen(v_ndc[0], width, height);
    v_s[1] = ndc_to_screen(v_ndc[1], width, height);
    v_s[2] = ndc_to_screen(v_ndc[2], width, height);

    // write to frame buffer
    int bytes_per_pixel = 4;
    int buffer_size = width * height * bytes_per_pixel;
    void* buffer = malloc((size_t)buffer_size);

    // output frame buffer
    int pitch = bytes_per_pixel * width;
    u8 *row = (u8*)buffer;
    for (int y = 0; y < height; y ++) {
        u32 *pixel = (u32*)row;
        for (int x = 0; x < width; x++) {
            u32 color = 0;

            v3 point = (v3){x, y, 1.0};
            if (is_in_triangle(point, v_s[0], v_s[1], v_s[2])) {
                color = 99;
            }

            *pixel++ = color;
        }
        row += pitch;
    }

    const char *out_path = "triangle.ppm";
    FILE *fp = fopen(out_path, "wb");
    // rgb header, P6 = binary
    fprintf(fp, "P6\n%d %d\n255\n", width, height);
    u32 *pixel = (u32*)buffer;
    for (int i = 0; i < width * height; i ++) {
        u32 color = pixel[i];
        u8 red = (color >> 16) & 0xFF;
        u8 green = (color >>  8) & 0xFF;
        u8 blue = (color >>  0) & 0xFF;
        fputc(red, fp);
        fputc(green, fp);
        fputc(blue, fp);
    }

    fclose(fp);

    free(buffer);

    return 0;
}