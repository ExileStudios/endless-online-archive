#include <vcl.h>
#pragma hdrstop

#include "Tmainform.h"
#include "Gfx_interface.h"
#include "Gfx_npc.h"
#include "Gfx_sprites.h"
#pragma package(smart_init)

// An empty list and the per-graphic tables: the drawing offsets of the frames and the attack frames,
// the head height a balloon sits above, and which graphics idle-animate.
// With preload_graphics the npc library loads now and stays loaded.
Npcs::Npcs(char preload_option)
{
    current_index = -1;
    library_ref_count = 0;
    preload_graphics = preload_option;
    if (preload_graphics)
    {
        graphics_library = LoadLibraryA(MainForm->strings->GetGfxPath(21).c_str());
        library_ref_count++;
    }
    for (int i = 0; i < MAX_NPC_GRAPHICS; i++)
    {
        frame_offsets[i].x = 0; frame_offsets[i].y = 0; attack_frame_offsets[i].x = 0; attack_frame_offsets[i].y = 0;
        has_idle_animation[i] = 0; head_heights[i] = 0;
    }
    frame_offsets[0].y = 16; head_heights[0] = 4;
    frame_offsets[1].y = 18; attack_frame_offsets[1].x = -6; attack_frame_offsets[1].y = -3;
    frame_offsets[2].y = 16; attack_frame_offsets[2].x = -8; attack_frame_offsets[2].y = -4;
    frame_offsets[3].y = 18; attack_frame_offsets[3].x = -8; attack_frame_offsets[3].y = -4; head_heights[3] = 50;
    frame_offsets[4].x = -2; frame_offsets[4].y = 7; attack_frame_offsets[4].x = -8; attack_frame_offsets[4].y = -4;
    head_heights[4] = 20;
    frame_offsets[5].y = 8; attack_frame_offsets[5].x = -6; attack_frame_offsets[5].y = -3; head_heights[5] = 10;
    frame_offsets[6].x = -2; frame_offsets[6].y = 6; attack_frame_offsets[6].x = -4; attack_frame_offsets[6].y = -2;
    head_heights[6] = 12;
    head_heights[7] = 75; attack_frame_offsets[7].x = -2; attack_frame_offsets[7].y = -1;
    frame_offsets[8].x = -4; frame_offsets[8].y = 6; attack_frame_offsets[8].x = -4; attack_frame_offsets[8].y = -2;
    head_heights[8] = 30;
    frame_offsets[9].x = 1; frame_offsets[9].y = -1; attack_frame_offsets[9].x = -6; attack_frame_offsets[9].y = -3;
    head_heights[9] = 8;
    frame_offsets[10].y = 5; attack_frame_offsets[10].x = -4; attack_frame_offsets[10].y = -2; head_heights[10] = 40;
    frame_offsets[11].y = 15; attack_frame_offsets[11].x = -6; attack_frame_offsets[11].y = -3; head_heights[11] = 33;
    frame_offsets[12].y = 13; attack_frame_offsets[12].x = 2; attack_frame_offsets[12].y = -2; head_heights[12] = 10;
    frame_offsets[13].x = -4; frame_offsets[13].y = 10; attack_frame_offsets[13].x = -2;
    attack_frame_offsets[13].y = -4; head_heights[13] = 53;
    frame_offsets[14].x = -1; frame_offsets[14].y = 17; head_heights[14] = 40;
    frame_offsets[15].y = 14; attack_frame_offsets[15].x = -6; attack_frame_offsets[15].y = -3;
    frame_offsets[16].y = 14; attack_frame_offsets[16].x = -6; attack_frame_offsets[16].y = -3;
    frame_offsets[17].y = 14; attack_frame_offsets[17].x = -6; attack_frame_offsets[17].y = -3;
    head_heights[15] = 45;
    head_heights[16] = 45;
    head_heights[17] = 45;
    frame_offsets[18].y = 11; attack_frame_offsets[18].x = -6; attack_frame_offsets[18].y = -3; head_heights[18] = 49;
    frame_offsets[19].x = 0; frame_offsets[19].y = 7; head_heights[19] = 40;
    frame_offsets[20].x = -2; frame_offsets[20].y = 17; attack_frame_offsets[20].x = -10;
    attack_frame_offsets[20].y = -5; head_heights[20] = 40;
    frame_offsets[21].x = 0; frame_offsets[21].y = 11; head_heights[21] = 45; has_idle_animation[21] = 1;
    frame_offsets[22].y = 17; head_heights[22] = 40;
    frame_offsets[23].y = 17; head_heights[23] = 40;
    frame_offsets[24].y = 6; head_heights[24] = 50; has_idle_animation[24] = 1;
    frame_offsets[25].x = -2; frame_offsets[25].y = 11; attack_frame_offsets[25].x = -6;
    attack_frame_offsets[25].y = -3; head_heights[25] = 0;
    frame_offsets[26].y = 9; head_heights[26] = 12; has_idle_animation[26] = 1;
    frame_offsets[27].x = -4; frame_offsets[27].y = 16; head_heights[27] = 42; has_idle_animation[27] = 1;
    frame_offsets[28].y = 7; attack_frame_offsets[28].x = -6; attack_frame_offsets[28].y = -3;
    frame_offsets[29].x = -10; frame_offsets[29].y = 13; attack_frame_offsets[29].x = -8;
    attack_frame_offsets[29].y = -4; head_heights[29] = 42;
    frame_offsets[30].x = 0; frame_offsets[30].y = 13; attack_frame_offsets[30].x = -8; attack_frame_offsets[30].y = -4;
    head_heights[30] = 42;
    frame_offsets[31].y = 17; head_heights[31] = 40;
    frame_offsets[32].y = 17; head_heights[32] = 40;
    frame_offsets[33].y = 17; head_heights[33] = 40;
    frame_offsets[34].x = -5; frame_offsets[34].y = 17; attack_frame_offsets[34].x = -6;
    attack_frame_offsets[34].y = -3; head_heights[34] = 40;
    frame_offsets[35].x = -5; frame_offsets[35].y = 17; attack_frame_offsets[35].x = -6;
    attack_frame_offsets[35].y = -3; head_heights[35] = 40;
    frame_offsets[36].x = -5; frame_offsets[36].y = 17; attack_frame_offsets[36].x = -6;
    attack_frame_offsets[36].y = -3; head_heights[36] = 40;
    frame_offsets[37].x = -1; frame_offsets[37].y = 12; attack_frame_offsets[37].x = -6;
    attack_frame_offsets[37].y = -3;
    frame_offsets[38].x = -1; frame_offsets[38].y = 12; attack_frame_offsets[38].x = -6;
    attack_frame_offsets[38].y = -3;
    frame_offsets[39].x = -1; frame_offsets[39].y = 12; attack_frame_offsets[39].x = -6;
    attack_frame_offsets[39].y = -3;
    frame_offsets[40].x = -1; frame_offsets[40].y = 12; attack_frame_offsets[40].x = -6;
    attack_frame_offsets[40].y = -3;
    frame_offsets[41].x = -1; frame_offsets[41].y = 12; attack_frame_offsets[41].x = -6;
    attack_frame_offsets[41].y = -3;
    frame_offsets[42].x = -1; frame_offsets[42].y = 12; attack_frame_offsets[42].x = -6;
    attack_frame_offsets[42].y = -3;
    frame_offsets[43].x = -1; frame_offsets[43].y = 12; attack_frame_offsets[43].x = -6;
    attack_frame_offsets[43].y = -3;
    frame_offsets[44].x = -1; frame_offsets[44].y = 12; attack_frame_offsets[44].x = -6;
    attack_frame_offsets[44].y = -3; head_heights[44] = 40;
    frame_offsets[45].x = -3; frame_offsets[45].y = 10; attack_frame_offsets[45].x = -6;
    attack_frame_offsets[45].y = -3; head_heights[45] = 42;
    frame_offsets[46].y = 10; attack_frame_offsets[46].x = -6; attack_frame_offsets[46].y = -3; head_heights[46] = 66;
    frame_offsets[47].x = 6; frame_offsets[47].y = 10; attack_frame_offsets[47].x = -6; attack_frame_offsets[47].y = -3;
    head_heights[47] = 66;
    frame_offsets[48].x = -9; frame_offsets[48].y = 13; attack_frame_offsets[48].x = -6;
    attack_frame_offsets[48].y = -3; head_heights[48] = 68;
    frame_offsets[49].x = 4; frame_offsets[49].y = 10; attack_frame_offsets[49].x = -6; attack_frame_offsets[49].y = -3;
    head_heights[49] = 82; has_idle_animation[49] = 1;
    frame_offsets[50].y = 13; attack_frame_offsets[50].x = -10; attack_frame_offsets[50].y = -5; head_heights[50] = 59;
    has_idle_animation[50] = 1;
    frame_offsets[51].y = 1; attack_frame_offsets[51].x = -4; attack_frame_offsets[51].y = -2; head_heights[51] = 48;
    has_idle_animation[51] = 1;
    frame_offsets[52].x = -2; frame_offsets[52].y = 17; head_heights[52] = 40;
    frame_offsets[53].y = 14; head_heights[53] = 28;
    frame_offsets[54].x = -3; frame_offsets[54].y = 4; attack_frame_offsets[54].x = -3; attack_frame_offsets[54].y = -2;
    head_heights[54] = 13; has_idle_animation[54] = 1;
    frame_offsets[55].x = -3; frame_offsets[55].y = 4; attack_frame_offsets[55].x = -3; attack_frame_offsets[55].y = -2;
    head_heights[55] = 13; has_idle_animation[55] = 1;
    frame_offsets[56].x = -3; frame_offsets[56].y = 4; attack_frame_offsets[56].x = -3; attack_frame_offsets[56].y = -2;
    head_heights[56] = 13; has_idle_animation[56] = 1;
    frame_offsets[57].y = 18; attack_frame_offsets[57].x = -8; attack_frame_offsets[57].y = -4; head_heights[57] = 42;
    has_idle_animation[57] = 1;
    frame_offsets[58].y = 16; attack_frame_offsets[58].x = -8; attack_frame_offsets[58].y = -4; head_heights[58] = 44;
    has_idle_animation[58] = 1;
    frame_offsets[59].x = -3; frame_offsets[59].y = 6; attack_frame_offsets[59].x = -6; attack_frame_offsets[59].y = -3;
    head_heights[59] = 10;
    attack_frame_offsets[60].x = -12; attack_frame_offsets[60].y = -6; head_heights[60] = 10;
    frame_offsets[61].x = -3; frame_offsets[61].y = 11; attack_frame_offsets[61].x = -8;
    attack_frame_offsets[61].y = -4; head_heights[61] = 10;
    frame_offsets[62].x = -3; frame_offsets[62].y = 11; attack_frame_offsets[62].x = -8;
    attack_frame_offsets[62].y = -4; head_heights[62] = 10;
    frame_offsets[63].y = 15; attack_frame_offsets[63].x = -12; attack_frame_offsets[63].y = -6; head_heights[63] = 41;
    has_idle_animation[63] = 1;
    frame_offsets[64].x = 3; frame_offsets[64].y = 12; attack_frame_offsets[64].x = -8; attack_frame_offsets[64].y = -4;
    head_heights[64] = 23;
    frame_offsets[65].x = -1; frame_offsets[65].y = 8; attack_frame_offsets[65].x = -10;
    attack_frame_offsets[65].y = -5; head_heights[65] = 53;
    frame_offsets[66].y = 12; attack_frame_offsets[66].x = -4; attack_frame_offsets[66].y = -2; head_heights[66] = 28;
    frame_offsets[67].y = 6; attack_frame_offsets[67].x = -4; attack_frame_offsets[67].y = -2; head_heights[67] = 93;
    frame_offsets[68].y = 20; head_heights[68] = 48; has_idle_animation[68] = 1;
    frame_offsets[69].x = -1; frame_offsets[69].y = 17; attack_frame_offsets[69].x = -8;
    attack_frame_offsets[69].y = -4; head_heights[69] = 18;
    frame_offsets[70].x = -2; frame_offsets[70].y = 10; attack_frame_offsets[70].x = -6;
    attack_frame_offsets[70].y = -3; head_heights[70] = 24;
    frame_offsets[71].x = 1; frame_offsets[71].y = 14; head_heights[71] = 20;
    frame_offsets[72].x = -3; frame_offsets[72].y = 13; attack_frame_offsets[72].x = -8;
    attack_frame_offsets[72].y = -4; head_heights[72] = 55;
    frame_offsets[73].y = 16; attack_frame_offsets[73].x = -4; attack_frame_offsets[73].y = -2; head_heights[73] = 22;
    frame_offsets[74].x = -1; frame_offsets[74].y = 17; attack_frame_offsets[74].x = -6;
    attack_frame_offsets[74].y = -3; head_heights[74] = 37; has_idle_animation[74] = 1;
    frame_offsets[75].x = -1; frame_offsets[75].y = 16; attack_frame_offsets[75].x = -4;
    attack_frame_offsets[75].y = -2; head_heights[75] = 41;
    frame_offsets[76].x = -2; frame_offsets[76].y = 10; head_heights[76] = 14;
    frame_offsets[77].x = -2; frame_offsets[77].y = 10; head_heights[77] = 14;
    frame_offsets[78].y = 15; head_heights[78] = 41; has_idle_animation[78] = 1;
    frame_offsets[79].y = 5; attack_frame_offsets[79].x = -8; attack_frame_offsets[79].y = -4; head_heights[79] = 31;
    frame_offsets[80].x = -3; frame_offsets[80].y = 15; attack_frame_offsets[80].x = -6;
    attack_frame_offsets[80].y = -3; head_heights[80] = 44; has_idle_animation[80] = 1;
    frame_offsets[81].x = -2; frame_offsets[81].y = 18; head_heights[81] = 40;
    frame_offsets[82].x = -2; frame_offsets[82].y = 17; head_heights[82] = 40;
    frame_offsets[83].x = -2; frame_offsets[83].y = 18; head_heights[83] = 40;
    frame_offsets[84].y = 18; attack_frame_offsets[84].x = -10; attack_frame_offsets[84].y = -5; head_heights[84] = 50;
    has_idle_animation[84] = 1;
    frame_offsets[85].x = -2; frame_offsets[85].y = 14; head_heights[85] = 43; has_idle_animation[85] = 1;
    frame_offsets[86].x = -4; frame_offsets[86].y = 11; attack_frame_offsets[86].x = -8;
    attack_frame_offsets[86].y = -4; head_heights[86] = 40;
    frame_offsets[87].x = 1; frame_offsets[87].y = 12; attack_frame_offsets[87].x = -10;
    attack_frame_offsets[87].y = -5; head_heights[87] = 54;
    frame_offsets[88].y = -4; head_heights[88] = 43; has_idle_animation[88] = 1;
    frame_offsets[89].x = 1; frame_offsets[89].y = 6; attack_frame_offsets[89].x = -8; attack_frame_offsets[89].y = -4;
    head_heights[89] = 54; has_idle_animation[89] = 1;
    frame_offsets[90].x = -14; frame_offsets[90].y = 14; attack_frame_offsets[90].x = -3;
    attack_frame_offsets[90].y = -2; head_heights[90] = 64;
    frame_offsets[91].y = 13; attack_frame_offsets[91].x = -6; attack_frame_offsets[91].y = -4; head_heights[91] = 21;
    frame_offsets[92].x = -9; frame_offsets[92].y = 14; head_heights[92] = 55; has_idle_animation[92] = 1;
    frame_offsets[93].x = -9; frame_offsets[93].y = 14; head_heights[93] = 50; has_idle_animation[93] = 1;
    frame_offsets[94].x = -7; frame_offsets[94].y = 9; head_heights[94] = 32;
    frame_offsets[95].x = -1; frame_offsets[95].y = 18; attack_frame_offsets[95].x = -6;
    attack_frame_offsets[95].y = -3; head_heights[95] = 46; has_idle_animation[95] = 1;
    frame_offsets[96].x = -2; frame_offsets[96].y = 14; attack_frame_offsets[96].x = -4;
    attack_frame_offsets[96].y = -2; head_heights[96] = 46;
    frame_offsets[97].x = -1; frame_offsets[97].y = 20; attack_frame_offsets[97].x = -8;
    attack_frame_offsets[97].y = -4; head_heights[97] = 50; has_idle_animation[97] = 1;
    frame_offsets[98].y = 17; attack_frame_offsets[98].x = -6; attack_frame_offsets[98].y = -3; head_heights[98] = 68;
    has_idle_animation[98] = 1;
    frame_offsets[99].y = 17; attack_frame_offsets[99].x = -6; attack_frame_offsets[99].y = -3; head_heights[99] = 30;
    frame_offsets[100].y = 17; attack_frame_offsets[100].x = -6; attack_frame_offsets[100].y = -3;
    head_heights[100] = 30;
    frame_offsets[101].y = 15; head_heights[101] = 84; has_idle_animation[101] = 1;
    frame_offsets[102].y = 16; attack_frame_offsets[102].x = -10; attack_frame_offsets[102].y = -5;
    head_heights[102] = 48;
    frame_offsets[103].y = 15; attack_frame_offsets[103].x = -8; attack_frame_offsets[103].y = -4;
    head_heights[103] = 60;
    frame_offsets[104].y = 15; attack_frame_offsets[104].x = -8; attack_frame_offsets[104].y = -4;
    head_heights[104] = 70; has_idle_animation[104] = 1;
    frame_offsets[105].y = 13; attack_frame_offsets[105].x = -6; attack_frame_offsets[105].y = -3;
    head_heights[105] = 30;
    frame_offsets[106].x = -31; frame_offsets[106].y = 4; attack_frame_offsets[106].x = -4;
    attack_frame_offsets[106].y = -2; head_heights[106] = 138;
    frame_offsets[107].y = 6; attack_frame_offsets[107].x = -10; attack_frame_offsets[107].y = -5;
    head_heights[107] = 12;
    frame_offsets[108].x = -2; frame_offsets[108].y = 16; head_heights[108] = 43;
    frame_offsets[109].x = -2; frame_offsets[109].y = 17; attack_frame_offsets[109].x = -6;
    attack_frame_offsets[109].y = -3; head_heights[109] = 59; has_idle_animation[109] = 1;
    frame_offsets[110].y = 17; attack_frame_offsets[110].x = -6; attack_frame_offsets[110].y = -3;
    head_heights[110] = 17; has_idle_animation[110] = 1;
    frame_offsets[111].x = 4; frame_offsets[111].y = 6; attack_frame_offsets[111].x = -6;
    attack_frame_offsets[111].y = -3; head_heights[111] = 30; has_idle_animation[111] = 1;
    frame_offsets[112].x = 1; frame_offsets[112].y = 7; head_heights[112] = 6;
    frame_offsets[113].y = 17; attack_frame_offsets[113].x = -4; attack_frame_offsets[113].y = -2;
    head_heights[113] = 44; has_idle_animation[113] = 1;
    frame_offsets[114].x = -6; frame_offsets[114].y = 16; attack_frame_offsets[114].x = -6;
    attack_frame_offsets[114].y = -3; head_heights[114] = 42; has_idle_animation[114] = 1;
    frame_offsets[115].y = 18; head_heights[115] = 40; has_idle_animation[115] = 1;
    frame_offsets[116].x = -2; frame_offsets[116].y = 15; head_heights[116] = 32; has_idle_animation[116] = 1;
    frame_offsets[117].x = -6; frame_offsets[117].y = 15; attack_frame_offsets[117].x = -4;
    attack_frame_offsets[117].y = -2; head_heights[117] = 47; has_idle_animation[117] = 1;
    frame_offsets[118].x = -1; frame_offsets[118].y = 13; attack_frame_offsets[118].x = -6;
    attack_frame_offsets[118].y = -3; head_heights[118] = 15;
    frame_offsets[119].x = -3; frame_offsets[119].y = 19; attack_frame_offsets[119].x = -8;
    attack_frame_offsets[119].y = -4; head_heights[119] = 19;
    frame_offsets[120].x = -1; frame_offsets[120].y = 16; attack_frame_offsets[120].x = -2;
    attack_frame_offsets[120].y = -1; head_heights[120] = 10; has_idle_animation[120] = 1;
    frame_offsets[121].y = 18; attack_frame_offsets[121].x = -8; attack_frame_offsets[121].y = -4;
    head_heights[121] = 58; has_idle_animation[121] = 1;
    frame_offsets[122].y = 4; head_heights[122] = 65;
    frame_offsets[123].x = -4; frame_offsets[123].y = 9; attack_frame_offsets[123].x = -10;
    attack_frame_offsets[123].y = -5; head_heights[123] = 71;
    head_heights[124] = 88;
    frame_offsets[125].y = 20; attack_frame_offsets[125].x = -10; attack_frame_offsets[125].y = -5;
    head_heights[125] = 58; has_idle_animation[125] = 1;
    frame_offsets[126].x = -4; frame_offsets[126].y = 12; attack_frame_offsets[126].x = -14;
    attack_frame_offsets[126].y = -5; head_heights[126] = 36; has_idle_animation[126] = 1;
    frame_offsets[127].x = -5; frame_offsets[127].y = 17; attack_frame_offsets[127].x = -4;
    attack_frame_offsets[127].y = -2; head_heights[127] = 36;
    frame_offsets[128].y = 7; head_heights[128] = 32; has_idle_animation[128] = 1;
    frame_offsets[129].y = 15; attack_frame_offsets[129].x = -10; attack_frame_offsets[129].y = -5;
    head_heights[129] = 5;
    frame_offsets[130].x = -1; frame_offsets[130].y = 16; attack_frame_offsets[130].x = -8;
    attack_frame_offsets[130].y = -4; head_heights[130] = 16;
    frame_offsets[131].x = -2; frame_offsets[131].y = 5; attack_frame_offsets[131].x = -8;
    attack_frame_offsets[131].y = -4; head_heights[131] = 40;
    frame_offsets[132].x = -2; frame_offsets[132].y = 5; attack_frame_offsets[132].x = -6;
    attack_frame_offsets[132].y = -3; head_heights[132] = 47;
    frame_offsets[133].y = 18; attack_frame_offsets[133].x = -10; attack_frame_offsets[133].y = -5;
    head_heights[133] = 80; has_idle_animation[133] = 1;
    frame_offsets[134].y = 18; attack_frame_offsets[134].y = 5; head_heights[134] = 41;
    frame_offsets[135].x = -2; frame_offsets[135].y = 19; attack_frame_offsets[135].x = -4;
    attack_frame_offsets[135].y = -2; head_heights[135] = 22;
    frame_offsets[136].y = 18; attack_frame_offsets[136].x = -8; attack_frame_offsets[136].y = -4;
    head_heights[136] = 45;
    frame_offsets[137].y = 18; attack_frame_offsets[137].x = -6; attack_frame_offsets[137].y = -3;
    head_heights[137] = 26;
    frame_offsets[138].y = 17; attack_frame_offsets[138].x = -6; attack_frame_offsets[138].y = -3;
    head_heights[138] = 32;
    frame_offsets[139].y = 18; attack_frame_offsets[139].x = -2; attack_frame_offsets[139].y = -1;
    head_heights[139] = 65; has_idle_animation[139] = 1;
    frame_offsets[140].y = 16; attack_frame_offsets[140].x = -6; attack_frame_offsets[140].y = -3;
    head_heights[140] = 25;
    frame_offsets[141].y = 18; attack_frame_offsets[141].x = -2; attack_frame_offsets[141].y = -1;
    head_heights[141] = 20; has_idle_animation[141] = 1;
    frame_offsets[142].y = 16; attack_frame_offsets[142].x = -6; attack_frame_offsets[142].y = -3;
    head_heights[142] = 40;
    frame_offsets[143].x = -3; frame_offsets[143].y = 8; head_heights[143] = 52;
    frame_offsets[144].y = 19; attack_frame_offsets[144].x = -6; attack_frame_offsets[144].y = -3;
    head_heights[144] = 48;
    frame_offsets[145].y = 19; attack_frame_offsets[145].x = -6; attack_frame_offsets[145].y = -3;
    head_heights[145] = 69;
    frame_offsets[146].y = 10; head_heights[146] = 32;
    frame_offsets[147].x = -4; frame_offsets[147].y = 16; attack_frame_offsets[147].x = -6;
    attack_frame_offsets[147].y = -3; head_heights[147] = 37; has_idle_animation[147] = 1;
    frame_offsets[148].x = 2; frame_offsets[148].y = 6; head_heights[148] = 98;
    frame_offsets[149].x = -1; frame_offsets[149].y = 18; attack_frame_offsets[149].x = -6;
    attack_frame_offsets[149].y = -3; head_heights[149] = 48;
    frame_offsets[150].x = -2; frame_offsets[150].y = 1; attack_frame_offsets[150].x = -2;
    attack_frame_offsets[150].y = 1; head_heights[150] = 18;
    frame_offsets[151].y = 10; attack_frame_offsets[151].x = -8; attack_frame_offsets[151].y = -2;
    head_heights[151] = 27;
    frame_offsets[152].x = -2; frame_offsets[152].y = 13; attack_frame_offsets[152].x = -6;
    attack_frame_offsets[152].y = -9; head_heights[152] = 39;
    frame_offsets[153].y = 15; attack_frame_offsets[153].x = -6; attack_frame_offsets[153].y = -3;
    head_heights[153] = 13;
    frame_offsets[154].y = 16; attack_frame_offsets[154].x = -6; attack_frame_offsets[154].y = -3;
    head_heights[154] = 18;
    frame_offsets[155].y = 15; attack_frame_offsets[155].x = -6; attack_frame_offsets[155].y = -3;
    head_heights[155] = 32;
    frame_offsets[156].y = 6; attack_frame_offsets[156].x = -8; attack_frame_offsets[156].y = -4;
    head_heights[156] = 58; has_idle_animation[156] = 1;
    frame_offsets[157].y = 15; head_heights[157] = 74; has_idle_animation[157] = 1;
    frame_offsets[158].x = -11; frame_offsets[158].y = 18; head_heights[158] = 57; has_idle_animation[158] = 1;
    frame_offsets[159].x = -4; frame_offsets[159].y = 18; head_heights[159] = 45; has_idle_animation[159] = 1;
    head_heights[160] = 65; has_idle_animation[160] = 1;
    frame_offsets[161].x = -1; frame_offsets[161].y = 10; attack_frame_offsets[161].x = -8;
    attack_frame_offsets[161].y = -4; head_heights[161] = 15;
    frame_offsets[162].y = 8; attack_frame_offsets[162].x = -6; attack_frame_offsets[162].y = -3;
    head_heights[162] = 12;
    frame_offsets[163].y = 15; attack_frame_offsets[163].x = -8; attack_frame_offsets[163].y = -4;
    head_heights[163] = 10;
    frame_offsets[164].y = 13; head_heights[164] = 10;
    frame_offsets[165].y = 18; head_heights[165] = 32; has_idle_animation[165] = 1;
    frame_offsets[166].y = 14; attack_frame_offsets[166].x = -6; attack_frame_offsets[166].y = -3;
    head_heights[166] = 22;
    frame_offsets[167].y = 15; attack_frame_offsets[167].x = -8; attack_frame_offsets[167].y = -4;
    head_heights[167] = 15;
    frame_offsets[168].y = 4; attack_frame_offsets[168].x = -8; attack_frame_offsets[168].y = -4;
    head_heights[168] = 19;
    frame_offsets[169].y = 19; attack_frame_offsets[169].x = -8; attack_frame_offsets[169].y = -4;
    head_heights[169] = 17;
}

