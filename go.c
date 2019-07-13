#include <u.h>
#include <libc.h>

#include "goban.h"

int groupindex[Maxgobansize * Maxgobansize] = {0};
int groupliberty[Maxgobansize * Maxgobansize] = {0};
int groupcounter;

int
playmove(int turn, int move)
{
	if(move< 0 || move > sgoban * sgoban){
		werrstr("Move is out of bounds.");
		return -1;
	}
	switch(goban[move]){
	case Black:
	case White:
		werrstr("There is already a stone here.");
		return -1;
	case Ko:
		werrstr("Cannot retake ko.");
		return -1;
	default:
		goban[move] = turn;
	}
	return 0;
}

int
undomove(int move)
{
	if(move == -1)
		move = pickundo();
	if(move != -1)
		goban[move] = 0;
	return 0;
}
