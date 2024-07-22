#pragma once

#include "Vertex.hpp"
#include "Shader.h"
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

//assimp sranja
#include <assimp/Importer.hpp>      
#include <assimp/scene.h>           
#include <assimp/postprocess.h> 

using namespace std;

class Objekt {
public:

	Objekt(string path);
	vector<Mesh> meshVektor;

	void IscrtajModel(Shader& sjencar, bool Culling, glm::mat4 Offset);

	void IscrtajViseModela(Shader& sjencar, int brojModela, vector<glm::mat4> matriceModela);

	void updateNormalMatriceMeshevima(glm::mat4 matricaTransformacije);

	void normalizirajMesheve();

	vector<float> BoundingBox();

	void assimpUcitaj(string path);


	//idemo po root nodeu i njegovoj djeci i ucitavamo sve podatke o meshevima
	void razrijesiNode(const aiScene* scene, aiNode* node);

	//funkcija koja pretvara aiMesh u Mesh objekt i vraca ga za daljnu obradu
	Mesh pretvaracMesh(const aiScene* scene, aiMesh* AssimpMesh);

	void updatePerspektivaMatriceMeshevima(glm::mat4 perspektivaMatrica);

	void updatePogledMatriceMeshevima(glm::mat4 pogledMatrica);

	void updateModelMatriceMeshevima(glm::mat4 modelMatrica);




};