Npcs::~Npcs()
{
    if (library_ref_count > 0 || preload_graphics)
        FreeLibrary(graphics_library);
}

// Frees the npc library if it is loaded; with preload_graphics it is loaded again at once.
void Npcs::ReloadGraphicsLibrary()
{
    if (library_ref_count > 0 || preload_graphics)
    {
        FreeLibrary(graphics_library);
        if (!preload_graphics)
            library_ref_count = 0;
    }
    if (preload_graphics)
        graphics_library = LoadLibraryA(MainForm->strings->GetGfxPath(21).c_str());
}

// One step of every npc's timers and animations, then the name label of the npc under the mouse.
void Npcs::Animate()
{
    Npcs *this_copy = this;
    char walk_started;
    char no_hover_yet;
    int screen_y;
    walk_started = 0;
    vector<Npc>::iterator it;
    for (it = npcs.begin(); it != npcs.end(); it++)
    {
        this_copy = this;
        // Graphics with an idle animation step their idle frame on every pass.
        if (has_idle_animation[it->graphic - 1])
        {
            it->idle_frame++;
            if (it->idle_frame > 3)
                it->idle_frame = -1;
            if (!it->idle_frame || it->idle_frame == 2)
                it->redraw = 1;
        }
        if (it->effect_start)
        {
            it->effect_start = 0;
            it->effect_running = 1;
            it->effect_frame = 0;
            MainForm->sound_mapper->PlayEffectSound(it->effect_id);
        }
        if (it->effect_running)
        {
            if (it->effect_frame <= it->effect_last_frame)
                it->effect_frame++;
            else
            {
                it->effect_loops_left--;
                it->effect_frame = 0;
                if (it->effect_loops_left <= 0)
                    it->effect_running = 0;
            }
            it->redraw = 1;
        }
        // A step starts at once; the npc is placed on the new tile as it starts.
        if (it->walk_start)
        {
            it->walk_start = 0;
            it->x = it->walk_target_x;
            it->y = it->walk_target_y;
            it->walk_direction = it->walk_target_direction;
            it->direction = it->walk_facing;
            it->walk_frame = -1;
            it->walking = 1;
            walk_started = 1;
        }
        // As for players: four frames from the pass the step starts, and the fifth pass ends it.
        if (it->walking)
        {
            if (it->walk_frame <= 2)
                it->walk_frame++;
            else
            {
                it->walk_frame = 0;
                it->walking = 0;
            }
            it->redraw = 1;
        }
        if (it->attack_start)
        {
            it->attack_start = 0;
            it->attack_direction = it->direction;
            it->attack_frame = -1;
            it->attacking = 1;
        }
        if (it->attacking)
        {
            if (it->attack_frame <= 2)
            {
                it->attack_frame++;
                if (it->attack_frame == 1)
                    MainForm->sfx->PlaySfx(10);
            }
            else
            {
                it->attack_frame = 0;
                it->attacking = 0;
            }
            it->redraw = 1;
        }
        if (it->alt_attack_start)
        {
            it->alt_attack_start = false;
            it->alt_attack_direction = it->direction;
            it->alt_attack_frame = -1;
            it->alt_attacking = true;
        }
        if (it->alt_attacking)
        {
            if (it->alt_attack_frame <= 2)
            {
                it->alt_attack_frame++;
                if (it->alt_attack_frame == 1)
                    MainForm->sfx->PlaySfx(11);
            }
            else
            {
                it->alt_attack_frame = 0;
                it->alt_attacking = false;
            }
            it->redraw = 1;
        }
        if (it->health_bar_start)
        {
            it->health_bar_start = 0;
            if (it->health_bar_timer > 0)
            {
                it->health_bar_shown = 1;
                it->redraw = 1;
            }
        }
        if (it->health_bar_shown)
        {
            it->health_bar_timer--;
            if (it->health_bar_timer <= 0)
            {
                it->health_bar_shown = 0;
                it->redraw = 1;
            }
        }
        if (it->damage_start)
        {
            it->damage_start = 0;
            if (it->damage_timer > 0)
            {
                it->damage_shown = 1;
                it->redraw = 1;
            }
        }
        if (it->damage_shown)
        {
            it->damage_timer--;
            it->redraw = 1;
            if (it->damage_timer <= 0)
            {
                it->damage_shown = 0;
                it->redraw = 1;
            }
        }
        if (it->dying)
        {
            it->death_countdown--;
            MainForm->redraw_needed = true;
        }
        if (it->balloon_shown)
        {
            it->balloon_timer--;
            if (it->balloon_timer <= 1)
            {
                it->CloseBalloon();
                if (it->queued_balloon_count > 0)
                {
                    it->queued_balloon_count--;
                    String FileHandle[6];
                    for (int j = 0; j < 6; j++)
                        FileHandle[j] = it->queued_balloon_lines[j][0];
                    it->ShowBalloon(FileHandle, it->queued_balloon_widths[0], it->queued_balloon_heights[0],
                        it->queued_balloon_tinted[0]);
                    for (int queue_slot = 1; queue_slot < 5; queue_slot++)
                    {
                        it->queued_balloon_lines[0][queue_slot - 1] = it->queued_balloon_lines[0][queue_slot];
                        it->queued_balloon_lines[1][queue_slot - 1] = it->queued_balloon_lines[1][queue_slot];
                        it->queued_balloon_lines[2][queue_slot - 1] = it->queued_balloon_lines[2][queue_slot];
                        it->queued_balloon_lines[3][queue_slot - 1] = it->queued_balloon_lines[3][queue_slot];
                        it->queued_balloon_lines[4][queue_slot - 1] = it->queued_balloon_lines[4][queue_slot];
                        it->queued_balloon_lines[5][queue_slot - 1] = it->queued_balloon_lines[5][queue_slot];
                        it->queued_balloon_widths[queue_slot - 1] = it->queued_balloon_widths[queue_slot];
                        it->queued_balloon_heights[queue_slot - 1] = it->queued_balloon_heights[queue_slot];
                        it->queued_balloon_tinted[queue_slot - 1] = it->queued_balloon_tinted[queue_slot];
                    }
                }
            }
        }
    }
    // A player under the mouse keeps the label; npcs get one only when no player has it.
    if (!MainForm->players->hover_label_shown)
    {
        no_hover_yet = 1;
        for (vector<Npc>::reverse_iterator it(npcs.end()); it != vector<Npc>::reverse_iterator(npcs.begin()); it++)
        {
            int screen_x = 32 * (*it).x - 32 * (*it).y + MainForm->map->GetScrollOffsetX() + 319;
            screen_y = 16 * (*it).x + 16 * (*it).y + MainForm->map->GetScrollOffsetY() + 160;
            if (!(screen_x - (*it).frame_width / 2 > mouse_x || (*it).frame_width / 2 + screen_x < mouse_x ||
                screen_y - (*it).frame_height > mouse_y || screen_y + 5 < mouse_y))
            {
                if (!(!no_hover_yet || (*it).dying))
                {
                    if (!(*it).label_shown)
                    {
                        String npc_name = MainForm->npc_values->GetName((*it).npc_id);
                        MainForm->gfx->sprites->ShowNameLabel(npc_name, 0);
                        (*it).label_shown = 1;
                        MainForm->redraw_needed = true;
                    }
                    no_hover_yet = 0;
                }
                else
                {
                    if ((*it).label_shown)
                    {
                        (*it).label_shown = 0;
                        MainForm->redraw_needed = true;
                    }
                }
            }
            else
            {
                if ((*it).label_shown)
                {
                    (*it).label_shown = 0;
                    MainForm->redraw_needed = true;
                }
            }
        }
        if (no_hover_yet)
            hover_label_shown = 0;
        else
            hover_label_shown = 1;
    }
    else
    {
        for (vector<Npc>::reverse_iterator it(npcs.end()); it != vector<Npc>::reverse_iterator(npcs.begin()); it++)
        {
            if ((*it).label_shown)
            {
                (*it).label_shown = 0;
                MainForm->redraw_needed = true;
            }
        }
        hover_label_shown = 0;
    }
    if (walk_started)
        SortByPosition();
}

