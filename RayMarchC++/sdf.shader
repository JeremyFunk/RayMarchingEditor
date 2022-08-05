

// 0 = radius
float sphere(vec3 pt, float radius) {
    return length(pt) - radius;
}

// 0 = radius, 1 = ring_radius
float torus(vec3 pt, float radius, float ring_radius) {
    float l = sqrt(pt.x * pt.x + pt.z * pt.z) - radius;
    float distance = sqrt(l * l + pt.y * pt.y) - ring_radius;
    return distance;
}

// 0 = bounds_x, 1 = bounds_y, 2 = bounds_z
float cube(vec3 pt, vec3 bounds) {
    vec3 dist_vec = abs(pt) - bounds;
    float distance = min(max(dist_vec.x, max(dist_vec.y, dist_vec.z)), 0.0) + length(max(dist_vec, 0.0));
    return distance;
}

// 0 = power
float mandelbulb(vec3 pt, float power, float nIts) {
    vec3 z = vec3(pt);
    float dr = 1.0;
    float r = 0.0;
    int i = 0;
    for (i = 0; i < nIts; i++) {
        r = length(z);
        if (r > 2.0) {
            break;
        }

        float theta = acos(z.z / r);
        float phi = atan(z.y, z.x);
        dr = pow(r, power - 1.0) * power * dr + 1.0;
        float zr = pow(r, power);
        theta *= power;
        phi *= power;

        z = vec3(sin(theta) * cos(phi) * zr, sin(phi) * sin(theta) * zr, cos(theta) * zr);
        z += pt;
    }
    return .5 * log(r) * r / dr;
}

//float julia(vec3 p, vec4 c, out vec4 oTrap){
float julia(vec3 p, vec4 c, float nIts) {
    vec4 z = vec4(p, 0.0);
    float md2 = 1.0;
    float mz2 = dot(z, z);

    vec4 trap = vec4(abs(z.xyz), dot(z, z));

    float n = 1.0;
    for (int i = 0; i < nIts; i++)
    {
        // dz -> 2·z·dz, meaning |dz| -> 2·|z|·|dz|
        // Now we take the 2.0 out of the loop and do it at the end with an exp2
        md2 *= 4.0 * mz2;
        // z  -> z^2 + c
        z = qsqr(z) + c;

        trap = min(trap, vec4(abs(z.xyz), dot(z, z)));

        mz2 = qlength2(z);
        if (mz2 > 4.0) break;
        n += 1.0;
    }

    //oTrap = trap;

    return 0.25 * sqrt(mz2 / md2) * log(mz2);  // d = 0.5·|z|·log|z|/|z'|
}
