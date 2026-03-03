/*----------------------------------------------------------------------------
 * Name:    Can.c
 * Purpose: CAN interface for for LPC17xx with MCB1700
 * Note(s): see also http://www.port.de/engl/canprod/sv_req_form.html
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2009 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#include <LPC17xx.h>                  /* LPC17xx definitions */
#include "CAN.h"                      /* LPC17xx CAN adaption layer */
#include "../GLCD/GLCD.h"
#include <stdio.h>

extern uint8_t icr ; 										//icr and result must be global in order to work with both real and simulated landtiger.
extern uint32_t result;
extern CAN_msg       CAN_TxMsg;    /* CAN message for sending */
extern CAN_msg       CAN_RxMsg;    /* CAN message for receiving */                                

uint32_t messaggio=0;

/*----------------------------------------------------------------------------
  CAN interrupt handler
 *----------------------------------------------------------------------------*/
void CAN_IRQHandler (void)  {
		uint8_t str[15];


  /* check CAN controller 1 */
	icr = 0;
  icr = (LPC_CAN1->ICR | icr) & 0xFF;               /* clear interrupts */
	
  if (icr & (1 << 0)) {                          		/* CAN Controller #1 meassage is received */
		/*CAN_rdMsg (1, &CAN_RxMsg);	                		//Read the message 
    LPC_CAN1->CMR = (1 << 2);                    		//Release receive buffer 
		
		val_RxCoordX = (CAN_RxMsg.data[0] << 8)  ;
		val_RxCoordX = val_RxCoordX | CAN_RxMsg.data[1];
		
		val_RxCoordY = (CAN_RxMsg.data[2] << 8);
		val_RxCoordY = val_RxCoordY | CAN_RxMsg.data[3];
		
		display.x = val_RxCoordX;
		display.y = val_RxCoordY-140;
		TP_DrawPoint_Magnifier(&display);
		
		puntiRicevuti1++;*/
  }
	if (icr & (1 << 1)) {                         /* CAN Controller #1 meassage is transmitted */
		// do nothing in this example
	}
		
	/* check CAN controller 2 */
	icr = 0;
	icr = (LPC_CAN2->ICR | icr) & 0xFF;             /* clear interrupts */

	if (icr & (1 << 0)) {                          	/* CAN Controller #2 meassage is received */
		CAN_rdMsg (2, &CAN_RxMsg);	                		/* Read the message */
    LPC_CAN2->CMR = (1 << 2);                    		/* Release receive buffer */
		
		
		messaggio=(CAN_RxMsg.data[3]<<24)|(CAN_RxMsg.data[2]<<16)|(CAN_RxMsg.data[1]<<8)|CAN_RxMsg.data[0];
		sprintf((char*)str,"SCORE: %d",messaggio&0xffff);
		GUI_Text(2, 305, (uint8_t *) str, Yellow, Black);
		sprintf((char*)str,"LIVES: %d",(messaggio>>16)&0xff);
		GUI_Text(110, 305, (uint8_t *) str, Yellow, Black);
		sprintf((char*)str,"%d s  ",messaggio>>24);
		GUI_Text(197, 305, (uint8_t *) str, Yellow, Black);
	}
	if (icr & (1 << 1)) {                         /* CAN Controller #2 meassage is transmitted */
		// do nothing in this example
	}
}
