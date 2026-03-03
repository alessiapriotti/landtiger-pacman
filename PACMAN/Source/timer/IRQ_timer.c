/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    timer.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "LPC17xx.h"
#include "timer.h"
#include "GLCD/GLCD.h" 
#include <stdio.h>
#include "RIT/RIT.h"
#include <stdlib.h>
#include "../sample.h"
#include "music/music.h"
#include "/CAN/CAN.h"


#define UPTICKS 1






/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
extern volatile int matrice[R][C];
extern volatile int nrand;
extern volatile int npowerpill;
extern volatile int nextX, nextY;
extern volatile int ghost_position_x, ghost_position_y;
extern volatile int pacman_position_x,pacman_position_y;
extern volatile int ghost_mode;
extern volatile int game_over;
extern volatile int away_point_x,away_point_y;
extern volatile int countdown;
extern volatile int countdown_ghost;
extern volatile int lives, score;
extern volatile NOTE dead[16];
extern volatile NOTE song[16];
extern volatile NOTE background[16];
extern volatile NOTE waka[16];
extern volatile int n;
extern volatile int semaforo;

uint16_t SinTable[45] =                                       /* ÕýÏÒ±í                       */
{
    410, 467, 523, 576, 627, 673, 714, 749, 778,
    799, 813, 819, 817, 807, 789, 764, 732, 694, 
    650, 602, 550, 495, 438, 381, 324, 270, 217,
    169, 125, 87 , 55 , 30 , 12 , 2  , 0  , 6  ,   
    20 , 41 , 70 , 105, 146, 193, 243, 297, 353
};

void powerpill(void){
	int pillx,pilly;
	npowerpill++;
	srand(read_RIT()|read_timer(2));
	do{
			pillx=rand()%(20-0+1);
			pilly=rand()%(26-0+1);
		}while(matrice[pilly][pillx] !=1);
	matrice[pilly][pillx] =2;
	LCD_DrawFullSquare(pillx*L,pilly*L,5,Magenta);
}

void TIMER0_IRQHandler (void)
{
	if(LPC_TIM0->IR & 1) // MR0
	{ 
		// your code
		static int sineticks=0;
		/* DAC management */	
		static int currentValue; 
		currentValue = SinTable[sineticks];
		currentValue -= 410;
		currentValue /= 1;
		currentValue += 410;
		LPC_DAC->DACR = currentValue <<6;
		sineticks++;
		if(sineticks==45) sineticks=0;
		LPC_TIM0->IR = 1;			//clear interrupt flag
	}
	else if(LPC_TIM0->IR & 2){ // MR1
		// your code	
		LPC_TIM0->IR = 2;			// clear interrupt flag 
	}
	else if(LPC_TIM0->IR & 4){ // MR2
		// your code	

		LPC_TIM0->IR = 4;			// clear interrupt flag 
	}
	else if(LPC_TIM0->IR & 8){ // MR3
		// your code	
		LPC_TIM0->IR = 8;			// clear interrupt flag 
	}
  return;
}

