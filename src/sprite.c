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

Dim2* sprite_build_char_locations(BumpAllocator* permanent) {
#define NUM_CHARACTERS_IN_TILESET 95

  usize size = sizeof(Dim2) * NUM_CHARACTERS_IN_TILESET;
  Dim2* dim2 = (Dim2*)BUMP_ALLOC(permanent, size);

  dim2[0].row  = 2;
  dim2[0].col  = 0; // space
  dim2[1].row  = 0;
  dim2[1].col  = 61; // !
  dim2[2].row  = 0;
  dim2[2].col  = 66; // "
  dim2[3].row  = 0;
  dim2[3].col  = 54; // #
  dim2[4].row  = 0;
  dim2[4].col  = 58; // $
  dim2[5].row  = 0;
  dim2[5].col  = 55; // %
  dim2[6].row  = 0;
  dim2[6].col  = 56; // &
  dim2[7].row  = 0;
  dim2[7].col  = 65; // '
  dim2[8].row  = 0;
  dim2[8].col  = 67; // (
  dim2[9].row  = 0;
  dim2[9].col  = 68; // )
  dim2[10].row = 0;
  dim2[10].col = 71; // *
  dim2[11].row = 0;
  dim2[11].col = 74; // +
  dim2[12].row = 0;
  dim2[12].col = 60; // ,
  dim2[13].row = 0;
  dim2[13].col = 75; // -
  dim2[14].row = 0;
  dim2[14].col = 59; // .
  dim2[15].row = 0;
  dim2[15].col = 72; // /

  dim2[16].row = 0;
  dim2[16].col = 0; // 0
  dim2[17].row = 0;
  dim2[17].col = 1; // 1
  dim2[18].row = 0;
  dim2[18].col = 2; // 2
  dim2[19].row = 0;
  dim2[19].col = 3; // 3
  dim2[20].row = 0;
  dim2[20].col = 4; // 4
  dim2[21].row = 0;
  dim2[21].col = 5; // 5
  dim2[22].row = 0;
  dim2[22].col = 6; // 6
  dim2[23].row = 0;
  dim2[23].col = 7; // 7
  dim2[24].row = 0;
  dim2[24].col = 8; // 8
  dim2[25].row = 0;
  dim2[25].col = 9; // 9

  dim2[26].row = 0;
  dim2[26].col = 63; // :
  dim2[27].row = 0;
  dim2[27].col = 64; // ;
  dim2[28].row = 0;
  dim2[28].col = 76; // <
  dim2[29].row = 0;
  dim2[29].col = 77; // =
  dim2[30].row = 0;
  dim2[30].col = 78; // >
  dim2[31].row = 0;
  dim2[31].col = 62; // ?
  dim2[32].row = 0;
  dim2[32].col = 65; // `

  dim2[33].row = 0;
  dim2[33].col = 10; // A
  dim2[34].row = 0;
  dim2[34].col = 11; // B
  dim2[35].row = 0;
  dim2[35].col = 12; // C
  dim2[36].row = 0;
  dim2[36].col = 13; // D
  dim2[37].row = 0;
  dim2[37].col = 14; // E
  dim2[38].row = 0;
  dim2[38].col = 15; // F
  dim2[39].row = 0;
  dim2[39].col = 16; // G
  dim2[40].row = 0;
  dim2[40].col = 17; // H
  dim2[41].row = 0;
  dim2[41].col = 18; // I
  dim2[42].row = 0;
  dim2[42].col = 19; // J
  dim2[43].row = 0;
  dim2[43].col = 20; // K
  dim2[44].row = 0;
  dim2[44].col = 21; // L
  dim2[45].row = 0;
  dim2[45].col = 22; // M
  dim2[46].row = 0;
  dim2[46].col = 23; // N
  dim2[47].row = 0;
  dim2[47].col = 24; // O
  dim2[48].row = 0;
  dim2[48].col = 25; // P
  dim2[49].row = 0;
  dim2[49].col = 26; // Q
  dim2[50].row = 0;
  dim2[50].col = 27; // R
  dim2[51].row = 0;
  dim2[51].col = 28; // S
  dim2[52].row = 0;
  dim2[52].col = 29; // T
  dim2[53].row = 0;
  dim2[53].col = 30; // U
  dim2[54].row = 0;
  dim2[54].col = 31; // V
  dim2[55].row = 0;
  dim2[55].col = 32; // W
  dim2[56].row = 0;
  dim2[56].col = 33; // X
  dim2[57].row = 0;
  dim2[57].col = 34; // Y
  dim2[58].row = 0;
  dim2[58].col = 35; // Z

  dim2[59].row = 0;
  dim2[59].col = 69; // dim2[
  dim2[60].row = 0;
  dim2[60].col = 0; // ~ todo: add this character
  dim2[61].row = 0;
  dim2[61].col = 70; // ]
  dim2[62].row = 0;
  dim2[62].col = 0; // ^ todo: add this character
  dim2[63].row = 0;
  dim2[63].col = 0; // _ todo: add this character
  dim2[64].row = 0;
  dim2[64].col = 57; // @

  dim2[65].row = 1;
  dim2[65].col = 0; // a
  dim2[66].row = 1;
  dim2[66].col = 1; // b
  dim2[67].row = 1;
  dim2[67].col = 2; // c
  dim2[68].row = 1;
  dim2[68].col = 3; // d
  dim2[69].row = 1;
  dim2[69].col = 4; // e
  dim2[70].row = 1;
  dim2[70].col = 5; // f
  dim2[71].row = 1;
  dim2[71].col = 6; // g
  dim2[72].row = 1;
  dim2[72].col = 7; // h
  dim2[73].row = 1;
  dim2[73].col = 8; // i
  dim2[74].row = 1;
  dim2[74].col = 9; // j
  dim2[75].row = 1;
  dim2[75].col = 10; // k
  dim2[76].row = 1;
  dim2[76].col = 11; // l
  dim2[77].row = 1;
  dim2[77].col = 12; // m
  dim2[78].row = 1;
  dim2[78].col = 13; // n
  dim2[79].row = 1;
  dim2[79].col = 14; // o
  dim2[80].row = 1;
  dim2[80].col = 15; // p
  dim2[81].row = 1;
  dim2[81].col = 16; // q
  dim2[82].row = 1;
  dim2[82].col = 17; // r
  dim2[83].row = 1;
  dim2[83].col = 18; // s
  dim2[84].row = 1;
  dim2[84].col = 19; // t
  dim2[85].row = 1;
  dim2[85].col = 20; // u
  dim2[86].row = 1;
  dim2[86].col = 21; // v
  dim2[87].row = 1;
  dim2[87].col = 22; // w
  dim2[88].row = 1;
  dim2[88].col = 23; // x
  dim2[89].row = 1;
  dim2[89].col = 24; // y
  dim2[90].row = 1;
  dim2[90].col = 25; // z

  dim2[91].row = 0;
  dim2[91].col = 67; // { using ( instead
  dim2[92].row = 0;
  dim2[92].col = 0; // | todo: add this character
  dim2[93].row = 0;
  dim2[93].col = 68; // } using ) instead
  dim2[94].row = 0;
  dim2[94].col = 0; // | todo: add this character

  return dim2;
}

