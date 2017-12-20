/*
+-------------------------------------------------------------------+
|Name:			main.c												|
|Author:		Philip Ansari, Maurice Hoffmann						|
|Creationdate:	11.01.2017											|
|Version:		1.0													|
|Company:		TUHH												|
+-------------------------------------------------------------------+
|Changelog:															|
|11.01.2016:	-maindatei erstellt									|
|				-Bomben unbegehbar									|
|				-visuelles leben verlieren							|
|				-visuelles sterben									|
|				-Speed geschwindigkeiten variable					|
|				-powerups verschwinden nach einer Zeit				|
|				-Firendly Fire ber�cksichtigen						|
|				-Explosionsanimation (Feuer und Bombe erneuern)		|
|				-Teams f�r Explosion Optimieren						|
|				-Audio einbringen									|
|				-Bomben explodieren nicht direkt					|
|				-Scores									 			|
|				-BUGS behoben: Leben verlieren (gleichzeitig)		|
|				-Bilder am ende Free								|
|				-BUG: Bomben verschwinden							|
|				-Bomben bewegbar									|
|				-Bombenbewegung mit -PIXEL bild verschwindet		|
|				-neue Bombenexplosion im feuer						|
|				-Bombenlegung ver�ndert								|
|				-items einsammeln runden							|
|				-Team initialisierung								|
|				-Bombenbewegung in funktion setzen					|
|				-Renderfunction.h angelegt							|
|				-Auswahlmen�										|
|				-Player selected �ndern								|
|				-Hintergrundbild des Men�s							|
|				-Teams entstehen nicht bei 2,3 Player (Team vorher)	|
|				-RIP Bild �ndern									|
|				-Player soll nicht beachtet werden bei tod			|
|				-per Mouse-Over ausw�hlbar							|
|				-Darstellung Selected Teams k�rzen					|
|				-Button initialisierung								|
|				-Itemanzeige										|
|				-Scores beim Spiel anzeigen lassen					|
|				-Zur�ck und Start Taste								|
|																	|
|-------------------------------------------------------------------|
|																	|
|To Do:		-Spiel:			Bomben werfen bei Skill 4 m�glich		|
|							Optionen einf�gen						|
|							Statistiken einf�gen					|
|																	|
|			-Oberfl�che:	andere Musik bei Men�					|
|							Sound bei Men�postionen					|
|																	|
|			-System:		Musik soll direkt starten ohne delay	|
|							Variable Feldgr��e						|
|							Fehlermeldungen einbauen				|
|							Bilder in Ordner packen					|
|							gamefield intialisation in for(x)		|
|							Menu Renderfunction.h					|
+-------------------------------------------------------------------+
*/
/*
BUGS:
Bombe verschwindet. Feuer kommt auf dem Feld

Performance verbessern:
-Programmneustart nicht alle Daten neu laden
-Bombenexplosion startfeld wiederholt 4 mal
-dynamische Arrays verwenden
-structs verwenden anstatt Arrays
*/

//Einbindung der Headerdateien
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "gamefunctions.h"
#include "renderfunctions.h"

//Globale Makros
#define CASE TRUE
#define POWERUP TRUE
#define CHANCE_CASE 88
#define CHANCE_SKILL 12
#define CHANCE_LIFE 3
#define CHANCE_SPEED 12
#define CHANCE_MAXBOMB 16
#define CHANCE_BOMBSIZE 12
#define DEFAULT_LIFE 3
#define DEFAULT_EXPLOSION_SIZE 2
#define DEFAULT_SPEED 1						// MAX 4
#define DEFAULT_MAXBOMB 1
#define DEFAULT_SKILL 1						// MAX 4:  0 -> Keine Bomben betrachten
														// 1 -> Bomben blockieren
														// 2 -> Bomben bewegbar
														// 3 -> Bomben kicken
														// 4 -> Bomben �berrennbar

