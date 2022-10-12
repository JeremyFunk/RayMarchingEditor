
vec3 mod_distort(Modifier m, vec3 pos) {
    float p = sin(m.modifierAttribute4 * pos.x + m.modifierAttribute0) * sin(m.modifierAttribute4 * pos.y + m.modifierAttribute1) * sin(m.modifierAttribute4 * pos.z + m.modifierAttribute2) * m.modifierAttribute3;
    return vec3(pos.x + p, pos.y + p, pos.z + p);
}

vec3 mod_twist(Modifier m, vec3 pos) {
    float c = cos(m.modifierAttribute0 * pos[1]);
    float s = sin(m.modifierAttribute0 * pos[1]);
    return vec3(c * pos.x - s * pos.z, s * pos.x + c * pos.z, pos.y);
}

vec3 mod_bend(Modifier m, vec3 pos) {
    float c = cos(m.modifierAttribute0 * pos[0]);
    float s = sin(m.modifierAttribute0 * pos[0]);
    return vec3(c * pos.x - s * pos.y, s * pos.x + c * pos.y, pos.z);
}

vec3 mod_repetition(Modifier m, vec3 pos) {
    return vec3(
        mod((pos.x + .5 * m.modifierAttribute0), m.modifierAttribute0) - 0.5 * m.modifierAttribute0,
        mod((pos.y + .5 * m.modifierAttribute0), m.modifierAttribute0) - 0.5 * m.modifierAttribute0,
        mod((pos.z + .5 * m.modifierAttribute0), m.modifierAttribute0) - 0.5 * m.modifierAttribute0
    );
}

vec3 mod_repetition_limited(Modifier m, vec3 pos) {
    return vec3(
        (pos.x - m.modifierAttribute0 * clamp(round(pos.x / m.modifierAttribute0), -m.modifierAttribute1, m.modifierAttribute1)),
        (pos.y - m.modifierAttribute0 * clamp(round(pos.y / m.modifierAttribute0), -m.modifierAttribute2, m.modifierAttribute2)),
        (pos.z - m.modifierAttribute0 * clamp(round(pos.z / m.modifierAttribute0), -m.modifierAttribute3, m.modifierAttribute3))
    );
}

vec3 mod_round(Modifier m, vec3 pos) {
    return vec3(
        (pos.x - m.modifierAttribute0 * clamp(round(pos.x / m.modifierAttribute0), -m.modifierAttribute1, m.modifierAttribute1)),
        (pos.y - m.modifierAttribute0 * clamp(round(pos.y / m.modifierAttribute0), -m.modifierAttribute2, m.modifierAttribute2)),
        (pos.z - m.modifierAttribute0 * clamp(round(pos.z / m.modifierAttribute0), -m.modifierAttribute3, m.modifierAttribute3))
    );
}

float modify_distance(float d, int i) {
    for (int j = 0; j < u_prim_count; j++) {
        if (primitives[i].modifiers[j].modifier == 6) {
            d = abs(d) - 0.01 - primitives[i].modifiers[j].modifierAttribute0;
        }
    }

    return d;
}

OperationResult op(float d1, float d2, GroupModifier m) {
    if (m.modifier == 2) { // Subtraction
        if (-d1 > d2) {
            return OperationResult(-d1, 0.0);
        }
        return OperationResult(d2, 1.0);
    }
    else if (m.modifier == 3) { // Intersection
        if (d1 > d2) {
            return OperationResult(d1, 0.0);
        }
        return OperationResult(d2, 1.0);
    }
    else if (m.modifier == 4) { // Smooth Union
        float h = clamp(0.5 + 0.5 * (d2 - d1) / m.primAttribute, 0.0, 1.0);
        return OperationResult(mix(d2, d1, h) - m.primAttribute * h * (1.0 - h), h);
    }
    else if (m.modifier == 5) { // Smooth Subtraction
        float h = clamp(0.5 - 0.5 * (d2 + d1) / m.primAttribute, 0.0, 1.0);
        return OperationResult(mix(d2, -d1, h) + m.primAttribute * h * (1.0 - h), h);
    }
    else if (m.modifier == 6) { // Smooth Intersection
        float h = clamp(0.5 - 0.5 * (d2 - d1) / m.primAttribute, 0.0, 1.0);
        return OperationResult(mix(d2, d1, h) + m.primAttribute * h * (1.0 - h), h);
    }

    // Union or invalid
    if (d1 < d2) {
        return OperationResult(d1, 0.0);
    }
    return OperationResult(d2, 1.0);
}