// Drops npcs whose death countdown ran out, and npcs that stay out of view of the map centre.
void Npcs::RemoveExpiredNpcs()
{
    int center_y;
    int center_x = 0;
    center_x = MainForm->map->GetCenterX();
    center_y = MainForm->map->GetCenterY();
    do
    {
        list_changed = 0;
        for (vector<Npc>::iterator i = npcs.begin(); i != npcs.end(); i++)
        {
            if (i->dying)
            {
                if (i->death_countdown <= 0)
                {
                    i->FreeResources();
                    npcs.erase(i);
                    list_changed = 1;
                    break;
                }
            }
            else
            {
                if (!IsInViewRange(center_x, center_y, i->x, i->y))
                {
                    if (!i->out_of_range)
                    {
                        i->out_of_range = 1;
                        i->removal_countdown = 6;
                    }
                    else
                    {
                        i->removal_countdown--;
                        if (i->removal_countdown <= 0)
                        {
                            i->FreeResources();
                            i->CloseBalloon();
                            npcs.erase(i);
                            list_changed = 1;
                            break;
                        }
                    }
                }
                else
                    i->out_of_range = 0;
            }
        }
    } while (list_changed);
}

// Loads each pending npc's frames over four calls: standing and idle, then walking, then attacking, then done.
// At most three pending npcs are worked on in a call.
void Npcs::LoadPendingGraphics()
{
    int loaded_count;
    int base_resource;
    int library;
    int j = 0;
    loaded_count = 0;
    for (vector<Npc>::iterator i = npcs.begin(); i != npcs.end() && loaded_count <= 2; i++)
    {
        if (i->reload_requested)
        {
            if (Application->Active)
            {
                i->FreeResources();
                i->CreateResources();
                i->reload_requested = 0;
                i->graphics_pending = 1;
                i->load_stage = 0;
            }
        }
        if (i->graphics_pending)
        {
            if (Application->Active || MainForm->config->GetFullscreen() == 0)
            {
                loaded_count++;
                // The first bitmap of the npc's graphic in the npc library.
                base_resource = RESOURCES_PER_GRAPHIC * i->graphic + 61;
                if (!i->load_stage)
                {
                    if (!preload_graphics && library_ref_count <= 0)
                        graphics_library = LoadLibraryA(MainForm->strings->GetGfxPath(21).c_str());
                    HMODULE library_handle = graphics_library;
                    i->resources->CreateSheets((int)library_handle, base_resource, has_idle_animation[i->graphic - 1]);
                    i->frame_width = i->resources->GetFrameWidth();
                    i->frame_height = i->resources->GetFrameHeight();
                    if (graphics_library)
                    {
                        i->resources->LoadFrontFrame(0, (int)library_handle, base_resource);
                        i->resources->LoadBackFrame(0, (int)library_handle, base_resource + 2);
                        if (has_idle_animation[i->graphic - 1])
                        {
                            i->resources->LoadFrontFrame(1, (int)library_handle, base_resource + 1);
                            i->resources->LoadBackFrame(1, (int)library_handle, base_resource + 3);
                        }
                    }
                }
                if (i->load_stage == 1)
                {
                    library = (int)graphics_library;
                    if (graphics_library)
                    {
                        for (j = 0; j < 4; j++)
                        {
                            i->resources->LoadFrontFrame(j + 2, library, j + base_resource + 4);
                            i->resources->LoadBackFrame(j + 2, library, j + base_resource + 8);
                        }
                    }
                }
                if (i->load_stage == 2)
                {
                    int attack_library = (int)graphics_library;
                    if (graphics_library)
                    {
                        i->resources->LoadFrontFrame(6, attack_library, base_resource + 12);
                        i->resources->LoadBackFrame(6, attack_library, base_resource + 14);
                        i->resources->LoadFrontFrame(7, attack_library, base_resource + 13);
                        i->resources->LoadBackFrame(7, attack_library, base_resource + 15);
                        i->resources->LoadFrontFrame(8, attack_library, base_resource + 16);
                        i->resources->LoadBackFrame(8, attack_library, base_resource + 17);
                    }
                }
                if (Application->Active || MainForm->config->GetFullscreen() == 0)
                    i->load_stage++;
                if (i->load_stage >= 4)
                {
                    i->graphics_pending = 0;
                    i->redraw = 1;
                    if (!preload_graphics)
                    {
                        if (library_ref_count > 0)
                            library_ref_count--;
                        if (!library_ref_count)
                        {
                            FreeLibrary(graphics_library);
                            graphics_library = 0;
                        }
                    }
                }
            }
        }
    }
}

