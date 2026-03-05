#include "game.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

/* Helper function definitions */
static void set_board_at(game_t *game, unsigned int row, unsigned int col,
                         char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_t *game, unsigned int snum);
static char next_square(game_t *game, unsigned int snum);
static void update_tail(game_t *game, unsigned int snum);
static void update_head(game_t *game, unsigned int snum);

/* Task 1 */
game_t *create_default_game() {
  // TODO: Implement this function.
  game_t *game = malloc(sizeof(game_t)); // Changed name from 'default'
  if (game == NULL)
    return NULL;

  game->num_rows = 18;  // [cite: 29]
  game->num_snakes = 1; // [cite: 25]

  game->board = malloc(game->num_rows * sizeof(char *)); // [cite: 23]
  for (int i = 0; i < game->num_rows; i++) {
    game->board[i] =
        malloc(22 * sizeof(char)); // 20 chars + \n + \0 [cite: 24, 30]
    if (i == 0 || i == 17) {
      strcpy(game->board[i], "####################\n");
    } else {
      strcpy(game->board[i], "#                  #\n");
    }
  }

  // Set snake and fruit
  game->board[2][2] = 'd';
  game->board[2][3] = '>';
  game->board[2][4] = 'D';
  game->board[2][9] = '*';

  // Allocate snake array [cite: 25]
  game->snakes = malloc(sizeof(snake_t)); // Only one malloc here!
  game->snakes[0].tail_row = 2;
  game->snakes[0].tail_col = 2;
  game->snakes[0].head_row = 2;
  game->snakes[0].head_col = 4;
  game->snakes[0].live = true;

  return game;
}

/* Task 2 */
void free_game(game_t *game) {
  // TODO: Implement this function.
  for (int i = 0; i < game->num_rows; i++) {
    free(game->board[i]);
  }
  free(game->board);
  free(game->snakes);
  free(game);
  return;
}

/* Task 3 */
void print_board(game_t *game, FILE *fp) {
  // TODO: Implement this function.
  for (int i = 0; i < game->num_rows; i++) {
    fprintf(fp, "%s", game->board[i]);
  }
  return;
}

/*
  Saves the current game into filename. Does not modify the game object.
  (already implemented for you).
*/
void save_board(game_t *game, char *filename) {
  FILE *f = fopen(filename, "w");
  print_board(game, f);
  fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_t *game, unsigned int row, unsigned int col) {
  return game->board[row][col];
}

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_t *game, unsigned int row, unsigned int col,
                         char ch) {
  game->board[row][col] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c) {
  // TODO: Implement this function.
  return c == 'w' || c == 'w' || c == 'a' || c == 's';
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c) {
  // TODO: Implement this function.
  return c == 'D' || c == 'W' || c == 'A' || c == 'S';
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  // TODO: Implement this function.
  return c == 'w' || c == 'w' || c == 'a' || c == 's' || c == 'D' || c == 'W' ||
         c == 'A' || c == 'S' || c == 'v' || c == '^' || c == '>' || c == '<';
}

/*
  Converts a character in the snake's body ("^<v>")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c) {
  // TODO: Implement this function.
  switch (c) {
  case '^':
    return 'w';
    break;

  case 'v':
    return 's';
    break;

  case '<':
    return 'a';
    break;

  case '>':
    return 'd';
    break;

  default:
    return '\0';
  }
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c) {
  // TODO: Implement this function.
  switch (c) {
  case 'W':
    return '^';
    break;

  case 'S':
    return 'v';
    break;

  case 'D':
    return '>';
    break;

  case 'A':
    return '<';
    break;

  default:
    return '\0';
  }
}

/*
  Returns cur_row + 1 if c is 'v' or 's' or 'S'.
  Returns cur_row - 1 if c is '^' or 'w' or 'W'.
  Returns cur_row otherwise.
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  // TODO: Implement this function.
  if (c == 'v' || c == 's' || c == 'S') {
    return cur_row + 1;
  } else if (c == '^' || c == 'w' || c == 'W') {
    return cur_row - 1;
  } else {
    return cur_row;
  }
}

/*
  Returns cur_col + 1 if c is '>' or 'd' or 'D'.
  Returns cur_col - 1 if c is '<' or 'a' or 'A'.
  Returns cur_col otherwise.
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  // TODO: Implement this function.
  if (c == '>' || c == 'd' || c == 'D') {
    return cur_col + 1;
  } else if (c == '<' || c == 'a' || c == 'A') {
    return cur_col - 1;
  } else {
    return cur_col;
  }
}

/*
  Task 4.2

  Helper function for update_game. Return the character in the cell the snake
  is moving into.
i
  This function should not modify anything.
*/
static char next_square(game_t *game, unsigned int snum) {
  // TODO: Implement this function.
  unsigned int head_row = game->snakes[snum].head_row;
  unsigned int head_col = game->snakes[snum].head_col;

  char current_head = get_board_at(game, head_row, head_col);

  unsigned int next_col = get_next_col(head_col, current_head);
  unsigned int next_row = get_next_row(head_row, current_head);
  return get_board_at(game, next_row, next_col);
}

/*
  Task 4.3

  Helper function for update_game. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving
  the head.
*/
static void update_head(game_t *game, unsigned int snum) {
  // TODO: Implement this function.
  //

  unsigned int head_col = game->snakes[snum].head_col;
  head_col++;
}

/*
  Task 4.4

  Helper function for update_game. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a tail character (wasd)

  ...in the snake struct: update the row and col of the tail
*/
static void update_tail(game_t *game, unsigned int snum) {
  // TODO: Implement this function.
  return;
}

/* Task 4.5 */
void update_game(game_t *game, int (*add_food)(game_t *game)) {
  // TODO: Implement this function.
  return;
}

/* Task 5.1 */
char *read_line(FILE *fp) {
  // TODO: Implement this function.
  return NULL;
}

/* Task 5.2 */
game_t *load_board(FILE *fp) {
  // TODO: Implement this function.
  return NULL;
}

/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail row and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
static void find_head(game_t *game, unsigned int snum) {
  // TODO: Implement this function.
  return;
}

/* Task 6.2 */
game_t *initialize_snakes(game_t *game) {
  // TODO: Implement this function.
  return NULL;
}
