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
s8int goban[Maxgobansize];

void
drawgoban(void)
{
	int i, j;
	int l, hr, sr;
	double scale;
	Point o;
	int px, py;
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
	px = o.x + (int)(scale * (Gobanw - (sgoban - 1) * Linew) / 2);
	py = o.y + (int)(scale * (Gobanh - (sgoban - 1) * Lineh) / 2);
	for(i = 0; i < sgoban; i++){
		p = Pt(px + (int)(i * scale * Linew), py);
		q = Pt(px + (int)(i * scale * Linew),
			py + (int)(scale * (sgoban - 1) * Lineh));
		line(screen, p, q, Enddisc, Enddisc, l / 2, display->black, ZP);
		p = Pt(px, py + (int)(i * scale * Lineh));
		q = Pt(px + (int)(scale * (sgoban - 1) * Linew),
			py + (int)(i * scale * Lineh));
		line(screen, p, q, Enddisc, Enddisc, l / 2, display->black, ZP);
	}

	/* Draw hoshi; only implemented for 19x19 gobans. */
	if(sgoban == 19){
		hr = scale * (Hoshidiam / 2);
		for(i = 3; i < 16; i += 6){
			for(j = 3; j < 16; j += 6){
				p = Pt(px + (int)(i * scale * Linew),
					py + (int)(j * scale * Lineh));
				fillellipse(screen, p, hr, hr, display->black,
					ZP);
			}
		}
	}
}

void
main(void)
{
	Mouse m;

	if(initdraw(0, 0, "goban") < 0)
		sysfatal("initgoban failed: %r");
	einit(Emouse);

	drawgoban();

	for(;;m = emouse()){
		if(m.buttons&4){
			switch(emenuhit(3, &m, &rmenu)){
			case 0:
				exits(0);
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
