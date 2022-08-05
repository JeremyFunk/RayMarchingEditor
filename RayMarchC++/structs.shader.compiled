struct Modifier {
    float modifierAttribute0;
    float modifierAttribute1;
    float modifierAttribute2;
    float modifierAttribute3;
    float modifierAttribute4;
    int modifier;
};

struct Light {
    float attribute0;
    float attribute1;
    float attribute2;
    float colorR;
    float colorG;
    float colorB;
    int type; // 1 = Point, 2 = Directional
};

struct Primitive {
    float attribute0;
    float attribute1;
    float attribute2;
    float attribute3;
    float attribute4;
    float attribute5;
    Modifier modifiers[MOD_COUNT];
    mat3 transformation;
    vec3 position;
    int prim_type; // 1 = Sphere, 2 = Torus, 3 = Cube, 4 = Mandelbulb, 5 = Julia 4D-Quat
};

struct GroupModifier {
    int prim0;
    int prim1;
    int prim2;
    int prim3;
    float primAttribute;
    int modifier;
};

struct Ray {
    vec3 ori;
    vec3 dir;
};
