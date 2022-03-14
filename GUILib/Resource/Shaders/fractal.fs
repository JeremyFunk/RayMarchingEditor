#version 330

#define MAX_STEPS 100
#define MAX_DIST 100.
#define MIN_DIST 0.0001
#define PRIM_COUNT 20
float s, c;
#define rotate(p, a) mat2(c=cos(a), s=-sin(a), -s, c) * p



struct Primitive{
    float attribute0;
    float attribute1;
    float attribute2;
    float attribute3;
    float attribute4;
    float attribute5;
    float attribute6;
    float attribute7;
    float attribute8;
    float attribute9;
    mat3 transformation;
    vec3 position;
    int prim_type; // 1 = Sphere, 2 = Torus, 3 = Cube, 4 = Mandelbulb
};

uniform vec2 u_resolution;
uniform Primitive u_primitives[PRIM_COUNT];
uniform vec3 camera_pos;
uniform mat3 camera_rot;
out vec4 fragColor;


in vec2 f_texCoords;

struct ray {
    vec3 ori;
    vec3 dir;
};
struct RayMarchResult {
    float dist;
    bool hit;
};

// 0 = radius
float sphere(vec3 pt, float radius){
    return length(pt) - radius;
}

// 0 = radius, 1 = ring_radius
float torus(vec3 pt, float radius, float ring_radius){
    float l = sqrt(pt.x * pt.x + pt.z * pt.z) - radius;
    float distance = sqrt(l * l + pt.y * pt.y) - ring_radius;
    return distance;
}

// 0 = bounds_x, 1 = bounds_y, 2 = bounds_z
float cube(vec3 pt, vec3 bounds){
    vec3 dist_vec = abs(pt) - bounds;
    float distance = min(max(dist_vec.x, max(dist_vec.y, dist_vec.z)), 0.0) + length(max(dist_vec, 0.0));
    return distance;
}

// 0 = power
float mandelbulb(vec3 pt, float power){
    vec3 z = vec3(pt);
    float dr = 1.0;
    float r = 0.0;
    int i = 0;
    for(i = 0; i < 15; i++){
        r = length(z);
        if(r > 2.0){
            break;
        }
        
        float theta = acos(z.z/r);
        float phi = atan(z.y, z.x);
        dr = pow(r, power - 1.0) * power * dr + 1.0;
        float zr = pow(r,power);
        theta *= power;
        phi *= power;
        
        z = vec3(sin(theta) * cos(phi) * zr, sin(phi) * sin(theta) * zr, cos(theta) * zr);
        z += pt;
    }
    return .5 * log(r) * r / dr;
}
float getDist(vec3 pt){
    float de = MAX_DIST;

    int i;
    for(i = 0; i < PRIM_COUNT; i++){
        vec3 transformed = u_primitives[i].transformation * (pt - u_primitives[i].position);

        if(u_primitives[i].prim_type == 1){
            de = min(sphere(transformed, u_primitives[i].attribute0), de);
        }else if(u_primitives[i].prim_type == 2){
            de = min(torus(transformed, u_primitives[i].attribute0, u_primitives[i].attribute1), de);
        }else if(u_primitives[i].prim_type == 3){
            de = min(cube(transformed, vec3(u_primitives[i].attribute0, u_primitives[i].attribute1, u_primitives[i].attribute2)), de);
        }else if(u_primitives[i].prim_type == 4){
            de = min(mandelbulb(transformed, u_primitives[i].attribute0), de);
        }
    }

    //de = sphere(pt, 1.5);
    
    return de;
}

vec3 getNormal(vec3 pt) {
    // distance of pt to closest object
    float dist = getDist(pt);
    vec2 e = vec2(.01, 0);
    
    vec3 offset = dist - vec3(
        getDist(pt - e.xyy),
        getDist(pt - e.yxy),
        getDist(pt - e.yyx));
        
    return normalize(offset);
}


RayMarchResult rayMarch(ray r){
    float marchDist = 0.;
    int i;
    bool hit = false;
    for(i = 0; i < MAX_STEPS; i++){
        vec3 marchForward = r.ori + r.dir * marchDist;
        float distToScene = getDist(marchForward);
        marchDist += distToScene;
        if(marchDist > MAX_DIST){
            break;
        }
        if(distToScene < MIN_DIST) {
            hit = true;
            break;
        }
    }
    return RayMarchResult(marchDist, hit);
}

// input: point to be shaded
// output: light value
float getLight(vec3 pt) {
    // define light source location
    //vec3 lightPos = vec3(-3, 8, -5);
    vec3 lightPos = vec3(-3, 20, -20);
    
    // make moving light
    lightPos.xz += vec2(sin(2), cos(2)*2.);
    
    // normalized light vector
    vec3 lightVec = normalize(lightPos - pt);
    
    // normal direction from pt
    vec3 ptNormal = getNormal(pt);
    
    //col += ptNormal;

    // calculate diffuse
    float dif = clamp(dot(ptNormal, lightVec), 0., 1.);
    
    // make shadow ray to calculate shadows
    ray shadowRay = ray(pt + ptNormal*MIN_DIST*2., lightVec);
    RayMarchResult shadowRayDist = rayMarch(shadowRay);
    
    // length: returns distance between points
    // if shadow ray is occluded, ignore ray
    //if(shadowRayDist.dist < length(lightPos - pt)) dif *= .01;
    
    return dif*1.25;
}
vec3 getNormalColor(vec3 pt) {
    // normal direction from pt
    vec3 ptNormal = getNormal(pt);
    
    return (ptNormal * 2. + .5);
}

void main()
{
    vec2 uv = ((f_texCoords.xy - 0.5) * u_resolution.xy) / u_resolution.y;

    vec3 rayDir = normalize(vec3(-uv.x, uv.y, -1.)) * camera_rot;
    vec2 res = u_resolution.xy;

    //vec3 rayDir = normalize(vec3(f_texCoords.x, f_texCoords.y, 1.));
    
    //vec2 res = u_resolution.xy;
    //vec3 camPos = vec3(-2., -2., -4.);
    
    // simple camera

    ray r = ray(camera_pos,
                rayDir);
    
    RayMarchResult dist = rayMarch(r);
    vec3 intersectPt = r.ori + r.dir * dist.dist;
    
    //vec3 color = vec3(1.0, 1.0, 1.0);
    //float dif = getLight(intersectPt);
    //if(dif == 0.) dif += 0.5;

    //color *= vec3(dif) * .5;
    //if(dist.hit){
    //    color = max(color, vec3(0.1, 0.1, 0.1));
    //}
    vec3 color = vec3(0.1);
    if(dist.hit){
        color = getNormalColor(intersectPt);
        color = vec3((color.x + color.y + color.z) / 4);
        color = vec3(1);
    }
    

    // Output to screen
    fragColor = vec4(color, 1.0);
}