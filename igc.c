#include <u.h>
#include <libc.h>
#include <draw.h>
#include <event.h>
#include "igc.h"

char *mbuttons[] =
{
	"pass",
	"undo",
	"resign",
	"mark",
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

	for(; isgameover == 0; m = emouse()){
		if(m.buttons&1){
			move = px2move(m.xy);
			if(move == -1){
				print("error: %r\n");
				continue;
			}
			if(playmove(move) == -1){
				print("error: %r\n");
				continue;
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
				isgameover = 1;
				print("Winner: %d\n", -turn);
				break;
			case 3:
				markstone();
				drawgoban();
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

void
eresized(int new)
{
	if(new)
		if(getwindow(display, Refnone) < 0)
			sysfatal("eresized failed: %r");
	drawgoban();
}
