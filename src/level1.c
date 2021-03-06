// Copyright (C) 2021 Inderjit Gill <email@indy.io>

// This file is part of Rona

// Rona is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// Rona is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.

// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

/*

  | Symbol | Description |
  |--------|-------------|
  |   H    | Hero        |
  |   B    | Block       |
  |   U    | Pit         |
*/
void level1_startup(Level* level, GameState* game_state) {

  // this bullshit comment is here to stop clang-format from repeatedly indenting/unindeting the next comment
  // clang-format off
// this bullshit comment is here to stop clang-format from repeatedly indenting/unindeting the previous comment


  // declare as: layout[height][width*2]
  #define L1_WIDTH 9
  #define L1_HEIGHT 11
  char layout[L1_HEIGHT][L1_WIDTH * 2] = {
    "    1 2 2 2 2 2 3 ",
    "    4 . . U . s 6 ",
    "    4 . B z 8 8 9 ",
    "    4 . . 6       ",
    "1 2 w . B x 2 2 3 ",
    "4 H . . . U . s 6 ",
    "7 8 y . U z 8 8 9 ",
    "    4 . . 6       ",
    "    4 . . x 2 2 3 ",
    "    4 . . U . s 6 ",
    "    7 8 8 8 8 8 9 "
  };
  // clang-format on

  level_build(game_state, level, L1_WIDTH * 2, L1_HEIGHT, layout);
  if (!command_system_startup(&level->undo_redo, &level->fixed_block_allocator,
                              RESERVE_NUM_COMMANDS_IN_UNDO_BUFFER)) {
    rona_error("level1_startup: command_system_startup failed");
  }
}

void level1_shutdown(Level* level) {
  command_system_shutdown(&level->undo_redo);
  level->bump_allocator.used = 0;
}
