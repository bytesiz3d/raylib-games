#include <memory.h>
#include <raylib.h>
#include <raymath.h>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

#define GRID_SQUARE_SZ 20
#define GRID_W 65
#define GRID_H 49
#define SNAKE_MAX (GRID_W * GRID_H)
#define SNAKE_SPEED 12.f

bool
CellOutOfBounds(Vector2 cell)
{
	return cell.x < 0 || cell.x >= GRID_W ||
		   cell.y < 0 || cell.y >= GRID_H;
}

Rectangle
CellScreenRec(Vector2 cell)
{
	return (Rectangle){cell.x * GRID_SQUARE_SZ, cell.y * GRID_SQUARE_SZ, GRID_SQUARE_SZ, GRID_SQUARE_SZ};
}

typedef enum Direction
{
	SNAKE_LEFT = KEY_LEFT,
	SNAKE_RIGHT = KEY_RIGHT,
	SNAKE_UP = KEY_UP,
	SNAKE_DOWN = KEY_DOWN,
} Direction;

typedef struct SnakeGame
{
	Vector2 snake[SNAKE_MAX];
	size_t snake_count;
	Direction snake_dir;
	bool snake_is_dead;
	Vector2 apple;
	float time_of_last_update;
} SnakeGame;
SnakeGame gGame = {0};

size_t
SnakeHead(SnakeGame *g)
{
	return g->snake_count - 1;
}

Vector2
RandomVacantCell(SnakeGame *g)
{
	Vector2 cell = {0};
	for (int ITERATION_LIMIT = 0; ITERATION_LIMIT < GRID_W * GRID_H; ITERATION_LIMIT++)
	{
		cell = (Vector2){GetRandomValue(0, GRID_W - 1), GetRandomValue(0, GRID_H - 1)};

		bool hit_snake_body = false;
		for (int i = 0; i < g->snake_count; ++i)
		{
			if (Vector2Equals(g->snake[i], cell))
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
InitGame(SnakeGame *g)
{
	g->snake[0] = (Vector2){GRID_W / 2, GRID_H / 2};
	g->snake[1] = (Vector2){GRID_W / 2 + 1, GRID_H / 2};
	g->snake[2] = (Vector2){GRID_W / 2 + 2, GRID_H / 2};
	g->snake_count = 3;

	g->snake_dir = SNAKE_RIGHT;
	g->snake_is_dead = false;
	g->time_of_last_update = GetTime();

	g->apple = RandomVacantCell(g);
}

void
UpdateHead(SnakeGame *g)
{
	Vector2 head_new = g->snake[SnakeHead(g)];
	switch (g->snake_dir)
	{
	case SNAKE_LEFT: head_new.x -= 1; break;
	case SNAKE_RIGHT: head_new.x += 1; break;
	case SNAKE_UP: head_new.y -= 1; break;
	case SNAKE_DOWN: head_new.y += 1; break;
	}

	if (CellOutOfBounds(head_new))
	{
		g->snake_is_dead = true;
		return;
	}

	for (int i = 1; i < SnakeHead(g); ++i)
	{
		if (Vector2Equals(head_new, g->snake[i]))
		{
			g->snake_is_dead = true;
			return;
		}
	}

	memcpy(g->snake, g->snake + 1, SnakeHead(g) * sizeof(g->snake[0]));
	g->snake[SnakeHead(g)] = head_new;

	if (Vector2Equals(head_new, g->apple))
	{
		g->snake_count++;
		memcpy(g->snake + 1, g->snake, SnakeHead(g) * sizeof(g->snake[0]));
		g->apple = RandomVacantCell(g);
	}
}

void
Update(SnakeGame *g)
{
	if (g->snake_is_dead == true)
	{
		if (IsKeyPressed(KEY_SPACE))
			InitGame(g);
		return;
	}

	Direction pressed_dir = 0;
	for (int key; key = GetKeyPressed();)
	{
		pressed_dir = key;
		if (pressed_dir == SNAKE_LEFT && g->snake_dir != SNAKE_RIGHT)
			g->snake_dir = SNAKE_LEFT;
		if (pressed_dir == SNAKE_RIGHT && g->snake_dir != SNAKE_LEFT)
			g->snake_dir = SNAKE_RIGHT;
		if (pressed_dir == SNAKE_UP && g->snake_dir != SNAKE_DOWN)
			g->snake_dir = SNAKE_UP;
		if (pressed_dir == SNAKE_DOWN && g->snake_dir != SNAKE_UP)
			g->snake_dir = SNAKE_DOWN;
	}

	float time_now = GetTime();
	if ((time_now - g->time_of_last_update) * SNAKE_SPEED >= 1 || pressed_dir != 0)
	{
		g->time_of_last_update = time_now;
		UpdateHead(g);
	}
}

void
Draw(SnakeGame *g)
{
	BeginDrawing();
	{
		ClearBackground(BLACK);

		if (g->snake_is_dead == true)
		{
			int width = MeasureText("GAME OVER", 20);
			DrawText("GAME OVER", (GetScreenWidth() - width) / 2, 40, 20, RAYWHITE);
		}

		for (int i = 0; i < SnakeHead(g); ++i)
			DrawRectangleRec(CellScreenRec(g->snake[i]), LIME);
		DrawRectangleRec(CellScreenRec(g->snake[SnakeHead(g)]), GREEN);

		DrawRectangleRec(CellScreenRec(g->apple), RED);
	}
	EndDrawing();
}

void
UpdateDrawFrame()
{
	Update(&gGame);
	Draw(&gGame);
}

int
main()
{
	InitWindow(GRID_SQUARE_SZ * GRID_W, GRID_SQUARE_SZ * GRID_H, "Snake");
	InitGame(&gGame);

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