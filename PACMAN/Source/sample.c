/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            The GLCD application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-11-7
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             Paolo Bernardi
** Modified date:           03/01/2020
** Version:                 v2.0
** Descriptions:            basic program for LCD and Touch Panel teaching
**
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "LPC17xx.h"
#include "GLCD/GLCD.h" 
#include "TouchPanel/TouchPanel.h"
#include "timer/timer.h"
#include "RIT/RIT.h"
#include "button_EXINT/button.h"
#include "joystick/joystick.h"
#include "../sample.h"
#include <stdio.h>
#include <stdlib.h>
#include "music/music.h"
#include "CAN/CAN.h"





#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif




volatile int matrice[R][C];//=0 muro, =1 pill, =2 superpill, =3 vuoto
volatile int score=0;
volatile int n=1;
volatile int lives=1;
volatile int pacman_position_x, pacman_position_y;
volatile int ghost_position_x, ghost_position_y;
volatile int countdown=60,countdown_ghost=0x5AE5A740;//61s
volatile int game_pause=1;
volatile int game_over=0;
volatile int countpill=240;
volatile int nrand;
volatile int npowerpill=0;
volatile int nextX=0, nextY=0;
volatile int away_point_x,away_point_y;
volatile int visited[R][C];
volatile Point queue[R*C];
volatile Point parent[R][C];
volatile int ghost_mode=Red;
volatile int semaforo=0;



volatile NOTE background[] = 
{
	{NOTE_E4, time_semiminima},
	{NOTE_G4, time_semiminima},
	{NOTE_C5, time_semiminima}, 
	{NOTE_G4, time_semiminima},  
	{NOTE_E4, time_semiminima},
	{NOTE_G4, time_semiminima},
	{NOTE_C5, time_semiminima},
	{NOTE_G4, time_semiminima}, 
	{NOTE_E4, time_croma},
	{NOTE_F4, time_croma},
	{NOTE_FS4, time_croma},
	{NOTE_G4, time_semiminima},
	{NOTE_E4, time_croma},
	{NOTE_F4, time_croma},
	{NOTE_FS4, time_croma},
	{NOTE_G4, time_semiminima}
	
};

volatile NOTE waka[] = 
{
	{NOTE_B4, time_croma},
	{NOTE_C5, time_croma},
	{NOTE_B4, time_croma},
	{NOTE_A4, time_croma},  
	{NOTE_G4, time_croma},
	{NOTE_A4, time_croma},
	{NOTE_G4, time_croma},
	{NOTE_F4, time_croma},
	{NOTE_B4, time_croma},
	{NOTE_C5, time_croma},
	{NOTE_B4, time_croma},
	{NOTE_A4, time_croma},  
	{NOTE_G4, time_croma},
	{NOTE_A4, time_croma},
	{NOTE_G4, time_croma},
	{NOTE_F4, time_croma}
};

volatile NOTE dead[] = 
{
	{NOTE_E4, time_semicroma},
	{NOTE_DS4, time_semicroma},
	{NOTE_D4, time_semicroma},
	{NOTE_CS4, time_semicroma},  
	{NOTE_C4, time_semicroma},
	{NOTE_B3, time_semicroma},
	{NOTE_AS3, time_semicroma},
	{NOTE_A3, time_semicroma},  
	{NOTE_GS3, time_semicroma},
	{NOTE_G3, time_semicroma},
	{NOTE_FS3, time_semicroma},
	{NOTE_F3, time_semicroma},  
	{NOTE_E3, time_semicroma},
	{NOTE_DS3, time_semicroma},
	{NOTE_D3, time_semicroma},
	{NOTE_C3, time_semicroma}
};



volatile NOTE song[16];


