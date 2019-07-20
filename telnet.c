#include <u.h>
#include <libc.h>
#include <thread.h>

void
threadmain(int argc, char *argv[])
{
	int telfd, n;
	char *c, buf[512];
	char devdir[NETPATHLEN];

	telfd = dial("net!igs.joyjoy.net!7777", 0, devdir, 0);
	if(telfd < 0)
		sysfatal("dial: %r");

	for(;;){
		n = read(telfd, buf, sizeof(buf));
		for(c = buf; c < buf + n; c++){
			if(*c != '\r')
				print("%c", *c);
		}
	}

	exits(0);
}
