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

// clang-format off
static Dim2 g_sprite_locations[] = {
  [0].row = 6, [0].col = 9, // TS_Block
  [1].row = 0, [1].col = 0, // TS_Hero
  [2].row = 0, [2].col = 1, // TS_Archer
  [3].row = 0, [3].col = 2, // TS_Warrior
  [4].row = 0, [4].col = 3, // TS_Cleric
  [5].row = 0, [5].col = 4, // TS_Rogue
  [6].row = 0, [6].col = 5, // TS_Sorcerer
  [7].row = 0, [7].col = 6, // TS_Mage
  [8].row = 0, [8].col = 7, // TS_King
  [9].row = 0, [9].col = 8, // TS_Boy
  [10].row = 0, [10].col = 9, // TS_Dog
  [11].row = 0, [11].col = 10, // TS_Toddler
  [12].row = 0, [12].col = 11, // TS_Fatman
  [13].row = 0, [13].col = 12, // TS_Ghost
  [14].row = 0, [14].col = 13, // TS_DebugChecker
  [15].row = 0, [15].col = 14, // TS_DebugBlank
  [16].row = 0, [16].col = 15, // TS_DebugFilled
  [17].row = 0, [17].col = 16, // TS_Debug4Corners
  [18].row = 0, [18].col = 17, // TS_BorderTop
  [19].row = 0, [19].col = 18, // TS_BorderTopRight
  [20].row = 0, [20].col = 19, // TS_BorderRight
  [21].row = 0, [21].col = 20, // TS_BorderBottomRight
  [22].row = 0, [22].col = 21, // TS_BorderBottom
  [23].row = 0, [23].col = 22, // TS_BorderLeft
  [24].row = 0, [24].col = 23, // TS_BorderBottomLeft
  [25].row = 0, [25].col = 24, // TS_BorderTopLeft
  [26].row = 1, [26].col = 0, // TS_Heroine
  [27].row = 1, [27].col = 1, // TS_FemaleArcher
  [28].row = 1, [28].col = 2, // TS_FemaleWarrior
  [29].row = 1, [29].col = 3, // TS_FemaleCleric
  [30].row = 1, [30].col = 4, // TS_FemaleRogue
  [31].row = 1, [31].col = 5, // TS_Sorceress
  [32].row = 1, [32].col = 6, // TS_FemaleMage
  [33].row = 1, [33].col = 7, // TS_Queen
  [34].row = 1, [34].col = 8, // TS_Girl
  [35].row = 1, [35].col = 9, // TS_Cat
  [36].row = 1, [36].col = 10, // TS_FemaleToddler
  [37].row = 1, [37].col = 11, // TS_Fatwoman
  [38].row = 1, [38].col = 12, // TS_FemaleGhost
  [39].row = 1, [39].col = 13, // TS_Floor
  [40].row = 2, [40].col = 0, // TS_OldMan
  [41].row = 2, [41].col = 1, // TS_Patches
  [42].row = 2, [42].col = 2, // TS_Hopolite
  [43].row = 2, [43].col = 3, // TS_MagicThing1
  [44].row = 2, [44].col = 4, // TS_MagicOldMan
  [45].row = 2, [45].col = 5, // TS_MagicThing2
  [46].row = 2, [46].col = 6, // TS_Mermaid
  [47].row = 2, [47].col = 13, // TS_BlockSub0
  [48].row = 2, [48].col = 14, // TS_BlockSub1
  [49].row = 2, [49].col = 15, // TS_BlockSub2
  [50].row = 2, [50].col = 16, // TS_BlockPat1
  [51].row = 2, [51].col = 17, // TS_BlockPat2
  [52].row = 3, [52].col = 0, // TS_FaceNeutral
  [53].row = 3, [53].col = 1, // TS_FaceSmile
  [54].row = 3, [54].col = 2, // TS_FaceJoy
  [55].row = 3, [55].col = 3, // TS_FaceSad
  [56].row = 3, [56].col = 4, // TS_FaceCry
  [57].row = 3, [57].col = 5, // TS_FaceAngry
  [58].row = 3, [58].col = 6, // TS_FaceShout
  [59].row = 3, [59].col = 7, // TS_FaceTaste
  [60].row = 3, [60].col = 8, // TS_FaceEat
  [61].row = 3, [61].col = 9, // TS_FaceSleep
  [62].row = 3, [62].col = 10, // TS_FacePatches
  [63].row = 3, [63].col = 11, // TS_FaceGlasses
  [64].row = 4, [64].col = 0, // TS_Rat
  [65].row = 4, [65].col = 1, // TS_Bat
  [66].row = 4, [66].col = 2, // TS_Caterpillar
  [67].row = 4, [67].col = 3, // TS_Spider
  [68].row = 4, [68].col = 4, // TS_Scorpion
  [69].row = 4, [69].col = 5, // TS_Snail
  [70].row = 4, [70].col = 6, // TS_Snake
  [71].row = 4, [71].col = 7, // TS_Dingo
  [72].row = 4, [72].col = 8, // TS_Panther
  [73].row = 4, [73].col = 9, // TS_Bear
  [74].row = 4, [74].col = 10, // TS_Drake
  [75].row = 4, [75].col = 11, // TS_Dragon
  [76].row = 4, [76].col = 12, // TS_Unicorn
  [77].row = 5, [77].col = 0, // TS_Gremlin
  [78].row = 5, [78].col = 1, // TS_Ogre
  [79].row = 5, [79].col = 2, // TS_Troll
  [80].row = 5, [80].col = 3, // TS_Minotaur
  [81].row = 6, [81].col = 0, // TS_Ghoul
  [82].row = 6, [82].col = 1, // TS_PhantomHand
  [83].row = 6, [83].col = 2, // TS_Skeleton
  [84].row = 6, [84].col = 3, // TS_SkeletonWarrior
  [85].row = 6, [85].col = 4, // TS_SkeletonDeath
  [86].row = 6, [86].col = 5, // TS_EnemyGhost
  [87].row = 6, [87].col = 6, // TS_GhostSpirit
  [88].row = 6, [88].col = 7, // TS_Phantom
  [89].row = 7, [89].col = 0, // TS_JellyFish
  [90].row = 7, [90].col = 1, // TS_Octopus
  [91].row = 7, [91].col = 2, // TS_Eyeball
  [92].row = 7, [92].col = 3, // TS_MidgetThief
  [93].row = 7, [93].col = 4, // TS_MidgetSprite
  [94].row = 7, [94].col = 5, // TS_WalkingChest
  [95].row = 7, [95].col = 6, // TS_Robot
  [96].row = 7, [96].col = 7, // TS_RobotBrute
  [97].row = 8, [97].col = 0, // TS_Brick1
  [98].row = 8, [98].col = 1, // TS_Brick2
  [99].row = 8, [99].col = 2, // TS_Brick3
  [100].row = 8, [100].col = 3, // TS_Ground1
  [101].row = 8, [101].col = 4, // TS_Ground2
  [102].row = 8, [102].col = 5, // TS_Ground3
  [103].row = 8, [103].col = 6, // TS_PressurePadPrimed
  [104].row = 8, [104].col = 7, // TS_PressurePadActivated
  [105].row = 8, [105].col = 8, // TS_PointyGround
  [106].row = 8, [106].col = 9, // TS_FloorSwitch
  [107].row = 8, [107].col = 10, // TS_StairsDown
  [108].row = 8, [108].col = 11, // TS_StairsUp
  [109].row = 8, [109].col = 12, // TS_DoorClosed
  [110].row = 8, [110].col = 13, // TS_DoorOpen
  [111].row = 8, [111].col = 14, // TS_FlamingTurd
  [112].row = 8, [112].col = 15, // TS_SquashedTurd
  [113].row = 9, [113].col = 0, // TS_Gravestone
  [114].row = 9, [114].col = 1, // TS_Sign
  [115].row = 9, [115].col = 2, // TS_Stool
  [116].row = 9, [116].col = 3, // TS_Cupboard
  [117].row = 9, [117].col = 4, // TS_Bookshelf
  [118].row = 9, [118].col = 5, // TS_Table
  [119].row = 9, [119].col = 6, // TS_Bed
  [120].row = 10, [120].col = 0, // TS_WallHorizontal
  [121].row = 10, [121].col = 1, // TS_WallVertical
  [122].row = 10, [122].col = 2, // TS_WallHorizontalPoint
  [123].row = 10, [123].col = 3, // TS_WallTLCorner
  [124].row = 10, [124].col = 4, // TS_WallTRCorner
  [125].row = 10, [125].col = 5, // TS_WallBLCorner
  [126].row = 10, [126].col = 6, // TS_WallBRCorner
  [127].row = 10, [127].col = 7, // TS_WallTeeLeft
  [128].row = 10, [128].col = 8, // TS_WallTeeRight
  [129].row = 10, [129].col = 9, // TS_WallTeeDown
  [130].row = 10, [130].col = 10, // TS_WallTeeUp
  [131].row = 10, [131].col = 11, // TS_WallCross
  [132].row = 11, [132].col = 0, // TS_WeakFloor
  [133].row = 11, [133].col = 1, // TS_SpiderWeb
  [134].row = 11, [134].col = 2, // TS_TrapPrimed
  [135].row = 11, [135].col = 3, // TS_TrapActivated
  [136].row = 11, [136].col = 4, // TS_SpikesOnFloor
  [137].row = 11, [137].col = 5, // TS_SpikesInFloor
  [138].row = 11, [138].col = 6, // TS_Grate
  [139].row = 11, [139].col = 7, // TS_Wat
  [140].row = 11, [140].col = 8, // TS_FloorSwitchPrimed
  [141].row = 11, [141].col = 9, // TS_FloorSwitchActivated
  [142].row = 11, [142].col = 10, // TS_LeverLeft
  [143].row = 11, [143].col = 11, // TS_LeverRight
  [144].row = 12, [144].col = 0, // TS_MapPlain1
  [145].row = 12, [145].col = 1, // TS_MapPlain2
  [146].row = 12, [146].col = 2, // TS_MapPlain3
  [147].row = 12, [147].col = 3, // TS_MapTree1
  [148].row = 12, [148].col = 4, // TS_MapTree2
  [149].row = 12, [149].col = 5, // TS_MapTree3
  [150].row = 12, [150].col = 6, // TS_MapWater
  [151].row = 12, [151].col = 7, // TS_MapHouses
  [152].row = 12, [152].col = 8, // TS_MapCave
  [153].row = 12, [153].col = 9, // TS_MapHouse
  [154].row = 12, [154].col = 10, // TS_MapCastle
  [155].row = 12, [155].col = 11, // TS_MapCaravan
  [156].row = 12, [156].col = 12, // TS_MapShip
  [157].row = 13, [157].col = 0, // TS_Torch
  [158].row = 13, [158].col = 1, // TS_Lantern
  [159].row = 13, [159].col = 2, // TS_Spade
  [160].row = 13, [160].col = 3, // TS_Pickaxe
  [161].row = 13, [161].col = 4, // TS_Rope
  [162].row = 13, [162].col = 5, // TS_Bomb
  [163].row = 13, [163].col = 6, // TS_ChestClosed
  [164].row = 13, [164].col = 7, // TS_ChestOpen
  [165].row = 13, [165].col = 8, // TS_Vase
  [166].row = 13, [166].col = 9, // TS_VaseBroken
  [167].row = 13, [167].col = 10, // TS_DoorKey
  [168].row = 13, [168].col = 11, // TS_ChestKey
  [169].row = 13, [169].col = 12, // TS_Pouch
  [170].row = 13, [170].col = 13, // TS_Coins
  [171].row = 13, [171].col = 14, // TS_Biscuit
  [172].row = 13, [172].col = 15, // TS_Rupee
  [173].row = 14, [173].col = 0, // TS_Pearl
  [174].row = 14, [174].col = 1, // TS_SkullAndBones
  [175].row = 14, [175].col = 2, // TS_Bone
  [176].row = 14, [176].col = 3, // TS_Tongue
  [177].row = 14, [177].col = 4, // TS_MonsterSkull
  [178].row = 15, [178].col = 0, // TS_FoodDrumStick
  [179].row = 15, [179].col = 1, // TS_FoodSteak
  [180].row = 15, [180].col = 2, // TS_FoodKebab
  [181].row = 15, [181].col = 3, // TS_FoodFish
  [182].row = 15, [182].col = 4, // TS_FoodFriedEgg
  [183].row = 15, [183].col = 5, // TS_FoodBreadSlice
  [184].row = 15, [184].col = 6, // TS_FoodEgg
  [185].row = 15, [185].col = 7, // TS_FoodQuiche
  [186].row = 15, [186].col = 8, // TS_FoodApple
  [187].row = 15, [187].col = 9, // TS_FoodPumpkin
  [188].row = 15, [188].col = 10, // TS_FoodTurnip
  [189].row = 15, [189].col = 11, // TS_FoodBasil
  [190].row = 15, [190].col = 12, // TS_FoodFlower
  [191].row = 15, [191].col = 13, // TS_FoodMushroom
  [192].row = 15, [192].col = 14, // TS_FoodSweet
  [193].row = 15, [193].col = 15, // TS_FoodCake
  [194].row = 16, [194].col = 0, // TS_Beer
  [195].row = 16, [195].col = 1, // TS_Pot
  [196].row = 16, [196].col = 2, // TS_Jug
  [197].row = 16, [197].col = 3, // TS_SmallJar
  [198].row = 16, [198].col = 4, // TS_LargeJar
  [199].row = 16, [199].col = 5, // TS_ExpensiveJar
  [200].row = 17, [200].col = 0, // TS_WeaponDagger
  [201].row = 17, [201].col = 1, // TS_WeaponSword
  [202].row = 17, [202].col = 2, // TS_WeaponAxe
  [203].row = 17, [203].col = 3, // TS_WeaponPickaxe
  [204].row = 17, [204].col = 4, // TS_WeaponStaff
  [205].row = 17, [205].col = 5, // TS_WeaponClub
  [206].row = 17, [206].col = 6, // TS_WeaponMallet
  [207].row = 17, [207].col = 7, // TS_WeaponShield1
  [208].row = 17, [208].col = 8, // TS_WeaponShield2
  [209].row = 17, [209].col = 9, // TS_WeaponBow
  [210].row = 17, [210].col = 10, // TS_WeaponArrows
  [211].row = 17, [211].col = 11, // TS_ArmourHelmet1
  [212].row = 17, [212].col = 12, // TS_ArmourHelmet2
  [213].row = 17, [213].col = 13, // TS_WizardHat
  [214].row = 17, [214].col = 14, // TS_ArmourGloves
  [215].row = 17, [215].col = 15, // TS_ArmourBracelets
  [216].row = 18, [216].col = 0, // TS_ClothesBoots
  [217].row = 18, [217].col = 1, // TS_ClothesTrousers
  [218].row = 18, [218].col = 2, // TS_ClothesBelt
  [219].row = 18, [219].col = 3, // TS_ClothesTop1
  [220].row = 18, [220].col = 4, // TS_ClothesTop2
  [221].row = 18, [221].col = 5, // TS_ClothesTop3
  [222].row = 18, [222].col = 6, // TS_ClothesHood
  [223].row = 18, [223].col = 7, // TS_ClothesRing
  [224].row = 18, [224].col = 8, // TS_ClothesPendant
  [225].row = 18, [225].col = 9, // TS_ClothesCrown
  [226].row = 18, [226].col = 10, // TS_ClothesGlasses
  [227].row = 19, [227].col = 0, // TS_MagickWand
  [228].row = 19, [228].col = 1, // TS_MagickWandJeweled
  [229].row = 19, [229].col = 2, // TS_MagickWandSkulled
  [230].row = 19, [230].col = 3, // TS_MagickGlobe
  [231].row = 19, [231].col = 4, // TS_MagickPendant
  [232].row = 19, [232].col = 5, // TS_MagickSkull
  [233].row = 19, [233].col = 6, // TS_MagickScroll1
  [234].row = 19, [234].col = 7, // TS_MagickScroll2
  [235].row = 19, [235].col = 8, // TS_MagickBook
  [236].row = 19, [236].col = 9, // TS_MagickPedestal
  [237].row = 19, [237].col = 10, // TS_MagickCouldron
  [238].row = 19, [238].col = 11, // TS_MagickHex1
  [239].row = 19, [239].col = 12, // TS_MagickHex2
  [240].row = 19, [240].col = 13, // TS_MagickHexHand
  [241].row = 19, [241].col = 14, // TS_MagickWat
  [242].row = 20, [242].col = 0, // TS_MusicMandolin
  [243].row = 20, [243].col = 1, // TS_MusicHarp
  [244].row = 20, [244].col = 2, // TS_MusicPanFlute
  [245].row = 20, [245].col = 3, // TS_MusicBell
  [246].row = 20, [246].col = 4, // TS_MusicXylophone
  [247].row = 20, [247].col = 5, // TS_MusicDrum
  [248].row = 21, [248].col = 0, // TS_At
  [249].row = 21, [249].col = 1, // TS_DontUse1
  [250].row = 21, [250].col = 2, // TS_DontUse2
  [251].row = 21, [251].col = 3, // TS_Plus
  [252].row = 21, [252].col = 4, // TS_Cross
  [253].row = 21, [253].col = 5, // TS_Heart
  [254].row = 21, [254].col = 6, // TS_Star
  [255].row = 21, [255].col = 7, // TS_Sun
  [256].row = 21, [256].col = 8, // TS_Moon
  [257].row = 21, [257].col = 9, // TS_Bullseye
  [258].row = 21, [258].col = 10, // TS_Warning
  [259].row = 21, [259].col = 11, // TS_Music
  [260].row = 21, [260].col = 12, // TS_Fire
  [261].row = 21, [261].col = 13, // TS_Frost
  [262].row = 21, [262].col = 14, // TS_Rain
  [263].row = 21, [263].col = 15, // TS_Lightning
  [264].row = 22, [264].col = 0, // TS_Whirl
  [265].row = 22, [265].col = 1, // TS_Bubble
  [266].row = 22, [266].col = 2, // TS_Sleep
  [267].row = 22, [267].col = 3, // TS_Skull
  [268].row = 22, [268].col = 4, // TS_Arms
  [269].row = 22, [269].col = 5, // TS_Shield
  [270].row = 22, [270].col = 6, // TS_Hourglass
  [271].row = 22, [271].col = 7, // TS_North
  [272].row = 22, [272].col = 8, // TS_NorthEast
  [273].row = 22, [273].col = 9, // TS_East
  [274].row = 22, [274].col = 10, // TS_SouthEast
  [275].row = 22, [275].col = 11, // TS_South
  [276].row = 22, [276].col = 12, // TS_SouthWest
  [277].row = 22, [277].col = 13, // TS_West
  [278].row = 22, [278].col = 14  // TS_NorthWest
};

