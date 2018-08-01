#include "wakawaka.h"

static const ::wak::STile tileDefinitions[] =
	{ { '0', "VOID"						, false,	}
	, { 'D', "CORNER_UP_LEFT"			, true,		}
	, { 'C', "CORNER_DOWN_LEFT"			, true,		}
	, { 'E', "CORNER_UP_RIGHT"			, true,		}
	, { 'F', "CORNER_DOWN_RIGHT"		, true,		}
	, { 'V', "VERTICAL"					, true,		}
	, { 'H', "HORIZONTAL"				, true,		}
	, { 'T', "T_DOWN_LEFT"				, true,		}
	, { 'U', "T_UP_RIGHT"				, true,		}
	, { 'R', "T_DOWN_RIGHT"				, true,		}
	, { 'L', "T_UP_LEFT"				, true,		}
	, { '4', "VERTICAL_LINE"			, true,		}
	, { 'P', "POINTS"					, false,	}
	, { 'W', "POWER_UP"					, false,	}
	, { 'A', "DOWN_HORIZONTAL"			, true,		}
	, { 'B', "VERTICAL_RIGHT"			, true,		}
	, { 'G', "INNER_CORNER_UP_RIGHT"	, true,		}
	, { 'I', "INNER_CORNER_UP_LEFT"		, true,		}
	, { 'J', "INNER_CORNER_DOWN_RIGHT"	, true,		}
	, { 'Q', "INNER_CORNER_DOWN_LEFT"	, true,		}
	, { 'K', "S_HORIZONTAL"				, true,		}
	, { 'M', "S_HORIZONTAL_DOWN"		, true,		}
	, { 'N', "S_VERTICAL"				, true,		}
	, { 'S', "S_VERTICAL_RIGHT"			, true,		}
	, { 'Y', "S_CORNER_DOWN_LEFT"		, true,		}
	, { 'Z', "S_CORNER_UP_LEFT"			, true,		}
	, { 'O', "S_CORNER_UP_RIGHT"		, true,		}
	, { '1', "S_CORNER_DOWN_RIGHT"		, true,		}
	, { '2', "T_RIGHT_UP"				, true,		}
	, { '3', "T_RIGHT_DOWN"				, true,		}
	, { '5', "T_LEFT_UP"				, true,		}
	, { '6', "T_LEFT_DOWN"				, true,		}
	};

unsigned char													 getImageByCode(unsigned char code) {
	for (uint32_t iTile = 0; iTile < (uint32_t)::gpk::size(tileDefinitions); ++iTile)
		if (tileDefinitions[iTile].Code == code)
			return (unsigned char)iTile;
	return 0;
}

void																					::wak::setupMap														(SGame& gameObject, const ::gpk::SImage<ubyte_t>& tileMap) {
	gameObject.Map.TilesMap			.resize(tileMap.metrics());
	gameObject.Map.TilesDecision	.resize(tileMap.metrics());
	gameObject.Map.TilesSolid		.resize(tileMap.metrics());

	gameObject.Map.Size		= tileMap.metrics().Cast<int32_t>();

	for (uint32_t y = 0; y < (uint32_t)gameObject.Map.Size.y; ++y) {
		for (uint32_t x = 0; x < (uint32_t)gameObject.Map.Size.x; ++x) {
			const uint8_t																	imageIndex								= getImageByCode(tileMap[y][x]);
			gameObject.Map.TilesMap[y][x]												= imageIndex;
			gameObject.Map.TilesDecision[y][x]											= false;
			gameObject.Map.TilesSolid[y][x]												= tileDefinitions[gameObject.Map.TilesMap[y][x]].Solid;
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
		SEnemy															newEnemy	= {};
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

		gameObject.Enemies.push_back(newEnemy);
	}
}

void																					::wak::setup														(SGame& gameObject, const ::gpk::SImage<ubyte_t>& tileMap) {
	setupMap(gameObject, tileMap);
	setupPlayer(gameObject);
	setupEnemies(gameObject);
}