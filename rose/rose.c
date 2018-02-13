#include <stdio.h>
#include <math.h>

const int max_iterations = 128;
const float stop_threshold = 0.01f;
const float grad_step = 0.01f;
const float clip_far = 10.0f;

const float PI = 3.14159265359f;
const float PI2 = 6.28318530718f;
const float DEG_TO_RAD = PI / 180.0f;

typedef struct { float x, y; } vec2;
typedef struct { float x, y, z; } vec3;
typedef struct { float m[9]; } mat3;

const vec3 light_pos = { 20.0f, 50.0f, 20.0f };

float min(float a, float b) { return a < b ? a : b; }
float max(float a, float b) { return a > b ? a : b; }
float clamp(float f, float a, float b) { return max(min(f, b), a); }
vec2 make2(float x, float y) { vec2 r = { x, y }; return r; }
vec2 add2(vec2 a, vec2 b) { vec2 r = { a.x + b.x, a.y + b.y }; return r; }
vec2 sub2(vec2 a, vec2 b) { vec2 r = { a.x - b.x, a.y - b.y }; return r; }
float dot2(vec2 a, vec2 b) { return a.x * b.x + a.y * b.y; }
float length2(vec2 v) { return sqrt(dot2(v, v)); }
vec3 make3(float x, float y, float z) { vec3 r = { x, y, z }; return r; }
vec3 add3(vec3 a, vec3 b) { vec3 r = { a.x + b.x, a.y + b.y, a.z + b.z }; return r; }
vec3 sub3(vec3 a, vec3 b) { vec3 r = { a.x - b.x, a.y - b.y, a.z - b.z }; return r; }
vec3 mul3(vec3 a, vec3 b) { vec3 r = { a.x * b.x, a.y * b.y, a.z * b.z }; return r; }
vec3 scale3(vec3 v, float s) { vec3 r = { v.x * s, v.y * s, v.z *s }; return r; }
float dot3(vec3 a, vec3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
float length3(vec3 v) { return sqrt(dot3(v, v)); }
vec3 normalize3(vec3 v) { return scale3(v, 1.0f / length3(v)); }
vec3 mul(mat3 m, vec3 v) { 
    return make3(
        m.m[0] * v.x + m.m[3] * v.y + m.m[6] * v.z,
        m.m[1] * v.x + m.m[4] * v.y + m.m[7] * v.z,
        m.m[2] * v.x + m.m[5] * v.y + m.m[8] * v.z);
}

mat3 rotationXY(float x, float y) {
    vec2 c = { cos(x), cos(y) }, s = { sin(x), sin(y) };
    mat3 m = {
        c.y      , 0.0f, -s.y,
        s.y * s.x,  c.x,  c.y * s.x,
        s.y * c.x, -s.x,  c.y * c.x
    };
    return m;
}

float opI(float d1, float d2) { return max(d1, d2); }
float opU(float d1, float d2) { return min(d1, d2); }
float opS(float d1, float d2) { return max(-d1, d2); }

float sdPetal(vec3 p, float s) {
    p = add3(mul3(p, make3(0.8f, 1.5f, 0.8f)), make3(0.1f, 0.0f, 0.0f));
    vec2 q = make2(length2(make2(p.x, p.z)), p.y);
    
    float lower = length2(q) - 1.0f;
    lower = opS(length2(q) - 0.97f, lower);
    lower = opI(lower, q.y);
    
    float upper = length2(sub2(q, make2(s, 0.0f))) + 1.0f - s;
    upper = opS(upper, length2(sub2(q, make2(s, 0.0f))) + 0.97f - s);
    upper = opI(upper, -q.y);
    upper = opI(upper, q.x - 2.0f);
    
    float region = length3(sub3(p, make3(1.0f, 0.0f, 0.0f))) - 1.0f;
    return opI(opU(upper, lower), region);
}

float map(vec3 p) {
    float d = 1000.0f, s = 2.0f;
    mat3 r = rotationXY(0.1f, PI2 * 0.618034f);
    r.m[0] *= 1.08f;  r.m[1] *= 1.08f;  r.m[2] *= 1.08f; 
    r.m[3] *= 0.995f; r.m[4] *= 0.995f; r.m[5] *= 0.995f;
    r.m[6] *= 1.08f;  r.m[7] *= 1.08f;  r.m[8] *= 1.08f;
    for (int i = 0; i < 21; i++) {
        d = opU(d, sdPetal(p, s));
        p = mul(r, p);
        p = add3(p, make3(0.0, -0.02, 0.0));
        s *= 1.05f;
    }
    return d;
}

vec3 gradient(vec3 pos) {
    const vec3 dx = { grad_step, 0.0, 0.0 };
    const vec3 dy = { 0.0, grad_step, 0.0 };
    const vec3 dz = { 0.0, 0.0, grad_step };
    return normalize3(make3(
        map(add3(pos, dx)) - map(sub3(pos, dx)),
        map(add3(pos, dy)) - map(sub3(pos, dy)),
        map(add3(pos, dz)) - map(sub3(pos, dz))));
}

float ray_marching(vec3 origin, vec3 dir, float start, float end) {
    float depth = start;
    for (int i = 0; i < max_iterations; i++) {
        float dist = map(add3(origin, scale3(dir, depth)));
        if (dist < stop_threshold)
            return depth;
        depth += dist * 0.3;
        if ( depth >= end)
            return end;
    }
    return end;
}

float shading(vec3 v, vec3 n, vec3 eye) {
    vec3 ev = normalize3(sub3(v, eye));
    vec3 vl = normalize3(sub3(light_pos, v));
    float diffuse = dot3(vl, n) * 0.5f + 0.5f;
    vec3 h = normalize3(sub3(vl, ev));
    float rim = pow(1.0f - max(-dot3(n, ev), 0.0f), 2.0f) * 0.15f;
    float ao = clamp(v.y * 0.5f + 0.5f, 0.0f, 1.0f);
    return (diffuse + rim) * ao;
}

vec3 ray_dir(float fov, vec2 pos) {
    vec3 r = { pos.x, pos.y, -tan((90.0f - fov * 0.5f) * DEG_TO_RAD) };
    return normalize3(r);
}

float f(vec2 fragCoord) {
    vec3 dir = ray_dir(45.0f, fragCoord);
    vec3 eye = { 0.0f, 0.0f, 4.5f };
    mat3 rot = rotationXY(-1.0f, 1.0f);
    
    dir = mul(rot, dir);
    eye = mul(rot, eye);
    
    float depth = ray_marching(eye, dir, 0.0f, clip_far);
    vec3 pos = add3(eye, scale3(dir, depth));
    if (depth >= clip_far)
        return 0.0f;
    else
        return shading(pos, gradient(pos), eye);
}

int main() {
    puts("\033[91m");
    for (int y = 0; y < 80; y++) {
        for (int x = 0; x < 160; x++)
            putchar("  .,-:;+=*#@"[(int)(f(make2((x / 160.0f - 0.5f) * 2.0f, (y / 80.0f - 0.5f) * -2.0f)) * 12.0f)]);
        putchar('\n');
    }
}
