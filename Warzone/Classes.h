#pragma once
#include "pch.h"

typedef float vec_t;

struct vec2_t
{
	vec_t x, y;
};

class vec3_t
{
public:

	union
	{
		struct { vec_t x, y, z; };
		vec_t v[3];
	};

	vec3_t() { x = y = z = 0; }
	vec3_t(const vec_t x, const vec_t y, const vec_t z) : x(x), y(y), z(z) {}
	vec3_t operator + (const vec3_t& rhs) const { return vec3_t(x + rhs.x, y + rhs.y, z + rhs.z); }
	vec3_t operator - (const vec3_t& rhs) const { return vec3_t(x - rhs.x, y - rhs.y, z - rhs.z); }
	vec3_t operator * (const vec_t& rhs) const { return vec3_t(x * rhs, y * rhs, z * rhs); }
	vec3_t operator / (const vec_t& rhs) const { return vec3_t(x / rhs, y / rhs, z / rhs); }
	vec3_t& operator += (const vec3_t& rhs) { return *this = *this + rhs; }
	vec3_t& operator -= (const vec3_t& rhs) { return *this = *this - rhs; }
	vec3_t& operator *= (const vec_t& rhs) { return *this = *this * rhs; }
	vec3_t& operator /= (const vec_t& rhs) { return *this = *this / rhs; }
	vec_t dot() const { return x * x + y * y + z * z; }
	vec_t Length() const { return sqrtf(dot()); }
	vec3_t Normalize() const { return *this * (1 / Length()); }
	vec_t Distance(const vec3_t& rhs) const { return (*this - rhs).Length(); }
};

struct vec4_t
{
	vec_t x, y, z, w;
};

enum RadarMode : __int8
{
	RADAR_MODE_NORMAL = 0x0,
	RADAR_MODE_FAST = 0x1,
	RADAR_MODE_CONSTANT = 0x2,
	RADAR_MODE_SLOW = 0x3,
	RADAR_MODE_COUNT = 0x4,
};

class playerState_s
{
public:
	char pad_0000[48]; //0x0000
	vec3_t vecOrig; //0x0030
	char pad_003C[416]; //0x003C
	vec3_t viewAngle; //0x01DC
	char pad_01E8[116]; //0x01E8
	int32_t Health; //0x025C
	char pad_0260[164]; //0x0260
	bool radarEnabled; //0x0304
	bool radarBlocked; //0x0305
	bool radarShowEnemyDirection; //0x0306
	RadarMode radarMode; //0x0307
};

/*class WeaponDef
{
public:
	class Weapon* Weapons[962]; //0x0000
	//char pad_0FA0[120]; //0x0FA0
}; //Size: 0x1018
//static_assert(sizeof(WeaponDef) == 0x1018);

class Weapon
{
public:
	char pad_0000[480]; //0x0000
	int32_t iFireTime; //0x01E0
	int32_t iFireDelay; //0x01E4
	int32_t iFireTimeAkimbo; //0x01E8
	int32_t iFireDelayAkimbo; //0x01EC
	float sprintOutTimerScale; //0x01F0
	char pad_01F4[660]; //0x01F4
}; //Size: 0x0488
static_assert(sizeof(Weapon) == 0x488);*/

// Created with GeSucri.DIO 1.2 by KN4CK3R

class WeaponCompleteDefArr
{
public:
	class WeaponCompleteDef* weaponCompleteDefArr[964]; //0x0000
}; 

class WeaponCompleteDef
{
public:
	char pad_0000[8]; //0x0000
	class WeaponDef* weapDef; //0x0008
	char pad_0010[120]; //0x0010
}; //Size: 0x0088
static_assert(sizeof(WeaponCompleteDef) == 0x88);

