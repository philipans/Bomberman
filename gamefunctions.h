/*
+-------------------------------------------------------------------+
|Name:			gamefuntctions.h									|
|Author:		Philip Ansari										|
|Creationdate:	11.01.2017											|
|Version:		1.0													|
|Company:		TUHH												|
+-------------------------------------------------------------------+
|Changelog:															|
|11.01.2016:	-change1											|
|				-change2											|
|																	|
|																	|
|																	|
|																	|
|																	|
|																	|
|																	|
|																	|
|																	|
|																	|
|																	|
|																	|
|																	|
|																	|
|																	|
|																	|
|																	|
+-------------------------------------------------------------------+
*/

#ifndef __gamefunctions_h__
#define __gamefunctions_h__

#include <time.h>

#define TRUE 1
#define FALSE 0
#define WIDTH 341
#define HEIGHT 128
#define PIXEL 40
#define FPS 70
#define DEFAULT_SPEED1 3
#define DEFAULT_SPEED2 4
#define DEFAULT_SPEED3 5
#define DEFAULT_SPEED4 6
#define UNIT 60					//Muss vielfaches von DEFAULT_SPEEDx sein
#define FRIENDLY_FIRE FALSE
#define EXPLOSION_DELAY 5
#define SOUNDS TRUE
#define MUSIC FALSE

typedef struct {
	char nickname[1];
	uint8_t player;
	uint16_t x;
	uint16_t y;
	int16_t score;
	int8_t lifes;
	uint8_t explosion_size;
	uint8_t speed;
	uint8_t max_bomb;
	uint8_t skill;
	uint8_t team;
	uint8_t direction;
	uint8_t invulnerable;
	uint8_t placed_bomb;
	uint8_t hit_player;
} player;

void print_gamefield(char gamefield[][15]) {
	printf("\n");
	for (int i = 0; i <= 12; i++) {
		for (int j = 0; j <= 14; j++) {
			printf("%c ", gamefield[i][j]);
		}
		printf("\n");
	}
}

void create_case(char gamefield[][15], int chance){
	int random, c=0;
	srand(time(NULL));
	for (int i = 1; i <= 11; i++) {
		for (int j = 1; j <= 13; j++) {
			if (gamefield[i][j] == ' ') {							//Auf Leere Felder mit einer Wahrscheinlichkeit eine Kiste erstellen
				random = rand() % 100 + 1;
				if (random <= chance) {
					gamefield[i][j] = 't'; c++;
				}
			}
		}
	}
	printf("\n%d Kisten wurden erfolgreich erstellt!\n", c);
}

void create_powerups(char gamefield[][15], int speed, int maxbomb, int bombsize, int life, int skill) {
	int random, l = 0, s = 0, b = 0, z = 0, sk = 0;
	srand(time(NULL));
	for (int i = 1; i <= 11; i++) {
		for (int j = 1; j <= 13; j++) {
			if (gamefield[i][j] == 't') {				//Wo eine Kiste ist
				random = rand() % 100 + 1;
				if (random <= skill) {
					gamefield[i][j] = 'v'; sk++;			//Powerup erstellen  mit Wahrscheinlichkeit chance
				}
				random = rand() % 100 + 1;
				if (random <= life) {
					gamefield[i][j] = 'w'; l++;			//Powerup erstellen  mit Wahrscheinlichkeit chance
				}
				random = rand() % 100 + 1;
				if (random <= speed) {
					gamefield[i][j] = 'x'; s++;			//Powerup erstellen  mit Wahrscheinlichkeit chance
				}
				random = rand() % 100 + 1;
				if (random <= maxbomb) {
					gamefield[i][j] = 'y'; b++;			//Powerup erstellen  mit Wahrscheinlichkeit chance
				}
				random = rand() % 100 + 1;
				if (random <= bombsize) {
					gamefield[i][j] = 'z'; z++;			//Powerup erstellen  mit Wahrscheinlichkeit chance
				}
			}
		}
	}
	printf("\nPowerups wurden erfolgreich erstellt!\n\nextra Leben: %d\nSpeed: %d\nextra Bombe: %d\nBombengroesse: %d\nSkill: %d\n", l, s, b, z, sk);
}

void clear_start(char gamefield[][15]) {
	for (int i = 0; i <= 12; i++) {
		for (int j = 0; j <= 14; j++) {
			if (gamefield[i][j] == '-') gamefield[i][j] = ' ';
		}
	}
}

