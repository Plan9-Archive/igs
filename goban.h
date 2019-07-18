enum{
	Pass = -2,
	White,
	Empty,
	Black,
	Ko,
	Marked = 10,
	Maxgobansize = 19
};

extern int sgoban;
extern int turn;
extern int npass;
extern int nblackcaptured;
extern int nwhitecaptured;
extern int ko;
extern int isgameover;
extern int goban[Maxgobansize * Maxgobansize];
extern int group[Maxgobansize * Maxgobansize]; /* group[coord] */
extern int liberty[Maxgobansize * Maxgobansize]; /* liberties[group] */

void initgoban(int);
int playmove(int);
int undomove(int);
int pickundo(void);