int main(int argc, char* args[]){

	printf("Starting Bomberman.exe...\n\n");
	//Deklarierung und Initialisierung der Localen Variablen
	player Players[4];												//Struct Player wird erstellt
	uint8_t number_of_players;										//Spieleranzahl
	uint8_t i, j, m;												//Schleifenhilfe
	int16_t shiftx[12][14] = { 0 }, shifty[12][14] = { 0 };			//aktuelle Verschiebung der Bombe
	uint16_t bomb[12][14] = { 0 };									//BombenTimer f�r die explosion
	uint16_t fire[12][14] = { 0 };									//FeuerTimer f�r aktuelles Feuer
	uint16_t powerup[12][14] = { 0 };								//PowerupTimer f�r verschwindene Items
	uint8_t bomb_move[12][14] = { 0 };								//Bewegungsrichtung der Bombe & Player abfrage
	uint32_t starting_tick;											//Framerate Begrenzung
	uint8_t menu_position[7] = { 1, 1, 1, 1, 1, 1, 1};				//Position des ausgew�hlten
	uint8_t menu_ebene = 0;											//Menu Ebene
	uint8_t player_team_pos[4] = { 1, 1, 2, 2 };					//Team positionen
	uint8_t isRunning_menu = 1, isRunning = 1;						//Programmbedingung f�r Menu
	uint8_t quit = FALSE;											//Bedingung f�r laufendes Programm
	uint8_t team = FALSE;
	char clife = 'a';												//char im Spielfeld
	char cspeed = 'b';
	char cmaxbomb = 'c';
	char cbombsize = 'd';
	char cskill = 'e';
	char* pPlayers = &Players[0].nickname[0];						//Pointer auf den erstellten Struct
	const uint8_t *keyState;										//Bewegungsrichtungspointer

	//SDL-Loading
	SDL_Event event;												//Key Down event
	SDL_Window* window = NULL;										//�ffnet SDL Fenster
	SDL_Renderer* renderer = NULL;									//Renderspeicher
	SDL_Surface* load_surface = NULL;								//Bild laden

	//SDL_mixer Loading
	Mix_Music* playback_mus = NULL;
	Mix_Chunk* bomb_explosion_mus = NULL;
	Mix_Chunk* item_mus = NULL;
	Mix_Chunk* hit_mus = NULL;
	Mix_Chunk* end_mus = NULL;
	Mix_Chunk* fuse_mus = NULL;
	Mix_Chunk* saying1_mus = NULL;

	//Texture Initialisation
	SDL_Texture* texture_menu_background = { NULL };
	SDL_Texture* texture_menu_credits = { NULL };
	SDL_Texture* texture_menu_1[2] = { NULL };
	SDL_Texture* texture_menu_2[2] = { NULL };
	SDL_Texture* texture_menu_3[2] = { NULL };
	SDL_Texture* texture_menu_4[2] = { NULL };
	SDL_Texture* texture_menu_player[6] = { NULL };
	SDL_Texture* texture_menu_team[4] = { NULL };
	SDL_Texture* texture_menu_team1_2[10] = { NULL };
	SDL_Texture* texture_menu_close[2] = { NULL };
	SDL_Texture* texture_menu_back[2] = { NULL };
	SDL_Texture* texture_menu_start[2] = { NULL };
	SDL_Texture* texture_background = NULL;
	SDL_Texture* texture_bomb[11] = { NULL };
	SDL_Texture* texture_player[4] = { NULL };
	SDL_Texture* texture_rip[4] = { NULL };
	SDL_Texture* texture_case = NULL;
	SDL_Texture* texture_life = NULL;
	SDL_Texture* texture_speed = NULL;
	SDL_Texture* texture_maxbomb = NULL;
	SDL_Texture* texture_bombsize = NULL;
	SDL_Texture* texture_skill = NULL;
	SDL_Texture* texture_explosion[4] = { NULL };
	SDL_Texture* texture_number[13] = { NULL };
	SDL_Texture* texture_item_display[4] = { NULL };

	//Rect Initialisation Menu
	SDL_Rect rect_button = { 0, 0, 500, 100 };
	SDL_Rect rect_button2 = { 0, 0, 300, 100 };
	SDL_Rect rect_button3 = { 0, 0, 200, 69 };
	SDL_Rect rect_icon = { 0, 0, 80, 80 };
	SDL_Rect rect_item_display = { 0, 0, 244, 209};
	SDL_Rect position_button[5] = { 0 };
	SDL_Rect position_button_player[4] = { 0 };
	SDL_Rect position_button_teams[6] = { 0 };
	SDL_Rect position_button_back = { 540, 700, 200, 69 };
	//Rect Initialisation Spiel
	SDL_Rect rect_pixel = { 0, 0, PIXEL, PIXEL };	//for all pictures
	SDL_Rect position_player[4] = { 0 };			//f�r 4 Spieler
	SDL_Rect position_player_dead[4] = { 0 };			//f�r 4 Spieler
	SDL_Rect position_item[110] = { 0 };			//f�r jegliche Felder im Spielfeld
	SDL_Rect position_level[4][6] = { 0 };
	SDL_Rect position_score[4][5] = { 0 };
	SDL_Rect position_item_display[4];

POS_START:

	//create Window
	printf("SDL is creating window\n");
	window = SDL_CreateWindow("Bomberman", 100, 100, 1280, 800, 0);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	printf("window creating successfull\n");

	//Musik Player �ffnen
	printf("starting music Player...\n");
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

	//Musik Initialisierung
	printf("SDL is loading music... \n");
	playback_mus = Mix_LoadMUS("./media/music/playback.mp3");
	bomb_explosion_mus = Mix_LoadWAV("./media/music/bomb_explosion.wav");
	item_mus = Mix_LoadWAV("./media/music/item.wav");
	hit_mus = Mix_LoadWAV("./media/music/hit.aiff");
	end_mus = Mix_LoadWAV("./media/music/end.flac");
	fuse_mus = Mix_LoadWAV("./media/music/fuse.wav");
	saying1_mus = Mix_LoadWAV("./media/music/saying1.wav");
	printf("music load is finished!\n");

	//Hintergrund Menu
	printf("SDL is loading textures...\n");
	load_surface = IMG_Load("./media/menu_background.png");
	texture_menu_background = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);

	//Buttons
	load_surface = IMG_Load("./media/button_play.png");
	texture_menu_1[0] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/button_play_selected.png");
	texture_menu_1[1] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/button_stats.png");
	texture_menu_2[0] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/button_stats_selected.png");
	texture_menu_2[1] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/button_optionen.png");
	texture_menu_3[0] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/button_optionen_selected.png");
	texture_menu_3[1] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/button_credits.png");
	texture_menu_4[0] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/button_credits_selected.png");
	texture_menu_4[1] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/buttons_beenden.png");
	texture_menu_close[0] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/buttons_beenden_selected.png");
	texture_menu_close[1] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/buttons_back.png");
	texture_menu_back[0] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/buttons_back_selected.png");
	texture_menu_back[1] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/button_start.png");
	texture_menu_start[0] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/button_start_selected.png");
	texture_menu_start[1] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);

	//Credit
	load_surface = IMG_Load("./media/credits.png");
	texture_menu_credits = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);

	//Spieler f�r Team
	load_surface = IMG_Load("./media/buttons_2player.png");
	texture_menu_player[0] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/buttons_2player_selected.png");
	texture_menu_player[1] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/buttons_3player.png");
	texture_menu_player[2] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/buttons_3player_selected.png");
	texture_menu_player[3] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/buttons_4player.png");
	texture_menu_player[4] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/buttons_4player_selected.png");
	texture_menu_player[5] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);

	//Teams auswahl
	load_surface = IMG_Load("./media/buttons_team.png");
	texture_menu_team[0] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/buttons_team_selected.png");
	texture_menu_team[1] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/buttons_noteam.png");
	texture_menu_team[2] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/buttons_noteam_selected.png");
	texture_menu_team[3] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);

	//Team/Icons
	load_surface = IMG_Load("./media/buttons_team1.png");
	texture_menu_team1_2[0] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/buttons_team2.png");
	texture_menu_team1_2[1] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);

	//Team Player
	load_surface = IMG_Load("./media/buttons_player_01.png");
	texture_menu_team1_2[2] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/buttons_player_02.png");
	texture_menu_team1_2[4] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/buttons_player_03.png");
	texture_menu_team1_2[6] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/buttons_player_04.png");
	texture_menu_team1_2[8] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);

	//Team Player selected
	load_surface = IMG_Load("./media/buttons_player_01_selected.png");
	texture_menu_team1_2[3] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/buttons_player_02_selected.png");
	texture_menu_team1_2[5] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/buttons_player_03_selected.png");
	texture_menu_team1_2[7] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/buttons_player_04_selected.png");
	texture_menu_team1_2[9] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);

	//Number Initialisierung
	load_surface = IMG_Load("./media/00.png");
	texture_number[0] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/01.png");
	texture_number[1] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/02.png");
	texture_number[2] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/03.png");
	texture_number[3] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/04.png");
	texture_number[4] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/05.png");
	texture_number[5] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/06.png");
	texture_number[6] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/07.png");
	texture_number[7] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/08.png");
	texture_number[8] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/09.png");
	texture_number[9] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/max.png");
	texture_number[10] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/team_01.png");
	texture_number[11] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/team_02.png");
	texture_number[12] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);

	//Background Initialisierung
	load_surface = IMG_Load("./media/background.png");
	texture_background = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);

	//Player Initialisierung
	load_surface = IMG_Load("./media/player_01.png");
	texture_player[0] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/player_02.png");
	texture_player[1] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/player_03.png");
	texture_player[2] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/player_04.png");
	texture_player[3] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/rip_01.png");
	texture_rip[0] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/rip_02.png");
	texture_rip[1] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/rip_03.png");
	texture_rip[2] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/rip_04.png");
	texture_rip[3] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);

	//Bomben Initialisierung
	load_surface = IMG_Load("./media/bomb_01.png");
	texture_bomb[0] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/bomb_02.png");
	texture_bomb[1] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/bomb_03.png");
	texture_bomb[2] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/bomb_04.png");
	texture_bomb[3] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/bomb_05.png");
	texture_bomb[4] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/bomb_06.png");
	texture_bomb[5] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/bomb_07.png");
	texture_bomb[6] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/bomb_08.png");
	texture_bomb[7] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/bomb_09.png");
	texture_bomb[8] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/bomb_10.png");
	texture_bomb[9] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/bomb_00.png");
	texture_bomb[10] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);

	//Case / Powerups Initialisierung
	load_surface = IMG_Load("./media/brick.png");
	texture_case = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/life.png");
	texture_life = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/speed.png");
	texture_speed = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/extra_bomb.png");
	texture_maxbomb = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/size.png");
	texture_bombsize = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/skill.png");
	texture_skill = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);

	//Fire Initialisierung
	load_surface = IMG_Load("./media/explosion_01.png");
	texture_explosion[0] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/explosion_02.png");
	texture_explosion[1] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/explosion_03.png");
	texture_explosion[2] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/explosion_04.png");
	texture_explosion[3] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);

	//Item Display Initialisierung
	load_surface = IMG_Load("./media/item_display_01.png");
	texture_item_display[0] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/item_display_02.png");
	texture_item_display[1] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/item_display_03.png");
	texture_item_display[2] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	load_surface = IMG_Load("./media/item_display_04.png");
	texture_item_display[3] = SDL_CreateTextureFromSurface(renderer, load_surface);
	SDL_FreeSurface(load_surface);
	printf("texture load is finished!\n");

	//Playerpos f�r Team
	for (i = 0; i < 4; i++) {
		position_button_teams[i + 2].x = 375;
		position_button_teams[i + 2].y = 250 + i * 80;
		position_button_teams[i + 2].w = 80;
		position_button_teams[i + 2].h = 80;
		position_button[i].x = 390;
		position_button[i].y = 150 + i * 120;
		position_button[i].w = 500;
		position_button[i].h = 100;
	}

	//Buttons 1-5
	for (i = 0; i < 5; i++) {
		position_button[i].x = 390;
		position_button[i].y = 150 + i * 110;
		position_button[i].w = 500;
		position_button[i].h = 100;
	}

	//Team button
	for (i = 0; i < 2; i++) {
		position_button_teams[i].x = 290 + i * 450;
		position_button_teams[i].y = 100;
		position_button_teams[i].w = 300;
		position_button_teams[i].h = 100;
	}

	//Team Start Verschiebung
	position_button_teams[4].x = 825;
	position_button_teams[5].x = 825;

	//position Initialisierung -> Pixel gr��e festlegen f�r die Darstellung
	for (i = 0; i < 4; i++) {
		position_player[i].w = position_player[i].h = PIXEL;
		position_player_dead[i].w = position_player_dead[i].h = PIXEL;
	}
	//position_case Initialisierung
	for (i = 0; i < 110; i++) position_item[i].w = position_item[i].h = PIXEL;

	//Itemanzeige im Game
	/*
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 6; j++) {
			position_level[i][j].h = position_level[i][j].w = PIXEL;
			position_level[i][0].x = 80;
			position_level[i][0].y = 90 + 185* i;
			position_level[i][1].x = 195;
			position_level[i][1].y = 90 + 185 * i;
			position_level[i][2].x = 80;
			position_level[i][2].y = 130 + 185 * i;
			position_level[i][3].x = 195;
			position_level[i][3].y = 130 + 185 * i;
			position_level[i][4].x = 80;
			position_level[i][4].y = 180 + 183 * i;
			position_level[i][5].x = 195;
			position_level[i][5].y = 180 + 183 * i;
		}
	}
	*/

	//Itemanzeige im Game
	int itemx[4] = { 45, 990, 990, 45 };
	int itemy[4] = { 35, 500, 35, 500 };
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 6; j++) {
			position_level[i][j].h = position_level[i][j].w = PIXEL;
		}
			position_level[i][0].x = itemx[i] + 52;
			position_level[i][0].y = itemy[i] + 43;
			position_level[i][1].x = itemx[i] + 172;
			position_level[i][1].y = itemy[i] + 43;
			position_level[i][2].x = itemx[i] + 52;
			position_level[i][2].y = itemy[i] + 83;
			position_level[i][3].x = itemx[i] + 172;
			position_level[i][3].y = itemy[i] + 83;
			position_level[i][4].x = itemx[i] + 52;
			position_level[i][4].y = itemy[i] + 123;
			position_level[i][5].x = itemx[i] + 122;
			position_level[i][5].y = itemy[i] + 123;
		for (j = 0; j < 5; j++) {
			position_score[i][j].h = position_score[i][j].w = PIXEL;
			position_score[i][j].x = itemx[i] + 122 + j * PIXEL / 2;
			position_score[i][j].y = itemy[i] + 165;
		}
		position_item_display[i].w = 244;
		position_item_display[i].h = 209;
		position_item_display[i].x = itemx[i];
		position_item_display[i].y = itemy[i];
	}

	//keine Teams
	for (i = 0; i < 4; i++) Players[i].team = i + 1;

