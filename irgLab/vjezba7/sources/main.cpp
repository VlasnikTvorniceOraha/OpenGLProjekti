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


class Curve {
public:
	//tocke koje korisnik definira -> pozicija kamere u nekom trenu
	vector<glm::vec3> tockeKontrolnogPoligona;
	vector<glm::vec3> tockeAproksimativneKrivulje;
	vector<glm::vec3> tockeInterpolacijskeKrivulje;
	vector<glm::vec3> tockeSegmentaSplajna;
	vector<glm::vec3> tangenteSegmenta;
	vector<glm::vec3 > normaleSegmenta;
	vector<glm::mat3> RotacijskeMatrica;
	vector<glm::mat4> RotacijskeMatrice4;
	vector<Curve> segmenti;
	//broj tocaka za izracun faktora
	int brojTocaka;
	//n povrh i faktori za izracun bernstein funkcija
	vector<int> faktori;
	//razina detalja krivulje
	int divizije;
	//buffer stvari
	GLuint VAO;
	GLuint VBO[4];
	GLuint EBO;
	Curve() {
		//ucitajBuffere();
		cout << "Ucitao krivulju" << endl;
	}

	void ucitajBuffere() {
		glGenVertexArrays(1, &VAO);

		glGenBuffers(4, VBO);

		//glGenBuffers(1, &EBO);

		

		glBindVertexArray(0);
	}

	void dodajTocku(glm::vec3 tocka) {
		
		cout << "Dodajem tocku: " << tocka.x << " " << tocka.y << " " << tocka.z << endl;

		tockeKontrolnogPoligona.push_back(tocka);

		brojTocaka = tockeKontrolnogPoligona.size();

		divizije = 10 * brojTocaka;

		if (brojTocaka >= 4) {

			//uzmi zadnje 4 tocke i napravi segment Bsplajna i dodaj u vektor krivulja

			Curve BSplajnSegment;

			BSplajnSegment.dodajTockuSegmentu(tockeKontrolnogPoligona[brojTocaka - 4]);
			BSplajnSegment.dodajTockuSegmentu(tockeKontrolnogPoligona[brojTocaka - 3]);
			BSplajnSegment.dodajTockuSegmentu(tockeKontrolnogPoligona[brojTocaka - 2]);
			BSplajnSegment.dodajTockuSegmentu(tockeKontrolnogPoligona[brojTocaka - 1]);

			BSplajnSegment = izracunajTockeBSplajnKrivuljeSegmenta(BSplajnSegment);

			segmenti.push_back(BSplajnSegment);
			

		}

		/*if (brojTocaka > 1) {

			izracunajTockeAproksimativneKrivulje();
		}

		

		if (brojTocaka > 3) {

			izracunajTockeInterpolacijskeKrivulje();
		}*/

	}

	void dodajTockuSegmentu(glm::vec3 tocka) {

		//cout << "Dodajem tocku segmentu: " << tocka.x << " " << tocka.y << " " << tocka.z << endl;

		tockeKontrolnogPoligona.push_back(tocka);

		brojTocaka = tockeKontrolnogPoligona.size();

		divizije = 10 * brojTocaka;
	}

