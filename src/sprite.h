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

#ifndef SPRITE_H
#define SPRITE_H

// image_dim and sprite_dim should be valid
void tileset_calc_uv_units(Tileset* tileset);
Vec2 tileset_get_uv(Tileset* tileset, SpriteInfo* sprite_info, Sprite sprite);
void tileset_add_char(RenderStruct* render_struct, char c, Vec2* pos, Vec4* fg, Vec4* bg);

Dim2*                        sprite_build_char_locations(BumpAllocator* permanent);
SpriteInfo*                  sprite_build_sprite_info(BumpAllocator* permanent);
AnimatedCharacterSpriteInfo* sprite_build_animated_character_sprite_info(BumpAllocator* permanent);

#endif /* SPRITE_H  */