//----------------------------------------------------------------------------------------------------------------------------MEN�STARTEINGABE

POS_MENU_START:
	team = FALSE;
	//Show Current Image
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture_menu_background, NULL, NULL);
	SDL_RenderCopy(renderer, texture_menu_1[1], &rect_button, &position_button[0]);
	SDL_RenderCopy(renderer, texture_menu_2[0], &rect_button, &position_button[1]);
	SDL_RenderCopy(renderer, texture_menu_3[0], &rect_button, &position_button[2]);
	SDL_RenderCopy(renderer, texture_menu_4[0], &rect_button, &position_button[3]);
	SDL_RenderPresent(renderer);

	//Anfang Spiel
	while (isRunning_menu){
		//Menuauswahl -> Eingabe
		while (SDL_PollEvent(&event)){ //Zum schlie�en der Anwendung
			if (event.type == SDL_QUIT){
				isRunning_menu = 0;
				isRunning = 0;
				quit = TRUE;
			}
			//Eingabe per Mouse
			else if (event.type == SDL_MOUSEMOTION) {
				switch (menu_ebene) {
				case 0:
					if ((event.button.x > position_button[0].x) && (event.button.x < position_button[0].x + position_button[0].w) &&
						(event.button.y > position_button[0].y) && (event.button.y < position_button[0].y + position_button[0].h))
						menu_position[menu_ebene] = 1;
					if ((event.button.x > position_button[1].x) && (event.button.x < position_button[1].x + position_button[1].w) &&
						(event.button.y > position_button[1].y) && (event.button.y < position_button[1].y + position_button[1].h))
						menu_position[menu_ebene] = 2;
					if ((event.button.x > position_button[2].x) && (event.button.x < position_button[2].x + position_button[2].w) &&
						(event.button.y > position_button[2].y) && (event.button.y < position_button[2].y + position_button[2].h))
						menu_position[menu_ebene] = 3;
					if ((event.button.x > position_button[3].x) && (event.button.x < position_button[3].x + position_button[3].w) &&
						(event.button.y > position_button[3].y) && (event.button.y < position_button[3].y + position_button[3].h))
						menu_position[menu_ebene] = 4;
					if ((event.button.x > position_button[4].x) && (event.button.x < position_button[4].x + position_button[4].w) &&
						(event.button.y > position_button[4].y) && (event.button.y < position_button[4].y + position_button[4].h))
						menu_position[menu_ebene] = 5;
					break;
				case 1:
					if ((event.button.x > position_button[0].x) && (event.button.x < position_button[0].x + position_button[0].w) &&
						(event.button.y > position_button[0].y) && (event.button.y < position_button[0].y + position_button[0].h))
						menu_position[menu_ebene] = 1;
					if ((event.button.x > position_button[1].x) && (event.button.x < position_button[1].x + position_button[1].w) &&
						(event.button.y > position_button[1].y) && (event.button.y < position_button[1].y + position_button[1].h))
						menu_position[menu_ebene] = 2;
					if ((event.button.x > position_button[2].x) && (event.button.x < position_button[2].x + position_button[2].w) &&
						(event.button.y > position_button[2].y) && (event.button.y < position_button[2].y + position_button[2].h))
						menu_position[menu_ebene] = 3;
					if ((event.button.x > position_button_back.x) && (event.button.x < position_button_back.x + position_button_back.w) &&
						(event.button.y > position_button_back.y) && (event.button.y < position_button_back.y + position_button_back.h))
						menu_position[menu_ebene] = 4;
					break;
				case 2:
					if ((event.button.x > position_button[0].x) && (event.button.x < position_button[0].x + position_button[0].w) &&
						(event.button.y > position_button[0].y) && (event.button.y < position_button[0].y + position_button[0].h))
						menu_position[menu_ebene] = 1;
					if ((event.button.x > position_button[1].x) && (event.button.x < position_button[1].x + position_button[1].w) &&
						(event.button.y > position_button[1].y) && (event.button.y < position_button[1].y + position_button[1].h))
						menu_position[menu_ebene] = 2;
					if ((event.button.x > position_button_back.x) && (event.button.x < position_button_back.x + position_button_back.w) &&
						(event.button.y > position_button_back.y) && (event.button.y < position_button_back.y + position_button_back.h))
						menu_position[menu_ebene] = 3;
					break;
				case 3:
					if ((event.button.x > position_button_teams[2].x) && (event.button.x < position_button_teams[2].x + position_button_teams[2].w) &&
						(event.button.y > position_button_teams[2].y) && (event.button.y < position_button_teams[2].y + position_button_teams[2].h))
						menu_position[menu_ebene] = 1;
					if ((event.button.x > position_button_teams[3].x) && (event.button.x < position_button_teams[3].x + position_button_teams[3].w) &&
						(event.button.y > position_button_teams[3].y) && (event.button.y < position_button_teams[3].y + position_button_teams[3].h))
						menu_position[menu_ebene] = 2;
					if ((event.button.x > position_button_teams[4].x) && (event.button.x < position_button_teams[4].x + position_button_teams[4].w) &&
						(event.button.y > position_button_teams[4].y) && (event.button.y < position_button_teams[4].y + position_button_teams[4].h))
						menu_position[menu_ebene] = 3;
					if ((event.button.x > position_button_teams[5].x) && (event.button.x < position_button_teams[5].x + position_button_teams[5].w) &&
						(event.button.y > position_button_teams[5].y) && (event.button.y < position_button_teams[5].y + position_button_teams[5].h))
						menu_position[menu_ebene] = 4;
					if ((event.button.x > position_button[4].x) && (event.button.x < position_button[4].x + position_button[4].w) &&
						(event.button.y > position_button[4].y) && (event.button.y < position_button[4].y + position_button[4].h))
						menu_position[menu_ebene] = 5;
					if ((event.button.x > position_button_back.x) && (event.button.x < position_button_back.x + position_button_back.w) &&
						(event.button.y > position_button_back.y) && (event.button.y < position_button_back.y + position_button_back.h))
						menu_position[menu_ebene] = 6;
					break;
				case 4:
					if ((event.button.x > position_button_back.x) && (event.button.x < position_button_back.x + position_button_back.w) &&
						(event.button.y > position_button_back.y) && (event.button.y < position_button_back.y + position_button_back.h))
						menu_position[menu_ebene] = 1;
					break;
				case 5:
					if ((event.button.x > position_button_back.x) && (event.button.x < position_button_back.x + position_button_back.w) &&
						(event.button.y > position_button_back.y) && (event.button.y < position_button_back.y + position_button_back.h))
						menu_position[menu_ebene] = 1;
					break;
				case 6:
					if ((event.button.x > position_button_back.x) && (event.button.x < position_button_back.x + position_button_back.w) &&
						(event.button.y > position_button_back.y) && (event.button.y < position_button_back.y + position_button_back.h))
						menu_position[menu_ebene] = 1;
					break;
				}
			}
			else if (event.type == SDL_MOUSEBUTTONUP) {
				if (event.button.button == SDL_BUTTON_LEFT) {
					switch (menu_ebene) {
					case 0:
						if ((event.button.x > position_button[0].x) && (event.button.x < position_button[0].x + position_button[0].w) &&
							(event.button.y > position_button[0].y) && (event.button.y < position_button[0].y + position_button[0].h))
							menu_ebene = 1;
						if ((event.button.x > position_button[1].x) && (event.button.x < position_button[1].x + position_button[1].w) &&
							(event.button.y > position_button[1].y) && (event.button.y < position_button[1].y + position_button[1].h))
							menu_ebene = 5;
						if ((event.button.x > position_button[2].x) && (event.button.x < position_button[2].x + position_button[2].w) &&
							(event.button.y > position_button[2].y) && (event.button.y < position_button[2].y + position_button[2].h))
							menu_ebene = 6;
						if ((event.button.x > position_button[3].x) && (event.button.x < position_button[3].x + position_button[3].w) &&
							(event.button.y > position_button[3].y) && (event.button.y < position_button[3].y + position_button[3].h))
							menu_ebene = 4;
						if ((event.button.x > position_button[4].x) && (event.button.x < position_button[4].x + position_button[4].w) &&
							(event.button.y > position_button[4].y) && (event.button.y < position_button[4].y + position_button[4].h)) {
							isRunning = isRunning_menu = 0;
							quit = TRUE;
						}
						break;
					case 1:
						if ((event.button.x > position_button[0].x) && (event.button.x < position_button[0].x + position_button[0].w) &&
							(event.button.y > position_button[0].y) && (event.button.y < position_button[0].y + position_button[0].h)) {
							number_of_players = 2;
							isRunning_menu = 0;
						}
						if ((event.button.x > position_button[1].x) && (event.button.x < position_button[1].x + position_button[1].w) &&
							(event.button.y > position_button[1].y) && (event.button.y < position_button[1].y + position_button[1].h)) {
							number_of_players = 3;
							isRunning_menu = 0;
						}
						if ((event.button.x > position_button[2].x) && (event.button.x < position_button[2].x + position_button[2].w) &&
							(event.button.y > position_button[2].y) && (event.button.y < position_button[2].y + position_button[2].h)) {
							number_of_players = 4;
							menu_ebene = 2;
						}
						if ((event.button.x > position_button_back.x) && (event.button.x < position_button_back.x + position_button_back.w) &&
							(event.button.y > position_button_back.y) && (event.button.y < position_button_back.y + position_button_back.h))
							menu_ebene--;
						break;
					case 2:
						if ((event.button.x > position_button[0].x) && (event.button.x < position_button[0].x + position_button[0].w) &&
							(event.button.y > position_button[0].y) && (event.button.y < position_button[0].y + position_button[0].h)) {
							menu_ebene = 3;																									//zur Teamauswahl
							for (i = 0; i < 4; i++) {
								if (position_button_teams[i+2].x == 825) Players[i].team = 2;	//aktuelles Team einstellen
								else Players[i].team = 1;
							}
						}
						if ((event.button.x > position_button[1].x) && (event.button.x < position_button[1].x + position_button[1].w) &&
							(event.button.y > position_button[1].y) && (event.button.y < position_button[1].y + position_button[1].h))
							isRunning_menu = FALSE;
						if ((event.button.x > position_button_back.x) && (event.button.x < position_button_back.x + position_button_back.w) &&
							(event.button.y > position_button_back.y) && (event.button.y < position_button_back.y + position_button_back.h))
							menu_ebene--;
						break;
					case 3:
						if ((event.button.x > position_button_teams[2].x) && (event.button.x < position_button_teams[2].x + position_button_teams[2].w) &&
							(event.button.y > position_button_teams[2].y) && (event.button.y < position_button_teams[2].y + position_button_teams[2].h)) {
							if (position_button_teams[2].x != 825) {
								position_button_teams[2].x = 825;
								Players[0].team = 2;
							}
							else {
								position_button_teams[2].x = 375;
								Players[0].team = 1;
							}
						}
						if ((event.button.x > position_button_teams[3].x) && (event.button.x < position_button_teams[3].x + position_button_teams[3].w) &&
							(event.button.y > position_button_teams[3].y) && (event.button.y < position_button_teams[3].y + position_button_teams[3].h)) {
							if (position_button_teams[3].x != 825) {
								position_button_teams[3].x = 825;
								Players[1].team = 2;
							}
							else {
								position_button_teams[3].x = 375;
								Players[1].team = 1;
							}
						}
						if ((event.button.x > position_button_teams[4].x) && (event.button.x < position_button_teams[4].x + position_button_teams[4].w) &&
							(event.button.y > position_button_teams[4].y) && (event.button.y < position_button_teams[4].y + position_button_teams[4].h)) {
							if (position_button_teams[4].x != 825) {
								position_button_teams[4].x = 825;
								Players[2].team = 2;
							}
							else {
								position_button_teams[4].x = 375;
								Players[2].team = 1;
							}
						}
						if ((event.button.x > position_button_teams[5].x) && (event.button.x < position_button_teams[5].x + position_button_teams[5].w) &&
							(event.button.y > position_button_teams[5].y) && (event.button.y < position_button_teams[5].y + position_button_teams[5].h)) {
							if (position_button_teams[5].x != 825) {
								position_button_teams[5].x = 825;
								Players[3].team = 2;
							}
							else {
								position_button_teams[5].x = 375;
								Players[3].team = 1;
							}
						}
						if ((event.button.x > position_button[4].x) && (event.button.x < position_button[4].x + position_button[4].w) &&
							(event.button.y > position_button[4].y) && (event.button.y < position_button[4].y + position_button[4].h))
							isRunning_menu = 0;
						if ((event.button.x > position_button_back.x) && (event.button.x < position_button_back.x + position_button_back.w) &&
							(event.button.y > position_button_back.y) && (event.button.y < position_button_back.y + position_button_back.h))
							menu_ebene--;
						break;
					case 4:
						if ((event.button.x > position_button_back.x) && (event.button.x < position_button_back.x + position_button_back.w) &&
							(event.button.y > position_button_back.y) && (event.button.y < position_button_back.y + position_button_back.h))
							menu_ebene = 0;
						break;
					case 5:
						if ((event.button.x > position_button_back.x) && (event.button.x < position_button_back.x + position_button_back.w) &&
							(event.button.y > position_button_back.y) && (event.button.y < position_button_back.y + position_button_back.h))
							menu_ebene = 0;
						break;
					case 6:
						if ((event.button.x > position_button_back.x) && (event.button.x < position_button_back.x + position_button_back.w) &&
							(event.button.y > position_button_back.y) && (event.button.y < position_button_back.y + position_button_back.h))
							menu_ebene = 0;
						break;
					}
				}
			}
			//eingabe per Tastatur
			else if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym){
				case SDLK_UP:
					switch (menu_ebene){
					case 0: if (menu_position[menu_ebene] > 1) menu_position[menu_ebene] -= 1; else menu_position[menu_ebene] = 5; break;
					case 1: if (menu_position[menu_ebene] > 1) menu_position[menu_ebene] -= 1; else menu_position[menu_ebene] = 4; break;
					case 2: if (menu_position[menu_ebene] > 1) menu_position[menu_ebene] -= 1; else menu_position[menu_ebene] = 3; break;
					case 3: if (menu_position[menu_ebene] > 1) menu_position[menu_ebene] -= 1; else menu_position[menu_ebene] = 6; break;
					}
					break;
				case SDLK_DOWN:
					switch (menu_ebene){
					case 0: if (menu_position[menu_ebene] < 5) menu_position[menu_ebene] += 1; else menu_position[menu_ebene] = 1; break;
					case 1: if (menu_position[menu_ebene] < 4) menu_position[menu_ebene] += 1; else menu_position[menu_ebene] = 1; break;
					case 2: if (menu_position[menu_ebene] < 3) menu_position[menu_ebene] += 1; else menu_position[menu_ebene] = 1; break;
					case 3: if (menu_position[menu_ebene] < 6) menu_position[menu_ebene] += 1; else menu_position[menu_ebene] = 1; break;
					}
					break;
				case SDLK_RETURN:
					switch (menu_ebene){
					case 0: //Ebene 0: Hauptmen�
						switch (menu_position[menu_ebene]){
						case 1: menu_ebene = 1; break;								//zur Spielerauswahl
						case 2: menu_ebene = 5; break;								//zu den Statistiken
						case 3: menu_ebene = 6; break;								//zu den Optionen
						case 4: menu_ebene = 4; break;								//zu den Credits
						case 5:														//Programm beenden
							isRunning = isRunning_menu = 0;
							quit = TRUE;
						}
						break;
					case 1: //Ebene 1: Spieler Auswahl
						switch (menu_position[menu_ebene]) {
						case 1: number_of_players = 2; isRunning_menu = 0; break;	//Beginn 2 Spieler
						case 2: number_of_players = 3; isRunning_menu = 0; break;	//Beginn 3 Spieler
						case 3: number_of_players = 4; menu_ebene = 2;	break;		//4 Spieler -> zu Team ja/nein
						case 4: menu_ebene--;
						}
						break;
					case 2: //Ebene 2: Team Auswahl
						switch (menu_position[menu_ebene]) {
						case 1:
							menu_ebene = 3;											//zur Teamauswahl
							for (i = 0; i < 4; i++) {
								if (position_button_teams[i+2].x == 825) Players[i].team = 2;	//aktuelles Team einstellen
								else Players[i].team = 1;
							}
							break;
						case 2: isRunning_menu = FALSE; break;						//keine Teams -> Beginn 4 Spieler
						case 3: menu_ebene--; break;
						}
						break;
					case 3:
						switch (menu_position[menu_ebene]) {
						case 5: isRunning_menu = FALSE; break;						//Spielstart
						case 6: menu_ebene--; for (i = 0; i < 4; i++) Players[i].team = i + 1; break;
						}
						break;
					case 4: menu_ebene = 0; break;
					case 5: menu_ebene = 0; break;
					case 6: menu_ebene = 0; break;
					}
					break;
				case SDLK_RIGHT:
					if (menu_ebene == 3) {
						switch (menu_position[menu_ebene]){							//Auswahl Team 2
						case 1:
							Players[0].team = player_team_pos[0] = 2;
							position_button_teams[2].x = 825;
							break;
						case 2:
							Players[1].team = player_team_pos[1] = 2;
							position_button_teams[3].x = 825;
							break;
						case 3:
							Players[2].team = player_team_pos[2] = 2;
							position_button_teams[4].x = 825;
							break;
						case 4:
							Players[3].team = player_team_pos[3] = 2;
							position_button_teams[5].x = 825;
							break;
						}
					}
					break;

				case SDLK_LEFT:
					if (menu_ebene == 3) {
						switch (menu_position[menu_ebene]){							//Auswahl Team 2
						case 1:
							Players[0].team = player_team_pos[0] = 1;
							position_button_teams[2].x = 375;
							break;
						case 2:
							Players[1].team = player_team_pos[1] = 1;
							position_button_teams[3].x = 375;
							break;
						case 3:
							Players[2].team = player_team_pos[2] = 1;
							position_button_teams[4].x = 375;
							break;
						case 4:
							Players[3].team = player_team_pos[3] = 1;
							position_button_teams[5].x = 375;
							break;
						}
					}
					break;

				case SDLK_ESCAPE:

					switch (menu_ebene){											//Ein Men� zur�ck
					case 1: menu_ebene--; break;
					case 2: menu_ebene--; break;
					case 3: menu_ebene--; for (i = 0; i < 4; i++) Players[i].team = i + 1; break;
					case 4: menu_ebene = 0; break;
					case 5: menu_ebene = 0; break;
					case 6: menu_ebene = 0; break;
					}
					break;
				}
			}
		}