void init_matrice(void){
		matrice[0][0]  = 0;  matrice[0][1]  = 0;  matrice[0][2]  = 0;  matrice[0][3]  = 0;  
    matrice[0][4]  = 0;  matrice[0][5]  = 0;  matrice[0][6]  = 0;  matrice[0][7]  = 0;  
    matrice[0][8]  = 0;  matrice[0][9]  = 0;  matrice[0][10]  = 0; matrice[0][11]  = 0; 
    matrice[0][12]  = 0; matrice[0][13]  = 0; matrice[0][14]  = 0; matrice[0][15]  = 0;
		matrice[0][16]  = 0; matrice[0][17]  = 0; matrice[0][18]  = 0; matrice[0][19]  = 0;
		matrice[0][20]  = 0;

    matrice[1][0]  = 0;  matrice[1][1]  = 1;  matrice[1][2]  = 1;  matrice[1][3]  = 1;  
    matrice[1][4]  = 1;  matrice[1][5]  = 1;  matrice[1][6]  = 1;  matrice[1][7]  = 1;  
    matrice[1][8]  = 1;  matrice[1][9]  = 1;  matrice[1][10]  = 1; matrice[1][11]  = 1; 
    matrice[1][12]  = 1; matrice[1][13]  = 1; matrice[1][14]  = 1; matrice[1][15]  = 1;
		matrice[1][16]  = 1; matrice[1][17]  = 1; matrice[1][18]  = 1; matrice[1][19]  = 1;
		matrice[1][20]  = 0;

		matrice[2][0]  = 0;  matrice[2][1]  = 1;  matrice[2][2]  = 0;  matrice[2][3]  = 0;  
    matrice[2][4]  = 0;  matrice[2][5]  = 0;  matrice[2][6]  = 1;  matrice[2][7]  = 0;
    matrice[2][8]  = 0;  matrice[2][9]  = 0;  matrice[2][10]  = 1; matrice[2][11]  = 0;
    matrice[2][12]  = 0; matrice[2][13]  = 0; matrice[2][14]  = 1; matrice[2][15]  = 0;
		matrice[2][16]  = 0; matrice[2][17]  = 0; matrice[2][18]  = 0; matrice[2][19]  = 1;
		matrice[2][20]  = 0;

    matrice[3][0]  = 0;  matrice[3][1]  = 1;  matrice[3][2]  = 0;  matrice[3][3]  = 0;
    matrice[3][4]  = 0;  matrice[3][5]  = 0;  matrice[3][6]  = 1;  matrice[3][7]  = 0;
    matrice[3][8]  = 0;  matrice[3][9]  = 0;  matrice[3][10]  = 1; matrice[3][11]  = 0;
    matrice[3][12]  = 0; matrice[3][13]  = 0; matrice[3][14]  = 1; matrice[3][15]  = 0;
		matrice[3][16]  = 0; matrice[3][17]  = 0; matrice[3][18]  = 0; matrice[3][19]  = 1;
		matrice[3][20]  = 0;

    matrice[4][0]  = 0;  matrice[4][1]  = 1;  matrice[4][2]  = 0;  matrice[4][3]  = 0;  
    matrice[4][4]  = 0;  matrice[4][5]  = 0;  matrice[4][6]  = 1;  matrice[4][7]  = 0;
    matrice[4][8]  = 0;  matrice[4][9]  = 0;  matrice[4][10]  = 1; matrice[4][11]  = 0;
    matrice[4][12]  = 0; matrice[4][13]  = 0; matrice[4][14]  = 1; matrice[4][15]  = 0;
		matrice[4][16]  = 0; matrice[4][17]  = 0; matrice[4][18]  = 0; matrice[4][19]  = 1;
		matrice[4][20]  = 0;

    matrice[5][0]  = 0;  matrice[5][1]  = 1;  matrice[5][2]  = 1;  matrice[5][3]  = 1;
    matrice[5][4]  = 1;  matrice[5][5]  = 1;  matrice[5][6]  = 1;  matrice[5][7]  = 1;
    matrice[5][8]  = 1;  matrice[5][9]  = 1;  matrice[5][10]  = 1; matrice[5][11]  = 1;
    matrice[5][12]  = 1; matrice[5][13]  = 1; matrice[5][14]  = 1; matrice[5][15]  = 1;
		matrice[5][16]  = 1; matrice[5][17]  = 1; matrice[5][18]  = 1; matrice[5][19]  = 1;
		matrice[5][20]  = 0;

    matrice[6][0]  = 0;  matrice[6][1]  = 0;  matrice[6][2]  = 0;  matrice[6][3]  = 0;
    matrice[6][4]  = 0;  matrice[6][5]  = 0;  matrice[6][6]  = 0;  matrice[6][7]  = 0;
    matrice[6][8]  = 1;  matrice[6][9]  = 0;  matrice[6][10]  = 0; matrice[6][11]  = 0;
    matrice[6][12]  = 1; matrice[6][13]  = 0; matrice[6][14]  = 1; matrice[6][15]  = 0;
		matrice[6][16]  = 0; matrice[6][17]  = 0; matrice[6][18]  = 0; matrice[6][19]  = 1;
		matrice[6][20]  = 0;

    matrice[7][0]  = 0;  matrice[7][1]  = 0;  matrice[7][2]  = 0;  matrice[7][3]  = 0;
    matrice[7][4]  = 1;  matrice[7][5]  = 1;  matrice[7][6]  = 0;  matrice[7][7]  = 0;
    matrice[7][8]  = 1;  matrice[7][9]  = 1;  matrice[7][10]  = 1; matrice[7][11]  = 1;
    matrice[7][12]  = 1; matrice[7][13]  = 0; matrice[7][14]  = 1; matrice[7][15]  = 1;
		matrice[7][16]  = 1; matrice[7][17]  = 1; matrice[7][18]  = 1; matrice[7][19]  = 1;
		matrice[7][20]  = 0;
		
		matrice[8][0]  = 0;  matrice[8][1]  = 1;  matrice[8][2]  = 1;  matrice[8][3]  = 1;  
    matrice[8][4]  = 1;  matrice[8][5]  = 1;  matrice[8][6]  = 1;  matrice[8][7]  = 0;
    matrice[8][8]  = 0;  matrice[8][9]  = 0;  matrice[8][10]  = 1; matrice[8][11]  = 1;
    matrice[8][12]  = 0; matrice[8][13]  = 0; matrice[8][14]  = 1; matrice[8][15]  = 0;
		matrice[8][16]  = 0; matrice[8][17]  = 0; matrice[8][18]  = 0; matrice[8][19]  = 0;
		matrice[8][20]  = 0;

    matrice[9][0]  = 0;  matrice[9][1]  = 1;  matrice[9][2]  = 0;  matrice[9][3]  = 0;  
    matrice[9][4]  = 0;  matrice[9][5]  = 0;  matrice[9][6]  = 0;  matrice[9][7]  = 0;  
    matrice[9][8]  = 0;  matrice[9][9]  = 0;  matrice[9][10]  = 1; matrice[9][11]  = 1;
    matrice[9][12]  = 1; matrice[9][13]  = 0; matrice[9][14]  = 1; matrice[9][15]  = 0;
		matrice[9][16]  = 0; matrice[9][17]  = 0; matrice[9][18]  = 0; matrice[9][19]  = 0;
		matrice[9][20]  = 0;

    matrice[10][0]  = 0; matrice[10][1]  = 1; matrice[10][2]  = 1; matrice[10][3]  = 1;  
    matrice[10][4]  = 1; matrice[10][5]  = 1; matrice[10][6]  = 1; matrice[10][7]  = 1;
    matrice[10][8]  = 1; matrice[10][9]  = 1; matrice[10][10]  = 1; matrice[10][11]  = 1;
    matrice[10][12]  = 1; matrice[10][13]  = 1; matrice[10][14]  = 1; matrice[10][15]  = 1;
		matrice[10][16]  = 1; matrice[10][17]  = 1; matrice[10][18]  = 1; matrice[10][19]  = 1;
		matrice[10][20]  = 0;

    matrice[11][0]  = 0; matrice[11][1]  = 0; matrice[11][2]  = 0; matrice[11][3]  = 1;  
    matrice[11][4]  = 0; matrice[11][5]  = 1; matrice[11][6]  = 1; matrice[11][7]  = 1;  
    matrice[11][8]  = 0; matrice[11][9]  = 0; matrice[11][10]  = 3; matrice[11][11]  = 0;
    matrice[11][12]  = 0; matrice[11][13]  = 0; matrice[11][14]  = 0; matrice[11][15]  = 0;
		matrice[11][16]  = 0; matrice[11][17]  = 0; matrice[11][18]  = 0; matrice[11][19]  = 1;
		matrice[11][20]  = 0;

    matrice[12][0]  = 0; matrice[12][1]  = 0; matrice[12][2]  = 0; matrice[12][3]  = 1;  
    matrice[12][4]  = 0; matrice[12][5]  = 1; matrice[12][6]  = 0; matrice[12][7]  = 1;
    matrice[12][8]  = 0; matrice[12][9]  = 3; matrice[12][10]  = 3; matrice[12][11]  = 3;
    matrice[12][12]  = 0; matrice[12][13]  = 0; matrice[12][14]  = 1; matrice[12][15]  = 1;
		matrice[12][16]  = 1; matrice[12][17]  = 1; matrice[12][18]  = 1; matrice[12][19]  = 1;
		matrice[12][20]  = 0;

    matrice[13][0]  = 0; matrice[13][1]  = 0; matrice[13][2]  = 0; matrice[13][3]  = 1;  
    matrice[13][4]  = 0; matrice[13][5]  = 1; matrice[13][6]  = 1; matrice[13][7]  = 1;
    matrice[13][8]  = 0; matrice[13][9]  = 3; matrice[13][10]  = 3; matrice[13][11]  = 3;
    matrice[13][12]  = 0; matrice[13][13]  = 0; matrice[13][14]  = 1; matrice[13][15]  = 0;
		matrice[13][16]  = 0; matrice[13][17]  = 1; matrice[13][18]  = 1; matrice[13][19]  = 0;
		matrice[13][20]  = 0;

    matrice[14][0]  = 0; matrice[14][1]  = 0; matrice[14][2]  = 0; matrice[14][3]  = 1;  
    matrice[14][4]  = 0; matrice[14][5]  = 0; matrice[14][6]  = 0; matrice[14][7]  = 0;  
    matrice[14][8]  = 0; matrice[14][9]  = 0; matrice[14][10]  = 0; matrice[14][11]  = 0;
    matrice[14][12]  = 0; matrice[14][13]  = 0; matrice[14][14]  = 1; matrice[14][15]  = 0;
		matrice[14][16]  = 0; matrice[14][17]  = 0; matrice[14][18]  = 0; matrice[14][19]  = 0;
		matrice[14][20]  = 0;

    matrice[15][0]  = 0; matrice[15][1]  = 0; matrice[15][2]  = 0; matrice[15][3]  = 1;  
    matrice[15][4]  = 0; matrice[15][5]  = 0; matrice[15][6]  = 1; matrice[15][7]  = 1;
    matrice[15][8]  = 1; matrice[15][9]  = 0; matrice[15][10]  = 3; matrice[15][11]  = 0;
    matrice[15][12]  = 0; matrice[15][13]  = 0; matrice[15][14]  = 1; matrice[15][15]  = 0;
		matrice[15][16]  = 0; matrice[15][17]  = 0; matrice[15][18]  = 0; matrice[15][19]  = 0;
		matrice[15][20]  = 0;

    matrice[16][0]  = 1; matrice[16][1]  = 1; matrice[16][2]  = 1; matrice[16][3]  = 1;  
    matrice[16][4]  = 1; matrice[16][5]  = 1; matrice[16][6]  = 1; matrice[16][7]  = 0;
    matrice[16][8]  = 1; matrice[16][9]  = 1; matrice[16][10]  = 1; matrice[16][11]  = 1;
    matrice[16][12]  = 1; matrice[16][13]  = 1; matrice[16][14]  = 1; matrice[16][15]  = 1;
		matrice[16][16]  = 1; matrice[16][17]  = 1; matrice[16][18]  = 1; matrice[16][19]  = 1;
		matrice[16][20]  = 1;

    matrice[17][0]  = 0; matrice[17][1]  = 0; matrice[17][2]  = 0; matrice[17][3]  = 0;  
    matrice[17][4]  = 0; matrice[17][5]  = 1; matrice[17][6]  = 0; matrice[17][7]  = 0;
    matrice[17][8]  = 0; matrice[17][9]  = 1; matrice[17][10]  = 0; matrice[17][11]  = 0;
    matrice[17][12]  = 0; matrice[17][13]  = 0; matrice[17][14]  = 1; matrice[17][15]  = 0;
		matrice[17][16]  = 0; matrice[17][17]  = 0; matrice[17][18]  = 0; matrice[17][19]  = 0;
		matrice[17][20]  = 0;

    matrice[18][0]  = 0; matrice[18][1]  = 1; matrice[18][2]  = 1; matrice[18][3]  = 1;  
    matrice[18][4]  = 1; matrice[18][5]  = 1; matrice[18][6]  = 1; matrice[18][7]  = 1;
    matrice[18][8]  = 0; matrice[18][9]  = 1; matrice[18][10]  = 0; matrice[18][11]  = 0;
    matrice[18][12]  = 0; matrice[18][13]  = 0; matrice[18][14]  = 1; matrice[18][15]  = 1;
		matrice[18][16]  = 1; matrice[18][17]  = 1; matrice[18][18]  = 1; matrice[18][19]  = 0;
		matrice[18][20]  = 0;

    matrice[19][0]  = 0; matrice[19][1]  = 1; matrice[19][2]  = 0; matrice[19][3]  = 0;  
    matrice[19][4]  = 1; matrice[19][5]  = 0; matrice[19][6]  = 0; matrice[19][7]  = 1;  
    matrice[19][8]  = 0; matrice[19][9]  = 1; matrice[19][10]  = 0; matrice[19][11]  = 0;
    matrice[19][12]  = 0; matrice[19][13]  = 0; matrice[19][14]  = 0; matrice[19][15]  = 0;
		matrice[19][16]  = 0; matrice[19][17]  = 0; matrice[19][18]  = 1; matrice[19][19]  = 0;
		matrice[19][20]  = 0;
		
		matrice[20][0]  = 0; matrice[20][1]  = 1; matrice[20][2]  = 0; matrice[20][3]  = 0;  
    matrice[20][4]  = 1; matrice[20][5]  = 0; matrice[20][6]  = 1; matrice[20][7]  = 1;  
    matrice[20][8]  = 0; matrice[20][9]  = 1; matrice[20][10]  = 1; matrice[20][11]  = 1;
    matrice[20][12]  = 1; matrice[20][13]  = 1; matrice[20][14]  = 1; matrice[20][15]  = 1;
		matrice[20][16]  = 1; matrice[20][17]  = 1; matrice[20][18]  = 1; matrice[20][19]  = 0;
		matrice[20][20]  = 0;
		
		matrice[21][0]  = 0; matrice[21][1]  = 1; matrice[21][2]  = 1; matrice[21][3]  = 1;  
    matrice[21][4]  = 1; matrice[21][5]  = 0; matrice[21][6]  = 0; matrice[21][7]  = 1;  
    matrice[21][8]  = 0; matrice[21][9]  = 0; matrice[21][10]  = 0; matrice[21][11]  = 0;
    matrice[21][12]  = 0; matrice[21][13]  = 0; matrice[21][14]  = 0; matrice[21][15]  = 1;
		matrice[21][16]  = 0; matrice[21][17]  = 0; matrice[21][18]  = 0; matrice[21][19]  = 0;
		matrice[21][20]  = 0;
		
		matrice[22][0]  = 0; matrice[22][1]  = 1; matrice[22][2]  = 0; matrice[22][3]  = 0;  
    matrice[22][4]  = 1; matrice[22][5]  = 1; matrice[22][6]  = 1; matrice[22][7]  = 1;  
    matrice[22][8]  = 0; matrice[22][9]  = 0; matrice[22][10]  = 0; matrice[22][11]  = 0;
    matrice[22][12]  = 1; matrice[22][13]  = 1; matrice[22][14]  = 1; matrice[22][15]  = 1;
		matrice[22][16]  = 1; matrice[22][17]  = 1; matrice[22][18]  = 0; matrice[22][19]  = 0;
		matrice[22][20]  = 0;
		
		matrice[23][0]  = 0; matrice[23][1]  = 1; matrice[23][2]  = 1; matrice[23][3]  = 1;  
    matrice[23][4]  = 1; matrice[23][5]  = 1; matrice[23][6]  = 1; matrice[23][7]  = 1;  
    matrice[23][8]  = 1; matrice[23][9]  = 0; matrice[23][10]  = 1; matrice[23][11]  = 1;
    matrice[23][12]  = 1; matrice[23][13]  = 0; matrice[23][14]  = 0; matrice[23][15]  = 0;
		matrice[23][16]  = 0; matrice[23][17]  = 1; matrice[23][18]  = 1; matrice[23][19]  = 1;
		matrice[23][20]  = 0;
		
		matrice[24][0]  = 0; matrice[24][1]  = 1; matrice[24][2]  = 0; matrice[24][3]  = 0;  
    matrice[24][4]  = 0; matrice[24][5]  = 0; matrice[24][6]  = 0; matrice[24][7]  = 0;  
    matrice[24][8]  = 1; matrice[24][9]  = 0; matrice[24][10]  = 1; matrice[24][11]  = 0;
    matrice[24][12]  = 0; matrice[24][13]  = 0; matrice[24][14]  = 0; matrice[24][15]  = 0;
		matrice[24][16]  = 0; matrice[24][17]  = 0; matrice[24][18]  = 0; matrice[24][19]  = 1;
		matrice[24][20]  = 0;
		
		matrice[25][0]  = 0; matrice[25][1]  = 1; matrice[25][2]  = 1; matrice[25][3]  = 1;  
    matrice[25][4]  = 1; matrice[25][5]  = 1; matrice[25][6]  = 1; matrice[25][7]  = 1;  
    matrice[25][8]  = 1; matrice[25][9]  = 1; matrice[25][10]  = 1; matrice[25][11]  = 1;
    matrice[25][12]  = 1; matrice[25][13]  = 1; matrice[25][14]  = 1; matrice[25][15]  = 1;
		matrice[25][16]  = 1; matrice[25][17]  = 1; matrice[25][18]  = 1; matrice[25][19]  = 1;
		matrice[25][20]  = 0;
		
		matrice[26][0]  = 0; matrice[26][1]  = 0; matrice[26][2]  = 0; matrice[26][3]  = 0;  
    matrice[26][4]  = 0; matrice[26][5]  = 0; matrice[26][6]  = 0; matrice[26][7]  = 0;  
    matrice[26][8]  = 0; matrice[26][9]  = 0; matrice[26][10]  = 0; matrice[26][11]  = 0;
    matrice[26][12]  = 0; matrice[26][13]  = 0; matrice[26][14]  = 0; matrice[26][15]  = 0;
		matrice[26][16]  = 0; matrice[26][17]  = 0; matrice[26][18]  = 0; matrice[26][19]  = 0;
		matrice[26][20]  = 0;
}

