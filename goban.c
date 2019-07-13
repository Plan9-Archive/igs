#include <u.h>
#include <libc.h>
#include <draw.h>
#include <event.h>

enum
{
	Maxgobansize = 19,
	White = -1,
	Black = 1,
	Ko = 4,
	/* Traditional dimensions in tenth of mm */
	Gobanw = 4242,
	Gobanh = 4545,
	Line = 10,
	Linew = 220,
	Lineh = 237,
	Hoshidiam = 40,
	Stonediam = 225
};

char *rbuttons[] = 
{
	"exit",
	0
};

Menu rmenu =
{
	rbuttons,
};

int sgoban = Maxgobansize; /* Goban size, sgoban x sgoban. */
int goban[Maxgobansize * Maxgobansize] = {0};
Point ogoban; /* Origin of the lines of the goban. */
double scale;

void drawgoban(void);
int px2move(Point);
void eresized(int);

void
main(void)
{
	int move;
	Mouse m;

	if(initdraw(0, 0, "goban") < 0)
		sysfatal("initgoban failed: %r");
	einit(Emouse);

	goban[160] = 1;
	goban[178] = 1;
	goban[180] = 1;
	goban[198] = 1;
	goban[161] = -1;
	goban[181] = -1;
	goban[199] = -1;

	drawgoban();

	for(;;m = emouse()){
		if(m.buttons&1){
			move = px2move(m.xy);
			if(move == -1)
				print("error: %r\n");
		}else if(m.buttons&4){
			switch(emenuhit(3, &m, &rmenu)){
			case 0:
				exits(0);
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
	return p.x * sgoban + p.y;
}

void
eresized(int new)
{
	if(new)
		if(getwindow(display, Refnone) < 0)
			sysfatal("eresized failed: %r");
	drawgoban();
}
