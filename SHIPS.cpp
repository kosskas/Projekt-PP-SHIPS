#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <time.h> 

using std::cin;
using std::cout;
using std::endl;

///OGOLNE ZDEFINIOWANE STALE I ZASADY
	/// 1 CAR RIER +++++ x1
	/// 2 BAT TLESHIP ++++ x2 
	/// 3 CRU ISER +++ x3 
	/// 4 DES TROYER ++ x4
#define L_RODZ_STATKOW 4
#define MAX_CMD_DL 20
#define MAX_SKROT_DL 4
#define MAX_NR_POJ_KLASY 10
#define MAX_STATEK_LENGHT 5
#define L_RUCHOW 3
#define L_RUCHOW_CAR 2
#define CAR_DL 5
#define BAT_DL 4
#define CRU_DL 3
#define DES_DL 2
#define DOMYSLNY_SEED 0
#define ZASIEG_SAMOLOTU 1.42 ///sqrt(2)
#define ZASIEG_ZNISZCZONEGO_RADARU 1
const char klasaStatku[L_RODZ_STATKOW][MAX_SKROT_DL] = { "CAR", "BAT", "CRU", "DES" };
const int dlStatku[L_RODZ_STATKOW] = { CAR_DL, BAT_DL, CRU_DL, DES_DL };

const char stan_poczDF[L_RODZ_STATKOW][MAX_STATEK_LENGHT + 1] = { "11111" , "1111" , "111" , "11" };
const char modulStatku[L_RODZ_STATKOW][MAX_STATEK_LENGHT + 1] = { "@!11%" , "@!1%" , "@!%" , "@%" };

enum Klasy
{
	CAR,
	BAT,
	CRU,
	DES
};

/// STRUKTURY
struct wspolrzedne {
	int y;
	int x;
};
struct pole {
	wspolrzedne lewa_gorna;
	wspolrzedne prawa_dolna;
};
struct statek {
	wspolrzedne czubek; //to takze radar
	wspolrzedne wsp_armaty;
	int indx_pocz;
	int indx_kon;
	char kierunek;
	int iter;
	bool aktywny;
	bool rozlozony;
	int dlugosc;
	int klasa;
	char stan[MAX_STATEK_LENGHT + 1];
	int id_dzialo;
	int id_silnik;
	int id_radar;
	int liczba_ruchow_na_runde;
	int liczba_strzalow_na_runde;
};
struct gracz {
	statek flota[L_RODZ_STATKOW][MAX_NR_POJ_KLASY];
	int liczba_statkow_na_klase[L_RODZ_STATKOW];
	int liczba_statkow_rozlozonych_na_klase[L_RODZ_STATKOW];
	int licz_zywe;
	pole ogranicz;
	int ilosc_na_start;
	char imie;
	bool bot;
};
struct plansza {
	int wym_Y = 0;
	int wym_X = 0;
	char* OUT = NULL;
	char* POZ = NULL;
	char* PER = NULL;

	char NEXT_PLAYER = 'A';
	bool EXTENDED = false;
	unsigned int seed = DOMYSLNY_SEED;
	/// Bezpieczniki
	bool lock_BOARD_SIZE = false;
	bool lock_INIT_POS = false;
	bool lock_SET_FLEET = false;
};

/// FUNKCJE
/// funkcje glowne
void PLACE_SHIP(gracz* N, plansza* P);
void SHOOT(gracz* N, gracz* M, plansza* P);
void BOARD_SIZE(gracz* N, gracz* M, plansza* P);
void SET_FLEET(gracz* N, gracz* M);
void SHIP(gracz* N, gracz* M, plansza* P);
void INIT_POSITION(gracz* N, gracz* M);
void REEF(plansza* P);
void SAVE(gracz* N, gracz* M, plansza* P);
void MOVE(gracz* N, plansza* P);
void SPY(gracz* N, plansza* P);
void PRINT(const plansza* P);
void STATE(gracz* N, gracz* M, plansza* P, char* polecenie);
void PLAYER(gracz* N, gracz* M, plansza* P, char* polecenie);
void NEXT_PLAYER(gracz* N, gracz* M, plansza* P);
void AI(gracz* N, gracz* M, plansza* P);

///PLACESHIP // SHIP // MOVE
void ustawstatek(gracz* N, plansza* P, int nr_stat, int nr_klasy);
void preShip(gracz* N, plansza* P);
void usunStatek(gracz* N, plansza* P, int nr_stat, int nr_klasy);
bool sprawdzCzyKwadratZawiera(const char* plansza, int wsp, char CO_SPRAWDZIC, int wiersze);
void okreslPozycjeStatku(gracz* N, plansza* P, int nr_stat, int nr_klasy, const char* rodzajRUCHU);
bool sprawdzMozliwoscPostawieniaStatku(gracz* N, plansza* P, int nr_stat, int nr_klasy, const char* rodzajRUCHU);
bool sprawdzCzySasiectwoWolne(const char* plansza, int wsp, int wymX);
int sprawdzNrStatku(const char* typ);
int zliczStatki(const plansza* P, const char CO_ZLICZYC);

///SET_FLEET I POZYCJONOWANIE
void ustawFlote(gracz* N, int a, int b, int c, int d);
void stworzGranice(pole* G, int y1, int x1, int y4, int x4);
bool sprawdzCzyInitPos(pole* G, int y, int x);
void resetujFunkcjeStatku(gracz* N);
void resetujFlote(gracz* N);

///Plansza
char* stworzPlansze(int wymY, int wymX);
void wypelnijPlansze(plansza* P, char* tablica, const char wypelnienie);
void nowaPlansza(plansza* P, int noweY, int noweX);
void odkryjRadar(gracz* N, plansza* P);
void wyczyscPlansze(plansza* P);

///PRINT
void perspectivePrint(const gracz* N, plansza* P);
void wypiszBasic(const char* plansza, const int wymY, const int wymX);
void wypiszExtended(const char* plansza, const int wymY, const int wymX);
void wypiszDev(const char* plansza, const int wymY, const int wymX);

///RUCH
wspolrzedne konwertujNaWspolrzedne(int indeks, int wym_X);
wspolrzedne nowyCzubek(wspolrzedne stary_czubek, int dlugosc, char staryKierunek, char skret);
char nowyKierunek(char staryKierunek, char skret);
bool sprawdzDystans(wspolrzedne A, wspolrzedne B, double odleglosc);

///SHOOT
void extenShoot(gracz* N, gracz* M, plansza* P);
void basicShoot(gracz* N, gracz* M, plansza* P);
void botBasShoot(gracz* N, plansza* P);
void botExShoot(gracz* N, plansza* P);
void sprawdzTrafienie(gracz* N, plansza* P);

///SAVE
bool sprawdzCzyKoniecGry(gracz* N, gracz* M);
void zapiszGracza(gracz* N, plansza* P);
void zapiszStanGry(gracz* N, gracz* M, plansza* P);
void nowaRunda(gracz* N, gracz* M, plansza* P);

///RESZTA
bool sprawdzNapis(const char* zrodlo, const char* klucz);