void writemaze(int x0, int x1,int y0, int y1){
	int i,j;
	for(i=y0;i<y1;i++){
		for(j=x0;j<x1;j++){
			switch(matrice[i][j] ){
			case 0:
				LCD_DrawFullSquare(j*L,i*L,L,Blue);
				break;
			case 1:
				LCD_DrawFullSquare(j*L,i*L,3,Magenta);
				break;
			case 2:
				LCD_DrawFullSquare(j*L,i*L,5,Magenta);
				break;
			case 3:
				LCD_DrawFullSquare(j*L,i*L,L,Black);
				break;
			default:
				break;
			}
		}
	}
}

void initgame(void){
	int i;
	uint8_t str[15];
	for(i=0;i<16;i++){
		song[i]=background[i];
	}
	writemaze(0, C, 0, R);
	pacman_position_x=10;
	pacman_position_y=15;
	ghost_position_x=10;//10
	ghost_position_y=12;//12
	LCD_DrawPacman(pacman_position_x*L,pacman_position_y*L,7,Yellow);
	LCD_DrawGhost(ghost_position_x*L,ghost_position_y*L,ghost_mode);
	
	CAN_TxMsg.data[0] = (score&0xff);
	CAN_TxMsg.data[1] = ((score>>8)&0xff);
	CAN_TxMsg.data[2] = lives;
	CAN_TxMsg.data[3] = countdown;
	CAN_TxMsg.len = 4;
	CAN_TxMsg.id = 2;
	CAN_TxMsg.format = STANDARD_FORMAT;
	CAN_TxMsg.type = DATA_FRAME;
	CAN_wrMsg (1, &CAN_TxMsg);// send from CAN1 to CAN2
	
}

