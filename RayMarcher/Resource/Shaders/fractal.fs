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
struct RayMarchStepResult{
    float dist;
    vec3 fractal_data;
};
struct RayMarchResult {
    float dist;
    bool hit;
    int steps;
    vec3 fractal_data;
    vec3 position;
};

// 0 = radius
RayMarchStepResult sphere(vec3 pt, float radius){
    float distance = length(pt) - radius;
    return RayMarchStepResult(distance, vec3(0.0));
}

// 0 = radius, 1 = ring_radius
RayMarchStepResult torus(vec3 pt, float radius, float ring_radius){
    float l = sqrt(pt.x * pt.x + pt.z * pt.z) - radius;
    float distance = sqrt(l * l + pt.y * pt.y) - ring_radius;
    return RayMarchStepResult(distance, vec3(0.0));
}

// 0 = bounds_x, 1 = bounds_y, 2 = bounds_z
RayMarchStepResult cube(vec3 pt, vec3 bounds){
    vec3 dist_vec = abs(pt) - bounds;
    float distance = min(max(dist_vec.x, max(dist_vec.y, dist_vec.z)), 0.0) + length(max(dist_vec, 0.0));
    return RayMarchStepResult(distance, vec3(0.0));
}

// 0 = power
RayMarchStepResult mandelbulb(vec3 pt, float power){
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
    float dist = .5 * log(r) * r / dr;
    vec3 data = vec3(float(i));
    return RayMarchStepResult(dist, data);
}
RayMarchStepResult getDist(vec3 pt){
    RayMarchStepResult de = RayMarchStepResult(MAX_DIST, vec3(0.0));

    int i;
    for(i = 0; i < PRIM_COUNT; i++){
        vec3 transformed = u_primitives[i].transformation * (pt - u_primitives[i].position);

        if(u_primitives[i].prim_type == 1){
            RayMarchStepResult current = sphere(transformed, u_primitives[i].attribute0);
            return current;
            if(current.dist < de.dist){
                de = current;
            }
        }else if(u_primitives[i].prim_type == 2){
            RayMarchStepResult current = torus(transformed, u_primitives[i].attribute0, u_primitives[i].attribute1);
            if(current.dist < de.dist){
                de = current;
            }
        }else if(u_primitives[i].prim_type == 3){
            RayMarchStepResult current = cube(transformed, vec3(u_primitives[i].attribute0, u_primitives[i].attribute1, u_primitives[i].attribute2));
            if(current.dist < de.dist){
                de = current;
            }
        }else if(u_primitives[i].prim_type == 4){
            RayMarchStepResult current = mandelbulb(transformed, u_primitives[i].attribute0);
            if(current.dist < de.dist){
                de = current;
            }
        }
    }

    //de = sphere(pt, 1.5);
    
    return de;
}

vec3 getNormal(vec3 pt) {
    // distance of pt to closest object
    RayMarchStepResult dist = getDist(pt);
    vec2 e = vec2(.01, 0);
    
    vec3 offset = dist.dist - vec3(
        getDist(pt - e.xyy).dist,
        getDist(pt - e.yxy).dist,
        getDist(pt - e.yyx).dist);
        
    return normalize(offset);
}


RayMarchResult rayMarch(ray r){
    RayMarchStepResult marchDist = RayMarchStepResult(0.0, vec3(0.0));
    int i;
    bool hit = false;
    vec3 marchForward;
    vec3 fractal_data;
    for(i = 0; i < MAX_STEPS; i++){
        vec3 marchForward = r.ori + r.dir * marchDist.dist;
        RayMarchStepResult distToScene = getDist(marchForward);
        marchDist.dist += distToScene.dist;
        marchDist.fractal_data = distToScene.fractal_data;
        if(marchDist.dist > MAX_DIST){
            break;
        }
        if(distToScene.dist < MIN_DIST) {
            hit = true;
            break;
        }
    }
    return RayMarchResult(marchDist.dist, hit, i, marchDist.fractal_data, marchForward);
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
    //ray shadowRay = ray(pt + ptNormal*MIN_DIST*2., lightVec);
    //RayMarchResult shadowRayDist = rayMarch(shadowRay);
    
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

vec3 fractalShader(RayMarchResult i){
    vec3 col1 = vec3(0.1, 0.1, 0.4);
    vec3 col2 = vec3(0.2, 0.9, 0.8);
    float darkness = 30.0;
    vec3 light_dir = vec3(-45.0);

    vec3 ptNormal = getNormal(i.position);

    float a = clamp(dot(ptNormal * .5 + .5, light_dir), 0.0, 1.0);
    float b = clamp(i.fractal_data[0] / 16.0, 0.0, 1.0);
    vec3 mix = clamp(a * col1 + b * col2, 0.0, 1.0);

    float rim = float(i.steps) / darkness;
    return mix * rim;
}

void main()
{
    float fov = 60.0;
    float ar = u_resolution.x / u_resolution.y;
    float scale = fov * (3.14159265358 / 360.0);
    float x = f_texCoords.x * u_resolution.x;
    float y = f_texCoords.y * u_resolution.y;
    float rx = (2.0 * (x + 0.5) / u_resolution.x - 1.0) * ar * scale;
    float ry = (1.0 - 2.0 * (y + 0.5) / u_resolution.y ) * scale;

    vec3 rayDir = normalize(vec3(-rx, ry, -1.)) * camera_rot;


    ray r = ray(camera_pos,
                rayDir);
    
    RayMarchResult result = rayMarch(r);
    
    //vec3 color = vec3(1.0, 1.0, 1.0);
    //float dif = getLight(intersectPt);
    //if(dif == 0.) dif += 0.5;

    //color *= vec3(dif) * .5;
    //if(dist.hit){
    //    color = max(color, vec3(0.1, 0.1, 0.1));
    //}
    vec3 color = vec3(0.1);
    if(result.hit){
        //color = getNormalColor(result.position);
        //color = vec3((color.x + color.y + color.z) / 4);
        float dif = getLight(result.position);
        color = vec3(dif);
        color = vec3(1.0);
        color = fractalShader(result);
    }
    

    // Output to screen
    fragColor = vec4(color, 1.0);
}