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

#include "Shader.h"
#include "FPSManager.h"
#include "Transform.hpp"
#include "Vertex.hpp"
#include "Mesh.hpp"
#include "Objekt.hpp"

//assimp sranja
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

Shader* loadShader(char* path, char* naziv) {
	std::string sPath(path);
	std::string pathVert;
	std::string pathFrag;

	pathVert.append(path, sPath.find_last_of("\\/") + 1);
	pathFrag.append(path, sPath.find_last_of("\\/") + 1);
	if (pathFrag[pathFrag.size() - 1] == '/') {
		pathVert.append("shaders/");
		pathFrag.append("shaders/");
	}
	else if (pathFrag[pathFrag.size() - 1] == '\\') {
		pathVert.append("shaders\\");
		pathFrag.append("shaders\\");
	}
	else {
		std::cerr << "nepoznat format pozicije shadera";
		exit(1);
	}

	pathVert.append(naziv);
	pathVert.append(".vert");
	pathFrag.append(naziv);
	pathFrag.append(".frag");

	return new Shader(pathVert.c_str(), pathFrag.c_str());
}

int width = 900, height = 600;

void framebuffer_size_callback(GLFWwindow* window, int Width, int Height)
{
	width = Width;
	height = Height;
	glViewport(0, 0, width, height);
}

int main(int argc, char* argv[]) {
	GLFWwindow* window;

	glfwInit();

	window = glfwCreateWindow(width, height, "idegas vjezba4", nullptr, nullptr);

	// Check for Valid Context
	if (window == nullptr) {
		fprintf(stderr, "Failed to Create OpenGL Context");
		exit(EXIT_FAILURE);
	}

	FPSManager FPSManagerObject(window, 60, 1.0, "Zadatak 3");

	glfwMakeContextCurrent(window);

	gladLoadGL();

	glClearColor(1, 1, 1, 1);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); //funkcija koja se poziva prilikom mijenjanja velicine prozora

	//Shader dio MORA BITI NAKON POSTAVLJANJA KONTEKSTA
	Shader* sjencar[1];

	//shader za trokute
	sjencar[0] = loadShader(argv[0], "maliuzi");

	//uniformne varijable

	GLint lokacijaTransformacijskeMatrice = glGetUniformLocation(sjencar[0]->ID, "transformacijskaMatrica");

	glm::mat4 startMatrica = glm::mat4(1.0f);

	glUseProgram(sjencar[0]->ID);

	glUniformMatrix4fv(lokacijaTransformacijskeMatrice, 1, GL_FALSE, glm::value_ptr(startMatrica));

	//ASSimp ucitavanje kocke

	Assimp::Importer importer;

	std::string path(argv[0]);
	std::string dirPath(path, 0, path.find_last_of("\\/"));
	std::string resPath(dirPath);
	resPath.append("\\resources"); //za linux pretvoriti u forwardslash
	std::string objPath(resPath);
	objPath.append("\\frog.obj"); //za linux pretvoriti u forwardslash

	const aiScene* scene = importer.ReadFile(objPath.c_str(),
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType |
		aiProcess_FixInfacingNormals);

	if (!scene) {
		std::cerr << importer.GetErrorString();
		std::cout << "jao buraz" << std::endl;
		return false;
	}

	Objekt ucitanaKocka(objPath);

	for (int i = 0; i < ucitanaKocka.meshVektor.size(); i++) {
		Mesh trenutniMesh = ucitanaKocka.meshVektor[i];
		cout << "pozicija vrhova mesha" << endl;
		for (int j = 0; j < trenutniMesh.vrhovi.size() && j < 10; j++) {
			cout << trenutniMesh.vrhovi[j].Pozicija.x << " " << trenutniMesh.vrhovi[j].Pozicija.y << " " << trenutniMesh.vrhovi[j].Pozicija.z << endl;
		}

		cout << "indeksi mesha" << endl;

		for (int j = 0; j < trenutniMesh.indeksi.size() && j < 10; j++) {
			cout << trenutniMesh.indeksi[j] + 1 << " " << trenutniMesh.indeksi[j + 1] + 1 << " " << trenutniMesh.indeksi[j + 2] + 1 << endl;
			j++;
			j++; //indeksi idu 3 po 3
		}
	}

	//nad kockom pozivamo le matricu, prvo novi transform objekt

	Transform TransObjekt;

	//Normiranje vrijeme

	vector<float> boundingBox;

	boundingBox = ucitanaKocka.BoundingBox();

	float xsrednje, ysrednje, zsrednje, M;

	xsrednje = (boundingBox[0] + boundingBox[1]) / 2;

	ysrednje = (boundingBox[2] + boundingBox[3]) / 2;

	zsrednje = (boundingBox[4] + boundingBox[5]) / 2;

	M = max(boundingBox[0] - boundingBox[1], boundingBox[2] - boundingBox[3]);

	M = max(M, boundingBox[4] - boundingBox[5]);

	glm::mat4 Translacija = TransObjekt.Translacija(glm::vec3(-xsrednje, -ysrednje, -zsrednje));

	glm::mat4 Skalirara = TransObjekt.Skaliranje(glm::vec3(2 / M, 2 / M, 2 / M));

	ucitanaKocka.updateTransMatriceMeshevima(Translacija);

	ucitanaKocka.updateTransMatriceMeshevima(Skalirara);

	ucitanaKocka.transformirajMesheve();

	fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));

	while (glfwWindowShouldClose(window) == false) {
		float deltaTime = (float)FPSManagerObject.enforceFPS(true);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		ucitanaKocka.IscrtajModel(*sjencar[0]);

		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	//UNISTI
	glfwTerminate();

	return EXIT_SUCCESS;
}