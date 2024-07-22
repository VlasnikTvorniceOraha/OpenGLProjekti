// Local Headers

// System Headers
#include "Grafika.h"

// Standard Headers
#include <cstdio>
#include <cstdlib>

#include <iostream>
#include <chrono>
#include <thread>

class Linija {

public:
	std::pair<int, int>pocetak;
	std::pair<int, int>kraj;
	std::vector<std::pair<int, int> >fragmenti;

	//koeficijenti jednadzbe pravca ax+by+ch
	int koeficijenti[3];


	void izracunajKoeficijente() {

		//izracunaj a, b i c koeficijente u homogenom prostoru za ovu liniju
		koeficijenti[0] = pocetak.second - kraj.second;
		koeficijenti[1] = -(pocetak.first - kraj.first);
		koeficijenti[2] = pocetak.first * kraj.second - pocetak.second * kraj.first;

	}

};



std::vector<std::pair<int, int> >klikovi;

std::vector<std::pair<int, int> >testTocke;

std::vector<std::pair<int, int> >zaObojati;

std::vector<Linija>SveLinije;

bool bojanjeDovrseno = false;

//polje od dva clana za definiranje orijentacije cijelog poligona (odreduje orijentaciju na cetvrtoj tocki kada provede dvije usporedbe)
int provjeraOrijentacije[2];




//kada se poligon iscrta disableati lijevi klik
bool poligonIscrtan = false;




//bool koji oznacava kada treba iscrtati liniju
bool trebaIscrtatLiniju = false;

//bool za zadnju liniju
bool finalnaLinija = false;

int ispodIznad(Linija& linija, std::pair<int, int>tocka) {

	return linija.koeficijenti[0] * tocka.first + linija.koeficijenti[1] * tocka.second + linija.koeficijenti[2];

}







void bresenham_desni(int x0, int y0, int x1, int y1, Linija& linija) {
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
			
			linija.fragmenti.push_back(std::make_pair(x, y));
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
			
			linija.fragmenti.push_back(std::make_pair(y, x));
			pogreska = pogreska + koeficijent_smjera;

			//provjeri treba li se pomaknuti za x???
			if (pogreska >= 0) {
				pogreska = pogreska + ispravak;
				y++;
			}
		}

	}

}

void bresenham_lijevi(int x0, int y0, int x1, int y1, Linija& linija) {
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
			
			linija.fragmenti.push_back(std::make_pair(x, y));
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
		
			linija.fragmenti.push_back(std::make_pair(y, x));
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

	//objekt klase linije koji ce se spremiti u listu
	Linija linija;

	linija.pocetak = std::make_pair(x0, y0);
	linija.kraj = std::make_pair(x1, y1);

	if (x0 <= x1) {

		//odredivanje kuta ispisa, izmedu 0 i 90 te 0 i -90
		if (y0 <= y1) {
			bresenham_desni(x0, y0, x1, y1, linija);
		}
		else {
			bresenham_lijevi(x0, y0, x1, y1, linija);
		}

	}
	else {

		//ista stvar ali s druge strane koordinatnog sustava
		if (y0 >= y1) {
			bresenham_desni(x1, y1, x0, y0, linija);
		}
		else {
			bresenham_lijevi(x1, y1, x0, y0, linija);
		}

	}

	linija.izracunajKoeficijente();

	//std::cout << linija.koeficijenti[0] << " " << linija.koeficijenti[1] << " " << linija.koeficijenti[2];

	SveLinije.push_back(linija);

	/*std::cout << "Upravo sam spremio liniju sa atributima: " << linija.pocetak.first << " " << linija.pocetak.second << " , " << linija.kraj.first << " " << linija.kraj.second << " i fragmentima velicine " << linija.fragmenti.size() << std::endl;

	for (int i = 0; i < linija.fragmenti.size(); i++) {
		std::cout << linija.fragmenti[i].first << " " << linija.fragmenti[i].second << std::endl;
	}*/

}







//funkcije za bojanje poligona


