#include <u.h>
#include <libc.h>

#include "goban.h"

int ko;
/* group[coord] = group to which coord belongs.
 * liberty[grp] = number of liberties of group grp.
 * groupcounter: next new group index in liberty.
 */
int group[Maxgobansize * Maxgobansize];
int liberty[Maxgobansize * Maxgobansize];

void listnbr(int, int*);
int capture(int);
void push(int, int*, int);
int newgroup(void);
void mergegroup(int, int);
void updatelib(int);

void
initgoban(void)
{
	int i;

	nblackcaptured = 0;
	nwhitecaptured = 0;
	ko = -1;
	for(i = 0; i < sgoban * sgoban; i++){
		group[i] = -1;
		liberty[i] = -1;
	}
}

int
playmove(int turn, int move)
{
	int i;
	int mlib, nnbr, ncap, cap, newgrp;
	int nbr[4], oppogrp[4];

	if(move< 0 || move > sgoban * sgoban){
		werrstr("Move is out of bounds.");
		return -1;
	}
	switch(goban[move]){
	case Black:
	case White:
		werrstr("There is already a stone here.");
		return -1;
	case Ko:
		werrstr("Cannot retake ko.");
		return -1;
	}

	mlib = 0;
	ncap = 0;
	cap = 0;
	nnbr = 0;
	listnbr(move, nbr);
	for(i = 0; i < 4; i++)
		oppogrp[i] = -1;
	for(i = 0; i < 4; i++){
		if(nbr[i] != -1 && goban[nbr[i]] == Empty
			|| goban[nbr[i]] == Ko)
		{
			mlib++;
		}else if(nbr[i] != -1 && goban[nbr[i]] == turn){
			nnbr += liberty[group[nbr[i]]] - 1;
		}else if(nbr[i] != -1 && liberty[group[nbr[i]]] == 1){
			mlib++;
			cap = nbr[i];
			ncap += capture(group[nbr[i]]);
		}else if(nbr[i] != -1){
			push(group[nbr[i]], oppogrp, 4);
		}
	}
	if(mlib == 0 && nnbr == 0){
		werrstr("Suicidal moves are illegal.");
		return -1;
	}

	goban[move] = turn;
	newgrp = newgroup();
	if(newgrp < 0){
		print("How???: %r\n");
		exits("Error");
	}
	group[move] = newgrp;
	liberty[newgrp] = mlib;

	/* Apply the liberty changes to neighbouring groups. */
	for(i = 0; i < 4; i++){
		if(oppogrp[i] != -1)
			liberty[oppogrp[i]]--;
	}
	for(i = 0; i < 4; i++){
		if(nbr[i] != -1 && goban[nbr[i]] == turn)
			mergegroup(newgrp, group[nbr[i]]);
	}

	if(ko != -1){
		goban[ko] = Empty;
		ko = -1;
	}
	if(mlib == 1 && ncap == 1){
		goban[cap] = Ko;
		ko = cap;
	}
	return 0;
}

void
listnbr(int move, int nbr[4])
{
	int i;

	for(i = 0; i < 4; i++)
		nbr[i] = -1;
	if(move % sgoban != 0)
		nbr[0] = move - 1;
	if(move % sgoban != sgoban - 1)
		nbr[1] = move + 1;
	if(move / sgoban != 0)
		nbr[2] = move - sgoban;
	if(move / sgoban != sgoban - 1)
		nbr[3] = move + sgoban;
}

int
capture(int grp)
{
	int i, j;
	int ncap, lsize;
	int nbr[4], oppogrp[Maxgobansize * Maxgobansize / 2];

	ncap = 0;
	lsize = Maxgobansize * Maxgobansize / 2;
	for(i = 0; i < lsize; i++)
		oppogrp[i] = -1;

	liberty[grp] = -1;
	for(i = 0; i < sgoban * sgoban; i++){
		if(group[i] == grp){
			ncap++;
			switch(goban[i]){
			case Black:
				nblackcaptured++;
				break;
			case White:
				nwhitecaptured++;
			}
			/* List opponent neighbouring groups. */
			listnbr(i, nbr);
			for(j = 0; j < 4; j++){
				if(nbr[j] != -1 && goban[nbr[j]] == -goban[i])
					push(group[nbr[j]], oppogrp, lsize);
			}
			goban[i] = Empty;
			group[i] = -1;
		}
	}
	for(i = 0; i < lsize; i++){
		if(oppogrp[i] == -1){
			break;
		}else{
			liberty[oppogrp[i]]++;
		}
	}
	return ncap;
}

/* Push val in a list l implemented with an array of size lsize.
 * Note that the array has to be initialized to -1.
 */
void
push(int val, int *l, int lsize)
{
	int i;

	for(i = 0; i < lsize; i++){
		if(l[i] == -1){
			l[i] = val;
			return;
		}else if(l[i] == val){
			return;
		}
	}
	exits("List full.");
}

int
newgroup(void)
{
	int i;

	for(i = 0; i < sgoban * sgoban; i++){
		if(liberty[i] == -1)
			return i;
	}
	werrstr("Could not make a new group.");
	return -1;
}

/* Merge h in g. */
void
mergegroup(int g, int h)
{
	int i;

	if(g == h)
		return;

	liberty[h] = -1;
	for(i = 0; i < sgoban * sgoban; i++) {
		if(group[i] == h)
			group[i] = g;
	}
	updatelib(g);
}	

/* Update liberties of group g. */
void
updatelib(int g)
{
	int i, j;
	int nbr[4], nlib;

	for(i = 0; i < sgoban * sgoban; i++) {
		/* Mark liberties. */
		if(group[i] == g){
			listnbr(i, nbr);
			for(j = 0; j < 4; j++){
				switch(goban[nbr[j]]){
				case Empty:
				case Ko:
					goban[nbr[j]] += Marked;
				}
			}
		}
	}
	/* Count and delete marks. */
	nlib = 0;
	for(i = 0; i < sgoban * sgoban; i++) {
		switch(goban[i]){
		case Marked:
		case Ko + Marked:
			nlib++;
			goban[i] -= Marked;
		}
	}
	liberty[g] = nlib;
}

int
undomove(int move)
{
	/* Pick move with the mouse. */
	if(move == -1)
		move = pickundo();
	/* Pick did not abort. */
	if(move != -1)
		capture(group[move]);
	return 0;
}
