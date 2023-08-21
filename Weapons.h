#pragma once
enum WeaponIndex {
	WEAPON_DEAGLE = 1,
	WEAPON_ELITE = 2,
	WEAPON_FIVESEVEN = 3,
	WEAPON_GLOCK = 4,
	WEAPON_AK47 = 7,
	WEAPON_AUG = 8,
	WEAPON_AWP = 9,
	WEAPON_FAMAS = 10,
	WEAPON_G3SG1 = 11,
	WEAPON_GALILAR = 13,
	WEAPON_M249 = 14,
	WEAPON_M4A1 = 16,
	WEAPON_MAC10 = 17,
	WEAPON_P90 = 19,
	WEAPON_UMP45 = 24,
	WEAPON_XM1014 = 25,
	WEAPON_BIZON = 26,
	WEAPON_MAG7 = 27,
	WEAPON_NEGEV = 28,
	WEAPON_SAWEDOFF = 29,
	WEAPON_TEC9 = 30,
	WEAPON_TASER = 31,
	WEAPON_HKP2000 = 32,
	WEAPON_MP7 = 33,
	WEAPON_MP9 = 34,
	WEAPON_NOVA = 35,
	WEAPON_P250 = 36,
	WEAPON_SCAR20 = 38,
	WEAPON_SG556 = 39,
	WEAPON_SSG08 = 40,
	WEAPON_KNIFE = 42,
	WEAPON_FLASHBANG = 43,
	WEAPON_HEGRENADE = 44,
	WEAPON_SMOKEGRENADE = 45,
	WEAPON_MOLOTOV = 46,
	WEAPON_DECOY = 47,
	WEAPON_INCGRENADE = 48,
	WEAPON_C4 = 49,
	WEAPON_KNIFE_T = 59,
	WEAPON_M4A1_SILENCER = 60,
	WEAPON_USP_SILENCER = 61,
	WEAPON_CZ75A = 63,
	WEAPON_REVOLVER = 64,
	WEAPON_KNIFE_BAYONET = 500,
	WEAPON_KNIFE_FLIP = 505,
	WEAPON_KNIFE_GUT = 506,
	WEAPON_KNIFE_KARAMBIT = 507,
	WEAPON_KNIFE_M9_BAYONET = 508,
	WEAPON_KNIFE_TACTICAL = 509,
	WEAPON_KNIFE_FALCHION = 512,
	WEAPON_KNIFE_SURVIVAL_BOWIE = 514,
	WEAPON_KNIFE_BUTTERFLY = 515,
	WEAPON_KNIFE_PUSH = 516
};

const char* getWeaponName(enum WeaponIndex weaponId)
{
	switch (weaponId) {
	case WEAPON_DEAGLE: return "Weapon: DEAGLE";
	case WEAPON_ELITE: return "Weapon: ELITE";
	case WEAPON_FIVESEVEN: return "Weapon: FIVESEVEN";
	case WEAPON_GLOCK: return "Weapon: GLOCK";
	case WEAPON_AK47: return "Weapon: AK47";
	case WEAPON_AUG: return "Weapon: AUG";
	case WEAPON_AWP: return "Weapon: AWP";
	case WEAPON_FAMAS: return "Weapon: FAMAS";
	case WEAPON_G3SG1: return "Weapon: G3SG1";
	case WEAPON_GALILAR: return "Weapon: GALILAR";
	case WEAPON_M249: return "Weapon: M249";
	case WEAPON_M4A1: return "Weapon: M4A1";
	case WEAPON_MAC10: return "Weapon: MAC10";
	case WEAPON_P90: return "Weapon: P90";
	case WEAPON_UMP45: return "Weapon: UMP45";
	case WEAPON_XM1014: return "Weapon: XM1014";
	case WEAPON_BIZON: return "Weapon: BIZON";
	case WEAPON_MAG7: return "Weapon: MAG7";
	case WEAPON_NEGEV: return "Weapon: NEGEV";
	case WEAPON_SAWEDOFF: return "Weapon: SAWEDOFF";
	case WEAPON_TEC9: return "Weapon: TEC9";
	case WEAPON_TASER: return "Weapon: TASER";
	case WEAPON_HKP2000: return "Weapon: P2000";
	case WEAPON_MP7: return "Weapon: MP7";
	case WEAPON_MP9: return "Weapon: MP9";
	case WEAPON_NOVA: return "Weapon: NOVA";
	case WEAPON_P250: return "Weapon: P250";
	case WEAPON_SCAR20: return "Weapon: SCAR20";
	case WEAPON_SG556: return "Weapon: SG556";
	case WEAPON_SSG08: return "Weapon: SSG08";
	case WEAPON_KNIFE: return "Weapon: KNIFE";
	case WEAPON_FLASHBANG: return "Weapon: FLASHBANG";
	case WEAPON_HEGRENADE: return "Weapon: HEGRENADE";
	case WEAPON_SMOKEGRENADE: return "Weapon: SMOKEGRENADE";
	case WEAPON_MOLOTOV: return "Weapon: MOLOTOV";
	case WEAPON_DECOY: return "Weapon: DECOY";
	case WEAPON_INCGRENADE: return "Weapon: INCGRENADE";
	case WEAPON_C4: return "Weapon: C4";
	case WEAPON_KNIFE_T: return "Weapon: KNIFE_T";
	case WEAPON_M4A1_SILENCER: return "Weapon: M4A1_SILENCER";
	case WEAPON_USP_SILENCER: return "Weapon: USP_SILENCER";
	case WEAPON_CZ75A: return "Weapon: CZ75A";
	case WEAPON_REVOLVER: return "Weapon: REVOLVER";
	case WEAPON_KNIFE_BAYONET: return "Weapon: KNIFE_BAYONET";
	case WEAPON_KNIFE_FLIP: return "Weapon: KNIFE_FLIP";
	case WEAPON_KNIFE_GUT: return "Weapon: KNIFE_GUT";
	case WEAPON_KNIFE_KARAMBIT: return "Weapon: KNIFE_KARAMBIT";
	case WEAPON_KNIFE_M9_BAYONET: return "Weapon: KNIFE_M9_BAYONET";
	case WEAPON_KNIFE_TACTICAL: return "Weapon: KNIFE_TACTICAL";
	case WEAPON_KNIFE_FALCHION: return "Weapon: KNIFE_FALCHION";
	case WEAPON_KNIFE_SURVIVAL_BOWIE: return "Weapon: KNIFE_SURVIVAL_BOWIE";
	case WEAPON_KNIFE_BUTTERFLY: return "Weapon: KNIFE_BUTTERFLY";
	case WEAPON_KNIFE_PUSH: return "Weapon: KNIFE_PUSH";
	default: return "Weapon: NULL";
	}
}