void obojajPoligon() {


	//nadi najveci i najmanji Y i X za pucanje zraka
	int najmanjiY = klikovi[0].second;
	int najveciY = klikovi[0].second;

	int najmanjiX = klikovi[0].first;
	int najveciX = klikovi[0].first;




	for (int i = 1; i < klikovi.size(); i++) {

		

		if (klikovi[i].second < najmanjiY) {
			najmanjiY = klikovi[i].second;
		}

		if (klikovi[i].second > najveciY) {
			najveciY = klikovi[i].second;
		}

		if (klikovi[i].first < najmanjiX) {
			najmanjiX = klikovi[i].first;
		}

		if (klikovi[i].first > najveciX) {
			najveciX = klikovi[i].first;
		}

	}

	std::cout << najmanjiY << " " << najveciY << "   " << najmanjiX << " " << najveciX << std::endl;

	bool bojanjeAktivno = false;

	//pucanje zraka!!

	double lijevoSjeciste;
	double desnoSjeciste;

	int lijevoSjeciste1;
	int desnoSjeciste1;

	Linija rastuciBrid;
	Linija padajuciBrid;
	
	for (int y = najmanjiY + 1; y < najveciY; y++) {
		lijevoSjeciste = najmanjiX;
		desnoSjeciste = najveciX;

		for (int j = 0; j < SveLinije.size(); j++) {

			if (SveLinije[j].pocetak.second >= y && SveLinije[j].kraj.second <= y) {

				rastuciBrid = SveLinije[j];

			}

			if (SveLinije[j].pocetak.second <= y && SveLinije[j].kraj.second >= y) {

				padajuciBrid = SveLinije[j];

			}



		}

		//smjer kazaljke na satu
		if (provjeraOrijentacije[0] == 1) {

			lijevoSjeciste = (-rastuciBrid.koeficijenti[1] * y - rastuciBrid.koeficijenti[2]) / (double)(rastuciBrid.koeficijenti[0]);

			desnoSjeciste = (-padajuciBrid.koeficijenti[1] * y - padajuciBrid.koeficijenti[2]) / (double)(padajuciBrid.koeficijenti[0]);

			lijevoSjeciste1 = round(lijevoSjeciste);

			desnoSjeciste1 = round(desnoSjeciste);

			

		}

		if (provjeraOrijentacije[0] == -1) {

			desnoSjeciste = (-rastuciBrid.koeficijenti[1] * y - rastuciBrid.koeficijenti[2]) / (double)(rastuciBrid.koeficijenti[0]);

			lijevoSjeciste = (-padajuciBrid.koeficijenti[1] * y - padajuciBrid.koeficijenti[2]) / (double)(padajuciBrid.koeficijenti[0]);

			lijevoSjeciste1 = round(lijevoSjeciste);

			desnoSjeciste1 = round(desnoSjeciste);

		}

		std::cout << "Otkrivena sjecista su: " << lijevoSjeciste1 << " i " << desnoSjeciste1 << " za y od " << y << std::endl;

		for (int i = lijevoSjeciste1 + 1; i < desnoSjeciste1; i++) {

			zaObojati.push_back(std::make_pair(i, y));

		}

		bojanjeDovrseno = true;


	}

	

}

void klikMisa(int x, int y, int vrsta) {

	if (vrsta == 0) {

		if (poligonIscrtan) {
			std::cout << "Lijevi klik zabranjen jer ste vec definirali poligon!" << std::endl;
			return;
		}

		std::cout << "lijevi:";
		std::cout << x << " " << y << std::endl;
		klikovi.push_back(std::make_pair(x, y));

		
		if (klikovi.size() == 2) {
			trebaIscrtatLiniju = true;
			return;
		}


		if (klikovi.size() > 2) {

			int provjera = 0;

			//prva provjera
			if (provjeraOrijentacije[0] == -2) {

				provjera = ispodIznad(SveLinije.back(), klikovi.back());

				if (provjera == 0) {

					std::cout << "Nemojte odabrati tocku na istom bridu, to nije lijepo!" << std::endl;
					klikovi.pop_back();

					return;
				}

				if (provjera > 0) {

					provjeraOrijentacije[0] = 1;
				}
				else {
					provjeraOrijentacije[0] = -1;
				}

				trebaIscrtatLiniju = true;
				return;

			}


			//druga provjera i odlucivanje smjera
			if (provjeraOrijentacije[1] == -2) {

				provjera = ispodIznad(SveLinije.back(), klikovi.back());

				if (provjera == 0) {

					std::cout << "Nemojte odabrati tocku na istom bridu, to nije lijepo!" << std::endl;
					klikovi.pop_back();

					return;
				}

				if (provjera > 0) {

					provjeraOrijentacije[1] = 1;
				}
				else {
					provjeraOrijentacije[1] = -1;
				}

				if (provjeraOrijentacije[0] != provjeraOrijentacije[1]) {

					std::cout << "Ova tocka bi ucinila poligon konkavnim, TO SE NE SMIJE!!!" << std::endl;
					provjeraOrijentacije[1] = -2;
					klikovi.pop_back();
					return;
				}

				trebaIscrtatLiniju = true;
				return;

			}

			//provjera orijentacije je gotova pa se moze odrediti valja li ova tocka ili ne
			provjera = ispodIznad(SveLinije.back(), klikovi.back());

			if (provjera > 0 && provjeraOrijentacije[0] == -1) {

				std::cout << "Ova tocka bi ucinila poligon konkavnim, TO SE NE SMIJE!!!" << std::endl;
				klikovi.pop_back();
				return;

			}

			if (provjera < 0 && provjeraOrijentacije[0] == 1) {
				std::cout << "Ova tocka bi ucinila poligon konkavnim, TO SE NE SMIJE!!!" << std::endl;
				klikovi.pop_back();
				return;
			}


			

			trebaIscrtatLiniju = true;
		}


	}
		
		
	//DESNI KLIK TERITORIJ!!!!!!!!!!!!!!!!!!!!!!!	
	if (vrsta == 1) {

		std::cout << "desni klik detektiran" << std::endl;
		if (klikovi.size() < 3) {
			std::cout << "Definirajte barem 3 tocke za poligon!" << std::endl;
			return;
		}

		if (poligonIscrtan == true && finalnaLinija == false && bojanjeDovrseno == false) {

			
			obojajPoligon();

		}

		if (!poligonIscrtan) {
			poligonIscrtan = true;

			finalnaLinija = true;

			return;
		}

		//int testiranje = ispodIznad()
		


		testTocke.push_back(std::make_pair(x, y));
	}
		

		

	
}