// The record id of the npc with the map index, or -1.
int Npcs::GetNpcId(int index)
{
    int npc_id = -1;
    for (vector<Npc>::iterator it = npcs.begin(); it != npcs.end(); it++)
    {
        if (it->index == (unsigned int)index)
        {
            npc_id = it->npc_id;
            break;
        }
    }
    return npc_id;
}

// True when any npc needs a redraw; clears the flags.
bool Npcs::ConsumeRedraw()
{
    bool any_redraw = 0;
    for (vector<Npc>::iterator it = npcs.begin(); it != npcs.end(); it++)
    {
        if (it->redraw)
        {
            it->redraw = 0;
            any_redraw = 1;
        }
    }
    return any_redraw;
}

// True when the tile is in view range of the centre, which reaches further past the centre on both axes.
DEFINE_IS_IN_VIEW_RANGE(Npcs)

// Sorts the npcs by their position on the map.
void Npcs::SortByPosition()
{
    char swapped;
    do
    {
        swapped = 0;
        for (int i = 0; i < (int)GetCount() - 1; i++)
        {
            Npc handle;
            if (npcs[i].y == npcs[i + 1].y)
            {
                if (npcs[i].x > npcs[i + 1].x)
                {
                    handle = npcs[i];
                    npcs[i] = npcs[i + 1];
                    npcs[i + 1] = handle;
                    swapped = 1;
                }
            }
            if (npcs[i].y > npcs[i + 1].y)
            {
                handle = npcs[i];
                npcs[i] = npcs[i + 1];
                npcs[i + 1] = handle;
                swapped = 1;
            }
        }
    } while (swapped);
}

