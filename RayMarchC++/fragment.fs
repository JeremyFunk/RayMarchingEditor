#version 330

#define MAX_STEPS 100
#define MAX_DIST 100.
#define MIN_DIST 0.0001
#define PRIM_COUNT 3
#define MOD_COUNT 3
#define GROUP_COUNT 2
float s, c;
#define rotate(p, a) mat2(c=cos(a), s=-sin(a), -s, c) * p

struct Modifier{
    float modifierAttribute0;
    float modifierAttribute1;
    float modifierAttribute2;
    float modifierAttribute3;
    float modifierAttribute4;
    int modifier;
};

struct Primitive{
    float attribute0;
    float attribute1;
    float attribute2;
    float attribute3;
    float attribute4;
    float attribute5;
    Modifier modifiers[MOD_COUNT];
    mat3 transformation;
    vec3 position;
    int prim_type; // 1 = Sphere, 2 = Torus, 3 = Cube, 4 = Mandelbulb
};

struct GroupModifier{
    int prim0;
    int prim1;
    int prim2;
    int prim3;
    float primAttribute;
    int modifier;
};

struct DistanceResult{
    float distance;
    float rawDistance;
};

uniform vec2 u_resolution;
uniform Primitive u_primitives[PRIM_COUNT];
uniform vec3 camera_pos;
uniform mat4 camera_rot;
uniform int shading_mode;
uniform int u_prim_count;
uniform GroupModifier u_group_modifier[GROUP_COUNT];
out vec4 fragColor;


in vec2 f_texCoords;

