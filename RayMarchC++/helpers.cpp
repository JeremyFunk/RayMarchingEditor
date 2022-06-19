
#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include "helpers.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// Simple helper function to load an image into a OpenGL texture with common settings
bool LoadTextureFromFile(const char* filename, int* out_texture)
{
    // Load from file
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
    if (image_data == NULL)
        return false;

    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

    // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
    //glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(image_data);

    *out_texture = image_texture;

    return true;
}

std::string gen_random(const int len) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    std::string tmp_s;
    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i) {
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    return tmp_s;
}


float PI = 3.14159265358979323846264338327950288f;

float to_radians(float degrees) {
    return degrees * (PI / 180.0);
}

glm::mat3x3 rotationMatrix(glm::vec3 rotation) {
    auto x = to_radians(rotation.x);
    auto y = to_radians(rotation.y);
    auto z = to_radians(rotation.z);
    auto x_cos = cos(x);
    auto x_sin = sin(x);

    auto y_cos = cos(y);
    auto y_sin = sin(y);

    auto z_cos = cos(z);
    auto z_sin = sin(z);


    auto x_mat = glm::mat3x3(1.0);
    x_mat[1][1] = x_cos;
    x_mat[2][1] = -x_sin;
    x_mat[1][2] = x_sin;
    x_mat[2][2] = x_cos;


    auto y_mat = glm::mat3x3(1.0);
    y_mat[0][0] = y_cos;
    y_mat[2][0] = y_sin;
    y_mat[0][2] = -y_sin;
    y_mat[2][2] = y_cos;
    auto z_mat = glm::mat3x3(1.0);
    z_mat[0][0] = z_cos;
    z_mat[1][0] = -z_sin;
    z_mat[0][1] = z_sin;
    z_mat[1][1] = z_cos;

    return x_mat * y_mat * z_mat;
}

glm::mat3x3 transformationMatrix(glm::vec3 rotation, glm::vec3 scale) {
    glm::mat3x3 matrix = rotationMatrix(rotation);
    glm::mat3x3 scaleMat = glm::scale(glm::mat4x4(1.0), scale);

    return matrix * scaleMat;
}

glm::vec3 sphericalCoords(glm::vec3 cartesian) {
    float r = glm::sqrt(glm::pow(cartesian.x, 2) + glm::pow(cartesian.y, 2) + glm::pow(cartesian.z, 2));

    float phi = glm::atan(cartesian.z / cartesian.x, cartesian.x);
    float theta = glm::acos(cartesian.y / r);

    return glm::vec3(r, phi, theta);
}

glm::vec3 cartesianCoords(glm::vec3 spherical) {
    glm::vec3 ret = glm::vec3();

    ret.x = spherical.x * glm::cos(spherical.z) * glm::cos(spherical.y);
    ret.x = spherical.x * glm::sin(spherical.z);
    ret.x = spherical.x * glm::cos(spherical.z) * glm::sin(spherical.y);

    return ret;
}