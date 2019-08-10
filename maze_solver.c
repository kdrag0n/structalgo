#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#define ARRAY_SIZE(arr) sizeof(arr) / sizeof(*arr)
typedef _Bool bool;
#define true 1
#define false 0

struct position {
    int x;
    int y;
};

// 8 lines, 10 cols  --  10x8
static char *maze[] = {
    "##########",
    "#   #S   #",
    "#   #### #",
    "###    # #",
    "#   #    #",
    "# ########",
    "#       E#",
    "##########"
};

static int maze_lines = 8;
static int maze_cols = 10;

bool is_solved(void) {
    for (int li = 0; li < maze_lines; li++) {
        char *line = maze[li];

        if (strstr(line, "SE"))
            return true;
    }

    return false;
}

void print_maze(void) {
    for (int li = 0; li < maze_lines; li++)
        printf("%s\n", maze[li]);
}

double pos_distance(struct position a, struct position b) {
    return round(sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2)));
}

int solve_maze(void) {
    int steps = 0;
    struct position cur_pos;
    struct position tgt_pos;

    while (!is_solved()) {
        // Find current mouse position
        for (cur_pos.y = 0; cur_pos.y < maze_lines; cur_pos.y++) {
            char *line = maze[cur_pos.y];

            char *mouse_addr = strchr(line, 'S');
            if (mouse_addr != NULL) {
                cur_pos.x = mouse_addr - line;
                break;
            }
        }

        // Look for an empty space that we haven't been in yet
        for (tgt_pos.y = 0; tgt_pos.y < maze_lines; tgt_pos.y++) {
            char *line = maze[tgt_pos.y];

            for (int ci = 0; ci < maze_cols; ci++) {
                if (line[ci] == ' ') {
                    tgt_pos.x = ci;

                    printf("(%d, %d) -> (%d, %d): distance = %lf\n", cur_pos.x, cur_pos.y, tgt_pos.x, tgt_pos.y, pos_distance(cur_pos, tgt_pos));
                    if (pos_distance(cur_pos, tgt_pos) <= 1.1f)
                        // Found a suitable target
                        goto search_end;
                }
            }
        }

search_end:
        printf("Current: (%d, %d)\n", cur_pos.x, cur_pos.y);
        printf("Target: (%d, %d)\n", tgt_pos.x, tgt_pos.y);

        // Move to that space
        maze[cur_pos.y][cur_pos.x] = ' ';
        maze[tgt_pos.y][tgt_pos.x] = 'S';
        cur_pos = tgt_pos;

        break;
        // Sleep and print
        steps++;
        print_maze();
        usleep(50 * 1000); // 50 ms
    }

    return steps;
}

int main(void) {
    // Print unsolved maze
    puts("Unsolved:");
    print_maze();
    putchar('\n');

    // Solve maze
    int steps = solve_maze();

    // Print solved maze
    puts("Solved:");
    print_maze();

    // Print step count
    printf("\nMaze solved in %d steps.\n", steps);

    return 0;
}