//su, giù, sinistra, destra
int dy[] = {-1, 1, 0, 0};
int dx[] = {0, 0, -1, 1};

void bfs(int endX, int endY) {
	int i,j;
	int front = 0, rear = 0;

	for (i = 0; i < R; i++){
		for (j = 0; j < C; j++){
			visited[i][j] = 0;
			parent[i][j]=(Point){-1,-1};
		}
	}

	queue[rear++] = (Point){ghost_position_x, ghost_position_y};
	visited[ghost_position_y][ghost_position_x] = 1;

	while (front < rear) {
		Point current = queue[front++];
		int x = current.x, y = current.y;

		//ricostruzione percorso
		if (x == endX && y == endY) {
			int pathX = endX, pathY = endY;
			while (!(parent[pathY][pathX].x == ghost_position_x && parent[pathY][pathX].y == ghost_position_y)) {
				int tempX = parent[pathY][pathX].x;
				int tempY = parent[pathY][pathX].y;
				pathX = tempX;
				pathY = tempY;
			}
			nextX = pathX;
			nextY = pathY;
			return;
		}
		//esplorazione
		for (i = 0; i < 4; i++) {
			int newX = x + dx[i];
			int newY = y + dy[i];

			if (newX >= 0 && newX < C && newY >= 0 && newY < R && matrice[newY][newX]  != 0 && !visited[newY][newX]) {
				if(((ghost_mode==Blue2)&&(abs(newX - pacman_position_x) + abs(newY - pacman_position_y) > 1))||(ghost_mode==Red)){
					queue[rear++] = (Point){newX, newY};
					visited[newY][newX] = 1;
					parent[newY][newX] = (Point){x, y};
				}
			}
		}
	}
}

