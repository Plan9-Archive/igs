enum{
	Pass = -2,
	White,
	Empty,
	Black,
	Marked,
	Ko,
	Maxgobansize = 19
};

extern int sgoban;
extern int goban[];
extern int nblackcaptured;
extern int nwhitecaptured;
extern int npass;
extern int isgameover;

void initgoban(void);
int playmove(int*, int);
int undomove(int);
int pickundo(void);