// True when an npc with the map index is on the list and not dying.
bool Npcs::HasNpc(int index)
{
    bool found = 0;
    vector<Npc>::iterator it;
    for (it = npcs.begin(); it != npcs.end(); it++)
    {
        if ((unsigned int)index == it->index && !it->dying)
        {
            found = 1;
            break;
        }
    }
    return found;
}

// True when a living npc stands on the tile.
bool Npcs::HasNpcAt(int x, int y)
{
    bool found = 0;
    vector<Npc>::iterator it;
    for (it = npcs.begin(); it != npcs.end(); it++)
    {
        if ((unsigned int)x == it->x && (unsigned int)y == it->y && !it->dying)
        {
            found = 1;
            break;
        }
    }
    return found;
}

// Starts the npc's death countdown and takes its map index away, so lookups by the index no longer find it.
void Npcs::KillNpc(int index)
{
    for (vector<Npc>::iterator it = npcs.begin(); it != npcs.end(); it++)
    {
        if ((unsigned int)index == it->index)
        {
            it->dying = 1;
            it->death_countdown = 9;
            it->index = -1;
            return;
        }
    }
}

// Starts the death countdown of every npc with the record id.
void Npcs::KillNpcsById(int npc_id)
{
    for (vector<Npc>::iterator it = npcs.begin(); it != npcs.end(); it++)
    {
        if (npc_id != it->npc_id)
            continue;
        it->dying = 1;
        it->death_countdown = 9;
        it->index = -1;
    }
}

