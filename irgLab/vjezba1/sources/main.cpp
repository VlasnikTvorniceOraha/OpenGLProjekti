// Local Headers

// System Headers
#include "Grafika.h"

// Standard Headers
#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
#include <chrono>
#include <thread>

std::vector<std::pair<int, int> >klikovi;

std::vector<std::pair<int, int> >fragmentiLinije;

std::vector<std::pair<int, int> >obrubProstora;

//brojac klikova
int kliketanje = 0;

//varijabla za kontrolu podprostora
bool odsijecanje = false;


//bresenham algoritmi

void bresenham_desni(int x0, int y0, int x1, int y1) {
	//algoritam za drugi i cetvrti kvadrant sto se tice kuteva linije

	int x; //x koordinata koja ce se povecavati kroz algoritam

	int y; //y koordinata koja ce se povecavati kroz algoritam

	int ispravak; //matematika stvar??

	int koeficijent_smjera; //2*duljina medu epsilonima

	int pogreska; //pogreska zbog uklanjanja floatova


	if (y1 - y0 <= x1 - x0) {
		koeficijent_smjera = 2 * (y1 - y0);
		y = y0;
		pogreska = -(x1 - x0);
		ispravak = -2 * (x1 - x0);

		for (x = x0; x <= x1; x++) {
			fragmentiLinije.push_back(std::make_pair(x, y));
			pogreska = pogreska + koeficijent_smjera;

			//provjeri treba li se pomaknuti za y???
			if (pogreska >= 0) {
				pogreska = pogreska + ispravak;
				y++;
			}
		}
	}
	else {
		// zamijena osi tj koordinata
		x = x1;
		x1 = y1;
		y1 = x;
		x = x0;
		x0 = y0;
		y0 = x;

		koeficijent_smjera = 2 * (y1 - y0);
		y = y0;
		pogreska = -(x1 - x0);
		ispravak = -2 * (x1 - x0);

		for (x = x0; x <= x1; x++) {
			fragmentiLinije.push_back(std::make_pair(y, x));
			pogreska = pogreska + koeficijent_smjera;

			//provjeri treba li se pomaknuti za x???
			if (pogreska >= 0) {
				pogreska = pogreska + ispravak;
				y++;
			}
		}

	}

}

void bresenham_lijevi(int x0, int y0, int x1, int y1) {
	//algoritam za prvi i treci kvadrant sto se tice kuteva

	int x; //x koordinata koja ce se povecavati kroz algoritam

	int y; //y koordinata koja ce se povecavati kroz algoritam

	int ispravak; //matematika stvar??

	int koeficijent_smjera; //2*duljina medu epsilonima

	int pogreska; //pogreska zbog uklanjanja floatova


	if (-(y1 - y0) <= x1 - x0) {
		koeficijent_smjera = 2 * (y1 - y0);
		y = y0;
		pogreska = x1 - x0;
		ispravak = 2 * (x1 - x0);

		for (x = x0; x <= x1; x++) {
			fragmentiLinije.push_back(std::make_pair(x, y));
			pogreska = pogreska + koeficijent_smjera;

			//provjeri treba li se pomaknuti za y???
			if (pogreska <= 0) {
				pogreska = pogreska + ispravak;
				y--;
			}
		}
	}
	else {
		// zamijena osi tj koordinata
		x = x1;
		x1 = y0;
		y0 = x;
		x = x0;
		x0 = y1;
		y1 = x;

		koeficijent_smjera = 2 * (y1 - y0);
		y = y0;
		pogreska = x1 - x0;
		ispravak = 2 * (x1 - x0);

		for (x = x0; x <= x1; x++) {
			fragmentiLinije.push_back(std::make_pair(y, x));
			pogreska = pogreska + koeficijent_smjera;

			//provjeri treba li se pomaknuti za x???
			if (pogreska <= 0) {
				pogreska = pogreska + ispravak;
				y--;
			}
		}

	}

}




void iscrtajLiniju(Grafika &grafika, int x0, int y0, int x1, int y1) {
	//iscrtavat ce se u zasebnom dijelu koda ne ovdje
	//grafika.osvijetliFragment(x0, y0, glm::vec3(0, 0.8, 0));

	//pocetna i krajnja tocka linije, maknuti kasnije da se vide te tocke
	//fragmentiLinije.push_back(std::make_pair(x0, y0));
	//fragmentiLinije.push_back(std::make_pair(x1, y1));

	//odabir varijante bresenhama i algoritam

	//crta se iscrtava od lijevo na desno, pa usporedujemo koordinate tocke
	//ako je kraj lijevije od pocetka, tocke se flipaju u funkciji
	if (x0 <= x1) {

		//odredivanje kuta ispisa, izmedu 0 i 90 te 0 i -90
		if (y0 <= y1) {
			bresenham_desni(x0, y0, x1, y1);
		}
		else {
			bresenham_lijevi(x0, y0, x1, y1);
		}

	}
	else {

		//ista stvar ali s druge strane koordinatnog sustava
		if (y0 >= y1) {
			bresenham_desni(x1, y1, x0, y0);
		} 
		else {
			bresenham_lijevi(x1, y1, x0, y0);
		}

	}



}

