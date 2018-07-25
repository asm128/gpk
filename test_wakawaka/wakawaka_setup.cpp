#include "wakawaka.h"

static const ::wak::STile tileDefinitions[] =
{ { ' '		, '0', "VOID"					, false,	}
,{ 250		, 'P', "POINTS"					, false,	}
,{ 15		, 'W', "POWER_UP"				, false,	}
,{ 3		, 'X', "TARGET_FRUIT"			, false,	}
,{ 179		, '4', "VERTICAL_LINE"			, true,		}
,{ 200		, 'C', "CORNER_DOWN_LEFT"		, true,		}
,{ 201		, 'D', "CORNER_UP_LEFT"			, true,		}
,{ 187		, 'E', "CORNER_UP_RIGHT"		, true,		}
,{ 188		, 'F', "CORNER_DOWN_RIGHT"		, true,		}
,{ 203		, 'T', "T_DOWN_LEFT"			, true,		}
,{ 202		, 'U', "T_UP_RIGHT"				, true,		}
,{ 204		, 'R', "T_DOWN_RIGHT"			, true,		}
,{ 185		, 'L', "T_UP_LEFT"				, true,		}
,{ 205		, 'H', "HORIZONTAL"				, true,		}
,{ 186		, 'V', "VERTICAL"				, true,		}
,{ 206		, 'A', "DOWN_HORIZONTAL"		, true,		}
,{ 180		, 'B', "VERTICAL_RIGHT"			, true,		}
,{ 181		, 'G', "INNER_CORNER_UP_RIGHT"	, true,		}
,{ 182		, 'I', "INNER_CORNER_UP_LEFT"	, true,		}
,{ 183		, 'J', "INNER_CORNER_DOWN_RIGHT", true,		}
,{ 184		, 'Q', "INNER_CORNER_DOWN_LEFT"	, true,		}
,{ 190		, 'K', "S_HORIZONTAL"			, true,		}
,{ 191		, 'M', "S_HORIZONTAL_DOWN"		, true,		}
,{ 192		, 'N', "S_VERTICAL"				, true,		}
,{ 193		, 'S', "S_VERTICAL_RIGHT"		, true,		}
,{ 194		, 'Y', "S_CORNER_DOWN_LEFT"		, true,		}
,{ 195		, 'Z', "S_CORNER_UP_LEFT"		, true,		}
,{ 196		, 'O', "S_CORNER_UP_RIGHT"		, true,		}
,{ 197		, '1', "S_CORNER_DOWN_RIGHT"	, true,		}
,{ 100		, '2', "T_RIGHT_UP"				, true,		}
,{ 101		, '3', "T_RIGHT_DOWN"			, true,		}
,{ 102		, '5', "T_LEFT_UP"				, true,		}
,{ 103		, '6', "T_LEFT_DOWN"			, true,		}
};

unsigned char													 getImageByCode(unsigned char code) {
	for (uint32_t iTile = 0; iTile < (uint32_t)::gpk::size(tileDefinitions); ++iTile)
		if (tileDefinitions[iTile].Code == code)
			return tileDefinitions[iTile].Image;
	return ' ';
}

bool															 getSolidByImage(unsigned char image) {
	for (uint32_t iTile = 0; iTile < (uint32_t)::gpk::size(tileDefinitions); ++iTile)
		if (tileDefinitions[iTile].Image == image)
			return tileDefinitions[iTile].Solid;
	return true;
}

