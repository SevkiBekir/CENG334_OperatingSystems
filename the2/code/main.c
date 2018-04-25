#include "do_not_submit.h"
#include <pthread.h>
#include <semaphore.h>
#include <zconf.h>

sem_t mutex[GRIDSIZE][GRIDSIZE];
sem_t *sleepMutex;
bool antFlag = false;

typedef struct Ant {
    int x, y;
    char state;
    int id;
} Ant;


typedef struct Position {
    int x, y;

} Position;

bool isInArea(int x, int y) {
    bool answer = false;
    if (x >= 0 && x < GRIDSIZE && y >= 0 && y < GRIDSIZE)
        answer = true;
    return answer;
}

bool isThereEmptyCell(int x, int y) {
    return lookCharAt(x, y) == '-';
}

int getRandomNo(int max) {
    return rand() % max;
}

bool isAntNewPositionTheSameOldPosition(int x, int y) {
    return x == 1 && y == 1;
}

Position getPosition(int x, int y, Ant ant) {
    Position pos;
    pos.x = ant.x;
    pos.y = ant.y;
    if (x == 0) {
        pos.x -= 1;
    } else if (x == 2) {
        pos.x += 1;
    }

    if (y == 0) {
        pos.y -= 1;
    } else if (y == 2) {
//        printf("girdi\n");
//        printPosition(pos);
        pos.y += 1;
//        printPosition(pos);
    }
    return pos;
}

int getPositionX(int x, int antPositionX) {
    if (x == 0) {
        antPositionX -= 1;
    } else if (x == 2) {
        antPositionX = +1;
    }

    return antPositionX;
}

int getPositionY(int y, int antPositionY) {
    if (y == 0) {
        antPositionY -= 1;
    } else if (y == 2) {
        antPositionY = +1;
    }

    return antPositionY;
}

bool isThereAnyFoodAsNeighborhood(Position position) {
    if (isInArea(position.x, position.y))
        return (lookCharAt(position.x, position.y) == 'o') && !(position.x == -1 && position.y == -1);
    else
        return false;
}

Position getPositionOfNeighborhoodFood(Ant ant) {
//    printf("***********\n");
    Position foodPosition;

    foodPosition.x = -1;
    foodPosition.y = -1;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (j == 1 && i == 1)
                continue;
            foodPosition = getPosition(i, j, ant);
//            printf("food position i->%d, j->%d\n", i, j);
//            printPosition(foodPosition);
            if (isInArea(foodPosition.x, foodPosition.y)) {
                sem_wait(&mutex[foodPosition.x][foodPosition.y]);
                if (lookCharAt(foodPosition.x, foodPosition.y) == 'o') {
//                    printf("\ngirdi\n");
//                printf("found food position\n");
//                printPosition(foodPosition);
                    return foodPosition;
                }
                sem_post(&mutex[foodPosition.x][foodPosition.y]);
            }


        }

    }

    return foodPosition;


}

bool isThereAnyFoodWhileCarrying(Position position, Ant ant) {
    Position foodPosition;

    foodPosition.x = -1;
    foodPosition.y = -1;

    foodPosition.y = -1;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (j == 1 && i == 1)
                continue;
            foodPosition = getPosition(i, j, ant);
            if (isInArea(foodPosition.x, foodPosition.y) && lookCharAt(foodPosition.x, foodPosition.y) == 'o') {
                return true;
            }


        }
    }

    return false;
}

void printAntInfo(Ant a) {
    printf("Ant\n");
    printf("(x,y)=(%d,%d) \n", a.x, a.y);
    printf("state->%c id->%d\n", a.state, a.id);
}


void printPosition(Position p) {
    printf("Position\n");
    printf("(x,y)=(%d,%d) \n", p.x, p.y);
}

