// Copyright (C) 2020 Inderjit Gill <email@indy.io>

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

CommandBuffer* command_buffer_allocate(BumpAllocator* bump) {
  CommandBuffer* cb = (CommandBuffer*)BUMP_ALLOC(bump, sizeof(CommandBuffer));
  if (!cb) {
    RONA_ERROR("command_buffer_allocate\n");
    return NULL;
  }

  cb->command = (Command*)BUMP_ALLOC(bump, sizeof(Command) * MEMORY_COMMANDS_IN_BUFFER);
  if (!cb->command) {
    RONA_ERROR("command_buffer_allocate\n");
    return NULL;
  }
  cb->prev = NULL;
  cb->next = NULL;
  cb->size = MEMORY_COMMANDS_IN_BUFFER;
  cb->used = 0;

  return cb;
}

bool command_buffer_startup(Level* level) {
  CommandBuffer* cb = command_buffer_allocate(&(level->mem));
  if (!cb) {
    RONA_ERROR("command_buffer_startup failed\n");
    return false;
  }

  level->command_buffer = cb;
  level->in_command_transaction = false;
  level->command_index_next_free = 0;

  level->command_index_furthest_future = 0;
  level->command_buffer_furthest_future = NULL;

  return true;
}

void command_buffer_shutdown(Level* level) {
}

bool command_transaction_begin(Level* level) {
  if (level->in_command_transaction) {
    RONA_ERROR("command_transaction_begin: already in transaction\n");
    return false;
  }

  level->in_command_transaction = true;

  return true;
}

bool command_transaction_end(Level* level) {
  if (!(level->in_command_transaction)) {
    RONA_ERROR("command_transaction_end: not in transaction\n");
    return false;
  }

  level->in_command_transaction = false;

  if (level->command_index_next_free == 0 && level->command_buffer->prev == NULL) {
    // there are no commands, so just return
    return true;
  } else if (level->command_index_next_free == 0 && level->command_buffer->prev) {
    // have just added a command that has caused a new command buffer to be allocated
    // go back to the previous command buffer and set the last command to be the last one in the
    // transaction
    CommandBuffer* cb = level->command_buffer->prev;
    cb->command[cb->used - 1].is_last_in_transaction = true;
    return true;
  } else {
    CommandBuffer* cb = level->command_buffer;
    cb->command[level->command_index_next_free - 1].is_last_in_transaction = true;
    return true;
  }
}

// returns Command for caller to fill out, call within a transaction
Command* command_add(Level* level) {
  RONA_ASSERT(level->in_command_transaction);
  RONA_ASSERT(level->command_buffer);

  CommandBuffer* cb = level->command_buffer;

  if (level->command_index_next_free >= cb->size) {
    // allocate another CommandBuffer
    CommandBuffer* new_cb = command_buffer_allocate(&(level->mem));
    if (!new_cb) {
      RONA_ERROR("command_add failed to allocate new CommandBuffer\n");
      return NULL;
    }

    new_cb->prev = cb;
    cb->next = new_cb;
    level->command_buffer = new_cb;
    level->command_index_next_free = 0;
    cb = new_cb;
  }

  Command* command = &(cb->command[level->command_index_next_free]);

  level->command_index_furthest_future = level->command_index_next_free;
  level->command_buffer_furthest_future = cb;

  level->command_index_next_free++;
  if (level->command_index_next_free > cb->used) {
    cb->used = level->command_index_next_free;
  }

  command->is_last_in_transaction = false;

  return command;
}

// returns the command which was just undone
Command* command_pop(Level* level) {

  if (level->command_index_next_free == 0) {
    if (level->command_buffer->prev) {
      level->command_buffer = level->command_buffer->prev;
      level->command_index_next_free = level->command_buffer->used - 1;
    } else {
      // already at the earliest point in the command buffer
      return NULL;
    }
  } else {
    level->command_index_next_free -= 1;
  }

  Command* command = &(level->command_buffer->command[level->command_index_next_free]);

  return command;
}

