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

void command_execute(Command* command, CommandExecute execute_type, GameState* game_state) {
  Entity* e = command->entity;

  //  command_pretty_print(command, undo, "execute");

  switch (command->type) {

    /*
     * Entity Move
     */
  case CommandType_EntityMove:
    switch (execute_type) {
    case CommandExecute_Play:
      vec2i_copy(&e->board_pos, &command->params.entity_move.new_params.board_pos);
      // world_pos will smoothly transition to world_target
      vec3_copy(&e->world_target, &command->params.entity_move.new_params.world_target);
      e->entity_state = command->params.entity_move.new_params.entity_state;
      break;
    case CommandExecute_Undo:
      vec2i_copy(&e->board_pos, &command->params.entity_move.old_params.board_pos);
      vec3_copy(&e->world_pos, &command->params.entity_move.old_params.world_pos);
      vec3_copy(&e->world_target, &command->params.entity_move.old_params.world_target);
      e->entity_state = command->params.entity_move.old_params.entity_state;
      break;
    case CommandExecute_Redo:
      vec2i_copy(&e->board_pos, &command->params.entity_move.new_params.board_pos);
      vec3_copy(&e->world_pos, &command->params.entity_move.new_params.world_pos);
      vec3_copy(&e->world_target, &command->params.entity_move.new_params.world_target);
      e->entity_state = command->params.entity_move.new_params.entity_state;
      break;
    case CommandExecute_Kill:
      break;
    }
    break;

#ifdef RONA_EDITOR
    // not expecting to offer a playback mode for editor commands (why would there be a demo mode
    // just for the editor?) so only deal with undo and redo command executions
    /*
     * Editor TileChange
     */
  case CommandType_Editor_TileChange: {
    Level*                   level       = game_state->level;
    CommandParamsTileChange* tile_change = &(command->params.tile_change);

    bool  requires_regen = false;
    Tile* tile           = tile_ensure_get(level, tile_change->tile_pos);

    switch (execute_type) {
    case CommandExecute_Play:
      return; // early return, no need to do anything here
    case CommandExecute_Undo:
      requires_regen = true;
      *tile          = tile_change->tile_old;
      break;
    case CommandExecute_Redo:
      requires_regen = true;
      *tile          = tile_change->tile_new;
      break;
    case CommandExecute_Kill:
      rona_log("CommandType_Editor_TileChange :: CommandExecute_Kill");
      break;
    }

    if (requires_regen) {
      // update the level's chunk geometry now that it's been changed
      RonaGL*       gl            = game_state->gl;
      RenderStruct* render_struct = &(game_state->render_struct);
      tile_regenerate_geometry(level, gl, render_struct);
    }
  } break;
  case CommandType_Editor_TileArea: {
    Level*                 level     = game_state->level;
    CommandParamsTileArea* tile_area = &(command->params.tile_area);
    Vec2i                  world_tl  = tile_area->tile_world_pos_top_left;
    Vec2i                  world_br  = tile_area->tile_world_pos_bottom_right;

    bool requires_regen = false;

    switch (execute_type) {
    case CommandExecute_Play:
      return; // early return, no need to do anything here
    case CommandExecute_Undo:
      requires_regen = true;
      // set all of the tiles to the old values
      //
      i32 index = 0;
      for (i32 y = world_tl.y; y <= world_br.y; y++) {
        for (i32 x = world_tl.x; x <= world_br.x; x++) {
          TilePos tile_pos     = tile_pos_from_world_tile_space(vec2i(x, y));
          Tile*   tile_current = tile_ensure_get(level, tile_pos);

          *tile_current = tile_area->tile_old_sb[index++];
        }
      }
      break;
    case CommandExecute_Redo:
      requires_regen = true;

      // set all of the tiles to the new value
      //
      for (i32 y = world_tl.y; y <= world_br.y; y++) {
        for (i32 x = world_tl.x; x <= world_br.x; x++) {
          TilePos tile_pos     = tile_pos_from_world_tile_space(vec2i(x, y));
          Tile*   tile_current = tile_ensure_get(level, tile_pos);

          *tile_current = tile_area->tile_new;
        }
      }
      break;
    case CommandExecute_Kill:
      rona_log("CommandType_Editor_TileArea :: CommandExecute_Kill");
      sb_free(&(editor_state.allocator_permanent), tile_area->tile_old_sb);
      break;
    }

    if (requires_regen) {
      // update the level's chunk geometry now that it's been changed
      RonaGL*       gl            = game_state->gl;
      RenderStruct* render_struct = &(game_state->render_struct);
      tile_regenerate_geometry(level, gl, render_struct);
    }
  } break;
#endif // RONA_EDITOR

  default:
    rona_error("command_execute: unknown command type %d", command->type);
    break;
  }
}

