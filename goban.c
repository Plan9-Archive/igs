#include <u.h>
#include <libc.h>
#include <draw.h>
#include <event.h>
#include <cursor.h>

#include "goban.h"

Cursor sightcursor = {
	{-7, -7},
	{0x1F, 0xF8, 0x3F, 0xFC, 0x7F, 0xFE, 0xFB, 0xDF,
	 0xF3, 0xCF, 0xE3, 0xC7, 0xFF, 0xFF, 0xFF, 0xFF,
	 0xFF, 0xFF, 0xFF, 0xFF, 0xE3, 0xC7, 0xF3, 0xCF,
	 0x7B, 0xDF, 0x7F, 0xFE, 0x3F, 0xFC, 0x1F, 0xF8},
	{0x00, 0x00, 0x0F, 0xF0, 0x31, 0x8C, 0x21, 0x84,
	 0x41, 0x82, 0x41, 0x82, 0x41, 0x82, 0x7F, 0xFE,
	 0x7F, 0xFE, 0x41, 0x82, 0x41, 0x82, 0x41, 0x82,
	 0x21, 0x84, 0x31, 0x8C, 0x0F, 0xF0, 0x00, 0x00}
};

/* Traditional dimensions in tenth of mm */
enum
{
	Gobanw = 4242,
	Gobanh = 4545,
	Line = 10,
	Linew = 220,
	Lineh = 237,
	Hoshidiam = 40,
	Stonediam = 225
};

