/*
+-------------------------------------------------------------------+
|Name:			renderfuntctions.h									|
|Author:		Maurice Hoffmann									|
|Creationdate:	11.01.2017											|
|Version:		1.0													|
|Company:		TUHH												|
+-------------------------------------------------------------------+
|Changelog:															|
|11.01.2016:	-headerdatei erstellt								|
|				-													|
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

#ifndef __renderfunctions_h__
#define __renderfunctions_h__



void show_item(char gamefield[][15], SDL_Renderer* renderer, SDL_Texture* texture_item, SDL_Rect rect_pixel, SDL_Rect position_item[], char item) {
	int a;
	for (int i = 0; i <= 12; i++) {
		for (int j = 0; j <= 14; j++) {
			a = 0;
			if (gamefield[i][j] == item) {
				position_item[a].x = WIDTH + j * PIXEL;
				position_item[a].y = HEIGHT + i * PIXEL;
				SDL_RenderCopy(renderer, texture_item, &rect_pixel, &position_item[a]);
				a++;
			}
		}
	}
}

void show_case(char gamefield[][15], SDL_Renderer* renderer, SDL_Texture* texture_case, SDL_Rect rect_pixel, SDL_Rect position_item[]) {
	int a;
	for (int i = 1; i <= 11; i++) {
		for (int j = 1; j <= 13; j++) {
			a = 0;
			if (gamefield[i][j] > 's') {
				position_item[a].x = WIDTH + j * PIXEL;
				position_item[a].y = HEIGHT + i * PIXEL;
				SDL_RenderCopy(renderer, texture_case, &rect_pixel, &position_item[a]);
				a++;
			}
		}
	}
}

void show_bomb(char gamefield[][15], SDL_Renderer* renderer, SDL_Texture* texture_bomb[], SDL_Rect rect_pixel, SDL_Rect position_item[], uint16_t bomb[][14], int16_t shiftx[][14], int16_t shifty[][14]) {
	for (int i = 1; i <= 11; i++) {
		for (int j = 1; j <= 13; j++) {
			if ((bomb[i][j] >= 1) && (bomb[i][j] < FPS * 2 - FPS / 10 * 9)) {
				position_item[10].x = WIDTH + j * PIXEL + shiftx[i][j];
				position_item[10].y = HEIGHT + i * PIXEL + shifty[i][j];
				SDL_RenderCopy(renderer, texture_bomb[10], &rect_pixel, &position_item[10]);
			}
			for (int k = 1; k <= 10; k++) {
				if ((bomb[i][j] >= FPS * 2 - FPS / 10 * (10-k)) && (bomb[i][j] <= FPS * 2 - FPS / 10 * (10-k-1))) {
					position_item[k].x = WIDTH + j * PIXEL + shiftx[i][j];
					position_item[k].y = HEIGHT + i * PIXEL + shifty[i][j];
					SDL_RenderCopy(renderer, texture_bomb[k], &rect_pixel, &position_item[k]);
				}
			}
		}
	}
}

void show_fire(char gamefield[][15], SDL_Renderer* renderer, SDL_Texture* texture_explosion[], SDL_Rect rect_pixel, SDL_Rect position_item[], uint16_t fire[][14]) {
	int a;
	for (int i = 1; i <= 11; i++) {
		for (int j = 1; j <= 13; j++) {
			a = 0;
			if (fire[i][j] >= 1) {
				int fire_player = fire[i][j] % 4 - 1;
				if (fire_player < 0) fire_player = 3;
				position_item[a].x = WIDTH + j * PIXEL;
				position_item[a].y = HEIGHT + i * PIXEL;
				SDL_RenderCopy(renderer, texture_explosion[fire_player], &rect_pixel, &position_item[a]);
				a++;
			}
		}
	}
}

void show_player(SDL_Renderer* renderer, SDL_Texture* texture_player[], SDL_Texture* texture_rip[], SDL_Rect rect_pixel, SDL_Rect position_item[], SDL_Rect position_player_dead[], char* c, int p_nb) {
	player* p;
	for (int k = 0; k < p_nb; k++){
		p = (player*)(c + k * sizeof(player));
		if ((p->lifes > 0) && (p->invulnerable / 5 % 2 == 0)) SDL_RenderCopy(renderer, texture_player[k], &rect_pixel, &position_item[k]);
		if (p->lifes == 0) SDL_RenderCopy(renderer, texture_rip[k], &rect_pixel, &position_player_dead[k]);
	}
}

void show_level(SDL_Renderer* renderer, SDL_Texture* texture_number[], SDL_Rect rect_pixel, SDL_Rect position_level[][6], char* c, int p_nb, uint8_t team) {
	player* p;
	for (int k = 0; k < p_nb; k++){
		p = (player*)(c + k * sizeof(player));
		//Item anzeigen lassen
		if (p->lifes >= 4) SDL_RenderCopy(renderer, texture_number[10], &rect_pixel, &position_level[p->player][0]);
		else SDL_RenderCopy(renderer, texture_number[p->lifes], &rect_pixel, &position_level[p->player][0]);
		if (p->speed >= 4) SDL_RenderCopy(renderer, texture_number[10], &rect_pixel, &position_level[p->player][1]);
		else SDL_RenderCopy(renderer, texture_number[p->speed], &rect_pixel, &position_level[p->player][1]);
		if (p->max_bomb >= 8) SDL_RenderCopy(renderer, texture_number[10], &rect_pixel, &position_level[p->player][2]);
		else SDL_RenderCopy(renderer, texture_number[p->max_bomb], &rect_pixel, &position_level[p->player][2]);
		if (p->explosion_size >= 10) SDL_RenderCopy(renderer, texture_number[10], &rect_pixel, &position_level[p->player][3]);
		else SDL_RenderCopy(renderer, texture_number[p->explosion_size], &rect_pixel, &position_level[p->player][3]);
		if (p->skill >= 4) SDL_RenderCopy(renderer, texture_number[10], &rect_pixel, &position_level[p->player][4]);
		else SDL_RenderCopy(renderer, texture_number[p->skill], &rect_pixel, &position_level[p->player][4]);
		if (team == TRUE) {
			if (p->team == 1) SDL_RenderCopy(renderer, texture_number[11], &rect_pixel, &position_level[p->player][5]);
			else SDL_RenderCopy(renderer, texture_number[12], &rect_pixel, &position_level[p->player][5]);
		}


	}
}

void show_score(SDL_Renderer* renderer, SDL_Texture* texture_number[], SDL_Rect rect_pixel, SDL_Rect position_score[][5], char* c, int p_nb) {
	player* p; int hilf;
	for (int k = 0; k < p_nb; k++){
		p = (player*)(c + k * sizeof(player));
		if (p->score >= 10000) SDL_RenderCopy(renderer, texture_number[p->score % 100000 / 10000], &rect_pixel, &position_score[p->player][0]);
		if (p->score >= 1000) SDL_RenderCopy(renderer, texture_number[p->score % 10000 / 1000], &rect_pixel, &position_score[p->player][1]);
		if (p->score >= 100) SDL_RenderCopy(renderer, texture_number[p->score % 1000 / 100], &rect_pixel, &position_score[p->player][2]);
		if (p->score >= 10) SDL_RenderCopy(renderer, texture_number[p->score % 100 / 10], &rect_pixel, &position_score[p->player][3]);
		if (p->score >= 1) SDL_RenderCopy(renderer, texture_number[p->score % 10 / 1], &rect_pixel, &position_score[p->player][4]);
		else SDL_RenderCopy(renderer, texture_number[0], &rect_pixel, &position_score[p->player][4]);
	}
}

void show_item_display(SDL_Renderer* renderer, SDL_Texture* texture_item_display[], SDL_Rect rect_item_display, SDL_Rect position_item_display[], int p_nb) {
	for (int i = 0; i < p_nb; i++) {
		SDL_RenderCopy(renderer, texture_item_display[i], &rect_item_display, &position_item_display[i]);
	}
}

#endif
