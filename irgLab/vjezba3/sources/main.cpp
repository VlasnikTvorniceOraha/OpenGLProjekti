// Local Headers

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"
#include "FPSManager.h"

// Standard Headers
#include <cstdio>
#include <cstdlib>
#include <vector>

#include <iostream>

int width = 500, height = 500;

//vektor vrhova
std::vector<float>Vrhovi;

//vektor boja vrhova (interpolirat ce ih ali ok)
std::vector<float>BojeVrhova;

//indeksi vrhova po kojima ce drawelements rasporediti trokute velicina je 3*broj trokuta
std::vector<int>indeksiVrhova;

//polje za trenutnu boju
GLfloat boje[3];

//malo je nespretno napravljeno jer ne koristimo c++17, a treba podrzati i windows i linux,
//slobodno pozivajte new Shader(...); direktno -> maliuzi.vert
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

//funkcija koja se poziva prilikom mijenjanja velicine prozora, moramo ju povezati pomocu glfwSetFramebufferSizeCallback
void framebuffer_size_callback(GLFWwindow * window, int Width, int Height)
{
	width = Width;
	height = Height;

	//postavi trenutnu radnu povrsinu na trenutnu velicinu prozora
	glViewport(0, 0, width, height);

}
  
//funkcija koja detektira mouse input -> direktno iz glfw prirucnika
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	//pozicija misa
	double xposdouble, yposdouble;
	//pozicija misa ali float
	float xpos, ypos;

	//funkcija koja vraca poziciju misa u double pa se mora pretvorit u float
	glfwGetCursorPos(window, &xposdouble, &yposdouble);

	//el pretvorba u float
	xpos = (float)xposdouble;
	ypos = (float)yposdouble;

	//pretvaranje iz 0-width, 0-height sustava u -1 1 sustav
	xpos = (xpos - width / 2) / (width / 2);

	ypos = (ypos - height / 2) / (height / 2);

	//ako lijevi klik pritisnut
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {

		//pushaj trenutnu poziciju misa i 0 (tehnicki smo u 3D prostoru ali ignoriramo trecu koordinatu pa je 2D)
		Vrhovi.push_back(xpos);
		Vrhovi.push_back(-ypos);
		Vrhovi.push_back(0.0f);

		//spremanje r,g i b trenutne boje da se kasnije poveze sa vrhom
		BojeVrhova.push_back(boje[0]);
		BojeVrhova.push_back(boje[1]);
		BojeVrhova.push_back(boje[2]);

		//printer
		std::cout << "Spremio sam klik na poziciji " << xpos << " i " << ypos << std::endl;

		//ako ima barem tri tocke spremi njihove indekse
		if (Vrhovi.size() >= 9) {
			
			//sprema najmlade 3 tocke kao indeksi novog trokuta koji ce se poslije iscrtati pomocu drawElements
			indeksiVrhova.push_back((Vrhovi.size() / 3) - 3);
			indeksiVrhova.push_back((Vrhovi.size() / 3) - 2);
			indeksiVrhova.push_back((Vrhovi.size() / 3) - 1);
		}

	}

		
}




