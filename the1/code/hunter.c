//
// Created by sbk on 12.03.2018.
//

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

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


int calculateManhattenDisctance(int hx, int hy, int px, int py) {
    int result = 0;
    result = abs(px - hx) + abs(py - hy);
    return result;
}

void printCoordinate(coordinate possibleCoordinates[], int index) {
    printf("[%d Coordinate] = (%d,%d)\n", index, possibleCoordinates[index].x, possibleCoordinates[index].y);
}

void printServerMessage(server_message serverMessage) {
    printf("Pos = (%d,%d)\n", serverMessage.pos.x, serverMessage.pos.y);
    printf("Adv Pos = (%d,%d)\n", serverMessage.adv_pos.x, serverMessage.adv_pos.y);
    printf("object Count = %d\n", serverMessage.object_count);
    for (int i = 0; i < serverMessage.object_count; ++i) {
        printf("[%d] object Pos = (%d,%d)\n ", i, serverMessage.object_pos[i].x, serverMessage.object_pos[i].y);
    }
}

void printPossibleMovements(coordinate possibleCoordinate[]) {
    for (int j = 0; j < 4; ++j) {
        printf("[Hunter] [%d. Coordinate] = (%d,%d)\n", j, possibleCoordinate[j].x, possibleCoordinate[j].y);
    }
}

void controlPossibleMovements(server_message serverMessage, coordinate possibleCoordinate[]) {
    for (int i = 0; i < serverMessage.object_count; ++i) {
        for (int j = 0; j < 4; ++j) {
            /*
             * engel var mı yok mu
             * varsa -1 yap.
             * yoksaa devam
            */
            if (serverMessage.object_pos[i].x == possibleCoordinate[j].x &&
                serverMessage.object_pos[i].y == possibleCoordinate[j].y) {
                possibleCoordinate[j].x = -1;
                possibleCoordinate[j].y = -1;
                break;
            }
        }
    }
}

void getPossibleMovementsWithAll(server_message serverMessage, int width, int height, coordinate possibleCoordinate[]) {
    /*
     *  sol üst köşe
     *  +--
     *  |HX
     *  |X
     */
    int i = 0;
    if (serverMessage.pos.x - 1 < 0 && serverMessage.pos.y - 1 < 0) {



        //sağ
        possibleCoordinate[i].x = serverMessage.pos.x;
        possibleCoordinate[i++].y = serverMessage.pos.y + 1;

        //alt
        possibleCoordinate[i].x = serverMessage.pos.x + 1;
        possibleCoordinate[i++].y = serverMessage.pos.y;
    }

        /*
         *  sağ üst köşe
         *  --+
         *  XH|
         *   X|
         */
    else if (serverMessage.pos.y + 1 == width && serverMessage.pos.x - 1 < 0) {
        //sol
        possibleCoordinate[i].x = serverMessage.pos.x;
        possibleCoordinate[i++].y = serverMessage.pos.y - 1;

        //alt
        possibleCoordinate[i].x = serverMessage.pos.x + 1;
        possibleCoordinate[i++].y = serverMessage.pos.y;
    }

        /*
         *  sağ alt köşe
         *   X|
         *  XH|
         *  --+
         */
    else if (serverMessage.pos.y + 1 == width && serverMessage.pos.x + 1 == height) {

        //üst
        possibleCoordinate[i].x = serverMessage.pos.x - 1;
        possibleCoordinate[i++].y = serverMessage.pos.y;

        //sol
        possibleCoordinate[i].x = serverMessage.pos.x;
        possibleCoordinate[i++].y = serverMessage.pos.y - 1;

    }


        /*
         *  sol alt köşe
         *  |X
         *  |HX
         *  +--
         */
    else if (serverMessage.pos.y - 1 < 0 && serverMessage.pos.x + 1 == height) {
        //üst
        possibleCoordinate[i].x = serverMessage.pos.x - 1;
        possibleCoordinate[i++].y = serverMessage.pos.y;

        //sağ
        possibleCoordinate[i].x = serverMessage.pos.x;
        possibleCoordinate[i++].y = serverMessage.pos.y + 1;

    }

        /*
             *  sol kenar
             *  |X
             *  |HX
             *  |X
             */
    else if (serverMessage.pos.y - 1 < 0) {

        //üst
        possibleCoordinate[i].x = serverMessage.pos.x - 1;
        possibleCoordinate[i++].y = serverMessage.pos.y;

        //sağ
        possibleCoordinate[i].x = serverMessage.pos.x;
        possibleCoordinate[i++].y = serverMessage.pos.y + 1;


        //alt
        possibleCoordinate[i].x = serverMessage.pos.x + 1;
        possibleCoordinate[i++].y = serverMessage.pos.y;

    }

        /*
                *  üst kenar
                *  ---
                *  XHX
                *   X
                */
    else if (serverMessage.pos.x - 1 < 0) {
        //sağ
        possibleCoordinate[i].x = serverMessage.pos.x;
        possibleCoordinate[i++].y = serverMessage.pos.y + 1;


        //alt
        possibleCoordinate[i].x = serverMessage.pos.x + 1;
        possibleCoordinate[i++].y = serverMessage.pos.y;

        //sol
        possibleCoordinate[i].x = serverMessage.pos.x;
        possibleCoordinate[i++].y = serverMessage.pos.y - 1;
    }

        /*
              *  sağ kenar
              *   X|
              *  XH|
              *   X|
              */
    else if (serverMessage.pos.y + 1 == width) {

        //alt
        possibleCoordinate[i].x = serverMessage.pos.x + 1;
        possibleCoordinate[i++].y = serverMessage.pos.y;

        //sol
        possibleCoordinate[i].x = serverMessage.pos.x;
        possibleCoordinate[i++].y = serverMessage.pos.y - 1;


        //üst
        possibleCoordinate[i].x = serverMessage.pos.x - 1;
        possibleCoordinate[i++].y = serverMessage.pos.y;

    }

        /*
                *  alt kenar
                *   X
                *  XHX
                *  ---
                */
    else if (serverMessage.pos.x + 1 == height) {

        //sol
        possibleCoordinate[i].x = serverMessage.pos.x;
        possibleCoordinate[i++].y = serverMessage.pos.y - 1;


        //üst
        possibleCoordinate[i].x = serverMessage.pos.x - 1;
        possibleCoordinate[i++].y = serverMessage.pos.y;


        //sağ
        possibleCoordinate[i].x = serverMessage.pos.x;
        possibleCoordinate[i++].y = serverMessage.pos.y + 1;

    } else {

        //sol
        possibleCoordinate[i].x = serverMessage.pos.x;
        possibleCoordinate[i++].y = serverMessage.pos.y - 1;


        //üst
        possibleCoordinate[i].x = serverMessage.pos.x - 1;
        possibleCoordinate[i++].y = serverMessage.pos.y;


        //sağ
        possibleCoordinate[i].x = serverMessage.pos.x;
        possibleCoordinate[i++].y = serverMessage.pos.y + 1;

        //alt
        possibleCoordinate[i].x = serverMessage.pos.x + 1;
        possibleCoordinate[i++].y = serverMessage.pos.y;
    }

}

