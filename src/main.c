#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

typedef uint8_t u8;
typedef uint32_t u32;

typedef struct v3 {
    float x,y,z;
} v3;


float signed_area(v3 origin, v3 p1, v3 p2) {
    // [a b]
    // [c d]
    // ad - bc
    return (p1.x - origin.x) * (p2.y - origin.y) - (p2.x - origin.x) * (p1.y - origin.y);
}

v3 get_signed_sub_triangle_areas(v3 p, v3 vert1, v3 vert2, v3 vert3) {
    return (v3) {
        signed_area(vert1, p, vert2),
        signed_area(vert2, p, vert3),
        signed_area(vert3, p, vert1)
    };
}

v3 ndc_to_screen(v3 p, int w, int h) {
    return (v3) {
        (p.x + 1.0f) * 0.5 * w,
        (p.y + 1.0f) * 0.5 * h,
        (p.z + 1.0f) * 0.5
    };
}

int main (int argc, char *argv[]) {

    const int screen_w = 600;
    const int screen_h = 480;


    v3 verts_ndc[3];
    verts_ndc[0] = (v3) { 0.0f, -0.5f, 1.0f};
    verts_ndc[1] = (v3) { 0.5f,  0.5f, 1.0f};
    verts_ndc[2] = (v3) {-0.5f,  0.5f, 1.0f};


    v3 verts_screen[3];
    verts_screen[0] = ndc_to_screen(verts_ndc[0], screen_w, screen_h);
    verts_screen[1] = ndc_to_screen(verts_ndc[1], screen_w, screen_h);
    verts_screen[2] = ndc_to_screen(verts_ndc[2], screen_w, screen_h);

    int bytes_per_pixel = 4;
    int buffer_size = bytes_per_pixel * screen_w * screen_h;
    void* buffer = malloc((size_t)buffer_size);


    int pitch = screen_w * bytes_per_pixel;
    u8 *row = (u8*)buffer;
    for (int y = 0; y < screen_h; y ++) {
        u32 *pixel = (u32*)row;
        for (int x = 0; x < screen_w; x++) {
            u32 color = 0;

            v3 p = (v3) {x, y, 1.0f};
            v3 sub_areas = get_signed_sub_triangle_areas(p, verts_screen[0], verts_screen[2], verts_screen[1]);

            bool is_inside_triangle = sub_areas.x >= 0.0f && sub_areas.y >= 0.0f && sub_areas.z >= 0.0f;
            if (is_inside_triangle) {
                color = 155 << 8;
            }

            *pixel++ = color;
        }
        row += pitch;
    }


    const char *out_path = "triangle.ppm";
    FILE *fp = fopen(out_path, "wb");
    fprintf(fp, "P6\n%d %d\n255\n", screen_w, screen_h);

    u32 *pixel = (u32*)buffer;
    for (int i = 0; i < screen_w * screen_h; i ++) {
        u32 color = pixel[i];
        u8 red   = (color >> 16) & 0xFF;
        u8 green = (color >>  8) & 0xFF;
        u8 blue  = (color >> 00) & 0xFF;
        fputc(red, fp);
        fputc(green, fp);
        fputc(blue, fp);
    }

    fclose(fp);

    return 0;
}