export function CartInit() {
  InitWindow(800, 450, "raylib [core] example - basic window");
}

export function CartUpdate() {
  ClearBackground(RAYWHITE);
  DrawText(
    "Congrats! You created your first raycart JS window!",
    135,
    200,
    20,
    LIGHTGRAY,
  );
}