	Curve izracunajTockeBSplajnKrivuljeSegmenta(Curve segment) {

		

		float t;

		glm::mat4 Bmatrica = glm::mat4(glm::vec4(-1, 3, -3, 1), glm::vec4(3, -6, 0, 4), glm::vec4(-3, 3, 3, 1), glm::vec4(1, 0, 0, 0));
		glm::mat4x3 Bmatrica2 = glm::mat4x3(glm::vec3(-1, 2, -1), glm::vec3(3, -4, 0), glm::vec3(-3, 2, 1), glm::vec3(1, 0, 0));



			//divizije zasad samo 10
		for (int i = 0; i <= 10; i++) {

			t = 1.0f / 10 * i;

			glm::vec4 T = glm::vec4(t * t * t, t * t, t, 1);
			glm::mat3x4 TockeMatrica = glm::mat3x4(glm::vec4(segment.tockeKontrolnogPoligona[0].x, segment.tockeKontrolnogPoligona[1].x, segment.tockeKontrolnogPoligona[2].x, segment.tockeKontrolnogPoligona[3].x),
				glm::vec4(segment.tockeKontrolnogPoligona[0].y, segment.tockeKontrolnogPoligona[1].y, segment.tockeKontrolnogPoligona[2].y, segment.tockeKontrolnogPoligona[3].y),
				glm::vec4(segment.tockeKontrolnogPoligona[0].z, segment.tockeKontrolnogPoligona[1].z, segment.tockeKontrolnogPoligona[2].z, segment.tockeKontrolnogPoligona[3].z));

			glm::vec3 tockaSplajn = (1.0f / 6.0f) * T * Bmatrica * TockeMatrica;

			//cout << "Izracunata tocka splajana: " << tockaSplajn.x << " " << tockaSplajn.y << " " << tockaSplajn.z << endl;


			segment.tockeSegmentaSplajna.push_back(tockaSplajn);

			//racunanje tangente na segment

			segment.tangenteSegmenta.push_back(tockaSplajn);

			//vektor tangente

			glm::vec3 T2 = glm::vec3(t * t, t, 1);

			glm::vec3 VektorTangente = (1.0f / 2.0f) * T2 * Bmatrica2 * TockeMatrica;

			segment.tangenteSegmenta.push_back(VektorTangente + tockaSplajn);

			//Racunanje normale

			glm::vec4 T3 = glm::vec4(6 * t, 2, 0, 0);

			glm::vec3 normalaDerivacija = (1.0f / 6.0f) * T3 * Bmatrica * TockeMatrica;

			glm::vec3 normala = glm::normalize(glm::cross(VektorTangente, normalaDerivacija));

			segment.normaleSegmenta.push_back(tockaSplajn);
			segment.normaleSegmenta.push_back(tockaSplajn + normala);

			glm::vec3 binormala = glm::cross(VektorTangente, normala);

			glm::mat3 RotacijskaMatrica = glm::mat3(glm::normalize(VektorTangente), glm::normalize(normala), glm::normalize(binormala));

			glm::mat4 RotacijskaMatrica4 = glm::mat4(glm::vec4(glm::normalize(VektorTangente), 0.0f), glm::vec4(glm::normalize(normala), 0.0f), glm::vec4(glm::normalize(binormala), 0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

			segment.RotacijskeMatrica.push_back(RotacijskaMatrica);

			segment.RotacijskeMatrice4.push_back(RotacijskaMatrica4);





		}

		for (glm::vec3 tocka : segment.tockeSegmentaSplajna) {

			cout << "tocka segmenta: " << tocka.x << " " << tocka.y << " " << tocka.z << endl;

		}

		

		return segment;


	}

	void izracunajFaktore() {

		int n = brojTocaka - 1;
		int a = 1;
		for (int i = 0; i < brojTocaka; i++) {
			faktori.push_back(0);
		}

		for (int i = 1; i <= n + 1; i++) {

			faktori[i - 1] = a;

			a = a * (n - i + 1) / i;

		}
		cout << "Uspjesno izracunao faktore" << endl;
	}

	void izracunajTockeAproksimativneKrivulje() {

		int n = brojTocaka - 1;
		vector<glm::vec3> izracunateTocke;

		izracunajFaktore();

		float bernstein;
		float t;

		for (int i = 0; i <= divizije; i++) {

			t = 1.0f / divizije * i;

			glm::vec3 tocka = glm::vec3(0.0, 0.0, 0.0);

			for (int j = 0; j <= n; j++) {

				if (j == 0) {
					bernstein = faktori[j] * pow(1 - t, n);
				}
				else if (j == n) {
					bernstein = faktori[j] * pow(t, n);
				}
				else {
					bernstein = faktori[j] * pow(t, j) * pow(1 - t, n - j);
				}

				tocka.x += bernstein * tockeKontrolnogPoligona[j].x;
				tocka.y += bernstein * tockeKontrolnogPoligona[j].y;
				tocka.z += bernstein * tockeKontrolnogPoligona[j].z;

			}

			izracunateTocke.push_back(tocka);

		}

		

		for (glm::vec3 tocka: izracunateTocke) {



			std::cout << "TockaAproksi: " << tocka.x << " " << tocka.y << " " << tocka.z << endl;


		}

		tockeAproksimativneKrivulje = izracunateTocke;


	}

	void izracunajTockeInterpolacijskeKrivulje() {

		// P = MATRICA TOCAKA KONTROLNOG POLIGONA 4x3
		//T = MATRICA POTENCIJA

		vector<glm::vec3> izracunateTocke;

		glm::mat4 uniformnaMatricaPotencija(1.0f);
		uniformnaMatricaPotencija[0] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		uniformnaMatricaPotencija[1] = glm::vec4(1.0f / 27.0f, 1.0f / 9.0f, 1.0f / 3.0f, 1.0f);
		uniformnaMatricaPotencija[2] = glm::vec4(8.0f / 27.0f, 4.0f / 9.0f, 2.0f / 3.0f, 1.0f);
		uniformnaMatricaPotencija[3] = glm::vec4(1.0f, 1.0f , 1.0f , 1.0f);

		glm::mat4 uniformnaFlip = glm::inverse(uniformnaMatricaPotencija);


		glm::mat4 matricaTocakaPoligona;

		for (int i = 0; i < 4; i++) {

			matricaTocakaPoligona[i] = glm::vec4(float(tockeKontrolnogPoligona[tockeKontrolnogPoligona.size() - 1 - i].x), float(tockeKontrolnogPoligona[tockeKontrolnogPoligona.size() - 1 - i].y), float(tockeKontrolnogPoligona[tockeKontrolnogPoligona.size() - 1 - i].z), 0.0f);

		}


		glm::mat4 Kmatrica = matricaTocakaPoligona * uniformnaFlip;

		for (int i = 0; i < 4; i++) {

			for (int j = 0; j < 4; j++) {

				cout << "Vrijednost na poziciji " << i << " i " << j << " je " << Kmatrica[i][j] << " " << endl;


			}
			cout << endl;

		}

		for (int i = 0; i <= divizije; i++) {

			float t = 1.0f / divizije * i;

			glm::vec3 tocka = glm::vec3(0.0, 0.0, 0.0);

			tocka.x = pow(t, 3) * Kmatrica[0].x + pow(t, 2) * Kmatrica[1].x + t * Kmatrica[2].x + Kmatrica[3].x;
			tocka.y = pow(t, 3) * Kmatrica[0].y + pow(t, 2) * Kmatrica[1].y + t * Kmatrica[2].y + Kmatrica[3].y;
			tocka.z = pow(t, 3) * Kmatrica[0].z + pow(t, 2) * Kmatrica[1].z + t * Kmatrica[2].z + Kmatrica[3].z;

			izracunateTocke.push_back(tocka);

		}

		for (glm::vec3 tocka : izracunateTocke) {

			if (tocka.x < 1 * pow(10, -5)) {
				tocka.x = 0;
			}

			if (tocka.y < 1 * pow(10, -5)) {
				tocka.y = 0;
			}

			if (tocka.z < 1 * pow(10, -5)) {
				tocka.z = 0;
			}

			std::cout << "TockaInterpol: " << tocka.x << " " << tocka.y << " " << tocka.z << endl;


		}

		tockeInterpolacijskeKrivulje = izracunateTocke;

	}

	void iscrtajKrivulje(Shader& sjencar) {
		glUseProgram(sjencar.ID);

		glLineWidth(3);


		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);

		glBufferData(GL_ARRAY_BUFFER, tockeKontrolnogPoligona.size() * 3 * sizeof(float), &tockeKontrolnogPoligona[0], GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
		glEnableVertexAttribArray(0);

		sjencar.setUniformVec3("bojaKrivulje", glm::vec3(0.0, 0.0, 0.0));

		glDrawArrays(GL_LINE_STRIP, 0, tockeKontrolnogPoligona.size());


		if (brojTocaka > 3) {

			int brojac = 1;
			//std::cout << "for" << endl;
			for (Curve segment : segmenti) {

				glLineWidth(5);

				glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
				//std::cout << "nakon bindanja buffera" << endl;
				brojac += 1;
				

				glBufferData(GL_ARRAY_BUFFER, segment.tockeSegmentaSplajna.size() * 3 * sizeof(float) * brojac, &segment.tockeSegmentaSplajna[0], GL_DYNAMIC_DRAW);
				//std::cout << "nakon slanja podataka" << endl;
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
				//std::cout << "nakon attrib pointera" << endl;
				glEnableVertexAttribArray(0);

				sjencar.setUniformVec3("bojaKrivulje", glm::vec3(1.0, 0.0, 1.0));
				glDrawArrays(GL_LINE_STRIP, 0, segment.tockeSegmentaSplajna.size());

				//crtanje tangenata
				glLineWidth(2);
				glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
				glBufferData(GL_ARRAY_BUFFER, segment.tangenteSegmenta.size() * 3 * sizeof(float), &segment.tangenteSegmenta[0], GL_DYNAMIC_DRAW);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
				glEnableVertexAttribArray(0);
				sjencar.setUniformVec3("bojaKrivulje", glm::vec3(0.0, 1.0, 1.0));
				glDrawArrays(GL_LINES, 0, segment.tangenteSegmenta.size());

				//crtanje normale

				glLineWidth(2);
				glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
				glBufferData(GL_ARRAY_BUFFER, segment.normaleSegmenta.size() * 3 * sizeof(float), &segment.normaleSegmenta[0], GL_DYNAMIC_DRAW);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
				glEnableVertexAttribArray(0);
				sjencar.setUniformVec3("bojaKrivulje", glm::vec3(0.5, 0.5, 1.0));
				glDrawArrays(GL_LINES, 0, segment.normaleSegmenta.size());

			}


		}


		/*if (brojTocaka > 1) {

			glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);

			glBufferData(GL_ARRAY_BUFFER, tockeAproksimativneKrivulje.size() * 3 * sizeof(float), &tockeAproksimativneKrivulje[0], GL_DYNAMIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
			glEnableVertexAttribArray(0);

			sjencar.setUniformVec3("bojaKrivulje", glm::vec3(1.0, 0.0, 1.0));
			glDrawArrays(GL_LINE_STRIP, 0, tockeAproksimativneKrivulje.size());


		}

		if (brojTocaka > 3) {

			glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);

			glBufferData(GL_ARRAY_BUFFER, tockeInterpolacijskeKrivulje.size() * 3 * sizeof(float), &tockeInterpolacijskeKrivulje[0], GL_DYNAMIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
			glEnableVertexAttribArray(0);

			sjencar.setUniformVec3("bojaKrivulje", glm::vec3(0.0, 1.0, 1.0));
			glDrawArrays(GL_LINE_STRIP, 0, tockeInterpolacijskeKrivulje.size());
		}*/


		glLineWidth(1);

		glBindVertexArray(0);
	}


};

Camera kameraScene(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.1f);
Curve glavnaKrivulja;

vector<Curve> BSplajn = glavnaKrivulja.segmenti;

bool kretaraAproksimativna;

bool kretaraInterpolacijska;

bool kretaraSplajn;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {

		glavnaKrivulja.dodajTocku(kameraScene.Pozicija);
	}

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);


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

	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {

		kretaraAproksimativna = true;

	}

	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {

		kretaraInterpolacijska = true;

	}

	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {

		kretaraSplajn = true;

	}




	

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
	Shader* sjencar[2];

