#pragma once

#include "Vertex.hpp"
#include "Shader.h"
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

class Mesh {
public:

	//Cisti podatci
	vector<Vertex> vrhovi;
	vector<unsigned int> indeksi;

	glm::mat4 normalMatrica;

	Transform transformObjekt;

	GLuint VAO;
	GLuint VBO;
	GLuint EBO;
	//konstruktor za mesh
	Mesh(std::vector<Vertex> vrhovi, std::vector<unsigned int> indeksi);
	

	//setupaj buffere za slanje podataka grafickoj
	void ucitajBuffere();

	void Iscrtaj(Shader& sjencar, bool Culling, glm::mat4 Offset);

	void IscrtajViseMesheva(Shader& sjencar, glm::mat4 matricaModela);

	void normalMatricaUpdate(glm::mat4 matricaTransformacije);


	void normaliziraj();

		

};