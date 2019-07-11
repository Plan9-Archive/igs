#include <u.h>
#include <libc.h>
#include <draw.h>
#include <event.h>

void
main(void)
{
	Mouse m;

	if(initdraw(0, 0, "goban") < 0)
		sysfatal("initgoban failed: %r");
	einit(Emouse);

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