void *antOperation(Ant *myAnt) {
//    printf("hello");

    while (!antFlag) {
        sem_wait(&sleepMutex[myAnt->id]);
        sem_post(&sleepMutex[myAnt->id]);
        if(!antFlag){
            break;
        }

//        sem_wait(&mutex);

//        printf("*******************************START******************\n");


//        printf("\n");
//        printAntInfo(*myAnt);


        //check movement depens on ant state
        /*
         *          0th column   1st column  2nd column
         * 0th row      0,0         1,0         2,0
         * 1st row      0,1         1,1         2,1
         * 2nd row      0,2         1,2         2,2
         *
         * if ant position 1,1 then again random no
         */
        if (myAnt->state == '1') {
//        printf("state-1\n");
//        printAntInfo(*myAnt);
            //without food state
            int x, y;
            int column = 3;
            Position antNewPosition;
            antNewPosition.x = -1;
            antNewPosition.y = -1;

            Position antOldPosition, foodPosition;
            antOldPosition.x = myAnt->x;
            antOldPosition.y = myAnt->y;
//            printf("Ant old position \n ");
//            printPosition(antOldPosition);
//


            // check food
            foodPosition = getPositionOfNeighborhoodFood(*myAnt);
//            printf("Ant food position check \n ");
//            printPosition(foodPosition);
            if (isThereAnyFoodAsNeighborhood(foodPosition)) {
//                printf("there is food of this location. Good boy!\n ");

                myAnt->x = foodPosition.x;
                myAnt->y = foodPosition.y;
                myAnt->state = 'P';
                putCharTo(myAnt->x, myAnt->y, 'P');
                putCharTo(antOldPosition.x, antOldPosition.y, '-');
                sem_post(&mutex[foodPosition.x][foodPosition.y]);
            } else {
//                sem_post(&mutex[foodPosition.x][foodPosition.y]);
                // not found food just move
//                printf("there is no food of this location\n");
                do {
                    x = getRandomNo(column);
                    y = getRandomNo(column);
//                        printf("getRandom->(x,y)=(%d,%d)\n", x, y);
                    if (isAntNewPositionTheSameOldPosition(x, y))
                        continue;

//                    printf("not this same\n");
                    antNewPosition = getPosition(x, y, *myAnt);
//                    printf("newPos -> (x,y)=(%d,%d)\n", antNewPosition.x, antNewPosition.y);
                    if (!isInArea(antNewPosition.x, antNewPosition.y))
                        continue;


                    sem_wait(&mutex[antNewPosition.x][antNewPosition.y]);


                    if (!isThereEmptyCell(antNewPosition.x, antNewPosition.y)) {
//                        printf("not empty or in area\n");
                        sem_post(&mutex[antNewPosition.x][antNewPosition.y]);

                        continue;
                    }
//                    printf("empty and in area\n");

                    myAnt->x = antNewPosition.x;
                    myAnt->y = antNewPosition.y;
                    myAnt->state = '1';
                    putCharTo(myAnt->x, myAnt->y, '1');
                    putCharTo(antOldPosition.x, antOldPosition.y, '-');
                    sem_post(&mutex[antNewPosition.x][antNewPosition.y]);

                    break;
                } while (1);


            }





            /*



            do {
                x = getRandomNo(column);
                y = getRandomNo(column);
                printf("getRandom->(x,y)=(%d,%d)\n",x,y);
                if(isAntNewPositionTheSameOldPosition(x,y))
                    continue;
                printf("not this same\n");
                antNewPosition = getPosition(x,y,*myAnt);
                x = getPositionX(x,myAnt->x);
                y = getPositionX(y,myAnt->y);
                printf("newPos\n");
                printf("(x,y)=(%d,%d)",x,y);

                if(!isInArea(x,y) || !isThereEmptyCell(x,y))
                    continue;
                printf("empty and in area\n");

                antNewPosition.x = x;
                antNewPosition.y = y;
                if(lookCharAt(x,y) != 'o'){
                    printf("there is no food of this location\n");
                    continue;
                }
                printf("there is food of this location. Good boy!\n ");

                myAnt->x = x;
                myAnt->y = y;
                myAnt->state = 'P';
                putCharTo(myAnt->x,myAnt->y,'P');
                putCharTo(antOldPosition.x,antOldPosition.y,'-');

                break;
            } while (1);

            if(myAnt->state == '1'){
                myAnt->x = antNewPosition.x;
                myAnt->y = antNewPosition.y;
                putCharTo(myAnt->x,myAnt->y,'1');
                putCharTo(antOldPosition.x,antOldPosition.y,'-');


            }*/


        } else if (myAnt->state == 'P') {
            int x, y;
            int column = 3;
            Position antNewPosition;
            antNewPosition.x = -1;
            antNewPosition.y = -1;

            Position antOldPosition, foodPosition;
            antOldPosition.x = myAnt->x;
            antOldPosition.y = myAnt->y;

            if (isThereAnyFoodWhileCarrying(antOldPosition, *myAnt)) {
                //çevrede food var. bırak o zaman
                if (isInArea(myAnt->x, myAnt->y)) {

                    do {
                        x = getRandomNo(column);
                        y = getRandomNo(column);
//                        printf("getRandom->(x,y)=(%d,%d)\n", x, y);
                        if (isAntNewPositionTheSameOldPosition(x, y))
                            continue;

//                    printf("not this same\n");
                        antNewPosition = getPosition(x, y, *myAnt);
//                    printf("newPos -> (x,y)=(%d,%d)\n", antNewPosition.x, antNewPosition.y);

                        if (!isInArea(antNewPosition.x, antNewPosition.y))
                            continue;


                        sem_wait(&mutex[antNewPosition.x][antNewPosition.y]);


                        if (!isThereEmptyCell(antNewPosition.x, antNewPosition.y)) {
//                        printf("not empty or in area\n");
                            sem_post(&mutex[antNewPosition.x][antNewPosition.y]);

                            continue;
                        }
//                    printf("empty and in area\n");

                        myAnt->x = antNewPosition.x;
                        myAnt->y = antNewPosition.y;
                        myAnt->state = 't';
                        putCharTo(myAnt->x, myAnt->y, '1');
                        putCharTo(antOldPosition.x, antOldPosition.y, 'o');
                        sem_post(&mutex[antNewPosition.x][antNewPosition.y]);


                        break;
                    } while (1);

                }
            } else {
                //çevrede food yok haraket et bro


                do {
                    x = getRandomNo(column);
                    y = getRandomNo(column);
//                        printf("getRandom->(x,y)=(%d,%d)\n", x, y);
                    if (isAntNewPositionTheSameOldPosition(x, y))
                        continue;

//                    printf("not this same\n");
                    antNewPosition = getPosition(x, y, *myAnt);
//                    printf("newPos -> (x,y)=(%d,%d)\n", antNewPosition.x, antNewPosition.y);
                    if (!isInArea(antNewPosition.x, antNewPosition.y))
                        continue;


                    sem_wait(&mutex[antNewPosition.x][antNewPosition.y]);


                    if (!isThereEmptyCell(antNewPosition.x, antNewPosition.y)) {
//                        printf("not empty or in area\n");
                        sem_post(&mutex[antNewPosition.x][antNewPosition.y]);

                        continue;
                    }
//                    printf("empty and in area\n");

                    myAnt->x = antNewPosition.x;
                    myAnt->y = antNewPosition.y;
                    myAnt->state = 'P';
                    putCharTo(myAnt->x, myAnt->y, 'P');
                    putCharTo(antOldPosition.x, antOldPosition.y, '-');
                    sem_post(&mutex[antNewPosition.x][antNewPosition.y]);

                    break;
                } while (1);

            }
        } else if (myAnt->state == 't') {
            int x, y;
            int column = 3;
            Position antNewPosition;
            antNewPosition.x = -1;
            antNewPosition.y = -1;

            Position antOldPosition, foodPosition;
            antOldPosition.x = myAnt->x;
            antOldPosition.y = myAnt->y;
            do {
                x = getRandomNo(column);
                y = getRandomNo(column);
//                        printf("getRandom->(x,y)=(%d,%d)\n", x, y);
                if (isAntNewPositionTheSameOldPosition(x, y))
                    continue;

//                    printf("not this same\n");
                antNewPosition = getPosition(x, y, *myAnt);
//                    printf("newPos -> (x,y)=(%d,%d)\n", antNewPosition.x, antNewPosition.y);
                if (!isInArea(antNewPosition.x, antNewPosition.y))
                    continue;


                sem_wait(&mutex[antNewPosition.x][antNewPosition.y]);


                if (!isThereEmptyCell(antNewPosition.x, antNewPosition.y)) {
//                        printf("not empty or in area\n");
                    sem_post(&mutex[antNewPosition.x][antNewPosition.y]);

                    continue;
                }
//                    printf("empty and in area\n");

                myAnt->x = antNewPosition.x;
                myAnt->y = antNewPosition.y;
                myAnt->state = '1';
                putCharTo(myAnt->x, myAnt->y, '1');
                putCharTo(antOldPosition.x, antOldPosition.y, '-');
                sem_post(&mutex[antNewPosition.x][antNewPosition.y]);

                break;
            } while (1);
        }
//        sem_post(&mutex);

        usleep(getDelay() * 1000 + (rand() % 5000));


//        printf("******************************************************\n");

    }

    return 0;

}

