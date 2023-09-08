#include <raylib.h>
#include <raymath.h>
#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

struct
{
	int player_left, player_right;
	int player_left_score, player_right_score;
	int player_speed;

	Vector2 ball_position;
	Vector2 ball_direction;
	int ball_speed;
} GAME;

constexpr auto SCREEN_W = 1200;
constexpr auto SCREEN_H = 800;
constexpr auto PLAYER_H = 120;
constexpr auto PLAYER_W = 30;
constexpr auto BALL_W = 20;
constexpr auto BALL_H = 20;
constexpr auto BOUNCE_ANGLE = (PI / 3);
constexpr Vector2 BALL_CENTER = {(SCREEN_W - BALL_W) / 2.f, (SCREEN_H - BALL_H) / 2.f};

void
InitGame()
{
	GAME = {0};

	GAME.player_left = GAME.player_right = (SCREEN_H - PLAYER_H) / 2;
	GAME.player_left_score = GAME.player_right_score = 0;
	GAME.player_speed = SCREEN_H / 1.5f;

	GAME.ball_position = BALL_CENTER;
	GAME.ball_direction = {-1.f, 0.f};
	GAME.ball_speed = SCREEN_W / 2.f;
}

float
PaddleCollisionRange(float player)
{
	float range = (GAME.ball_position.y + BALL_H / 2) - (player + PLAYER_H / 2); // [-PLAYER_H/2, PLAYER_H/2]
	return range / (PLAYER_H / 2); // [-1, 1]
}

Rectangle
BallRectangle()
{
	return {GAME.ball_position.x, GAME.ball_position.y, BALL_W, BALL_H};
}

void
UpdateDrawFrame(void)
{
	if (IsKeyDown(KEY_W))
	{
		GAME.player_left = Clamp(GAME.player_left - GetFrameTime() * GAME.player_speed, 0, SCREEN_H - PLAYER_H);
	}
	if (IsKeyDown(KEY_S))
	{
		GAME.player_left = Clamp(GAME.player_left + GetFrameTime() * GAME.player_speed, 0, SCREEN_H - PLAYER_H);
	}
	if (IsKeyDown(KEY_DOWN))
	{
		GAME.player_right = Clamp(GAME.player_right + GetFrameTime() * GAME.player_speed, 0, SCREEN_H - PLAYER_H);
	}
	if (IsKeyDown(KEY_UP))
	{
		GAME.player_right = Clamp(GAME.player_right - GetFrameTime() * GAME.player_speed, 0, SCREEN_H - PLAYER_H);
	}

	Vector2 ball_distance_travelled = Vector2Scale(GAME.ball_direction, GetFrameTime() * GAME.ball_speed);
	GAME.ball_position = Vector2Clamp(
		Vector2Add(GAME.ball_position, ball_distance_travelled),
		{0, 0}, {(float)SCREEN_W - BALL_W, (float)SCREEN_H - BALL_H}
	);

	bool player_left_hit = GAME.ball_direction.x < 0 && CheckCollisionRecs(
		{0.f, (float)GAME.player_left, PLAYER_W, PLAYER_H},
		BallRectangle()
	);

	bool player_right_hit = GAME.ball_direction.x > 0 && CheckCollisionRecs(
		{(float)SCREEN_W - PLAYER_W, (float)GAME.player_right, PLAYER_W, PLAYER_H},
		BallRectangle()
	);

	if (GAME.ball_position.x == 0)
	{
		GAME.player_right_score += 1;
		GAME.ball_position = BALL_CENTER;
		GAME.ball_direction = {-1.0f, 0.f};
	}
	else if (GAME.ball_position.x == SCREEN_W - BALL_W)
	{
		GAME.player_left_score += 1;
		GAME.ball_position = BALL_CENTER;
		GAME.ball_direction = {1.0f, 0.f};
	}
	else if (player_left_hit)
	{
		float range = PaddleCollisionRange(GAME.player_left);
		GAME.ball_direction = {cosf(range * BOUNCE_ANGLE), sinf(range * BOUNCE_ANGLE)};
	}
	else if (player_right_hit)
	{
		float range = PaddleCollisionRange(GAME.player_right);
		GAME.ball_direction = {-cosf(range * BOUNCE_ANGLE), sinf(range * BOUNCE_ANGLE)};
	}
	else if (GAME.ball_position.y == 0)
	{
		GAME.ball_direction = Vector2Reflect(GAME.ball_direction, {0.f, 1.f});
	}
	else if (GAME.ball_position.y == SCREEN_H - BALL_H)
	{
		GAME.ball_direction = Vector2Reflect(GAME.ball_direction, {0.f, -1.f});
	}

	BeginDrawing();
	{
		ClearBackground(BLACK);

		DrawLine(SCREEN_W / 2, 0, SCREEN_W / 2, SCREEN_H, RAYWHITE);

		DrawText(TextFormat("%d", GAME.player_left_score), SCREEN_W / 4, 10, 32, RAYWHITE);
		DrawText(TextFormat("%d", GAME.player_right_score), 3 * SCREEN_W / 4, 10, 32, RAYWHITE);

		DrawRectangle(0, GAME.player_left, PLAYER_W, PLAYER_H, RAYWHITE);
		DrawRectangle(SCREEN_W - PLAYER_W, GAME.player_right, PLAYER_W, PLAYER_H, RAYWHITE);

		DrawRectangle(GAME.ball_position.x, GAME.ball_position.y, BALL_W, BALL_H, LIGHTGRAY);
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