#include <raylib.h>
#include <raymath.h>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

struct
{
	int player_left, player_right;
	int player_left_score, player_right_score;
	Vector2 ball_position, ball_direction;
} GAME;

#define SCREEN_W     1200
#define SCREEN_H     800
#define PLAYER_H     120
#define PLAYER_W     30
#define BALL_W       20
#define BALL_H       20
#define PLAYER_SPEED (SCREEN_H / 1.5f)
#define BALL_SPEED   (SCREEN_W / 2.0f)
#define BOUNCE_ANGLE (PI / 3)
#define BALL_CENTER  ((Vector2){(SCREEN_W - BALL_W) / 2.f, (SCREEN_H - BALL_H) / 2.f})

void
InitGame()
{
	GAME.player_left = GAME.player_right = (SCREEN_H - PLAYER_H) / 2;
	GAME.player_left_score = GAME.player_right_score = 0;

	GAME.ball_position = BALL_CENTER;
	GAME.ball_direction = (Vector2){-1.f, 0.f};
}

float
PaddleHitRange(float player)
{
	float range = (GAME.ball_position.y + BALL_H / 2) - (player + PLAYER_H / 2); // [-PLAYER_H/2, PLAYER_H/2]
	return range / (PLAYER_H / 2); // [-1, 1]
}

Rectangle
PlayerLeftRectangle()
{
	return (Rectangle){0, GAME.player_left, PLAYER_W, PLAYER_H};
}

Rectangle
PlayerRightRectangle()
{
	return (Rectangle){SCREEN_W - PLAYER_W, GAME.player_right, PLAYER_W, PLAYER_H};
}

Rectangle
BallRectangle()
{
	return (Rectangle){GAME.ball_position.x, GAME.ball_position.y, BALL_W, BALL_H};
}

void
UpdateDrawFrame(void)
{
	float player_distance_travelled = GetFrameTime() * PLAYER_SPEED;
	if (IsKeyDown(KEY_W))
	{
		GAME.player_left = Clamp(GAME.player_left - player_distance_travelled, 0, SCREEN_H - PLAYER_H);
	}
	if (IsKeyDown(KEY_S))
	{
		GAME.player_left = Clamp(GAME.player_left + player_distance_travelled, 0, SCREEN_H - PLAYER_H);
	}
	if (IsKeyDown(KEY_DOWN))
	{
		GAME.player_right = Clamp(GAME.player_right + player_distance_travelled, 0, SCREEN_H - PLAYER_H);
	}
	if (IsKeyDown(KEY_UP))
	{
		GAME.player_right = Clamp(GAME.player_right - player_distance_travelled, 0, SCREEN_H - PLAYER_H);
	}

	Vector2 ball_distance_travelled = Vector2Scale(GAME.ball_direction, GetFrameTime() * BALL_SPEED);
	GAME.ball_position = Vector2Clamp(
		Vector2Add(GAME.ball_position, ball_distance_travelled),
		Vector2Zero(), (Vector2){SCREEN_W - BALL_W, SCREEN_H - BALL_H}
	);

	bool ball_passed_left  = GAME.ball_position.x == 0;
	bool ball_passed_right = GAME.ball_position.x + BALL_W == SCREEN_W;

	bool ball_hit_top      = GAME.ball_position.y == 0;
	bool ball_hit_bottom   = GAME.ball_position.y + BALL_H == SCREEN_H;

	bool player_left_hit = 
		GAME.ball_direction.x < 0 && CheckCollisionRecs(PlayerLeftRectangle(), BallRectangle());

	bool player_right_hit =
		GAME.ball_direction.x > 0 && CheckCollisionRecs(PlayerRightRectangle(), BallRectangle());

	if (ball_passed_left)
	{
		GAME.player_right_score += 1;
		GAME.ball_position = BALL_CENTER;
		GAME.ball_direction = (Vector2){-1.f, 0.f};
	}
	else if (ball_passed_right)
	{
		GAME.player_left_score += 1;
		GAME.ball_position = BALL_CENTER;
		GAME.ball_direction = (Vector2){1.f, 0.f};
	}
	else if (ball_hit_top)
	{
		GAME.ball_direction = Vector2Reflect(GAME.ball_direction, (Vector2){0.f, 1.f});
	}
	else if (ball_hit_bottom)
	{
		GAME.ball_direction = Vector2Reflect(GAME.ball_direction, (Vector2){0.f, -1.f});
	}
	else if (player_left_hit)
	{
		float range = PaddleHitRange(GAME.player_left);
		GAME.ball_direction = (Vector2){cosf(range * BOUNCE_ANGLE), sinf(range * BOUNCE_ANGLE)};
	}
	else if (player_right_hit)
	{
		float range = PaddleHitRange(GAME.player_right);
		GAME.ball_direction = (Vector2){-cosf(range * BOUNCE_ANGLE), sinf(range * BOUNCE_ANGLE)};
	}

	BeginDrawing();
	{
		ClearBackground(BLACK);

		DrawLine(SCREEN_W / 2, 0, SCREEN_W / 2, SCREEN_H, RAYWHITE);

		DrawText(TextFormat("%d", GAME.player_left_score), SCREEN_W / 4, 10, 32, RAYWHITE);
		DrawText(TextFormat("%d", GAME.player_right_score), 3 * SCREEN_W / 4, 10, 32, RAYWHITE);

		DrawRectangleRec(PlayerLeftRectangle(), RAYWHITE);
		DrawRectangleRec(PlayerRightRectangle(), RAYWHITE);
		DrawRectangleRec(BallRectangle(), LIGHTGRAY);
	}
	EndDrawing();
}

int
main()
{
	InitWindow(SCREEN_W, SCREEN_H, "Pong");
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