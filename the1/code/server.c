#include <stdio.h>
//#include <malloc.h>
#include <stdlib.h>
#include<sys/socket.h>
#include <unistd.h>
#include <sys/wait.h>
#include<sys/select.h>
#include <string.h>
#include <limits.h>
#include <math.h>


#define PIPE(fd) socketpair(AF_UNIX, SOCK_STREAM, PF_UNIX, fd)

// STRUCTS
typedef struct Obstacle {
    int x, y;
} Obstacle;

typedef struct Hunter {
    int x, y, energy, isLive;
} Hunter;

typedef struct Pray {
    int x, y, energy, isLive;
} Pray;

typedef struct PrintStruct {
    int x, y;
    char mode;
} PrintStruct;

typedef struct coordinate {
    int x;
    int y;
} coordinate;
typedef struct server_message {
    coordinate pos;
    coordinate adv_pos;
    int object_count;
    coordinate object_pos[4];
} server_message;

typedef struct ph_message {
    coordinate move_request;
} ph_message;

//FUNCTIONS
void printTopAndBottom(int mapWidth);

void sortPrintStructs(PrintStruct *, int);

void printAll(PrintStruct *printStructs, int structSize);

void printOutput(int width, int height, PrintStruct *printStructs, int structSize);

void printRow(int row, PrintStruct *printStructs, int structSize, int width, int *beforeIndex);

int getMaximumFileDescriptor(int fd[][2], int size);

int calculateManhattenDisctance(int hx, int hy, int px, int py) {
    int result = 0;
    result = abs(px - hx) + abs(py - hy);
    return result;
}

int getIndexFromNearestPray(Hunter hunter, Pray prays[], int praySize) {
    int minimum = INT_MAX;
    int closestIndex = -1;
    for (int i = 0; i < praySize; ++i) {
        if (!prays[i].isLive)
            continue;
        int calculate = calculateManhattenDisctance(hunter.x, hunter.y, prays[i].x, prays[i].y);
        if (minimum > calculate) {
            minimum = calculate;
            closestIndex = i;
        }
    }
    return closestIndex;
}


int getIndexFromNearestHunter(Pray pray, Hunter hunter[], int hunterSize) {
    int maximum = 0;
    int closestIndex = -1;
    for (int i = 0; i < hunterSize; ++i) {
        if (!hunter[i].isLive)
            continue;
        int calculate = calculateManhattenDisctance(hunter[i].x, hunter[i].y, pray.x, pray.y);
        if (maximum < calculate) {
            maximum = calculate;
            closestIndex = i;
        }
    }
    return closestIndex;
}

void printServerMessage(server_message serverMessage) {
    printf("Pos = (%d,%d)\n", serverMessage.pos.x, serverMessage.pos.y);
    printf("Adv Pos = (%d,%d)\n", serverMessage.adv_pos.x, serverMessage.adv_pos.y);
    printf("object Count = %d\n", serverMessage.object_count);
    for (int i = 0; i < serverMessage.object_count; ++i) {
        printf("[%d] object Pos = (%d,%d)\n ", i, serverMessage.object_pos[i].x, serverMessage.object_pos[i].y);
    }
}

void deneme(int a[], int i) {
    a[i] = 4;
}

void arrangeObjectPositionFindObstacleForHunter(int sizeObstacle, Hunter hunters[], Obstacle obstacles[],
                                                server_message hunterMessage[], int hunterMessageIndex, int mapWidth,
                                                int mapHeight) {
    int j, i = hunterMessageIndex;
    for (j = 0; j < sizeObstacle; ++j) {
        /*
         *    (x-1,y) -> X
         *  (x,y-1) -> X H X <- (x,y+1)
         *    (x+1,y) -> X
         *
         *    first corner check,
         *    second top, left, bottom, right check
         *    ow check middle
         */


        if (!(hunters[j].isLive))
            continue;
        /*
         *  sol üst köşe
         *  +--
         *  |HX
         *  |X
         */
        if (hunters[i].x - 1 < 0 && hunters[i].y - 1 < 0) {
            if (hunters[i].y + 1 == obstacles[j].y && hunters[i].x == obstacles[j].x) {
                hunterMessage[i].object_pos[hunterMessage[i].object_count].x = obstacles[j].x;
                hunterMessage[i].object_pos[hunterMessage[i].object_count].y = obstacles[j].y;
                hunterMessage[i].object_count++;
                continue;
            }

            if (hunters[i].x + 1 == obstacles[j].x && hunters[i].y == obstacles[j].y) {
                hunterMessage[i].object_pos[hunterMessage[i].object_count].x = obstacles[j].x;
                hunterMessage[i].object_pos[hunterMessage[i].object_count].y = obstacles[j].y;
                hunterMessage[i].object_count++;
                continue;

            }
        }

            /*
             *  sağ üst köşe
             *  --+
             *  XH|
             *   X|
             */
        else if (hunters[i].y + 1 == mapWidth && hunters[i].x - 1 < 0) {
            if (hunters[i].y - 1 == obstacles[j].y && hunters[i].x == obstacles[j].x) {
                hunterMessage[i].object_pos[hunterMessage[i].object_count].x = obstacles[j].x;
                hunterMessage[i].object_pos[hunterMessage[i].object_count].y = obstacles[j].y;
                hunterMessage[i].object_count++;
                continue;
            }

            if (hunters[i].x + 1 == obstacles[j].x && hunters[i].y == obstacles[j].y) {
                hunterMessage[i].object_pos[hunterMessage[i].object_count].x = obstacles[j].x;
                hunterMessage[i].object_pos[hunterMessage[i].object_count].y = obstacles[j].y;
                hunterMessage[i].object_count++;
                continue;

            }
        }

            /*
             *  sağ alt köşe
             *   X|
             *  XH|
             *  --+
             */
        else if (hunters[i].y + 1 == mapWidth && hunters[i].x + 1 == mapHeight) {
            if (hunters[i].y - 1 == obstacles[j].y && hunters[i].x == obstacles[j].x) {
                hunterMessage[i].object_pos[hunterMessage[i].object_count].x = obstacles[j].x;
                hunterMessage[i].object_pos[hunterMessage[i].object_count].y = obstacles[j].y;
                hunterMessage[i].object_count++;
                continue;
            }

            if (hunters[i].x - 1 == obstacles[j].x && hunters[i].y == obstacles[j].y) {
                hunterMessage[i].object_pos[hunterMessage[i].object_count].x = obstacles[j].x;
                hunterMessage[i].object_pos[hunterMessage[i].object_count].y = obstacles[j].y;
                hunterMessage[i].object_count++;
                continue;

            }
        }


            /*
             *  sol alt köşe
             *  |X
             *  |HX
             *  +--
             */
        else if (hunters[i].y - 1 < 0 && hunters[i].x + 1 == mapHeight) {
            if (hunters[i].y + 1 == obstacles[j].y && hunters[i].x == obstacles[j].x) {
                hunterMessage[i].object_pos[hunterMessage[i].object_count].x = obstacles[j].x;
                hunterMessage[i].object_pos[hunterMessage[i].object_count].y = obstacles[j].y;
                hunterMessage[i].object_count++;
                continue;
            }

            if (hunters[i].x - 1 == obstacles[j].x && hunters[i].y == obstacles[j].y) {
                hunterMessage[i].object_pos[hunterMessage[i].object_count].x = obstacles[j].x;
                hunterMessage[i].object_pos[hunterMessage[i].object_count].y = obstacles[j].y;
                hunterMessage[i].object_count++;
                continue;

            }
        }

            /*
                 *  sol kenar
                 *  |X
                 *  |HX
                 *  |X
                 */
        else if (hunters[i].y - 1 < 0) {
            if (hunters[i].y + 1 == obstacles[j].y && hunters[i].x == obstacles[j].x) {
                hunterMessage[i].object_pos[hunterMessage[i].object_count].x = obstacles[j].x;
                hunterMessage[i].object_pos[hunterMessage[i].object_count].y = obstacles[j].y;
                hunterMessage[i].object_count++;
                continue;
            }

            if (hunters[i].x - 1 == obstacles[j].x && hunters[i].y == obstacles[j].y) {
                hunterMessage[i].object_pos[hunterMessage[i].object_count].x = obstacles[j].x;
                hunterMessage[i].object_pos[hunterMessage[i].object_count].y = obstacles[j].y;
                hunterMessage[i].object_count++;
                continue;

            }


            if (hunters[i].x + 1 == obstacles[j].x && hunters[i].y == obstacles[j].y) {
                hunterMessage[i].object_pos[hunterMessage[i].object_count].x = obstacles[j].x;
                hunterMessage[i].object_pos[hunterMessage[i].object_count].y = obstacles[j].y;
                hunterMessage[i].object_count++;
                continue;

            }


        }

            /*
                    *  üst kenar
                    *  ---
                    *  XHX
                    *   X
                    */
        else if (hunters[i].x - 1 < 0) {
            if (hunters[i].y + 1 == obstacles[j].y && hunters[i].x == obstacles[j].x) {
                hunterMessage[i].object_pos[hunterMessage[i].object_count].x = obstacles[j].x;
                hunterMessage[i].object_pos[hunterMessage[i].object_count].y = obstacles[j].y;
                hunterMessage[i].object_count++;
                continue;
            }


            if (hunters[i].x - 1 == obstacles[j].x && hunters[i].y == obstacles[j].y) {
                hunterMessage[i].object_pos[hunterMessage[i].object_count].x = obstacles[j].x;
                hunterMessage[i].object_pos[hunterMessage[i].object_count].y = obstacles[j].y;
                hunterMessage[i].object_count++;
                continue;

            }

            if (hunters[i].y - 1 == obstacles[j].y && hunters[i].x == obstacles[j].x) {
                hunterMessage[i].object_pos[hunterMessage[i].object_count].x = obstacles[j].x;
                hunterMessage[i].object_pos[hunterMessage[i].object_count].y = obstacles[j].y;
                hunterMessage[i].object_count++;
                continue;
            }

        }

            /*
                  *  sağ kenar
                  *   X|
                  *  XH|
                  *   X|
                  */
        else if (hunters[i].y + 1 == mapWidth) {


            if (hunters[i].x - 1 == obstacles[j].x && hunters[i].y == obstacles[j].y) {
                hunterMessage[i].object_pos[hunterMessage[i].object_count].x = obstacles[j].x;
                hunterMessage[i].object_pos[hunterMessage[i].object_count].y = obstacles[j].y;
                hunterMessage[i].object_count++;
                continue;

            }

            if (hunters[i].y - 1 == obstacles[j].y && hunters[i].x == obstacles[j].x) {
                hunterMessage[i].object_pos[hunterMessage[i].object_count].x = obstacles[j].x;
                hunterMessage[i].object_pos[hunterMessage[i].object_count].y = obstacles[j].y;
                hunterMessage[i].object_count++;
                continue;
            }


            if (hunters[i].x + 1 == obstacles[j].x && hunters[i].y == obstacles[j].y) {
                hunterMessage[i].object_pos[hunterMessage[i].object_count].x = obstacles[j].x;
                hunterMessage[i].object_pos[hunterMessage[i].object_count].y = obstacles[j].y;
                hunterMessage[i].object_count++;
                continue;

            }

        }

            /*
                    *  alt kenar
                    *   X
                    *  XHX
                    *  ---
                    */
        else if (hunters[i].x + 1 == mapHeight) {


            if (hunters[i].x - 1 == obstacles[j].x && hunters[i].y == obstacles[j].y) {
                hunterMessage[i].object_pos[hunterMessage[i].object_count].x = obstacles[j].x;
                hunterMessage[i].object_pos[hunterMessage[i].object_count].y = obstacles[j].y;
                hunterMessage[i].object_count++;
                continue;

            }

            if (hunters[i].y - 1 == obstacles[j].y && hunters[i].x == obstacles[j].x) {
                hunterMessage[i].object_pos[hunterMessage[i].object_count].x = obstacles[j].x;
                hunterMessage[i].object_pos[hunterMessage[i].object_count].y = obstacles[j].y;
                hunterMessage[i].object_count++;
                continue;
            }

            if (hunters[i].y + 1 == obstacles[j].y && hunters[i].x == obstacles[j].x) {
                hunterMessage[i].object_pos[hunterMessage[i].object_count].x = obstacles[j].x;
                hunterMessage[i].object_pos[hunterMessage[i].object_count].y = obstacles[j].y;
                hunterMessage[i].object_count++;
                continue;
            }

        } else {

            if (hunters[i].x - 1 == obstacles[j].x && hunters[i].y == obstacles[j].y) {
                hunterMessage[i].object_pos[hunterMessage[i].object_count].x = obstacles[j].x;
                hunterMessage[i].object_pos[hunterMessage[i].object_count].y = obstacles[j].y;
                hunterMessage[i].object_count++;
                continue;

            }


            if (hunters[i].x + 1 == obstacles[j].x && hunters[i].y == obstacles[j].y) {
                hunterMessage[i].object_pos[hunterMessage[i].object_count].x = obstacles[j].x;
                hunterMessage[i].object_pos[hunterMessage[i].object_count].y = obstacles[j].y;
                hunterMessage[i].object_count++;
                continue;

            }

            if (hunters[i].y - 1 == obstacles[j].y && hunters[i].x == obstacles[j].x) {
                hunterMessage[i].object_pos[hunterMessage[i].object_count].x = obstacles[j].x;
                hunterMessage[i].object_pos[hunterMessage[i].object_count].y = obstacles[j].y;
                hunterMessage[i].object_count++;
                continue;
            }

            if (hunters[i].y + 1 == obstacles[j].y && hunters[i].x == obstacles[j].x) {
                hunterMessage[i].object_pos[hunterMessage[i].object_count].x = obstacles[j].x;
                hunterMessage[i].object_pos[hunterMessage[i].object_count].y = obstacles[j].y;
                hunterMessage[i].object_count++;
                continue;
            }
        }


    }
}


