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
	std::string pathGeom;

	pathVert.append(path, sPath.find_last_of("\\/") + 1);
	pathFrag.append(path, sPath.find_last_of("\\/") + 1);
	pathGeom.append(path, sPath.find_last_of("\\/") + 1);
	if (pathFrag[pathFrag.size() - 1] == '/') {
		pathVert.append("shaders/");
		pathFrag.append("shaders/");
		pathGeom.append("shaders/");
	}
	else if (pathFrag[pathFrag.size() - 1] == '\\') {
		pathVert.append("shaders\\");
		pathFrag.append("shaders\\");
		pathGeom.append("shaders\\");
	}
	else {
		std::cerr << "nepoznat format pozicije shadera";
		exit(1);
	}

	pathVert.append(naziv);
	pathVert.append(".vert");
	pathFrag.append(naziv);
	pathFrag.append(".frag");
	pathGeom.append(naziv);
	pathGeom.append(".geom");

	return new Shader(pathVert.c_str(), pathFrag.c_str(), pathGeom.c_str());
}

//globalne varijable
int width = 900, height = 600;

float lastXpos = width / 2;
float lastYpos = height / 2;

float delte[2] = {0.0f, 0.0f};



void framebuffer_size_callback(GLFWwindow* window, int Width, int Height)
{
	width = Width;
	height = Height;
	glViewport(0, 0, width, height);
	lastXpos = width / 2;
	lastYpos = height / 2;

}


class Camera : public Transform {
public:
	//Pozicija kamere u svijetu
	glm::vec3 Pozicija;
	//u sto gleda kamera
	glm::vec3 CentarPozornosti;
	//Vektor gore svijeta (globalno)
	glm::vec3 GoreVektor;
	//smjer gledanja kamere
	glm::vec3 smjer;
	//desna os kamere (lokalno)
	glm::vec3 kameraDesnaOs;
	//gornja os kamere (lokalno)
	glm::vec3 kameraGoreOs;

	//brzina kamere
	float kameraBrzina;
	//pitch i yaw za rotiranje eulerovim kutevima il nesto
	float pitch = 0.0f;
	float yaw = -90.0f;

	Camera(glm::vec3 Pozicija, glm::vec3 CentarPozornosti, glm::vec3 GoreVektor, float brzina) {

		this->Pozicija = Pozicija;
		this->CentarPozornosti = CentarPozornosti;
		this->GoreVektor = GoreVektor;

		kameraBrzina = brzina;

		smjer.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		smjer.y = sin(glm::radians(pitch));
		smjer.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

		

		kameraDesnaOs = glm::normalize(glm::cross(GoreVektor, smjer));

		kameraGoreOs = glm::normalize(glm::cross(smjer, kameraDesnaOs));

	}

	void updateSmjer() {
		glm::vec3 nenormaliziraniSmjer;
		nenormaliziraniSmjer.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		nenormaliziraniSmjer.y = sin(glm::radians(pitch));
		nenormaliziraniSmjer.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

		smjer = glm::normalize(nenormaliziraniSmjer);

		

		kameraDesnaOs = glm::normalize(glm::cross(GoreVektor, smjer));

		kameraGoreOs = glm::normalize(glm::cross(smjer, kameraDesnaOs));
	}


	glm::mat4 IzradiPerspektivnuMatricu(int width, int height, float fov) {

		glm::mat4 perspektiva = glm::perspective(fov, (float)width / (float)height, 0.1f, 100.0f);

		return perspektiva;

	}

	glm::mat4 IzradiPogledMatricu() {

		glm::mat4 pogled = glm::lookAt(Pozicija, Pozicija + smjer, GoreVektor);

		return pogled;

	}

};

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	
	//pretvaranje iz 0-width, 0-height sustava u -1 1 sustav
	//xpos = (xpos - width / 2) / (width / 2);
	//ypos = (ypos - height / 2) / (height / 2);

	//cout << xpos << " " << ypos << endl;

	float xposFloat = (float)xpos;
	float yposFloat = (float)ypos;

	//izracunaj deltu
	float deltaX = xposFloat - lastXpos;
	float deltaY = yposFloat - lastYpos;

	//postavi zadnju poziciju
	lastXpos = xposFloat;
	lastYpos = yposFloat;
	//cout << xpos << " " << ypos << " | " << lastXpos << " " << lastYpos << endl;
	//u slucaju da bude prejako ili preslabo micanje kamere
	float sensitivity = 1.5f;
	deltaX *= sensitivity;
	deltaY *= sensitivity;

	

	
	delte[0] = deltaX;
	delte[1] = -deltaY;


}

class Renderer {
public:
	vector<Objekt> objekti;

	Renderer(vector<Objekt> objekti) {

		this->objekti = objekti;
		

	}

	void iscrtaj(Shader& sjencar) {

		for (int i = 0; i < objekti.size(); i++) {

			objekti[i].IscrtajModel(sjencar, 1, glm::mat4(1.0f));
		}
	}


};