bool command_system_startup(UndoRedo* undo_redo, FixedBlockAllocator* fixed_block_allocator,
                            usize num_reserved_commands) {
  undo_redo->sb_commands = sb_add(fixed_block_allocator, undo_redo->sb_commands, num_reserved_commands);

  undo_redo->in_transaction          = false;
  undo_redo->command_index_next_free = 0;

  undo_redo->command_index_furthest_future = 0;

  return true;
}

void command_system_shutdown(UndoRedo* undo_redo) {
}

bool command_transaction_begin(UndoRedo* undo_redo) {
  if (undo_redo->in_transaction) {
    rona_error("command_transaction_begin: already in transaction");
    return false;
  }

  undo_redo->in_transaction = true;

  return true;
}

bool command_transaction_end(UndoRedo* undo_redo) {
  if (!(undo_redo->in_transaction)) {
    rona_error("command_transaction_end: not in transaction");
    return false;
  }

  undo_redo->in_transaction = false;

  if (undo_redo->command_index_next_free == 0) {
    // there are no commands, so just return
    return true;
  } else {
    undo_redo->sb_commands[undo_redo->command_index_next_free - 1].is_last_in_transaction = true;
    return true;
  }
}

// returns Command for caller to fill out, call within a transaction
Command* command_add(UndoRedo* undo_redo, FixedBlockAllocator* fixed_block_allocator, GameState* game_state) {
  RONA_ASSERT(fixed_block_allocator);
  RONA_ASSERT(undo_redo->in_transaction);

  // about to add a new command, so kill all the commands on the now invalid future timeline
  //

  // stay on the current command buffer
  if (undo_redo->command_index_furthest_future > undo_redo->command_index_next_free) {
    // weird, usize friendly loop style:
    usize i = undo_redo->command_index_furthest_future + 1;
    do {
      i--;
      command_execute(&(undo_redo->sb_commands[i]), CommandExecute_Kill, game_state);
    } while (i > undo_redo->command_index_next_free);
  } else {
    // already at the furthest future command, there are no commands that need to be killed
  }

  Command* command = NULL;
  if (undo_redo->command_index_next_free >= sb_count(undo_redo->sb_commands)) {
    // rona_log("sb_add (%d, %d)", undo_redo->command_index_next_free,
    // sb_count(undo_redo->sb_commands));
    command = sb_add(fixed_block_allocator, undo_redo->sb_commands, 1);
  } else {
    command = &(undo_redo->sb_commands[undo_redo->command_index_next_free]);
  }

  undo_redo->command_index_furthest_future = undo_redo->command_index_next_free;

  undo_redo->command_index_next_free++;

  command->is_last_in_transaction = false;

  return command;
}

// returns the command which was just undone
Command* command_pop(UndoRedo* undo_redo) {

  if (undo_redo->command_index_next_free == 0) {
    // already at the earliest point in the command buffer
    return NULL;
  } else {
    undo_redo->command_index_next_free -= 1;
  }

  Command* command = &(undo_redo->sb_commands[undo_redo->command_index_next_free]);

  return command;
}

Command* command_peek(UndoRedo* undo_redo) {
  usize command_index_next_free;

  if (undo_redo->command_index_next_free == 0) {
    // already at the earliest point in the command buffer
    return NULL;
  } else {
    command_index_next_free = undo_redo->command_index_next_free - 1;
  }

  Command* command = &(undo_redo->sb_commands[command_index_next_free]);

  return command;
}

