/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "LPC17xx.h"
#include "RIT.h"
#include "../sample.h"
#include "GLCD/GLCD.h"
#include <stdio.h>
#include "timer/timer.h"
#include "music/music.h"
#include "CAN/CAN.h"





/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
volatile int down_0 = 0;
volatile int down_1 = 0;
volatile int down_2 = 0;

volatile int currentNote = 0;
volatile int ticks = 0;

extern volatile int matrice[R][C];
extern volatile int score;
extern volatile int n;
extern volatile int lives;
extern volatile int pacman_position_x, pacman_position_y;
extern volatile int game_pause;
extern volatile int game_over;
extern volatile int countpill;
extern volatile int ghost_mode;
extern volatile int countdown, countdown_ghost ;
extern volatile int ghost_position_x, ghost_position_y;
extern volatile NOTE song[16];
extern volatile NOTE background[16];
extern volatile NOTE waka[16];
extern volatile NOTE dead[16];
extern volatile int semaforo;



// beat 1/4 = 1.65/4 seconds
#define RIT_SEMIMINIMA 8
#define RIT_MINIMA 16
#define RIT_INTERA 32

#define UPTICKS 1


//SHORTENING UNDERTALE: TOO MANY REPETITIONS


void updateVariables(void){
	uint8_t str[15];
	int i;
	if((pacman_position_x==ghost_position_x) && (pacman_position_y==ghost_position_y)&&(ghost_mode==Blue2)){
		semaforo++;
		if(semaforo==1){
			disable_timer(3);
			score=score+100;		
			ghost_mode=0;
			countdown_ghost=read_timer(2)+(3*0x17D7840);
			ghost_position_x=10;
			ghost_position_y=12;
			enable_timer(3);
			if(score>=(n*1000)){
				lives++;
				n++;
			}
		}
		semaforo=0;
	}
	
	if((pacman_position_x==ghost_position_x) && (pacman_position_y==ghost_position_y)&&(ghost_mode==Red)){
		semaforo++;
		if(semaforo==1){
			lives--;
			if(lives==0){
				disable_timer(2);
				disable_timer(3);
				game_over=1;
				for(i=0;i<16;i++){
					song[i]=dead[i];
				}
				GUI_Text(84, 152, (uint8_t *) "GAME OVER!", Yellow, Black);
			}
			pacman_position_x=10;
			pacman_position_y=15;
			LCD_DrawPacman(pacman_position_x*L,pacman_position_y*L,7,Yellow);
		}
		semaforo=0;
	}
	if ((matrice[pacman_position_y][pacman_position_x] ==1)||(matrice[pacman_position_y][pacman_position_x] ==2)){
		playNote((NOTE){NOTE_A7, time_croma});
		countpill--;
		score=score+10;
		if (matrice[pacman_position_y][pacman_position_x] ==2){
			score=score+40;		
			ghost_mode=Blue2;
			countdown_ghost=read_timer(2)+(10*0x17D7840);

		}
		if(score>=(n*1000)){
			lives++;
			n++;
		}
		if(countpill==0){
			disable_timer(2);
			disable_timer(3);
			for(i=0;i<16;i++){
				song[i]=background[i];
			}
			GUI_Text(88, 152, (uint8_t *) "VICTORY!", Yellow, Black);
		}
	}

	matrice[pacman_position_y][pacman_position_x] =3;
	
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

	

void RIT_IRQHandler (void)
{			
/* Static as its value persists between calls to the function. It is not reinitialized each time the function is executed.*/
static int j_up=0;
static int j_down=0;
static int j_left=0;
static int j_right=0;
static int j_select=0;
static int input_joystick=0;//up=1,down=2,left=3,right=4

	if(!isNotePlaying())
	{
		++ticks;
		if(ticks == UPTICKS)
		{
			ticks = 0;
			playNote(song[currentNote++]);
		}
	}
	
	if(currentNote == (sizeof(song) / sizeof(song[0])))
	{
		currentNote=0;
	}

if(game_over==0){	
if(game_pause==0){
	

/*************************joystickUP***************************/
	if((LPC_GPIO1->FIOPIN & (1<<29)) == 0){	
		/* Joytick UP pressed */
		j_up++;
		switch(j_up){
			default:
				input_joystick=1;
				break;
		}
	}
	else{
			j_up=0;
	}

/*************************joystickDOWN***************************/

	if((LPC_GPIO1->FIOPIN & (1<<26)) == 0){	
		/* Joytick down pressed */
		j_down++;
		switch(j_down){
			default:
				input_joystick=2;
				break;
			
		}
	}
	else{
			j_down=0;
	}
	
	/*************************joystickLEFT***************************/

	if((LPC_GPIO1->FIOPIN & (1<<27)) == 0){	
		/* Joytick down pressed */
		j_left++;
		switch(j_left){
			default:
				input_joystick=3;
				break;
			
		}
	}
	else{
			j_left=0;
	}
	
	/*************************joystickRIGHT***************************/

	if((LPC_GPIO1->FIOPIN & (1<<28)) == 0){	
		/* Joytick down pressed */
		j_right++;
		switch(j_right){
			default:
				input_joystick=4;
				
				break;
		
		}
	}
	else{
			j_right=0;
	}
	
	/*************************joystickSELECT***************************/

	if((LPC_GPIO1->FIOPIN & (1<<25)) == 0){	
		/* Joytick down pressed */
		j_select++;
		switch(j_select){
			case 1:
				break;
			case 60:	//3sec = 3000ms/50ms = 60
			
				break;
			default:
				break;
		}
	}
	else{
			j_select=0;
	}
	
	
switch(input_joystick){
			case 1:
				if (matrice[pacman_position_y-1][pacman_position_x] !=0){
					LCD_DrawFullSquare(pacman_position_x*L,pacman_position_y*L,L,Black);
					pacman_position_y--;
					LCD_DrawPacman(pacman_position_x*L,pacman_position_y*L,7,Yellow);
					updateVariables();
				}
				break;
			case 2:
				if (matrice[pacman_position_y+1][pacman_position_x] !=0){
					LCD_DrawFullSquare(pacman_position_x*L,pacman_position_y*L,L,Black);
					pacman_position_y++;
					LCD_DrawPacman(pacman_position_x*L,pacman_position_y*L,7,Yellow);
					updateVariables();
				}
				break;
			case 3:
				if (pacman_position_x==0){
					LCD_DrawFullSquare(pacman_position_x*L,pacman_position_y*L,L,Black);
					pacman_position_x=20;
					LCD_DrawPacman(pacman_position_x*L,pacman_position_y*L,7,Yellow);
					updateVariables();
				}
				else if (matrice[pacman_position_y][pacman_position_x-1] !=0){
					LCD_DrawFullSquare(pacman_position_x*L,pacman_position_y*L,L,Black);
					pacman_position_x--;
					LCD_DrawPacman(pacman_position_x*L,pacman_position_y*L,7,Yellow);
					updateVariables();
					
				}
				break;
			case 4:
				if (pacman_position_x==20){
					LCD_DrawFullSquare(pacman_position_x*L,pacman_position_y*L,L,Black);
					pacman_position_x=0;
					LCD_DrawPacman(pacman_position_x*L,pacman_position_y*L,7,Yellow);
					updateVariables();
				}
				else if (matrice[pacman_position_y][pacman_position_x+1] !=0){
					LCD_DrawFullSquare(pacman_position_x*L,pacman_position_y*L,L,Black);
					pacman_position_x++;
					LCD_DrawPacman(pacman_position_x*L,pacman_position_y*L,7,Yellow);
					updateVariables();
					
				}
				break;
				default:
					break;
		}
	
	
	
	
}
	
/*************************INT0***************************/
if(down_0 !=0){
	down_0++;
	if((LPC_GPIO2->FIOPIN & (1<<10)) == 0){
		int i;
		switch(down_0){
			case 2:
				if(game_pause==0){
					disable_timer(2);
					disable_timer(3);
					GUI_Text(95, 152, (uint8_t *) "PAUSE", Yellow, Black);
					for(i=0;i<16;i++){
						song[i]=background[i];
					}
					game_pause=1;
				}
				else{
					enable_timer(2);
					enable_timer(3);
					writemaze(8,13,13,16);
					LCD_DrawPacman(pacman_position_x*L,pacman_position_y*L,7,Yellow);
					for(i=0;i<16;i++){
						song[i]=waka[i];
					}
					game_pause=0;
					
				}
				

				break;
			default:
				break;
		}
	}
	else {	/* button released */
		down_0=0;			
		NVIC_EnableIRQ(EINT0_IRQn);							 /* disable Button interrupts			*/
		LPC_PINCON->PINSEL4    |= (1 << 20);     /* External interrupt 0 pin selection */
	}
} // end INT0

/*************************KEY1***************************/
if(down_1 !=0){
	down_1++;
	if((LPC_GPIO2->FIOPIN & (1<<11)) == 0){
		switch(down_1){
			case 2:
				
				
				break;
			default:
				break;
		}
	}
	else {	/* button released */
		down_1=0;			
		NVIC_EnableIRQ(EINT1_IRQn);							 /* disable Button interrupts			*/
		LPC_PINCON->PINSEL4    |= (1 << 22);     /* External interrupt 0 pin selection */
	}
} // end KEY1

/*************************KEY2***************************/
if(down_2 !=0){
	down_2++;
	if((LPC_GPIO2->FIOPIN & (1<<12)) == 0){
		switch(down_2){
			case 2:
				

				break;
			default:
				break;
		}
	}
	else {	/* button released */
		down_2=0;		
		NVIC_EnableIRQ(EINT2_IRQn);							 /* disable Button interrupts			*/
		LPC_PINCON->PINSEL4    |= (1 << 24);     /* External interrupt 0 pin selection */
	}
} // end KEY2
	}
	reset_RIT();
  LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
	
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
