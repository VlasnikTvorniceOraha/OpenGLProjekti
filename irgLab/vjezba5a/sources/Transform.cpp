#include "Transform.hpp"


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



Transform::Transform() {
	modelMatrica = glm::mat4(1.0f);
	pogledMatrica = glm::mat4(1.0f);
	perspektivaMatrica = glm::mat4(1.0f);
}

glm::mat4 Transform::Rotacija(char os, float kut) {

	glm::mat4 RotacijaMatrica = glm::mat4(1.0f);

	glm::vec3 vektorOsi;

	if (os == 'x') {
		vektorOsi = glm::vec3(1.0, 0.0, 0.0);
	}
	else if (os == 'y') {
		vektorOsi = glm::vec3(0.0, 1.0, 0.0);
	}
	else if (os == 'z') {
		vektorOsi = glm::vec3(0.0, 0.0, 1.0);
	}
	else {
		vektorOsi = glm::vec3(0.0, 0.0, 0.0);
	}

	RotacijaMatrica = glm::rotate(RotacijaMatrica, glm::radians(kut), vektorOsi);

	return RotacijaMatrica;

}

glm::mat4 Transform::Translacija(glm::vec3 translacijaVektor) {

	glm::mat4 TranslacijaMatrica = glm::mat4(1.0f);

	TranslacijaMatrica = glm::translate(TranslacijaMatrica, translacijaVektor);

	return TranslacijaMatrica;

}


glm::mat4 Transform::Skaliranje(glm::vec3 skaliranjeVektor) {

	glm::mat4 SkaliranjeMatrica = glm::mat4(1.0f);

	SkaliranjeMatrica = glm::scale(SkaliranjeMatrica, skaliranjeVektor);

	return SkaliranjeMatrica;
}