int main()
{
	plansza Gra;
	gracz A, B;
	A.imie = 'A';
	B.imie = 'B';
	A.bot = false;
	B.bot = false;
	char polecenie[MAX_CMD_DL] = {};

	/// WYMAGANE DO WERSJI BASIC
	int wymY = 21;
	int wymX = 10;
	nowaPlansza(&Gra, wymY, wymX);
	ustawFlote(&A, 1, 2, 3, 4);
	ustawFlote(&B, 1, 2, 3, 4);
	stworzGranice(&A.ogranicz, 0, 0, 9, 9);
	stworzGranice(&B.ogranicz, 11, 0, 20, 9);
	Gra.NEXT_PLAYER = 'A';
	Gra.EXTENDED = false;

	/// GLOWNA PETLA
	while (cin >> polecenie)
	{
		if (sprawdzNapis(polecenie, "[state]"))
		{
			do
			{
				cin >> polecenie;
				STATE(&A, &B, &Gra, polecenie);
			} while (!sprawdzNapis(polecenie, "[state]"));
		}
		if (sprawdzNapis(polecenie, "[playerA]"))
		{
			NEXT_PLAYER(&A, &B, &Gra);
			do {
				cin >> polecenie;
				PLAYER(&A, &B, &Gra, polecenie);
			} while (!sprawdzNapis(polecenie, "[playerA]"));
		}
		if (sprawdzNapis(polecenie, "[playerB]"))
		{
			NEXT_PLAYER(&B, &A, &Gra);
			do {
				cin >> polecenie;
				PLAYER(&B, &A, &Gra, polecenie);
			} while (!sprawdzNapis(polecenie, "[playerB]"));
		}
		nowaRunda(&A, &B, &Gra);
	}
	///BOT
	if (feof(stdin) != 0 && (A.bot || B.bot))
		AI(&A, &B, &Gra);

	wyczyscPlansze(&Gra);
	return EXIT_SUCCESS;
}

void STATE(gracz* N, gracz* M, plansza* P, char* polecenie)
{
	if (sprawdzNapis(polecenie, "INIT_POSITION") && !P->lock_INIT_POS)
	{
		INIT_POSITION(N, M);
	}
	else if (sprawdzNapis(polecenie, "BOARD_SIZE") && !P->lock_BOARD_SIZE)
	{
		BOARD_SIZE(N, M, P);
	}
	else if (sprawdzNapis(polecenie, "SHIP"))
	{
		SHIP(N, M, P);
	}
	else if (sprawdzNapis(polecenie, "SET_FLEET") && !P->lock_SET_FLEET)
	{
		SET_FLEET(N, M);
	}
	else if (sprawdzNapis(polecenie, "PRINT"))
	{
		PRINT(P);
	}
	else if (sprawdzNapis(polecenie, "REEF"))
	{
		REEF(P);
	}
	else if (sprawdzNapis(polecenie, "SAVE"))
	{
		SAVE(N, M, P);
	}
	else if (sprawdzNapis(polecenie, "NEXT_PLAYER"))
	{
		char Z;
		cin >> Z;
		P->NEXT_PLAYER = Z;
	}
	else if (sprawdzNapis(polecenie, "EXTENDED_SHIPS"))
	{
		P->EXTENDED = true;
	}
	else if (sprawdzNapis(polecenie, "SRAND"))
	{
		int n;
		cin >> n;
		P->seed = n;
	}
	else if (sprawdzNapis(polecenie, "SET_AI_PLAYER"))
	{
		char Q;
		cin >> Q;
		if (Q == N->imie)
			N->bot = true;
		else
			M->bot = true;
	}
	else
		return;
}

void PLAYER(gracz* N, gracz* M, plansza* P, char* polecenie)
{
	P->lock_BOARD_SIZE = true;
	P->lock_INIT_POS = true;
	P->lock_SET_FLEET = true;
	if (sprawdzNapis(polecenie, "PLACE_SHIP"))
	{
		PLACE_SHIP(N, P);
	}
	else if (sprawdzNapis(polecenie, "SHOOT"))
	{
		SHOOT(N, M, P);
	}
	else if (sprawdzNapis(polecenie, "MOVE"))
	{
		wypelnijPlansze(P, P->PER, '?');
		MOVE(N, P);
	}
	else if (sprawdzNapis(polecenie, "PRINT"))
	{
		perspectivePrint(N, P);
	}
	else if (sprawdzNapis(polecenie, "SPY"))
	{
		SPY(N, P);
	}
	else
		return;
}

void NEXT_PLAYER(gracz* N, gracz* M, plansza* P)
{
	if (P->NEXT_PLAYER != N->imie)
	{
		cout << "INVALID OPERATION " << '"' << "[player" << N->imie << "] " << '"' << ": THE OTHER PLAYER EXPECTED" << endl;
		wyczyscPlansze(P);
		exit(EXIT_FAILURE);
	}
	P->NEXT_PLAYER = M->imie;
}

void nowaRunda(gracz* N, gracz* M, plansza* P)
{
	resetujFunkcjeStatku(N);
	resetujFunkcjeStatku(M);
	wypelnijPlansze(P, P->PER, '?');
	sprawdzTrafienie(N, P);
	sprawdzTrafienie(M, P);

	if (sprawdzCzyKoniecGry(N, M))
	{
		wyczyscPlansze(P);
		exit(EXIT_SUCCESS);
	}
}

wspolrzedne konwertujNaWspolrzedne(int indeks, int wym_X)
{
	wspolrzedne pomoc;
	pomoc.x = indeks % wym_X;
	pomoc.y = (indeks - pomoc.x) / wym_X;
	return pomoc;
}

bool sprawdzNapis(const char* zrodlo, const char* klucz)
{
	if (*zrodlo == '\0')
		return false;
	while (*zrodlo != '\0')
	{
		if (*(zrodlo++) != *(klucz++))
			return false;
	}
	return true;
}

void PRINT(const plansza* P)
{
	int tryb;
	cin >> tryb;
	if (tryb == 0) ///BASIC
	{
		wypiszBasic(P->OUT, P->wym_Y, P->wym_X);
	}
	if (tryb == 1) /// EXTENDED
	{
		wypiszExtended(P->OUT, P->wym_Y, P->wym_X);
	}
	if (tryb == 2) ///DEV 
	{
		wypiszDev(P->OUT, P->wym_Y, P->wym_X);
	}
	if (tryb == 3) /// DEV
	{
		wypiszDev(P->POZ, P->wym_Y, P->wym_X);
	}
	cout << "PARTS REMAINING:: A : " << zliczStatki(P, 'A') << " B : " << zliczStatki(P, 'B') << endl;
}

void wypiszBasic(const char* plansza, const int wymY, const int wymX)
{
	for (int y = 0; y < wymY; y++)
	{
		for (int x = 0; x < wymX; x++)
		{
			if (plansza[y * wymX + x] == '.')
				cout << " ";
			else if (plansza[y * wymX + x] == 'x')
				cout << 'x';
			else if (plansza[y * wymX + x] == '#')
				cout << '#';
			else if (plansza[y * wymX + x] == '?')
				cout << '?';
			else
				cout << '+';
		}
		cout << endl;
	}
}

void wypiszExtended(const char* plansza, const int wymY, const int wymX)
{
	cout << "  ";
	for (int x = 0; x < wymX; x++)
	{
		cout << x;
	}
	cout << endl;
	for (int y = 0; y < wymY; y++)
	{
		if (y < 10)
			cout << '0' << y;
		else
			cout << y;
		for (int x = 0; x < wymX; x++)
		{
			if (plansza[y * wymX + x] == '.')
				cout << " ";
			else if (plansza[y * wymX + x] == 'x')
				cout << 'x';
			else if (plansza[y * wymX + x] == '1')
				cout << '+';
			else
				cout << plansza[y * wymX + x];

		}
		cout << endl;
	}
}

