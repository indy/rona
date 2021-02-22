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
    bool requires_regen = false;
    Tile* tile = chunk_tile_ensure_get(command->data.editor_tile_change.level,
                                       command->data.editor_tile_change.chunk_pos);
    switch (execute_type) {
    case CommandExecute_Play:
      return; // early return, no need to do anything here
    case CommandExecute_Undo:
      requires_regen = true;
      *tile = command->data.editor_tile_change.tile_old;
      break;
    case CommandExecute_Redo:
      requires_regen = true;
      *tile = command->data.editor_tile_change.tile_new;
      break;
    case CommandExecute_Kill:
      RONA_LOG("CommandType_Editor_TileChange :: CommandExecute_Kill\n");
      break;
    }

    if (requires_regen) {
      // update the level's chunk geometry now that it's been changed
      Level*   level = game_state->level;
      RonaGL*  gl = game_state->gl;
      Tileset* tileset = &(game_state->render_struct.tileset);
      chunk_regenerate_geometry(level, gl, tileset);
    }
  } break;
#endif // RONA_EDITOR

  default:
    RONA_ERROR("command_execute: unknown command type %d\n", command->type);
    break;
  }
}

CommandBuffer* command_buffer_allocate(BumpAllocator* bump_allocator) {
  CommandBuffer* cb = (CommandBuffer*)BUMP_ALLOC(bump_allocator, sizeof(CommandBuffer));
  if (!cb) {
    RONA_ERROR("command_buffer_allocate\n");
    return NULL;
  }

  cb->command = (Command*)BUMP_ALLOC(bump_allocator, sizeof(Command) * MEMORY_RESERVE_COMMANDS_IN_BUFFER);
  if (!cb->command) {
    RONA_ERROR("command_buffer_allocate\n");
    return NULL;
  }
  cb->prev = NULL;
  cb->next = NULL;
  cb->size = MEMORY_RESERVE_COMMANDS_IN_BUFFER;
  cb->used = 0;

  return cb;
}

bool command_buffer_startup(BumpAllocator* bump_allocator, UndoRedo* undo_redo) {
  CommandBuffer* cb = command_buffer_allocate(bump_allocator);
  if (!cb) {
    RONA_ERROR("command_buffer_startup failed\n");
    return false;
  }

  undo_redo->command_buffer = cb;
  undo_redo->in_command_transaction = false;
  undo_redo->command_index_next_free = 0;

  undo_redo->command_index_furthest_future = 0;
  undo_redo->command_buffer_furthest_future = cb;

  return true;
}

void command_buffer_shutdown(UndoRedo* undo_redo) {
}

bool command_transaction_begin(UndoRedo* undo_redo) {
  if (undo_redo->in_command_transaction) {
    RONA_ERROR("command_transaction_begin: already in transaction\n");
    return false;
  }

  undo_redo->in_command_transaction = true;

  return true;
}

bool command_transaction_end(UndoRedo* undo_redo) {
  if (!(undo_redo->in_command_transaction)) {
    RONA_ERROR("command_transaction_end: not in transaction\n");
    return false;
  }

  undo_redo->in_command_transaction = false;

  if (undo_redo->command_index_next_free == 0 && undo_redo->command_buffer->prev == NULL) {
    // there are no commands, so just return
    return true;
  } else if (undo_redo->command_index_next_free == 0 && undo_redo->command_buffer->prev) {
    // have just added a command that has caused a new command buffer to be allocated
    // go back to the previous command buffer and set the last command to be the last one in the
    // transaction
    CommandBuffer* cb = undo_redo->command_buffer->prev;
    cb->command[cb->used - 1].is_last_in_transaction = true;
    return true;
  } else {
    CommandBuffer* cb = undo_redo->command_buffer;
    cb->command[undo_redo->command_index_next_free - 1].is_last_in_transaction = true;
    return true;
  }
}