void arrangeObjectPositionFindObstacleForPray(int sizeObstacle, Pray prays[], Obstacle obstacles[],
                                              server_message prayMessage[], int prayMessageIndex, int mapWidth,
                                              int mapHeight) {
    int j, i = prayMessageIndex;
    for (j = 0; j < sizeObstacle; ++j) {
        /*
         *    (x-1,y) -> X
         *  (x,y-1) -> X H X <- (x,y+1)
         *    (x+1,y) -> X
         *
         *    first corner check,
         *    second top, left, bottom, right check
         *    ow check middle
         */


        if (!(prays[j].isLive))
            continue;
        /*
         *  sol üst köşe
         *  +--
         *  |HX
         *  |X
         */
        if (prays[i].x - 1 < 0 && prays[i].y - 1 < 0) {
            if (prays[i].y + 1 == obstacles[j].y && prays[i].x == obstacles[j].x) {
                prayMessage[i].object_pos[prayMessage[i].object_count].x = obstacles[j].x;
                prayMessage[i].object_pos[prayMessage[i].object_count].y = obstacles[j].y;
                prayMessage[i].object_count++;
                continue;
            }

            if (prays[i].x + 1 == obstacles[j].x && prays[i].y == obstacles[j].y) {
                prayMessage[i].object_pos[prayMessage[i].object_count].x = obstacles[j].x;
                prayMessage[i].object_pos[prayMessage[i].object_count].y = obstacles[j].y;
                prayMessage[i].object_count++;
                continue;

            }
        }

            /*
             *  sağ üst köşe
             *  --+
             *  XH|
             *   X|
             */
        else if (prays[i].y + 1 == mapWidth && prays[i].x - 1 < 0) {
            if (prays[i].y - 1 == obstacles[j].y && prays[i].x == obstacles[j].x) {
                prayMessage[i].object_pos[prayMessage[i].object_count].x = obstacles[j].x;
                prayMessage[i].object_pos[prayMessage[i].object_count].y = obstacles[j].y;
                prayMessage[i].object_count++;
                continue;
            }

            if (prays[i].x + 1 == obstacles[j].x && prays[i].y == obstacles[j].y) {
                prayMessage[i].object_pos[prayMessage[i].object_count].x = obstacles[j].x;
                prayMessage[i].object_pos[prayMessage[i].object_count].y = obstacles[j].y;
                prayMessage[i].object_count++;
                continue;

            }
        }

            /*
             *  sağ alt köşe
             *   X|
             *  XH|
             *  --+
             */
        else if (prays[i].y + 1 == mapWidth && prays[i].x + 1 == mapHeight) {
            if (prays[i].y - 1 == obstacles[j].y && prays[i].x == obstacles[j].x) {
                prayMessage[i].object_pos[prayMessage[i].object_count].x = obstacles[j].x;
                prayMessage[i].object_pos[prayMessage[i].object_count].y = obstacles[j].y;
                prayMessage[i].object_count++;
                continue;
            }

            if (prays[i].x - 1 == obstacles[j].x && prays[i].y == obstacles[j].y) {
                prayMessage[i].object_pos[prayMessage[i].object_count].x = obstacles[j].x;
                prayMessage[i].object_pos[prayMessage[i].object_count].y = obstacles[j].y;
                prayMessage[i].object_count++;
                continue;

            }
        }


            /*
             *  sol alt köşe
             *  |X
             *  |HX
             *  +--
             */
        else if (prays[i].y - 1 < 0 && prays[i].x + 1 == mapHeight) {
            if (prays[i].y + 1 == obstacles[j].y && prays[i].x == obstacles[j].x) {
                prayMessage[i].object_pos[prayMessage[i].object_count].x = obstacles[j].x;
                prayMessage[i].object_pos[prayMessage[i].object_count].y = obstacles[j].y;
                prayMessage[i].object_count++;
                continue;
            }

            if (prays[i].x - 1 == obstacles[j].x && prays[i].y == obstacles[j].y) {
                prayMessage[i].object_pos[prayMessage[i].object_count].x = obstacles[j].x;
                prayMessage[i].object_pos[prayMessage[i].object_count].y = obstacles[j].y;
                prayMessage[i].object_count++;
                continue;

            }
        }

            /*
                 *  sol kenar
                 *  |X
                 *  |HX
                 *  |X
                 */
        else if (prays[i].y - 1 < 0) {
            if (prays[i].y + 1 == obstacles[j].y && prays[i].x == obstacles[j].x) {
                prayMessage[i].object_pos[prayMessage[i].object_count].x = obstacles[j].x;
                prayMessage[i].object_pos[prayMessage[i].object_count].y = obstacles[j].y;
                prayMessage[i].object_count++;
                continue;
            }

            if (prays[i].x - 1 == obstacles[j].x && prays[i].y == obstacles[j].y) {
                prayMessage[i].object_pos[prayMessage[i].object_count].x = obstacles[j].x;
                prayMessage[i].object_pos[prayMessage[i].object_count].y = obstacles[j].y;
                prayMessage[i].object_count++;
                continue;

            }


            if (prays[i].x + 1 == obstacles[j].x && prays[i].y == obstacles[j].y) {
                prayMessage[i].object_pos[prayMessage[i].object_count].x = obstacles[j].x;
                prayMessage[i].object_pos[prayMessage[i].object_count].y = obstacles[j].y;
                prayMessage[i].object_count++;
                continue;

            }


        }

            /*
                    *  üst kenar
                    *  ---
                    *  XHX
                    *   X
                    */
        else if (prays[i].x - 1 < 0) {
            if (prays[i].y + 1 == obstacles[j].y && prays[i].x == obstacles[j].x) {
                prayMessage[i].object_pos[prayMessage[i].object_count].x = obstacles[j].x;
                prayMessage[i].object_pos[prayMessage[i].object_count].y = obstacles[j].y;
                prayMessage[i].object_count++;
                continue;
            }


            if (prays[i].x - 1 == obstacles[j].x && prays[i].y == obstacles[j].y) {
                prayMessage[i].object_pos[prayMessage[i].object_count].x = obstacles[j].x;
                prayMessage[i].object_pos[prayMessage[i].object_count].y = obstacles[j].y;
                prayMessage[i].object_count++;
                continue;

            }

            if (prays[i].y - 1 == obstacles[j].y && prays[i].x == obstacles[j].x) {
                prayMessage[i].object_pos[prayMessage[i].object_count].x = obstacles[j].x;
                prayMessage[i].object_pos[prayMessage[i].object_count].y = obstacles[j].y;
                prayMessage[i].object_count++;
                continue;
            }

        }

            /*
                  *  sağ kenar
                  *   X|
                  *  XH|
                  *   X|
                  */
        else if (prays[i].y + 1 == mapWidth) {


            if (prays[i].x - 1 == obstacles[j].x && prays[i].y == obstacles[j].y) {
                prayMessage[i].object_pos[prayMessage[i].object_count].x = obstacles[j].x;
                prayMessage[i].object_pos[prayMessage[i].object_count].y = obstacles[j].y;
                prayMessage[i].object_count++;
                continue;

            }

            if (prays[i].y - 1 == obstacles[j].y && prays[i].x == obstacles[j].x) {
                prayMessage[i].object_pos[prayMessage[i].object_count].x = obstacles[j].x;
                prayMessage[i].object_pos[prayMessage[i].object_count].y = obstacles[j].y;
                prayMessage[i].object_count++;
                continue;
            }


            if (prays[i].x + 1 == obstacles[j].x && prays[i].y == obstacles[j].y) {
                prayMessage[i].object_pos[prayMessage[i].object_count].x = obstacles[j].x;
                prayMessage[i].object_pos[prayMessage[i].object_count].y = obstacles[j].y;
                prayMessage[i].object_count++;
                continue;

            }

        }

            /*
                    *  alt kenar
                    *   X
                    *  XHX
                    *  ---
                    */
        else if (prays[i].x + 1 == mapHeight) {


            if (prays[i].x - 1 == obstacles[j].x && prays[i].y == obstacles[j].y) {
                prayMessage[i].object_pos[prayMessage[i].object_count].x = obstacles[j].x;
                prayMessage[i].object_pos[prayMessage[i].object_count].y = obstacles[j].y;
                prayMessage[i].object_count++;
                continue;

            }

            if (prays[i].y - 1 == obstacles[j].y && prays[i].x == obstacles[j].x) {
                prayMessage[i].object_pos[prayMessage[i].object_count].x = obstacles[j].x;
                prayMessage[i].object_pos[prayMessage[i].object_count].y = obstacles[j].y;
                prayMessage[i].object_count++;
                continue;
            }

            if (prays[i].y + 1 == obstacles[j].y && prays[i].x == obstacles[j].x) {
                prayMessage[i].object_pos[prayMessage[i].object_count].x = obstacles[j].x;
                prayMessage[i].object_pos[prayMessage[i].object_count].y = obstacles[j].y;
                prayMessage[i].object_count++;
                continue;
            }

        } else {

            if (prays[i].x - 1 == obstacles[j].x && prays[i].y == obstacles[j].y) {
                prayMessage[i].object_pos[prayMessage[i].object_count].x = obstacles[j].x;
                prayMessage[i].object_pos[prayMessage[i].object_count].y = obstacles[j].y;
                prayMessage[i].object_count++;
                continue;

            }


            if (prays[i].x + 1 == obstacles[j].x && prays[i].y == obstacles[j].y) {
                prayMessage[i].object_pos[prayMessage[i].object_count].x = obstacles[j].x;
                prayMessage[i].object_pos[prayMessage[i].object_count].y = obstacles[j].y;
                prayMessage[i].object_count++;
                continue;

            }

            if (prays[i].y - 1 == obstacles[j].y && prays[i].x == obstacles[j].x) {
                prayMessage[i].object_pos[prayMessage[i].object_count].x = obstacles[j].x;
                prayMessage[i].object_pos[prayMessage[i].object_count].y = obstacles[j].y;
                prayMessage[i].object_count++;
                continue;
            }

            if (prays[i].y + 1 == obstacles[j].y && prays[i].x == obstacles[j].x) {
                prayMessage[i].object_pos[prayMessage[i].object_count].x = obstacles[j].x;
                prayMessage[i].object_pos[prayMessage[i].object_count].y = obstacles[j].y;
                prayMessage[i].object_count++;
                continue;
            }
        }


    }
}

