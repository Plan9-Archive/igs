#include <u.h>
#include <libc.h>
#include <draw.h>
#include <event.h>
#include "igc.h"

/* Traditional dimensions in tenth of mm */
enum{
	Gobanw = 4242,
	Gobanh = 4545,
	Line = 10,
	Linew = 220,
	Lineh = 237,
	Hoshidiam = 40,
	Stonediam = 225
};

enum{
	DGoban = 0xD79C5EFF
};

static Point ogoban; /* Origin of the lines of the goban. */
static double scale;

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
	bg = allocimage(display, Rect(0, 0, 1, 1), RGB24, 1, DGoban);
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
	default:
		hoshi[0] = 0;
	}
	for(i = 0; i < 3 && hoshi[0] != 0; i++){
		for(j = 0; j < 3; j++){
			p = Pt(ogoban.x + (int)(hoshi[i] * scale * Linew),
				ogoban.y + (int)(hoshi[j] * scale * Lineh));
			fillellipse(screen, p, hr, hr, display->black, ZP);
		}
	}

	/* Draw stones. */
	sr = scale * Stonediam / 2;
	for(i = 0; i < sgoban *  sgoban; i++){
		switch(goban[i]){
		case Black:
			p = Pt(ogoban.x + i % sgoban * scale * Linew,
				ogoban.y + i / sgoban * scale * Lineh);
			fillellipse(screen, p, sr, sr, display->black, ZP);
			break;
		case White:
			p = Pt(ogoban.x + i % sgoban * scale * Linew,
				ogoban.y + i / sgoban * scale * Lineh);
			ellipse(screen, p, sr, sr, 0, display->black, ZP);
			fillellipse(screen, p, sr-1, sr-1, display->white, ZP);
			break;
		case Black + Marked:
			p = Pt(ogoban.x + i % sgoban * scale * Linew,
				ogoban.y + i / sgoban * scale * Lineh);
			bg = allocimagemix(display, DBlack, DGoban);
			fillellipse(screen, p, sr, sr, bg, ZP);
			freeimage(bg);
			break;
		case White + Marked:
			p = Pt(ogoban.x + i % sgoban * scale * Linew,
				ogoban.y + i / sgoban * scale * Lineh);
			bg = allocimagemix(display, DWhite, DGoban);
			fillellipse(screen, p, sr, sr, bg, ZP);
			freeimage(bg);
		}
	}
}

/* TODO: better split move picking between igc.c and draw.c */
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

/* TODO: mark group & split pick/mark */
void
markgroup(void)
{
	int move;
	Mouse m;

	for(;; m = emouse()){
		/* This event is sent when the button is released. */
		if(m.buttons&1 && m.buttons&2 && m.buttons&4){
			continue;
		}else if(m.buttons&1 || m.buttons&4){
			break;
		}else if(m.buttons&2){
			move = px2move(m.xy);
			switch(goban[move]){
			case Black:
			case White:
				/* should mark the entire group */
				goban[move] += Marked;
				return;
				break;
			case Black + Marked:
			case White + Marked:
				/* Use telnet undo command */
				goban[move] -= Marked;
				return;
			}
		}
	}
}
