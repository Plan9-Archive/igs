#include <u.h>
#include <libc.h>
#include <stdio.h>
#include <draw.h>
#include <event.h>
#include "igc.h"

char *mbuttons[] =
{
	"pass",
	"undo",
	"restart scoring",
	"resign",
	0
};

char *rbuttons[] =
{
	"exit",
	0
};

Menu mmenu =
{
	mbuttons,
};

Menu rmenu =
{
	rbuttons,
};

static char* move2coord(int);
static int coord2move(char*);

static void removedeadgroup(int);

static void
usage(void)
{
	fprint(2, "Internet Go Client\n");
	fprint(2, "usage: %s [-s gobansize]\n", argv0);
	exits("usage");
}

void
main(int argc, char *argv[])
{
	int move, sg;
	Mouse m;

	sg = Maxgobansize;
	ARGBEGIN {
	case 's':
		sg = atoi(EARGF(usage()));
		break;
	case 'h':
		usage();
	} ARGEND
	if(argc != 0)
		usage();

	if(initdraw(0, 0, "goban") < 0)
		sysfatal("initdraw failed: %r");
	einit(Emouse);

	initgoban(sg);
	drawgoban();

	for(;; m = emouse()){
		if(m.buttons&1){
			move = px2move(m.xy);
			if(move == -1){
				print("error: %r\n");
				continue;
			}
			if(isgameover == 0 && playmove(move) == -1){
				print("error: %r\n");
				continue;
			}else if(isgameover == 1){
				removedeadgroup(move);
			}
			drawgoban();
		}else if(m.buttons&2){
			switch(emenuhit(2, &m, &mmenu)){
			case 0:
				playmove(Pass);
				break;
			case 1:
				/* call igs undoplease */
				break;
			case 2:
				/* call igs undo when scoring */
				break;
			case 3:
				isgameover = 1;
				print("Winner: %d\n", -turn);
			}
		}else if(m.buttons&4){
			switch(emenuhit(3, &m, &rmenu)){
			case 0:
				isgameover = 1;
				break;
			}
		}
	}
}

static char*
move2coord(int move)
{
	char coord[4];

	/* There is no I in coordinates: it goes from H to J. */
	if(move % 19 < 'I' - 'A' + 1)
		coord[0] = 'A' + move % 19;
	else
		coord[0] = 'A' + move % 19 + 1;
	sprintf(coord + 1, "%d", 19 - move / 19);

	return coord;
}

static int
coord2move(char* coord)
{
	int move;

	/* There is no I in coordinates: it goes from H to J. */
	if(coord[0] < 'I')
		move = coord[0] - 'A';
	else
		move = coord[0] - 'A' - 1;
	move += (19 - atoi(coord + 1)) * sgoban;
	return move;
}

static void
removedeadgroup(int move)
{
	Mouse m;

	for(;; m = emouse()){
		/* This event is sent when the button is released. */
		if(m.buttons&1 && m.buttons&2 && m.buttons&4){
			continue;
		}else if(m.buttons&1){
			switch(goban[move]){
			case Black:
			case White:
				/* should mark the entire group */
				goban[move] += Marked;
				return;
				break;
			}
		}
	}
}
 
void
eresized(int new)
{
	if(new)
		if(getwindow(display, Refnone) < 0)
			sysfatal("eresized failed: %r");
	drawgoban();
}
