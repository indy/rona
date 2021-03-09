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
Command* command_add(UndoRedo* undo_redo, FixedBlockAllocator* fixed_block_allocator, GameState* game_state);

// these are helper functions that will call command_add
//
Command* command_add_ingame(GameState* game_state, CommandType command_type, Entity* entity);

bool command_undo(UndoRedo* undo_redo, GameState* game_state);
bool command_redo(UndoRedo* undo_redo, GameState* game_state);

// Play_TurnBegin is called at the start of a turn, will enable smooth transitions
// Play_TurnEnd is called at the end of a Play command
// Redo is a way of snapping to the results of a Play_TurnBegin, ..animations.., Play_TurnEnd
// Undo: restore the previous state, opposite of redo
// Kill: after some undos a new command is added so the old commands will need an opportunity to
// deallocate memory
//
// note: should be able to call Undo/Redo repeatedly
// note: Redo should be equivalent to calling Play_TurnBegin + Play_TurnEnd
//
typedef enum {
  CommandExecute_Play_TurnBegin,
  CommandExecute_Play_TurnEnd,
  CommandExecute_Undo,
  CommandExecute_Redo,
  CommandExecute_Kill
} CommandExecute;

void command_execute(Command* command, CommandExecute execute_type, GameState* game_state);

void command_pretty_print(Command* command, bool undo, const char* msg);
void command_debug(UndoRedo* undo_redo);

#endif /* COMMAND_H */
