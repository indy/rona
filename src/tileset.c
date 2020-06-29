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

Dim2 get_sprite_location(TilesetSprite sprite);

void tileset_calc_uv_units(Tileset *tileset) {
  tileset->uv_unit.u = 1.0f / (f32)(tileset->image_dim.width / tileset->sprite_dim.width);
  tileset->uv_unit.v = 1.0f / (f32)(tileset->image_dim.height / tileset->sprite_dim.height);
}

Vec2 tileset_get_uv(Tileset *tileset, TilesetSprite sprite) {
  Vec2 res;
  Dim2 sprite_location = get_sprite_location(sprite);

  res.u = sprite_location.col * tileset->uv_unit.u;
  res.v = sprite_location.row * tileset->uv_unit.v;

  return res;
}

Dim2 get_sprite_location(TilesetSprite sprite) {
  Dim2 res;
  // clang-format off
  switch(sprite) {
  case TS_Block: res.row = 6; res.col = 9; return res;

  case TS_Hero: res.row = 0; res.col = 0; return res;
  case TS_Archer: res.row = 0; res.col = 1; return res;
  case TS_Warrior: res.row = 0; res.col = 2; return res;
  case TS_Cleric: res.row = 0; res.col = 3; return res;
  case TS_Rogue: res.row = 0; res.col = 4; return res;
  case TS_Sorcerer: res.row = 0; res.col = 5; return res;
  case TS_Mage: res.row = 0; res.col = 6; return res;
  case TS_King: res.row = 0; res.col = 7; return res;
  case TS_Boy: res.row = 0; res.col = 8; return res;
  case TS_Dog: res.row = 0; res.col = 9; return res;
  case TS_Toddler: res.row = 0; res.col = 10; return res;
  case TS_Fatman: res.row = 0; res.col = 11; return res;
  case TS_Ghost: res.row = 0; res.col = 12; return res;

  case TS_DebugChecker: res.row = 0; res.col = 13; return res;
  case TS_DebugBlank: res.row = 0; res.col = 14; return res;
  case TS_DebugFilled: res.row = 0; res.col = 15; return res;
  case TS_Debug4Corners: res.row = 0; res.col = 16; return res;

  case TS_BorderTop: res.row = 0; res.col = 17; return res;
  case TS_BorderTopRight: res.row = 0; res.col = 18; return res;
  case TS_BorderRight: res.row = 0; res.col = 19; return res;
  case TS_BorderBottomRight: res.row = 0; res.col = 20; return res;
  case TS_BorderBottom: res.row = 0; res.col = 21; return res;
  case TS_BorderLeft: res.row = 0; res.col = 22; return res;
  case TS_BorderBottomLeft: res.row = 0; res.col = 23; return res;
  case TS_BorderTopLeft: res.row = 0; res.col = 24; return res;

  case TS_Heroine: res.row = 1; res.col = 0; return res;
  case TS_FemaleArcher: res.row = 1; res.col = 1; return res;
  case TS_FemaleWarrior: res.row = 1; res.col = 2; return res;
  case TS_FemaleCleric: res.row = 1; res.col = 3; return res;
  case TS_FemaleRogue: res.row = 1; res.col = 4; return res;
  case TS_Sorceress: res.row = 1; res.col = 5; return res;
  case TS_FemaleMage: res.row = 1; res.col = 6; return res;
  case TS_Queen: res.row = 1; res.col = 7; return res;
  case TS_Girl: res.row = 1; res.col = 8; return res;
  case TS_Cat: res.row = 1; res.col = 9; return res;
  case TS_FemaleToddler: res.row = 1; res.col = 10; return res;
  case TS_Fatwoman: res.row = 1; res.col = 11; return res;
  case TS_FemaleGhost: res.row = 1; res.col = 12; return res;

  case TS_Floor: res.row = 1; res.col = 13; return res;

  case TS_OldMan: res.row = 2; res.col = 0; return res;
  case TS_Patches: res.row = 2; res.col = 1; return res;
  case TS_Hopolite: res.row = 2; res.col = 2; return res;
  case TS_MagicThing1: res.row = 2; res.col = 3; return res;
  case TS_MagicOldMan: res.row = 2; res.col = 4; return res;
  case TS_MagicThing2: res.row = 2; res.col = 5; return res;
  case TS_Mermaid: res.row = 2; res.col = 6; return res;

  case TS_BlockSub0: res.row = 2; res.col = 13; return res;
  case TS_BlockSub1: res.row = 2; res.col = 14; return res;
  case TS_BlockSub2: res.row = 2; res.col = 15; return res;
  case TS_BlockPat1: res.row = 2; res.col = 16; return res;
  case TS_BlockPat2: res.row = 2; res.col = 17; return res;

  case TS_FaceNeutral: res.row = 3; res.col = 0; return res;
  case TS_FaceSmile: res.row = 3; res.col = 1; return res;
  case TS_FaceJoy: res.row = 3; res.col = 2; return res;
  case TS_FaceSad: res.row = 3; res.col = 3; return res;
  case TS_FaceCry: res.row = 3; res.col = 4; return res;
  case TS_FaceAngry: res.row = 3; res.col = 5; return res;
  case TS_FaceShout: res.row = 3; res.col = 6; return res;
  case TS_FaceTaste: res.row = 3; res.col = 7; return res;
  case TS_FaceEat: res.row = 3; res.col = 8; return res;
  case TS_FaceSleep: res.row = 3; res.col = 9; return res;
  case TS_FacePatches: res.row = 3; res.col = 10; return res;
  case TS_FaceGlasses: res.row = 3; res.col = 11; return res;

  case TS_Rat: res.row = 4; res.col = 0; return res;
  case TS_Bat: res.row = 4; res.col = 1; return res;
  case TS_Caterpillar: res.row = 4; res.col = 2; return res;
  case TS_Spider: res.row = 4; res.col = 3; return res;
  case TS_Scorpion: res.row = 4; res.col = 4; return res;
  case TS_Snail: res.row = 4; res.col = 5; return res;
  case TS_Snake: res.row = 4; res.col = 6; return res;
  case TS_Dingo: res.row = 4; res.col = 7; return res;
  case TS_Panther: res.row = 4; res.col = 8; return res;
  case TS_Bear: res.row = 4; res.col = 9; return res;
  case TS_Drake: res.row = 4; res.col = 10; return res;
  case TS_Dragon: res.row = 4; res.col = 11; return res;
  case TS_Unicorn: res.row = 4; res.col = 12; return res;

  case TS_Gremlin: res.row = 5; res.col = 0; return res;
  case TS_Ogre: res.row = 5; res.col = 1; return res;
  case TS_Troll: res.row = 5; res.col = 2; return res;
  case TS_Minotaur: res.row = 5; res.col = 3; return res;

  case TS_Ghoul: res.row = 6; res.col = 0; return res;
  case TS_PhantomHand: res.row = 6; res.col = 1; return res;
  case TS_Skeleton: res.row = 6; res.col = 2; return res;
  case TS_SkeletonWarrior: res.row = 6; res.col = 3; return res;
  case TS_SkeletonDeath: res.row = 6; res.col = 4; return res;
  case TS_EnemyGhost: res.row = 6; res.col = 5; return res;
  case TS_GhostSpirit: res.row = 6; res.col = 6; return res;
  case TS_Phantom: res.row = 6; res.col = 7; return res;

  case TS_JellyFish: res.row = 7; res.col = 0; return res;
  case TS_Octopus: res.row = 7; res.col = 1; return res;
  case TS_Eyeball: res.row = 7; res.col = 2; return res;
  case TS_MidgetThief: res.row = 7; res.col = 3; return res;
  case TS_MidgetSprite: res.row = 7; res.col = 4; return res;
  case TS_WalkingChest: res.row = 7; res.col = 5; return res;
  case TS_Robot: res.row = 7; res.col = 6; return res;
  case TS_RobotBrute: res.row = 7; res.col = 7; return res;

  case TS_Brick1: res.row = 8; res.col = 0; return res;
  case TS_Brick2: res.row = 8; res.col = 1; return res;
  case TS_Brick3: res.row = 8; res.col = 2; return res;
  case TS_Ground1: res.row = 8; res.col = 3; return res;
  case TS_Ground2: res.row = 8; res.col = 4; return res;
  case TS_Ground3: res.row = 8; res.col = 5; return res;
  case TS_PressurePadPrimed: res.row = 8; res.col = 6; return res;
  case TS_PressurePadActivated: res.row = 8; res.col = 7; return res;
  case TS_PointyGround: res.row = 8; res.col = 8; return res;
  case TS_FloorSwitch: res.row = 8; res.col = 9; return res;
  case TS_StairsDown: res.row = 8; res.col = 10; return res;
  case TS_StairsUp: res.row = 8; res.col = 11; return res;
  case TS_DoorClosed: res.row = 8; res.col = 12; return res;
  case TS_DoorOpen: res.row = 8; res.col = 13; return res;
  case TS_FlamingTurd: res.row = 8; res.col = 14; return res;
  case TS_SquashedTurd: res.row = 8; res.col = 15; return res;

  case TS_Gravestone: res.row = 9; res.col = 0; return res;
  case TS_Sign: res.row = 9; res.col = 1; return res;
  case TS_Stool: res.row = 9; res.col = 2; return res;
  case TS_Cupboard: res.row = 9; res.col = 3; return res;
  case TS_Bookshelf: res.row = 9; res.col = 4; return res;
  case TS_Table: res.row = 9; res.col = 5; return res;
  case TS_Bed: res.row = 9; res.col = 6; return res;

  case TS_WallHorizontal: res.row = 10; res.col = 0; return res;
  case TS_WallVertical: res.row = 10; res.col = 1; return res;
  case TS_WallHorizontalPoint: res.row = 10; res.col = 2; return res;
  case TS_WallTLCorner: res.row = 10; res.col = 3; return res;
  case TS_WallTRCorner: res.row = 10; res.col = 4; return res;
  case TS_WallBLCorner: res.row = 10; res.col = 5; return res;
  case TS_WallBRCorner: res.row = 10; res.col = 6; return res;
  case TS_WallTeeLeft: res.row = 10; res.col = 7; return res;
  case TS_WallTeeRight: res.row = 10; res.col = 8; return res;
  case TS_WallTeeDown: res.row = 10; res.col = 9; return res;
  case TS_WallTeeUp: res.row = 10; res.col = 10; return res;
  case TS_WallCross: res.row = 10; res.col = 11; return res;

  case TS_WeakFloor: res.row = 11; res.col = 0; return res;
  case TS_SpiderWeb: res.row = 11; res.col = 1; return res;
  case TS_TrapPrimed: res.row = 11; res.col = 2; return res;
  case TS_TrapActivated: res.row = 11; res.col = 3; return res;
  case TS_SpikesOnFloor: res.row = 11; res.col = 4; return res;
  case TS_SpikesInFloor: res.row = 11; res.col = 5; return res;
  case TS_Grate: res.row = 11; res.col = 6; return res;
  case TS_Wat: res.row = 11; res.col = 7; return res;
  case TS_FloorSwitchPrimed: res.row = 11; res.col = 8; return res;
  case TS_FloorSwitchActivated: res.row = 11; res.col = 9; return res;
  case TS_LeverLeft: res.row = 11; res.col = 10; return res;
  case TS_LeverRight: res.row = 11; res.col = 11; return res;

  case TS_MapPlain1: res.row = 12; res.col = 0; return res;
  case TS_MapPlain2: res.row = 12; res.col = 1; return res;
  case TS_MapPlain3: res.row = 12; res.col = 2; return res;
  case TS_MapTree1: res.row = 12; res.col = 3; return res;
  case TS_MapTree2: res.row = 12; res.col = 4; return res;
  case TS_MapTree3: res.row = 12; res.col = 5; return res;
  case TS_MapWater: res.row = 12; res.col = 6; return res;
  case TS_MapHouses: res.row = 12; res.col = 7; return res;
  case TS_MapCave: res.row = 12; res.col = 8; return res;
  case TS_MapHouse: res.row = 12; res.col = 9; return res;
  case TS_MapCastle: res.row = 12; res.col = 10; return res;
  case TS_MapCaravan: res.row = 12; res.col = 11; return res;
  case TS_MapShip: res.row = 12; res.col = 12; return res;

  case TS_Torch: res.row = 13; res.col = 0; return res;
  case TS_Lantern: res.row = 13; res.col = 1; return res;
  case TS_Spade: res.row = 13; res.col = 2; return res;
  case TS_Pickaxe: res.row = 13; res.col = 3; return res;
  case TS_Rope: res.row = 13; res.col = 4; return res;
  case TS_Bomb: res.row = 13; res.col = 5; return res;
  case TS_ChestClosed: res.row = 13; res.col = 6; return res;
  case TS_ChestOpen: res.row = 13; res.col = 7; return res;
  case TS_Vase: res.row = 13; res.col = 8; return res;
  case TS_VaseBroken: res.row = 13; res.col = 9; return res;
  case TS_DoorKey: res.row = 13; res.col = 10; return res;
  case TS_ChestKey: res.row = 13; res.col = 11; return res;
  case TS_Pouch: res.row = 13; res.col = 12; return res;
  case TS_Coins: res.row = 13; res.col = 13; return res;
  case TS_Biscuit: res.row = 13; res.col = 14; return res;
  case TS_Rupee: res.row = 13; res.col = 15; return res;

  case TS_Pearl: res.row = 14; res.col = 0; return res;
  case TS_SkullAndBones: res.row = 14; res.col = 1; return res;
  case TS_Bone: res.row = 14; res.col = 2; return res;
  case TS_Tongue: res.row = 14; res.col = 3; return res;
  case TS_MonsterSkull: res.row = 14; res.col = 4; return res;

  case TS_FoodDrumStick: res.row = 15; res.col = 0; return res;
  case TS_FoodSteak: res.row = 15; res.col = 1; return res;
  case TS_FoodKebab: res.row = 15; res.col = 2; return res;
  case TS_FoodFish: res.row = 15; res.col = 3; return res;
  case TS_FoodFriedEgg: res.row = 15; res.col = 4; return res;
  case TS_FoodBreadSlice: res.row = 15; res.col = 5; return res;
  case TS_FoodEgg: res.row = 15; res.col = 6; return res;
  case TS_FoodQuiche: res.row = 15; res.col = 7; return res;
  case TS_FoodApple: res.row = 15; res.col = 8; return res;
  case TS_FoodPumpkin: res.row = 15; res.col = 9; return res;
  case TS_FoodTurnip: res.row = 15; res.col = 10; return res;
  case TS_FoodBasil: res.row = 15; res.col = 11; return res;
  case TS_FoodFlower: res.row = 15; res.col = 12; return res;
  case TS_FoodMushroom: res.row = 15; res.col = 13; return res;
  case TS_FoodSweet: res.row = 15; res.col = 14; return res;
  case TS_FoodCake: res.row = 15; res.col = 15; return res;

  case TS_Beer: res.row = 16; res.col = 0; return res;
  case TS_Pot: res.row = 16; res.col = 1; return res;
  case TS_Jug: res.row = 16; res.col = 2; return res;
  case TS_SmallJar: res.row = 16; res.col = 3; return res;
  case TS_LargeJar: res.row = 16; res.col = 4; return res;
  case TS_ExpensiveJar: res.row = 16; res.col = 5; return res;

  case TS_WeaponDagger: res.row = 17; res.col = 0; return res;
  case TS_WeaponSword: res.row = 17; res.col = 1; return res;
  case TS_WeaponAxe: res.row = 17; res.col = 2; return res;
  case TS_WeaponPickaxe: res.row = 17; res.col = 3; return res;
  case TS_WeaponStaff: res.row = 17; res.col = 4; return res;
  case TS_WeaponClub: res.row = 17; res.col = 5; return res;
  case TS_WeaponMallet: res.row = 17; res.col = 6; return res;
  case TS_WeaponShield1: res.row = 17; res.col = 7; return res;
  case TS_WeaponShield2: res.row = 17; res.col = 8; return res;
  case TS_WeaponBow: res.row = 17; res.col = 9; return res;
  case TS_WeaponArrows: res.row = 17; res.col = 10; return res;
  case TS_ArmourHelmet1: res.row = 17; res.col = 11; return res;
  case TS_ArmourHelmet2: res.row = 17; res.col = 12; return res;
  case TS_WizardHat: res.row = 17; res.col = 13; return res;
  case TS_ArmourGloves: res.row = 17; res.col = 14; return res;
  case TS_ArmourBracelets: res.row = 17; res.col = 15; return res;

  case TS_ClothesBoots: res.row = 18; res.col = 0; return res;
  case TS_ClothesTrousers: res.row = 18; res.col = 1; return res;
  case TS_ClothesBelt: res.row = 18; res.col = 2; return res;
  case TS_ClothesTop1: res.row = 18; res.col = 3; return res;
  case TS_ClothesTop2: res.row = 18; res.col = 4; return res;
  case TS_ClothesTop3: res.row = 18; res.col = 5; return res;
  case TS_ClothesHood: res.row = 18; res.col = 6; return res;
  case TS_ClothesRing: res.row = 18; res.col = 7; return res;
  case TS_ClothesPendant: res.row = 18; res.col = 8; return res;
  case TS_ClothesCrown: res.row = 18; res.col = 9; return res;
  case TS_ClothesGlasses: res.row = 18; res.col = 10; return res;

  case TS_MagickWand: res.row = 19; res.col = 0; return res;
  case TS_MagickWandJeweled: res.row = 19; res.col = 1; return res;
  case TS_MagickWandSkulled: res.row = 19; res.col = 2; return res;
  case TS_MagickGlobe: res.row = 19; res.col = 3; return res;
  case TS_MagickPendant: res.row = 19; res.col = 4; return res;
  case TS_MagickSkull: res.row = 19; res.col = 5; return res;
  case TS_MagickScroll1: res.row = 19; res.col = 6; return res;
  case TS_MagickScroll2: res.row = 19; res.col = 7; return res;
  case TS_MagickBook: res.row = 19; res.col = 8; return res;
  case TS_MagickPedestal: res.row = 19; res.col = 9; return res;
  case TS_MagickCouldron: res.row = 19; res.col = 10; return res;
  case TS_MagickHex1: res.row = 19; res.col = 11; return res;
  case TS_MagickHex2: res.row = 19; res.col = 12; return res;
  case TS_MagickHexHand: res.row = 19; res.col = 13; return res;
  case TS_MagickWat: res.row = 19; res.col = 14; return res;

  case TS_MusicMandolin: res.row = 20; res.col = 0; return res;
  case TS_MusicHarp: res.row = 20; res.col = 1; return res;
  case TS_MusicPanFlute: res.row = 20; res.col = 2; return res;
  case TS_MusicBell: res.row = 20; res.col = 3; return res;
  case TS_MusicXylophone: res.row = 20; res.col = 4; return res;
  case TS_MusicDrum: res.row = 20; res.col = 5; return res;

  case TS_At: res.row = 21; res.col = 0; return res;
  case TS_DontUse1: res.row = 21; res.col = 1; return res;
  case TS_DontUse2: res.row = 21; res.col = 2; return res;
  case TS_Plus: res.row = 21; res.col = 3; return res;
  case TS_Cross: res.row = 21; res.col = 4; return res;
  case TS_Heart: res.row = 21; res.col = 5; return res;
  case TS_Star: res.row = 21; res.col = 6; return res;
  case TS_Sun: res.row = 21; res.col = 7; return res;
  case TS_Moon: res.row = 21; res.col = 8; return res;
  case TS_Bullseye: res.row = 21; res.col = 9; return res;
  case TS_Warning: res.row = 21; res.col = 10; return res;
  case TS_Music: res.row = 21; res.col = 11; return res;
  case TS_Fire: res.row = 21; res.col = 12; return res;
  case TS_Frost: res.row = 21; res.col = 13; return res;
  case TS_Rain: res.row = 21; res.col = 14; return res;
  case TS_Lightning: res.row = 21; res.col = 15; return res;

  case TS_Whirl: res.row = 22; res.col = 0; return res;
  case TS_Bubble: res.row = 22; res.col = 1; return res;
  case TS_Sleep: res.row = 22; res.col = 2; return res;
  case TS_Skull: res.row = 22; res.col = 3; return res;
  case TS_Arms: res.row = 22; res.col = 4; return res;
  case TS_Shield: res.row = 22; res.col = 5; return res;
  case TS_Hourglass: res.row = 22; res.col = 6; return res;
  case TS_North: res.row = 22; res.col = 7; return res;
  case TS_NorthEast: res.row = 22; res.col = 8; return res;
  case TS_East: res.row = 22; res.col = 9; return res;
  case TS_SouthEast: res.row = 22; res.col = 10; return res;
  case TS_South: res.row = 22; res.col = 11; return res;
  case TS_SouthWest: res.row = 22; res.col = 12; return res;
  case TS_West: res.row = 22; res.col = 13; return res;
  case TS_NorthWest: res.row = 22; res.col = 14; return res;

  default: res.row = 0; res.col = 0; return res;
  }
  // clang-format on
}
