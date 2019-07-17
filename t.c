#include <u.h>
#include <libc.h>

void
main(void)
{
	int telfd, n;
	char *c, buf[512];
	char devdir[NETPATHLEN];

	telfd = dial("net!igs.joyjoy.net!7777", 0, devdir, 0);
	print("%d %s\n", telfd, devdir);

	for(;;){
		n = read(telfd, buf, sizeof(buf));
		for(c = buf; c < buf + n; c++)
			print("%c", *c);
	}

	exits(0);
}