	//shader za stvari
	sjencar[0] = loadShader(argv[0], "maliuzi2");
	sjencar[1] = loadShader(argv[0], "maliuzi");


	//uniformne varijable

	//GLint lokacijaTransformacijskeMatrice = glGetUniformLocation(sjencar[0]->ID, "transformacijskaMatrica");

	//glm::mat4 startMatrica = glm::mat4(1.0f);aaa

	glUseProgram(sjencar[0]->ID);

	glUseProgram(sjencar[1]->ID);


	//ASSimp ucitavanje kocke

	Assimp::Importer importer;

	std::string path(argv[0]);
	std::string dirPath(path, 0, path.find_last_of("\\/"));
	std::string resPath(dirPath);
	resPath.append("\\resources"); //za linux pretvoriti u forwardslash
	std::string objPath(resPath);
	objPath.append("\\nerd.obj"); //za linux pretvoriti u forwardslash

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

	/*for (int i = 0; i < ucitanaKocka.meshVektor.size(); i++) {
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
	*/
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

	ucitanaKocka.updateNormalMatriceMeshevima(Translacija);

	ucitanaKocka.updateNormalMatriceMeshevima(Skalirara);

	ucitanaKocka.normalizirajMesheve();

	//Camera kameraScene(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.1f);

	glm::mat4 perspektivnaMatrica = kameraScene.IzradiPerspektivnuMatricu(width, height, 45.0f);