//colonel vacap
int main(int argc, char * argv[]) {
	std::cout << argv[0] << std::endl;
	/*********************************************************************************************/
	//postavljanje OpenGL konteksta, dohvacanje dostupnih OpenGL naredbi
	GLFWwindow* window;

	glfwInit();
	gladLoadGL();

	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//gejfon
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	//buraz ovo je hint za verziju
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

	//stvaranje prozora
	window = glfwCreateWindow(width, height, "Zadatak 3", nullptr, nullptr);
	// provjeri je li se uspio napraviti prozor
	if (window == nullptr) {
		fprintf(stderr, "Failed to Create OpenGL Context");
		exit(EXIT_FAILURE);
	}
	//reci grafickoj da zelis raditi u prozoru koji si stvorio (glupa je)
	glfwMakeContextCurrent(window);

	// dohvati sve dostupne OpenGL funkcije
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		fprintf(stderr, "Failed to initialize GLAD");
		exit(-1);
	}
	fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));


	glEnable(GL_DEPTH_TEST); //ukljuci z spremnik (prikazuju se oni fragmenti koji su najblizi promatracu)
	glDepthFunc(GL_LESS);

	//glEnable(GL_CULL_FACE); //ukljuci uklanjanje straznjih poligona -- za ovaj primjer je iskljuceno
	//glCullFace(GL_BACK); 


	//mijenjanje boja dio!!!!

	//neke random default boje
	boje[0] = 0.1;
	boje[1] = 0.1;
	boje[2] = 0.1;
	//on pokazuje na r,g ili b
	GLint pokazivac = 0;

	glClearColor(boje[0], boje[1], boje[2], 1); //boja brisanja platna izmedu iscrtavanja dva okvira


	glfwSwapInterval(0); //ne cekaj nakon iscrtavanja (vsync)


	FPSManager FPSManagerObject(window, 60, 1.0, "Zadatak 3");

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); //funkcija koja se poziva prilikom mijenjanja velicine prozora
	   	 
	
	/*********************************************************************************************/
	//indeksirani vrhovi i boje u odvojenim poljima. Konkretni podaci (ovo je ostalo od primjera, nisam htio to izbrisati)
	/*
	//svaki redak je jedna koordinata (x, y, z)
	float indeksiraniVrhovi[18] = {
			-1,  -1, 0,
			 1,  -1, 0,
			 0,   1, 0,
			-0.4, -0.1, 0,
			 0.4, -0.1, 0,
			 0,  -0.9, 0
	};

	

	//svaki redak je jedna boja vezana uz vrh na istoj poziciji kao u prethodnom polju (r, g, b)
	float indeksiraneBoje[18] = {
		1, 0, 0,
		0, 1, 0,
		0, 0, 1,
		0, 1, 1,
		1, 0, 1,
		1, 1, 0
	};

	
	

	//svaki redak je jedan trokut
	unsigned int indeksi[12] = {
		0, 5, 3,
		3, 5, 4,
		5, 1, 4,
		3, 4, 2
	};

	*/

	//vertexi el kvadrata s trenutnom bojom

	float kvadratKoordinate[12] = {
		//  koordinate gore lijevo
			1.0f, 1.0f, 0.0f,
			0.8f, 1.0f, 0.0f,
			1.0f, 0.8f, 0,
			0.8f, 0.8f, 0.0f
	};

	
	GLboolean postojiTocka = false; // ako postoji tocka onda izgeneriraj buffere za linije pomocu vrhova jer se crasha ako je vektor prazan







	//ucitavanje sjencara i dohvat uniform varijable

	Shader* sjencar[3];
	//shader za kvadrat
	sjencar[0] = loadShader(argv[0], "shader");
	//shader za liniju
	sjencar[1] = loadShader(argv[0], "shader");
	//shader za trokute
	sjencar[2] = loadShader(argv[0], "maliuzi");

	//uniformna varijabla kojom saljemo podatke sjencaru
	
	//uniformna varijabla za boju kvadrata
	GLint lokacijaUniformBojaKvadrataVarijable = glGetUniformLocation(sjencar[0]->ID, "u_color");

	//uniformna varijabla za boju linije
	GLint lokacijaUniformBojeLinijeVarijable = glGetUniformLocation(sjencar[1]->ID, "u_color");


	/*********************************************************************************************/
	//prenosenje podataka i objasnjavanje u kojem formatu su ti podaci
	//generiranje buffera

	//Vertex Array Object, ima sve bitne VBO u sebi i sluzi za pronalazenje VBOa, mijenjanje VBOa i govori kako da interpretira VBO
	GLuint VAO[3];
	//Vertex buffer object, sprema podatke o vertexu objekta i ima ih onoliko koliko ima objekata za iscrtavanje
	GLuint VBO[5];
	GLuint EBO;


	//generiraj VAO, na jednom objektu
	glGenVertexArrays(3, VAO);

	//generira buffer za objekt, zasad samo 4
	glGenBuffers(5, VBO);

	//stvori EBO za indicesoide
	glGenBuffers(1, &EBO);

	//isto binda trenutni VAO kao glavni
	glBindVertexArray(VAO[0]);

	//VBO 0 I VAO 0 za kvadrat
	//binda trenutni object kao glavni da moze vrsit promjene nad njime
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	//prosljedivanje samih podataka grafickoj, buffer, velicina podataka, podatci i nacin koristenja (STREAM, STATIC, DYNAMIC) i (READ, COPY, DRAW)
	glBufferData(GL_ARRAY_BUFFER, sizeof(kvadratKoordinate), kvadratKoordinate, GL_STATIC_DRAW);



	//objasni glupom shaderu sto treba radit sa hrpom instrukcija
	//pozicija atribute vertexa, 0 - koordinate vrhova, 1 - boje vertexa
	//koliko vrijednosti ima svaka tocka, 2, 3 ili 4
	//vrsta varijable vrhova
	//je li vektor normaliziran
	//kolika je velicina podataka svakog vrha (3 * float za tri koordinate)
	//gdje pocinju bitni podaci od pocetka liste za ovaj tip podataka o vrhovima
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	//ukljuci ovaj vertex atribut
	glEnableVertexAttribArray(0);

	//zakomentirani stari dio


	/*glGenVertexArrays(1, &VAO);
	glGenBuffers(2, VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
		//buffer za koordinate i povezi s nultim mjestom u sjencaru -- layout (location = 0)
		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(indeksiraniVrhovi), indeksiraniVrhovi, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		
		//buffer za boje i povezi s prvim mjestom u sjencaru -- layout (location = 1)
		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(indeksiraneBoje), indeksiraneBoje, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		//buffer za indekse, moze biti samo jedan GL_ELEMENT_ARRAY_BUFFER po VAO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indeksi), (void*)(&indeksi[0]), GL_STATIC_DRAW);
	glBindVertexArray(0);*/
	
	




	
	/*********************************************************************************************/
	//"instanciranje objekata" svaka matrica reprezentira novu instancu objekta. Izrada polja transformacija koji  postavljaju objekte u mrezu 4x4
	/*
	glm::mat4 jedinicna = glm::mat4(1);
	glm::mat4 skaliranje = glm::scale(jedinicna, glm::vec3(0.25, 0.25, 0.25));

	glm::mat4 poljeTransformacija[32];
	
	glm::vec3 osRotacije = glm::vec3(1, 0, 0);
	float kutRotacije = 3.1415 / 32;

	int brojac = 0;
	for (float i = -1; i < 1; i+=0.5) {
		for (float j = -1; j < 1; j+=0.5) {
			poljeTransformacija[brojac] =  glm::translate(jedinicna, glm::vec3(j+0.25, i+0.25 , 0)) 
										* skaliranje;

			poljeTransformacija[brojac + 16] =  glm::translate(jedinicna, glm::vec3(j + 0.25, i + 0.25, 0)) *
												glm::rotate(jedinicna, brojac * kutRotacije, osRotacije) * 
												glm::rotate(jedinicna, (float)3.141592, glm::vec3(0, 0, 1)) *
												skaliranje;
			brojac++;
		}
	}

	+/
	/*********************************************************************************************/
	
	//definicija pozicije misa, ali ovaj puta u mainu
	double xposdouble, yposdouble;

	float xpos, ypos;

	//glavna petlja za prikaz
	while (glfwWindowShouldClose(window) == false) {

		float deltaTime = (float)FPSManagerObject.enforceFPS(true);

		//funkcija koja se zove ako se klikne misem
		glfwSetMouseButtonCallback(window, mouse_button_callback);
		

		//pobrisi platno
		glClearColor(1, 1, 1, 1);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		/*
		//osvjezavanje podataka 
		//poljeTransformacija[28] = poljeTransformacija[28]* glm::rotate(jedinicna, deltaTime *(float)3.141592/4, glm::vec3(0, 0, 1));*/
		


		//Pozicija misa u ovom frameu
		glfwGetCursorPos(window, &xposdouble, &yposdouble);

		xpos = (float)xposdouble;
		ypos = (float)yposdouble;

		xpos = (xpos - width / 2) / (width / 2);

		ypos = (ypos - height / 2) / (height / 2);



		

		
		//prepoznavanje inputa koji mijenja boju kvadrata i time vrhova
		if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {

			if (boje[pokazivac] <= 1) {
				boje[pokazivac] = boje[pokazivac] + 0.02;
			}

		}

		if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {

			if (boje[pokazivac] >= 0) {
				boje[pokazivac] = boje[pokazivac] - 0.02;
			}

		}

		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {

			pokazivac = 0;

		}

		if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {

			pokazivac = 1;

		}

		if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {

			pokazivac = 2;

		}


		
		//ovo ce runati kada postoji tocno jedna tocka i postaviti ce bool za kasniji dio programa

		if (Vrhovi.empty() == false && postojiTocka == false) {

			postojiTocka = true;

			//VAO 1 te VBO 1 za linije (boja ide kroz uniformnu)

			glBindVertexArray(VAO[1]);

			glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);

			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * Vrhovi.size(), &Vrhovi[0], GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

			glEnableVertexAttribArray(0);

			//std::cout << "Mlatim majmunaa" << std::endl;

		}

		//ako postoji tocka odi ovdje crtat n shit

		if (postojiTocka) {

			//sjencar linije
			glUseProgram(sjencar[1]->ID);
			
			//linije su crvene boje
			glUniform3f(lokacijaUniformBojeLinijeVarijable, 1, 0, 0);

			glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);

			


			glLineWidth(3);

			//kada postoji samo jedna tocka
			if (Vrhovi.size() == 3) {

				//dobi tu liniju i strpaj je u buffer, linija ide od jedine definirane tocke do pozicije misa
				float trenutnaLinija[6]{
				Vrhovi[Vrhovi.size() - 3], Vrhovi[Vrhovi.size() - 2], Vrhovi[Vrhovi.size() - 1],
				xpos, -ypos, 0.0f
				};

				//buraz aj iscrtaj liniju
				glBufferData(GL_ARRAY_BUFFER, sizeof(trenutnaLinija), trenutnaLinija, GL_STATIC_DRAW);

				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


				glEnableVertexAttribArray(0);



				glBindVertexArray(VAO[1]);


				glDrawArrays(GL_LINE_STRIP, 0, 2);

			}

			//kada ima vise od dvije tocke onda je ovaj postupak
			if (Vrhovi.size() >= 6) {

				//matrica za radenje trokuta od linije, GL_LINE_STRIP uzima vertexe po redu ko susjedne i crta n-1 linija znaci treba 4 tocke da se iscrta trokut
				float trenutnaLinija[3*4]{
					Vrhovi[Vrhovi.size() - 6], Vrhovi[Vrhovi.size() - 5], Vrhovi[Vrhovi.size() - 4],
					Vrhovi[Vrhovi.size() - 3], Vrhovi[Vrhovi.size() - 2], Vrhovi[Vrhovi.size() - 1],
					xpos, -ypos, 0.0f,
					Vrhovi[Vrhovi.size() - 6], Vrhovi[Vrhovi.size() - 5], Vrhovi[Vrhovi.size() - 4]
				};

				glBufferData(GL_ARRAY_BUFFER, sizeof(trenutnaLinija), trenutnaLinija, GL_STATIC_DRAW);

				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

				glEnableVertexAttribArray(0);

				glBindVertexArray(VAO[1]);

				//iscrtaj za 4 tocke iliti napravi trokut od linija
				glDrawArrays(GL_LINE_STRIP, 0, 4);
				
			}

			//makni vao da se ne polomi (mnogo takvih slucajeva)
			glBindVertexArray(0);

		}


		//CRTANJE TROKUTORA

		//kada ima 3 vrha onda je spremno napravit trokut, svi podaci se generiraju u funkciji za klik misa, ali iscrtavaju se za svaki frame
		if (Vrhovi.size() >= 9) {

			//shoutout mali uzi vert
			glUseProgram(sjencar[2]->ID);

			glBindVertexArray(VAO[2]);

			//salji mi vrhove pls
			glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float)* Vrhovi.size(), &Vrhovi[0], GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

			//salji boje za svaki vrh pls
			glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * BojeVrhova.size(), &BojeVrhova[0], GL_STATIC_DRAW);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);

			//salji indekse vrhova da znam iscrtat UwU
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * indeksiVrhova.size(), (void*)(&indeksiVrhova[0]), GL_STATIC_DRAW);


			glDrawElements(GL_TRIANGLES, sizeof(float) * indeksiVrhova.size(), GL_UNSIGNED_INT, 0);

			glBindVertexArray(0);




		}

		

		//le bojanje i stvaranje kvadrata POJAVIO SE
		glUseProgram(sjencar[0]->ID);
		//svaki frame crtaj bojom koja se moze promijenit i pomocu prikladne uniformne varijable
		glUniform3f(lokacijaUniformBojaKvadrataVarijable, boje[0], boje[1], boje[2]);
		
		glBindVertexArray(VAO[0]);

		//nacrtaj 2 trokuta iliti kvadrat, VAZAN REDOSLIJED VRHOVA
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glBindVertexArray(0);






			/*for (int i = 0; i <32; i++) {
				glUniformMatrix4fv(lokacijaUniformVarijable, 1, GL_FALSE, &poljeTransformacija[i][0][0]);
				glDrawElements(GL_TRIANGLES, sizeof(indeksi), GL_UNSIGNED_INT, 0);  
			}*/
		//glBindVertexArray(0);

		//updateaj skrin
		glfwSwapBuffers(window);

		//radi svoj posao primanja glfw stvari
		glfwPollEvents();

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

	}
		
	//brisi sve stvari, sta je sad se ne crvenite mrzim vas sve
	delete sjencar;
	glDeleteBuffers(5, VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(3, VAO);

	//UNISTI
	glfwTerminate();

    return EXIT_SUCCESS;
}