static Dim2 g_char_locations[] = {
  [0].row = 23, [0].col = 10, // space
  [1].row = 28, [1].col = 7, // !
  [2].row = 28, [2].col = 12, // "
  [3].row = 28, [3].col = 0, // #
  [4].row = 28, [4].col = 4, // $
  [5].row = 28, [5].col = 1, // %
  [6].row = 28, [6].col = 2, // &
  [7].row = 28, [7].col = 11, // '
  [8].row = 28, [8].col = 13, // (
  [9].row = 28, [9].col = 14,  // )
  [10].row = 28, [10].col = 17, // *
  [11].row = 28, [11].col = 20, // +
  [12].row = 28, [12].col = 6, // ,
  [13].row = 28, [13].col = 21, // -
  [14].row = 28, [14].col = 5, // .
  [15].row = 28, [15].col = 18, // /
  [16].row = 23, [16].col = 0, // 0
  [17].row = 23, [17].col = 1, // 1
  [18].row = 23, [18].col = 2, // 2
  [19].row = 23, [19].col = 3, // 3
  [20].row = 23, [20].col = 4, // 4
  [21].row = 23, [21].col = 5, // 5
  [22].row = 23, [22].col = 6, // 6
  [23].row = 23, [23].col = 7, // 7
  [24].row = 23, [24].col = 8, // 8
  [25].row = 23, [25].col = 9,  // 9
  [26].row = 28, [26].col = 9, // :
  [27].row = 28, [27].col = 10, // ;
  [28].row = 28, [28].col = 22, // <
  [29].row = 28, [29].col = 23, // =
  [30].row = 28, [30].col = 24, // >
  [31].row = 28, [31].col = 8, // ?
  [32].row = 28, [32].col = 11, // `
  [33].row = 24, [33].col = 0, // A
  [34].row = 24, [34].col = 1, // B
  [35].row = 24, [35].col = 2,  // C
  [36].row = 24, [36].col = 3, // D
  [37].row = 24, [37].col = 4, // E
  [38].row = 24, [38].col = 5,  // F
  [39].row = 24, [39].col = 6, // G
  [40].row = 24, [40].col = 7, // H
  [41].row = 24, [41].col = 8,  // I
  [42].row = 24, [42].col = 9, // J
  [43].row = 24, [43].col = 10, // K
  [44].row = 24, [44].col = 11,  // L
  [45].row = 24, [45].col = 12, // M
  [46].row = 24, [46].col = 13, // N
  [47].row = 24, [47].col = 14,  // O
  [48].row = 24, [48].col = 15, // P
  [49].row = 24, [49].col = 16, // Q
  [50].row = 24, [50].col = 17,  // R
  [51].row = 24, [51].col = 18, // S
  [52].row = 24, [52].col = 19, // T
  [53].row = 24, [53].col = 20,  // U
  [54].row = 24, [54].col = 21, // V
  [55].row = 24, [55].col = 22, // W
  [56].row = 24, [56].col = 23,  // X
  [57].row = 24, [57].col = 24, // Y
  [58].row = 24, [58].col = 25, // Z
  [59].row = 28, [59].col = 15,  // [
  [60].row = 0, [60].col = 0, // ~ todo: add this character
  [61].row = 28, [61].col = 16, // ]
  [62].row = 0, [62].col = 0,  // ^ todo: add this character
  [63].row = 0, [63].col = 0, // _ todo: add this character
  [64].row = 28, [64].col = 3, // @
  [65].row = 26, [65].col = 0,  // a
  [66].row = 26, [66].col = 1,  // b
  [67].row = 26, [67].col = 2, // c
  [68].row = 26, [68].col = 3, // d
  [69].row = 26, [69].col = 4,  // e
  [70].row = 26, [70].col = 5, // f
  [71].row = 26, [71].col = 6, // g
  [72].row = 26, [72].col = 7,  // h
  [73].row = 26, [73].col = 8,  // i
  [74].row = 26, [74].col = 9, // j
  [75].row = 26, [75].col = 10, // k
  [76].row = 26, [76].col = 11,  // l
  [77].row = 26, [77].col = 12, // m
  [78].row = 26, [78].col = 13, // n
  [79].row = 26, [79].col = 14,  // o
  [80].row = 26, [80].col = 15,  // p
  [81].row = 26, [81].col = 16, // q
  [82].row = 26, [82].col = 17, // r
  [83].row = 26, [83].col = 18,  // s
  [84].row = 26, [84].col = 19, // t
  [85].row = 26, [85].col = 20, // u
  [86].row = 26, [86].col = 21,  // v
  [87].row = 26, [87].col = 22,  // w
  [88].row = 26, [88].col = 23, // x
  [89].row = 26, [89].col = 24, // y
  [90].row = 26, [90].col = 25,  // z
  [91].row = 28, [91].col = 13, // { using ( instead
  [92].row = 0, [92].col = 0, // | todo: add this character
  [93].row = 28, [93].col = 14,  // } using ) instead
  [94].row = 0, [94].col = 0  // | todo: add this character
};

void tileset_calc_uv_units(Tileset* tileset) {
  tileset->uv_unit.u = 1.0f / (f32)(tileset->image_dim.width / tileset->sprite_dim.width);
  tileset->uv_unit.v = 1.0f / (f32)(tileset->image_dim.height / tileset->sprite_dim.height);
}

Vec2 tileset_get_uv(Tileset* tileset, TilesetSprite sprite) {
  Vec2 res;
  Dim2 sprite_location = g_sprite_locations[sprite];

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

  f32 *e = &(render_struct->text_vertices[TILED_QUAD_GEOMETRY_SIZEOF_1 * render_struct->num_characters]);

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

  u32 *i = &(render_struct->text_indices[TILED_QUAD_INDICES_SIZEOF_1 * render_struct->num_characters]);
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