SpriteInfo* sprite_build_sprite_info(BumpAllocator* permanent) {
  usize       size = sizeof(SpriteInfo) * S_NumSprites;
  SpriteInfo* si   = (SpriteInfo*)BUMP_ALLOC(permanent, size);

#define GENERATE_SPRITE_INFO(ENUM, TILE_X, TILE_Y, SO_X, SO_Y)                                               \
  [ENUM].location.row = TILE_X, [ENUM].location.col = TILE_Y, [ENUM].stage_offset.x = SO_X,                  \
  [ENUM].stage_offset.y = SO_Y,

  SpriteInfo temp_sprite_info[] = {FOREACH_SPRITE(GENERATE_SPRITE_INFO)};
  for (i32 i = 0; i < S_NumSprites; i++) {
    si[i] = temp_sprite_info[i];
  }

  return si;
}

AnimatedCharacterSpriteInfo* sprite_build_animated_character_sprite_info(BumpAllocator* permanent) {
  // grab relevent info declared in the ANIMATED_CHARACTER_SPRITE_DECL
  //
  typedef struct {
    usize row;
    bool  has_special;
  } DeclaredInfo;

#define GENERATE_ACS_DECLARED_INFO(ENUM, ROW, HAS_SPECIAL) [ENUM].row = ROW, [ENUM].has_special = HAS_SPECIAL,
  DeclaredInfo declared_info[] = {FOREACH_ANIMATED_CHARACTER_SPRITE(GENERATE_ACS_DECLARED_INFO)};

  // allocate permanent memory for AnimatedCharacterSpriteInfo and populate it with date from declared_info
  //
  usize                        size = sizeof(AnimatedCharacterSpriteInfo) * ACS_NumAnimatedCharacterSprites;
  AnimatedCharacterSpriteInfo* acsi = (AnimatedCharacterSpriteInfo*)BUMP_ALLOC(permanent, size);

  f32 playback_speed = 1.0f;
  for (i32 i = 0; i < ACS_NumAnimatedCharacterSprites; i++) {
    usize row                = declared_info[i].row;
    acsi[i].stage_offset     = vec2(0.0f, -4.0f);
    acsi[i].profile_location = dim2(0, row);

    acsi[i].idle.location       = dim2(1, row);
    acsi[i].idle.num_frames     = 4;
    acsi[i].idle.playback_speed = playback_speed;
    acsi[i].idle.loop           = true;

    acsi[i].walk.location       = dim2(5, row);
    acsi[i].walk.num_frames     = 4;
    acsi[i].walk.playback_speed = playback_speed;
    acsi[i].walk.loop           = true;

    acsi[i].attack.location       = dim2(9, row);
    acsi[i].attack.num_frames     = 4;
    acsi[i].attack.playback_speed = playback_speed;
    acsi[i].attack.loop           = false;

    acsi[i].hit.location       = dim2(13, row);
    acsi[i].hit.num_frames     = 4;
    acsi[i].hit.playback_speed = playback_speed;
    acsi[i].hit.loop           = false;

    acsi[i].death.location       = dim2(17, row);
    acsi[i].death.num_frames     = 4;
    acsi[i].death.playback_speed = playback_speed;
    acsi[i].death.loop           = false;

    acsi[i].has_special_animation = declared_info[i].has_special;

    acsi[i].special.location       = dim2(21, row);
    acsi[i].special.num_frames     = 4;
    acsi[i].special.playback_speed = playback_speed;
    acsi[i].special.loop           = false;
  }

  return acsi;
}