// returns Command for caller to fill out, call within a transaction
Command* command_add(BumpAllocator* bump_allocator, UndoRedo* undo_redo, GameState* game_state) {
  RONA_ASSERT(bump_allocator);
  RONA_ASSERT(undo_redo->in_command_transaction);
  RONA_ASSERT(undo_redo->command_buffer);

  CommandBuffer* cb = undo_redo->command_buffer;

  // about to add a new command, so kill all the commands on the now invalid future timeline
  //
  if (undo_redo->command_buffer_furthest_future != cb) {
    // todo: there are commandbuffers from the future, call kill on all of their commands
  } else {
    // stay on the current command buffer
    if (undo_redo->command_index_furthest_future > undo_redo->command_index_next_free) {
      for (i32 i = undo_redo->command_index_furthest_future; i >= undo_redo->command_index_next_free; i--) {
        command_execute(&(cb->command[i]), CommandExecute_Kill, game_state);
      }
    } else {
      // already at the furthest future command, there are no commands that need to be killed
    }
  }

  if (undo_redo->command_index_next_free >= cb->size) {
    // allocate another CommandBuffer
    CommandBuffer* new_cb = command_buffer_allocate(bump_allocator);
    if (!new_cb) {
      RONA_ERROR("command_add failed to allocate new CommandBuffer\n");
      return NULL;
    }

    new_cb->prev = cb;
    cb->next = new_cb;
    undo_redo->command_buffer = new_cb;
    undo_redo->command_index_next_free = 0;
    cb = new_cb;
  }

  Command* command = &(cb->command[undo_redo->command_index_next_free]);

  undo_redo->command_index_furthest_future = undo_redo->command_index_next_free;
  undo_redo->command_buffer_furthest_future = cb;

  undo_redo->command_index_next_free++;
  if (undo_redo->command_index_next_free > cb->used) {
    cb->used = undo_redo->command_index_next_free;
  }

  command->is_last_in_transaction = false;

  return command;
}

// returns the command which was just undone
Command* command_pop(UndoRedo* undo_redo) {

  if (undo_redo->command_index_next_free == 0) {
    if (undo_redo->command_buffer->prev) {
      undo_redo->command_buffer = undo_redo->command_buffer->prev;
      undo_redo->command_index_next_free = undo_redo->command_buffer->used - 1;
    } else {
      // already at the earliest point in the command buffer
      return NULL;
    }
  } else {
    undo_redo->command_index_next_free -= 1;
  }

  Command* command = &(undo_redo->command_buffer->command[undo_redo->command_index_next_free]);

  return command;
}

Command* command_peek(UndoRedo* undo_redo) {
  usize          command_index_next_free;
  CommandBuffer* command_buffer;

  if (undo_redo->command_index_next_free == 0) {
    if (undo_redo->command_buffer->prev) {
      command_buffer = undo_redo->command_buffer->prev;
      command_index_next_free = command_buffer->used - 1;
    } else {
      // already at the earliest point in the command buffer
      return NULL;
    }
  } else {
    command_buffer = undo_redo->command_buffer;
    command_index_next_free = undo_redo->command_index_next_free - 1;
  }

  Command* command = &(command_buffer->command[command_index_next_free]);

  return command;
}

Command* command_pop_up(UndoRedo* undo_redo) {
  Command* command;

  if (undo_redo->command_index_next_free == undo_redo->command_buffer->size) {
    if (undo_redo->command_buffer->next) {

      undo_redo->command_buffer = undo_redo->command_buffer->next;
      undo_redo->command_index_next_free = 1;

      command = &(undo_redo->command_buffer->command[0]);
    } else {
      RONA_ERROR("command_pop_up: cannot pop forward\n");
      return NULL;
    }
  } else {

    if (undo_redo->command_index_next_free > undo_redo->command_index_furthest_future) {
      if (undo_redo->command_buffer->next == NULL) {
        // can't go any further
        RONA_ERROR("command_pop_up: at the furthest future command\n");
        return NULL;
      }
    }

    command = &(undo_redo->command_buffer->command[undo_redo->command_index_next_free]);
    undo_redo->command_index_next_free += 1;
  }

  return command;
}

bool command_undo(UndoRedo* undo_redo, GameState* game_state) {
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
    RONA_ERROR("command_undo: command_peek returned null\n");
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
  if (undo_redo->command_buffer_furthest_future == undo_redo->command_buffer &&
      undo_redo->command_index_furthest_future == (undo_redo->command_index_next_free - 1)) {
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
  RONA_LOG("undo_redo.in_command_transaction %d\n", undo_redo->in_command_transaction);
  RONA_LOG("undo_redo.command_index_next_free %d\n", undo_redo->command_index_next_free);
  RONA_LOG("undo_redo.command_index_furthest_future %d\n",
           undo_redo->command_index_furthest_future);

  for (int i = 0; i <= undo_redo->command_index_furthest_future; i++) {
    RONA_LOG("command %d:\n", i);
    command_pretty_print(&(undo_redo->command_buffer->command[i]), true, "    ");
  }
}
