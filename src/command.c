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
      vec2i_copy(&e->board_pos, &command->data.entity_move.new_params.board_pos);
      // world_pos will smoothly transition to world_target
      vec3_copy(&e->world_target, &command->data.entity_move.new_params.world_target);
      e->entity_state = command->data.entity_move.new_params.entity_state;
      break;
    case CommandExecute_Undo:
      vec2i_copy(&e->board_pos, &command->data.entity_move.old_params.board_pos);
      vec3_copy(&e->world_pos, &command->data.entity_move.old_params.world_pos);
      vec3_copy(&e->world_target, &command->data.entity_move.old_params.world_target);
      e->entity_state = command->data.entity_move.old_params.entity_state;
      break;
    case CommandExecute_Redo:
      vec2i_copy(&e->board_pos, &command->data.entity_move.new_params.board_pos);
      vec3_copy(&e->world_pos, &command->data.entity_move.new_params.world_pos);
      vec3_copy(&e->world_target, &command->data.entity_move.new_params.world_target);
      e->entity_state = command->data.entity_move.new_params.entity_state;
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
    bool  requires_regen = false;
    Tile* tile           = chunk_tile_ensure_get(command->data.editor_tile_change.level,
                                       command->data.editor_tile_change.chunk_pos);
    switch (execute_type) {
    case CommandExecute_Play:
      return; // early return, no need to do anything here
    case CommandExecute_Undo:
      requires_regen = true;
      *tile          = command->data.editor_tile_change.tile_old;
      break;
    case CommandExecute_Redo:
      requires_regen = true;
      *tile          = command->data.editor_tile_change.tile_new;
      break;
    case CommandExecute_Kill:
      RONA_LOG("CommandType_Editor_TileChange :: CommandExecute_Kill\n");
      break;
    }

    if (requires_regen) {
      // update the level's chunk geometry now that it's been changed
      Level*        level         = game_state->level;
      RonaGL*       gl            = game_state->gl;
      RenderStruct* render_struct = &(game_state->render_struct);
      chunk_regenerate_geometry(level, gl, render_struct);
    }
  } break;
#endif // RONA_EDITOR

  default:
    RONA_ERROR("command_execute: unknown command type %d\n", command->type);
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
    RONA_ERROR("command_transaction_begin: already in transaction\n");
    return false;
  }

  undo_redo->in_transaction = true;

  return true;
}

bool command_transaction_end(UndoRedo* undo_redo) {
  if (!(undo_redo->in_transaction)) {
    RONA_ERROR("command_transaction_end: not in transaction\n");
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
    // RONA_LOG("sb_add (%d, %d)\n", undo_redo->command_index_next_free,
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
    RONA_ERROR("command_pop_up: cannot pop forward\n");
    return NULL;
  } else {
    if (undo_redo->command_index_next_free > undo_redo->command_index_furthest_future) {
      // can't go any further
      RONA_ERROR("command_pop_up: at the furthest future command\n");
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
      RONA_ERROR("command_undo: command_peek returned null\n");
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
    RONA_LOG("%s old_params EntityMove (undo) e %p board (%d, %d), world (%.2f, %.2f, %.2f), state "
             "%d is_last_tnx %d\n",
             msg, e, command->data.entity_move.old_params.board_pos.x,
             command->data.entity_move.old_params.board_pos.y,
             command->data.entity_move.old_params.world_target.x,
             command->data.entity_move.old_params.world_target.y,
             command->data.entity_move.old_params.world_target.z, e->entity_state,
             command->is_last_in_transaction);
    //    } else {
    RONA_LOG("%s new EntityMove e %p board (%d, %d), world (%.2f, %.2f, %.2f), state %d "
             "is_last_tnx %d\n",
             msg, e, command->data.entity_move.new_params.board_pos.x,
             command->data.entity_move.new_params.board_pos.y,
             command->data.entity_move.new_params.world_target.x,
             command->data.entity_move.new_params.world_target.y,
             command->data.entity_move.new_params.world_target.z, e->entity_state,
             command->is_last_in_transaction);
    //    }
    break;
  default:
    RONA_ERROR("command_pretty_print: unknown command type %d\n", command->type);
    break;
  }
}

void command_debug(UndoRedo* undo_redo) {
  RONA_LOG("\n");
  RONA_LOG("undo_redo.in_transaction %d\n", undo_redo->in_transaction);
  RONA_LOG("undo_redo.command_index_next_free %d\n", undo_redo->command_index_next_free);
  RONA_LOG("undo_redo.command_index_furthest_future %d\n", undo_redo->command_index_furthest_future);

  for (int i = 0; i <= undo_redo->command_index_furthest_future; i++) {
    RONA_LOG("command %d:\n", i);
    command_pretty_print(&(undo_redo->sb_commands[i]), true, "    ");
  }
}