void normalizirajObjekt(Objekt objekt) {

	Transform TransObjekt;

	vector<float> boundingBox;

	boundingBox = objekt.BoundingBox();

	float xsrednje, ysrednje, zsrednje, M;

	xsrednje = (boundingBox[0] + boundingBox[1]) / 2;

	ysrednje = (boundingBox[2] + boundingBox[3]) / 2;

	zsrednje = (boundingBox[4] + boundingBox[5]) / 2;

	M = max(boundingBox[0] - boundingBox[1], boundingBox[2] - boundingBox[3]);

	M = max(M, boundingBox[4] - boundingBox[5]);

	glm::mat4 Translacija = TransObjekt.Translacija(glm::vec3(-xsrednje, -ysrednje, -zsrednje));

	glm::mat4 Skalirara = TransObjekt.Skaliranje(glm::vec3(2 / M, 2 / M, 2 / M));

	objekt.updateNormalMatriceMeshevima(Translacija);

	objekt.updateNormalMatriceMeshevima(Skalirara);

	objekt.normalizirajMesheve();

}


int main(int argc, char* argv[]) {
	GLFWwindow* window;

	srand((unsigned)time(NULL));


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
	Shader* sjencar[3];

	//shader za prve 2 kocke
	sjencar[0] = loadShader(argv[0], "maliuzi2");

	//shader za kocku sa algoritmom 2
	sjencar[1] = loadShader(argv[0], "maliuzi");

	//shader za algoritam 3
	sjencar[2] = loadShader(argv[0], "maliuzi3");

	//uniformne varijableas

	//GLint lokacijaTransformacijskeMatrice = glGetUniformLocation(sjencar[0]->ID, "transformacijskaMatrica");

	//glm::mat4 startMatrica = glm::mat4(1.0f);

	//glUseProgram(sjencar[0]->ID);

	//glUseProgram(sjencar[1]->ID);

	//glUseProgram(sjencar[2]->ID);

	//ASSimp ucitavanje kocke

	Assimp::Importer importer;

	std::string path(argv[0]);
	std::string dirPath(path, 0, path.find_last_of("\\/"));
	std::string resPath(dirPath);
	resPath.append("\\resources"); //za linux pretvoriti u forwardslash
	std::string objPath1(resPath);
	objPath1.append("\\kocka.obj"); //za linux pretvoriti u forwardslash

	std::string objPath2(resPath);
	objPath2.append("\\kocka.obj"); //za linux pretvoriti u forwardslash

	const aiScene* scene = importer.ReadFile(objPath1.c_str(),
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

	Objekt ucitanaKocka(objPath1);

	Objekt ucitanaKocka2(objPath2);

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

	normalizirajObjekt(ucitanaKocka); 

	normalizirajObjekt(ucitanaKocka2);

	Camera kameraScene(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.1f);

	glm::mat4 perspektivnaMatrica = kameraScene.IzradiPerspektivnuMatricu(width, height, 45.0f);

	glm::mat4 matricaPogleda = kameraScene.IzradiPogledMatricu();

	ucitanaKocka.updatePerspektivaMatriceMeshevima(perspektivnaMatrica);

	ucitanaKocka.updatePogledMatriceMeshevima(matricaPogleda);

	ucitanaKocka2.updatePerspektivaMatriceMeshevima(perspektivnaMatrica);

	ucitanaKocka2.updatePogledMatriceMeshevima(matricaPogleda);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (glfwRawMouseMotionSupported()) {
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	}
		
	float lastXpos = width / 2;
	float lastYpos = height / 2;

	//stvaranje matrica za instanciranje vise objekata
	vector<glm::mat4> matriceZaViseIscrtavanja;

	int random1 = rand() % 360;
	int random2 = rand() % 360;
	int random3 = rand() % 360;


	glm::mat4 randMatricaTranslacije = TransObjekt.Translacija(glm::vec3(3.0f, 0.0f, 0.0f));

	/*glm::mat4 randMatricaRotacije = TransObjekt.Rotacija('x', random1);

	randMatricaRotacije *= TransObjekt.Rotacija('y', random2);

	randMatricaRotacije *= TransObjekt.Rotacija('z', random3);

	randMatricaRotacije = randMatricaTranslacije * randMatricaRotacije;
	*/
	matriceZaViseIscrtavanja.push_back(glm::mat4(1.0f));
	matriceZaViseIscrtavanja.push_back(randMatricaTranslacije);






	fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));

	while (glfwWindowShouldClose(window) == false) {
		float deltaTime = (float)FPSManagerObject.enforceFPS(true);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			kameraScene.Pozicija += kameraScene.kameraBrzina * kameraScene.smjer;
			kameraScene.CentarPozornosti += kameraScene.kameraBrzina * kameraScene.smjer;
		}

		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			kameraScene.Pozicija -= kameraScene.kameraBrzina * kameraScene.smjer;
			kameraScene.CentarPozornosti -= kameraScene.kameraBrzina * kameraScene.smjer;
		}

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			kameraScene.Pozicija += kameraScene.kameraBrzina * glm::normalize(glm::cross(kameraScene.smjer, kameraScene.kameraGoreOs));
			kameraScene.CentarPozornosti += kameraScene.kameraBrzina * glm::normalize(glm::cross(kameraScene.smjer, kameraScene.kameraGoreOs));
		}

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			kameraScene.Pozicija -= kameraScene.kameraBrzina * glm::normalize(glm::cross(kameraScene.smjer, kameraScene.kameraGoreOs));
			kameraScene.CentarPozornosti -= kameraScene.kameraBrzina * glm::normalize(glm::cross(kameraScene.smjer, kameraScene.kameraGoreOs));
		}

		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
			kameraScene.Pozicija += kameraScene.kameraBrzina * glm::normalize(glm::cross(kameraScene.smjer, kameraScene.kameraDesnaOs));
			kameraScene.CentarPozornosti += kameraScene.kameraBrzina * glm::normalize(glm::cross(kameraScene.smjer, kameraScene.kameraDesnaOs));
		}

		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
			kameraScene.Pozicija -= kameraScene.kameraBrzina * glm::normalize(glm::cross(kameraScene.smjer, kameraScene.kameraDesnaOs));
			kameraScene.CentarPozornosti -= kameraScene.kameraBrzina * glm::normalize(glm::cross(kameraScene.smjer, kameraScene.kameraDesnaOs));
		}

		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
			kameraScene.Pozicija = glm::vec3(0.0f, 0.0f, 2.0f);
			kameraScene.CentarPozornosti = glm::vec3(0.0f, 0.0f, 0.0f);
			glm::vec3 resetSmjer;
			resetSmjer.x = cos(glm::radians(-90.0f)) * cos(glm::radians(0.0f));
			resetSmjer.y = sin(glm::radians(0.0f));
			resetSmjer.z = sin(glm::radians(-90.0f)) * cos(glm::radians(0.0f));

			kameraScene.yaw = -90.0f;
			kameraScene.pitch = 0.0f;
			kameraScene.smjer = resetSmjer;

		}
		
		if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {

			cout << "Pozicija kamere je: " << kameraScene.Pozicija.x << " " << kameraScene.Pozicija.y << " " << kameraScene.Pozicija.z << endl;

			cout << "Centar pozornosti je: " << kameraScene.CentarPozornosti.x << " " << kameraScene.CentarPozornosti.y << " " << kameraScene.CentarPozornosti.z << endl;

			cout << "Smjer gledanja kamere je: " << kameraScene.smjer.x << " " << kameraScene.smjer.y << " " << kameraScene.smjer.z << endl;

			cout << "Desna os kamere je: " << kameraScene.kameraDesnaOs.x << " " << kameraScene.kameraDesnaOs.y << " " << kameraScene.kameraDesnaOs.z << endl;

			cout << "Gornja os kamere je: " << kameraScene.kameraGoreOs.x << " " << kameraScene.kameraGoreOs.y << " " << kameraScene.kameraGoreOs.z << endl;

			cout << endl;
		}


		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {

			kameraScene.kameraBrzina = 0.3f;
		}

		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) {

			kameraScene.kameraBrzina = 0.1f;
		}

		//mis dio

		glfwSetCursorPosCallback(window, mouse_callback);

		//updateaj pitch i yaw kamere za rotaciju
		kameraScene.yaw += delte[0];
		kameraScene.pitch += delte[1];
		delte[0] = 0.0f;
		delte[1] = 0.0f;
		//constraint da se kamera ne vrti u cudne kuteve
		if (kameraScene.pitch > 89.0f) {
			kameraScene.pitch = 89.0f;
		}

		if (kameraScene.pitch < -89.0f) {
			kameraScene.pitch = -89.0f;
		}

		kameraScene.updateSmjer();


		matricaPogleda = kameraScene.IzradiPogledMatricu();

		ucitanaKocka.updatePogledMatriceMeshevima(matricaPogleda);

		ucitanaKocka2.updatePogledMatriceMeshevima(matricaPogleda);

		//ucitanaKocka.IscrtajModel(*sjencar[0]);
		sjencar[1]->setUniformVec3("ociste", kameraScene.Pozicija);
		

		for (int i = 0; i < 4; i++) {

			if (i == 0) {
				//kocka bez cullinga
				ucitanaKocka.IscrtajModel(*sjencar[0], 0, TransObjekt.Translacija(glm::vec3(0.0f, 0.0f, 0.0f)));
			}

			if (i == 1) {
				//kocka sa GL cullingom
				ucitanaKocka.IscrtajModel(*sjencar[0], 1, TransObjekt.Translacija(glm::vec3(3.0f, 0.0f, 0.0f)));
			}

			if (i == 2) {
				//kocka sa algoritam 3 cullingom burasaaaaaaaaasadsassdadsa
				
				ucitanaKocka.IscrtajModel(*sjencar[2], 0, TransObjekt.Translacija(glm::vec3(6.0f, 0.0f, 0.0f)));
			}

			if (i == 3) {
				//kocka sa algoritmom 2 aaaaaaaa

				ucitanaKocka.IscrtajModel(*sjencar[1], 0, TransObjekt.Translacija(glm::vec3(9.0f, 0.0f, 0.0f)));
			}

		}

		

		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	//UNISTI
	glfwTerminate();

	return EXIT_SUCCESS;
}