class WeaponDef
{
public:
	char pad_0000[1112]; //0x0000
	float spread; //0x0458
	float spread2; //0x045C
	float spread3; //0x0460
	float spread4; //0x0464
	float spread5; //0x0468
	float spread6; //0x046C
	float spread7; //0x0470
	char pad_0474[4]; //0x0474
	float spread8; //0x0478
	float spread9; //0x047C
	float spread10; //0x0480
	float spread11; //0x0484
	float spread12; //0x0488
	float spread13; //0x048C
	float spread14; //0x0490
	char pad_0494[76]; //0x0494
	float spread15; //0x04E0
	float spread16; //0x04E4
	float spread17; //0x04E8
	float spread18; //0x04EC
	char pad_04F0[8]; //0x04F0
	float spread19; //0x04F8
	char pad_04FC[20]; //0x04FC
	float spread20; //0x0510
	float spread21; //0x0514
	float spread22; //0x0518
	char pad_051C[12]; //0x051C
	float spread23; //0x0528
	float spread24; //0x052C
	float spread25; //0x0530
	float spread26; //0x0534
	float spread27; //0x0538
	float spread28; //0x053C
	float spread29; //0x0540
	float spread30; //0x0544
	float spread31; //0x0548
	float spread32; //0x054C
	float spread33; //0x0550
	float spread34; //0x0554
	float spread35; //0x0558
	float spread36; //0x055C
	float spread37; //0x0560
	float spread38; //0x0564
	float spread39; //0x0568
	float spread40; //0x056C
	float spread41; //0x0570
	char pad_0574[4]; //0x0574
	float spread42; //0x0578
	float spread43; //0x057C
	float spread44; //0x0580
	float spread45; //0x0584
	float spread46; //0x0588
	char pad_058C[4]; //0x058C
	float spread47; //0x0590
	char pad_0594[4]; //0x0594
	float spread48; //0x0598
	float spread49; //0x059C
	float spread50; //0x05A0
	float spread51; //0x05A4
	float spread52; //0x05A8
	float spread53; //0x05AC
	char pad_05B0[8]; //0x05B0
	float spread54; //0x05B8
	float spread55; //0x05BC
	float spread56; //0x05C0
	char pad_05C4[36]; //0x05C4
	float spread57; //0x05E8
	float spread58; //0x05EC
	char pad_05F0[8]; //0x05F0
	float spread59; //0x05F8
	float spread60; //0x05FC
	float spread61; //0x0600
	float spread62; //0x0604
	float spread63; //0x0608
	float spread64; //0x060C
	float spread65; //0x0610
	char pad_0614[8]; //0x0614
	float spread66; //0x061C
	char pad_0620[4]; //0x0620
	float spread67; //0x0624
	float spread68; //0x0628
	char pad_062C[16]; //0x062C
	float spread69; //0x063C
	float spread70; //0x0640
	float spread71; //0x0644
	float spread72; //0x0648
	float spread73; //0x064C
	float spread74; //0x0650
	float spread75; //0x0654
	float spread76; //0x0658
	float spread77; //0x065C
	char pad_0660[20]; //0x0660
	float spread78; //0x0674
	float spread79; //0x0678
	float spread80; //0x067C
	float spread81; //0x0680
	float spread82; //0x0684
	float spread83; //0x0688
	float spread84; //0x068C
	float spread85; //0x0690
	float spread86; //0x0694
	float spread87; //0x0698
	char pad_069C[24]; //0x069C
	float spread88; //0x06B4
	float spread89; //0x06B8
	float spread90; //0x06BC
	float spread91; //0x06C0
	char pad_06C4[48]; //0x06C4
	float spread92; //0x06F4
	float spread93; //0x06F8
	float spread94; //0x06FC
	float spread95; //0x0700
	float spread96; //0x0704
	float spread97; //0x0708
	float spread98; //0x070C
	char pad_0710[340]; //0x0710
	float spread99; //0x0864
	float spread100; //0x0868
	char pad_086C[8]; //0x086C
	float spread101; //0x0874
	float spread102; //0x0878
	float spread103; //0x087C
	char pad_0880[4]; //0x0880
	float spread104; //0x0884
	float spread105; //0x0888
	char pad_088C[16]; //0x088C
	float spread106; //0x089C
	float spread107; //0x08A0
	char pad_08A4[136]; //0x08A4
	float spread108; //0x092C
	char pad_0930[20]; //0x0930
	float spread109; //0x0944
	float spread110; //0x0948
	float spread111; //0x094C
	float spread112; //0x0950
	float spread113; //0x0954
	float spread114; //0x0958
	float spread115; //0x095C
	float spread116; //0x0960
	float spread117; //0x0964
	float spread118; //0x0968
	float spread119; //0x096C
	float spread120; //0x0970
	float spread121; //0x0974
	float spread122; //0x0978
	float spread123; //0x097C
	float spread124; //0x0980
	float spread125; //0x0984
	float spread126; //0x0988
	float spread127; //0x098C
	float spread128; //0x0990
	float spread129; //0x0994
	float spread130; //0x0998
	float spread131; //0x099C
	char pad_09A0[36]; //0x09A0
	float spread132; //0x09C4
	float spread133; //0x09C8
	float spread134; //0x09CC
	char pad_09D0[4]; //0x09D0
	float spread135; //0x09D4
	float spread136; //0x09D8
	char pad_09DC[24]; //0x09DC
	float spread137; //0x09F4
	float spread138; //0x09F8
	float spread139; //0x09FC
	float spread140; //0x0A00
	float spread141; //0x0A04
	float spread142; //0x0A08
	float spread143; //0x0A0C
	char pad_0A10[4]; //0x0A10
	float spread144; //0x0A14
	float spread145; //0x0A18
	char pad_0A1C[152]; //0x0A1C
	float spread146; //0x0AB4
	float spread147; //0x0AB8
	float spread148; //0x0ABC
	float spread149; //0x0AC0
	float spread150; //0x0AC4
	float spread151; //0x0AC8
	float spread152; //0x0ACC
	float spread153; //0x0AD0
	float spread154; //0x0AD4
	float spread155; //0x0AD8
	float spread156; //0x0ADC
	float spread157; //0x0AE0
	float spread158; //0x0AE4
	float spread159; //0x0AE8
	float spread160; //0x0AEC
	float spread161; //0x0AF0
	float spread162; //0x0AF4
	float spread163; //0x0AF8
	float spread164; //0x0AFC
	float spread165; //0x0B00
	float spread166; //0x0B04
	float spread167; //0x0B08
	float spread168; //0x0B0C
	char pad_0B10[36]; //0x0B10
	float spread169; //0x0B34
	float spread170; //0x0B38
	float spread171; //0x0B3C
	float spread172; //0x0B40
	float spread173; //0x0B44
	float spread174; //0x0B48
	float spread175; //0x0B4C
	float spread176; //0x0B50
	float spread177; //0x0B54
	float spread178; //0x0B58
	float spread179; //0x0B5C
	float spread180; //0x0B60
	char pad_0B64[8]; //0x0B64
	float spread181; //0x0B6C
	float spread182; //0x0B70
	float spread183; //0x0B74
	float spread184; //0x0B78
	float spread185; //0x0B7C
	char pad_0B80[4]; //0x0B80
	float spread186; //0x0B84
	float spread187; //0x0B88
	char pad_0B8C[152]; //0x0B8C
	float spread188; //0x0C24
	float spread189; //0x0C28
	float spread190; //0x0C2C
	float spread191; //0x0C30
	float spread192; //0x0C34
	float spread193; //0x0C38
	char pad_0C3C[8]; //0x0C3C
	float spread194; //0x0C44
	float spread195; //0x0C48
	float spread196; //0x0C4C
	float spread197; //0x0C50
	float spread198; //0x0C54
	float spread199; //0x0C58
	float spread200; //0x0C5C
	float spread201; //0x0C60
	float spread202; //0x0C64
	float spread203; //0x0C68
	float spread204; //0x0C6C
	float spread205; //0x0C70
	float spread206; //0x0C74
	float spread207; //0x0C78
	float spread208; //0x0C7C
}; //Size: 0x0898