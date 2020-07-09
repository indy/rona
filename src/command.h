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

#ifndef COMMAND_H
#define COMMAND_H

bool command_buffer_startup(Level* level);
void command_buffer_shutdown(Level* level);

bool command_transaction_begin(Level* level);
bool command_transaction_end(Level* level);

// returns Command for caller to fill out, call within a transaction
Command* command_add(Level* level);

bool command_undo(Level* level);
bool command_redo(Level* level);

typedef enum { CommandExecute_Play, CommandExecute_Undo, CommandExecute_Redo } CommandExecute;

void command_execute(Command* command, CommandExecute execute_type);

void command_pretty_print(Command* command, bool undo, const char* msg);
void command_debug(Level* level);

#endif /* COMMAND_H */