//--------------------------------------------------------------------------------------------------------------------------------DARSTELLUNG

		//Clear und Hintergrundbild f�r alle
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture_menu_background, NULL, NULL);
		switch (menu_ebene) {
		case 0:
			SDL_RenderCopy(renderer, texture_menu_1[0], &rect_button, &position_button[0]);
			SDL_RenderCopy(renderer, texture_menu_2[0], &rect_button, &position_button[1]);
			SDL_RenderCopy(renderer, texture_menu_3[0], &rect_button, &position_button[2]);
			SDL_RenderCopy(renderer, texture_menu_4[0], &rect_button, &position_button[3]);
			SDL_RenderCopy(renderer, texture_menu_close[0], &rect_button, &position_button[4]);
			switch (menu_position[menu_ebene]) {
			case 1: SDL_RenderCopy(renderer, texture_menu_1[1], &rect_button, &position_button[0]);	break;
			case 2:	SDL_RenderCopy(renderer, texture_menu_2[1], &rect_button, &position_button[1]);	break;
			case 3:	SDL_RenderCopy(renderer, texture_menu_3[1], &rect_button, &position_button[2]);	break;
			case 4:	SDL_RenderCopy(renderer, texture_menu_4[1], &rect_button, &position_button[3]);	break;
			case 5:	SDL_RenderCopy(renderer, texture_menu_close[1], &rect_button, &position_button[4]);	break;
			}
			break;
		case 1:
			SDL_RenderCopy(renderer, texture_menu_player[0], &rect_button, &position_button[0]);
			SDL_RenderCopy(renderer, texture_menu_player[2], &rect_button, &position_button[1]);
			SDL_RenderCopy(renderer, texture_menu_player[4], &rect_button, &position_button[2]);
			SDL_RenderCopy(renderer, texture_menu_back[0], &rect_button3, &position_button_back);
			switch (menu_position[menu_ebene]){
			case 1:	SDL_RenderCopy(renderer, texture_menu_player[1], &rect_button, &position_button[0]); break;
			case 2:	SDL_RenderCopy(renderer, texture_menu_player[3], &rect_button, &position_button[1]); break;
			case 3:	SDL_RenderCopy(renderer, texture_menu_player[5], &rect_button, &position_button[2]); break;
			case 4: SDL_RenderCopy(renderer, texture_menu_back[1], &rect_button3, &position_button_back); break;
			}
			break;
		case 2:
			SDL_RenderCopy(renderer, texture_menu_team[0], &rect_button, &position_button[0]);
			SDL_RenderCopy(renderer, texture_menu_team[2], &rect_button, &position_button[1]);
			SDL_RenderCopy(renderer, texture_menu_back[0], &rect_button3, &position_button_back);
			switch (menu_position[menu_ebene]){
			case 1: SDL_RenderCopy(renderer, texture_menu_team[1], &rect_button, &position_button[0]); break;
			case 2:	SDL_RenderCopy(renderer, texture_menu_team[3], &rect_button, &position_button[1]); break;
			case 3: SDL_RenderCopy(renderer, texture_menu_back[1], &rect_button3, &position_button_back); break;
			}
			break;
		case 3:
			SDL_RenderCopy(renderer, texture_menu_team1_2[0], &rect_button2, &position_button_teams[0]);
			SDL_RenderCopy(renderer, texture_menu_team1_2[1], &rect_button2, &position_button_teams[1]);
			SDL_RenderCopy(renderer, texture_menu_team1_2[2], &rect_button2, &position_button_teams[2]);
			SDL_RenderCopy(renderer, texture_menu_team1_2[4], &rect_button2, &position_button_teams[3]);
			SDL_RenderCopy(renderer, texture_menu_team1_2[6], &rect_button2, &position_button_teams[4]);
			SDL_RenderCopy(renderer, texture_menu_team1_2[8], &rect_button2, &position_button_teams[5]);
			SDL_RenderCopy(renderer, texture_menu_start[0], &rect_button, &position_button[4]);
			SDL_RenderCopy(renderer, texture_menu_back[0], &rect_button3, &position_button_back);
			switch (menu_position[menu_ebene]){
			case 1: SDL_RenderCopy(renderer, texture_menu_team1_2[3], &rect_button2, &position_button_teams[2]); break;
			case 2: SDL_RenderCopy(renderer, texture_menu_team1_2[5], &rect_button2, &position_button_teams[3]); break;
			case 3: SDL_RenderCopy(renderer, texture_menu_team1_2[7], &rect_button2, &position_button_teams[4]); break;
			case 4: SDL_RenderCopy(renderer, texture_menu_team1_2[9], &rect_button2, &position_button_teams[5]); break;
			case 5: SDL_RenderCopy(renderer, texture_menu_start[1], &rect_button, &position_button[4]); break;
			case 6: SDL_RenderCopy(renderer, texture_menu_back[1], &rect_button3, &position_button_back); break;
			}
			break;
		case 4:
			SDL_RenderCopy(renderer, texture_menu_credits, NULL, NULL);
			SDL_RenderCopy(renderer, texture_menu_back[0], &rect_button3, &position_button_back);
			switch (menu_position[menu_ebene]) {
			case 1: SDL_RenderCopy(renderer, texture_menu_back[1], &rect_button3, &position_button_back); break;
			}
			break;
		case 5:
			SDL_RenderCopy(renderer, texture_menu_back[0], &rect_button3, &position_button_back);
			switch (menu_position[menu_ebene]) {
			case 1: SDL_RenderCopy(renderer, texture_menu_back[1], &rect_button3, &position_button_back); break;
			}
			break;
		case 6:
			SDL_RenderCopy(renderer, texture_menu_back[0], &rect_button3, &position_button_back);
			switch (menu_position[menu_ebene]) {
			case 1: SDL_RenderCopy(renderer, texture_menu_back[1], &rect_button3, &position_button_back); break;
			}
			break;
		}
		SDL_RenderPresent(renderer);
	}