void arrangeObjectPositionFindHunterForHunter(int sizeHunter, Hunter hunters[], server_message hunterMessage[],
                                              int hunterMessageIndex, int mapWidth, int mapHeight) {
    int i = hunterMessageIndex, j;
    for (j = 0; j < sizeHunter; ++j) {
        /*
         *    (x-1,y) -> X
         *  (x,y-1) -> X H X <- (x,y+1)
         *    (x+1,y) -> X
         *
         *    first corner check,
         *    second top, left, bottom, right check
         *    ow check middle
         */

        /*
         *  sol üst köşe
         *  +--
         *  |HX
         *  |X
         */

        if (!(hunters[j].isLive))
            continue;
        if (hunters[i].x - 1 < 0 && hunters[i].y - 1 < 0) {
            if (hunters[i].y + 1 == hunters[j].y && hunters[i].x == hunters[j].x) {
                hunterMessage[i].object_pos[hunterMessage[i].object_count].x = hunters[j].x;
                hunterMessage[i].object_pos[hunterMessage[i].object_count].y = hunters[j].y;
                hunterMessage[i].object_count++;
                continue;
            }

            if (hunters[i].x + 1 == hunters[j].x && hunters[i].y == hunters[j].y) {
                hunterMessage[i].object_pos[hunterMessage[i].object_count].x = hunters[j].x;
                hunterMessage[i].object_pos[hunterMessage[i].object_count].y = hunters[j].y;
                hunterMessage[i].object_count++;
                continue;

            }
        }

            /*
             *  sağ üst köşe
             *  --+
             *  XH|
             *   X|
             */
        else if (hunters[i].y + 1 == mapWidth && hunters[i].x - 1 < 0) {
            if (hunters[i].y - 1 == hunters[j].y && hunters[i].x == hunters[j].x) {
                hunterMessage[i].object_pos[hunterMessage[i].object_count].x = hunters[j].x;
                hunterMessage[i].object_pos[hunterMessage[i].object_count].y = hunters[j].y;
                hunterMessage[i].object_count++;
                continue;
            }

            if (hunters[i].x + 1 == hunters[j].x && hunters[i].y == hunters[j].y) {
                hunterMessage[i].object_pos[hunterMessage[i].object_count].x = hunters[j].x;
                hunterMessage[i].object_pos[hunterMessage[i].object_count].y = hunters[j].y;
                hunterMessage[i].object_count++;
                continue;

            }
        }

            /*
             *  sağ alt köşe
             *   X|
             *  XH|
             *  --+
             */
        else if (hunters[i].y + 1 == mapWidth && hunters[i].x + 1 == mapHeight) {
            if (hunters[i].y - 1 == hunters[j].y && hunters[i].x == hunters[j].x) {
                hunterMessage[i].object_pos[hunterMessage[i].object_count].x = hunters[j].x;
                hunterMessage[i].object_pos[hunterMessage[i].object_count].y = hunters[j].y;
                hunterMessage[i].object_count++;
                continue;
            }

            if (hunters[i].x - 1 == hunters[j].x && hunters[i].y == hunters[j].y) {
                hunterMessage[i].object_pos[hunterMessage[i].object_count].x = hunters[j].x;
                hunterMessage[i].object_pos[hunterMessage[i].object_count].y = hunters[j].y;
                hunterMessage[i].object_count++;
                continue;

            }
        }


            /*
             *  sol alt köşe
             *  |X
             *  |HX
             *  +--
             */
        else if (hunters[i].y - 1 < 0 && hunters[i].x + 1 == mapHeight) {
            if (hunters[i].y + 1 == hunters[j].y && hunters[i].x == hunters[j].x) {
                hunterMessage[i].object_pos[hunterMessage[i].object_count].x = hunters[j].x;
                hunterMessage[i].object_pos[hunterMessage[i].object_count].y = hunters[j].y;
                hunterMessage[i].object_count++;
                continue;
            }

            if (hunters[i].x - 1 == hunters[j].x && hunters[i].y == hunters[j].y) {
                hunterMessage[i].object_pos[hunterMessage[i].object_count].x = hunters[j].x;
                hunterMessage[i].object_pos[hunterMessage[i].object_count].y = hunters[j].y;
                hunterMessage[i].object_count++;
                continue;

            }
        }

            /*
                 *  sol kenar
                 *  |X
                 *  |HX
                 *  |X
                 */
        else if (hunters[i].y - 1 < 0) {
            if (hunters[i].y + 1 == hunters[j].y && hunters[i].x == hunters[j].x) {
                hunterMessage[i].object_pos[hunterMessage[i].object_count].x = hunters[j].x;
                hunterMessage[i].object_pos[hunterMessage[i].object_count].y = hunters[j].y;
                hunterMessage[i].object_count++;
                continue;
            }

            if (hunters[i].x - 1 == hunters[j].x && hunters[i].y == hunters[j].y) {
                hunterMessage[i].object_pos[hunterMessage[i].object_count].x = hunters[j].x;
                hunterMessage[i].object_pos[hunterMessage[i].object_count].y = hunters[j].y;
                hunterMessage[i].object_count++;
                continue;

            }


            if (hunters[i].x + 1 == hunters[j].x && hunters[i].y == hunters[j].y) {
                hunterMessage[i].object_pos[hunterMessage[i].object_count].x = hunters[j].x;
                hunterMessage[i].object_pos[hunterMessage[i].object_count].y = hunters[j].y;
                hunterMessage[i].object_count++;
                continue;

            }


        }

            /*
                    *  üst kenar
                    *  ---
                    *  XHX
                    *   X
                    */
        else if (hunters[i].x - 1 < 0) {
            if (hunters[i].y + 1 == hunters[j].y && hunters[i].x == hunters[j].x) {
                hunterMessage[i].object_pos[hunterMessage[i].object_count].x = hunters[j].x;
                hunterMessage[i].object_pos[hunterMessage[i].object_count].y = hunters[j].y;
                hunterMessage[i].object_count++;
                continue;
            }


            if (hunters[i].x - 1 == hunters[j].x && hunters[i].y == hunters[j].y) {
                hunterMessage[i].object_pos[hunterMessage[i].object_count].x = hunters[j].x;
                hunterMessage[i].object_pos[hunterMessage[i].object_count].y = hunters[j].y;
                hunterMessage[i].object_count++;
                continue;

            }

            if (hunters[i].y - 1 == hunters[j].y && hunters[i].x == hunters[j].x) {
                hunterMessage[i].object_pos[hunterMessage[i].object_count].x = hunters[j].x;
                hunterMessage[i].object_pos[hunterMessage[i].object_count].y = hunters[j].y;
                hunterMessage[i].object_count++;
                continue;
            }

        }

            /*
                  *  sağ kenar
                  *   X|
                  *  XH|
                  *   X|
                  */
        else if (hunters[i].y + 1 == mapWidth) {


            if (hunters[i].x - 1 == hunters[j].x && hunters[i].y == hunters[j].y) {
                hunterMessage[i].object_pos[hunterMessage[i].object_count].x = hunters[j].x;
                hunterMessage[i].object_pos[hunterMessage[i].object_count].y = hunters[j].y;
                hunterMessage[i].object_count++;
                continue;

            }

            if (hunters[i].y - 1 == hunters[j].y && hunters[i].x == hunters[j].x) {
                hunterMessage[i].object_pos[hunterMessage[i].object_count].x = hunters[j].x;
                hunterMessage[i].object_pos[hunterMessage[i].object_count].y = hunters[j].y;
                hunterMessage[i].object_count++;
                continue;
            }


            if (hunters[i].x + 1 == hunters[j].x && hunters[i].y == hunters[j].y) {
                hunterMessage[i].object_pos[hunterMessage[i].object_count].x = hunters[j].x;
                hunterMessage[i].object_pos[hunterMessage[i].object_count].y = hunters[j].y;
                hunterMessage[i].object_count++;
                continue;

            }

        }

            /*
                    *  alt kenar
                    *   X
                    *  XHX
                    *  ---
                    */
        else if (hunters[i].x + 1 == mapHeight) {


            if (hunters[i].x - 1 == hunters[j].x && hunters[i].y == hunters[j].y) {
                hunterMessage[i].object_pos[hunterMessage[i].object_count].x = hunters[j].x;
                hunterMessage[i].object_pos[hunterMessage[i].object_count].y = hunters[j].y;
                hunterMessage[i].object_count++;
                continue;

            }

            if (hunters[i].y - 1 == hunters[j].y && hunters[i].x == hunters[j].x) {
                hunterMessage[i].object_pos[hunterMessage[i].object_count].x = hunters[j].x;
                hunterMessage[i].object_pos[hunterMessage[i].object_count].y = hunters[j].y;
                hunterMessage[i].object_count++;
                continue;
            }

            if (hunters[i].y + 1 == hunters[j].y && hunters[i].x == hunters[j].x) {
                hunterMessage[i].object_pos[hunterMessage[i].object_count].x = hunters[j].x;
                hunterMessage[i].object_pos[hunterMessage[i].object_count].y = hunters[j].y;
                hunterMessage[i].object_count++;
                continue;
            }

        } else {

            if (hunters[i].x - 1 == hunters[j].x && hunters[i].y == hunters[j].y) {
                hunterMessage[i].object_pos[hunterMessage[i].object_count].x = hunters[j].x;
                hunterMessage[i].object_pos[hunterMessage[i].object_count].y = hunters[j].y;
                hunterMessage[i].object_count++;
                continue;

            }


            if (hunters[i].x + 1 == hunters[j].x && hunters[i].y == hunters[j].y) {
                hunterMessage[i].object_pos[hunterMessage[i].object_count].x = hunters[j].x;
                hunterMessage[i].object_pos[hunterMessage[i].object_count].y = hunters[j].y;
                hunterMessage[i].object_count++;
                continue;

            }

            if (hunters[i].y - 1 == hunters[j].y && hunters[i].x == hunters[j].x) {
                hunterMessage[i].object_pos[hunterMessage[i].object_count].x = hunters[j].x;
                hunterMessage[i].object_pos[hunterMessage[i].object_count].y = hunters[j].y;
                hunterMessage[i].object_count++;
                continue;
            }

            if (hunters[i].y + 1 == hunters[j].y && hunters[i].x == hunters[j].x) {
                hunterMessage[i].object_pos[hunterMessage[i].object_count].x = hunters[j].x;
                hunterMessage[i].object_pos[hunterMessage[i].object_count].y = hunters[j].y;
                hunterMessage[i].object_count++;
                continue;
            }
        }


    }
};