	glm::mat4 matricaPogleda = kameraScene.IzradiPogledMatricu();

	ucitanaKocka.updatePerspektivaMatriceMeshevima(perspektivnaMatrica);

	ucitanaKocka.updatePogledMatriceMeshevima(matricaPogleda);

	



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

	sjencar[1]->setUniformMat4("perspektiva", perspektivnaMatrica);
	sjencar[1]->setUniformMat4("pogled", matricaPogleda);

	//testiranje curvea dio

	glavnaKrivulja.ucitajBuffere();

	glavnaKrivulja.divizije = 10;
	//spiraloida
	glavnaKrivulja.dodajTocku(glm::vec3(0, 0, 0));
	glavnaKrivulja.dodajTocku(glm::vec3(0, 10, 5));
	glavnaKrivulja.dodajTocku(glm::vec3(10, 10, 10));
	glavnaKrivulja.dodajTocku(glm::vec3(10, 0, 15));
	glavnaKrivulja.dodajTocku(glm::vec3(0, 0, 20));
	glavnaKrivulja.dodajTocku(glm::vec3(0, 10, 25));
	glavnaKrivulja.dodajTocku(glm::vec3(10, 10, 30));
	glavnaKrivulja.dodajTocku(glm::vec3(10, 0, 35));
	glavnaKrivulja.dodajTocku(glm::vec3(0, 0, 40));
	glavnaKrivulja.dodajTocku(glm::vec3(0, 10, 45));
	glavnaKrivulja.dodajTocku(glm::vec3(10, 10, 50));
	glavnaKrivulja.dodajTocku(glm::vec3(10, 0, 55));
	