struct ray {
    vec3 ori;
    vec3 dir;
};
struct RayMarchResult {
    float dist;
    float rawDist;
    bool hit;
    bool hitRaw;
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


vec3 mod_distort(Modifier m, vec3 pos){
    float p = sin(m.modifierAttribute4 * pos.x + m.modifierAttribute0) * sin(m.modifierAttribute4 * pos.y + m.modifierAttribute1) * sin(m.modifierAttribute4 * pos.z + m.modifierAttribute2) * m.modifierAttribute3;
    return vec3(pos.x + p, pos.y + p, pos.z + p);
}

vec3 mod_twist(Modifier m, vec3 pos){
    float c = cos(m.modifierAttribute0 * pos[1]);
    float s = sin(m.modifierAttribute0 * pos[1]);
    return vec3(c * pos.x - s * pos.z, s * pos.x + c * pos.z, pos.y);
}

vec3 mod_bend(Modifier m, vec3 pos){
    float c = cos(m.modifierAttribute0 * pos[0]);
    float s = sin(m.modifierAttribute0 * pos[0]);
    return vec3(c * pos.x - s * pos.y, s * pos.x + c * pos.y, pos.z);
}

vec3 mod_repetition(Modifier m, vec3 pos){
    return vec3(
        mod((pos.x + .5 * m.modifierAttribute0), m.modifierAttribute0) - 0.5 * m.modifierAttribute0,
        mod((pos.y + .5 * m.modifierAttribute0), m.modifierAttribute0) - 0.5 * m.modifierAttribute0,
        mod((pos.z + .5 * m.modifierAttribute0), m.modifierAttribute0) - 0.5 * m.modifierAttribute0
    );
}

vec3 mod_repetition_limited(Modifier m, vec3 pos){
    return vec3(
        (pos.x - m.modifierAttribute0 * clamp(round(pos.x / m.modifierAttribute0), -m.modifierAttribute1, m.modifierAttribute1)),
        (pos.y - m.modifierAttribute0 * clamp(round(pos.y / m.modifierAttribute0), -m.modifierAttribute2, m.modifierAttribute2)),
        (pos.z - m.modifierAttribute0 * clamp(round(pos.z / m.modifierAttribute0), -m.modifierAttribute3, m.modifierAttribute3))
    );
}

vec3 mod_round(Modifier m, vec3 pos){
    return vec3(
        (pos.x - m.modifierAttribute0 * clamp(round(pos.x / m.modifierAttribute0), -m.modifierAttribute1, m.modifierAttribute1)),
        (pos.y - m.modifierAttribute0 * clamp(round(pos.y / m.modifierAttribute0), -m.modifierAttribute2, m.modifierAttribute2)),
        (pos.z - m.modifierAttribute0 * clamp(round(pos.z / m.modifierAttribute0), -m.modifierAttribute3, m.modifierAttribute3))
    );
}

float modify_distance(float d, int i){
    for(int j = 0; j < MOD_COUNT; j++){
        if(u_primitives[i].modifiers[j].modifier == 6){
            d = abs(d) - 0.01 - u_primitives[i].modifiers[j].modifierAttribute0;
        }
    }

    return d;
}

float op (float d1, float d2, GroupModifier m){
    if(m.modifier == 1){ // Union
        return min(d1,d2);
    }else if(m.modifier == 2){ // Subtraction
        return max(-d1,d2);
    }else if(m.modifier == 3){ // Intersection
        return max(d1,d2);
    }else if(m.modifier == 4){ // Smooth Union
        float h = clamp( 0.5 + 0.5*(d2-d1)/m.primAttribute, 0.0, 1.0 );
        return mix( d2, d1, h ) - m.primAttribute*h*(1.0-h);
    }else if(m.modifier == 5){ // Smooth Subtraction
        float h = clamp( 0.5 - 0.5*(d2+d1)/m.primAttribute, 0.0, 1.0 );
        return mix( d2, -d1, h ) + m.primAttribute*h*(1.0-h);
    }else if(m.modifier == 6){ // Smooth Intersection
        float h = clamp( 0.5 - 0.5*(d2-d1)/m.primAttribute, 0.0, 1.0 );
        return mix( d2, d1, h ) + m.primAttribute*h*(1.0-h);
    }
    return min(d1, d2);
}

DistanceResult getDist(vec3 pt, int full){
    float de[PRIM_COUNT];

    int i;
    for(i = 0; i < u_prim_count; i++){
        de[i] = MAX_DIST;
        vec3 transformed = u_primitives[i].transformation * (pt - u_primitives[i].position);

        for(int j = 0; j < MOD_COUNT; j++){
            if(u_primitives[i].modifiers[j].modifier == 1){
                transformed = mod_distort(u_primitives[i].modifiers[j], transformed);
            }else if(u_primitives[i].modifiers[j].modifier == 2){
                transformed = mod_twist(u_primitives[i].modifiers[j], transformed);
            }else if(u_primitives[i].modifiers[j].modifier == 3){
                transformed = mod_bend(u_primitives[i].modifiers[j], transformed);
            }else if(u_primitives[i].modifiers[j].modifier == 4){
                transformed = mod_repetition(u_primitives[i].modifiers[j], transformed);
            }else if(u_primitives[i].modifiers[j].modifier == 5){
                transformed = mod_repetition_limited(u_primitives[i].modifiers[j], transformed);
            }
        }

        if(u_primitives[i].prim_type == 1){
            de[i] = modify_distance(sphere(transformed, u_primitives[i].attribute0), i);
        }else if(u_primitives[i].prim_type == 2){
            de[i] = modify_distance(torus(transformed, u_primitives[i].attribute0, u_primitives[i].attribute1), i);
        }else if(u_primitives[i].prim_type == 3){
            de[i] = modify_distance(cube(transformed, vec3(u_primitives[i].attribute0, u_primitives[i].attribute1, u_primitives[i].attribute2)), i);
        }else if(u_primitives[i].prim_type == 4){
            de[i] = modify_distance(mandelbulb(transformed, u_primitives[i].attribute0), i);
        }
    }
    float de_all = MAX_DIST;
    float rawDistance = MAX_DIST;
    for(i = 0; i < u_prim_count; i++){
        int found = 0;

        if(full == 1){
            for(int j = 0; j < GROUP_COUNT; j++){
                if(u_group_modifier[j].modifier != 0 && (u_group_modifier[j].prim0 == i || u_group_modifier[j].prim1 == i || u_group_modifier[j].prim2 == i || u_group_modifier[j].prim3 == i)){
                    float de_cur = op(de[u_group_modifier[j].prim0], de[u_group_modifier[j].prim1], u_group_modifier[j]);
                
                    if(u_group_modifier[j].prim2 != -1){
                        de_cur = op(de_cur, de[u_group_modifier[j].prim2], u_group_modifier[j]);
                    }
                    if(u_group_modifier[j].prim3 != -1){
                        de_cur = op(de_cur, de[u_group_modifier[j].prim3], u_group_modifier[j]);
                    }

                    de_all = min(de_all, de_cur);

                    found = 1;
                }
            }
        }
        if(found == 0){
            de_all = min(de_all, de[i]);
        }
        rawDistance = min(rawDistance, de[i]);
    }
    
    return DistanceResult(de_all, rawDistance);
}

vec3 getNormal(vec3 pt) {
    // distance of pt to closest object
    float dist = getDist(pt, 1).distance;
    vec2 e = vec2(.01, 0);
    
    vec3 offset = dist - vec3(
        getDist(pt - e.xyy, 1).distance,
        getDist(pt - e.yxy, 1).distance,
        getDist(pt - e.yyx, 1).distance);
        
    return normalize(offset);
}


RayMarchResult rayMarch(ray r){
    float marchDist = 0.;
    int i;
    bool hit = false;
    for(i = 0; i < MAX_STEPS; i++){
        vec3 marchForward = r.ori + r.dir * marchDist;
        DistanceResult distToScene = getDist(marchForward, 1);
        marchDist += distToScene.distance;
        if(marchDist > MAX_DIST){
            break;
        }
        if(distToScene.distance < MIN_DIST) {
            hit = true;
            break;
        }
    }
    return RayMarchResult(marchDist, MAX_DIST, hit, false);
}
RayMarchResult rayMarchFull(ray r){
    float rawDist = 0.;
    float marchDist = 0.;
    int i;
    bool hit = false;
    bool hitRaw = false;
    for(i = 0; i < MAX_STEPS; i++){
        vec3 marchForward = r.ori + r.dir * marchDist;
        DistanceResult distToScene = getDist(marchForward, 1);
        marchDist += distToScene.distance;
        if(marchDist > MAX_DIST){
            break;
        }
        if(distToScene.distance < MIN_DIST) {
            hit = true;
            break;
        }
    }
    for(i = 0; i < MAX_STEPS / 8; i++){
        vec3 marchForward = r.ori + r.dir * rawDist;
        DistanceResult distToScene = getDist(marchForward, 0);
        rawDist += distToScene.rawDistance;
        if(rawDist > MAX_DIST){
            break;
        }
        if(distToScene.distance < MIN_DIST * 100) {
            hitRaw = true;
            break;
        }
    }
    return RayMarchResult(marchDist, rawDist, hit, hitRaw);
}

// input: point to be shaded
// output: light value
float getLight(vec3 pt) {
    // define light source location
    //vec3 lightPos = vec3(-3, 8, -5);
    vec3 lightPos = vec3(-3, 5, 10);
    
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

    vec3 rayDir = (vec4(normalize(vec3(-uv.x, uv.y, -1.)), 0.0) * camera_rot).xyz;
    vec2 res = u_resolution.xy;

    //vec3 rayDir = normalize(vec3(f_texCoords.x, f_texCoords.y, 1.));
    
    //vec2 res = u_resolution.xy;
    //vec3 camPos = vec3(-2., -2., -4.);
    
    // simple camera

    ray r = ray(camera_pos,
                rayDir);
    
    RayMarchResult dist = rayMarchFull(r);
    vec3 intersectPt = r.ori + r.dir * dist.dist;
    vec3 color = vec3(1.0, 1.0, 1.0);
    if(shading_mode == 0){
        float dif = getLight(intersectPt);
    
        color *= vec3(dif) * .5;
        if(dist.hit){
            color = max(color, vec3(0.12, 0.12, 0.12));
        }else{
            color = vec3(0.1, 0.1, 0.1);
        }
    }else if(shading_mode == 1){
        color = vec3(0.1);
        if(dist.hit){
            color = getNormalColor(intersectPt);
            color = vec3((color.x + color.y + color.z) / 4);
            color = vec3(1);
        }
    }else if(shading_mode == 2){
        color = vec3(0.1);
        if(dist.hit){
            color = vec3(1.0);
        }
    }

    if(dist.hitRaw || dist.hit){
        //color += vec3(0.2, 0.0, 0.0);
    }
    
    // Output to screen
    fragColor = vec4(color, 1.0);
}