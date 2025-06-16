#include <math.h>

#include <avrio.c>
#include <line.c>
#include <print.c>
#include <print_int.c>

struct vec3 { float x, y, z; };
struct vec2i { int x, y; };

struct vec2i projection(struct vec3 m) {

    struct vec2i n;
    
    n.x = 128.0 + m.x * 128.0 / m.z;
    n.y = 120.0 - m.y * 128.0 / m.z;

    return n;
};

int main() {

    outp(0x01, 2);

    /**  4---5
     * 0---1 |
     * | 7-|-6
     * 3---2
     */
    struct vec3 dot[8] = { 
        {-1.0,  1.0,  1.0}, // 0
        { 1.0,  1.0,  1.0}, // 1
        { 1.0, -1.0,  1.0}, // 2
        {-1.0, -1.0,  1.0}, // 3
        {-1.0,  1.0, -1.0}, // 4
        { 1.0,  1.0, -1.0}, // 5
        { 1.0, -1.0, -1.0}, // 6
        {-1.0, -1.0, -1.0}, // 7
    };

    int quad[6][4] = {
        {0, 1, 2, 3},
        {4, 5, 1, 0},
        {1, 5, 6, 2},
        {2, 3, 7, 6},
        {4, 0, 3, 7},
        {2, 3, 7, 6},
    };

    int i;
    float a = 0.1;
    for (i = 0; i < 8; i++) {

        float x = cos(a) * dot[i].x - sin(a) * dot[i].z;
        float z = cos(a) * dot[i].z + sin(a) * dot[i].x;

        dot[i].x = x;
        dot[i].z = z + 3.0;
    }

    for (i = 0; i < 6; i++) {

        struct vec2i a = projection(dot[ quad[i][0] ]);
        struct vec2i b = projection(dot[ quad[i][1] ]);
        struct vec2i c = projection(dot[ quad[i][2] ]);
        struct vec2i d = projection(dot[ quad[i][3] ]);

        line(a.x, a.y, b.x, b.y, 1);
        line(b.x, b.y, c.x, c.y, 1);
        line(c.x, c.y, d.x, d.y, 1);
        line(d.x, d.y, a.x, a.y, 1);
    }
 
    for(;;);
}