	//krivulja.izracunajTockeAproksimativneKrivulje();

	cout << endl;

	//krivulja.izracunajTockeInterpolacijskeKrivulje();

	glfwSetKeyCallback(window, key_callback);


	fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));

	int brojacTocakaA = 0;
	int brojacTocakaI = 0;
	int brojacTocakaS = 0;
	int brojacSegmenta = 0;
	

	while (glfwWindowShouldClose(window) == false) {
		float deltaTime = (float)FPSManagerObject.enforceFPS(true);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

		//krivulja put stvar

		

		if (kretaraAproksimativna) {

			kameraScene.Pozicija = glavnaKrivulja.tockeAproksimativneKrivulje[brojacTocakaA];


			brojacTocakaA += 1;

			if (brojacTocakaA == glavnaKrivulja.tockeAproksimativneKrivulje.size()) {
				kretaraAproksimativna = false;
				brojacTocakaA = 0;
			}



		

		}

		if (kretaraInterpolacijska) {

			kameraScene.Pozicija = glavnaKrivulja.tockeInterpolacijskeKrivulje[brojacTocakaI];


			brojacTocakaI += 1;

			if (brojacTocakaI == glavnaKrivulja.tockeInterpolacijskeKrivulje.size()) {
				kretaraInterpolacijska = false;
				brojacTocakaI = 0;
			}

		}

		if (kretaraSplajn) {

			//kameraScene.Pozicija = glavnaKrivulja.segmenti[brojacSegmenta].tockeSegmentaSplajna[brojacTocakaS];

			//matriceZaViseIscrtavanja[0] = TransObjekt.Translacija(glavnaKrivulja.segmenti[brojacSegmenta].tockeSegmentaSplajna[brojacTocakaS]);

			//matriceZaViseIscrtavanja[0] = TransObjekt.Translacija(glavnaKrivulja.segmenti[brojacSegmenta].tockeSegmentaSplajna[brojacTocakaS]) * glavnaKrivulja.segmenti[brojacSegmenta].RotacijskeMatrica[brojacTocakaS];
			
			matriceZaViseIscrtavanja[0] = TransObjekt.Translacija(glavnaKrivulja.segmenti[brojacSegmenta].tockeSegmentaSplajna[brojacTocakaS]) * glavnaKrivulja.segmenti[brojacSegmenta].RotacijskeMatrice4[brojacTocakaS];
			//kameraScene.smjer = glavnaKrivulja.segmenti[brojacSegmenta].tangenteSegmenta[brojacTocakaS * 2 + 1];
			//kameraScene.updateSmjer();

			brojacTocakaS += 1;

			

			if (brojacTocakaS == glavnaKrivulja.segmenti[brojacSegmenta].tockeSegmentaSplajna.size()) {

				brojacSegmenta += 1;
				brojacTocakaS = 0;
			}

			if (brojacSegmenta == glavnaKrivulja.segmenti.size()) {
				kretaraSplajn = false;
				brojacSegmenta = 0;
				brojacTocakaS = 0;
			}

			

		}





		matricaPogleda = kameraScene.IzradiPogledMatricu();

		sjencar[1]->setUniformMat4("pogled", matricaPogleda);

		ucitanaKocka.updatePogledMatriceMeshevima(matricaPogleda);

		ucitanaKocka.updateModelMatriceMeshevima(TransObjekt.Translacija(glm::vec3(1.0f, 0, 0)));

		ucitanaKocka.IscrtajViseModela(*sjencar[0], 1, matriceZaViseIscrtavanja);

		if (glavnaKrivulja.tockeKontrolnogPoligona.size() > 0) {
			glavnaKrivulja.iscrtajKrivulje(*sjencar[1]);
		}
		

		

		glfwSwapBuffers(window);

		glfwPollEvents();

		
	}

	//UNISTI
	glfwTerminate();

	return EXIT_SUCCESS;
}