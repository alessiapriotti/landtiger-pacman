#define R 27
#define C 21
#define L 11
#define INF 1000000


// Struttura per rappresentare una cella nella coda
typedef struct Point{
    int x, y;   // Coordinate
} Point;

/*sample.c*/
void writemaze(int x0, int x1, int y0, int y1);
void init_matrice(void);
void initgame(void);
void bfs(int endX, int endY);
void point_away_pacman(void);