void arrangeObjectPositionFindPrayForPray(int sizePray, Pray prays[], server_message prayMessage[],
                                          int prayMessageIndex, int mapWidth, int mapHeight) {
    int i = prayMessageIndex, j;
    for (j = 0; j < sizePray; ++j) {
        /*
         *    (x-1,y) -> X
         *  (x,y-1) -> X H X <- (x,y+1)
         *    (x+1,y) -> X
         *
         *    first corner check,
         *    second top, left, bottom, right check
         *    ow check middle
         */

        /*
         *  sol üst köşe
         *  +--
         *  |HX
         *  |X
         */

        if (!(prays[j].isLive))
            continue;
        if (prays[i].x - 1 < 0 && prays[i].y - 1 < 0) {
            if (prays[i].y + 1 == prays[j].y && prays[i].x == prays[j].x) {
                prayMessage[i].object_pos[prayMessage[i].object_count].x = prays[j].x;
                prayMessage[i].object_pos[prayMessage[i].object_count].y = prays[j].y;
                prayMessage[i].object_count++;
                continue;
            }

            if (prays[i].x + 1 == prays[j].x && prays[i].y == prays[j].y) {
                prayMessage[i].object_pos[prayMessage[i].object_count].x = prays[j].x;
                prayMessage[i].object_pos[prayMessage[i].object_count].y = prays[j].y;
                prayMessage[i].object_count++;
                continue;

            }
        }

            /*
             *  sağ üst köşe
             *  --+
             *  XH|
             *   X|
             */
        else if (prays[i].y + 1 == mapWidth && prays[i].x - 1 < 0) {
            if (prays[i].y - 1 == prays[j].y && prays[i].x == prays[j].x) {
                prayMessage[i].object_pos[prayMessage[i].object_count].x = prays[j].x;
                prayMessage[i].object_pos[prayMessage[i].object_count].y = prays[j].y;
                prayMessage[i].object_count++;
                continue;
            }

            if (prays[i].x + 1 == prays[j].x && prays[i].y == prays[j].y) {
                prayMessage[i].object_pos[prayMessage[i].object_count].x = prays[j].x;
                prayMessage[i].object_pos[prayMessage[i].object_count].y = prays[j].y;
                prayMessage[i].object_count++;
                continue;

            }
        }

            /*
             *  sağ alt köşe
             *   X|
             *  XH|
             *  --+
             */
        else if (prays[i].y + 1 == mapWidth && prays[i].x + 1 == mapHeight) {
            if (prays[i].y - 1 == prays[j].y && prays[i].x == prays[j].x) {
                prayMessage[i].object_pos[prayMessage[i].object_count].x = prays[j].x;
                prayMessage[i].object_pos[prayMessage[i].object_count].y = prays[j].y;
                prayMessage[i].object_count++;
                continue;
            }

            if (prays[i].x - 1 == prays[j].x && prays[i].y == prays[j].y) {
                prayMessage[i].object_pos[prayMessage[i].object_count].x = prays[j].x;
                prayMessage[i].object_pos[prayMessage[i].object_count].y = prays[j].y;
                prayMessage[i].object_count++;
                continue;

            }
        }


            /*
             *  sol alt köşe
             *  |X
             *  |HX
             *  +--
             */
        else if (prays[i].y - 1 < 0 && prays[i].x + 1 == mapHeight) {
            if (prays[i].y + 1 == prays[j].y && prays[i].x == prays[j].x) {
                prayMessage[i].object_pos[prayMessage[i].object_count].x = prays[j].x;
                prayMessage[i].object_pos[prayMessage[i].object_count].y = prays[j].y;
                prayMessage[i].object_count++;
                continue;
            }

            if (prays[i].x - 1 == prays[j].x && prays[i].y == prays[j].y) {
                prayMessage[i].object_pos[prayMessage[i].object_count].x = prays[j].x;
                prayMessage[i].object_pos[prayMessage[i].object_count].y = prays[j].y;
                prayMessage[i].object_count++;
                continue;

            }
        }

            /*
                 *  sol kenar
                 *  |X
                 *  |HX
                 *  |X
                 */
        else if (prays[i].y - 1 < 0) {
            if (prays[i].y + 1 == prays[j].y && prays[i].x == prays[j].x) {
                prayMessage[i].object_pos[prayMessage[i].object_count].x = prays[j].x;
                prayMessage[i].object_pos[prayMessage[i].object_count].y = prays[j].y;
                prayMessage[i].object_count++;
                continue;
            }

            if (prays[i].x - 1 == prays[j].x && prays[i].y == prays[j].y) {
                prayMessage[i].object_pos[prayMessage[i].object_count].x = prays[j].x;
                prayMessage[i].object_pos[prayMessage[i].object_count].y = prays[j].y;
                prayMessage[i].object_count++;
                continue;

            }


            if (prays[i].x + 1 == prays[j].x && prays[i].y == prays[j].y) {
                prayMessage[i].object_pos[prayMessage[i].object_count].x = prays[j].x;
                prayMessage[i].object_pos[prayMessage[i].object_count].y = prays[j].y;
                prayMessage[i].object_count++;
                continue;

            }


        }

            /*
                    *  üst kenar
                    *  ---
                    *  XHX
                    *   X
                    */
        else if (prays[i].x - 1 < 0) {
            if (prays[i].y + 1 == prays[j].y && prays[i].x == prays[j].x) {
                prayMessage[i].object_pos[prayMessage[i].object_count].x = prays[j].x;
                prayMessage[i].object_pos[prayMessage[i].object_count].y = prays[j].y;
                prayMessage[i].object_count++;
                continue;
            }


            if (prays[i].x - 1 == prays[j].x && prays[i].y == prays[j].y) {
                prayMessage[i].object_pos[prayMessage[i].object_count].x = prays[j].x;
                prayMessage[i].object_pos[prayMessage[i].object_count].y = prays[j].y;
                prayMessage[i].object_count++;
                continue;

            }

            if (prays[i].y - 1 == prays[j].y && prays[i].x == prays[j].x) {
                prayMessage[i].object_pos[prayMessage[i].object_count].x = prays[j].x;
                prayMessage[i].object_pos[prayMessage[i].object_count].y = prays[j].y;
                prayMessage[i].object_count++;
                continue;
            }

        }

            /*
                  *  sağ kenar
                  *   X|
                  *  XH|
                  *   X|
                  */
        else if (prays[i].y + 1 == mapWidth) {


            if (prays[i].x - 1 == prays[j].x && prays[i].y == prays[j].y) {
                prayMessage[i].object_pos[prayMessage[i].object_count].x = prays[j].x;
                prayMessage[i].object_pos[prayMessage[i].object_count].y = prays[j].y;
                prayMessage[i].object_count++;
                continue;

            }

            if (prays[i].y - 1 == prays[j].y && prays[i].x == prays[j].x) {
                prayMessage[i].object_pos[prayMessage[i].object_count].x = prays[j].x;
                prayMessage[i].object_pos[prayMessage[i].object_count].y = prays[j].y;
                prayMessage[i].object_count++;
                continue;
            }


            if (prays[i].x + 1 == prays[j].x && prays[i].y == prays[j].y) {
                prayMessage[i].object_pos[prayMessage[i].object_count].x = prays[j].x;
                prayMessage[i].object_pos[prayMessage[i].object_count].y = prays[j].y;
                prayMessage[i].object_count++;
                continue;

            }

        }

            /*
                    *  alt kenar
                    *   X
                    *  XHX
                    *  ---
                    */
        else if (prays[i].x + 1 == mapHeight) {


            if (prays[i].x - 1 == prays[j].x && prays[i].y == prays[j].y) {
                prayMessage[i].object_pos[prayMessage[i].object_count].x = prays[j].x;
                prayMessage[i].object_pos[prayMessage[i].object_count].y = prays[j].y;
                prayMessage[i].object_count++;
                continue;

            }

            if (prays[i].y - 1 == prays[j].y && prays[i].x == prays[j].x) {
                prayMessage[i].object_pos[prayMessage[i].object_count].x = prays[j].x;
                prayMessage[i].object_pos[prayMessage[i].object_count].y = prays[j].y;
                prayMessage[i].object_count++;
                continue;
            }

            if (prays[i].y + 1 == prays[j].y && prays[i].x == prays[j].x) {
                prayMessage[i].object_pos[prayMessage[i].object_count].x = prays[j].x;
                prayMessage[i].object_pos[prayMessage[i].object_count].y = prays[j].y;
                prayMessage[i].object_count++;
                continue;
            }

        } else {

            if (prays[i].x - 1 == prays[j].x && prays[i].y == prays[j].y) {
                prayMessage[i].object_pos[prayMessage[i].object_count].x = prays[j].x;
                prayMessage[i].object_pos[prayMessage[i].object_count].y = prays[j].y;
                prayMessage[i].object_count++;
                continue;

            }


            if (prays[i].x + 1 == prays[j].x && prays[i].y == prays[j].y) {
                prayMessage[i].object_pos[prayMessage[i].object_count].x = prays[j].x;
                prayMessage[i].object_pos[prayMessage[i].object_count].y = prays[j].y;
                prayMessage[i].object_count++;
                continue;

            }

            if (prays[i].y - 1 == prays[j].y && prays[i].x == prays[j].x) {
                prayMessage[i].object_pos[prayMessage[i].object_count].x = prays[j].x;
                prayMessage[i].object_pos[prayMessage[i].object_count].y = prays[j].y;
                prayMessage[i].object_count++;
                continue;
            }

            if (prays[i].y + 1 == prays[j].y && prays[i].x == prays[j].x) {
                prayMessage[i].object_pos[prayMessage[i].object_count].x = prays[j].x;
                prayMessage[i].object_pos[prayMessage[i].object_count].y = prays[j].y;
                prayMessage[i].object_count++;
                continue;
            }
        }


    }
};