void point_away_pacman(void) {
	int i,j;
	int maxDistance = -1;
	for (i = 0; i < R; i++) {
		for (j = 0; j < C; j++) {
			if (matrice[i][j]  != 0) { 
				int distance = abs(j - pacman_position_x) + abs(i - pacman_position_y);
				if (distance > maxDistance) {
					maxDistance = distance;
					away_point_y= i;
					away_point_x = j;
				}
			}
		}
	}
}

int main(void)
{
  SystemInit();  												/* System Initialization (i.e., PLL)  */
	CAN_Init();
	BUTTON_init();												/* BUTTON Initialization              */
	joystick_init();											/* Joystick Initialization            */
	
	LPC_SC -> PCONP |= (1 << 22);  // TURN ON TIMER 2
	LPC_SC -> PCONP |= (1 << 23);  // TURN ON TIMER 3	
	
	init_RIT(0x2FAF080); //0.5s
  LCD_Initialization();
	init_matrice();
	LCD_Clear(Black);
	initgame();
	init_timer(2,0,1,1,(0x17D7840*((60-countdown)+1)));
	init_timer(3,0,0,3,0x1312D00);//0.8s

	srand(11);
	nrand=rand()%(54-0+1);
	nrand=(nrand*25000000)+0x1312D0; //nrand*f+0.05s
	init_timer(2,0,0,1,nrand);
	
	GUI_Text(95, 152, (uint8_t *) "PAUSE", Yellow, Black);
	enable_RIT();
	
	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= ~(0x2);						
	
	LPC_PINCON->PINSEL1 |= (1<<21);
	LPC_PINCON->PINSEL1 &= ~(1<<20);
	LPC_GPIO0->FIODIR |= (1<<26);
	
  while (1)	
  {
		__ASM("wfi");
  }
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
