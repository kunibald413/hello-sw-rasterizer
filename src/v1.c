#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef uint8_t u8;
typedef uint32_t u32;


typedef struct v3  {
    float x,y,z;
} v3;

v3 NDC_To_Screen(v3 hdc, int screen_w, int screen_h) {
    return (v3) {
        (hdc.x + 1.0f) * 0.5f * (float)screen_w,
        (hdc.y + 1.0f) * 0.5f * (float)screen_h,
        (hdc.z + 1.0f) * 0.5f // depth
    };
}


float sign (v3 p1, v3 p2, v3 p3)
{
    // 2D cross product (determinant of two edge vectors)
    return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

bool PointInTriangle (v3 pt, v3 v1, v3 v2, v3 v3)
{
    float d1, d2, d3;
    bool has_neg, has_pos;

    d1 = sign(pt, v1, v2);
    d2 = sign(pt, v2, v3);
    d3 = sign(pt, v3, v1);

    // winding-order agnosic
    has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

    return !(has_neg && has_pos);
}




int main() {
    printf("Hello, World!\n");

    const int screen_w = 600;
    const int screen_h = 480;

    int verticies_count = 3;
    v3 verticies[verticies_count];

    verticies[0] = (v3) { 0.5f, 0.5f, 1.0f};
    verticies[1] = (v3) {-0.5f, 0.5f, 1.0f};
    verticies[2] = (v3) { 0.0f, -0.5f, 1.0f};

    v3 verticies_screen[verticies_count];
    for (int i = 0 ; i < verticies_count; i ++) {
        v3 screen_pos = NDC_To_Screen(verticies[i], screen_w, screen_h);
        printf("hdc to screen: %f %f %f\n", screen_pos.x, screen_pos.y, screen_pos.z);
        verticies_screen[i] = screen_pos;
    }

    int bytes_per_pixel = 4;
    int buffer_size = screen_w * screen_h * bytes_per_pixel;
    void* data = malloc((size_t)buffer_size);

    if (!data) {
        fprintf(stderr, "Failed to allocate memory!\n");
        return 1;
    }

    printf("alloced %d bytes, (%d KiB)\n", buffer_size, buffer_size / 1024);

    int pitch = screen_w * bytes_per_pixel;
    u8 *row = (u8*)data;
    for (int y = 0; y < screen_h; y++) {
        u32 *pixel = (u32*)row;
        for (int x = 0; x < screen_w; x++) {
            u8 red = 12;
            u8 green = (u8)y;
            u8 blue = (u8)x;

            if (!PointInTriangle((v3){(float)x,(float)y,(float)1}, verticies_screen[0], verticies_screen[1], verticies_screen[2])) {
                red = 0;
                green = 0;
                blue = 0;
            }

            *pixel++ = (red << 16) | (green << 8) | blue;
        }
        row += pitch;
    }

    // write
    const char *out_filename = "triangle.ppm";
    FILE *fp = fopen(out_filename, "wb");
    if (!fp) {
        return 1;
    }
    // PPM header (P6 = binary RGB), 
    fprintf(fp, "P6\n%d %d\n255\n", screen_w, screen_h);
    
    // ppm needs 3 bytes per pixel
    u32 *pixel = (u32*)data;
    for (int i = 0; i < screen_w * screen_h; i++) {
        u32 color = pixel[i];
        u8 r = (color >> 16) & 0xFF;
        u8 g = (color >>  8) & 0xFF;
        u8 b = (color >>  0) & 0xFF;
        fputc(r, fp);
        fputc(g, fp);
        fputc(b, fp);
    }

    fclose(fp);
    printf("Written %s (%d x %d)\n", out_filename, screen_w, screen_h);


    free(data); // os reclaims it on exit, but just for clarity

    return 0;
}
