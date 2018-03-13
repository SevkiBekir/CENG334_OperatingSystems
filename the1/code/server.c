#include <stdio.h>
//#include <malloc.h>
#include <stdlib.h>
#include<sys/socket.h>
#include <unistd.h>
#include <sys/wait.h>
#include<sys/select.h>


#define PIPE(fd) socketpair(AF_UNIX, SOCK_STREAM, PF_UNIX, fd)

// STRUCTS
typedef struct Obstacle {
    int x, y;
} Obstacle;

typedef struct Hunter {
    int x, y, energy;
} Hunter;

typedef struct Pray {
    int x, y, energy;
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

//FUNCTIONS
void printTopAndBottom(int mapWidth);

void sortPrintStructs(PrintStruct *, int);

void printAll(PrintStruct *printStructs, int structSize);

void printOutput(int width, int height, PrintStruct *printStructs, int structSize);

void printRow(int row, PrintStruct *printStructs, int structSize, int width, int *beforeIndex);

//MAIN
int main(int argc, char const *argv[]) {
    int mapWidth, mapHeight;
    int sizeObstacle = 0, sizeHunter = 0, sizePray = 0, i, x, y, e, totalSize = 0;
    PrintStruct *printStructs;
    FILE *fp;
    char buff[255];
    int tempSize;

    fp = fopen("input.txt", "r");


    fscanf(fp, "%d %d", &mapWidth, &mapHeight);


    fscanf(fp, "%d", &sizeObstacle);
    Obstacle obstacles[sizeObstacle];
    totalSize = sizeObstacle;
    printStructs = (PrintStruct *) malloc(totalSize * sizeof(PrintStruct));
    for (i = 0; i < sizeObstacle; i++) {
        fscanf(fp, "%d %d", &x, &y);
        obstacles[i].x = x;
        obstacles[i].y = y;
        printStructs[i].x = x;
        printStructs[i].y = y;
        printStructs[i].mode = 'X';
    }

    fscanf(fp, "%d", &sizeHunter);
    Hunter hunters[sizeHunter];
    tempSize = sizeHunter + totalSize;
    printStructs = (PrintStruct *) realloc(printStructs, (tempSize * sizeof(PrintStruct)));
    for (i = 0; i < sizeHunter; i++) {
        fscanf(fp, "%d %d %d", &x, &y, &e);
        hunters[i].x = x;
        hunters[i].y = y;
        hunters[i].energy = e;
        printStructs[i + totalSize].x = x;
        printStructs[i + totalSize].y = y;
        printStructs[i + totalSize].mode = 'H';
    }
    totalSize += sizeHunter;


    fscanf(fp, "%d", &sizePray);
    Pray prays[sizePray];
    tempSize = sizePray + totalSize;
    printStructs = (PrintStruct *) realloc(printStructs, (tempSize * sizeof(PrintStruct)));
    for (i = 0; i < sizePray; i++) {
        fscanf(fp, "%d %d %d", &x, &y, &e);
        prays[i].x = x;
        prays[i].y = y;
        prays[i].energy = e;
        printStructs[i + totalSize].x = x;
        printStructs[i + totalSize].y = y;
        printStructs[i + totalSize].mode = 'P';
    }

    totalSize += sizePray;


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

    pid_t pid[sizeHunter];
    int fd[sizeHunter][2];
    for (i = 0; i < sizeHunter; i++) {
        PIPE(fd[i]);
        pid[i] = fork();
        if (pid[i] == 0) {
            //it is child;
            fflush(stdout);
            dup2(fd[i][1], 1);
            dup2(fd[i][1], 0);
            execl("hunter", "hunter", NULL, NULL);

        }


    }

//    fd_set readset;


    while (1) {

        char buff2[255];
        server_message server_message1;
        printf("it's parent\n");
        ssize_t n = read(fd[0][0], buff, 255);
        ssize_t n1 = read(fd[1][0], buff2, 255);
//        write(1, buff, n);
        printf("%s", buff);
        printf("%s", buff2);
        write(fd[0][0], "Server1\n", 8);
        write(fd[1][0], "Server2\n", 8);
    }


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