void klikMisa(int x, int y, int vrsta) {

	if (vrsta == 0) {
		std::cout << "lijevi:";
		std::cout << x << " " << y << std::endl;
		klikovi.push_back(std::make_pair(x, y));
		kliketanje++;
	}
		


	if (vrsta == 1) {
		std::cout << "desni klik, odsijecanje je sada ";

		odsijecanje = !odsijecanje;

		if (odsijecanje == true) {
			std::cout << "ukljuceno!" << std::endl;
		}
		else {
			std::cout << "iskljuceno!" << std::endl;
		}

		


	}
		

		

	

	

}

int main(int argc, char * argv[]) {

	int width = 97, height = 97;
	std::cout << argv[0] << std::endl;
	Grafika grafika(width, height, glm::vec3(0, 0, 0), argv[0]);

	//dodavanje rubova prostora za kasnije iscrtavanje


	obrubProstora.push_back(std::make_pair(24, 24));
	obrubProstora.push_back(std::make_pair(24, 72));
	obrubProstora.push_back(std::make_pair(72, 72));
	obrubProstora.push_back(std::make_pair(72, 24));

	//prosljeduje se pokazivac na funkciju. Ta funkcija ce se pozvati kada korisnik pritisne misem
	grafika.registrirajFunkcijuZaKlikMisa(klikMisa);

	while (grafika.trebaZatvoriti()) {

		//osvjezavanje pozicija razlicitih objekata i fizikalna simulacija bi se izvrsavala u ovom dijelu

		grafika.pobrisiProzor();

		//iscrtavanje uzorka sahovnice
		for (int i = 0; i < height; i += 1)
			for (int j = 0; j < width; j += 1) {
				if ((i + j) % 2 == 0)
					grafika.osvijetliFragment(i, j, glm::vec3(0.1, 0.1, 0.1));
				if (i % 10 == 0 && j % 10 == 0)
					grafika.osvijetliFragment(i, j, glm::vec3(0.15, 0.15, 0.2));
			}

		//hendlanje brojaca klikova i zvanje funkcije

		if (kliketanje == 2) {
			kliketanje = 0;
			iscrtajLiniju(grafika, klikovi[klikovi.size() - 2].first, klikovi[klikovi.size() - 2].second, klikovi[klikovi.size() - 1].first, klikovi[klikovi.size() - 1].second);
		}

		//odsijecanje blok
		if (odsijecanje) {


			for (int i = 0; i < 49; i++) {
				grafika.osvijetliFragment(obrubProstora[0].first + i, obrubProstora[0].second, glm::vec3(0, 1, 0));
				grafika.osvijetliFragment(obrubProstora[0].first, obrubProstora[0].second + i, glm::vec3(0, 1, 0));
				grafika.osvijetliFragment(obrubProstora[2].first - i, obrubProstora[2].second, glm::vec3(0, 1, 0));
				grafika.osvijetliFragment(obrubProstora[2].first, obrubProstora[2].second - i, glm::vec3(0, 1, 0));
			}
			

		}

		//iscrtavanje pritisnutih fragmenata
		//ishodiste koordinatnog sustava za operacijski sustav je u gornjem lijevom kutu, a za OpenGL je u donjem lijevom, pa je potrebno okrenuti predznak
		for (int i = 0; i < klikovi.size(); i++) {

			if (odsijecanje) {
				if ((klikovi[i].first >= 24 && klikovi[i].first <= 72) && (klikovi[i].second >= 24 && klikovi[i].second <= 72)) {
					grafika.osvijetliFragment(klikovi[i].first, height - klikovi[i].second - 1, glm::vec3(0.6, 0.2, 0));
				}

			}
			else {
				grafika.osvijetliFragment(klikovi[i].first, height - klikovi[i].second - 1, glm::vec3(0.6, 0.2, 0));
			}

		}
			


		//iscrtavaje dijelova linije
		for (int i = 0; i < fragmentiLinije.size(); i++) {

			

			

			if (odsijecanje) {
				if ((fragmentiLinije[i].first >= 24 && fragmentiLinije[i].first <= 72) && (fragmentiLinije[i].second >= 24 && fragmentiLinije[i].second <= 72)) {
					grafika.osvijetliFragment(fragmentiLinije[i].first, height - fragmentiLinije[i].second - 1, glm::vec3(0, 1, 1));
				}

			}
			else {
				grafika.osvijetliFragment(fragmentiLinije[i].first, height - fragmentiLinije[i].second - 1, glm::vec3(0, 1 , 1));
			}

		}
		

		grafika.iscrtajRaster();

		//kako program ne bi trosio previse resursa, na kraj petlje se moze dodati poziv funkcije za cekanje
		//kao npr Sleep ili od c++11 na dalje this_thread::sleep_for(chrono::milliseconds(16));
		std::this_thread::sleep_for(std::chrono::milliseconds(16));

	}   
	

    return EXIT_SUCCESS;
}
