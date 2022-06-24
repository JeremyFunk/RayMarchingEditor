#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <string>
std::string gen_random(const int len);
bool LoadTextureFromFile(const char* filename, int* out_texture);