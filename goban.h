enum{
	Pass = -2,
	White,
	Empty,
	Black,
	Marked,
	Ko,
	Maxgobansize = 19
};

typedef struct Goban Goban;
struct Goban
{
	int sgoban;
	int turn;
	int npass;
	int nblackcaptured;
	int nwhitecaptured;
	int ko;
	int isgameover;
	int goban[Maxgobansize * Maxgobansize];
	int group[Maxgobansize * Maxgobansize]; /* group[coord] */
	int liberty[Maxgobansize * Maxgobansize]; /* liberties[group] */
	Point ogoban; /* Origin of the lines of the goban. */
	double scale;
};

void initgoban(Goban*);
int playmove(int*, int);
int undomove(int);
int pickundo(void);