//----------------------------------------------------------------------------------------------------------------------------MEN�ENDE

	//Spielfeld Initialisierung und Deklarierung
	char gamefield[13][15] =
	{
		{ "sssssssssssssss" },
		{ "s---       ---s" },
		{ "s-s-s s s s-s-s" },
		{ "s--         --s" },
		{ "s s s s s s s s" },
		{ "s             s" },
		{ "s s s s s s s s" },
		{ "s             s" },
		{ "s s s s s s s s" },
		{ "s--         --s" },
		{ "s-s-s s s s-s-s" },
		{ "s---       ---s" },
		{ "sssssssssssssss" }
	};

	if (isRunning == TRUE) {
		//Player Deklarieren nach bestimmten Eigenschaften
		for (i = 0; i < number_of_players; i++) {
			Players[i].player = i;
			Players[i].lifes = DEFAULT_LIFE;
			Players[i].explosion_size = DEFAULT_EXPLOSION_SIZE;
			Players[i].speed = DEFAULT_SPEED;
			Players[i].max_bomb = DEFAULT_MAXBOMB;
			Players[i].skill = DEFAULT_SKILL;
			Players[i].score = 0;
			Players[i].placed_bomb = 0;
			Players[i].hit_player = 0;
			Players[i].invulnerable = 0;
		}

		//Start positionen der Player im System & an der Overfl�che
		start_position(pPlayers, position_player, number_of_players);
		//Kisten im Spielfeld erstellen
		if (CASE == TRUE) create_case(&gamefield, CHANCE_CASE);
		//Powerups im Spielfeld erstellen
		if (POWERUP == TRUE) create_powerups(&gamefield, CHANCE_SPEED, CHANCE_MAXBOMB, CHANCE_BOMBSIZE, CHANCE_LIFE, CHANCE_SKILL);
		//Startpositionen freigeben
		clear_start(&gamefield);
		//Spielfeld in der Konsole anzeigen lassen
		print_gamefield(&gamefield);

		//Team check
		if (Players[3].team != 4) team = TRUE;

		//Alle Arrays vor Spielstart leeren
		for (int i = 1; i <= 11; i++) {
			for (int j = 0; j <= 13; j++) {
				bomb[i][j] = 0;
				bomb_move[i][j] = 0;
				fire[i][j] = 0;
				shiftx[i][j] = 0;
				shifty[i][j] = 0;
				powerup[i][j] = 0;
			}
		}

		//Musik Starten
		Mix_VolumeMusic(20);
		if (MUSIC == TRUE) Mix_FadeInMusic(playback_mus, -1, 4000);
	}