void tileset_calc_uv_units(Tileset* tileset) {
  tileset->uv_unit.u = 1.0f / (f32)(tileset->image_dim.width / tileset->sprite_dim.width);
  tileset->uv_unit.v = 1.0f / (f32)(tileset->image_dim.height / tileset->sprite_dim.height);
}

Vec2 tileset_get_uv(Tileset* tileset, SpriteInfo* sprite_info, Sprite sprite) {
  Vec2 res;
  Dim2 sprite_location = sprite_info[sprite].location;

  res.u = sprite_location.col * tileset->uv_unit.u;
  res.v = sprite_location.row * tileset->uv_unit.v;

  return res;
}

Vec2 tileset_get_uv_char(RenderStruct* render_struct, char c) {
  Tileset* tileset = &(render_struct->tileset);

  usize index = (usize)(c - ' ');
  Vec2  res;
  Dim2  sprite_location = render_struct->char_locations[index];

  res.u = sprite_location.col * (tileset->uv_unit.u / 2.0f);
  res.v = sprite_location.row * tileset->uv_unit.v;

  return res;
}

// pos is where the lower left corner of the character is placed
//
// clang-format off
void tileset_add_char(RenderStruct *render_struct, char c, Vec2 *pos, Vec4 *fg, Vec4 *bg) {
  Tileset *tileset = &(render_struct->tileset);

  // geometry

  f32 *e = &(render_struct->text_vertices[TILED_QUAD_NUM_FLOATS * render_struct->num_characters]);

  Vec2 uv = tileset_get_uv_char(render_struct, c);
  f32 u = uv.u;
  f32 v = uv.v;
  f32 ud = tileset->uv_unit.u / 2.0;
  f32 vd = tileset->uv_unit.v;
  // f32 half_dim_x = TILE_CHAR_WIDTH * 0.5f;
  // f32 half_dim_y = TILE_CHAR_HEIGHT * 0.5f;
  f32 xpos = pos->x;
  f32 ypos = pos->y;

  // RONA_LOG("x dim: %.4f -> %.4f\n", -half_dim_x + xpos, half_dim_x + xpos);
  // RONA_LOG("%.4f %.4f, %.4f %.4f\n", xpos, ypos, xpos + TILE_CHAR_WIDTH, ypos + TILE_CHAR_HEIGHT);

  *e++ = xpos;
  *e++ = ypos;

  *e++ = u;
  *e++ = v;

  *e++ = fg->e[0]; *e++ = fg->e[1]; *e++ = fg->e[2]; *e++ = fg->e[3];
  *e++ = bg->e[0]; *e++ = bg->e[1]; *e++ = bg->e[2]; *e++ = bg->e[3];

  *e++ = xpos;
  *e++ = ypos + TILE_CHAR_HEIGHT;

  *e++ = u;
  *e++ = v + vd;

  *e++ = fg->e[0]; *e++ = fg->e[1]; *e++ = fg->e[2]; *e++ = fg->e[3];
  *e++ = bg->e[0]; *e++ = bg->e[1]; *e++ = bg->e[2]; *e++ = bg->e[3];

  *e++ = xpos + TILE_CHAR_WIDTH;
  *e++ = ypos + TILE_CHAR_HEIGHT;

  *e++ = u + ud;
  *e++ = v + vd;

  *e++ = fg->e[0]; *e++ = fg->e[1]; *e++ = fg->e[2]; *e++ = fg->e[3];
  *e++ = bg->e[0]; *e++ = bg->e[1]; *e++ = bg->e[2]; *e++ = bg->e[3];

  *e++ = xpos + TILE_CHAR_WIDTH;
  *e++ = ypos;

  *e++ = u + ud;
  *e++ = v;

  *e++ = fg->e[0]; *e++ = fg->e[1]; *e++ = fg->e[2]; *e++ = fg->e[3];
  *e++ = bg->e[0]; *e++ = bg->e[1]; *e++ = bg->e[2]; *e++ = bg->e[3];

  // indices

  u32 *i = &(render_struct->text_indices[TILED_QUAD_NUM_INDICES * render_struct->num_characters]);
  i32 offset = render_struct->num_characters * 4;

  *i++ = offset + 0;
  *i++ = offset + 1;
  *i++ = offset + 2;
  *i++ = offset + 0;
  *i++ = offset + 2;
  *i++ = offset + 3;

  render_struct->num_characters++;
}
// clang-format on
