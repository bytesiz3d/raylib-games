package main

import rl "github.com/gen2brain/raylib-go/raylib"

const (
	GRID_SQUARE_SZ = 20
	GRID_W         = 65
	GRID_H         = 49
	SNAKE_MAX      = GRID_W * GRID_H
	SNAKE_SPEED    = 12.0
)

func main() {
	rl.InitWindow(GRID_W*GRID_SQUARE_SZ, GRID_H*GRID_SQUARE_SZ, "Snake")
	defer rl.CloseWindow()

	var g SnakeGame
	g.Init()

	for !rl.WindowShouldClose() {
		g.Update()
		g.Draw()
	}
}

type SnakeGame struct {
	snake            []rl.Vector2
	snake_direction  Direction
	snake_is_dead    bool
	apple            rl.Vector2
	time_last_update float64
}

var snake_game SnakeGame

func (g SnakeGame) Head() int {
	return len(g.snake) - 1
}

type Direction int

const (
	Direction_Left  Direction = rl.KeyLeft
	Direction_Right Direction = rl.KeyRight
	Direction_Up    Direction = rl.KeyUp
	Direction_Down  Direction = rl.KeyDown
)

func (g *SnakeGame) Init() {
	g.snake = []rl.Vector2{
		{X: GRID_W / 2, Y: GRID_H / 2},
		{X: GRID_W/2 + 1, Y: GRID_H / 2},
		{X: GRID_W/2 + 2, Y: GRID_H / 2},
	}
	g.snake_direction = Direction_Right
	g.snake_is_dead = false
	g.apple = g.RandomVacantCell()
	g.time_last_update = rl.GetTime()
}

func (g *SnakeGame) Update() {
	if g.snake_is_dead {
		if rl.IsKeyPressed(rl.KeySpace) {
			g.Init()
		}
		return
	}

	var pressed_direction Direction
	for key := rl.GetKeyPressed(); key != 0; key = rl.GetKeyPressed() {
		pressed_direction = Direction(key)
		switch {
		case pressed_direction == Direction_Left && g.snake_direction != Direction_Right:
			g.snake_direction = pressed_direction
		case pressed_direction == Direction_Right && g.snake_direction != Direction_Left:
			g.snake_direction = pressed_direction
		case pressed_direction == Direction_Up && g.snake_direction != Direction_Down:
			g.snake_direction = pressed_direction
		case pressed_direction == Direction_Down && g.snake_direction != Direction_Up:
			g.snake_direction = pressed_direction
		}
	}

	if time_now := rl.GetTime(); (time_now-g.time_last_update)*SNAKE_SPEED >= 1 || pressed_direction != 0 {
		g.time_last_update = time_now
		g.UpdateHead()
	}
}

func (g SnakeGame) Draw() {
	rl.BeginDrawing()
	defer rl.EndDrawing()
	rl.ClearBackground(rl.Black)

	if g.snake_is_dead {
		width := rl.MeasureText("GAME OVER", 20)
		rl.DrawText("GAME OVER", (int32(rl.GetScreenWidth())-width)/2, 40, 20, rl.RayWhite)
	}

	for _, cell := range g.snake {
		rl.DrawRectangleRec(CellScreenRec(cell), rl.Lime)
	}
	rl.DrawRectangleRec(CellScreenRec(g.snake[g.Head()]), rl.Green)

	rl.DrawRectangleRec(CellScreenRec(g.apple), rl.Red)
}

func (g SnakeGame) RandomVacantCell() (cell rl.Vector2) {
iteration:
	for range GRID_W * GRID_H {
		cell = rl.Vector2{
			X: float32(rl.GetRandomValue(0, GRID_W-1)),
			Y: float32(rl.GetRandomValue(0, GRID_H-1)),
		}
		for _, snake := range g.snake {
			if rl.Vector2Equals(snake, cell) {
				continue iteration
			}
		}
		break
	}
	return
}

func (g *SnakeGame) UpdateHead() {
	head_new := g.snake[g.Head()]
	switch g.snake_direction {
	case Direction_Left:
		head_new.X--
	case Direction_Right:
		head_new.X++
	case Direction_Up:
		head_new.Y--
	case Direction_Down:
		head_new.Y++
	}

	if CellOutOfBounds(head_new) {
		g.snake_is_dead = true
		return
	}

	for _, body := range g.snake[:g.Head()] {
		if rl.Vector2Equals(body, head_new) {
			g.snake_is_dead = true
			return
		}
	}

	copy(g.snake[:g.Head()], g.snake[1:])
	g.snake[g.Head()] = head_new

	if rl.Vector2Equals(head_new, g.apple) {
		g.snake = append(g.snake, rl.Vector2{})
		copy(g.snake[1:], g.snake[:g.Head()])
		g.apple = g.RandomVacantCell()
	}
}

func CellOutOfBounds(self rl.Vector2) bool {
	return self.X < 0 || self.X >= GRID_W || self.Y < 0 || self.Y >= GRID_H
}

func CellScreenRec(self rl.Vector2) rl.Rectangle {
	return rl.Rectangle{
		X:      self.X * GRID_SQUARE_SZ,
		Y:      self.Y * GRID_SQUARE_SZ,
		Width:  GRID_SQUARE_SZ,
		Height: GRID_SQUARE_SZ,
	}
}
