#include <raylib.h>
#include <raymath.h>
#include <memory.h>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

#define GRID_SQUARE_SZ 20
#define GRID_W 65
#define GRID_H 49
#define SNAKE_MAX (GRID_W * GRID_H)
#define SNAKE_SPEED 10.f

typedef struct Grid_Cell
{
	int x, y;
} Grid_Cell;

bool
CellEqual(Grid_Cell a, Grid_Cell b)
{
	return a.x == b.x && a.y == b.y;
}

bool
CellOutOfBounds(Grid_Cell cell)
{
	return cell.x < 0 || cell.x >= GRID_W ||
		   cell.y < 0 || cell.y >= GRID_H;
}

Grid_Cell snake[SNAKE_MAX] = {0};
int snake_count;
#define SNAKE_HEAD (snake_count-1)

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
	Grid_Cell cell = {0};
	for (int ITERATION_LIMIT = 0; ITERATION_LIMIT < GRID_W * GRID_H; ITERATION_LIMIT++)
	{
		cell = (Grid_Cell){GetRandomValue(0, GRID_W - 1), GetRandomValue(0, GRID_H - 1)};

		bool hit_snake_body = false;
		for (int i = 0; i < snake_count; ++i)
		{
			if (CellEqual(snake[i], cell))
			{
				hit_snake_body = true;
				break;
			}
		}
		if (hit_snake_body == false)
			return cell;
	}
	return cell;
}

void
InitGame()
{
	snake[0] = (Grid_Cell){GRID_W / 2, GRID_H / 2};
	snake[1] = (Grid_Cell){snake[0].x + 1, snake[0].y};
	snake[2] = (Grid_Cell){snake[1].x + 1, snake[1].y};
	snake_count = 3;

	snake_dir = SNAKE_RIGHT;
	snake_state = SNAKE_ALIVE;
	time_of_last_update = GetTime();

	apple_pos = RandomVacantCell();
}

void
Update(void)
{
	if (snake_state == SNAKE_DEAD)
	{
		if (IsKeyPressed(KEY_SPACE))
			InitGame();

		return;
	}

	for (int key; key = GetKeyPressed();)
	{
		if (key == KEY_RIGHT && snake_dir != SNAKE_LEFT)
			snake_dir = SNAKE_RIGHT;
		if (key == KEY_LEFT && snake_dir != SNAKE_RIGHT)
			snake_dir = SNAKE_LEFT;
		if (key == KEY_UP && snake_dir != SNAKE_DOWN)
			snake_dir = SNAKE_UP;
		if (key == KEY_DOWN && snake_dir != SNAKE_UP)
			snake_dir = SNAKE_DOWN;
	}

	float time_now = GetTime();
	if ((time_now - time_of_last_update) * SNAKE_SPEED >= 1)
	{
		time_of_last_update = time_now;

		Grid_Cell head_new = snake[SNAKE_HEAD];
		switch (snake_dir)
		{
		case SNAKE_LEFT: head_new.x -= 1; break;
		case SNAKE_RIGHT: head_new.x += 1; break;
		case SNAKE_UP: head_new.y -= 1; break;
		case SNAKE_DOWN: head_new.y += 1; break;
		}

		if (CellOutOfBounds(head_new))
		{
			snake_state = SNAKE_DEAD;
			return;
		}
		for (int i = 1; i < SNAKE_HEAD; ++i) // Ate self, skip last tail
		{
			if (CellEqual(head_new, snake[i]))
			{
				snake_state = SNAKE_DEAD;
				return;
			}
		}

		memcpy(snake, snake + 1, snake_count * sizeof(snake[0]));
		snake[SNAKE_HEAD] = head_new;
	}

	if (CellEqual(snake[SNAKE_HEAD], apple_pos))
	{
		memcpy(snake + 1, snake, snake_count * sizeof(snake[0]));
		snake_count++;
		apple_pos = RandomVacantCell();
	}
}

void
UpdateDrawFrame(void)
{
	Update();

	BeginDrawing();
	{
		ClearBackground(BLACK);

		for (int i = 0; i < SNAKE_HEAD; ++i)
			DrawRectangle(snake[i].x * GRID_SQUARE_SZ, snake[i].y * GRID_SQUARE_SZ, GRID_SQUARE_SZ, GRID_SQUARE_SZ, RAYWHITE);

		DrawRectangle(snake[SNAKE_HEAD].x * GRID_SQUARE_SZ, snake[SNAKE_HEAD].y * GRID_SQUARE_SZ, GRID_SQUARE_SZ, GRID_SQUARE_SZ, LIGHTGRAY);

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