int
getSizeAndArrangePrintStruct(PrintStruct *printStructs, Hunter *hunters, Obstacle *obstacles, Pray *prays,
                             int sizeHunter,
                             int sizePray, int sizeObstacle) {

    int i, printStructIndex = 0;

    for (i = 0; i < sizeObstacle; ++i, printStructIndex++) {
        printStructs[printStructIndex].x = obstacles[i].x;
        printStructs[printStructIndex].y = obstacles[i].y;
        printStructs[printStructIndex].mode = 'X';
    }

    for (i = 0; i < sizeHunter; i++) {
        if (hunters[i].isLive) {
            printStructs[printStructIndex].x = hunters[i].x;
            printStructs[printStructIndex].y = hunters[i].y;
            printStructs[printStructIndex].mode = 'H';
            printStructIndex++;
        }
//        printf("hunter[%d] isLive:%d\n", i, hunters[i].isLive);
    }

    for (i = 0; i < sizePray; i++) {
        if (prays[i].isLive) {
            printStructs[printStructIndex].x = prays[i].x;
            printStructs[printStructIndex].y = prays[i].y;
            printStructs[printStructIndex].mode = 'P';
            printStructIndex++;
        }
    }

    return printStructIndex;

}

int CanMoveNextForHunter(int index, ph_message receivingMessageFromHunter, Hunter hunters[], PrintStruct printStructs[],
                         int totalSize) {
    int canGoNextStep = -1;
    if (hunters[index].isLive) {
        if (hunters[index].energy > 0) {
            for (int i = 0; i < totalSize; ++i) {
                /*
                 * engel var mı yok mu kontrol et.
                 */



                if (receivingMessageFromHunter.move_request.x == printStructs[i].x &&
                    receivingMessageFromHunter.move_request.y == printStructs[i].y &&
                    (printStructs[i].mode == 'X' || printStructs[i].mode == 'H')) {
                    canGoNextStep = 0;
                    break;
                } else
                    canGoNextStep = 1;
            }
            if (canGoNextStep == 1) {
                //hareket ettir.
                hunters[index].energy--;
                hunters[index].x = receivingMessageFromHunter.move_request.x;
                hunters[index].y = receivingMessageFromHunter.move_request.y;


            }
        }
    }

    return canGoNextStep;

}

