#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

int grid_width;
int grid_height;

void printUsage(const char *programName) {
    printf("usage: %s <width> <height> <density> <steps>\n", programName);
}

void initField(bool **src_grid, bool **dst_grid, double probability) {
    //initialize all columns
    for (int i = 0; i < grid_width; ++i) {
        src_grid[i] = (bool *) malloc(grid_width * sizeof(int));
        dst_grid[i] = (bool *) malloc(grid_width * sizeof(int));
    }

    bool random;
    for (int i = 0; i < grid_height; ++i) {
        for (int j = 0; j < grid_width; ++j) {
            random = rand() < probability * ((double) RAND_MAX + 1.0);
            src_grid[i][j] = random;
            /*printf("%d ", src_grid[i][j]);*/
        }
        /*printf("\n");*/
    }
}

void copy_array(bool **src_grid, bool **dst_grid) {
    for (int i = 0; i < grid_height; i++) {
        for (int j = 0; j < grid_width; j++) {
            memcpy(src_grid[i], dst_grid[i], grid_width);
        }
    }
}

void freeMemory(bool **src_grid, bool **dst_grid) {
    for (int i = 0; i < grid_width; ++i) {
        free(src_grid[i]);
        free(dst_grid[i]);
    }
    free(src_grid);
    free(dst_grid);
}

int nextGen(int height, int width, bool **src_grid) {
    int counterCell = 0;
    for (int i = height - 1; i <= height + 1; i++) {
        for (int j = width - 1; j <= width + 1; j++) {
            if ((i == height && j == width) || (i < 0 || j < 0) ||
                (i >= grid_height || j >= grid_width))
                continue;
            if (src_grid[i][j] == true)
                ++counterCell;
        }
    }
    /*    printf("[%d,%d]Counter: %d\n", height, width, counterCell);*/
    return counterCell;
}

/*commands to execute
    valgrind --tool=memcheck --leak-check=yes ./game
    gcc -std=c11 -Wall -Wextra -o game main.c
    ./game 200 200 0.1 100
    convert -filter point -resize 300%x300% -delay 20 gol_*.pbm gol.gif

*/

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }

    grid_width = atoi(argv[1]);
    grid_height = atoi(argv[2]);
    const double density = atof(argv[3]);
    const int steps = atoi(argv[4]);

    printf("width:   %4d\n", grid_width);
    printf("height:  %4d\n", grid_height);
    printf("density: %4.0f%%\n", density * 100);
    printf("steps:   %4d\n", steps);

    // Seeding the random number generator so we get a different starting field
    // every time.
    srand(time(NULL));

    //allocate memory --> initialize all rows on grid
    bool **src_grid = (bool **) malloc(grid_height * sizeof(bool *));
    bool **dst_grid = (bool **) malloc(grid_height * sizeof(bool *));

    //init grid
    initField(src_grid, dst_grid, density);

    int neighbours = 0;
    FILE *fPbm;
    char buf[14];

    // write into .pbm files
    for (int i = 0; i < steps; i++) {
        snprintf(buf, 12, "gol_%05d", i); // puts string into buffer
        fPbm = fopen(strcat(buf, ".pbm"), "wb"); //write the file in binary mode
        fprintf(fPbm, "P1\n"); // Writing Magic Number to the File
        fprintf(fPbm, "%d %d\n", grid_width, grid_height);
        for (int j = 0; j < grid_height; j++) {
            for (int k = 0; k < grid_width; k++) {
                /* printf("%d ", src_grid[j][k]);*/
                fprintf(fPbm, "%d ", src_grid[j][k]);
                neighbours = nextGen(j, k, src_grid);
                //rules based on: https://conwaylife.com/wiki/Conway%27s_Game_of_Life
                //Any dead cell with exactly three live neighbours will come to life
                if (neighbours == 3 && src_grid[j][k] == false) {
                    dst_grid[j][k] = true;
                } else if (neighbours == 2 || neighbours == 3) {
                    //Any live cell with two or three live neighbours lives, unchanged, to the next generation.
                    dst_grid[j][k] = src_grid[j][k];
                } else if (neighbours < 2 || neighbours > 3) {
                    dst_grid[j][k] = false;
                }

            }
            fprintf(fPbm, "\n");
            /*printf("\n");*/
        }
        copy_array(src_grid, dst_grid);
        /* printf("\n");*/
        fclose(fPbm);
    }
    freeMemory(src_grid, dst_grid);

    return EXIT_SUCCESS;
}

