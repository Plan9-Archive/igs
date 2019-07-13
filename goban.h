enum
{
	Maxgobansize = 19,
	White = -1,
	Black = 1,
	Ko = 4
};

int sgoban; /* Goban size, sgoban x sgoban. */
int goban[Maxgobansize * Maxgobansize] = {0};

int playmove(int, int);
int undomove(int);
int pickundo(void);
