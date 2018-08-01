#pragma once
#include "gpk_frameinfo.h"
#include "gpk_image.h"

#define														AXIS_Y							36
#define														AXIS_X							30

namespace wak
{

	enum MAP_TILE 
		{	MAP_VOID
		,	MAP_UP_LEFT
		,	MAP_DOWN_LEFT
		,	MAP_UP_RIGHT
		,	MAP_DOWN_RIGHT
		,	MAP_VERTICAL
		,	MAP_HORIZONTAL
		,	MAP_T_DOWN_LEFT
		,	MAP_T_UP_RIGHT
		,	MAP_T_DOWN_RIGHT
		,	MAP_T_UP_LEFT
		,	MAP_LINE
		,	MAP_PELLET
		,	MAP_ENERGYZER
		,	MAP_DOWN_HORIZONTAL
		,	MAP_VERTICAL_RIGHT
		,	MAP_INNER_CORNER_UP_RIGHT
		,	MAP_INNER_CORNER_UP_LEFT
		,	MAP_INNER_CORNER_DOWN_RIGHT
		,	MAP_INNER_CORNER_DOWN_LEFT
		,	MAP_S_HORIZONTAL
		,	MAP_S_HORIZONTAL_DOWN
		,	MAP_S_VERTICAL
		,	MAP_S_VERTICAL_RIGHT
		,	MAP_S_CORNER_DOWN_LEFT
		,	MAP_S_CORNER_UP_LEFT
		,	MAP_S_CORNER_UP_RIGHT
		,	MAP_S_CORNER_DOWN_RIGHT
		,	MAP_T_RIGHT_UP
		,	MAP_T_RIGHT_DOWN
		,	MAP_T_LEFT_UP
		,	MAP_T_LEFT_DOWN
		,	TEXTURE_COUNT
		};	
	
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
		unsigned char																		Code;
		::gpk::view_const_string															Name;
		bool																				Solid;
	};

	struct														SMap {
		::gpk::SCoord2<int32_t>																Size;
		::gpk::SImage<unsigned char>														TilesMap		;
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
		bool																				Fruit	= false;
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

