#pragma once
#include "gpk_frameinfo.h"
#include "gpk_image.h"

#define														AXIS_Y							36
#define														AXIS_X							30
#define														INVALID_ENEMY					0xFF
#define														TILE_VOID						0
#define														TILE_PELLET						250
#define														TILE_ENERGYZER					15
#define														TILE_FRUIT						3
#define														TILE_VERTICAL_LINE				179
#define														TILE_DOWN_LEFT					200
#define														TILE_UP_LEFT					201
#define														TILE_UP_RIGHT					187
#define														TILE_DOWN_RIGHT					188
#define														TILE_T_DOWN_LEFT				203
#define														TILE_T_UP_RIGHT					202
#define														TILE_T_DOWN_RIGHT				204
#define														TILE_T_UP_LEFT					185
#define														TILE_HORIZONTAL					205
#define														TILE_VERTICAL					186
#define														TILE_DOWN_HORIZONTAL			206
#define														TILE_VERTICAL_RIGHT				180
#define									 					TILE_INNER_CORNER_UP_RIGHT		181
#define														TILE_INNER_CORNER_UP_LEFT		182
#define														TILE_INNER_CORNER_DOWN_RIGHT	183
#define														TILE_INNER_CORNER_DOWN_LEFT		184
#define														TILE_S_HORIZONTAL				190
#define														TILE_S_HORIZONTAL_DOWN			191
#define														TILE_S_VERTICAL					192
#define														TILE_S_VERTICAL_RIGHT			193
#define														TILE_S_CORNER_DOWN_LEFT			194
#define														TILE_S_CORNER_UP_LEFT			195
#define														TILE_S_CORNER_UP_RIGHT			196
#define														TILE_S_CORNER_DOWN_RIGHT		197
#define														TILE_T_RIGHT_UP					100
#define														TILE_T_RIGHT_DOWN				101
#define														TILE_T_LEFT_UP					102
#define														TILE_T_LEFT_DOWN				103

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
		::gpk::SImage<unsigned char>														TilesMap		;
		::gpk::SImage<unsigned char>														TilesEnemy		;
		::gpk::SImage<bool		   >														TilesDecision	;
		::gpk::SImage<bool		   >														TilesSolid		;
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

		DIRECTION																			NextDirection;



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

