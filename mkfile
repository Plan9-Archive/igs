goban: goban.6 go.6
	6l -o goban goban.6 go.6

goban.6: goban.c
	6c -FTVw goban.c

go.6: go.c
	6c -FTVw go.c

clean:V:
	rm -rf goban *.6
