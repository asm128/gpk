#pragma once
#include "gpk_frameinfo.h"
#include "gpk_image.h"

#define														AXIS_Y						36
#define														AXIS_X						30
#define														INVALID_ENEMY				0xFF
#define														TILE_VOID					0
#define														TILE_PELLET					250
#define														TILE_ENERGYZER				15
#define														TILE_FRUIT					3
namespace wak
{
	enum														DIRECTION 
		{ LEFT
		, UP
		, RIGHT
		, DOWN
		};

	enum														MODE {
		MODE_DISBAND,
		MODE_PURSUIT,
		MODE_ESCAPE
	};

	enum														GHOST {
		REDDY,
		ROSITA,
		DONJAIME,
		COOLGUY,
		GHOST_COUNT
	};

	struct														STile {
		unsigned char																		Image;
		unsigned char																		Code;
		::gpk::view_const_string															Name;
		bool																				Solid;
	};

	struct														SMap {
		::gpk::SCoord2<int32_t>																Size;
		unsigned char																		TilesMap[AXIS_Y][AXIS_X];
		unsigned char																		TilesEnemy[AXIS_Y][AXIS_X];
		bool																				TilesDecision[AXIS_Y][AXIS_X];
		bool																				TilesSolid[AXIS_Y][AXIS_X];
		::gpk::SCoord2<int32_t>																posToDraw;
	};

	struct														SAnimatedObject {
		DIRECTION																			CurrentDirection;
		::gpk::SCoord2<int32_t>																Position;
		float																				Speed;
		::gpk::SCoord2<float>																PositionDeltas;
		bool																				Stand				= false;
	};

	struct														SEnemy : public SAnimatedObject {
		GHOST																				CurrentGhost;
		MODE																				CurrentMode;
		MODE																				PrevtMode;
		::gpk::SCoord2<int32_t>																TargetTile;
		bool																				Vulnerability;
		bool																				InHouse;
		bool																				Reverse;
		bool																				AlreadyOut;
		bool																				Dead;
		::gpk::SCoord2<int32_t>																PrevTile;
	};

	struct														SGame {
		SMap																				Map;
		SAnimatedObject																		Player;
		::gpk::array_obj<SEnemy>															Enemies;

		unsigned int																		Score					= 0;
		unsigned int																		CounterPellet			= 0;
		unsigned int																		Lives					= 3;
		float																				CounterScatter			= 0;
		float																				CounterChase			= 0;
		float																				CounterFrightened		= 0;
		float																				CounterFruit			= 0;
		uint32_t																			CounterMode				= 0;

		double																				AnimationTimePacMan		= 0;
		double																				AnimationTimeOther		= 0;

		uint32_t																			CounterAnimation		= 0;
		uint32_t																			CounterAnimationPlayer	= 0;
		bool																				PacManStand				= true;
	};

	void																					setup															(SGame& gameObject, const ::gpk::SImage<ubyte_t>& tileMap);
	void																					setupMap														(SGame& gameObject, const ::gpk::SImage<ubyte_t>& tileMap);
	void																					setupPlayer														(SGame& gameObject);
	void																					setupEnemies													(SGame& gameObject);

	void																					updatePlayer													(SGame& gameObject, float fLastFrameTime);
	void																					updateEnemies													(SGame& gameObject, float fLastFrameTime);
	void																					update															(SGame& gameObject, float fLastFrameTime);
} 

