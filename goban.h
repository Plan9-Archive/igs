enum
{
	Pass = -2,
	White,
	Empty,
	Black,
	Marked,
	Ko,
	Maxgobansize = 19
};

int sgoban; /* Goban size, sgoban x sgoban. */
int goban[Maxgobansize * Maxgobansize] = {Empty};
int nblackcaptured;
int nwhitecaptured;


void initgoban(void);
int playmove(int*, int);
int undomove(int);
int pickundo(void);
