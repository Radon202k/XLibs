#define STB_IMAGE_IMPLEMENTATION
#include "w:/libs/stb_image.h"

#include "w:/libs/xlibs/xd3d11.h"
#include "w:/libs/xlibs/xrender2d.h"

global XRenderBatch layer1;

typedef struct SnakePiece
{
    v2i position;
    struct SnakePiece *prev;
    struct SnakePiece *next;
} SnakePiece;

typedef struct
{
    v2i position;
    v2i forward;
    SnakePiece *head;
    SnakePiece *tail;
    u32 count;
} Snake;

typedef struct
{
    f32 timer;
    Snake snake;
    v2i apple;
    
} State;

void snake_push_tail(Snake *snake)
{
    SnakePiece *piece = xalloc(sizeof *piece);
    piece->prev = 0;
    piece->next = 0;
    
    if (!snake->head)
    {
        assert(!snake->tail);
        snake->head = piece;
        snake->tail = piece;
        
        piece->position = snake->position;
    }
    else
    {
        piece->position = snake->tail->position;
        
        snake->tail->next = piece;
        piece->prev = snake->tail;
        snake->tail = piece;
    }
    
    ++snake->count;
}

global State state;

void initialize(void);
void render_scene(void);

XWINMAIN()
{
    initialize();
    
    state.snake.position = (v2i){5,5};
    state.snake.forward = (v2i){1,0};
    
    snake_push_tail(&state.snake);
    snake_push_tail(&state.snake);
    snake_push_tail(&state.snake);
    
    f32 seconds = 0.25f;
    state.timer = seconds;
    
    v2i board_size = {(s32)(xd11.back_buffer_size.x/50), (s32)(xd11.back_buffer_size.y/50)};
    state.apple = rnd2i((v2i){0,0}, (v2i){board_size.x-1,board_size.y-1});
    
    while (xd11.running)
    {
        board_size = (v2i){(s32)(xd11.back_buffer_size.x/50), (s32)(xd11.back_buffer_size.y/50)};
        
        xrender2d_pre_update();
        
        /* Arrows input */
        if (xwin.key.up.pressed)
            state.snake.forward = (v2i){0,-1};
        if (xwin.key.down.pressed)
            state.snake.forward = (v2i){0,1};
        if (xwin.key.left.pressed)
            state.snake.forward = (v2i){-1,0};
        if (xwin.key.right.pressed)
            state.snake.forward = (v2i){1,0};
        
        /* Timer */
        state.timer -= xd11.dt;
        if (state.timer <= 0)
        {
            state.timer += seconds;
            
            /* Tick */
            
            /* For all tail pieces */
            SnakePiece *piece = state.snake.tail;
            while (piece && piece->prev)
            {
                /* Move to position of earlier piece */
                piece->position = piece->prev->position;
                
                piece = piece->prev;
            }
            
            /* Move head forward */
            piece = state.snake.head;
            
            /* Wrap head */
            if (piece->position.x + state.snake.forward.x >= board_size.x)
                piece->position.x = 0;
            else if (piece->position.y + state.snake.forward.y >= board_size.y)
                piece->position.y = 0;
            else if (piece->position.x + state.snake.forward.x < 0)
                piece->position.x = board_size.x-1;
            else if (piece->position.y + state.snake.forward.y < 0)
                piece->position.y = board_size.y-1;
            else
                padd2i(&piece->position, state.snake.forward);
            
            /* Check collision with apple */
            if (piece->position.x == state.apple.x &&
                piece->position.y == state.apple.y)
            {
                state.apple = rnd2i((v2i){0,0}, (v2i){board_size.x-1,board_size.y-1});
                snake_push_tail(&state.snake);
                seconds *= 0.98f;
            }
            
            // TODO: Check collision against tail pieces
            
            piece = piece->next;
        }
        
        /* Draw snake */
        SnakePiece *piece = state.snake.head;
        while (piece)
        {
            v2f piece_dim = {50,50};
            v2f piece_pos = {piece->position.x*piece_dim.x, piece->position.y*piece_dim.y};
            draw_rect(&layer1, piece_pos, piece_dim, gol4f);
            
            piece = piece->next;
        }
        
        /* Draw apple */
        draw_rect(&layer1,
                  (v2f){state.apple.x*50.0f + 5, state.apple.y*50.0f + 5},
                  (v2f){40,40},
                  cri4f);
        
        /* Render Scene */
        render_scene();
        
        xrender2d_post_update(&layer1, 1);
        
        /* Reset Batches */
        xrender2d_reset_batch(&layer1);
    }
    
    /* Free renderer resources */
    SnakePiece *piece = state.snake.head;
    while (piece)
    {
        SnakePiece *next = piece->next;
        
        xfree(piece);
        
        piece = next;
    }
    
    xrender2d_shutdown();
    
    xd11_shutdown();
}

void initialize(void)
{
    /* Initialize Direct3D 11 */
    xd11_initialize((XD11Config){window_proc, 0, 0, L"My Window"});
    
    /* Initialize Input */
    xwin_initialize((XWindowConfig){LoadCursor(NULL, IDC_ARROW), xd11.window_handle});
    
    /* Initialize renderer */
    xrender2d_initialize((v4f){.2f,.2f,.2f,1});
    
    random_seed(0);
    
    /* Update texture atlas */
    xd11_texture2d_update(xrender2d.texture_atlas.texture,
                          xrender2d.texture_atlas.bytes);
    
}

void render_scene(void)
{
    draw_grid(&layer1, (v2f){50,50}, (v2f){1,1}, (v4f){.3f,.3f,.3f,1});
    
}

LRESULT window_proc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;
    switch (message) {
        XWNDPROC;
        
        default:
        {
            result = DefWindowProcW(window, message, wParam, lParam);
        } break;
    }
    return result;
}