void start_position(char* c, SDL_Rect position_player[], int p_nb) {
	player* p1 = (player*)(c + 0 * sizeof(player));
	player* p2 = (player*)(c + 1 * sizeof(player));
	player* p3 = (player*)(c + 2 * sizeof(player));
	player* p4 = (player*)(c + 3 * sizeof(player));
	p1->x = UNIT + (UNIT / 2);																//Oben links
	p1->y = UNIT + (UNIT / 2);
	position_player[0].x = PIXEL + WIDTH;													//Oben links
	position_player[0].y = PIXEL + HEIGHT;
	p2->x = 14 * UNIT - (UNIT / 2);															//Unten rechts
	p2->y = 12 * UNIT - (UNIT / 2);
	position_player[1].x = 13 * PIXEL + WIDTH;												//Unten rechts
	position_player[1].y = 11 * PIXEL + HEIGHT;
	if (p_nb >= 3) {																		//Spielerzahl pr�fen
		p3->x = 14 * UNIT - (UNIT / 2);														//Oben rechts
		p3->y = UNIT + (UNIT / 2);
		position_player[2].x = 13 * PIXEL + WIDTH;											//Oben rechts
		position_player[2].y = PIXEL + HEIGHT;
	}
	if (p_nb >= 4) {																		//Spielerzahl pr�fen
		p4->x = UNIT + (UNIT / 2);															//Unten links
		p4->y = 12 * UNIT - (UNIT / 2);
		position_player[3].x = PIXEL + WIDTH;												//Unten links
		position_player[3].y = 11 * PIXEL + HEIGHT;
	}

}

void player_check(char gamefield[][15], char* c, int nb_p, Mix_Chunk* item_mus, uint16_t powerup[][14]) {
	player *p;
	for (int i = 0; i < nb_p; i++) {
		p = (player*)(c + i * sizeof(player));
		//if (((p->x - (UNIT / 2)) % UNIT == 0) && ((p->y - (UNIT / 2)) % UNIT == 0)) {		//Wenn man geanu auf einem Feld steht
			if (gamefield[p->y / UNIT][p->x / UNIT] == 'a') {								//Wenn ein Item vorhanden ist
				gamefield[p->y / UNIT][p->x / UNIT] = ' ';									//Feld leeren
				p->lifes++;
				p->score += 100;
				powerup[p->y / UNIT][p->x / UNIT] = 0;										//Powerup Timer auf 0 setzen
				if (SOUNDS == TRUE) Mix_PlayChannel(-1, item_mus, 0);
			}
			if (gamefield[p->y / UNIT][p->x / UNIT] == 'b') {
				gamefield[p->y / UNIT][p->x / UNIT] = ' ';
				p->speed++;
				p->score += 50;
				if (p->speed == 2) {										//Speed �nderung auf dem Feld, ->Pixel beachten
					switch (p->direction) {
					case 1: p->y += 1; break;
					case 2:	p->x -= 2; break;
					case 3:	p->y -= 2; break;
					case 4:	p->x += 1; break;
					}
				}
				if (p->speed == 3) {										//Speed �nderung auf dem Feld, ->Pixel beachten
					switch (p->direction) {
					case 1: p->y += 2; break;
					case 2:	p->x -= 2; break;
					case 3:	p->y -= 2; break;
					case 4:	p->x += 2; break;
					}
				}
				if (p->speed == 4) {										//Speed �nderung auf dem Feld, ->Pixel beachten
					switch (p->direction) {
					case 1: p->y -= 1; break;
					case 4:	p->x -= 1; break;
					}
				}
				powerup[p->y / UNIT][p->x / UNIT] = 0;
				if (SOUNDS == TRUE) Mix_PlayChannel(-1, item_mus, 0);
			}
			if (gamefield[p->y / UNIT][p->x / UNIT] == 'c') {
				gamefield[p->y / UNIT][p->x / UNIT] = ' ';
				p->max_bomb++;
				p->score += 50;
				powerup[p->y / UNIT][p->x / UNIT] = 0;
				if (SOUNDS == TRUE) Mix_PlayChannel(-1, item_mus, 0);
			}
			if (gamefield[p->y / UNIT][p->x / UNIT] == 'd') {
				gamefield[p->y / UNIT][p->x / UNIT] = ' ';
				p->explosion_size++;
				p->score += 50;
				powerup[p->y / UNIT][p->x / UNIT] = 0;
				if (SOUNDS == TRUE) Mix_PlayChannel(-1, item_mus, 0);
			}
			if (gamefield[p->y / UNIT][p->x / UNIT] == 'e') {
				gamefield[p->y / UNIT][p->x / UNIT] = ' ';
				p->skill++;
				p->score += 50;
				powerup[p->y / UNIT][p->x / UNIT] = 0;
				if (SOUNDS == TRUE) Mix_PlayChannel(-1, item_mus, 0);
			}
		//}
		if (p->invulnerable > 0) p->invulnerable++;									//Unverwundbarkeit hochz�hlen
		if (p->invulnerable >= FPS + 1) p->invulnerable = 0;					//Wenn l�nger als halbe Sekunde (+1 wegen Bombenexplosion beachten)
		if (p->lifes == 0 && p->invulnerable == 0) {								//Wenn Tod dann Position auf 0|0
			p->x = 0;
			p->y = 0;
		}
	}
}

void create_bomb(char gamefield[][15], uint16_t bomb[][14], char* c, int un) {
	player *p;
	p = (player*)(c + un * sizeof(player));															//Struct Player finden
	p->placed_bomb++;
	int i, j, max = 0;
	for (i = 1; i < 14; i++) {																		//Bomben auf dem Feld z�hlen
		for (j = 1; j < 12; j++) {
			if (gamefield[j][i] == p->player + '0') max++;
		}
	}
	if (max < p->max_bomb) {																		//Wenn unter max_bomb
		if (gamefield[p->y / UNIT][p->x / UNIT] == ' ')  {											//Bombe generieren
			gamefield[p->y / UNIT][p->x / UNIT] = p->player + '0';
			bomb[p->y / UNIT][p->x / UNIT] = 1;														//Timer f�r Bombe starten
		}
	}
}

