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


int main(int argc, char const *argv[]) {
//    printf("hello boy!\n");
        server_message server_message1;
    while (1){
        char buff[255];
        write(1, "Hunter\n", 7);
        ssize_t n = read(0, buff, 255);
        write(STDERR_FILENO, buff, n);
    }
}