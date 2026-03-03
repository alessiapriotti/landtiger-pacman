#include "music.h"

#include "../timer/timer.h"

void playNote(NOTE note)
{
	if(note.freq != pause)
	{
		reset_timer(0);
		init_timer(0,0,0,3,note.freq*8);
		enable_timer(0);
	}
	reset_timer(1);
	init_timer(1,0,0,7, note.duration);
	enable_timer(1);
}

BOOL isNotePlaying(void)
{
	return ((LPC_TIM0->TCR != 0) || (LPC_TIM1->TCR != 0));
}
