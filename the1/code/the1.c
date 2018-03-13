#include <stdio.h>
//#include <malloc.h>
#include <stdlib.h>

// STRUCTS
typedef struct Obstacles {
    int x, y;
} Obstacle;

typedef struct Hunters {
    int x, y, energy;
} Hunter;

typedef struct Prays {
    int x, y, energy;
} Pray;

typedef struct PrintStructs {
    int x, y;
    char mode;
} PrintStruct;

//FUNCTIONS
void printTopAndBottom(int mapWidth);

void sortPrintStructs(PrintStruct *, int);

void printAll(PrintStruct *printStructs, int structSize);

void printOutput(int width, int height, PrintStruct *printStructs, int structSize);
void printRow(int row, PrintStruct *printStructs, int structSize, int width,int *beforeIndex);

//MAIN
int main(int argc, char const *argv[]) {
    int mapWidth, mapHeight;
    int size = 0, i, x, y, e, totalSize = 0;
    PrintStruct *printStructs;
    FILE *fp;
    char buff[255];
    int tempSize;

    fp = fopen("input.txt", "r");


    while (!feof(fp)) {
        fscanf(fp, "%d %d", &mapWidth, &mapHeight);


        fscanf(fp, "%d", &size);
        Obstacle obstacles[size];
        totalSize = size;
        printStructs = (PrintStruct *) malloc(size * sizeof(PrintStruct));
        for (i = 0; i < size; i++) {
            fscanf(fp, "%d %d", &x, &y);
            obstacles[i].x = x;
            obstacles[i].y = y;
            printStructs[i].x = x;
            printStructs[i].y = y;
            printStructs[i].mode = 'X';
        }

        fscanf(fp, "%d", &size);
        Hunter hunters[size];
        tempSize = size + totalSize;
        printStructs = (PrintStruct *) realloc(printStructs, (tempSize * sizeof(PrintStruct)));
        for (i = 0; i < size; i++) {
            fscanf(fp, "%d %d %d", &x, &y, &e);
            hunters[i].x = x;
            hunters[i].y = y;
            hunters[i].energy = e;
            printStructs[i + totalSize].x = x;
            printStructs[i + totalSize].y = y;
            printStructs[i + totalSize].mode = 'H';
        }
        totalSize += size;


        fscanf(fp, "%d", &size);
        Pray prays[size];
        tempSize = size + totalSize;
        printStructs = (PrintStruct *) realloc(printStructs, (tempSize * sizeof(PrintStruct)));
        for (i = 0; i < size; i++) {
            fscanf(fp, "%d %d %d", &x, &y, &e);
            prays[i].x = x;
            prays[i].y = y;
            prays[i].energy = e;
            printStructs[i + totalSize].x = x;
            printStructs[i + totalSize].y = y;
            printStructs[i + totalSize].mode = 'P';
        }

        totalSize += size;
        sortPrintStructs(printStructs, totalSize);
        printAll(printStructs, totalSize);

        printOutput(mapWidth, mapHeight, printStructs, totalSize);

    }
    fclose(fp);


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


void sort(int *values) {
    int size = sizeof(values) / sizeof(int);
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (values[j] > values[i]) {
                int tmp = values[i];
                values[i] = values[j];
                values[j] = tmp;
            }
        }
    }
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
    int i,*beforeIndex;
    beforeIndex = (int*) malloc(sizeof(int));
    *beforeIndex = 0;
    printTopAndBottom(width);
    for (i = 0; i < height; i++) {
        printRow(i, printStructs, structSize, width,beforeIndex);
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