void moving_bomb(char* c, char gamefield[][15], uint16_t bomb[][14], uint8_t bomb_move[][14], int16_t shiftx[][14], int16_t shifty[][14]) {
	player *p;
	for (int i = 1; i < 12; i++) {
		for (int j = 1; j < 14; j++) {
			if (bomb_move[i][j] % 5 == 1) {																				//Bewegungsrichtung herausfinden
				if (shifty[i][j] > -PIXEL) shifty[i][j] -= 5;															//Wenn noch kein ganzes Feld vorrangekommen
				else {																									//Sobald ein Feld verschoben
					shifty[i][j] = 0;																					//Verscheibung auf 0 setzen
					p = (player*)(c + (bomb_move[i][j] / 5) *sizeof(player));											//Player herausfinden um States zu vergleichen
					bomb_move[i][j] = 0;																				//Bombenrichtung auf 0 setzen
					int temp_bomb = bomb[i][j];																			//BombTimer auf das n�chste Feld �bergeben
					bomb[i][j] = 0;
					bomb[i - 1][j] = temp_bomb;
					gamefield[i - 1][j] = gamefield[i][j];																//Gamefield manipulieren f�r das neue Feld
					gamefield[i][j] = ' ';																				//altes Feld leeren
					if (gamefield[i - 2][j] == ' ' && (p->skill >= 3)) bomb_move[i - 1][j] = 1;							//Wenn dahinter frei ist, und skills erlauben -> weiter bewegen
				}
			}
			if (bomb_move[i][j] % 5 == 2) {																				//Bewegungsrichtung herausfinden
				if (shiftx[i][j] < PIXEL)
					shiftx[i][j] += 5;																					//Wenn noch kein ganzes Feld vorrangekommen
				else {																									//Sobald ein Feld verschoben
					shiftx[i][j] = 0;																					//Verscheibung auf 0 setzen
					p = (player*)(c + (bomb_move[i][j] / 5) *sizeof(player));											//Player herausfinden um States zu vergleichen
					bomb_move[i][j] = 0;																				//Bombenrichtung auf 0 setzen
					int temp_bomb = bomb[i][j];																			//BombTimer auf das n�chste Feld �bergeben
					bomb[i][j] = 0;
					bomb[i][j + 1] = temp_bomb;
					gamefield[i][j + 1] = gamefield[i][j];																//Gamefield manipulieren f�r das neue Feld
					gamefield[i][j] = ' ';																				//altes Feld leeren
					if (gamefield[i][j + 2] == ' ' && (p->skill >= 3)) bomb_move[i][j + 1] = 2;							//Wenn dahinter frei ist, und skills erlauben -> weiter bewegen
				}
			}
			if (bomb_move[i][j] % 5 == 3) {																				//Bewegungsrichtung herausfinden
				if (shifty[i][j] < PIXEL) shifty[i][j] += 5;															//Wenn noch kein ganzes Feld vorrangekommen
				else {																									//Sobald ein Feld verschoben
					shifty[i][j] = 0;																					//Verscheibung auf 0 setzen
					p = (player*)(c + (bomb_move[i][j] / 5) *sizeof(player));											//Player herausfinden um States zu vergleichen
					bomb_move[i][j] = 0;																				//Bombenrichtung auf 0 setzen
					int temp_bomb = bomb[i][j];																			//BombTimer auf das n�chste Feld �bergeben
					bomb[i][j] = 0;
					bomb[i + 1][j] = temp_bomb;
					gamefield[i + 1][j] = gamefield[i][j];																//Gamefield manipulieren f�r das neue Feld
					gamefield[i][j] = ' ';																				//altes Feld leeren
					if (gamefield[i + 2][j] == ' ' && (p->skill >= 3)) bomb_move[i + 1][j] = 3;							//Wenn dahinter frei ist, und skills erlauben -> weiter bewegen
					bomb_move[i][j] = 0;
				}
			}
			if (bomb_move[i][j] % 5 == 4) {																				//Bewegungsrichtung herausfinden
				if (shiftx[i][j] > -PIXEL) shiftx[i][j] -= 5;															//Wenn noch kein ganzes Feld vorrangekommen
				else {																									//Sobald ein Feld verschoben
					shiftx[i][j] = 0;																					//Verscheibung auf 0 setzen
					p = (player*)(c + (bomb_move[i][j] / 5) *sizeof(player));											//Player herausfinden um States zu vergleichen
					bomb_move[i][j] = 0;																				//Bombenrichtung auf 0 setzen
					int temp_bomb = bomb[i][j];																			//BombTimer auf das n�chste Feld �bergeben
					bomb[i][j] = 0;
					bomb[i][j - 1] = temp_bomb;
					gamefield[i][j - 1] = gamefield[i][j];																//Gamefield manipulieren f�r das neue Feld
					gamefield[i][j] = ' ';																				//altes Feld leeren
					if (gamefield[i][j - 2] == ' ' && (p->skill >= 3)) bomb_move[i][j - 1] = 4;							//Wenn dahinter frei ist, und skills erlauben -> weiter bewegen
				}
			}
		}
	}
}

