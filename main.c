#include <math.h>
#include <stdbool.h>
#include <unistd.h>
#include <ncurses.h>

static const int SCRSIZE = 50;

typedef struct { float x, y, z; } vec3;
typedef struct { float x, y; } vec2;

#define ROTX(a) { \
    { 1.f, 0.f, 0.f }, \
    { 0.f, cosf(a), -sinf(a) }, \
    { 0.f, sinf(a), cosf(a) } \
}

#define ROTY(a) { \
    { cosf(a), 0.f, sinf(a) }, \
    { 0.f, 1.f, 0.f }, \
    { -sinf(a), 0.f, cosf(a) } \
}

#define ROTZ(a) { \
    { cosf(a), -sinf(a), 0.f }, \
    { sinf(a), cosf(a), 0.f }, \
    { 0.f, 0.f, 1.f } \
}

vec3 vec3_add(vec3 a, vec3 b)
{
    return (vec3){ a.x + b.x, a.y + b.y, a.z + b.z };
}

vec3 vec3_sub(vec3 a, vec3 b)
{
    return (vec3){ a.x - b.x, a.y - b.x, a.z - b.z };
}

vec3 matmul(float mat[3][3], vec3 p)
{
    return (vec3){
        .x = mat[0][0] * p.x + mat[0][1] * p.y + mat[0][2] * p.z,
        .y = mat[1][0] * p.x + mat[1][1] * p.y + mat[1][2] * p.z,
        .z = mat[2][0] * p.x + mat[2][1] * p.y + mat[2][2] * p.z
    };
}

vec2 project(vec3 p)
{
    p.x /= p.z;
    p.y /= p.z;
    return (vec2){
        (p.x + .5f) * SCRSIZE,
        (p.y + .5f) * SCRSIZE / 2
    };
}

int main()
{
    initscr();
    noecho();
    curs_set(FALSE);

    vec3 angle = { 0.f, 0.f, 0.f };

    const vec3 center = { 0.f, 0.f, -10.f };
    const float tstep = .01f,
                pstep = .01f;
    const float in_r = 2.f,  // Inner radius
                out_r = .7f; // Ring radius

    while (true)
    {
        erase();

        float rotx[3][3] = ROTX(angle.x);
        float roty[3][3] = ROTY(angle.y);
        float rotz[3][3] = ROTZ(angle.z);

        // Assume donut standing upright with camera looking through hole
        for (float theta = 0.f; theta < 2.f * M_PI; theta += tstep)
        {
            // Z rot
            float trot[3][3] = ROTZ(theta);

            for (float phi = 0.f; phi < 2.f * M_PI; phi += pstep)
            {
                // X rot
                float prot[3][3] = ROTX(phi);

                // Rotate along inner ring
                vec3 ring_center = center;
                ring_center.y += in_r + out_r;
                vec3 pos = vec3_add(ring_center, matmul(prot, (vec3){ 0.f, out_r, 0.f }));

                // Rotate along outer ring
                vec3 tcenter = center;
                tcenter.z = pos.z;
                vec3 rel = vec3_sub(pos, tcenter);
                pos = vec3_add(tcenter, matmul(trot, rel));

                // Rotate full donut
                rel = vec3_sub(pos, center);
                pos = vec3_add(center, matmul(rotz, matmul(roty, matmul(rotx, rel))));

                vec2 proj = project(pos);
                mvaddch(proj.y, proj.x, '#');
            }
        }

        angle.x += .1f;
        angle.y += .08f;

        refresh();
        usleep(1e4);
    }

    endwin();
    return 0;
}