int CanMoveNextForPray(int index, ph_message receivingMessageFromPray, Pray pray[], PrintStruct printStructs[],
                       int totalSize) {
    int canGoNextStep = -1;
    if (pray[index].isLive) {


        for (int i = 0; i < totalSize; ++i) {
            /*
             * engel var mı yok mu kontrol et.
             */



            if (receivingMessageFromPray.move_request.x == printStructs[i].x &&
                receivingMessageFromPray.move_request.y == printStructs[i].y &&
                (printStructs[i].mode == 'X' || printStructs[i].mode == 'P')) {
                canGoNextStep = 0;
                break;
            } else
                canGoNextStep = 1;
        }
        if (canGoNextStep == 1) {
            //hareket ettir.
            pray[index].x = receivingMessageFromPray.move_request.x;
            pray[index].y = receivingMessageFromPray.move_request.y;


        }
    }

    return canGoNextStep;

}

int checkHunterKillPray(int hunterIndex, Hunter hunter[], Pray pray[], int *praySize, int *killedIndex,
                        int *counterPraySize, int *counterHunterSize) {
    *killedIndex = -1;
    int isPrayKilled = 0;
    int killedPrayIndex = -1;
//    printf("Hunter Info: x:%d, y:%d, e:%d, isLive:%d\n", hunter[hunterIndex].x, hunter[hunterIndex].y,
//           hunter[hunterIndex].energy, hunter[hunterIndex].isLive);

    for (int i = 0; i < *praySize; ++i) {
        if (hunter[hunterIndex].x == pray[i].x && hunter[hunterIndex].y == pray[i].y && pray[i].isLive) {
//            printf("hunter[%d] => x=%d, y=%d\n",hunterIndex,pray[i].x, pray[i].y);
            pray[i].isLive = 0;
            isPrayKilled = 1;
            killedPrayIndex = i;
            (*counterPraySize)--;
            hunter[hunterIndex].energy += pray[i].energy;
//            printf("Hunter captured pray. hE: %d, pE:%d\n", hunter[hunterIndex].energy, pray[i].energy);
            break;
        }
    }
//    printf("Hunter Info-BeforeEnergyControl: x:%d, y:%d, e:%d, isLive:%d\n", hunter[hunterIndex].x,
//           hunter[hunterIndex].y, hunter[hunterIndex].energy, hunter[hunterIndex].isLive);

    // eğer geldiği yerde pray yoksa o zaman prayin enerjisi 0 ise öldür.
    if (!isPrayKilled) {
        if (hunter[hunterIndex].energy == 0) {
//            printf("[%d]Hunter die!\n", hunterIndex);
            hunter[hunterIndex].isLive = 0;
            (*counterHunterSize)--;
            (*killedIndex) = hunterIndex;
        }
    }

    return killedPrayIndex;

//        printf("Hunter Info: x:%d, y:%d, e:%d, isLive:%d\n", hunter[hunterIndex].x, hunter[hunterIndex].y,
//           hunter[hunterIndex].energy, hunter[hunterIndex].isLive);

}

