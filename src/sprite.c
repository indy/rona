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

#define GENERATE_SPRITE_INFO(ENUM, TILE_X, TILE_Y, SO_X, SO_Y)                                               \
  [ENUM].location.row = TILE_X, [ENUM].location.col = TILE_Y, [ENUM].stage_offset.x = SO_X,                  \
  [ENUM].stage_offset.y = SO_Y,
static TilesetSpriteInfo g_sprite_info[] = {FOREACH_TILESETSPRITE(GENERATE_SPRITE_INFO)};

static Dim2 g_char_locations[] = {
    [0].row = 2,  [0].col = 0,   // space
    [1].row = 0,  [1].col = 61,  // !
    [2].row = 0,  [2].col = 66,  // "
    [3].row = 0,  [3].col = 54,  // #
    [4].row = 0,  [4].col = 58,  // $
    [5].row = 0,  [5].col = 55,  // %
    [6].row = 0,  [6].col = 56,  // &
    [7].row = 0,  [7].col = 65,  // '
    [8].row = 0,  [8].col = 67,  // (
    [9].row = 0,  [9].col = 68,  // )
    [10].row = 0, [10].col = 71, // *
    [11].row = 0, [11].col = 74, // +
    [12].row = 0, [12].col = 60, // ,
    [13].row = 0, [13].col = 75, // -
    [14].row = 0, [14].col = 59, // .
    [15].row = 0, [15].col = 72, // /

    [16].row = 0, [16].col = 0, // 0
    [17].row = 0, [17].col = 1, // 1
    [18].row = 0, [18].col = 2, // 2
    [19].row = 0, [19].col = 3, // 3
    [20].row = 0, [20].col = 4, // 4
    [21].row = 0, [21].col = 5, // 5
    [22].row = 0, [22].col = 6, // 6
    [23].row = 0, [23].col = 7, // 7
    [24].row = 0, [24].col = 8, // 8
    [25].row = 0, [25].col = 9, // 9

    [26].row = 0, [26].col = 63, // :
    [27].row = 0, [27].col = 64, // ;
    [28].row = 0, [28].col = 76, // <
    [29].row = 0, [29].col = 77, // =
    [30].row = 0, [30].col = 78, // >
    [31].row = 0, [31].col = 62, // ?
    [32].row = 0, [32].col = 65, // `

    [33].row = 0, [33].col = 10, // A
    [34].row = 0, [34].col = 11, // B
    [35].row = 0, [35].col = 12, // C
    [36].row = 0, [36].col = 13, // D
    [37].row = 0, [37].col = 14, // E
    [38].row = 0, [38].col = 15, // F
    [39].row = 0, [39].col = 16, // G
    [40].row = 0, [40].col = 17, // H
    [41].row = 0, [41].col = 18, // I
    [42].row = 0, [42].col = 19, // J
    [43].row = 0, [43].col = 20, // K
    [44].row = 0, [44].col = 21, // L
    [45].row = 0, [45].col = 22, // M
    [46].row = 0, [46].col = 23, // N
    [47].row = 0, [47].col = 24, // O
    [48].row = 0, [48].col = 25, // P
    [49].row = 0, [49].col = 26, // Q
    [50].row = 0, [50].col = 27, // R
    [51].row = 0, [51].col = 28, // S
    [52].row = 0, [52].col = 29, // T
    [53].row = 0, [53].col = 30, // U
    [54].row = 0, [54].col = 31, // V
    [55].row = 0, [55].col = 32, // W
    [56].row = 0, [56].col = 33, // X
    [57].row = 0, [57].col = 34, // Y
    [58].row = 0, [58].col = 35, // Z

    [59].row = 0, [59].col = 69, // [
    [60].row = 0, [60].col = 0,  // ~ todo: add this character
    [61].row = 0, [61].col = 70, // ]
    [62].row = 0, [62].col = 0,  // ^ todo: add this character
    [63].row = 0, [63].col = 0,  // _ todo: add this character
    [64].row = 0, [64].col = 57, // @

    [65].row = 1, [65].col = 0,  // a
    [66].row = 1, [66].col = 1,  // b
    [67].row = 1, [67].col = 2,  // c
    [68].row = 1, [68].col = 3,  // d
    [69].row = 1, [69].col = 4,  // e
    [70].row = 1, [70].col = 5,  // f
    [71].row = 1, [71].col = 6,  // g
    [72].row = 1, [72].col = 7,  // h
    [73].row = 1, [73].col = 8,  // i
    [74].row = 1, [74].col = 9,  // j
    [75].row = 1, [75].col = 10, // k
    [76].row = 1, [76].col = 11, // l
    [77].row = 1, [77].col = 12, // m
    [78].row = 1, [78].col = 13, // n
    [79].row = 1, [79].col = 14, // o
    [80].row = 1, [80].col = 15, // p
    [81].row = 1, [81].col = 16, // q
    [82].row = 1, [82].col = 17, // r
    [83].row = 1, [83].col = 18, // s
    [84].row = 1, [84].col = 19, // t
    [85].row = 1, [85].col = 20, // u
    [86].row = 1, [86].col = 21, // v
    [87].row = 1, [87].col = 22, // w
    [88].row = 1, [88].col = 23, // x
    [89].row = 1, [89].col = 24, // y
    [90].row = 1, [90].col = 25, // z

    [91].row = 0, [91].col = 67, // { using ( instead
    [92].row = 0, [92].col = 0,  // | todo: add this character
    [93].row = 0, [93].col = 68, // } using ) instead
    [94].row = 0, [94].col = 0   // | todo: add this character
};

void tileset_calc_uv_units(Tileset* tileset) {
  tileset->uv_unit.u = 1.0f / (f32)(tileset->image_dim.width / tileset->sprite_dim.width);
  tileset->uv_unit.v = 1.0f / (f32)(tileset->image_dim.height / tileset->sprite_dim.height);
}

Vec2 tileset_get_uv(Tileset* tileset, TilesetSprite sprite) {
  Vec2 res;
  Dim2 sprite_location = g_sprite_info[sprite].location;

  res.u = sprite_location.col * tileset->uv_unit.u;
  res.v = sprite_location.row * tileset->uv_unit.v;

  return res;
}

Vec2 tileset_get_uv_char(Tileset* tileset, char c) {
  usize index = (usize)(c - ' ');
  Vec2  res;
  Dim2  sprite_location = g_char_locations[index];

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

  Vec2 uv = tileset_get_uv_char(tileset, c);
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

Vec2 sprite_get_stage_offset(TilesetSprite sprite) {
  Vec2 stage_offset = g_sprite_info[sprite].stage_offset;

  return stage_offset;
}
