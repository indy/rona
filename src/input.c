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

bool key_down(RonaInput* input, RonaKey key) {
  return input->active && input->key[input->key_toggle_idx][key] == ButtonState_Down;
}
bool key_up(RonaInput* input, RonaKey key) {
  return input->active && input->key[input->key_toggle_idx][key] == ButtonState_Up;
}
bool key_pressed(RonaInput* input, RonaKey key) {
  return input->active && input->key[input->key_toggle_idx][key] == ButtonState_Down &&
         input->key[1 - input->key_toggle_idx][key] == ButtonState_Up;
}

bool key_pressed_ignore_active_flag(RonaInput* input, RonaKey key) {
  return input->key[input->key_toggle_idx][key] == ButtonState_Down &&
         input->key[1 - input->key_toggle_idx][key] == ButtonState_Up;
}

bool mouse_down(RonaInput* input, RonaMouseButton mouse_button) {
  return input->active && input->mouse[input->mouse_toggle_idx][mouse_button] == ButtonState_Down;
}
bool mouse_up(RonaInput* input, RonaMouseButton mouse_button) {
  return input->active && input->mouse[input->mouse_toggle_idx][mouse_button] == ButtonState_Up;
}
bool mouse_pressed(RonaInput* input, RonaMouseButton mouse_button) {
  return input->active && input->mouse[input->mouse_toggle_idx][mouse_button] == ButtonState_Down &&
         input->mouse[1 - input->mouse_toggle_idx][mouse_button] == ButtonState_Up;
}
