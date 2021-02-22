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

#ifndef COMMAND_H
#define COMMAND_H

bool command_system_startup(UndoRedo* undo_redo, FixedBlockAllocator* fixed_block_allocator,
                            usize num_reserved_commands);
void command_system_shutdown(UndoRedo* undo_redo);

bool command_transaction_begin(UndoRedo* undo_redo);
bool command_transaction_end(UndoRedo* undo_redo);

// returns Command for caller to fill out, call within a transaction
// note: the GameState is only for passing through to command_execute during a CommandExecute_Kill.
// If no CommandExecute_Kill requires access to GameState this argument can be removed and I'll just
// pass in NULL to command_execute
Command* command_add(UndoRedo* undo_redo, FixedBlockAllocator* fixed_block_allocator, GameState* game_state);

bool command_undo(UndoRedo* undo_redo, GameState* game_state);
bool command_redo(UndoRedo* undo_redo, GameState* game_state);

// Play is here to enable autoplay demo mode in the future
// Kill: after some undos a new command is added so the old commands will need an opportunity to
// deallocate memory
//
typedef enum {
  CommandExecute_Play,
  CommandExecute_Undo,
  CommandExecute_Redo,
  CommandExecute_Kill
} CommandExecute;

void command_execute(Command* command, CommandExecute execute_type, GameState* game_state);

void command_pretty_print(Command* command, bool undo, const char* msg);
void command_debug(UndoRedo* undo_redo);

#endif /* COMMAND_H */
