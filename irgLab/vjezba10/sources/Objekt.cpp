#include "Objekt.hpp"
#include "Material.hpp"

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
#include "Texture.hpp"
#include "Shader.h"
#include "Mesh.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//assimp sranja
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace std;

//Objekt() {
	//cout << "napusi se kurcine oop" << endl;
//}

Objekt::Objekt(string path, string resPath)
{	//objekt konstruktor
	assimpUcitaj(path, resPath);	//ucitaj pomocu assimpa
}

vector<Mesh> meshVektor;

vector<Material> Materiali;

void Objekt::IscrtajModel(Shader& sjencar, bool Culling, glm::mat4 Offset) {
	//cout << "Da kolega ja printam model" << endl;
	for (int i = 0; i < meshVektor.size(); i++) {
		meshVektor[i].Iscrtaj(sjencar, Culling, Offset, Materiali[1]);
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

void Objekt::assimpUcitaj(string path, string resPath) {
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

	razrijesiNode(scene, korijenNode, resPath); //funkcija koja ce popuniti listu mesheva objekta i definirati ostale node relacije iz assimpa


	




	


}

//idemo po root nodeu i njegovoj djeci i ucitavamo sve podatke o meshevima
void Objekt::razrijesiNode(const aiScene* scene, aiNode* node, string resPath) {
	for (int i = 0; i < node->mNumMeshes; i++) {
		//izvuci mesh iz nodea koji indeksira neki mesh u mMeshes od scene
		aiMesh* AssimpMesh = scene->mMeshes[node->mMeshes[i]];

		//taj aiMesh objekt pretvori u normalni Mesh objekt za spremanje
		Mesh pretvoreniMesh = pretvaracMesh(scene, AssimpMesh, resPath);
		meshVektor.push_back(pretvoreniMesh);
	}

	//trazimo djecu nodea ako postoje i njih razrjesavamo

	for (int i = 0; i < node->mNumChildren; i++) {
		razrijesiNode(scene, node->mChildren[i], resPath);
	}
}

//funkcija koja pretvara aiMesh u Mesh objekt i vraca ga za daljnu obradu
Mesh Objekt::pretvaracMesh(const aiScene* scene, aiMesh* AssimpMesh, string resPath) {
	//ovo ce direktno ici u konstruktor za Mesh objekt
	vector<Vertex> vrhovi;
	vector<unsigned int> indeksi;
	vector<Texture> teksture;

	for (int i = 0; i < AssimpMesh->mNumVertices; i++) {
		Vertex trenutnaTocka;

		//strpaj poziciju u tocku
		glm::vec3 vektor;
		vektor.x = AssimpMesh->mVertices[i].x;
		vektor.y = AssimpMesh->mVertices[i].y;
		vektor.z = AssimpMesh->mVertices[i].z;

		trenutnaTocka.Pozicija = vektor;
		glm::vec3 vektor2;
		vektor2.x = AssimpMesh->mNormals[i].x;
		vektor2.y = AssimpMesh->mNormals[i].y;
		vektor2.z = AssimpMesh->mNormals[i].z;
		//cout << "ide normala " << vektor.x << " " << vektor.y << " " << vektor.z << endl;
		trenutnaTocka.Normala = vektor2;

		//UV koordinate tocke

		if (AssimpMesh->mTextureCoords[0]) {

			glm::vec2 UVvektor;
			//koristi se samo channel 0 za teksture
			UVvektor.x = AssimpMesh->mTextureCoords[0][i].x;
			UVvektor.y = 1 - AssimpMesh->mTextureCoords[0][i].y;

			trenutnaTocka.TexKordinate = UVvektor;
		}
		else {
			trenutnaTocka.TexKordinate = glm::vec2(0.0f, 0.0f);
		}

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

	//difuzna tekstura i amterijali

	if (scene->HasMaterials()) {
		std::cout << "ucitana svojstva materijala" << std::endl;

		for (int i = 0; i < scene->mNumMaterials; i++) {
			std::cout << "materijal[" << i << "]:\n";

			aiString naziv;

			Material Materijal;

			Texture tekstura;

			std::cout << "naziv: ";
			scene->mMaterials[i]->Get(AI_MATKEY_NAME, naziv);
			std::cout << naziv.C_Str() << std::endl;
			Materijal.naziv = naziv.C_Str();

			std::cout << "pozicija teksture: ";

			unsigned int textureID;
			glGenTextures(1, &textureID);
			tekstura.id = textureID;
			std::string texPath(resPath);
			aiString texturePosition;
			int width, height, nrChannels;
			unsigned char* data;
			if (AI_SUCCESS == scene->mMaterials[i]->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), texturePosition)) {
				std::cout << texturePosition.C_Str() << std::endl;

				
				

				tekstura.type = "DIFFUSE";
				texPath.append("\\");
				texPath.append(texturePosition.C_Str());
				data = stbi_load(texPath.c_str(), &width, &height, &nrChannels, 0);

			}
			else {
				cout << "Bruh nema teksture" << endl;
				data = nullptr;
			}

			tekstura.path = texPath;

			if (data) {

				std::cout << "Texture loaded at path: " << texPath << std::endl;

				GLenum format;
				if (nrChannels == 1)
					format = GL_RED;
				else if (nrChannels == 3)
					format = GL_RGB;
				else if (nrChannels == 4)
					format = GL_RGBA;
				cout << format << " " << width << " " << height << endl;
				glBindTexture(GL_TEXTURE_2D, textureID);
				glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				stbi_image_free(data);

			}
			else {
				std::cout << "Texture failed to load at path: " << texPath << std::endl;
				stbi_image_free(data);
			}

			teksture.push_back(tekstura);


			glm::vec3 ambientColor;
			aiColor3D ambientK, diffuseK, specularK, reflectiveK, emissiveK;

			float Ns;


			scene->mMaterials[i]->Get(AI_MATKEY_SHININESS, Ns);
			Materijal.KolicinaSjaja = Ns;

			//std::cout << "ambient: ";
			scene->mMaterials[i]->Get(AI_MATKEY_COLOR_AMBIENT, ambientK);
			//std::cout << ambientK.r << " " << ambientK.g << " " << ambientK.b << std::endl;
			Materijal.ambijentalna = glm::vec3(ambientK.r, ambientK.g, ambientK.b);

			//std::cout << "diffuse: ";
			scene->mMaterials[i]->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseK);
			//std::cout << diffuseK.r << " " << diffuseK.g << " " << diffuseK.b << std::endl;
			Materijal.difuzna = glm::vec3(diffuseK.r, diffuseK.g, diffuseK.b);

			//std::cout << "specular: ";
			scene->mMaterials[i]->Get(AI_MATKEY_COLOR_SPECULAR, specularK);
			//std::cout << specularK.r << " " << specularK.g << " " << specularK.b << std::endl;
			Materijal.spekularna = glm::vec3(specularK.r, specularK.g, specularK.b);

			Materiali.push_back(Materijal);

		}



	}

	
	


	

	return Mesh(vrhovi, indeksi, teksture);
}