char *mbuttons[] = 
{
	"pass",
	"undo",
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

Point ogoban; /* Origin of the lines of the goban. */
double scale;

void drawgoban(void);
int px2move(Point);

void
usage(void)
{
	fprint(2, "usage: %s [-s gobansize]\n", argv0);
	exits("usage");
}

void
main(int argc, char *argv[])
{
	int move, turn;
	Mouse m;

	sgoban = Maxgobansize;
	ARGBEGIN {
	case 's':
		sgoban = atoi(EARGF(usage()));
		break;
	case 'h':
		usage();
	} ARGEND
	if(argc != 0)
		usage();

	if(initdraw(0, 0, "goban") < 0)
		sysfatal("initgoban failed: %r");
	einit(Emouse);

	initgoban();
	drawgoban();

	turn = Black;
	for(; isgameover == 0; m = emouse()){
		if(m.buttons&1){
			move = px2move(m.xy);
			if(move == -1){
				print("error: %r\n");
				continue;
			}
			if(playmove(&turn, move) == -1){
				print("error: %r\n");
				continue;
			}
			drawgoban();
		}else if(m.buttons&2){
			switch(emenuhit(2, &m, &mmenu)){
			case 0:
				playmove(&turn, Pass);
				break;
			case 1:
				undomove(-1);
				drawgoban();
				break;
			case 2:
				isgameover = 1;
				print("Winner: %d\n", -turn);
			}
		}else if(m.buttons&4){
			switch(emenuhit(3, &m, &rmenu)){
			case 0:
				break;
			}
		}
	}
}

void
drawgoban(void)
{
	int i, j;
	int l, hr, sr;
	int hoshi[3];
	Point o;
	Point p, q;
	Rectangle r;
	Point poly[4];
	Image *bg;

	o = screen->r.min;
	if(Dx(screen->r) * Gobanh < Dy(screen->r) * Gobanw){
		scale = (double)Dx(screen->r) / Gobanw;
	}else{
		scale = (double)Dy(screen->r) / Gobanh;
	}
	/* The line function uses 1 + 2 * width as its thickness. */
	l = (Line * scale - 1) / 2;

	/* Fill goban background. */
	p = Pt(o.x + (int)(scale * Gobanw), o.y + (int)(scale * Gobanh));
	r = Rpt(o, p);
	poly[0] = r.min;
	poly[1] = Pt(r.max.x, r.min.y);
	poly[2] = r.max;
	poly[3] = Pt(r.min.x, r.max.y);
	bg = allocimage(display, Rect(0, 0, 1, 1), RGB24, 1, 0xD79C5EFF);
	fillpoly(screen, poly, 4, ~0, bg, ZP);
	freeimage(bg);

	/* Draw lines. */
	ogoban = Pt(o.x + scale * (Gobanw - (sgoban - 1) * Linew) / 2,
		o.y + scale * (Gobanh - (sgoban - 1) * Lineh) / 2);
	for(i = 0; i < sgoban; i++){
		p = Pt(ogoban.x + (int)(i * scale * Linew), ogoban.y);
		q = Pt(ogoban.x + (int)(i * scale * Linew),
			ogoban.y + (int)(scale * (sgoban - 1) * Lineh));
		line(screen, p, q, Enddisc, Enddisc, l / 2, display->black, ZP);
		p = Pt(ogoban.x, ogoban.y + (int)(i * scale * Lineh));
		q = Pt(ogoban.x + (int)(scale * (sgoban - 1) * Linew),
			ogoban.y + (int)(i * scale * Lineh));
		line(screen, p, q, Enddisc, Enddisc, l / 2, display->black, ZP);
	}

	/* Draw hoshi; only meaningful for 19x19, 13x13 and 9x9. */
	hr = scale * Hoshidiam / 2;
	switch(sgoban){
	case 19:
		hoshi[0] = 3;
		hoshi[1] = 9;
		hoshi[2] = 15;
		break;
	case 13:
		hoshi[0] = 3;
		hoshi[1] = 6;
		hoshi[2] = 9;
		break;
	case 9:
		hoshi[0] = 2;
		hoshi[1] = 4;
		hoshi[2] = 6;
		break;
	}
	for(i = 0; i < 3; i++){
		for(j = 0; j < 3; j++){
			p = Pt(ogoban.x + (int)(hoshi[i] * scale * Linew),
				ogoban.y + (int)(hoshi[j] * scale * Lineh));
			fillellipse(screen, p, hr, hr, display->black, ZP);
		}
	}

	/* Draw stones. */
	sr = scale * Stonediam / 2;
	for(i = 0; i < sgoban *  sgoban; i++){
		if(goban[i] == Black){
			p = Pt(ogoban.x + i % sgoban * scale * Linew,
				ogoban.y + i / sgoban * scale * Lineh);
			fillellipse(screen, p, sr, sr, display->black, ZP);
		}else if(goban[i] == White){
			p = Pt(ogoban.x + i % sgoban * scale * Linew,
				ogoban.y + i / sgoban * scale * Lineh);
			ellipse(screen, p, sr, sr, 0, display->black, ZP);
			fillellipse(screen, p, sr-1, sr-1, display->white, ZP);
		}
	}
}

/* Pixel coordinates to Go move. */
int
px2move(Point px)
{
	Point p;
	Rectangle r;

	p = Pt(ogoban.x - scale * Linew / 2, ogoban.y - scale * Lineh / 2);
	r = Rect(p.x, p.y, p.x + scale * sgoban * Linew,
		p.y + scale * sgoban * Lineh);
	if(ptinrect(px, r) == 0){
		werrstr("Click out of the goban.");
		return -1;
	}
	p = subpt(px, r.min);
	p.x /= scale * Linew;
	p.y /= scale * Lineh;
	return p.y * sgoban + p.x;
}

int
pickundo(void)
{
	int move;
	Mouse m;

	esetcursor(&sightcursor);
	for(;; m = emouse()){
		/* This event is sent when the button is released. */
		if(m.buttons&1 && m.buttons&2 && m.buttons&4){
			continue;
		}else if(m.buttons&1 || m.buttons&4){
			move = -1;
			break;
		}else if(m.buttons&2){
			move = px2move(m.xy);
			break;
		}
	}
	esetcursor(nil);
	return move;
}

void
eresized(int new)
{
	if(new)
		if(getwindow(display, Refnone) < 0)
			sysfatal("eresized failed: %r");
	drawgoban();
}