void wypiszDev(const char* plansza, const int wymY, const int wymX)
{
	for (int y = 0; y < wymY; y++)
	{
		for (int x = 0; x < wymX; x++)
		{
			cout << plansza[y * wymX + x] << " ";
		}
		cout << endl;
	}
}

void odkryjRadar(const gracz* N, plansza* P)
{
	for (int nr_klasy = 0; nr_klasy < L_RODZ_STATKOW; nr_klasy++)
	{
		for (int nr_statku = 0; nr_statku < N->liczba_statkow_na_klase[nr_klasy]; nr_statku++)
		{
			if (N->flota[nr_klasy][nr_statku].rozlozony)
			{
				///Of course, a ship knows it's position always.
				int poc = N->flota[nr_klasy][nr_statku].indx_pocz;
				int kon = N->flota[nr_klasy][nr_statku].indx_kon;
				int skok = N->flota[nr_klasy][nr_statku].iter;
				for (int i = poc; i != kon + skok; i += skok)
				{
					P->PER[i] = P->OUT[i];
				}
				///Radar
				for (int i = 0; i < P->wym_Y * P->wym_X; i++)
				{
					wspolrzedne puknt = konwertujNaWspolrzedne(i, P->wym_X);
					if (P->POZ[N->flota[nr_klasy][nr_statku].id_radar] == 'x')
					{
						if (sprawdzDystans(N->flota[nr_klasy][nr_statku].czubek, puknt, ZASIEG_ZNISZCZONEGO_RADARU))
							P->PER[i] = P->OUT[i];
					}
					else if (sprawdzDystans(N->flota[nr_klasy][nr_statku].czubek, puknt, N->flota[nr_klasy][nr_statku].dlugosc))
						P->PER[i] = P->OUT[i];
				}
			}
		}
	}
}

void perspectivePrint(const gracz* N, plansza* P)
{
	int tryb;
	cin >> tryb;

	odkryjRadar(N, P);
	//ship zna swoja pozycje
	if (tryb == 0)
	{
		wypiszBasic(P->PER, P->wym_Y, P->wym_X);
	}
	else if (tryb == 1)
	{
		wypiszExtended(P->PER, P->wym_Y, P->wym_X);
	}
	else
	{
		wypiszDev(P->PER, P->wym_Y, P->wym_X);
	}
}

void PLACE_SHIP(gracz* N, plansza* P)
{
	int y, x, i;
	char C[MAX_SKROT_DL] = {};
	char D;
	cin >> y >> x >> D >> i >> C;

	int indeks_klasy = sprawdzNrStatku(C); /// KONWERSJA STRING NA INT
	if (N->flota[indeks_klasy][i].rozlozony)
	{
		cout << "INVALID OPERATION " << '"' << "PLACE_SHIP " << y << " " << x << " " << D << " " << i << " " << C << '"' << ": SHIP ALREADY PRESENT" << endl;
		wyczyscPlansze(P);
		exit(EXIT_FAILURE);
	}
	if (N->liczba_statkow_rozlozonych_na_klase[indeks_klasy] == 0 || !N->flota[indeks_klasy][i].aktywny)
	{
		cout << "INVALID OPERATION " << '"' << "PLACE_SHIP " << y << " " << x << " " << D << " " << i << " " << C << '"' << ": ALL SHIPS OF THE CLASS ALREADY SET" << endl;
		wyczyscPlansze(P);
		exit(EXIT_FAILURE);
	}

	N->flota[indeks_klasy][i].czubek.y = y;
	N->flota[indeks_klasy][i].czubek.x = x;
	N->flota[indeks_klasy][i].kierunek = D;
	strcpy(N->flota[indeks_klasy][i].stan, stan_poczDF[indeks_klasy]); ///STAN POCZATKOWY STAKU CZYLI CALY

	if (sprawdzMozliwoscPostawieniaStatku(N, P, i, indeks_klasy, "PLACE_SHIP"))
	{
		ustawstatek(N, P, i, indeks_klasy);
		N->flota[indeks_klasy][i].rozlozony = true;
		N->liczba_statkow_rozlozonych_na_klase[indeks_klasy]--;
		N->ilosc_na_start--;
	}
	else
	{
		cout << "INVALID OPERATION " << '"' << "PLACE_SHIP " << y << " " << x << " " << D << " " << i << " " << C << '"' << ": SHIP ALREADY PRESENT" << endl;
		wyczyscPlansze(P);
		exit(EXIT_FAILURE);
	}
}

void SHIP(gracz* N, gracz* M, plansza* P)
{
	char Kto;
	cin >> Kto;

	if (Kto == N->imie)
	{
		preShip(N, P);
	}
	else
	{
		preShip(M, P);
	}
}

void okreslPozycjeStatku(gracz* N, plansza* P, int nr_stat, int nr_klasy, const char* rodzajRUCHU)
{
	int y = N->flota[nr_klasy][nr_stat].czubek.y;
	int x = N->flota[nr_klasy][nr_stat].czubek.x;
	char kierunek = N->flota[nr_klasy][nr_stat].kierunek;
	int dlugosc = N->flota[nr_klasy][nr_stat].dlugosc;

	int poc;
	int kon;
	int skok;
	bool NOT_IN_STARTING_POS = false;
	/// 4 ROZNE PRZYPADKI POLOZENIA CZUBKA STATKU I JEGO KONCA
	if (kierunek == 'N') //tyl sk. w dol /+ dl/ //OGRANICZENIA!
	{
		poc = y * P->wym_X + x;
		kon = y * P->wym_X + (dlugosc - 1) * P->wym_X + x;
		skok = P->wym_X;
		if ((!sprawdzCzyInitPos(&N->ogranicz, y, x) || !sprawdzCzyInitPos(&N->ogranicz, y + dlugosc - 1, x)) && (sprawdzNapis(rodzajRUCHU, "PLACE_SHIP")))
			NOT_IN_STARTING_POS = true;
	}
	else if (kierunek == 'S') ///tyl sk w gore /- dl/
	{
		poc = y * P->wym_X + x;
		kon = y * P->wym_X - (dlugosc - 1) * P->wym_X + x;
		skok = -P->wym_X;
		if ((!sprawdzCzyInitPos(&N->ogranicz, y, x) || !sprawdzCzyInitPos(&N->ogranicz, y - dlugosc + 1, x)) && (sprawdzNapis(rodzajRUCHU, "PLACE_SHIP")))
			NOT_IN_STARTING_POS = true;
	}
	else if (kierunek == 'E') //tyl sk. w prawo /+ dl/
	{
		poc = y * P->wym_X + x;
		kon = y * P->wym_X + x - dlugosc + 1;
		skok = -1;
		if ((!sprawdzCzyInitPos(&N->ogranicz, y, x) || !sprawdzCzyInitPos(&N->ogranicz, y, x - dlugosc + 1)) && (sprawdzNapis(rodzajRUCHU, "PLACE_SHIP")))
			NOT_IN_STARTING_POS = true;
	}
	else if (kierunek == 'W') //tyl sk. w lewo /-dlugossc/
	{
		poc = y * P->wym_X + x;
		kon = y * P->wym_X + x + dlugosc - 1;
		skok = 1;
		if ((!sprawdzCzyInitPos(&N->ogranicz, y, x) || !sprawdzCzyInitPos(&N->ogranicz, y, x + dlugosc - 1)) && (sprawdzNapis(rodzajRUCHU, "PLACE_SHIP")))
			NOT_IN_STARTING_POS = true;
	}
	else
	{
		cout << "NIEZNANY KIERUNEK" << endl;
		wyczyscPlansze(P);
		exit(EXIT_FAILURE);
	}
	if (NOT_IN_STARTING_POS)
	{
		cout << "INVALID OPERATION " << '"' << rodzajRUCHU << " " << y << " " << x << " " << kierunek << " " << nr_stat << " " << klasaStatku[nr_klasy] << '"' << ": NOT IN STARTING POSITION" << endl;
		wyczyscPlansze(P);
		exit(EXIT_FAILURE);
	}
	/// PRZYPADKI WYKLUCZAJACE RUCH
	if (poc < 0 || kon < 0 || poc > P->wym_X * P->wym_Y || kon > P->wym_X * P->wym_Y || (poc > kon && poc % P->wym_X < kon % P->wym_X) || (poc < kon && poc % P->wym_X > kon % P->wym_X))
	{
		cout << "INVALID OPERATION " << '"' << rodzajRUCHU << kierunek << " " << nr_stat << " " << klasaStatku[nr_klasy] << '"' << ": SHIP WENT FROM BOARD";
		wyczyscPlansze(P);
		exit(EXIT_FAILURE);
	}
	N->flota[nr_klasy][nr_stat].indx_pocz = poc;
	N->flota[nr_klasy][nr_stat].indx_kon = kon;
	N->flota[nr_klasy][nr_stat].iter = skok;
}

