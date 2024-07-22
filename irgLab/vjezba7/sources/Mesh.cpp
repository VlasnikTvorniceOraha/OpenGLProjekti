#include "Mesh.hpp"

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
#include "Vertex.hpp"
#include "Shader.h"
#include "Transform.hpp"


using namespace std;

//Cisti podatci
vector<Vertex> vrhovi; 
vector<unsigned int> indeksi;

Transform transformObjekt;

glm::mat4 normalMatrica;

GLuint VAO;
GLuint VBO;
GLuint EBO;
//konstruktor za mesh

Mesh::Mesh(std::vector<Vertex> vrhovi, std::vector<unsigned int> indeksi)
{
	this->vrhovi = vrhovi;
	this->indeksi = indeksi;
	transformObjekt = Transform();

	normalMatrica = glm::mat4(1.0f);

	ucitajBuffere();
}

//setupaj buffere za slanje podataka grafickoj
void Mesh::ucitajBuffere() {
	glGenVertexArrays(1, &VAO);

	glGenBuffers(1, &VBO);

	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, vrhovi.size() * sizeof(Vertex), &vrhovi[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indeksi.size() * sizeof(int), &indeksi[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void Mesh::Iscrtaj(Shader& sjencar, bool Culling, glm::mat4 Offset) {
	glUseProgram(sjencar.ID);

	glBindVertexArray(VAO);

	//slanje uniformnih matrica u shader

	sjencar.setUniformMat4("perspektiva", transformObjekt.perspektivaMatrica);

	sjencar.setUniformMat4("pogled", transformObjekt.pogledMatrica);

	sjencar.setUniformMat4("model", transformObjekt.modelMatrica * Offset);


	//nemoj fillati nego iscrtaj wireframe
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//ukljuci face culling za poligone koji se ne vide ako je bool ukljucen
	
	if (Culling == 1) {
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}
	else {
		glDisable(GL_CULL_FACE);
	}
	

	//samo draw elements jer bi svi podatci trebali biti vec poslani u ucitavanju mesha
	glDrawElements(GL_TRIANGLES, indeksi.size(), GL_UNSIGNED_INT, 0);

	//glEnable(GL_DEPTH_TEST); //ukljuci z spremnik (prikazuju se oni fragmenti koji su najblizi promatracu)
	//glDepthFunc(GL_LESS);

	glBindVertexArray(0);
}

void Mesh::IscrtajViseMesheva(Shader& sjencar, glm::mat4 matricaModela) {

	glUseProgram(sjencar.ID);

	glBindVertexArray(VAO);

	//slanje uniformnih matrica u shader

	sjencar.setUniformMat4("perspektiva", transformObjekt.perspektivaMatrica);

	sjencar.setUniformMat4("pogled", transformObjekt.pogledMatrica);

	sjencar.setUniformMat4("model", matricaModela);




	//nemoj fillati nego iscrtaj wireframe
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//ukljuci face culling za poligone koji se ne vide
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);

	//samo draw elements jer bi svi podatci trebali biti vec poslani u ucitavanju mesha
	glDrawElements(GL_TRIANGLES, indeksi.size(), GL_UNSIGNED_INT, 0);

	//glEnable(GL_DEPTH_TEST); //ukljuci z spremnik (prikazuju se oni fragmenti koji su najblizi promatracu)
	//glDepthFunc(GL_LESS);

	glBindVertexArray(0);

}

void Mesh::normalMatricaUpdate(glm::mat4 matricaTransformacije) {
	normalMatrica = matricaTransformacije * normalMatrica;
}

void Mesh::normaliziraj() {
	vector<Vertex> noviVrhovi;

	for (int i = 0; i < vrhovi.size(); i++) {
		glm::vec4 jeboVektor = normalMatrica * glm::vec4(vrhovi[i].Pozicija, 1.0);

		Vertex noviVrh;
		noviVrh.Pozicija.x = jeboVektor.x;
		noviVrh.Pozicija.y = jeboVektor.y;
		noviVrh.Pozicija.z = jeboVektor.z;

		noviVrhovi.push_back(noviVrh);
	}

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, noviVrhovi.size() * sizeof(Vertex), &noviVrhovi[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}