int main(int argc, char *argv[]) {
    char *antNumberS = argv[1];
    char *foodNumberS = argv[2];
    char *endTimeS = argv[3];
    int antNumber = atoi(antNumberS);
    int foodNumber = atoi(foodNumberS);
    int endTime = atoi(endTimeS);



    sleepMutex = malloc(sizeof(sem_t) * antNumber);

    srand(time(NULL));
    for (int l = 0; l < GRIDSIZE; ++l) {
        for (int i = 0; i < GRIDSIZE; ++i) {
            sem_init(&mutex[l][i], 0, 1);

        }
    }

    for (int l = 0; l < antNumber; ++l) {
        sem_init(&sleepMutex[l], 0, 1);

    }


    Ant *ants = malloc(antNumber * sizeof(Ant));


    //////////////////////////////
    // Fills the grid randomly to have somthing to draw on screen.
    // Empty spaces have to be -.
    // You should get the number of ants and foods from the arguments 
    // and make sure that a food and an ant does not placed at the same cell.
    // You must use putCharTo() and lookCharAt() to access/change the grid.
    // You should be delegating ants to separate threads
    int i, j;
    for (i = 0; i < GRIDSIZE; i++) {
        for (j = 0; j < GRIDSIZE; j++) {
            putCharTo(i, j, '-');
        }
    }
    int a, b;

    for (i = 0; i < antNumber; i++) {
        do {
            a = rand() % GRIDSIZE;
            b = rand() % GRIDSIZE;
            ants[i].x = a;
            ants[i].y = b;
            ants[i].id = i;
            ants[i].state = '1';
        } while (lookCharAt(a, b) != '-');
        putCharTo(a, b, '1');
//        printAntInfo(ants[i]);
    }
    for (i = 0; i < foodNumber; i++) {
        do {
            a = rand() % GRIDSIZE;
            b = rand() % GRIDSIZE;
        } while (lookCharAt(a, b) != '-');
        putCharTo(a, b, 'o');
//        printf("Food-%d -> %d,%d\n", i, a, b);
    }

//    printf("*******************END OF GRID************");
    //////////////////////////////

    // you have to have following command to initialize ncurses.



    startCurses();

    // You can use following loop in your program. But pay attention to 
    // the function calls, they do not have any access control, you 
    // have to ensure that.
    char c;
//    char *ali = "ALi";
//    printf("\n%c\n",*(ali+2));
    pthread_t ptr, ctr;
    pthread_t tids[antNumber];
    for (int k = 0; k < antNumber; ++k) {
        pthread_create(tids + k, NULL, antOperation, (void *) ants + k * sizeof(Ant));
    }

    double timeNow = time(NULL);
    double timeStart = timeNow;
    while (TRUE ) {
        timeNow = time(NULL);
        if(timeNow - timeStart > (double)endTime){
            antFlag = true;
            break;
        }

        drawWindow();

        c = 0;
        c = getch();

        if (c == 'q' || c == ESC){
            antFlag = true;
            break;
        }
        if (c == '+') {
            setDelay(getDelay() + 10);
        }
        if (c == '-') {
            setDelay(getDelay() - 10);
        }
        if (c == '*') {
            if (getSleeperN() + 1 <= antNumber) {

                sem_wait(&sleepMutex[getSleeperN()]);
                Ant ant = ants[getSleeperN()];
                if (lookCharAt(ant.x, ant.y) == '1') {
                    putCharTo(ant.x, ant.y, 's');
                } else if (lookCharAt(ant.x, ant.y) == 'P') {
                    putCharTo(ant.x, ant.y, '$');
                }
                setSleeperN(getSleeperN() + 1);
            }
        }
        if (c == '/') {
            if (getSleeperN() - 1 >= 0) {
                setSleeperN(getSleeperN() - 1);
                Ant ant = ants[getSleeperN()];
                if (lookCharAt(ant.x, ant.y) == 's') {
                    putCharTo(ant.x, ant.y, '1');
                } else if (lookCharAt(ant.x, ant.y) == '$') {
                    putCharTo(ant.x, ant.y, 'P');
                }
                sem_post(&sleepMutex[getSleeperN()]);

            }
        }
        usleep(DRAWDELAY);

        // each ant thread have to sleep with code similar to this
        //usleep(getDelay() * 1000 + (rand() % 5000));
    }


    // do not forget freeing the resources you get
    endCurses();
    for (int m = 0; m < antNumber; ++m) {
        sem_post(&sleepMutex[m]);
    }



    for (int k = 0; k < antNumber; ++k) {
        pthread_join(tids[k], NULL);

    }

    for (int l = 0; l < GRIDSIZE; ++l) {
        for (int i = 0; i < GRIDSIZE; ++i) {
            sem_destroy(&mutex[l][i]);

        }
    }

    for (int m = 0; m < antNumber; ++m) {
        sem_destroy(&sleepMutex[m]);
    }



    free(sleepMutex);
//    free(mutex);
//    free(tids);
    free(ants);



//    pthread_join(tids+0, NULL);

    return 0;
}
