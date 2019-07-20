#include <u.h>
#include <libc.h>

extern void game(int);

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
	int sg;

	sg = 0;
	ARGBEGIN {
	case 's':
		sg = atoi(EARGF(usage()));
		break;
	case 'h':
		usage();
	} ARGEND
	if(argc != 0)
		usage();

	switch(rfork(RFPROC)){
	case 0:
		game(sg);
		break;
	default:
		wait();
	}
	exits(0);
}
