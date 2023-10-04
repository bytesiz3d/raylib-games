#include <raylib.h>
#include <raymath.h>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

#define GRID_SQUARE_SZ 20
#define GRID_W 65
#define GRID_H 49
#define SNAKE_MAX (GRID_W * GRID_H)
#define SNAKE_SPEED 10.f

struct Grid_Cell
{
	int x, y;

	bool
	operator==(const Grid_Cell& other) const = default;
};

bool
CellInside(Grid_Cell cell)
{
	return 0 <= cell.x && cell.x < GRID_W && 0 <= cell.y && cell.y < GRID_H;
}

Grid_Cell snake[SNAKE_MAX] = {0};
int snake_count;
int snake_head;
int snake_tail;
#define SNAKE_INC(cell) (snake_tail + ((cell - snake_tail + 1) % snake_count))

enum
{
	SNAKE_LEFT,
	SNAKE_RIGHT,
	SNAKE_UP,
	SNAKE_DOWN
} snake_dir;

enum
{
	SNAKE_DEAD,
	SNAKE_ALIVE
} snake_state;

Grid_Cell apple_pos = {0};
float time_of_last_update;

Grid_Cell
RandomVacantCell()
{
	Grid_Cell cell{};

	for (int ITERATION_LIMIT = 0; ITERATION_LIMIT < GRID_W * GRID_H; ITERATION_LIMIT++)
	{
		cell = {GetRandomValue(0, GRID_W - 1), GetRandomValue(0, GRID_H - 1)};

		bool hit_snake_body = false;
		int i = snake_tail;
		do
		{
			if (snake[i] == apple_pos)
			{
				hit_snake_body = true;
				break;
			}
		} while ((i = SNAKE_INC(i)) != snake_tail);

		if (hit_snake_body == false)
			return cell;
	}
	return cell;
}

void
InitGame()
{
	apple_pos = RandomVacantCell();

	snake[0] = {GRID_W / 2, GRID_H / 2};
	snake[1] = {snake[0].x + 1, snake[0].y};
	snake[2] = {snake[1].x + 1, snake[1].y};
	snake_tail = 0;
	snake_head = 2;
	snake_count = 3;

	snake_dir = SNAKE_RIGHT;
	snake_state = SNAKE_ALIVE;
	time_of_last_update = GetTime();
}

void
UpdateDrawFrame(void)
{
	int key;
	while (key = GetKeyPressed())
	{
		switch (key)
		{
		case KEY_RIGHT:
			if (snake_dir != SNAKE_LEFT) snake_dir = SNAKE_RIGHT;
			break;
		case KEY_LEFT:
			if (snake_dir != SNAKE_RIGHT) snake_dir = SNAKE_LEFT;
			break;
		case KEY_UP:
			if (snake_dir != SNAKE_DOWN) snake_dir = SNAKE_UP;
			break;
		case KEY_DOWN:
			if (snake_dir != SNAKE_UP) snake_dir = SNAKE_DOWN;
			break;
		}
	}

	if (IsKeyDown(KEY_SPACE) && snake_state == SNAKE_DEAD)
		InitGame();

	if (snake_state == SNAKE_ALIVE)
	{
		float time_now = GetTime();
		if ((time_now - time_of_last_update) * SNAKE_SPEED >= 1)
		{
			time_of_last_update = time_now;

			Grid_Cell head_new = snake[snake_head];
			switch (snake_dir)
			{
			case SNAKE_LEFT: head_new.x -= 1; break;
			case SNAKE_RIGHT: head_new.x += 1; break;
			case SNAKE_UP: head_new.y -= 1; break;
			case SNAKE_DOWN: head_new.y += 1; break;
			}

			snake_head = (snake_head + 1) % SNAKE_MAX;
			snake_tail = (snake_tail + 1) % SNAKE_MAX;
			snake[snake_head] = head_new;
		}

		if (snake[snake_head] == apple_pos)
		{
			int snake_tail_new = (snake_tail - 1 + SNAKE_MAX) % SNAKE_MAX;
			snake[snake_tail_new] = snake[snake_tail];
			snake_tail = snake_tail_new;
			snake_count += 1;
			apple_pos = RandomVacantCell();
		}
		else if (!CellInside(snake[snake_head]))
		{
			snake_state = SNAKE_DEAD;
			snake_head = (snake_head - 1 + SNAKE_MAX) % SNAKE_MAX; // Rewind one move
			snake_tail = (snake_tail - 1 + SNAKE_MAX) % SNAKE_MAX;
		}
		else
		{
			for (int i = snake_tail; i != snake_head; i = SNAKE_INC(i))
			{
				if (snake[i] == snake[snake_head])
				{
					snake_state = SNAKE_DEAD;
					break;
				}
			}
		}
	}

	BeginDrawing();
	{
		ClearBackground(BLACK);

		for (int i = snake_tail; i != snake_head; i = SNAKE_INC(i))
			DrawRectangle(snake[i].x * GRID_SQUARE_SZ, snake[i].y * GRID_SQUARE_SZ, GRID_SQUARE_SZ, GRID_SQUARE_SZ, RAYWHITE);
		DrawRectangle(snake[snake_head].x * GRID_SQUARE_SZ, snake[snake_head].y * GRID_SQUARE_SZ, GRID_SQUARE_SZ, GRID_SQUARE_SZ, LIGHTGRAY);

		DrawRectangle(apple_pos.x * GRID_SQUARE_SZ, apple_pos.y * GRID_SQUARE_SZ, GRID_SQUARE_SZ, GRID_SQUARE_SZ, RED);

		if (snake_state == SNAKE_DEAD)
			DrawText("GAME OVER", GetScreenWidth() / 2 - 90, 40, 20, RAYWHITE);
	}
	EndDrawing();
}

int
main()
{
	InitWindow(GRID_SQUARE_SZ * GRID_W, GRID_SQUARE_SZ * GRID_H, "Snake");
	InitGame();

#if defined(PLATFORM_WEB)
	emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
	SetTargetFPS(60);
	while (!WindowShouldClose())
		UpdateDrawFrame();
#endif
	CloseWindow();
	return 0;
}