Npc Npcs::GetNpc(int index)
{
    return npcs[index];
}

// Queues a step to the tile, facing direction.
void Npcs::StartWalk(int index, int x, int y, int direction)
{
    vector<Npc>::iterator it;
    for (it = npcs.begin(); it != npcs.end(); it++)
    {
        if ((unsigned int)index == it->index)
        {
            it->walk_start = 1;
            it->walk_target_x = x;
            it->walk_target_y = y;
            it->walk_target_direction = direction;
            it->walk_facing = direction;
            return;
        }
    }
}

// Shows a damage or heal number over the npc while its timer runs.
void Npcs::ShowDamage(int index, int amount, char heal)
{
    for (vector<Npc>::iterator it = npcs.begin(); it != npcs.end(); it++)
    {
        if ((unsigned int)index == it->index)
        {
            if (amount >= 0)
            {
                String amount_text = IntToStr(amount);
                it->damage_digits = amount_text.Length();
            }
            it->damage_start = 1;
            it->damage_shown = 0;
            it->damage_timer = 8;
            it->damage_amount = amount;
            it->damage_heal = heal;
            break;
        }
    }
}

// Shows the npc's health bar for duration passes; percent scales to the bar's width.
void Npcs::ShowHealthBar(int index, int percent, int duration)
{
    double bar_width;
    vector<Npc>::iterator it;
    for (it = npcs.begin(); it != npcs.end(); it++)
    {
        if ((unsigned int)index == it->index)
        {
            bar_width = percent * HEALTH_BAR_SCALE;
            it->health_bar_start = 1;
            it->health_bar_shown = 0;
            it->health_bar_timer = duration;
            it->health_bar_width = bar_width;
            return;
        }
    }
}