//----------------------------------------------------------------------------------------------------------------------------EINGABESTART
	//Anfang Spiel
	while (isRunning){
		starting_tick = SDL_GetTicks();

		//Keydown Event f�r Bombelegung
		while (SDL_PollEvent(&event)){ //Zum schlie�en der Anwendung
			if (event.type == SDL_QUIT){
				isRunning = 0;
				quit = TRUE;
			}
			else if (event.type == SDL_KEYDOWN) {									//Nicht m�glich wenn man gerade getroffen wurde
				switch (event.key.keysym.sym){
				case SDLK_m: if (Players[0].lifes > 0) if (Players[0].invulnerable >= FPS / 2 || Players[0].invulnerable == 0)
					create_bomb(&gamefield, &bomb, pPlayers, Players[0].player); break;
				case SDLK_q: if (Players[1].lifes > 0) if (Players[1].invulnerable >= FPS / 2 || Players[1].invulnerable == 0)
					create_bomb(&gamefield, &bomb, pPlayers, Players[1].player); break;
				case SDLK_3: if (Players[2].lifes > 0) if (Players[2].invulnerable >= FPS / 2 || Players[2].invulnerable == 0)
					create_bomb(&gamefield, &bomb, pPlayers, Players[2].player); break;
				case SDLK_8: if (Players[3].lifes > 0) if (Players[3].invulnerable >= FPS / 2 || Players[3].invulnerable == 0)
					create_bomb(&gamefield, &bomb, pPlayers, Players[3].player); break;

				case SDLK_1: if (SOUNDS == TRUE) Mix_PlayChannel(-1, saying1_mus, 0); break;		//Kampfspruch
				case SDLK_ESCAPE:																	//Spiel verlassen
					menu_ebene = 0;
					isRunning_menu = TRUE;
					for (i = 0; i < 4; i++) Players[i].team = i + 1;
					goto POS_MENU_START;															//Zur�ck ins Men�
					break;

				}
			}

			//Richtung auf 0 setzen -> Bewegung nur bei aktives dr�cken
			for (i = 0; i <4; i++) Players[i].direction = 0;

			//Bewegungen per KeyStates
			keyState = SDL_GetKeyboardState(NULL);

			//Steuerung Player0
			if (Players[0].lifes > 0) {
				if (keyState[SDL_SCANCODE_UP]) Players[0].direction = 1;
				else if (keyState[SDL_SCANCODE_RIGHT]) Players[0].direction = 2;
				else if (keyState[SDL_SCANCODE_DOWN]) Players[0].direction = 3;
				else if (keyState[SDL_SCANCODE_LEFT]) Players[0].direction = 4;
			}

			//Steuerung Player1
			if (Players[1].lifes > 0) {
				if (keyState[SDL_SCANCODE_W]) Players[1].direction = 1;
				else if (keyState[SDL_SCANCODE_D]) Players[1].direction = 2;
				else if (keyState[SDL_SCANCODE_S]) Players[1].direction = 3;
				else if (keyState[SDL_SCANCODE_A]) Players[1].direction = 4;
			}

			//Steuerung Player2
			if (Players[2].lifes > 0) {
				if (keyState[SDL_SCANCODE_4]) Players[2].direction = 1;
				else if (keyState[SDL_SCANCODE_T]) Players[2].direction = 2;
				else if (keyState[SDL_SCANCODE_R]) Players[2].direction = 3;
				else if (keyState[SDL_SCANCODE_E]) Players[2].direction = 4;
			}

			//Steuerung Player3
			if (Players[3].lifes > 0) {
				if (keyState[SDL_SCANCODE_9]) Players[3].direction = 1;
				else if (keyState[SDL_SCANCODE_P]) Players[3].direction = 2;
				else if (keyState[SDL_SCANCODE_O]) Players[3].direction = 3;
				else if (keyState[SDL_SCANCODE_I]) Players[3].direction = 4;
			}
		}

//----------------------------------------------------------------------------------------------------------------------------EINGABEENDE

		//Bewegungen
		moving(pPlayers, number_of_players, &gamefield, &bomb_move, &position_player);
		//Items-einsammeln
		player_check(&gamefield, pPlayers, number_of_players, item_mus, &powerup);
		//Bombenbewegung animieren
		moving_bomb(pPlayers, &gamefield, &bomb, &bomb_move, &shiftx, &shifty);
		//Bombenexplosion
		explosion(&gamefield, &bomb, &fire, &powerup, number_of_players, pPlayers, hit_mus, end_mus, bomb_explosion_mus, fuse_mus, &position_player_dead);

//----------------------------------------------------------------------------------------------------------------------------NEUEBILDERLADEN

		//SDL Bild leeren
		SDL_RenderClear(renderer);
		//Hintergrundbild anzeigen lassen
		SDL_RenderCopy(renderer, texture_background, NULL, NULL);
		show_case(&gamefield, renderer, texture_case, rect_pixel, position_item);
		show_item(&gamefield, renderer, texture_life, rect_pixel, position_item, clife);
		show_item(&gamefield, renderer, texture_speed, rect_pixel, position_item, cspeed);
		show_item(&gamefield, renderer, texture_maxbomb, rect_pixel, position_item, cmaxbomb);
		show_item(&gamefield, renderer, texture_bombsize, rect_pixel, position_item, cbombsize);
		show_item(&gamefield, renderer, texture_skill, rect_pixel, position_item, cskill);
		show_bomb(&gamefield, renderer, texture_bomb, rect_pixel, position_item, &bomb, &shiftx, &shifty);
		show_fire(&gamefield, renderer, texture_explosion, rect_pixel, position_item, &fire);
		show_player(renderer, texture_player, texture_rip, rect_pixel, position_player, position_player_dead, pPlayers, number_of_players);
		show_level(renderer, texture_number, rect_pixel, position_level, pPlayers, number_of_players, team);
		show_score(renderer, texture_number, rect_pixel, position_score, pPlayers, number_of_players);
		show_item_display(renderer, texture_item_display, rect_item_display, position_item_display, number_of_players);
		//Aktuelles Bild anzeigen lassen
		SDL_RenderPresent(renderer);

//--------------------------------------------------------------------------------------------------------------------------------ANZEIGEENDE

		//Pr�fen wie viele Spieler noch am Leben sind
		switch (check_dead(pPlayers, number_of_players, team)) {
		case 1: isRunning = FALSE; SDL_Delay(3000); if (SOUNDS == TRUE) Mix_PlayChannel(-1, end_mus, 0); break;
		case 2: isRunning = FALSE; SDL_Delay(3000); if (SOUNDS == TRUE) Mix_PlayChannel(-1, end_mus, 0); break;
		default: break;
		}
		//Delay f�r die Framerate
		framerate(starting_tick);
	}

	//Programm Ende -> Alles leeren!
	//Free Musik
	Mix_FreeMusic(playback_mus);
	Mix_FreeMusic(bomb_explosion_mus);
	Mix_FreeMusic(item_mus);
	Mix_FreeMusic(hit_mus);
	Mix_FreeMusic(end_mus);
	Mix_FreeMusic(fuse_mus);
	Mix_FreeMusic(saying1_mus);
	Mix_CloseAudio();

	//Free Images
	SDL_DestroyTexture(texture_menu_background);
	SDL_DestroyTexture(texture_menu_credits);
	SDL_DestroyTexture(texture_menu_1);
	SDL_DestroyTexture(texture_menu_2);
	SDL_DestroyTexture(texture_menu_3);
	SDL_DestroyTexture(texture_menu_4);
	SDL_DestroyTexture(texture_menu_player);
	SDL_DestroyTexture(texture_menu_team);
	SDL_DestroyTexture(texture_menu_team1_2);
	SDL_DestroyTexture(texture_menu_close);
	SDL_DestroyTexture(texture_menu_back);
	SDL_DestroyTexture(texture_menu_start);
	SDL_DestroyTexture(texture_background);
	SDL_DestroyTexture(texture_bomb);
	SDL_DestroyTexture(texture_player);
	SDL_DestroyTexture(texture_case);
	SDL_DestroyTexture(texture_life);
	SDL_DestroyTexture(texture_speed);
	SDL_DestroyTexture(texture_maxbomb);
	SDL_DestroyTexture(texture_bombsize);
	SDL_DestroyTexture(texture_skill);
	SDL_DestroyTexture(texture_explosion);
	SDL_DestroyTexture(texture_rip);
	SDL_DestroyTexture(texture_number);
	SDL_DestroyTexture(texture_item_display);

	//Fenster zerst�ren
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	//Neustart des Programms
	if (quit == FALSE) {
		get_score(pPlayers, number_of_players, team);
		getchar();
		//Konsolenanzeige
		isRunning_menu = isRunning = 1;
		menu_ebene = 0;
		goto POS_START;
	}

	//Beenden von SDL
	SDL_Quit();

	return(0);
}