//MAIN
int main(int argc, char const *argv[]) {
    int mapWidth, mapHeight;
    int sizeObstacle = 0, sizeHunter = 0, sizePray = 0, i, x, y, e, totalSize = 0;
    PrintStruct *printStructs;
//    FILE *fp;
    char buff[255];
    int tempSize, totalLive = 0;

//    fp = fopen("inputClosest.txt", "r");


    scanf("%d %d", &mapWidth, &mapHeight);


    scanf("%d", &sizeObstacle);
    Obstacle obstacles[sizeObstacle];
    totalSize = sizeObstacle;
    for (i = 0; i < sizeObstacle; i++) {
        scanf("%d %d", &x, &y);
        obstacles[i].x = x;
        obstacles[i].y = y;

    }

    scanf("%d", &sizeHunter);
    Hunter hunters[sizeHunter];
    tempSize = sizeHunter + totalSize;
    for (i = 0; i < sizeHunter; i++) {
        scanf("%d %d %d", &x, &y, &e);
        hunters[i].x = x;
        hunters[i].y = y;
        hunters[i].energy = e;
        hunters[i].isLive = 1;

        totalLive++;
    }
    totalSize += sizeHunter;


    scanf("%d", &sizePray);
    Pray prays[sizePray];
    tempSize = sizePray + totalSize;
    for (i = 0; i < sizePray; i++) {
        scanf("%d %d %d", &x, &y, &e);
        prays[i].x = x;
        prays[i].y = y;
        prays[i].energy = e;
        prays[i].isLive = 1;

        totalLive++;
    }

    totalSize += sizePray;

    printStructs = (PrintStruct *) malloc(totalSize * sizeof(PrintStruct));

    totalSize = getSizeAndArrangePrintStruct(printStructs, hunters, obstacles, prays, sizeHunter, sizePray,
                                             sizeObstacle);


    sortPrintStructs(printStructs, totalSize);
    printAll(printStructs, totalSize);

    printOutput(mapWidth, mapHeight, printStructs, totalSize);

//    fclose(fp);



    printf("it's starting\n");


    char width[50] = "";
    int length = snprintf(NULL, 0, "%d", mapWidth);
    char *str = malloc((size_t) (length + 1));
    snprintf(str, (size_t) (length + 1), "%d", mapWidth);
    strcat(width, str);
    free(str);

    char height[50] = "";
    length = snprintf(NULL, 0, "%d", mapHeight);
    char *str2 = malloc((size_t) (length + 1));
    snprintf(str, (size_t) (length + 1), "%d", mapHeight);
    strcat(height, str);
    free(str2);


    server_message hunterMessage[sizeHunter];
    server_message prayMessage[sizePray];
    for (i = 0; i < sizeHunter; i++) {
        hunterMessage[i].pos.x = hunters[i].x;
        hunterMessage[i].pos.y = hunters[i].y;

        int closestIndex = getIndexFromNearestPray(hunters[i], prays, sizePray);
        hunterMessage[i].adv_pos.x = prays[closestIndex].x;
        hunterMessage[i].adv_pos.y = prays[closestIndex].y;

        hunterMessage[i].object_count = 0;

        arrangeObjectPositionFindHunterForHunter(sizeHunter, hunters, hunterMessage, i, mapWidth, mapHeight);
        arrangeObjectPositionFindObstacleForHunter(sizeObstacle, hunters, obstacles, hunterMessage, i, mapWidth,
                                                   mapHeight);


    }

    for (i = 0; i < sizePray; i++) {
        prayMessage[i].pos.x = prays[i].x;
        prayMessage[i].pos.y = prays[i].y;

        int closestIndex = getIndexFromNearestHunter(prays[i], hunters, sizeHunter);
        prayMessage[i].adv_pos.x = hunters[closestIndex].x;
        prayMessage[i].adv_pos.y = hunters[closestIndex].y;

        prayMessage[i].object_count = 0;


        arrangeObjectPositionFindPrayForPray(sizePray, prays, prayMessage, i, mapWidth, mapHeight);
        arrangeObjectPositionFindObstacleForPray(sizeObstacle, prays, obstacles, prayMessage, i, mapWidth,
                                                 mapHeight);


    }


    pid_t pid[sizeHunter];
    pid_t pidPray[sizePray];
    int fd[sizeHunter][2];
    int fdPray[sizePray][2];
    for (i = 0; i < sizeHunter; i++) {
        PIPE(fd[i]);
        pid[i] = fork();
        if (pid[i] == 0) {
            //it is child;
            fflush(stdout);
            dup2(fd[i][1], 0);
            execl("hunter", "hunter", width, height, NULL, NULL);

        }


    }

    for (i = 0; i < sizePray; i++) {
        PIPE(fdPray[i]);
        pidPray[i] = fork();
        if (pidPray[i] == 0) {
            //it is child;
            fflush(stdout);
            dup2(fdPray[i][1], 0);
            execl("prey", "prey", width, height, NULL, NULL);

        }


    }


    fd_set readset;
    int open[sizeHunter];
    int openPray[sizePray];
    int sizeOpen = sizeHunter;
    int sizeOpenPray = sizePray;
    for (i = 0; i < sizeHunter; i++) {
        open[i] = 1;
    }

    for (i = 0; i < sizePray; i++) {
        openPray[i] = 1;
    }
    int childStatus[sizeHunter];
    int childStatusForPray[sizePray];
    int maximum = getMaximumFileDescriptor(fd, sizeHunter);
    int maximum2 = getMaximumFileDescriptor(fdPray, sizePray);

    if (maximum2 > maximum)
        maximum = maximum2;


    for (i = 0; i < sizeHunter; i++) {

        write(fd[i][0], &hunterMessage[i], sizeof(server_message));
    }

    for (i = 0; i < sizePray; i++) {

        write(fdPray[i][0], &prayMessage[i], sizeof(server_message));
    }

    ph_message receivingMessageFromHunter[sizeHunter];
    ph_message receivingMessageFromPray[sizePray];

    int counterPraySize = sizePray;
    int counterHunterSize = sizeHunter;

    while (sizeOpen > 0 && sizeOpenPray > 0 && counterHunterSize > 0 && counterPraySize > 0) {
        FD_ZERO(&readset);
        for (i = 0; i < sizeHunter; i++) {
            if (open[i])
                FD_SET(fd[i][0], &readset);

        }

        for (i = 0; i < sizePray; i++) {
            if (openPray[i])
                FD_SET(fdPray[i][0], &readset);

        }
        select(maximum, &readset, NULL, NULL, NULL);  /* no wset, eset, timeout */
        for (i = 0; i < sizeHunter; i++) {
            if (FD_ISSET(fd[i][0], &readset)) {


//                printf("--------BEGIN HUNTER [%d]-------\n", i);
                int killedIndex = -1;
                read(fd[i][0], &receivingMessageFromHunter[i], sizeof(ph_message));
//                printf("Request Hunter(%d,%d)\n", receivingMessageFromHunter[i].move_request.x,
//                       receivingMessageFromHunter[i].move_request.y);
                if (receivingMessageFromHunter[i].move_request.y == -1 &&
                    receivingMessageFromHunter[i].move_request.x == -1) {
                    printStructs = (PrintStruct *) realloc(printStructs, (totalSize * sizeof(PrintStruct)));

                    totalSize = getSizeAndArrangePrintStruct(printStructs, hunters, obstacles, prays, sizeHunter,
                                                             sizePray,
                                                             sizeObstacle);

                    sortPrintStructs(printStructs, totalSize);
                    printAll(printStructs, totalSize);

                    printOutput(mapWidth, mapHeight, printStructs, totalSize);


                    hunterMessage[i].pos.x = hunters[i].x;
                    hunterMessage[i].pos.y = hunters[i].y;

                    int closestIndex = getIndexFromNearestPray(hunters[i], prays, sizePray);
                    if (closestIndex == -1) {
                        open[i] = 0;
                        sizeOpen--;
                        kill(pid[i], SIGTERM);
                        waitpid(pid[i], &(childStatus[i]), 0);
                        printf("Closed Hunter[%d]\n", i);
                    }
                    hunterMessage[i].adv_pos.x = prays[closestIndex].x;
                    hunterMessage[i].adv_pos.y = prays[closestIndex].y;

                    hunterMessage[i].object_count = 0;

                    arrangeObjectPositionFindHunterForHunter(sizeHunter, hunters, hunterMessage, i, mapWidth,
                                                             mapHeight);
                    arrangeObjectPositionFindObstacleForHunter(sizeObstacle, hunters, obstacles, hunterMessage, i,
                                                               mapWidth,
                                                               mapHeight);

                    fflush(stdout);
                    write(fd[i][0], &hunterMessage[i], sizeof(server_message));

                    continue;
                }

                int canMoveAnswer = CanMoveNextForHunter(i, receivingMessageFromHunter[i], hunters, printStructs,
                                                         totalSize);
                if (canMoveAnswer == 0) {


                    printStructs = (PrintStruct *) realloc(printStructs, (totalSize * sizeof(PrintStruct)));

                    totalSize = getSizeAndArrangePrintStruct(printStructs, hunters, obstacles, prays, sizeHunter,
                                                             sizePray,
                                                             sizeObstacle);


                    sortPrintStructs(printStructs, totalSize);
                    printAll(printStructs, totalSize);

                    printOutput(mapWidth, mapHeight, printStructs, totalSize);


                    hunterMessage[i].pos.x = hunters[i].x;
                    hunterMessage[i].pos.y = hunters[i].y;

                    int closestIndex = getIndexFromNearestPray(hunters[i], prays, sizePray);
                    if (closestIndex == -1) {
                        open[i] = 0;
                        sizeOpen--;
                        kill(pid[i], SIGTERM);
                        waitpid(pid[i], &(childStatus[i]), 0);
                        printf("Closed Hunter[%d]\n", i);
                    }
                    hunterMessage[i].adv_pos.x = prays[closestIndex].x;
                    hunterMessage[i].adv_pos.y = prays[closestIndex].y;

                    hunterMessage[i].object_count = 0;

                    arrangeObjectPositionFindHunterForHunter(sizeHunter, hunters, hunterMessage, i, mapWidth,
                                                             mapHeight);
                    arrangeObjectPositionFindObstacleForHunter(sizeObstacle, hunters, obstacles, hunterMessage, i,
                                                               mapWidth,
                                                               mapHeight);

//                    printServerMessage(hunterMessage[i]);
                    fflush(stdout);
                    write(fd[i][0], &hunterMessage[i], sizeof(server_message));


                } else {
                    int prayKilledIndex = checkHunterKillPray(i, hunters, prays, &sizePray, &killedIndex,
                                                              &counterPraySize,
                                                              &counterHunterSize);
                    if (prayKilledIndex != -1) {
                        openPray[prayKilledIndex] = 0;
                        sizeOpenPray--;
                        kill(pidPray[prayKilledIndex], SIGTERM);
                        waitpid(pidPray[prayKilledIndex], &(childStatusForPray[prayKilledIndex]), 0);
                        printf("Closed Pray[%d]\n", prayKilledIndex);
                    }

                    if (killedIndex != -1) {
                        open[killedIndex] = 0;
                        sizeOpen--;
                        kill(pid[killedIndex], SIGTERM);
                        waitpid(pid[killedIndex], &(childStatus[killedIndex]), 0);
                        printf("Closed Hunter[%d]\n", killedIndex);


                    } else {
                        printStructs = (PrintStruct *) realloc(printStructs, (totalSize * sizeof(PrintStruct)));

                        totalSize = getSizeAndArrangePrintStruct(printStructs, hunters, obstacles, prays, sizeHunter,
                                                                 sizePray,
                                                                 sizeObstacle);


                        sortPrintStructs(printStructs, totalSize);
                        printAll(printStructs, totalSize);

                        printOutput(mapWidth, mapHeight, printStructs, totalSize);
//                }



                        hunterMessage[i].pos.x = hunters[i].x;
                        hunterMessage[i].pos.y = hunters[i].y;

                        /*
                         * burayı değiştir.
                         */
                        int closestIndex = getIndexFromNearestPray(hunters[i], prays, sizePray);
                        if (closestIndex == -1) {
                            open[i] = 0;
                            sizeOpen--;
                            kill(pid[i], SIGTERM);
                            waitpid(pid[i], &(childStatus[i]), 0);
                            printf("Closed Hunter[%d]\n", i);
                        }
                        hunterMessage[i].adv_pos.x = prays[closestIndex].x;
                        hunterMessage[i].adv_pos.y = prays[closestIndex].y;

                        hunterMessage[i].object_count = 0;

                        arrangeObjectPositionFindHunterForHunter(sizeHunter, hunters, hunterMessage, i, mapWidth,
                                                                 mapHeight);
                        arrangeObjectPositionFindObstacleForHunter(sizeObstacle, hunters, obstacles, hunterMessage, i,
                                                                   mapWidth,
                                                                   mapHeight);
//
//                        printServerMessage(hunterMessage[i]);
//                        printf("--------END [%d]-------\n", i);


                        fflush(stdout);
                        write(fd[i][0], &hunterMessage[i], sizeof(server_message));

                    }
                }


            }
        }

        /*
         * PRAYYYYYYYY
         */

        for (i = 0; i < sizePray; i++) {
            if (FD_ISSET(fdPray[i][0], &readset)) {

//                printf("--------BEGIN PRAY [%d]-------\n", i);


                read(fdPray[i][0], &receivingMessageFromPray[i], sizeof(ph_message));
//                printf("Request (%d,%d)\n", receivingMessageFromPray[i].move_request.x,
//                       receivingMessageFromPray[i].move_request.y);
                if (receivingMessageFromPray[i].move_request.y == -1 &&
                    receivingMessageFromPray[i].move_request.x == -1) {
                    printStructs = (PrintStruct *) realloc(printStructs, (totalSize * sizeof(PrintStruct)));

                    totalSize = getSizeAndArrangePrintStruct(printStructs, hunters, obstacles, prays, sizeHunter,
                                                             sizePray,
                                                             sizeObstacle);


                    sortPrintStructs(printStructs, totalSize);
                    printAll(printStructs, totalSize);

                    printOutput(mapWidth, mapHeight, printStructs, totalSize);


                    prayMessage[i].pos.x = prays[i].x;
                    prayMessage[i].pos.y = prays[i].y;

                    int closestIndex = getIndexFromNearestHunter(prays[i], hunters, sizeHunter);
                    if (closestIndex == -1) {
                        openPray[i] = 0;
                        sizeOpenPray--;
                        kill(pidPray[i], SIGTERM);
                        waitpid(pidPray[i], &(childStatusForPray[i]), 0);
                        printf("Closed Pray[%d]\n", i);
                    }
                    prayMessage[i].adv_pos.x = hunters[closestIndex].x;
                    prayMessage[i].adv_pos.y = hunters[closestIndex].y;

                    prayMessage[i].object_count = 0;

                    arrangeObjectPositionFindPrayForPray(sizePray, prays, prayMessage, i, mapWidth, mapHeight);
                    arrangeObjectPositionFindObstacleForPray(sizeObstacle, prays, obstacles, prayMessage, i, mapWidth,
                                                             mapHeight);

//                    printServerMessage(prayMessage[i]);
//                    printf("--------END [%d]-------\n", i);

                    fflush(stdout);
                    write(fdPray[i][0], &prayMessage[i], sizeof(server_message));
                    continue;
                }
                int canMoveAnswer = CanMoveNextForPray(i, receivingMessageFromPray[i], prays, printStructs,
                                                       totalSize);
                if (canMoveAnswer == 0 || canMoveAnswer == 1) {


                    printStructs = (PrintStruct *) realloc(printStructs, (totalSize * sizeof(PrintStruct)));

                    totalSize = getSizeAndArrangePrintStruct(printStructs, hunters, obstacles, prays, sizeHunter,
                                                             sizePray,
                                                             sizeObstacle);


                    sortPrintStructs(printStructs, totalSize);
                    printAll(printStructs, totalSize);

                    printOutput(mapWidth, mapHeight, printStructs, totalSize);


                    prayMessage[i].pos.x = prays[i].x;
                    prayMessage[i].pos.y = prays[i].y;

                    int closestIndex = getIndexFromNearestHunter(prays[i], hunters, sizeHunter);
                    if (closestIndex == -1) {
                        openPray[i] = 0;
                        sizeOpenPray--;
                        kill(pidPray[i], SIGTERM);
                        waitpid(pidPray[i], &(childStatusForPray[i]), 0);
                        printf("Closed Pray[%d]\n", i);
                    }
                    prayMessage[i].adv_pos.x = hunters[closestIndex].x;
                    prayMessage[i].adv_pos.y = hunters[closestIndex].y;

                    prayMessage[i].object_count = 0;

                    arrangeObjectPositionFindPrayForPray(sizePray, prays, prayMessage, i, mapWidth, mapHeight);
                    arrangeObjectPositionFindObstacleForPray(sizeObstacle, prays, obstacles, prayMessage, i, mapWidth,
                                                             mapHeight);

//                    printServerMessage(prayMessage[i]);
//                    printf("--------END [%d]-------\n", i);

                    fflush(stdout);
                    write(fdPray[i][0], &prayMessage[i], sizeof(server_message));


                }
            }
        }


//        printf("sizeOpen=%d, sizeOpenPray=%d, hSize:%d, pSize=%d",sizeOpen,sizeOpenPray,counterHunterSize,counterPraySize);
    }

    for (int j = 0; j < sizeHunter; ++j) {
        if (open[j]) {
            open[j] = 0;
            kill(pid[j], SIGTERM);
            waitpid(pid[j], &(childStatus[j]), 0);
            printf("All Closed Hunters Check\n --------\nClosed [%d]\n", j);
        }
    }

    for (int j = 0; j < sizePray; ++j) {
        if (openPray[j]) {
            openPray[j] = 0;
            kill(pidPray[j], SIGTERM);
            waitpid(pidPray[j], &(childStatusForPray[j]), 0);
            printf("All Closed Check\n --------\nClosed [%d]\n", j);
        }
    }
//    char buff2[255];
//    server_message server_message1;
//    printf("it's parent\n");
//    ssize_t n = read(fd[0][0], buff, 255);
//    ssize_t n1 = read(fd[1][0], buff2, 255);
////      write(1, buff, n);
//    printf("%s", buff);
//    printf("%s", buff2);
//    write(fd[0][0], "Server1\n", 8);
//    write(fd[1][0], "Server2\n", 8);


    return 0;
}

