#include "Objekt.hpp"

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
#include "Mesh.hpp"

//assimp sranja
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace std;

//Objekt() {
	//cout << "napusi se kurcine oop" << endl;
//}

Objekt::Objekt(string path)
{	//objekt konstruktor
	assimpUcitaj(path);	//ucitaj pomocu assimpa
}

vector<Mesh> meshVektor;

void Objekt::IscrtajModel(Shader& sjencar, bool Culling, glm::mat4 Offset) {
	//cout << "Da kolega ja printam model" << endl;
	for (int i = 0; i < meshVektor.size(); i++) {
		meshVektor[i].Iscrtaj(sjencar, Culling, Offset);
	}
}

void Objekt::IscrtajViseModela(Shader& sjencar, int brojModela, vector<glm::mat4> matriceModela) {

	for (int i = 0; i < brojModela; i++) {

		for (int j = 0; j < meshVektor.size(); j++) {
			meshVektor[j].IscrtajViseMesheva(sjencar, matriceModela[i]);
		}

	}


}

void Objekt::updateNormalMatriceMeshevima(glm::mat4 matricaTransformacije) {
	for (int i = 0; i < meshVektor.size(); i++) {
		meshVektor[i].normalMatricaUpdate(matricaTransformacije);
	}
}

void Objekt::normalizirajMesheve() {
	for (int i = 0; i < meshVektor.size(); i++) {
		meshVektor[i].normaliziraj();
	}
}

void Objekt::updatePerspektivaMatriceMeshevima(glm::mat4 perspektivaMatrica) {
	for (int i = 0; i < meshVektor.size(); i++) {
		meshVektor[i].transformObjekt.perspektivaMatrica = perspektivaMatrica; //* meshVektor[i].transformObjekt.perspektivaMatrica;
	}

}

void Objekt::updatePogledMatriceMeshevima(glm::mat4 pogledMatrica) {
	for (int i = 0; i < meshVektor.size(); i++) {
		meshVektor[i].transformObjekt.pogledMatrica = pogledMatrica; //*meshVektor[i].transformObjekt.pogledMatrica;
	}

}

void Objekt::updateModelMatriceMeshevima(glm::mat4 modelMatrica) {
	for (int i = 0; i < meshVektor.size(); i++) {
		meshVektor[i].transformObjekt.modelMatrica = modelMatrica * meshVektor[i].transformObjekt.modelMatrica;
	}

}


vector<float> Objekt::BoundingBox() {
	vector<float> boundingBox;

	float najveciX = meshVektor[0].vrhovi[0].Pozicija.x;
	float najmanjiX = meshVektor[0].vrhovi[0].Pozicija.x;

	float najveciY = meshVektor[0].vrhovi[0].Pozicija.y;
	float najmanjiY = meshVektor[0].vrhovi[0].Pozicija.y;

	float najveciZ = meshVektor[0].vrhovi[0].Pozicija.z;
	float najmanjiZ = meshVektor[0].vrhovi[0].Pozicija.z;

	for (int i = 0; i < meshVektor.size(); i++) {
		for (int j = 0; j < meshVektor[i].vrhovi.size(); j++) {
			Vertex trenutnaTocka = meshVektor[i].vrhovi[j];

			if (trenutnaTocka.Pozicija.x > najveciX) {
				najveciX = trenutnaTocka.Pozicija.x;
			}
			else if (trenutnaTocka.Pozicija.x < najmanjiX) {
				najmanjiX = trenutnaTocka.Pozicija.x;
			}

			if (trenutnaTocka.Pozicija.y > najveciY) {
				najveciY = trenutnaTocka.Pozicija.y;
			}
			else if (trenutnaTocka.Pozicija.y < najmanjiY) {
				najmanjiY = trenutnaTocka.Pozicija.y;
			}

			if (trenutnaTocka.Pozicija.z > najveciZ) {
				najveciZ = trenutnaTocka.Pozicija.z;
			}
			else if (trenutnaTocka.Pozicija.z < najmanjiZ) {
				najmanjiZ = trenutnaTocka.Pozicija.z;
			}
		}
	}

	boundingBox.push_back(najveciX);
	boundingBox.push_back(najmanjiX);

	boundingBox.push_back(najveciY);
	boundingBox.push_back(najmanjiY);

	boundingBox.push_back(najveciZ);
	boundingBox.push_back(najmanjiZ);

	return boundingBox;
}

void Objekt::assimpUcitaj(string path) {
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(path.c_str(),
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType |
		aiProcess_FixInfacingNormals);

	if (!scene) {
		std::cerr << importer.GetErrorString();
		std::cout << "jao buraz" << std::endl;
		return;
	}

	aiNode* korijenNode = scene->mRootNode; //root node pomocu kojega cemo izvuci sve potrebne mesheve

	razrijesiNode(scene, korijenNode); //funkcija koja ce popuniti listu mesheva objekta i definirati ostale node relacije iz assimpa
}

//idemo po root nodeu i njegovoj djeci i ucitavamo sve podatke o meshevima
void Objekt::razrijesiNode(const aiScene* scene, aiNode* node) {
	for (int i = 0; i < node->mNumMeshes; i++) {
		//izvuci mesh iz nodea koji indeksira neki mesh u mMeshes od scene
		aiMesh* AssimpMesh = scene->mMeshes[node->mMeshes[i]];

		//taj aiMesh objekt pretvori u normalni Mesh objekt za spremanje
		Mesh pretvoreniMesh = pretvaracMesh(scene, AssimpMesh);
		meshVektor.push_back(pretvoreniMesh);
	}

	//trazimo djecu nodea ako postoje i njih razrjesavamo

	for (int i = 0; i < node->mNumChildren; i++) {
		razrijesiNode(scene, node->mChildren[i]);
	}
}

//funkcija koja pretvara aiMesh u Mesh objekt i vraca ga za daljnu obradu
Mesh Objekt::pretvaracMesh(const aiScene* scene, aiMesh* AssimpMesh) {
	//ovo ce direktno ici u konstruktor za Mesh objekt
	vector<Vertex> vrhovi;
	vector<unsigned int> indeksi;

	for (int i = 0; i < AssimpMesh->mNumVertices; i++) {
		Vertex trenutnaTocka;

		//strpaj poziciju u tocku
		glm::vec3 vektor;
		vektor.x = AssimpMesh->mVertices[i].x;
		vektor.y = AssimpMesh->mVertices[i].y;
		vektor.z = AssimpMesh->mVertices[i].z;

		trenutnaTocka.Pozicija = vektor;

		vrhovi.push_back(trenutnaTocka);
	}

	//procesiranje indeksa za lica mreze poligona

	for (int i = 0; i < AssimpMesh->mNumFaces; i++) {
		aiFace lice = AssimpMesh->mFaces[i]; //ovo nije pointer iz nekog razloga

		//stvara se vektor indeksa ko u primjeru 4 za iscrtavanje pomocu drawElements
		for (int j = 0; j < lice.mNumIndices; j++) {
			indeksi.push_back(lice.mIndices[j]);
		}
	}

	return Mesh(vrhovi, indeksi);
}