int test_direction(char gamefield[][15], int x, int y, int richtung, int un, uint8_t bomb_move[][14], char* c) {
	player* p;
	p = (player*)(c + un * sizeof(player));
	// If Norden gedr�ckt
	if (richtung == 1) {
		if (y <= (UNIT / 2 + UNIT)) return(0);																															//Randbedingung Norden
		if ((gamefield[(y - (UNIT / 2 + UNIT)) / UNIT][(x - (UNIT / 2)) / UNIT] != ' ') &&																				//Bombe im Weg
			(gamefield[(y - (UNIT / 2 + UNIT)) / UNIT][(x - (UNIT / 2)) / UNIT] <= '4') && ((y - (UNIT / 2 + UNIT)) % UNIT == 0)) {
			if (gamefield[(y - (UNIT / 2 + 2 * UNIT)) / UNIT][(x - (UNIT / 2)) / UNIT] != ' ') {																		//Wenn Hinter Bombe kein Feld frei ist
				if (p->skill >= 4) return(1);																															//Bei Skill 4 m�glich
				return (0);																																				//sonst nicht
			}
			else {																																						//Wenn Hinter Bombe ein Feld frei ist
				if (p->skill >= 2) {																																	//Bei Skill 2 m�gleich
					bomb_move[(y - (UNIT / 2 + UNIT)) / UNIT][(x - (UNIT / 2)) / UNIT] = 1 + 5 * p->player;																//bomb_move wird auf Norden gestellt / Player[x] merken
					return(1);
				}
				return(0);																																				//sonst nicht
			}
		}
		if (((x - (UNIT / 2)) % UNIT == 0) && (gamefield[(y - (UNIT / 2 + 1)) / UNIT][(x - (UNIT / 2)) / UNIT] < 's')) return(1);										//freies Feld -> m�glich
		if (((x - (UNIT / 2)) % UNIT == 0) && (gamefield[(y - (UNIT / 2)) / UNIT][(x - (UNIT / 2)) / UNIT] != ' ')) return(0); 											//genau Zwischen zwei K�sten -> keine Abweichung m�glich
		if (((x - (UNIT / 2)) % UNIT != 0) && ((x - (UNIT / 2)) / UNIT % 2 != 0) && (gamefield[(y - (UNIT / 2)) / UNIT][(x - (UNIT / 2)) / UNIT] < 's'))return(4);		//�ndern nach Westen, wenn Feld frei
		if (((x - (UNIT / 2)) % UNIT != 0) && ((x - (UNIT / 2)) / UNIT % 2 == 0) && (gamefield[(y - (UNIT / 2)) / UNIT][(x + (UNIT / 2)) / UNIT] < 's')) return(2);		//�ndern nach Osten, wenn Feld frei
	}
	// If Osten gedr�ckt
	if (richtung == 2) {
		if (x >= 14 * UNIT - (UNIT / 2)) return(0);																														//Randbedingung Osten
		if ((gamefield[(y - (UNIT / 2)) / UNIT][(x + (UNIT / 2)) / UNIT] != ' ') &&																						//Bombe im Weg
			(gamefield[(y - (UNIT / 2)) / UNIT][(x + (UNIT / 2)) / UNIT] <= '4') && ((x + (UNIT / 2)) % UNIT == 0)) {
			if (gamefield[(y - (UNIT / 2)) / UNIT][(x + (UNIT / 2 + UNIT)) / UNIT] != ' ') {																			//Wenn Hinter Bombe kein Feld frei ist
				if (p->skill >= 4) return(2);																															//Bei Skill 4 m�glich
				return (0);																																				//sonst nicht
			}
			else {																																						//Wenn Hinter Bombe ein Feld frei ist
				if (p->skill >= 2) {																																	//Bei Skill 2 m�gleich
					bomb_move[(y - (UNIT / 2)) / UNIT][(x + (UNIT / 2)) / UNIT] = 2 + 5 * p->player;																	//bomb_move wird auf Osten gestellt / Player[x] merken
					return(2);
				}
				return(0);																																				//sonst nicht
			}
		}
		if (((y - (UNIT / 2)) % UNIT == 0) && (gamefield[(y - (UNIT / 2)) / UNIT][(x + (UNIT / 2)) / UNIT] < 's')) return(2);											//freies Feld -> m�glich//freies Feld
		if (((y - (UNIT / 2)) % UNIT == 0) && (gamefield[(y - (UNIT / 2)) / UNIT][(x + (UNIT / 2)) / UNIT] != ' ')) return(0); 											//genau Zwischen zwei K�sten -> keine Abweichung m�glich//genau Zwischen zwei K�sten
		if (((y - (UNIT / 2)) % UNIT != 0) && ((y - (UNIT / 2)) / UNIT % 2 != 0) && (gamefield[(y - (UNIT / 2)) / UNIT][(x - (UNIT / 2)) / UNIT] < 's')) return(1);		//�ndern nach Norden, wenn Feld frei//�ndern auf hoch
		if (((y - (UNIT / 2)) % UNIT != 0) && ((y - (UNIT / 2)) / UNIT % 2 == 0) && (gamefield[(y + (UNIT / 2)) / UNIT][(x - (UNIT / 2)) / UNIT] < 's')) return(3);		//�ndern nach S�den, wenn Feld frei//�ndern auf runter
	}
	// If S�den gedr�ckt
	if (richtung == 3) {
		if (y >= 12 * UNIT - (UNIT / 2)) return(0);																														//Randbedingung S�den
		if ((gamefield[(y + (UNIT / 2)) / UNIT][(x - (UNIT / 2)) / UNIT] != ' ') &&																						//Bombe im Weg
			(gamefield[(y + (UNIT / 2)) / UNIT][(x - (UNIT / 2)) / UNIT] <= '4') && ((y + (UNIT / 2)) % UNIT == 0)) {
			if (gamefield[(y + (UNIT / 2) + UNIT) / UNIT][(x - (UNIT / 2)) / UNIT] != ' ') {																			//Wenn Hinter Bombe kein Feld frei ist
				if (p->skill >= 4) return(3);																															//Bei Skill 4 m�glich
				return (0);																																				//sonst nicht
			}
			else {																																						//Wenn Hinter Bombe ein Feld frei ist
				if (p->skill >= 2) {																																	//Bei Skill 2 m�gleich
					bomb_move[(y + (UNIT / 2)) / UNIT][(x - (UNIT / 2)) / UNIT] = 3 + 5 * p->player;																	//bomb_move wird auf S�den gestellt / Player[x] merken
					return(3);
				}
				return(0);																																				//sonst nicht
			}
		}
		if (((x - (UNIT / 2)) % UNIT == 0) && (gamefield[(y + (UNIT / 2)) / UNIT][(x - (UNIT / 2)) / UNIT] < 's')) return(3);											//freies Feld -> m�glich//freies Feld
		if (((x - (UNIT / 2)) % UNIT == 0) && (gamefield[(y + (UNIT / 2)) / UNIT][(x - (UNIT / 2)) / UNIT] != ' ')) return(0); 											//genau Zwischen zwei K�sten -> keine Abweichung m�glich//genau Zwischen zwei K�sten
		if (((x - (UNIT / 2)) % UNIT != 0) && ((x - (UNIT / 2)) / UNIT % 2 != 0) && (gamefield[(y - (UNIT / 2)) / UNIT][(x - (UNIT / 2)) / UNIT] < 's'))return(4);		//�ndern nach Westen, wenn Feld frei//�ndern auf links
		if (((x - (UNIT / 2)) % UNIT != 0) && ((x - (UNIT / 2)) / UNIT % 2 == 0) && (gamefield[(y - (UNIT / 2)) / UNIT][(x + (UNIT / 2)) / UNIT] < 's')) return(2);		//�ndern nach Osten, wenn Feld frei//�ndern auf rechts
	}
	// If Westen gedr�ckt
	if (richtung == 4) {
		if (x <= (UNIT / 2 + UNIT)) return(0);																															//Randbedingung Westen
		if ((gamefield[(y - (UNIT / 2)) / UNIT][(x - (UNIT / 2 + UNIT)) / UNIT] != ' ') &&																				//Bombe im Weg
			(gamefield[(y - (UNIT / 2)) / UNIT][(x - (UNIT / 2 + UNIT)) / UNIT] <= '4') && ((x - (UNIT / 2 + UNIT)) % UNIT == 0)) {
			if (gamefield[(y - (UNIT / 2)) / UNIT][(x - (UNIT / 2 + 2 * UNIT)) / UNIT] != ' ') {																		//Wenn Hinter Bombe kein Feld frei ist
				if (p->skill >= 4) return(4);																															//Bei Skill 4 m�glich
				return (0);																																				//sonst nicht
			}
			else {																																						//Wenn Hinter Bombe ein Feld frei ist
				if (p->skill >= 2) {																																	//Bei Skill 2 m�gleich
					bomb_move[(y - (UNIT / 2)) / UNIT][(x - (UNIT / 2 + UNIT)) / UNIT] = 4 + 5 * p->player;																//bomb_move wird auf Westen gestellt / Player[x] merken
					return(4);
				}
				return(0);																																				//sonst nicht
			}
		}
		if (((y - (UNIT / 2)) % UNIT == 0) && (gamefield[(y - (UNIT / 2)) / UNIT][(x - (UNIT / 2 + 1)) / UNIT] < 's')) return(4);										//freies Feld -> m�glich//freies Feld
		if (((y - (UNIT / 2)) % UNIT == 0) && (gamefield[(y - (UNIT / 2)) / UNIT][(x - (UNIT / 2)) / UNIT] != ' ')) return(0); 											//genau Zwischen zwei K�sten -> keine Abweichung m�glich//genau Zwischen zwei K�sten
		if (((y - (UNIT / 2)) % UNIT != 0) && ((y - (UNIT / 2)) / UNIT % 2 != 0) && (gamefield[(y - (UNIT / 2)) / UNIT][(x - (UNIT / 2)) / UNIT] < 's')) return(1);		//�ndern nach Norden, wenn Feld frei//�ndern auf hoch
		if (((y - (UNIT / 2)) % UNIT != 0) && ((y - (UNIT / 2)) / UNIT % 2 == 0) && (gamefield[(y + (UNIT / 2)) / UNIT][(x - (UNIT / 2)) / UNIT] < 's')) return(3);		//�ndern nach S�den, wenn Feld frei//�ndern auf runter
	}
	return(0);
}

