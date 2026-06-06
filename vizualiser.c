//                                          time: 3:22:30


#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int scc(int code){
    if(code < 0){
        fprintf(stderr, "SDL ERROR: %s\n", SDL_GetError());
        exit(1);
    }
    return code;
}

void* scp(void* ptr){
    if(ptr == NULL){
        fprintf(stderr, "SDL ERROR: %s\n", SDL_GetError());
        exit(1);
    }
    return ptr;
}


#define SCREEN_WIDHT 1024
#define SCREEN_HEIGHT 786

#define BACKGROUND_COLOR {.hex = 0xffaabbcc}



#define ROWS 110
#define COLS 90

#define CELL_WIDTH  ((float)SCREEN_WIDHT / (float)COLS)
#define CELL_HEIGHT ((float) SCREEN_HEIGHT / (float)ROWS)


typedef enum{
    O = 0,
    I
} Cell;

typedef struct{
    union{
        unsigned int hex;
        struct{
            unsigned char r;
            unsigned char g;
            unsigned char b;
            unsigned char a;
        };
    };
} Color;

Color cell_color[2]  = {
    [O] = {.hex = 0x00000000},
    [I] = {.hex = 0xffaabbcc}
};


#define PATTERN(A, B, C) ((A << 2) | (B << 1) | (C))

Cell cell_pattern[8] = {
    [PATTERN(O, O, O)] = O,
    [PATTERN(O, O, I)] = I,
    [PATTERN(O, I, O)] = I,
    [PATTERN(O, I, I)] = I,
    [PATTERN(I, O, O)] = O,
    [PATTERN(I, O, I)] = I,
    [PATTERN(I, I, O)] = I,
    [PATTERN(I, I, I)] = O
};

typedef struct{
    Cell cells[COLS];
} Row;

Row next_row(Row prev){
    Row next = {0};

    for(int i = 1; i < COLS - 1; i++){
        const int index = PATTERN(prev.cells[(i-1)],
                                  prev.cells[(i)],
                                  prev.cells[(i+1)]);
        next.cells[i] = cell_pattern[index];
    }

    return next;
}


Row random_row(void){
    Row result = {0};

    for(int i = 0; i < COLS; i++){
        result.cells[i] = rand() % 2;
    }
    return result;
}

void render_row(SDL_Renderer *renderer, Row row, int row_index){
    int current_y = (int)floorf(row_index)*CELL_HEIGHT;
    int next_y = (int)floorf((row_index + 1)*CELL_HEIGHT);

    int h = next_y - current_y;

    for(int i = 0; i < COLS; i++){

        int current_x = (int)floorf(i * CELL_WIDTH);
        int next_x = (int)floorf((i+1) * CELL_WIDTH);
        int w = next_x - current_x;

        const SDL_Rect rect = {
            .x = current_x,
            .y = current_y,
            .w = w,
            .h = h
        };
        scc(SDL_SetRenderDrawColor(renderer,
                                   cell_color[row.cells[i]].r,
                                   cell_color[row.cells[i]].g,
                                   cell_color[row.cells[i]].b,
                                   cell_color[row.cells[i]].a));

        scc(SDL_RenderFillRect(renderer, &rect));
    }
}

int main(){

    Color background_color = {.hex = 0xff000000};



    scc(SDL_Init(SDL_INIT_VIDEO));
    SDL_Window* const window =
            scp(SDL_CreateWindow("Cellular Automata",
                                  0, 0,
                                  SCREEN_WIDHT, SCREEN_HEIGHT,
                                  SDL_WINDOW_RESIZABLE));

    SDL_Renderer* const renderer =
                scp(SDL_CreateRenderer(window,
                                       -1,
                                       SDL_RENDERER_ACCELERATED));
    scc(SDL_RenderSetLogicalSize(renderer,
                                 SCREEN_WIDHT,
                                 SCREEN_HEIGHT));

    bool quit = false;

    Row row[ROWS] = {0};
    _Static_assert(ROWS > 0, "We need to have atleast 1 row\n");
    row[0] = random_row();

    for(int i = 1; i < ROWS; i++){
        row[i] = next_row(row[i-1]);
    }

    int counter = 1;

    while(!quit){
        SDL_Event event;

        while(SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_QUIT: {
                    quit = true;
                } break;
            }
        }

        scc(SDL_SetRenderDrawColor(
                renderer,
                background_color.r,
                background_color.g,
                background_color.b,
                background_color.a));

        scc(SDL_RenderClear(renderer));

        for(int i = 0; i < counter; i++){
            render_row(renderer, row[i], i);
        }
        if(counter < ROWS) counter++;

        SDL_RenderPresent(renderer);
        SDL_Delay(100);
    }

    SDL_Quit();
    return 0;
}