Command* command_pop_up(UndoRedo* undo_redo) {
  Command* command;

  if (undo_redo->command_index_next_free == sb_count(undo_redo->sb_commands)) {
    rona_error("command_pop_up: cannot pop forward");
    return NULL;
  } else {
    if (undo_redo->command_index_next_free > undo_redo->command_index_furthest_future) {
      // can't go any further
      rona_error("command_pop_up: at the furthest future command");
      return NULL;
    }

    command = &(undo_redo->sb_commands[undo_redo->command_index_next_free]);
    undo_redo->command_index_next_free += 1;
  }

  return command;
}

bool command_undo(UndoRedo* undo_redo, GameState* game_state) {
  if (undo_redo->command_index_next_free == 0) {
    return false;
  }

  // always pop and undo the top command
  Command* command = command_pop(undo_redo);
  if (command == NULL) {
    return false;
  }

  // this latest command will always have is_last_in_transaction == true

  command_execute(command, CommandExecute_Undo, game_state);

  // now peek at the preceeding commands if they're not is_last_in_transaction
  // that means they're part of this transaction and should be undone as well
  //
  command = command_peek(undo_redo);
  if (command == NULL) {
    // peek at the command preceeding the popped one, will
    // be null if we started at command_index_next_free = 1
    //
    return false;
  }

  while (!command->is_last_in_transaction) {
    command = command_pop(undo_redo);
    command_execute(command, CommandExecute_Undo, game_state);

    // peek at the previous command in the undo stack
    command = command_peek(undo_redo);
    if (command == NULL) {
      rona_error("command_undo: command_peek returned null");
      return false;
    }
  }

  return true;
}

bool command_redo(UndoRedo* undo_redo, GameState* game_state) {
  if (undo_redo->command_index_furthest_future == (undo_redo->command_index_next_free - 1)) {
    // this is the furthest future point in this 'timeline'
    return false;
  }

  Command* command;

  do {
    command = command_pop_up(undo_redo);
    RONA_ASSERT(command);

    command_execute(command, CommandExecute_Redo, game_state);
  } while (!command->is_last_in_transaction);

  return true;
}

void command_pretty_print(Command* command, bool undo, const char* msg) {
  Entity* e = command->entity;
  switch (command->type) {
  case CommandType_EntityMove:
    // if (undo) {
    rona_log("%s old_params EntityMove (undo) e %p board (%d, %d), world (%.2f, %.2f, %.2f), state "
             "%d is_last_tnx %d\n",
             msg, e, command->params.entity_move.old_params.board_pos.x,
             command->params.entity_move.old_params.board_pos.y,
             command->params.entity_move.old_params.world_target.x,
             command->params.entity_move.old_params.world_target.y,
             command->params.entity_move.old_params.world_target.z, e->entity_state,
             command->is_last_in_transaction);
    //    } else {
    rona_log("%s new EntityMove e %p board (%d, %d), world (%.2f, %.2f, %.2f), state %d "
             "is_last_tnx %d\n",
             msg, e, command->params.entity_move.new_params.board_pos.x,
             command->params.entity_move.new_params.board_pos.y,
             command->params.entity_move.new_params.world_target.x,
             command->params.entity_move.new_params.world_target.y,
             command->params.entity_move.new_params.world_target.z, e->entity_state,
             command->is_last_in_transaction);
    //    }
    break;
  default:
    rona_error("command_pretty_print: unknown command type %d", command->type);
    break;
  }
}

void command_debug(UndoRedo* undo_redo) {
  rona_log("");
  rona_log("undo_redo.in_transaction %d", undo_redo->in_transaction);
  rona_log("undo_redo.command_index_next_free %d", undo_redo->command_index_next_free);
  rona_log("undo_redo.command_index_furthest_future %d", undo_redo->command_index_furthest_future);

  for (int i = 0; i <= undo_redo->command_index_furthest_future; i++) {
    rona_log("command %d:", i);
    command_pretty_print(&(undo_redo->sb_commands[i]), true, "    ");
  }
}
