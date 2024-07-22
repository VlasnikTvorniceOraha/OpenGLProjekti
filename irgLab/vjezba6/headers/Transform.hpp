#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Standard Headers
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <iostream>

using namespace std;


class Transform {
public:

	Transform();

	glm::mat4 modelMatrica;

	glm::mat4 pogledMatrica;

	glm::mat4 perspektivaMatrica;

	glm::mat4 Rotacija(char os, float kut);


	glm::mat4 Translacija(glm::vec3 translacijaVektor);


	glm::mat4 Skaliranje(glm::vec3 skaliranjeVektor);



};