bool sprawdzMozliwoscPostawieniaStatku(gracz* N, plansza* P, int nr_stat, int nr_klasy, const char* rodzajRUCHU)
{
	okreslPozycjeStatku(N, P, nr_stat, nr_klasy, rodzajRUCHU);

	int y = N->flota[nr_klasy][nr_stat].czubek.y;
	int x = N->flota[nr_klasy][nr_stat].czubek.x;
	char kierunek = N->flota[nr_klasy][nr_stat].kierunek;
	int poc = N->flota[nr_klasy][nr_stat].indx_pocz;
	int kon = N->flota[nr_klasy][nr_stat].indx_kon;
	int skok = N->flota[nr_klasy][nr_stat].iter;

	/// SPRAWDZENIE CZY SASIECTWO WOLNE I CZY NIE MA NICZEGO NA DRODZE STATKU
	for (int i = poc; i != kon + skok; i += skok)
	{
		if (!sprawdzCzySasiectwoWolne(P->POZ, i, P->wym_X))
		{
			cout << "INVALID OPERATION " << '"' << rodzajRUCHU << " " << y << " " << x << " " << kierunek << " " << nr_stat << " " << klasaStatku[nr_klasy] << '"' << ": PLACING SHIP TOO CLOSE TO OTHER SHIP" << endl;
			wyczyscPlansze(P);
			exit(EXIT_FAILURE);
		}
		if (P->POZ[i] == '#')
		{
			cout << "INVALID OPERATION " << '"' << rodzajRUCHU << " " << y << " " << x << " " << kierunek << " " << nr_stat << " " << klasaStatku[nr_klasy] << '"' << ": PLACING SHIP ON REEF" << endl;
			wyczyscPlansze(P);
			exit(EXIT_FAILURE);
		}
	}
	return true;
}

bool sprawdzCzyKwadratZawiera(const char* plansza, int wsp, char CO_SPRAWDZIC, int wiersze)
{
	if (wsp % wiersze == 0) ///PRAWY WYKLUCZA WSZYSTKIE W
	{
		if (plansza[wsp] == CO_SPRAWDZIC //M
			|| plansza[wsp + 1] == CO_SPRAWDZIC //E 
			|| plansza[wsp - wiersze] == CO_SPRAWDZIC //N
			|| plansza[wsp + wiersze] == CO_SPRAWDZIC //S
			|| plansza[wsp - wiersze + 1] == CO_SPRAWDZIC //NE
			|| plansza[wsp + wiersze + 1] == CO_SPRAWDZIC //SE
			)
			return true;
		else
			return false;
	}
	else if (wsp % wiersze == wiersze - 1) //SPRAWDZIC LWEY BOK BO CZESC NALEZY WYKLUCZYC WSZYSTKIE E
	{
		if (plansza[wsp] == CO_SPRAWDZIC //M
			|| plansza[wsp - 1] == CO_SPRAWDZIC //W
			|| plansza[wsp - wiersze] == CO_SPRAWDZIC //N
			|| plansza[wsp + wiersze] == CO_SPRAWDZIC //S
			|| plansza[wsp - wiersze - 1] == CO_SPRAWDZIC //NW
			|| plansza[wsp + wiersze - 1] == CO_SPRAWDZIC //SW
			)
			return true;
		else
			return false;
	}
	else if (plansza[wsp] == CO_SPRAWDZIC //M
		|| plansza[wsp + 1] == CO_SPRAWDZIC //E 
		|| plansza[wsp - 1] == CO_SPRAWDZIC //W
		|| plansza[wsp - wiersze] == CO_SPRAWDZIC //N
		|| plansza[wsp + wiersze] == CO_SPRAWDZIC //S
		|| plansza[wsp - wiersze - 1] == CO_SPRAWDZIC //NW
		|| plansza[wsp - wiersze + 1] == CO_SPRAWDZIC //NE
		|| plansza[wsp + wiersze - 1] == CO_SPRAWDZIC //SW
		|| plansza[wsp + wiersze + 1] == CO_SPRAWDZIC //SE
		)
		return true;
	else
		return false;
}

bool sprawdzCzySasiectwoWolne(const char* plansza, int wsp, int wymX)
{
	if (sprawdzCzyKwadratZawiera(plansza, wsp, 'A', wymX))
		return false;
	if (sprawdzCzyKwadratZawiera(plansza, wsp, 'B', wymX))
		return false;
	if (sprawdzCzyKwadratZawiera(plansza, wsp, 'x', wymX))
		return false;
	return true;
}

int sprawdzNrStatku(const char* typ)
{
	for (int i = 0; i < L_RODZ_STATKOW; i++)
	{
		if (sprawdzNapis(typ, klasaStatku[i]))
			return i;
	}
	cout << "BLAD W F. sprawdzNRSTATKU" << endl;
	return -1;
}