void																					::wak::setupMap														(SGame& gameObject, const ::gpk::SImage<ubyte_t>& tileMap) {
	gameObject.Map.TilesMap			.resize(tileMap.metrics());
	gameObject.Map.TilesEnemy		.resize(tileMap.metrics());
	gameObject.Map.TilesDecision	.resize(tileMap.metrics());
	gameObject.Map.TilesSolid		.resize(tileMap.metrics());

	gameObject.Map.Size		= tileMap.metrics().Cast<int32_t>();

	for (uint32_t y = 0; y < (uint32_t)gameObject.Map.Size.y; ++y) {
		for (uint32_t x = 0; x < (uint32_t)gameObject.Map.Size.x; ++x) {
			gameObject.Map.TilesMap[y][x]												= getImageByCode(tileMap[y][x]);
			gameObject.Map.TilesEnemy[y][x]												= (unsigned char)INVALID_ENEMY;
			gameObject.Map.TilesDecision[y][x]											= false;
			gameObject.Map.TilesSolid[y][x]												= getSolidByImage(gameObject.Map.TilesMap[y][x]);
		}
	}
	gameObject.Map.TilesDecision[4][7]		= gameObject.Map.TilesDecision[4][22]	= gameObject.Map.TilesDecision[8][2]	= gameObject.Map.TilesDecision[8][7]	= gameObject.Map.TilesDecision[8][10] =
	gameObject.Map.TilesDecision[8][13]		= gameObject.Map.TilesDecision[8][16]	= gameObject.Map.TilesDecision[8][19]	= gameObject.Map.TilesDecision[8][22]	= gameObject.Map.TilesDecision[8][27] =
	gameObject.Map.TilesDecision[11][7]		= gameObject.Map.TilesDecision[11][22]	= gameObject.Map.TilesDecision[14][13]	= gameObject.Map.TilesDecision[14][16]	= gameObject.Map.TilesDecision[17][7] =
	gameObject.Map.TilesDecision[17][10]	= gameObject.Map.TilesDecision[17][19]	= gameObject.Map.TilesDecision[17][22]	= gameObject.Map.TilesDecision[20][10]	= gameObject.Map.TilesDecision[20][19] =
	gameObject.Map.TilesDecision[23][7]		= gameObject.Map.TilesDecision[23][10]	= gameObject.Map.TilesDecision[23][19]	= gameObject.Map.TilesDecision[23][22]	= gameObject.Map.TilesDecision[26][7] =
	gameObject.Map.TilesDecision[26][10]	= gameObject.Map.TilesDecision[26][13]	= gameObject.Map.TilesDecision[26][16]	= gameObject.Map.TilesDecision[26][19]	= gameObject.Map.TilesDecision[26][22] =
	gameObject.Map.TilesDecision[29][4]		= gameObject.Map.TilesDecision[29][25]	= gameObject.Map.TilesDecision[32][13]	= gameObject.Map.TilesDecision[32][16]	= true;
}

void																					::wak::setupPlayer														(SGame& gameObject) {
	gameObject.Player.CurrentDirection								= DOWN;
	gameObject.Player.Position.y									= 26;
	gameObject.Player.Position.x									= 15;
	gameObject.Player.PositionDeltas								= { 0, 0.5f };
	gameObject.Player.Speed											= 4.5f;
}

void																					::wak::setupEnemies														(SGame& gameObject) {
	gameObject.Enemies.clear();
	for (uint32_t enemyCount = 0; enemyCount < GHOST_COUNT; enemyCount++) {
		SEnemy														newEnemy;
		newEnemy.CurrentMode										= MODE_DISBAND;
		newEnemy.PrevtMode											= MODE_DISBAND;
		newEnemy.PositionDeltas										= { 0, 0.5f };
		newEnemy.Speed												= 4.0f;
		newEnemy.Reverse											=
		newEnemy.Dead												=
		newEnemy.AlreadyOut											= false;
		newEnemy.InHouse											= true;

		if (enemyCount == REDDY) {
			newEnemy.Position.x											= 15;
			newEnemy.Position.y											= 14;
			newEnemy.CurrentDirection									= LEFT;
			newEnemy.CurrentGhost										= REDDY;
			newEnemy.InHouse											= false;
			newEnemy.TargetTile.y										= 0;
			newEnemy.TargetTile.x										= 26;
			newEnemy.AlreadyOut											=
			newEnemy.Reverse											= true;
		}
		else if (enemyCount == ROSITA) {
			newEnemy.Position.x											= 15;
			newEnemy.Position.y											= 17;
			newEnemy.CurrentDirection									= RIGHT;
			newEnemy.CurrentGhost										= ROSITA;
			newEnemy.TargetTile.y										= 0;
			newEnemy.TargetTile.x										= 3;
		}
		else if (enemyCount == DONJAIME) {
			newEnemy.Position.x											= 13;
			newEnemy.Position.y											= 17;
			newEnemy.CurrentDirection									= DOWN;
			newEnemy.CurrentGhost										= DONJAIME;
			newEnemy.TargetTile.y										= 35;
			newEnemy.TargetTile.x										= 28;
		}
		else if (enemyCount == COOLGUY) {
			newEnemy.Position.x											= 17;
			newEnemy.Position.y											= 17;
			newEnemy.CurrentDirection									= UP;
			newEnemy.CurrentGhost										= COOLGUY;
			newEnemy.TargetTile.y										= 35;
			newEnemy.TargetTile.x										= 1;
		}

		gameObject.Map.TilesEnemy[newEnemy.Position.y][newEnemy.Position.x] = 1;
		gameObject.Enemies.push_back(newEnemy);
	}
}

void																					::wak::setup														(SGame& gameObject, const ::gpk::SImage<ubyte_t>& tileMap) {
	setupMap(gameObject, tileMap);
	setupPlayer(gameObject);
	setupEnemies(gameObject);
}