/******************************************************************************
** Function name:		Timer1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

void TIMER1_IRQHandler (void)
{

	if(LPC_TIM1->IR & 1) // MR0
	{ 
		// your code
		disable_timer(0);
		LPC_TIM1->IR = 1;			//clear interrupt flag
	}
	else if(LPC_TIM1->IR & 2){ // MR1
		// your code
		LPC_TIM1->IR = 2;			// clear interrupt flag 
	}
	else if(LPC_TIM1->IR & 4){ // MR2
		// your code	
		LPC_TIM1->IR = 4;			// clear interrupt flag 
	}
	else if(LPC_TIM1->IR & 8){ // MR3
		// your code	
		LPC_TIM1->IR = 8;			// clear interrupt flag 
	} 

	return;
}

/******************************************************************************
** Function name:		Timer2_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER2_IRQHandler (void)
{
	uint8_t str[15];

	if(LPC_TIM2->IR & 1) // MR0
	{ if(game_over==0){
			if(npowerpill<6){		
				disable_timer(2);
				powerpill();
				nrand=rand()%((54+npowerpill)-(60-countdown+1)+1)+(60-countdown+1);
				nrand=(nrand*25000000)+0x1312D0; //nrand*f+0.05s
				init_timer(2,0,0,1,nrand);
				enable_timer(2);
			}
		}

		LPC_TIM2->IR = 1;			//clear interrupt flag
	}
	else if(LPC_TIM2->IR & 2){ // MR1
		int i;
		if(game_over==0){
			countdown--;
			CAN_TxMsg.data[0] = (score&0xff);
			CAN_TxMsg.data[1] = ((score>>8)&0xff);
			CAN_TxMsg.data[2] = lives;
			CAN_TxMsg.data[3] = countdown;
			CAN_TxMsg.len = 4;
			CAN_TxMsg.id = 2;
			CAN_TxMsg.format = STANDARD_FORMAT;
			CAN_TxMsg.type = DATA_FRAME;
			CAN_wrMsg (1, &CAN_TxMsg);// send from CAN1 to CAN2
			
			if(countdown==0){
				disable_timer(2);
				disable_timer(3);
				game_over=1;
				for(i=0;i<16;i++){
						song[i]=dead[i];
					}
				GUI_Text(84, 152, (uint8_t *) "GAME OVER!", Yellow, Black);
			}
			else if(countdown>0){
				disable_timer(2);
				init_timer(2,0,1,1,(0x17D7840*((60-countdown)+1)));
				enable_timer(2);
			}
		}
		LPC_TIM2->IR = 2;			// clear interrupt flag 
	
	}
	else if(LPC_TIM2->IR & 4){ // MR2
		// your code	

		LPC_TIM2->IR = 4;			// clear interrupt flag 
	}
	else if(LPC_TIM2->IR & 8){ // MR3
		// your code	
		LPC_TIM2->IR = 8;			// clear interrupt flag 
	} 

	return;
}


/******************************************************************************
** Function name:		Timer2_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER3_IRQHandler (void)
{
		uint8_t str[15];

  if(LPC_TIM3->IR & 1) // MR0
	{ 
		// your code
		//il fantasma parte con un periodo di 0.8s, man mano che procede il gioco il periodo diminuisce a 0.5s e poi 0.3s,
		//se il fantasma diventa blu invece il periodo torna ad essere 0.8s, altrimenti pacman non riesce a prenderlo 
		if(ghost_mode==Blue2){
			disable_timer(3);
			init_timer(3,0,0,3,0x1312D00);//0.8s=0x1312D00

			enable_timer(3);
		}
		if((ghost_mode==Red)&&(countdown<40)&&(countdown>20)){
			disable_timer(3);
			init_timer(3,0,0,3,0xBEBC20);//0.5s=0xBEBC20

			enable_timer(3);
		}
		if((ghost_mode==Red)&&(countdown<20)){
			disable_timer(3);
			init_timer(3,0,0,3,0x7270E0);//0.3s=0x7270E0
			enable_timer(3);
		}
		if(countdown_ghost<=read_timer(2)){
			ghost_mode=Red;
			countdown_ghost=0x5AE5A740;//61s
		}
		if(ghost_mode==Red){
			bfs(pacman_position_x,pacman_position_y);
			LCD_DrawFullSquare(ghost_position_x*L,ghost_position_y*L,L,Black);
			writemaze(ghost_position_x,ghost_position_x+1,ghost_position_y,ghost_position_y+1);
			ghost_position_x=nextX;
			ghost_position_y=nextY;
			LCD_DrawGhost(ghost_position_x*L,ghost_position_y*L,ghost_mode);
		}
		else if (ghost_mode==Blue2){
			point_away_pacman();
			if(!((ghost_position_x==away_point_x)&&(ghost_position_y==away_point_y))){
				bfs(away_point_x,away_point_y);
				LCD_DrawFullSquare(ghost_position_x*L,ghost_position_y*L,L,Black);
				writemaze(ghost_position_x,ghost_position_x+1,ghost_position_y,ghost_position_y+1);
				ghost_position_x=nextX;
				ghost_position_y=nextY;
				LCD_DrawGhost(ghost_position_x*L,ghost_position_y*L,ghost_mode);
			}
		}
		else if(ghost_mode==0){
			nextX=10;
			nextY=12;
		}
		
		if((pacman_position_x==ghost_position_x) && (pacman_position_y==ghost_position_y)&&(ghost_mode==Red)){
			semaforo++;
			if(semaforo==1){
				lives--;
				if(lives==0){
					disable_timer(2);
					disable_timer(3);
					game_over=1;
					int i;
					for(i=0;i<16;i++){
						song[i]=dead[i];
					}
					GUI_Text(84, 152, (uint8_t *) "GAME OVER!", Yellow, Black);
				}
				pacman_position_x=10;
				pacman_position_y=15;
				LCD_DrawPacman(pacman_position_x*L,pacman_position_y*L,7,Yellow);
				
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
			semaforo=0;
		}
		
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
				/*sprintf((char*)str,"SCORE: %d",score);
				GUI_Text(2, 305, (uint8_t *) str, Yellow, Black);
				sprintf((char*)str,"LIVES: %d",lives);
				GUI_Text(110, 305, (uint8_t *) str, Yellow, Black);*/
				
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
			semaforo=0;
		}
	
		LPC_TIM3->IR = 1;			//clear interrupt flag
	}
	else if(LPC_TIM3->IR & 2){ // MR1
		// your code
		LPC_TIM3->IR = 2;			// clear interrupt flag 
	}
	else if(LPC_TIM3->IR & 4){ // MR2
		// your code	
		LPC_TIM3->IR = 4;			// clear interrupt flag 
	}
	else if(LPC_TIM3->IR & 8){ // MR3
		// your code	
		LPC_TIM3->IR = 8;			// clear interrupt flag 
	} 

	return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/