void ustawstatek(gracz* N, plansza* P, int nr_stat, int nr_klasy)
{
	int skok = N->flota[nr_klasy][nr_stat].iter;
	int poc = N->flota[nr_klasy][nr_stat].indx_pocz;
	int kon = N->flota[nr_klasy][nr_stat].indx_kon;
	char ID_graczA = N->imie;
	int iterator = 0;

	///WLASCIWE USTAWIANIE STATKU
	for (int i = poc; i != kon + skok; i += skok) ///poc moze byc > kon
	{
		P->POZ[i] = ID_graczA;
		if (N->flota[nr_klasy][nr_stat].stan[iterator] == '0')
		{
			P->OUT[i] = 'x';
			P->POZ[i] = 'x';
		}
		else
			P->OUT[i] = modulStatku[N->flota[nr_klasy][nr_stat].klasa][iterator];
		iterator++;
	}
	/// ZAPISYWANIE POLOZENIA CZESCI STATKU
	N->flota[nr_klasy][nr_stat].id_radar = poc;
	N->flota[nr_klasy][nr_stat].id_dzialo = poc + skok;
	N->flota[nr_klasy][nr_stat].id_silnik = kon;
	N->flota[nr_klasy][nr_stat].wsp_armaty = konwertujNaWspolrzedne(N->flota[nr_klasy][nr_stat].id_dzialo, P->wym_X);
}

void preShip(gracz* N, plansza* P)
{
	char D;
	int y, x, i;
	char C[MAX_SKROT_DL] = {};
	char Stan_Nowy[MAX_STATEK_LENGHT + 1];
	cin >> y >> x >> D >> i >> C >> Stan_Nowy;

	int indeks_klasy = sprawdzNrStatku(C);

	if (N->flota[indeks_klasy][i].rozlozony)
	{
		cout << "INVALID OPERATION " << '"' << "SHIP " << y << " " << x << " " << D << " " << i << " " << C << '"' << ": SHIP ALREADY PRESENT" << endl;
		wyczyscPlansze(P);
		exit(EXIT_FAILURE);
	}
	if (N->liczba_statkow_rozlozonych_na_klase[indeks_klasy] == 0 || !N->flota[indeks_klasy][i].aktywny)
	{
		cout << "INVALID OPERATION " << '"' << "SHIP " << y << " " << x << " " << D << " " << i << " " << C << '"' << ": ALL SHIPS OF THE CLASS ALREADY SET" << endl;
		wyczyscPlansze(P);
		exit(EXIT_FAILURE);
	}

	N->flota[indeks_klasy][i].czubek.y = y;
	N->flota[indeks_klasy][i].czubek.x = x;
	N->flota[indeks_klasy][i].kierunek = D;

	///ZMIANA DOMYSLNEGO STANU STATKU NA PODANY
	strcpy(N->flota[indeks_klasy][i].stan, Stan_Nowy);

	int iterator = 0;
	while (Stan_Nowy[iterator] != '\0')
	{
		if (Stan_Nowy[iterator] == '0')
			N->licz_zywe--;
		iterator++;
	}

	if (sprawdzMozliwoscPostawieniaStatku(N, P, i, indeks_klasy, "SHIP"))
	{
		ustawstatek(N, P, i, indeks_klasy);
		N->flota[indeks_klasy][i].rozlozony = true;
		N->liczba_statkow_rozlozonych_na_klase[indeks_klasy]--;
		N->ilosc_na_start--;
	}
}

void SHOOT(gracz* N, gracz* M, plansza* P)
{
	if (P->EXTENDED)
		extenShoot(N, M, P);
	else
		basicShoot(N, M, P);

	sprawdzTrafienie(N, P);
	sprawdzTrafienie(M, P);
}

void basicShoot(gracz* N, gracz* M, plansza* P)
{
	int y, x;
	cin >> y >> x;
	if (y * P->wym_X + x > P->wym_Y * P->wym_X)
	{
		cout << "INVALID OPERATION " << '"' << "SHOOT " << y << " " << x << '"' << ": FIELD DOES NOT EXIST";
		wyczyscPlansze(P);
		exit(EXIT_FAILURE);
	}
	if (N->ilosc_na_start != 0 || M->ilosc_na_start != 0)
	{
		cout << "INVALID OPERATION " << '"' << "SHOOT " << y << " " << x << '"' << ": NOT ALL SHIPS PLACED";
		wyczyscPlansze(P);
		exit(EXIT_FAILURE);
	}
	///STRZELANIE W WODE, RAFE LUB W ZNISZCZONA CZESC SIE NIE LICZY
	if (P->OUT[y * P->wym_X + x] != '.' && P->OUT[y * P->wym_X + x] != 'x' && P->OUT[y * P->wym_X + x] != '#')
	{
		P->OUT[y * P->wym_X + x] = 'x';
		if (P->POZ[y * P->wym_X + x] == N->imie) ///dzialanie jesli strzela w siebie
		{
			P->POZ[y * P->wym_X + x] = 'x';
			///ZMIANA WARUNKU KONCA GRY
			N->licz_zywe--;
		}
		else
		{
			P->POZ[y * P->wym_X + x] = 'x';
			M->licz_zywe--;
		}
	}
}

void extenShoot(gracz* N, gracz* M, plansza* P) //w zal od N
{
	int nr_statku;
	char C[MAX_SKROT_DL] = {};
	int y, x;
	cin >> nr_statku >> C >> y >> x;
	int indeks_klasy = sprawdzNrStatku(C);

	if (N->flota[indeks_klasy][nr_statku].liczba_strzalow_na_runde == 0)
	{
		cout << "INVALID OPERATION " << '"' << "SHOOT " << nr_statku << " " << C << " " << y << " " << x << '"' << ": TOO MANY SHOOTS";
		wyczyscPlansze(P);
		exit(EXIT_FAILURE);
	}
	if (y * P->wym_X + x > P->wym_Y * P->wym_X)
	{
		cout << "INVALID OPERATION " << '"' << "SHOOT " << nr_statku << " " << C << " " << y << " " << x << '"' << ": FIELD DOES NOT EXIST";
		wyczyscPlansze(P);
		exit(EXIT_FAILURE);
	}
	if (N->ilosc_na_start != 0 || M->ilosc_na_start != 0)
	{
		cout << "INVALID OPERATION " << '"' << "SHOOT " << nr_statku << " " << C << " " << y << " " << x << '"' << ": NOT ALL SHIPS PLACED";
		wyczyscPlansze(P);
		exit(EXIT_FAILURE);
	}
	if (P->POZ[N->flota[indeks_klasy][nr_statku].id_dzialo] == 'x')
	{
		cout << "INVALID OPERATION " << '"' << "SHOOT " << nr_statku << " " << C << " " << y << " " << x << '"' << ": SHIP CANNOT SHOOT";
		wyczyscPlansze(P);
		exit(EXIT_FAILURE);
	}

	wspolrzedne punkt;
	punkt.y = y;
	punkt.x = x;

	///ROZSZERZENIE O ODLEGLOSC STRZALU
	if (!sprawdzNapis(C, "CAR"))
	{
		if (!sprawdzDystans(N->flota[indeks_klasy][nr_statku].wsp_armaty, punkt, N->flota[indeks_klasy][nr_statku].dlugosc))
		{
			cout << "INVALID OPERATION " << '"' << "SHOOT " << nr_statku << " " << C << " " << y << " " << x << '"' << ": SHOOTING TOO FAR";
			wyczyscPlansze(P);
			exit(EXIT_FAILURE);
		}
	}

	N->flota[indeks_klasy][nr_statku].liczba_strzalow_na_runde--;

	if (P->OUT[y * P->wym_X + x] != '.' && P->OUT[y * P->wym_X + x] != 'x')
	{
		P->OUT[y * P->wym_X + x] = 'x';
		if (P->POZ[y * P->wym_X + x] == N->imie)
		{
			P->POZ[y * P->wym_X + x] = 'x';
			N->licz_zywe--;
		}
		else
		{
			P->POZ[y * P->wym_X + x] = 'x';
			M->licz_zywe--;
		}
	}
}

