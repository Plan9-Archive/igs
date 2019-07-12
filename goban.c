#include <u.h>
#include <libc.h>
#include <draw.h>
#include <event.h>

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

void
drawgoban(int size, s8int *goban)
{
	int i;
	int l;
	double scale;
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
	if((l = (int)((double)Line * scale)) < 1)
		l = 1;

	/* Draw goban border. */
	p = Pt(o.x + (int)(scale * Gobanw), o.y + (int)(scale * Gobanh));
	r = Rect(o, p);
	border(screen, r, l, display->black, ZP);

	/* Fill goban background. */
	r = insetrect(r, l);
	poly[0] = r.min;
	poly[1] = Pt(r.max.x, r.min.y);
	poly[2] = r.max;
	poly[3] = Pt(r.min.x, r.max.y);
	bg = allocimage(display, Rect(0, 0, 1, 1), RGB24, 1, 0xD79C5EFF);
	fillpoly(screen, poly, 4, ~0, bg, ZP);
	freeimage(bg);
}

void
main(void)
{
	Mouse m;

	if(initdraw(0, 0, "goban") < 0)
		sysfatal("initgoban failed: %r");
	einit(Emouse);

	drawgoban(5, nil);

	for(;;m = emouse())
		if(m.buttons)
			break;

	exits(0);
}

void
eresized(int new)
{
	if(new)
		if(getwindow(display, Refnone) < 0)
			sysfatal("eresized failed: %r");
}