void printAll(PrintStruct *printStructs, int structSize) {
    int i;
    for (i = 0; i < structSize; ++i) {
        printf("[%d,%d,%c]", printStructs[i].x, printStructs[i].y, printStructs[i].mode);
    }
    printf("\n");
}

void printTopAndBottom(int mapWidth) {
    printf("+");
    for (size_t i = 0; i < mapWidth; i++) {
        printf("-");
    }
    printf("+\n");
}


void sortPrintStructs(PrintStruct *printStructs, int structSize) {
    int i, j;
    for (i = 0; i < structSize; i++) {
        for (j = 0; j < structSize; j++) {
            if (printStructs[j].x > printStructs[i].x) {
                PrintStruct tmp = printStructs[i];
                printStructs[i] = printStructs[j];
                printStructs[j] = tmp;
            }
        }
    }

    for (i = 0; i < structSize; i++) {
        for (j = i + 1; j < structSize; j++) {
            if (printStructs[j].x == printStructs[i].x) {
                if (printStructs[j].y < printStructs[i].y) {
                    PrintStruct tmp = printStructs[i];
                    printStructs[i] = printStructs[j];
                    printStructs[j] = tmp;
                }
            }
            break;

        }
    }
}


void printOutput(int width, int height, PrintStruct *printStructs, int structSize) {
    int i, *beforeIndex;
    beforeIndex = (int *) malloc(sizeof(int));
    *beforeIndex = 0;
    printTopAndBottom(width);
    for (i = 0; i < height; i++) {
        printRow(i, printStructs, structSize, width, beforeIndex);
    }
    printTopAndBottom(width);

}

void printRow(int row, PrintStruct *printStructs, int structSize, int width, int *beforeIndex) {
    int cursor = 0, i;
    printf("|");
    for (cursor = 0, i = *beforeIndex; cursor < width; cursor++) {
        if (printStructs[i].x == row && cursor == printStructs[i].y && structSize > i) {
            //ilk ise öncesine bakmaya gerek yok. direk yazalım. İlk değilse öncesi ile devam eden bir şey mi
            printf("%c", printStructs[i].mode);
            i++;

        } else {
            printf(" ");
        }
    }
    printf("|\n");
    *beforeIndex = i;
}

int getMaximumFileDescriptor(int fd[][2], int size) {
    int maximum = 0;
    for (int i = 0; i < size; ++i) {
        if (fd[i][0] > maximum)
            maximum = fd[i][0];

    }
    return maximum + 1;
}