void resetujFlote(gracz* N)
{
	///OGOLNA DEFINICJA KLASY STATKU
	for (int nr_klasy = 0; nr_klasy < L_RODZ_STATKOW; nr_klasy++)
	{
		for (int nr_statku = 0; nr_statku < MAX_NR_POJ_KLASY; nr_statku++)
		{
			N->flota[nr_klasy][nr_statku].aktywny = false;
			N->flota[nr_klasy][nr_statku].rozlozony = false;
			N->flota[nr_klasy][nr_statku].klasa = nr_klasy;
			N->flota[nr_klasy][nr_statku].dlugosc = dlStatku[nr_klasy];

			N->flota[nr_klasy][nr_statku].liczba_strzalow_na_runde = N->flota[nr_klasy][nr_statku].dlugosc;

			N->flota[nr_klasy][nr_statku].liczba_ruchow_na_runde = L_RUCHOW;
			if (nr_klasy == CAR)
				N->flota[nr_klasy][nr_statku].liczba_ruchow_na_runde = L_RUCHOW_CAR;
		}
	}
	/// RESET ILOSCI ZYC DLA GRACZA
	N->licz_zywe = 0;
	N->ilosc_na_start = 0;
}

void ustawFlote(gracz* N, int a, int b, int c, int d) ///1for w funckje
{
	resetujFlote(N);
	/// ZMIANA NA ILOSC PODANA

	N->liczba_statkow_na_klase[CAR] = a;
	N->liczba_statkow_na_klase[BAT] = b;
	N->liczba_statkow_na_klase[CRU] = c;
	N->liczba_statkow_na_klase[DES] = d;

	for (int nr_klasy = 0; nr_klasy < L_RODZ_STATKOW; nr_klasy++)
	{
		N->liczba_statkow_rozlozonych_na_klase[nr_klasy] = N->liczba_statkow_na_klase[nr_klasy];
		N->licz_zywe += N->liczba_statkow_na_klase[nr_klasy] * dlStatku[nr_klasy];
		N->ilosc_na_start += N->liczba_statkow_na_klase[nr_klasy];
		for (int nr_statku = 0; nr_statku < N->liczba_statkow_na_klase[nr_klasy]; nr_statku++)
		{
			N->flota[nr_klasy][nr_statku].aktywny = true;
		}
	}
}

bool sprawdzCzyKoniecGry(gracz* N, gracz* M)
{
	if (N->licz_zywe == 0)
	{
		cout << M->imie << " won";
		return true;
	}
	else if (M->licz_zywe == 0)
	{
		cout << N->imie << " won";
		return true;
	}
	else
		return false;
}

void wypelnijPlansze(plansza* P, char* tablica, const char wypelnienie)
{
	for (int y = 0; y < P->wym_Y; y++)
	{
		for (int x = 0; x < P->wym_X; x++)
		{
			if (tablica[y * P->wym_X + x] == '#')
				tablica[y * P->wym_X + x] = '#';
			else
				tablica[y * P->wym_X + x] = wypelnienie;

		}
	}
}

char* stworzPlansze(int wymY, int wymX)
{
	///DYNAMICZNE TWORZENIE PLANSZY
	int NOWY_ROZMIAR = wymY * wymX;
	return new char[NOWY_ROZMIAR];
}

void nowaPlansza(plansza* P, int noweY, int noweX)
{
	///USUWA STARA PLANSZE I TWORZY NOWA O NOWYCH WYMIARACH
	wyczyscPlansze(P);

	P->wym_Y = noweY;
	P->wym_X = noweX;

	P->OUT = stworzPlansze(noweY, noweX);
	P->POZ = stworzPlansze(noweY, noweX);
	P->PER = stworzPlansze(noweY, noweX);

	wypelnijPlansze(P, P->OUT, '.');
	wypelnijPlansze(P, P->POZ, '.');
	wypelnijPlansze(P, P->PER, '?');

}

bool sprawdzCzyInitPos(pole* G, int y, int x)
{
	///OGRANICZENIE DO INIT_POS
	if (y >= G->lewa_gorna.y &&
		y <= G->prawa_dolna.y &&
		x >= G->lewa_gorna.x &&
		x <= G->prawa_dolna.x)
		return true;
	else
		return false;
}

void stworzGranice(pole* G, int y1, int x1, int y4, int x4)
{
	//TWORZY OGRANICZENIE DO STAWIANIA STATKOW
	G->lewa_gorna.y = y1;
	G->lewa_gorna.x = x1;
	G->prawa_dolna.y = y4;
	G->prawa_dolna.x = x4;
}

int zliczStatki(const plansza* P, const char CO_ZLICZYC)
{
	int zlicz = 0;
	for (int i = 0; i < P->wym_Y * P->wym_X; i++)
	{
		if (P->POZ[i] == CO_ZLICZYC)
			zlicz++;
	}
	return zlicz;
}

void wyczyscPlansze(plansza* P)
{
	delete[] P->OUT;
	delete[] P->POZ;
	delete[] P->PER;
	P->OUT = NULL;
	P->POZ = NULL;
	P->PER = NULL;
}

void INIT_POSITION(gracz* N, gracz* M)
{
	char Kto;
	int y1, x1, y2, x2;
	cin >> Kto >> y1 >> x1 >> y2 >> x2;
	if (Kto == N->imie)
		stworzGranice(&N->ogranicz, y1, x1, y2, x2);
	else
		stworzGranice(&M->ogranicz, y1, x1, y2, x2);
}

void BOARD_SIZE(gracz* N, gracz* M, plansza* P)
{
	int noweY, nowyX;
	cin >> noweY >> nowyX;
	nowaPlansza(P, noweY, nowyX);
	///Przyjmuje ze na nowej planszy gracze moga klasc statki gdziekolwiek dopoki nie zostanie to zmienione
	stworzGranice(&N->ogranicz, 0, 0, noweY - 1, nowyX - 1);
	stworzGranice(&M->ogranicz, 0, 0, noweY - 1, nowyX - 1);
}

void SET_FLEET(gracz* N, gracz* M)
{
	char Kto;
	int a, b, c, d;
	cin >> Kto >> a >> b >> c >> d;
	if (Kto == N->imie)
		ustawFlote(N, a, b, c, d);
	else
		ustawFlote(M, a, b, c, d);
}

void REEF(plansza* P)
{
	int y, x;
	cin >> y >> x;

	if (y > P->wym_Y || x > P->wym_X)
	{
		cout << "INVALID OPERATION: REEF IS NOT PLACED ON BOARD";
		wyczyscPlansze(P);
		exit(EXIT_FAILURE);
	}
	// RAFE MOZNA KLASC TYLKO JESLI NA WSPOLRZEDNYCH NIC NIE MA
	if (P->POZ[y * P->wym_X + x] == '.')
	{
		P->OUT[y * P->wym_X + x] = '#';
		P->POZ[y * P->wym_X + x] = '#';
		P->PER[y * P->wym_X + x] = '#';
	}
}