int main(int argc, char * argv[]) {

	int width = 97, height = 97;
	std::cout << argv[0] << std::endl;
	Grafika grafika(width, height, glm::vec3(0, 0, 0), argv[0]);

	//pocetne vrijednosti za provjeru orijentacija
	provjeraOrijentacije[0] = -2;
	provjeraOrijentacije[1] = -2;

	//prosljeduje se pokazivac na funkciju. Ta funkcija ce se pozvati kada korisnik pritisne misem
	grafika.registrirajFunkcijuZaKlikMisa(klikMisa);

	while (grafika.trebaZatvoriti()) {

		//osvjezavanje pozicije razlicitih objekata i fizikalna simulacija bi se izvrsavala u ovom dijelu

		grafika.pobrisiProzor();

		//iscrtavanje uzorka sahovnice
		for (int i = 0; i < height; i += 1)
			for (int j = 0; j < width; j += 1) {
				if ((i + j) % 2 == 0)
					grafika.osvijetliFragment(i, j, glm::vec3(0.1, 0.1, 0.1));
				if (i % 10 == 0 && j % 10 == 0)
					grafika.osvijetliFragment(i, j, glm::vec3(0.15, 0.15, 0.2));
			}


		if (klikovi.size() > 1) {

			if (trebaIscrtatLiniju) {



				iscrtajLiniju(grafika, klikovi[klikovi.size() - 2].first, klikovi[klikovi.size() - 2].second, klikovi[klikovi.size() - 1].first, klikovi[klikovi.size() - 1].second);

				trebaIscrtatLiniju = false;


			}

		}

		if (finalnaLinija) {
			finalnaLinija = false;
			iscrtajLiniju(grafika, klikovi[klikovi.size() - 1].first, klikovi[klikovi.size() - 1].second, klikovi[0].first, klikovi[0].second);
		}


		

		//iscrtavanje linije
		

		for (int i = 0; i < SveLinije.size(); i++) {

			for (int j = 0; j < SveLinije[i].fragmenti.size(); j++) {
				if (poligonIscrtan) {
					grafika.osvijetliFragment(SveLinije[i].fragmenti[j].first, height - SveLinije[i].fragmenti[j].second - 1, glm::vec3(0, 0, 1));
				}
				else {
					grafika.osvijetliFragment(SveLinije[i].fragmenti[j].first, height - SveLinije[i].fragmenti[j].second - 1, glm::vec3(0, 1, 1));
				}
			}

		}

		//iscrtavanje pritisnutih fragmenata
		//ishodiste koordinatnog sustava za operacijski sustav je u gornjem lijevom kutu, a za OpenGL je u donjem lijevom, pa je potrebno okrenuti predznak
		for (int i = 0; i < klikovi.size(); i++)
			grafika.osvijetliFragment(klikovi[i].first, height - klikovi[i].second - 1, glm::vec3(0.6, 0.2, 0));


		//iscrtavanje unutrasnjosti poligona

		for (int i = 0; i < zaObojati.size(); i++)
			grafika.osvijetliFragment(zaObojati[i].first, height - zaObojati[i].second - 1, glm::vec3(0.5, 0.2, 0.7));


		//iscrtavanje testnih tocki i bojanje ovisno o tome jesu li unutra ili van poligona
		for (int i = 0; i < testTocke.size(); i++) {

			bool unutarpoligona = true;

			for (int j = 0; j < SveLinije.size(); j++) {

				int provjera = ispodIznad(SveLinije[j], testTocke[i]);

				if (provjera > 0 && provjeraOrijentacije[0] == -1) {
					unutarpoligona = false;
					break;
				}

				if (provjera < 0 && provjeraOrijentacije[0] == 1) {
					unutarpoligona = false;
					break;
				}

			}

			if (unutarpoligona) {
				grafika.osvijetliFragment(testTocke[i].first, height - testTocke[i].second - 1, glm::vec3(0, 1, 0));
			}
			else {
				grafika.osvijetliFragment(testTocke[i].first, height - testTocke[i].second - 1, glm::vec3(1, 0, 0));
			}

		}


		grafika.iscrtajRaster();

		//kako program ne bi trosio previse resursa, na kraj petlje se moze dodati poziv funkcije za cekanje
		//kao npr Sleep ili od c++11 na dalje this_thread::sleep_for(chrono::milliseconds(16));

		std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}   
	

    return EXIT_SUCCESS;
}