void moving(char* c, int un, char gamefield[][15], uint8_t bomb_move[][14], SDL_Rect position_player[]) {
	player* p;
	for (int i = 0; i <= un; i++) {
		p = (player*)(c + i * sizeof(player));
		switch (test_direction(gamefield, p->x, p->y, p->direction, i, bomb_move, c)){
		case 1:
			if (p->speed == 1) p->y -= DEFAULT_SPEED1;
			if (p->speed == 2) p->y -= DEFAULT_SPEED2;
			if (p->speed == 3) p->y -= DEFAULT_SPEED3;
			if (p->speed >= 4) p->y -= DEFAULT_SPEED4;
			position_player[i].x = (p->x - UNIT / 2) * PIXEL / UNIT + WIDTH;
			position_player[i].y = (p->y - UNIT / 2) * PIXEL / UNIT + HEIGHT;
			break;
		case 2:
			if (p->speed == 1)p->x += DEFAULT_SPEED1;
			if (p->speed == 2)p->x += DEFAULT_SPEED2;
			if (p->speed == 3)p->x += DEFAULT_SPEED3;
			if (p->speed >= 4)p->x += DEFAULT_SPEED4;
			position_player[i].x = (p->x - UNIT / 2) * PIXEL / UNIT + WIDTH;
			position_player[i].y = (p->y - UNIT / 2) * PIXEL / UNIT + HEIGHT;
			break;
		case 3:
			if (p->speed == 1) p->y += DEFAULT_SPEED1;
			if (p->speed == 2) p->y += DEFAULT_SPEED2;
			if (p->speed == 3) p->y += DEFAULT_SPEED3;
			if (p->speed >= 4) p->y += DEFAULT_SPEED4;
			position_player[i].x = (p->x - UNIT / 2) * PIXEL / UNIT + WIDTH;
			position_player[i].y = (p->y - UNIT / 2) * PIXEL / UNIT + HEIGHT;
			break;
		case 4:
			if (p->speed == 1) p->x -= DEFAULT_SPEED1;
			if (p->speed == 2) p->x -= DEFAULT_SPEED2;
			if (p->speed == 3) p->x -= DEFAULT_SPEED3;
			if (p->speed >= 4) p->x -= DEFAULT_SPEED4;
			position_player[i].x = (p->x - UNIT / 2) * PIXEL / UNIT + WIDTH;
			position_player[i].y = (p->y - UNIT / 2) * PIXEL / UNIT + HEIGHT;
			break;
		default: break;
		}
	}
}