void zapiszStanGry(gracz* N, gracz* M, plansza* P)
{
	cout << "[state]" << endl;
	cout << "BOARD_SIZE " << P->wym_Y << " " << P->wym_X << endl;
	for (int y = 0; y < P->wym_Y; y++)
	{
		for (int x = 0; x < P->wym_X; x++)
		{
			if (P->POZ[y * P->wym_X + x] == '#')
				cout << "REEF " << y << " " << x << endl;
		}
	}
	cout << "NEXT_PLAYER " << P->NEXT_PLAYER << endl;
	if (P->EXTENDED)
		cout << "EXTENDED_SHIPS" << endl;
	if (P->seed != DOMYSLNY_SEED)
		cout << "SRAND " << P->seed + 1 << endl;
	zapiszGracza(N, P);
	zapiszGracza(M, P);
	cout << "[state]" << endl;
}

void zapiszGracza(gracz* N, plansza* P)
{
	///INIT POSITION
	cout << "INIT_POSITION " << N->imie << " ";
	cout << N->ogranicz.lewa_gorna.y << " " << N->ogranicz.lewa_gorna.x << " "
		<< N->ogranicz.prawa_dolna.y << " " << N->ogranicz.prawa_dolna.x << endl;

	///SET_FLEET
	cout << "SET_FLEET " << N->imie << " ";
	for (int i = 0; i < L_RODZ_STATKOW; i++)
		cout << N->liczba_statkow_na_klase[i] << " ";
	cout << endl;
	/// SHIP
	for (int nr_klasy = 0; nr_klasy < L_RODZ_STATKOW; nr_klasy++)
	{
		for (int nr_statku = 0; nr_statku < N->liczba_statkow_na_klase[nr_klasy]; nr_statku++)
		{
			if (N->flota[nr_klasy][nr_statku].rozlozony)
			{
				cout << "SHIP " << N->imie << " " << N->flota[nr_klasy][nr_statku].czubek.y << " " << N->flota[nr_klasy][nr_statku].czubek.x << " "
					<< N->flota[nr_klasy][nr_statku].kierunek << " "
					<< nr_statku << " " << klasaStatku[nr_klasy] << " ";

				int poc = N->flota[nr_klasy][nr_statku].indx_pocz;
				int kon = N->flota[nr_klasy][nr_statku].indx_kon;
				int skok = N->flota[nr_klasy][nr_statku].iter;
				for (int i = poc; i != kon + skok; i += skok) /// dopoki poc != kon, +skok
				{
					cout << (P->POZ[i] == 'x' ? '0' : '1');
				}
				cout << endl;
			}
		}
	}
}

void SAVE(gracz* N, gracz* M, plansza* P)
{
	zapiszStanGry(N, M, P);
	wyczyscPlansze(P);
	exit(EXIT_SUCCESS);
}

char nowyKierunek(char staryKierunek, char skret)
{
	///NOWY KIERUNEK W ZALEZNOSCI OD SKRETU i PERSPEKTYWY STATKU
	if (staryKierunek == 'N')
	{
		if (skret == 'F')
			return 'N';
		if (skret == 'L')
			return 'W';
		if (skret == 'R')
			return 'E';
	}
	else if (staryKierunek == 'S')
	{
		if (skret == 'F')
			return 'S';
		if (skret == 'L')
			return 'E';
		if (skret == 'R')
			return 'W';
	}
	else if (staryKierunek == 'E')
	{
		if (skret == 'F')
			return 'E';
		if (skret == 'L')
			return 'N';
		if (skret == 'R')
			return 'S';
	}
	else // (staryKierunek == 'W')
	{
		if (skret == 'F')
			return 'W';
		if (skret == 'L')
			return 'S';
		if (skret == 'R')
			return 'N';
	}
	return 'N';
}

wspolrzedne nowyCzubek(wspolrzedne stary_czubek, int dlugosc, char staryKierunek, char skret)
{
	wspolrzedne nowe = stary_czubek;
	///OKRESLENIE NOWYCH WSPOLRZEDNYCH W ZALEZNOSCI OD STARYCH, ZASAD RUCHU I SKRETU
	if (staryKierunek == 'N')
	{
		if (skret == 'F')
		{
			nowe.y--;
		}
		if (skret == 'L')
		{
			nowe.y--;
			nowe.x = nowe.x - dlugosc + 1;
		}
		if (skret == 'R')
		{
			nowe.y--;
			nowe.x = nowe.x + dlugosc - 1;
		}
	}
	else if (staryKierunek == 'S')
	{
		if (skret == 'F')
		{
			nowe.y++;
		}
		if (skret == 'R')
		{
			nowe.y++;
			nowe.x = nowe.x - dlugosc + 1;
		}
		if (skret == 'L')
		{
			nowe.y++;
			nowe.x = nowe.x + dlugosc - 1;
		}
	}
	else if (staryKierunek == 'E')
	{
		if (skret == 'F')
		{
			nowe.x++;
		}
		if (skret == 'R')
		{
			nowe.x++;
			nowe.y = nowe.y + dlugosc - 1;
		}
		if (skret == 'L')
		{
			nowe.x++;
			nowe.y = nowe.y - dlugosc + 1;
		}
	}
	else if (staryKierunek == 'W')
	{
		if (skret == 'F')
		{
			nowe.x--;
		}
		else if (skret == 'L')
		{
			nowe.x--;
			nowe.y = nowe.y + dlugosc - 1;
		}
		else if (skret == 'R')
		{
			nowe.x--;
			nowe.y = nowe.y - dlugosc + 1;
		}
	}
	return nowe;
}

void usunStatek(gracz* N, plansza* P, int nr_stat, int nr_klasy)
{
	///USUWA statek NA POTRZEBY RUCHU
	int skok = N->flota[nr_klasy][nr_stat].iter;
	int poc = N->flota[nr_klasy][nr_stat].indx_pocz;
	int kon = N->flota[nr_klasy][nr_stat].indx_kon;

	for (int i = poc; i != kon + skok; i += skok)
	{
		P->OUT[i] = '.';
		P->POZ[i] = '.';
	}
}

void MOVE(gracz* N, plansza* P)
{
	int nr_statku;
	char klasa_stat[MAX_SKROT_DL];
	char skret;
	cin >> nr_statku >> klasa_stat >> skret;

	int nr_klasy = sprawdzNrStatku(klasa_stat);

	if (N->flota[nr_klasy][nr_statku].aktywny == false || P->POZ[N->flota[nr_klasy][nr_statku].id_silnik] == 'x')
	{
		cout << "INVALID OPERATION " << '"' << "MOVE " << nr_statku << " " << klasa_stat << " " << skret << '"' << ": SHIP CANNOT MOVE";
		wyczyscPlansze(P);
		exit(EXIT_FAILURE);
	}
	if (N->flota[nr_klasy][nr_statku].liczba_ruchow_na_runde == 0)
	{
		cout << "INVALID OPERATION " << '"' << "MOVE " << nr_statku << " " << klasa_stat << " " << skret << '"' << ": SHIP MOVED ALREADY";
		wyczyscPlansze(P);
		exit(EXIT_FAILURE);
	}
	if (N->flota[nr_klasy][nr_statku].rozlozony)
	{
		wspolrzedne stary_czubek = N->flota[nr_klasy][nr_statku].czubek;
		char stary_kierunek = N->flota[nr_klasy][nr_statku].kierunek;

		wspolrzedne nowy_czubek = nowyCzubek(stary_czubek, N->flota[nr_klasy][nr_statku].dlugosc, stary_kierunek, skret);
		char nowy_kierunek = nowyKierunek(stary_kierunek, skret);

		usunStatek(N, P, nr_statku, nr_klasy);
		N->flota[nr_klasy][nr_statku].czubek = nowy_czubek;
		N->flota[nr_klasy][nr_statku].kierunek = nowy_kierunek;
		N->flota[nr_klasy][nr_statku].liczba_ruchow_na_runde--;

		if (sprawdzMozliwoscPostawieniaStatku(N, P, nr_statku, nr_klasy, "MOVE"))
		{
			ustawstatek(N, P, nr_statku, nr_klasy);
		}
	}
}

