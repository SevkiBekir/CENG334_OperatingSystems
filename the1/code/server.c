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
        if(!prays[i].isLive)
            continue;
        int calculate = calculateManhattenDisctance(hunter.x, hunter.y, prays[i].x, prays[i].y);
        if (minimum > calculate) {
            minimum = calculate;
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
        printf("hunter[%d] isLive:%d\n",i,hunters[i].isLive);
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
                    (printStructs[i].x == 'X' || printStructs[i].x == 'H')) {
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

void checkHunterKillPray(int hunterIndex, Hunter hunter[], Pray pray[],int *praySize, int *killedIndex, int *counterPraySize, int *counterHunterSize){
    *killedIndex = -1;
    int isPrayKilled = 0;
    printf("Hunter Info: x:%d, y:%d, e:%d, isLive:%d\n",hunter[hunterIndex].x,hunter[hunterIndex].y,hunter[hunterIndex].energy,hunter[hunterIndex].isLive);

    for (int i = 0; i < *praySize; ++i) {
        if(hunter[hunterIndex].x == pray[i].x && hunter[hunterIndex].y == pray[i].y){
            pray[i].isLive = 0;
            isPrayKilled = 1;
            (*counterPraySize)--;
            hunter[hunterIndex].energy +=pray[i].energy;
            printf("Hunter captured pray. hE: %d, pE:%d\n",hunter[hunterIndex].energy,pray[i].energy);
            break;
        }
    }
    printf("Hunter Info-BeforeEnergyControl: x:%d, y:%d, e:%d, isLive:%d\n",hunter[hunterIndex].x,hunter[hunterIndex].y,hunter[hunterIndex].energy,hunter[hunterIndex].isLive);

    // eğer geldiği yerde pray yoksa o zaman prayin enerjisi 0 ise öldür.
    if(!isPrayKilled){
        if(hunter[hunterIndex].energy == 0){
            printf("[%d]Hunter die!\n",hunterIndex);
            hunter[hunterIndex].isLive = 0;
            (*counterHunterSize)--;
            (*killedIndex) = hunterIndex;
        }
    }

}

//MAIN
int main(int argc, char const *argv[]) {
    int mapWidth, mapHeight;
    int sizeObstacle = 0, sizeHunter = 0, sizePray = 0, i, x, y, e, totalSize = 0;
    PrintStruct *printStructs;
    FILE *fp;
    char buff[255];
    int tempSize, totalLive = 0;

    fp = fopen("inputClosest.txt", "r");


    fscanf(fp, "%d %d", &mapWidth, &mapHeight);


    fscanf(fp, "%d", &sizeObstacle);
    Obstacle obstacles[sizeObstacle];
    totalSize = sizeObstacle;
//    printStructs = (PrintStruct *) malloc(99999 * sizeof(PrintStruct));
    for (i = 0; i < sizeObstacle; i++) {
        fscanf(fp, "%d %d", &x, &y);
        obstacles[i].x = x;
        obstacles[i].y = y;
//        printStructs[i].x = x;
//        printStructs[i].y = y;
//        printStructs[i].mode = 'X';
    }

    fscanf(fp, "%d", &sizeHunter);
    Hunter hunters[sizeHunter];
    tempSize = sizeHunter + totalSize;
//    printStructs = (PrintStruct *) realloc(printStructs, (tempSize * sizeof(PrintStruct)));
    for (i = 0; i < sizeHunter; i++) {
        fscanf(fp, "%d %d %d", &x, &y, &e);
        hunters[i].x = x;
        hunters[i].y = y;
        hunters[i].energy = e;
        hunters[i].isLive = 1;
//        printStructs[i + totalSize].x = x;
//        printStructs[i + totalSize].y = y;
//        printStructs[i + totalSize].mode = 'H';
        totalLive++;
    }
    totalSize += sizeHunter;


    fscanf(fp, "%d", &sizePray);
    Pray prays[sizePray];
    tempSize = sizePray + totalSize;
//    printStructs = (PrintStruct *) realloc(printStructs, (tempSize * sizeof(PrintStruct)));
    for (i = 0; i < sizePray; i++) {
        fscanf(fp, "%d %d %d", &x, &y, &e);
        prays[i].x = x;
        prays[i].y = y;
        prays[i].energy = e;
        prays[i].isLive = 1;
//        printStructs[i + totalSize].x = x;
//        printStructs[i + totalSize].y = y;
//        printStructs[i + totalSize].mode = 'P';
        totalLive++;
    }

    totalSize += sizePray;

    printStructs = (PrintStruct *) malloc(totalSize * sizeof(PrintStruct));

    totalSize = getSizeAndArrangePrintStruct(printStructs, hunters, obstacles, prays, sizeHunter, sizePray,
                                             sizeObstacle);


    sortPrintStructs(printStructs, totalSize);
    printAll(printStructs, totalSize);

    printOutput(mapWidth, mapHeight, printStructs, totalSize);

    fclose(fp);


//        pid_t pid[sizeHunter];
//        for (i = 0; i < sizeHunter; ++i) {
//            pid[i] = fork();
//            if(pid[i]){
//                //it's child
//                execl("hunter","hunter",NULL,NULL);
//
//            }
//
//        }

    printf("it's starting\n");
//
//        pid_t pid;
//        int fd[2];
//        PIPE(fd);
//        pid = fork();
//        if (pid==0) {
//            //it is child;
//            fflush(stdout);
//            dup2(fd[1], 1);
//            dup2(fd[1], 0);
//            execl("hunter", "hunter", NULL, NULL);
//
//        }
//            printf("it's parent\n");
//            ssize_t n = read(fd[0], buff, 255);
//            write(0, buff, n);

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


//    printf("w:%s h:%s\n",width,height);


    server_message hunterMessage[sizeHunter];
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


//        printServerMessage(hunterMessage[i]);
    }


    pid_t pid[sizeHunter];
    int fd[sizeHunter][2];
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


    fd_set readset;
    int open[sizeHunter];
    int sizeOpen = sizeHunter;
    for (i = 0; i < sizeHunter; i++) {
        open[i] = 1;
    }

    int childStatus[sizeHunter];
    int maximum = getMaximumFileDescriptor(fd, sizeHunter);
//    printf("max:%d",maximum);



    for (i = 0; i < sizeHunter; i++) {


//
//        char msj[50] = "Server-";
//        int length = snprintf(NULL, 0, "%d", i);
//        char *str = malloc((size_t) (length + 1));
//        snprintf(str, (size_t) (length + 1), "%d", i);
//        strcat(msj, str);

//        write(fd[i][0], msj, 9);
//        if (i == 2)
        write(fd[i][0], &hunterMessage[i], sizeof(server_message));
    }

    ph_message receivingMessageFromHunter[sizeHunter];

    int counterPraySize = sizePray;
    int counterHunterSize= sizeHunter;

    while (sizeOpen > 0 && counterHunterSize > 0 && counterPraySize > 0) {
        FD_ZERO(&readset);
        for (i = 0; i < sizeHunter; i++) {
            if (open[i])
                FD_SET(fd[i][0], &readset);

        }
        select(maximum, &readset, NULL, NULL, NULL);  /* no wset, eset, timeout */
        for (i = 0; i < sizeHunter; i++) {
            if (FD_ISSET(fd[i][0], &readset)) {

//
//                ssize_t n = read(fd[i][0], buff, 255);
//                printf("%s", buff);
//
//                char msj[50] = "Server-";
//                int length = snprintf( NULL, 0, "%d", i );
//                char* str = malloc((size_t) (length + 1));
//                snprintf(str, (size_t) (length + 1), "%d", i );
//                strcat(msj,str);
//
//                write(fd[i][0], msj, 9);

                printf("--------BEGIN [%d]-------\n",i);
                int killedIndex = -1;
                read(fd[i][0], &receivingMessageFromHunter[i], sizeof(ph_message));
                printf("Request (%d,%d)\n",receivingMessageFromHunter[i].move_request.x,receivingMessageFromHunter[i].move_request.y);
                CanMoveNextForHunter(i,receivingMessageFromHunter[i],hunters,printStructs,totalSize);
                checkHunterKillPray(i,hunters,prays,&sizePray,&killedIndex,&counterPraySize,&counterHunterSize);

                printf("Hunter Size -> %d\n",sizeHunter);
                if(killedIndex != -1){
                    open[killedIndex] = 0;
                    sizeOpen--;
                    kill(pid[killedIndex],SIGTERM);
                    waitpid(pid[killedIndex],&(childStatus[killedIndex]),0);
                    printf("Closed [%d]\n",killedIndex);


                }else{
                    printStructs = (PrintStruct *) realloc(printStructs, (totalSize * sizeof(PrintStruct)));

                    int oldTotalSize = totalSize;
                    totalSize = getSizeAndArrangePrintStruct(printStructs, hunters, obstacles, prays, sizeHunter, sizePray,
                                                             sizeObstacle);


                    sortPrintStructs(printStructs, totalSize);
//                if(oldTotalSize != totalSize){
                    printAll(printStructs, totalSize);

                    printOutput(mapWidth, mapHeight, printStructs, totalSize);
//                }



                    hunterMessage[i].pos.x = hunters[i].x;
                    hunterMessage[i].pos.y = hunters[i].y;

                    /*
                     * burayı değiştir.
                     */
                    int closestIndex = getIndexFromNearestPray(hunters[i], prays, sizePray);
                    if(closestIndex ==-1){
                        open[i] = 0;
                        sizeOpen--;
                        kill(pid[i],SIGTERM);
                        waitpid(pid[i],&(childStatus[i]),0);
                        printf("Closed [%d]\n",i);
                    }
                    hunterMessage[i].adv_pos.x = prays[closestIndex].x;
                    hunterMessage[i].adv_pos.y = prays[closestIndex].y;

                    hunterMessage[i].object_count = 0;

                    arrangeObjectPositionFindHunterForHunter(sizeHunter, hunters, hunterMessage, i, mapWidth, mapHeight);
                    arrangeObjectPositionFindObstacleForHunter(sizeObstacle, hunters, obstacles, hunterMessage, i, mapWidth,
                                                               mapHeight);

                    printServerMessage(hunterMessage[i]);
                    printf("--------END [%d]-------\n",i);


//                printf("[Server] ReceivingMessage->%d, x->%d, y->%d", i, receivingMessageFromHunter[i].move_request.x,receivingMessageFromHunter[i].move_request.y);

//                if (server_message1[i].pos.x <= 10 && (i == 0)) {
//
//                    printf("[first] Server->%d, x->%d, y->%d\n", i, server_message1[i].pos.x, server_message1[i].pos.y);
////                    server_message1.pos.y++;
//                    server_message1[i].pos.x++;
//                    printf("[inc] Server->%d, x->%d, y->%d\n", i, server_message1[i].pos.x, server_message1[i].pos.y);
//                }


                    fflush(stdout);
                    write(fd[i][0], &hunterMessage[i], sizeof(server_message));

                }



            }
        }


    }

    for (int j = 0; j < counterHunterSize; ++j) {
        if(open[j])
        {
            open[j] = 0;
            kill(pid[j],SIGTERM);
            waitpid(pid[j],&(childStatus[j]),0);
            printf("All Closed Check\n --------\nClosed [%d]\n",j);
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