int getShortestPathIndexInPossibleCoordinate(server_message serverMessage, int oldMD, coordinate possibleCoordinate[]) {

//    printf("SHORTEST PATH CALC\n");
    int newMD = oldMD;
//    printf("oldMD = %d\n", oldMD);
    int index = 0;
    for (int i = 0; i < 4; ++i) {
//        printCoordinate(possibleCoordinate, i);
        if (possibleCoordinate[i].x == -1 && possibleCoordinate[i].y == -1) {
//            printf("CONTINUE\n");
            continue;
        }
//        printf("CALC TEMPMD\n");

        int tempMD = calculateManhattenDisctance(possibleCoordinate[i].x, possibleCoordinate[i].y,
                                                 serverMessage.adv_pos.x, serverMessage.adv_pos.y);

//        printf("TempMD = %d, newMD = %d\n", tempMD, newMD);

        if (tempMD < newMD) {
            newMD = tempMD;
            index = i;
//            printf("updated index -> %d\n", index);

        }


    }
//    printf("returning index -> %d\n", index);


    return index;
}

int main(int argc, char const *argv[]) {
//    printf("argc:%d argv:%s %s\n",argc,argv[1],argv[2]);
    int width = atoi(argv[1]);
    int height = atoi(argv[2]);
    printf("[Hunter] w:%d, h:%d\n", width, height);

//    printf("hello boy!\n");
    server_message serverMessage;
    coordinate possibleCoordinates[4];
//    int i = -1;
    while (1) {
//        write(1, "Hunter\n", 7);
//
//        char buff[1024];
//        ssize_t n = read(0, buff, 1024);
//        write(STDERR_FILENO, buff, n);

        for (int i = 0; i < 4; ++i) {
            possibleCoordinates[i].x = -1;
            possibleCoordinates[i].y = -1;
        }


        read(0, &serverMessage, sizeof(server_message));

        int oldMD = calculateManhattenDisctance(serverMessage.pos.x, serverMessage.pos.y, serverMessage.adv_pos.x,
                                                serverMessage.adv_pos.y);


//        printServerMessage(serverMessage);

        /*
         * bakması gereken boşlukları getiriyor.
         */
        getPossibleMovementsWithAll(serverMessage, width, height, possibleCoordinates);
        controlPossibleMovements(serverMessage, possibleCoordinates);
//        printPossibleMovements(possibleCoordinates);

        int index = getShortestPathIndexInPossibleCoordinate(serverMessage, oldMD, possibleCoordinates);
//        printf("Selected\n");
//        printCoordinate(possibleCoordinates, index);
//
//        printf("-------------------------------------\n");

//        if(server_message1.pos.x<=10){
//            printf("[first] Hunter, x->%d, y->%d\n", server_message1.pos.x, server_message1.pos.y);
//            server_message1.pos.x++;
//            printf("[inc] Hunter, x->%d, y->%d\n", server_message1.pos.x, server_message1.pos.y);
//        }

        ph_message sendingMessage;
        sendingMessage.move_request.x = possibleCoordinates[index].x;
        sendingMessage.move_request.y = possibleCoordinates[index].y;
        fflush(stdout);
        write(0, &sendingMessage, sizeof(ph_message));


//        printf("%d %d\n",server_message1.pos.x, server_message1.pos.y);



//        write(STDERR_FILENO, msj, sizeof(msj));
        usleep(10000 * (1 + rand()%9));

    }
}