Command* command_peek(Level* level) {
  usize          command_index_next_free;
  CommandBuffer* command_buffer;

  if (level->command_index_next_free == 0) {
    if (level->command_buffer->prev) {
      command_buffer = level->command_buffer->prev;
      command_index_next_free = command_buffer->used - 1;
    } else {
      // already at the earliest point in the command buffer
      return NULL;
    }
  } else {
    command_buffer = level->command_buffer;
    command_index_next_free = level->command_index_next_free - 1;
  }

  Command* command = &(command_buffer->command[command_index_next_free]);

  return command;
}

Command* command_pop_up(Level* level) {
  Command* command;

  if (level->command_index_next_free == level->command_buffer->size) {
    if (level->command_buffer->next) {

      level->command_buffer = level->command_buffer->next;
      level->command_index_next_free = 1;

      command = &(level->command_buffer->command[0]);
    } else {
      RONA_ERROR("command_pop_up: cannot pop forward\n");
      return NULL;
    }
  } else {

    if (level->command_index_next_free > level->command_index_furthest_future) {
      if (level->command_buffer->next == NULL) {
        // can't go any further
        RONA_ERROR("command_pop_up: at the furthest future command\n");
        return NULL;
      }
    }

    command = &(level->command_buffer->command[level->command_index_next_free]);
    level->command_index_next_free += 1;
  }

  return command;
}

bool command_undo(Level* level) {
  // always pop and undo the top command
  Command* command = command_pop(level);
  if (command == NULL) {
    return false;
  }

  // this latest command will always have is_last_in_transaction == true

  command_execute(command, CommandExecute_Undo);

  // now peek at the preceeding commands if they're not is_last_in_transaction
  // that means they're part of this transaction and should be undone as well
  //
  command = command_peek(level);
  if (command == NULL) {
    RONA_ERROR("command_undo: command_peek returned null\n");
    return false;
  }

  while (!command->is_last_in_transaction) {
    command = command_pop(level);
    command_execute(command, CommandExecute_Undo);

    // peek at the previous command in the undo stack
    command = command_peek(level);
    if (command == NULL) {
      RONA_ERROR("command_undo: command_peek returned null\n");
      return false;
    }
  }

  return true;
}

bool command_redo(Level* level) {
  if (level->command_buffer_furthest_future == level->command_buffer &&
      level->command_index_furthest_future == (level->command_index_next_free - 1)) {
    // this is the furthest future point in this 'timeline'
    return false;
  }

  Command* command;

  do {
    command = command_pop_up(level);
    RONA_ASSERT(command);

    command_execute(command, CommandExecute_Redo);
  } while (!command->is_last_in_transaction);

  return true;
}

void command_execute(Command* command, CommandExecute execute_type) {
  Entity* e = command->entity;

  //  command_pretty_print(command, undo, "execute");

  switch (command->type) {
  case CommandType_EntityMove:
    switch (execute_type) {
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
    case CommandExecute_Play:
      vec2i_copy(&e->board_pos, &command->data.entity_move.new_params.board_pos);
      // world_pos will smoothly transition to world_target
      vec3_copy(&e->world_target, &command->data.entity_move.new_params.world_target);
      e->entity_state = command->data.entity_move.new_params.entity_state;
      break;
    }

    break;
  default:
    RONA_ERROR("command_execute: unknown command type %d\n", command->type);
    break;
  }
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
    RONA_ERROR("command_execute: unknown command type %d\n", command->type);
    break;
  }
}

void command_debug(Level* level) {
  RONA_LOG("\n");
  RONA_LOG("in_command_transaction %d\n", level->in_command_transaction);
  RONA_LOG("command_index_next_free %d\n", level->command_index_next_free);
  RONA_LOG("command_index_furthest_future %d\n", level->command_index_furthest_future);

  for (int i = 0; i <= level->command_index_furthest_future; i++) {
    RONA_LOG("command %d:\n", i);
    command_pretty_print(&(level->command_buffer->command[i]), true, "    ");
  }
}