void explosion(char gamefield[][15], uint16_t bomb[][14], uint16_t fire[][14], uint16_t powerup[][14], int nb_p, char* c, Mix_Chunk* hit_mus, Mix_Chunk* end_mus, Mix_Chunk* bomb_explosion_mus, Mix_Chunk* fuse_mus, SDL_Rect position_player_dead[]) {
	int i, j, k, l, m, x, y, score_bonus=0;
	player *p;
	player *p2;
	player *p3;
	for (i = 1; i < 12; i++) {																				//Alle Felder des Spielfeldes betrachten
		for (j = 1; j < 14; j++) {
			if (powerup[i][j] >= 1) powerup[i][j] += 1;														//Wenn Powerup vorhanden, hoch z�hlen
			if (powerup[i][j] >= FPS * 10) {
				gamefield[i][j] = ' ';																		//Powerup zerst�ren nach 10s
				powerup[i][j] = 0;
			}
			if (bomb[i][j] >= 1) bomb[i][j] += 1;
			if (bomb[i][j] == 60) {																			//Nach einer Sekunde Z�ndsound starten (DELAY BEIM SOUND)
				Mix_VolumeChunk(fuse_mus, 15);
				if (SOUNDS == TRUE) Mix_PlayChannelTimed(-1, fuse_mus, 0, 1000);
			}
			if (bomb[i][j] == FPS * 2 - FPS / 10 - EXPLOSION_DELAY)
			if (SOUNDS == TRUE) Mix_PlayChannel(-1, bomb_explosion_mus, 0);													//Kurz vor Z�ndung, Sound starten (DELAY BEIM SOUND)
			if (fire[i][j] >= FPS / 2 * 4) fire[i][j] = 0;													//Wenn feuer schon 0,5s vorhanden ist, kein Feuer
			if (fire[i][j] >= 1) {																			//pr�fen ob ein Feuer vorhanden ist
				fire[i][j] += 4;																			//Wenn eine Bombe liegt, hoch Z�hlen
				if (bomb[i][j] >= 1 && bomb[i][j] <= FPS * 2 - EXPLOSION_DELAY) {							//Wenn Bombe scharf, dann ausl�sen mit delay
					bomb[i][j] = FPS * 2;																	//Bei Feuer werden Bomben direkt ausgel�st, wenn sie scharf sind
					if (SOUNDS == TRUE) Mix_PlayChannel(-1, bomb_explosion_mus, 0);												//MUSIK DELAY
					p3 = (player*)(c + (gamefield[i][j] - '0') * sizeof(player));
					p3->score += 200;																		//f�r Ausgel�ste Bombe im Feuer +score
				}
				for (m = 0; m <= nb_p; m++) {																//Pr�fen ob ein Spieler sich im Feuer befindet
					p2 = (player*)(c + m * sizeof(player));
					if ((p2->x / UNIT == j) && (p2->y / UNIT == i) && (p2->invulnerable == FALSE)) {		//pr�fen ob ein Player sich im Feuer befindet
						p = (player*)(c + (fire[i][j] % 4 - 1) * sizeof(player));
						if (FRIENDLY_FIRE == TRUE) {														//Friendly Fire aktiviert
							if (p->player == p2->player || p->team == p2->team) {							//Team/Spieler �berpr�fen
								p->score -= 300;
								p2->invulnerable = TRUE;
								continue;
							}
						}
						p2->lifes--;																	//verliert leben
						if (p2->lifes == 0) {
							position_player_dead[p2->player].x = (p2->x - UNIT / 2) * PIXEL / UNIT + WIDTH;
							position_player_dead[p2->player].y = (p2->y - UNIT / 2) * PIXEL / UNIT + HEIGHT;
						}
						if (p2->lifes > 0) { if (SOUNDS == TRUE) Mix_PlayChannel(-1, hit_mus, 0); }
						else if (SOUNDS == TRUE) Mix_PlayChannel(-1, end_mus, 0);											//Musik abspielen
						if (p->player == p2->player) p->score -= 600;
						else {
							p->score += 500;
							p->hit_player++;
						}
						p2->invulnerable = TRUE;														//Unverwundbarkeit aktivieren
						if (p2->lifes == 0) p->score += 500;
					}
				}
			}
			if (bomb[i][j] >= FPS * 2) {																	//Wenn Bombe 2 Sekunden liegt, dann explodiert sie
				bomb[i][j] = 0;																				//Bombe im System Weg
				p = (player*)(c + (gamefield[i][j] - '0') * sizeof(player));								//PlayerX hat Bombe gelegt
				gamefield[i][j] = ' ';																		//Bombe verschwindet vom Spielfeld
				for (l = 1; l <= 4; l++) {																	//Norden, Osten, S�den, Westen betrachten
					for (k = 0; k <= p->explosion_size; k++) {												//Explosionsgr��e pr�fen
						x = j;
						y = i;
						switch (l) {																		//Explosionsrichtungen switch
						case 1: y -= k; break;																//bis explosion size in jede Richtung
						case 2: x += k; break;
						case 3: y += k; break;
						case 4: x -= k; break;
						}
						if (gamefield[y][x] == 's') break;													//Spielfeld, Ende der Explosionsrichtung
						else
						if ((gamefield[y][x] <= '3') && (gamefield[y][x] >= '0')) {							//Pr�fen nach weiteren Bombenexplosionen
							bomb[y][x] = FPS * 2 - EXPLOSION_DELAY;											//Bombe scharf setzen / BOMBENVERZ�GERUNG
							if (fire[y][x] ==  0) fire[y][x] = gamefield[y][x] - '0' + 1;					//Feueranimation starten f�r richtigen Player
							if (p->player != fire[y][x] - 1) p->score += 150;								//Bonus bei ausgel�ster fremder Bombe
							break;
						}
						fire[y][x] = p->player + 1;															//Wenn noch kein Feuer vorhanden, Feuer erstellen mit PlayerNummer
						for (m = 0; m <= nb_p; m++) {														//Pr�fen ob ein Spieler sich im Bombenradius befindet
							p2 = (player*)(c + m * sizeof(player));											// (funktionalit�t siehe bei Feuer)
							if ((p2->x / UNIT == x) && (p2->y / UNIT == y) && (p2->invulnerable == FALSE)) {
								if (FRIENDLY_FIRE == TRUE) {
									if (p->player == p2->player || p->team == p2->team) {
										p->score -= 300;
										p2->invulnerable = TRUE;
										continue;
									}
								}
								p2->lifes--;
								if (p2->lifes == 0) {
									position_player_dead[p2->player].x = (p2->x - UNIT / 2) * PIXEL / UNIT + WIDTH;
									position_player_dead[p2->player].y = (p2->y - UNIT / 2) * PIXEL / UNIT + HEIGHT;
								}
								if (p2->lifes > 0) { if (SOUNDS == TRUE) Mix_PlayChannel(-1, hit_mus, 0); }
								else if (SOUNDS == TRUE) Mix_PlayChannel(-1, end_mus, 0);
								if (p->player == p2->player) p->score -= 600;
								else {
									p->score += 500;
									score_bonus++;
									p->hit_player++;
								}
								p2->invulnerable = TRUE;
								if (p2->lifes == 0) score_bonus++;
							}
						}
						if (gamefield[y][x] == 't') {									//Kiste wird zerst�rt
							gamefield[y][x] = ' ';
							p->score += 13;
							break;
						}
						if (gamefield[y][x] == 'w') {									//Powerup Leben
							gamefield[y][x] =  'a';
							powerup[y][x] = 1;
							p->score += 80;
							break;
						}
						if (gamefield[y][x] == 'x') {									//Powerup Speed
							gamefield[y][x] =  'b';
							powerup[y][x] = 1;
							p->score += 35;
							break;
						}
						if (gamefield[y][x] == 'y') {									//Powerup extra Bombe
							gamefield[y][x] =  'c';
							powerup[y][x] = 1;
							p->score += 42;
							break;
						}
						if (gamefield[y][x] == 'z') {									//Powerup Explosionsgr��e
							gamefield[y][x] =  'd';
							powerup[y][x] = 1;
							p->score += 27;
							break;
						}
						if (gamefield[y][x] == 'v') {									//Powerup Skill
							gamefield[y][x] = 'e';
							powerup[y][x] = 1;
							p->score += 66;
							break;
						}
						if (gamefield[y][x] == 'a') {									//Powerup Leben Zerst�rung
							gamefield[y][x] = ' ';
							p->score -= 60;
							break;
						}
						if (gamefield[y][x] == 'b') {									//Powerup Speed Zerst�rung
							gamefield[y][x] = ' ';
							p->score -= 60;
							break;
						}
						if (gamefield[y][x] == 'c') {									//Powerup extra Bombe Zerst�rung
							gamefield[y][x] = ' ';
							p->score -= 60;
							break;
						}
						if (gamefield[y][x] == 'd') {									//Powerup Explosionsgr��e Zerst�rung
							gamefield[y][x] = ' ';
							p->score -= 60;
							break;
						}
						if (gamefield[y][x] == 'e') {									//Powerup Skill Zerst�rung
							gamefield[y][x] = ' ';
							p->score -= 60;
							break;
						}
					}
				}
				if (score_bonus >= 2) p->score += 500 * (score_bonus -1);				//Extra Punkte bei mehr treffer gleichzeitig
			}
		}
	}
}