// Starts a spell effect on the npc, with its layers, frames and loops from the magic map.
void Npcs::StartEffect(int index, int effect_id)
{
    vector<Npc>::iterator it;
    for (it = npcs.begin(); it != npcs.end(); it++)
    {
        if ((unsigned int)index == it->index)
        {
            it->effect_start = 1;
            it->effect_running = 0;
            it->effect_id = effect_id;
            it->effect_frame = 0;
            it->effect_base_layer = MainForm->magic_mapper->base_layer[effect_id];
            it->effect_blend_layer = MainForm->magic_mapper->blend_layer[effect_id];
            it->effect_solid_layer = MainForm->magic_mapper->solid_layer[effect_id];
            it->effect_last_frame = MainForm->magic_mapper->GetFrameCount(effect_id) - 2;
            it->effect_loops_left = MainForm->magic_mapper->GetLoopCount(effect_id);
            return;
        }
    }
}

// Queues an attack in the direction.
void Npcs::StartAttack(int index, int direction)
{
    for (vector<Npc>::iterator it = npcs.begin(); it != npcs.end(); it++)
    {
        if ((unsigned int)index == it->index)
        {
            it->attack_start = 1;
            it->direction = direction;
            return;
        }
    }
}

// Adds an npc from a packet, unless one with the map index is on the list.
void Npcs::AddNpc(int index, int npc_id, int graphic, String name, int x, int y, int direction)
{
    if (!HasNpc(index))
    {
        Npc handle(index, npc_id, graphic, name, x, y, direction);
        handle.hidden = 0;
        handle.CreateResources();
        npcs.insert(npcs.end(), handle);
        SortByPosition();
    }
}

// Frees every npc's cells and balloon, then empties the list.
void Npcs::Clear()
{
    int __slot1;
    int __slot2;
    for (vector<Npc>::iterator it = npcs.begin(); it != npcs.end(); it++)
    {
        it->FreeResources();
        it->CloseBalloon();
    }
    if (GetCount() >= 1)
    {
        npcs.clear();
        current_index = -1;
    }
}

unsigned int Npcs::GetCount()
{
    return npcs.size();
}

void Npcs::ShowBalloon(int index, String *lines, int width, int height, int tinted)
{
    vector<Npc>::iterator it;
    for (it = npcs.begin(); it != npcs.end(); it++)
    {
        if ((unsigned int)index == it->index)
        {
            it->ShowBalloon(lines, width, height, tinted);
            return;
        }
    }
}

void Npcs::SetMousePosition(int cursor_x, int cursor_y)
{
    mouse_x = cursor_x;
    mouse_y = cursor_y;
}
