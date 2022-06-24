#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h>

float to_radians(float degrees);
glm::mat3x3 rotationMatrix(glm::vec3 rotation);
glm::mat3x3 transformationMatrix(glm::vec3 rotation, glm::vec3 scale);
glm::vec3 sphericalCoords(glm::vec3 cartesian);
glm::vec3 cartesianCoords(glm::vec3 spherical);