int check_dead(char* c, int nb_p, int team) {
	int i, alive=0, team1_alive = 0, team2_alive = 0;
	player *p;
	for (i = 0; i < nb_p; i++) {
		p = (player*)(c + i * sizeof(player));
		if (team == TRUE) {
			if (p->team == 1) if (p->lifes > 0) team1_alive++;
			if (p->team == 2) if (p->lifes > 0) team2_alive++;
		}
		if (p->lifes > 0) alive++;				//wenn einer lebt, dann alive++
	}
	if (team == TRUE) if (team1_alive == 0 || team2_alive == 0) return (1);
	if (alive == 1) return (1);
	if (alive == 0) return (2);
	if (alive >= 1) return (0);
}

void get_score(char* c, int nb_p, int team) {
	player *p;
	for (int i = 0; i <= 25; i++) printf("\n");
	int max = -10000, player_nr = 0, max_team[2] = { 0, 0 }, team_nr = 0, max_team_p;
	for (int i = 0; i < nb_p; i++) {
		p = (player*)(c + i * sizeof(player));
		if (max < p->score) {
			max = p->score;
			player_nr = p->player;
		}
		if (team == TRUE) {
			if (p->team == 1) max_team[0] += p->score;
			if (p->team == 2) max_team[1] += p->score;
		}
		printf("\n--- Stats Player%d ---------------------\n", p->player + 1);
		printf("%d - uebrige Leben\n%d - Speed\n%d - Maximale Bombenanzahl\n%d - Explosionsgroesse\n%d - Skill\n%d - getroffene Gegner\n", p->lifes, p->speed, p->max_bomb, p->explosion_size, p->skill, p->hit_player);
		printf("Spieler%d hat im Spiel insgesamt %d Punkte erreicht!\n", p->player + 1, p->score);
	}
	if (max_team[0] > max_team[1]) {
		max_team_p = max_team[0];
		team_nr = 1;
	}
	else {
		max_team_p = max_team[1];
		team_nr = 2;
	}
	if (team == FALSE) printf("\n------------------------------------------------\nEs gewinnt der Spieler%d mit %d Punkten!\n------------------------------------------------\n", player_nr+1, max);
	if (team == TRUE) printf("\n------------------------------------------------\nEs gewinnt das Team %d mit %d Punkten!\n------------------------------------------------\n", team_nr, max_team_p);
}

void framerate(int starting_tick) {
	if ((1000 / FPS) > SDL_GetTicks() - starting_tick) {
		SDL_Delay(1000 / FPS - (SDL_GetTicks() - starting_tick));
	}
}

#endif
