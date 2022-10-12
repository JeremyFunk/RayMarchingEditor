
float PHI = 1.61803398874989484820459;  // Φ = Golden Ratio   
const float TWOPI = 2.0f * PI;

float gold_noise(in vec2 xy, in float seed) {
    return clamp(fract(tan(distance(xy * PHI, xy) * seed) * xy.x), 0.0, 1.0);
}

vec4 qsqr(vec4 a) // square a quaterion
{
    return vec4(a.x * a.x - a.y * a.y - a.z * a.z - a.w * a.w,
        2.0 * a.x * a.y,
        2.0 * a.x * a.z,
        2.0 * a.x * a.w);
}
vec4 qmul(vec4 a, vec4 b)
{
    return vec4(
        a.x * b.x - a.y * b.y - a.z * b.z - a.w * b.w,
        a.y * b.x + a.x * b.y + a.z * b.w - a.w * b.z,
        a.z * b.x + a.x * b.z + a.w * b.y - a.y * b.w,
        a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y);

}
vec4 qconj(vec4 a)
{
    return vec4(a.x, -a.yzw);
}
float qlength2(vec4 q)
{
    return dot(q, q);
}


uint baseHash(uvec2 p) {
    p = 1103515245U * ((p >> 1U) ^ (p.yx));
    uint h32 = 1103515245U * ((p.x) ^ (p.y >> 3U));
    return h32 ^ (h32 >> 16);
}
vec2 hash2(inout float seed) {
    uint n = baseHash(floatBitsToUint(vec2(seed += .1, seed += .1)));
    uvec2 rz = uvec2(n, n * 48271U);
    return vec2(rz.xy & uvec2(0x7fffffffU)) / float(0x7fffffff);
}
vec2 randomInUnitDisk(float seed) {
    vec2 h = hash2(seed) * vec2(1., 6.28318530718);
    float phi = h.y;
    float r = sqrt(h.x);
    return r * vec2(sin(phi), cos(phi));
}



uint wang_hash(inout uint seed)
{
    seed = uint(seed ^ uint(61)) ^ uint(seed >> uint(16));
    seed *= uint(9);
    seed = seed ^ (seed >> 4);
    seed *= uint(0x27d4eb2d);
    seed = seed ^ (seed >> 15);
    return seed;
}
float RandomFloat01(inout uint state)
{
    return float(wang_hash(state)) / 4294967296.0;
}
vec3 RandomUnitVector(inout uint state)
{
    float z = RandomFloat01(state) * 2.0f - 1.0f;
    float a = RandomFloat01(state) * TWOPI;
    float r = sqrt(1.0f - z * z);
    float x = r * cos(a);
    float y = r * sin(a);
    return vec3(x, y, z);
}