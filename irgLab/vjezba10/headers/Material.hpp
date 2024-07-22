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

class Material {
public:
	string naziv;
	glm::vec3 ambijentalna;
	glm::vec3 difuzna;
	glm::vec3 spekularna;
	float KolicinaSjaja;

};