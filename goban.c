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
	int n;
	double scale;
	Point o;
	Rectangle r;

	o = screen->r.min;
	n = Dx(screen->r) < Dy(screen->r) ? Dx(screen->r) : Dy(screen->r);
	scale = (double)n / (double)Gobanh;

	r = Rect(o.x, o.y, o.x + n, o.y + n);
	border(screen, r, 1, display->black, ZP);
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
