#ifndef CONTROLS_HPP
#define CONTROLS_HPP

#include <vector>
#include <glm/glm.hpp>

void  computeMatricesFromInputs(std::vector<glm::vec3> vertices);
glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();
unsigned int getcurrentnumber();
#endif
