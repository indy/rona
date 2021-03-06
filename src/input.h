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

#ifndef INPUT_H
#define INPUT_H

bool key_down(RonaInput* input, RonaKey key);
bool key_up(RonaInput* input, RonaKey key);
bool key_pressed(RonaInput* input, RonaKey key);
bool key_pressed_ignore_active_flag(RonaInput* input, RonaKey key);

bool mouse_down(RonaInput* input, RonaMouseButton mouse_button);
bool mouse_up(RonaInput* input, RonaMouseButton mouse_button);
bool mouse_pressed(RonaInput* input, RonaMouseButton mouse_button);
bool mouse_released(RonaInput* input, RonaMouseButton mouse_button);

#endif /* INPUT_H */