void resetujFunkcjeStatku(gracz* N)
{
	for (int nr_klasy = 0; nr_klasy < L_RODZ_STATKOW; nr_klasy++)
	{
		for (int nr_statku = 0; nr_statku < MAX_NR_POJ_KLASY; nr_statku++)
		{
			N->flota[nr_klasy][nr_statku].liczba_strzalow_na_runde = N->flota[nr_klasy][nr_statku].dlugosc;
			N->flota[nr_klasy][nr_statku].liczba_ruchow_na_runde = L_RUCHOW;
			if (nr_klasy == CAR)
				N->flota[nr_klasy][nr_statku].liczba_ruchow_na_runde = L_RUCHOW_CAR;
		}
	}
}

bool sprawdzDystans(wspolrzedne A, wspolrzedne B, double odleglosc)
{
	double dy = (double)(A.y) - (double)(B.y);
	double dx = (double)(A.x) - (double)(B.x);

	///KWADRATY ODLEGLOSCI
	if (((dy * dy) + (dx * dx)) <= odleglosc * odleglosc)
		return true;
	else
		return false;
}

void SPY(gracz* N, plansza* P)
{
	int nr_statku;
	int y, x;
	cin >> nr_statku >> y >> x;

	int nr_klasy = CAR;
	///SPY JEST LICZONE JAK SHOOT
	if (!N->flota[nr_klasy][nr_statku].rozlozony || P->POZ[N->flota[nr_klasy][nr_statku].id_dzialo] == 'x')
	{
		cout << "SHIP: " << nr_statku << " CANNOT SEND PLANE";
		wyczyscPlansze(P);
		exit(EXIT_FAILURE);
	}
	else if (N->flota[nr_klasy][nr_statku].liczba_strzalow_na_runde == 0)
	{
		cout << "SHIP: " << nr_statku << " HAS ALL PLANES SENT";
		wyczyscPlansze(P);
		exit(EXIT_FAILURE);

	}
	else
	{
		N->flota[nr_klasy][nr_statku].liczba_strzalow_na_runde--;
		wspolrzedne samolot;
		samolot.y = y;
		samolot.x = x;
		for (int i = 0; i < P->wym_X * P->wym_Y; i++)
		{
			wspolrzedne punkt = konwertujNaWspolrzedne(i, P->wym_X);
			if (sprawdzDystans(samolot, punkt, ZASIEG_SAMOLOTU)) ///kw 3x3 wiec nalezy wziac sqrt2
				P->PER[i] = P->OUT[i];
		}
	}
}

void sprawdzTrafienie(gracz* N, plansza* P)
{
	for (int nr_klasy = 0; nr_klasy < L_RODZ_STATKOW; nr_klasy++)
	{
		for (int nr_statku = 0; nr_statku < N->liczba_statkow_na_klase[nr_klasy]; nr_statku++)
		{
			if (N->flota[nr_klasy][nr_statku].rozlozony)
			{
				int iterator = 0;
				int poc = N->flota[nr_klasy][nr_statku].indx_pocz;
				int kon = N->flota[nr_klasy][nr_statku].indx_kon;
				int skok = N->flota[nr_klasy][nr_statku].iter;
				for (int i = poc; i != kon + skok; i += skok)
				{
					if (P->POZ[i] == 'x')
					{
						N->flota[nr_klasy][nr_statku].stan[iterator] = '0';
					}
					iterator++;
				}
			}
		}
	}
}

void AI(gracz* N, gracz* M, plansza* P)
{
	if (P->seed == DOMYSLNY_SEED)
		srand((unsigned int)time(NULL));
	else
		srand(P->seed);

	gracz* Bot = (N->bot ? N : M);
	P->NEXT_PLAYER = Bot->imie;
	zapiszStanGry(N, M, P);

	cout << "[state]" << endl;
	cout << "PRINT 0" << endl;
	cout << "[state]" << endl;

	cout << "[player" << Bot->imie << "]" << endl;
	///Bot tylko strzela
	if (P->EXTENDED)
		botExShoot(Bot, P);
	else
		botBasShoot(Bot, P);
	cout << "[player" << Bot->imie << "]" << endl;

	cout << "[state]" << endl;
	cout << "PRINT 0" << endl;
	cout << "[state]" << endl;
}

void botBasShoot(gracz* N, plansza* P)
{
	int y = rand() % P->wym_Y;
	int x = rand() % P->wym_X;
	while (P->POZ[y * P->wym_X + x] == N->imie || P->POZ[y * P->wym_X + x] == '#' || P->POZ[y * P->wym_X + x] == 'x' || P->POZ[y * P->wym_X + x] == '.')
	{
		y = rand() % P->wym_Y;
		x = rand() % P->wym_X;
	}
	cout << "SHOOT " << y << " " << x << endl;
}

void botExShoot(gracz* N, plansza* P)
{
	for (int nr_klasy = 0; nr_klasy < L_RODZ_STATKOW; nr_klasy++)
	{
		for (int nr_statku = 0; nr_statku < N->liczba_statkow_na_klase[nr_klasy]; nr_statku++)
		{
			if (N->flota[nr_klasy][nr_statku].rozlozony && P->POZ[N->flota[nr_klasy][nr_statku].id_dzialo] != 'x')
			{
				while (N->flota[nr_klasy][nr_statku].liczba_strzalow_na_runde--)
				{
					int y = rand() % P->wym_Y;
					int x = rand() % P->wym_X;
					///CAR ZAWSZE STRZELI WE WROGA
					if (nr_klasy == CAR)
					{
						while (P->POZ[y * P->wym_X + x] == N->imie || P->POZ[y * P->wym_X + x] == '#' || P->POZ[y * P->wym_X + x] == 'x' || P->POZ[y * P->wym_X + x] == '.')
						{
							y = rand() % P->wym_Y;
							x = rand() % P->wym_X;
						}
					}
					else
					{
						wspolrzedne punkt;
						punkt.y = y;
						punkt.x = x;
						///STRZELA JESLI JEST W ODL
						while (!sprawdzDystans(N->flota[nr_klasy][nr_statku].wsp_armaty, punkt, N->flota[nr_klasy][nr_statku].dlugosc)
							|| P->POZ[y * P->wym_X + x] == N->imie || P->POZ[y * P->wym_X + x] == 'x' || P->POZ[y * P->wym_X + x] == '#')
						{
							y = rand() % P->wym_Y;
							x = rand() % P->wym_X;
							punkt.y = y;
							punkt.x = x;
						}
					}
					cout << "SHOOT " << nr_statku << " " << klasaStatku[nr_klasy] << " " << y << " " << x << endl;
				}
			}
		}
	}
}