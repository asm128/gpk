#include "wakawaka.h"

#include<Windows.h>

void																	refreshPosFromDeltas										(::wak::SAnimatedObject& character) {
	::gpk::SCoord2<float>													&charDeltas		= character.PositionDeltas;
	int																		deltaX			= (int)charDeltas.x;
	character.Position.x																	+= deltaX;
	charDeltas.x																			-= deltaX;
	if (charDeltas.x < 0) {
		character.Position.x -= 1;
		charDeltas.x += 1;
	}
	int																		deltaY			= (int)charDeltas.y;
	character.Position.y																	+= deltaY;
	charDeltas.y																			-= deltaY;
	if (charDeltas.y < 0) {
		character.Position.y																-= 1;
		charDeltas.y																		+= 1;
	}
}

void																			lifeLost												(::wak::SGame & gameObject) { 
	::wak::setupPlayer(gameObject); 
	::wak::setupEnemies(gameObject); 
	for(uint32_t i=0; i < 4; ++i) {
		gameObject.Enemies[i].InHouse											= false;
		gameObject.Enemies[i].Position.y					= 14;
		gameObject.Enemies[i].Position.x					= 15;
	}
}
void																			ghostLost												(::wak::SGame & gameObject) { ::wak::setupEnemies(gameObject); }

void																			moveInDirection								(::wak::SGame & gameObject, ::wak::SAnimatedObject & character, float fLastFrameTime) {
	if (character.Position.x == 0) {
		if (character.CurrentDirection == ::wak::LEFT) {
			if (character.PositionDeltas.x < 0.5f) {
				character.Position.x = 29;
				character.PositionDeltas.x = 0.9f;
			}
			else
				character.PositionDeltas.x -= character.Speed * fLastFrameTime;
		}
		else
			character.PositionDeltas.x += character.Speed * fLastFrameTime;
	}
	else if (character.Position.x == 29) {
		if (character.CurrentDirection == ::wak::RIGHT) {
			if (character.PositionDeltas.x > 0.5f) {
				character.Position.x = 0;
				character.PositionDeltas.x = 0.1f;
			}
			else
				character.PositionDeltas.x += character.Speed * fLastFrameTime;
		}
		else
			character.PositionDeltas.x -= character.Speed * fLastFrameTime;
	}
	else if (character.CurrentDirection == ::wak::UP) {
		if (gameObject.Map.TilesSolid[character.Position.y - 1][character.Position.x] != true || gameObject.Map.TilesSolid[character.Position.y - 1][character.Position.x] == true && character.PositionDeltas.y > 0.5f)
			character.PositionDeltas.y			-= character.Speed * fLastFrameTime;
		else { character.PositionDeltas.y		= 0.5f;		character.Stand					= true; }
	}
	else if (character.CurrentDirection == ::wak::DOWN) {
		if (gameObject.Map.TilesSolid[character.Position.y + 1][character.Position.x] != true || gameObject.Map.TilesSolid[character.Position.y + 1][character.Position.x] == true && character.PositionDeltas.y < 0.5f)
			character.PositionDeltas.y			+= character.Speed * fLastFrameTime;
		else { character.PositionDeltas.y		= 0.5f;		character.Stand					= true;	}
	}
	else if (character.CurrentDirection == ::wak::RIGHT) {
		if (gameObject.Map.TilesSolid[character.Position.y][character.Position.x + 1] != true || gameObject.Map.TilesSolid[character.Position.y][character.Position.x + 1] == true && character.PositionDeltas.x < 0.5f)
			character.PositionDeltas.x			+= character.Speed * fLastFrameTime;
		else { character.PositionDeltas.x		= 0.5f;		character.Stand					= true;	}
	}
	else if (character.CurrentDirection == ::wak::LEFT) {
		if (gameObject.Map.TilesSolid[character.Position.y][character.Position.x - 1] != true || gameObject.Map.TilesSolid[character.Position.y][character.Position.x - 1] == true && character.PositionDeltas.x > 0.5f)
			character.PositionDeltas.x			-= character.Speed * fLastFrameTime;
		else { character.PositionDeltas.x		= 0.5f;		character.Stand					= true;	}
	}
}

void																			getDirectionByCollission										(::wak::SAnimatedObject &currentEnemy, ::wak::SGame & gameObject) {
	if (currentEnemy.CurrentDirection == ::wak::UP || currentEnemy.CurrentDirection == ::wak::DOWN) {
		if (gameObject.Map.TilesSolid[currentEnemy.Position.y][currentEnemy.Position.x - 1] == false)
			currentEnemy.CurrentDirection = ::wak::LEFT;
		else
			currentEnemy.CurrentDirection = ::wak::RIGHT;
	}
	else if (currentEnemy.CurrentDirection == ::wak::LEFT || currentEnemy.CurrentDirection == ::wak::RIGHT) {
		if (gameObject.Map.TilesSolid[currentEnemy.Position.y - 1][currentEnemy.Position.x] == false)
			currentEnemy.CurrentDirection = ::wak::UP;
		else
			currentEnemy.CurrentDirection = ::wak::DOWN;
	}
	currentEnemy.Stand = false;
}

void																	::wak::updatePlayer												(SGame& gameObject, float fLastFrameTime) {
	SAnimatedObject																& player						= gameObject.Player;
	SMap																		& map							= gameObject.Map;

	::gpk::SCoord2<float>														&playerDeltas					= gameObject.Player.PositionDeltas;
	
	player.Stand															= false;

	bool set = false;
		 if (gameObject.NextDirection == ::wak::RIGHT	&& player.Position.x != map.Size.x - 1	&& map.TilesSolid[player.Position.y][player.Position.x + 1] != true && playerDeltas.y > 0.3f && playerDeltas.y < 0.7f)															{ set = true; }
	else if (gameObject.NextDirection == ::wak::LEFT	&& player.Position.x != 0				&& map.TilesSolid[player.Position.y][player.Position.x - 1] != true && playerDeltas.y > 0.3f && playerDeltas.y < 0.7f)															{ set = true; }
	else if (gameObject.NextDirection == ::wak::UP		&& player.Position.y != 0				&& map.TilesSolid[player.Position.y - 1][player.Position.x] != true && playerDeltas.x > 0.3f && playerDeltas.x < 0.7f && player.Position.x != 0 && player.Position.x != 29)	{ set = true; }
	else if (gameObject.NextDirection == ::wak::DOWN	&& player.Position.y != map.Size.y - 1	&& map.TilesSolid[player.Position.y + 1][player.Position.x] != true && playerDeltas.x > 0.3f && playerDeltas.x < 0.7f && player.Position.x != 0 && player.Position.x != 29)	{ set = true; }

	if(set) {
		player.CurrentDirection = gameObject.NextDirection; 
		if(0 == player.CurrentDirection % 2)
			playerDeltas.y			= 0.5f; 
		else
			playerDeltas.x			= 0.5f; 
	}
	
	moveInDirection(gameObject, player, fLastFrameTime);

	refreshPosFromDeltas(player);

	if (gameObject.CounterPellet == 70 || gameObject.CounterPellet == 170) {
		gameObject.Map.TilesMap[20][15] = TILE_FRUIT;
		gameObject.CounterFruit += fLastFrameTime;
	}
	if(gameObject.CounterFruit > 0)
		gameObject.CounterFruit += fLastFrameTime;
	if (gameObject.CounterFruit >= 10) {
		gameObject.CounterFruit = 0;
		gameObject.Map.TilesMap[20][15] = TILE_VOID;
	}

	if (map.TilesMap[player.Position.y][player.Position.x] == TILE_PELLET && playerDeltas.y < 0.7f && playerDeltas.x < 0.7f && playerDeltas.y > 0.3f && playerDeltas.x > 0.3f) {
		gameObject.Score											+= 10;
		gameObject.CounterPellet									++;
		map.TilesMap[player.Position.y][player.Position.x]			= TILE_VOID;
	}
	else if (map.TilesMap[player.Position.y][player.Position.x] == TILE_FRUIT && playerDeltas.x < 0.3f && playerDeltas.x > 0) {
		gameObject.Map.TilesMap[player.Position.y][player.Position.x] = TILE_VOID;
		gameObject.CounterFruit = 0;
	}
	else if (map.TilesMap[player.Position.y][player.Position.x] == TILE_ENERGYZER && playerDeltas.y < 0.7f && playerDeltas.x < 0.7f && playerDeltas.y > 0.3f && playerDeltas.x > 0.3f) {
		gameObject.Score											+= 50;
		map.TilesMap[player.Position.y][player.Position.x]			= TILE_VOID;
		if (gameObject.CounterFrightened > 0)
			gameObject.CounterFrightened = 0.1f;
		else
			gameObject.CounterFrightened += fLastFrameTime;
		for (uint32_t i = 0; i < GHOST_COUNT; ++i) {
			if (gameObject.Enemies[i].InHouse == false && gameObject.Enemies[i].Dead == false) {
				gameObject.Enemies[i].CurrentMode	= MODE_ESCAPE;
				gameObject.Enemies[i].Reverse		= true;
			}
		}
	}
	for (uint32_t iEnemy = 0; iEnemy < GHOST_COUNT; ++iEnemy) {
		if (player.Position == gameObject.Enemies[iEnemy].Position && gameObject.Enemies[iEnemy].Dead == false) {
			float deltasX = playerDeltas.x - gameObject.Enemies[iEnemy].PositionDeltas.x;
			float deltasY = playerDeltas.y - gameObject.Enemies[iEnemy].PositionDeltas.y;
			float finalDelta = deltasY - deltasX;
			if (finalDelta > -0.3f && finalDelta < 0.3f) {
				if (gameObject.Enemies[iEnemy].CurrentMode == MODE_ESCAPE) {
					gameObject.Enemies[iEnemy].Dead = true;
					gameObject.Enemies[iEnemy].CurrentMode = gameObject.Enemies[iEnemy].PrevtMode;
				}
				else
					lifeLost(gameObject);
			}
		}
	}

	gameObject.AnimationTimePacMan		+= fLastFrameTime;
	if (player.Stand == false) {
		if(gameObject.AnimationTimePacMan > 0.05) {
			++gameObject.CounterAnimationPlayer;
			gameObject.AnimationTimePacMan	= 0;
		}
	}
}

void																	getDirectionByTarget (::gpk::SCoord2<int32_t> target, ::wak::SGame & gameObject, ::wak::SEnemy & ghost) {
	::gpk::SCoord2<int32_t>									anotherTarget;
	anotherTarget.y = uint32_t(target.y - ghost.Position.y);
	anotherTarget.x = uint32_t(target.x - ghost.Position.x);
	if (anotherTarget.y <= anotherTarget.x) {
		if (target.y < ghost.Position.y) {
			if (gameObject.Map.TilesSolid[ghost.Position.y - 1][ghost.Position.x] == false && ghost.CurrentDirection != ::wak::DOWN)
				ghost.CurrentDirection = ::wak::UP;
			else if (target.x <= ghost.Position.x && gameObject.Map.TilesSolid[ghost.Position.y][ghost.Position.x - 1] == false && ghost.CurrentDirection != ::wak::RIGHT)
				ghost.CurrentDirection = ::wak::LEFT;
			else if (target.x > ghost.Position.x && gameObject.Map.TilesSolid[ghost.Position.y][ghost.Position.x + 1] == false && ghost.CurrentDirection != ::wak::LEFT)
				ghost.CurrentDirection = ::wak::RIGHT;
		}
		else if (target.y == ghost.Position.y) {
			if (target.x < ghost.Position.x && ghost.CurrentDirection != ::wak::RIGHT && gameObject.Map.TilesSolid[ghost.Position.y][ghost.Position.x - 1] == false)
				ghost.CurrentDirection = ::wak::LEFT;
			else if(target.x > ghost.Position.x && ghost.CurrentDirection != ::wak::LEFT && gameObject.Map.TilesSolid[ghost.Position.y][ghost.Position.x + 1] == false)
				ghost.CurrentDirection = ::wak::RIGHT;
		}
		else {
			if (gameObject.Map.TilesSolid[ghost.Position.y + 1][ghost.Position.x] == false && ghost.CurrentDirection != ::wak::UP)
				ghost.CurrentDirection = ::wak::DOWN;
			else if (target.x <= ghost.Position.x && gameObject.Map.TilesSolid[ghost.Position.y][ghost.Position.x - 1] == false && ghost.CurrentDirection != ::wak::RIGHT)
				ghost.CurrentDirection = ::wak::LEFT;
			else if (target.x > ghost.Position.x && gameObject.Map.TilesSolid[ghost.Position.y][ghost.Position.x + 1] == false && ghost.CurrentDirection != ::wak::LEFT)
				ghost.CurrentDirection = ::wak::RIGHT;
		}
	}
	else {
		if (target.x <= ghost.Position.x) {
			if (gameObject.Map.TilesSolid[ghost.Position.y][ghost.Position.x - 1] == false && ghost.CurrentDirection != ::wak::RIGHT)
				ghost.CurrentDirection = ::wak::LEFT;
			else if (target.y <= ghost.Position.y && gameObject.Map.TilesSolid[ghost.Position.y - 1][ghost.Position.x] == false && ghost.CurrentDirection != ::wak::DOWN)
				ghost.CurrentDirection = ::wak::UP;
			else if (target.y > ghost.Position.y && gameObject.Map.TilesSolid[ghost.Position.y + 1][ghost.Position.x] == false && ghost.CurrentDirection != ::wak::UP)
				ghost.CurrentDirection = ::wak::DOWN;
		}
		else if (target.x == ghost.Position.x) {
			if (target.y < ghost.Position.y && ghost.CurrentDirection != ::wak::DOWN && gameObject.Map.TilesSolid[ghost.Position.y - 1][ghost.Position.x] == false)
				ghost.CurrentDirection = ::wak::UP;
			else if (target.y > ghost.Position.y && ghost.CurrentDirection != ::wak::UP && gameObject.Map.TilesSolid[ghost.Position.y + 1][ghost.Position.x] == false)
				ghost.CurrentDirection = ::wak::DOWN;
		}
		else {
			if (gameObject.Map.TilesSolid[ghost.Position.y][ghost.Position.x + 1] == false && ghost.CurrentDirection != ::wak::LEFT)
				ghost.CurrentDirection = ::wak::RIGHT;
			else if (target.y <= ghost.Position.y && gameObject.Map.TilesSolid[ghost.Position.y - 1][ghost.Position.x] == false && ghost.CurrentDirection != ::wak::DOWN)
				ghost.CurrentDirection = ::wak::UP;
			else if (target.y > ghost.Position.y && gameObject.Map.TilesSolid[ghost.Position.y + 1][ghost.Position.x] == false && ghost.CurrentDirection != ::wak::UP)
				ghost.CurrentDirection = ::wak::DOWN;
		}
	}
}

void																	getDirection														(::wak::SGame & gameObject, ::wak::SEnemy & ghost) {
	::gpk::SCoord2<int32_t>														targetTile															= {};
	if (ghost.CurrentMode == ::wak::MODE_ESCAPE && ghost.PrevTile != ghost.Position)
		ghost.CurrentDirection												= (::wak::DIRECTION)(rand() % 4);
	else if (ghost.CurrentMode == ::wak::MODE_DISBAND) {
		targetTile = ghost.TargetTile;
		getDirectionByTarget(targetTile, gameObject, ghost);
	}
	else {
		if (ghost.CurrentGhost == ::wak::REDDY) 
			targetTile = gameObject.Player.Position;
		else if (ghost.CurrentGhost == ::wak::ROSITA) {
			if (gameObject.Player.CurrentDirection == ::wak::RIGHT) {
				targetTile.y = gameObject.Player.Position.y;
				targetTile.x = gameObject.Player.Position.x + 4;
			}
			else if (gameObject.Player.CurrentDirection == ::wak::LEFT) {
				targetTile.y = gameObject.Player.Position.y;
				targetTile.x = gameObject.Player.Position.x - 4;
			}
			else if (gameObject.Player.CurrentDirection == ::wak::UP) {
				targetTile.y = gameObject.Player.Position.y - 4;
				targetTile.x = gameObject.Player.Position.x;
			}
			else {
				targetTile.y = gameObject.Player.Position.y + 4;
				targetTile.x = gameObject.Player.Position.x;
			}
		}
		else if (ghost.CurrentGhost == ::wak::DONJAIME) {
			targetTile.y = uint32_t((gameObject.Player.Position.y - gameObject.Enemies[::wak::REDDY].Position.y) + gameObject.Player.Position.y);
			targetTile.x = uint32_t((gameObject.Player.Position.x - gameObject.Enemies[::wak::REDDY].Position.x) + gameObject.Player.Position.x);
		}
		else 
			targetTile = gameObject.Player.Position;
		
		getDirectionByTarget(targetTile, gameObject, ghost);
	}
}
void																	reverse																(::wak::SAnimatedObject & ghost) {
	switch(ghost.CurrentDirection){
	case ::wak::UP		: ghost.CurrentDirection = ::wak::DOWN	;
	case ::wak::DOWN	: ghost.CurrentDirection = ::wak::UP	;
	case ::wak::LEFT	: ghost.CurrentDirection = ::wak::RIGHT	;
	case ::wak::RIGHT	: ghost.CurrentDirection = ::wak::LEFT	;
	}
}
void																	::wak::updateEnemies												(SGame& gameObject, float fLastFrameTime) {
	for (uint32_t y = 0; y < (uint32_t)gameObject.Map.Size.y; y++)
		memset(gameObject.Map.TilesEnemy[y].begin(), INVALID_ENEMY, gameObject.Map.Size.x);

	if		(gameObject.CounterPellet == 1)		gameObject.Enemies[ROSITA].AlreadyOut		= true;
	else if (gameObject.CounterPellet == 30)	gameObject.Enemies[DONJAIME].AlreadyOut		= true;
	else if (gameObject.CounterPellet == 80)	gameObject.Enemies[COOLGUY].AlreadyOut		= true;

	::gpk::SCoord2<int32_t>										modeCheck = {};
	modeCheck.y = uint32_t(gameObject.Player.Position.y - gameObject.Enemies[COOLGUY].Position.y);
	modeCheck.x = uint32_t(gameObject.Player.Position.x - gameObject.Enemies[COOLGUY].Position.x);
	if (gameObject.Enemies[COOLGUY].CurrentMode != MODE_ESCAPE) {
		if (modeCheck.y < 8 && modeCheck.x < 8) {
			gameObject.Enemies[COOLGUY].CurrentMode				= MODE_DISBAND;
			gameObject.Enemies[COOLGUY].PrevtMode				= MODE_DISBAND;
		}
		else {
			gameObject.Enemies[COOLGUY].CurrentMode				= MODE_PURSUIT;
			gameObject.Enemies[COOLGUY].PrevtMode				= MODE_PURSUIT;
		}
	}
	if (gameObject.CounterFrightened == 0 && gameObject.CounterMode != 4) {
		if (gameObject.CounterChase == 0) {
			gameObject.CounterScatter += fLastFrameTime;
			if (gameObject.CounterScatter >= 7) {
				gameObject.CounterChase += fLastFrameTime;
				gameObject.CounterScatter = 0;
				gameObject.CounterMode++;
				for (uint32_t iEnemy = 0; iEnemy < GHOST_COUNT - 1; ++iEnemy) {
					gameObject.Enemies[iEnemy].CurrentMode	= 
					gameObject.Enemies[iEnemy].PrevtMode	= MODE_PURSUIT;
					gameObject.Enemies[iEnemy].Reverse		= true;
				}
			}
		}
		else {
			gameObject.CounterChase += fLastFrameTime;
			if (gameObject.CounterChase >= 20) {
				gameObject.CounterChase = 0;
				for (uint32_t iEnemy = 0; iEnemy < GHOST_COUNT - 1; ++iEnemy) {
					gameObject.Enemies[iEnemy].CurrentMode	= 
					gameObject.Enemies[iEnemy].PrevtMode	= MODE_DISBAND;
					gameObject.Enemies[iEnemy].Reverse		= true;
				}
			}
		}
	}
	else {
		gameObject.CounterFrightened += fLastFrameTime;
		if (gameObject.CounterFrightened >= 7) {
			gameObject.CounterFrightened = 0;
			for (uint32_t iEnemy = 0; iEnemy < GHOST_COUNT; ++iEnemy)
				gameObject.Enemies[iEnemy].CurrentMode = gameObject.Enemies[iEnemy].PrevtMode;
		}
	}

	for (uint32_t enemyC = 0; enemyC < ::wak::GHOST_COUNT; ++enemyC) {
		::wak::SEnemy								& currentEnemy = gameObject.Enemies[enemyC];

		if (currentEnemy.Dead == true)
			currentEnemy.Speed = 9.0f;
		else if (currentEnemy.CurrentMode == MODE_ESCAPE || currentEnemy.Position.x < 10 && currentEnemy.Position.y == 17 || currentEnemy.Position.x > 19 && currentEnemy.Position.y == 17)
			currentEnemy.Speed = 3.0f;
		else if (currentEnemy.CurrentGhost == ::wak::REDDY && currentEnemy.CurrentMode == ::wak::MODE_PURSUIT)
			currentEnemy.Speed = 4.5f;
		else
			currentEnemy.Speed = 4.0f;

		if (currentEnemy.Stand == true)
			getDirectionByCollission(currentEnemy, gameObject);
		else if (currentEnemy.Reverse == true) {
			reverse(currentEnemy);
			currentEnemy.Reverse = false;
		}
		else if (gameObject.Map.TilesDecision[currentEnemy.Position.y][currentEnemy.Position.x] == true && currentEnemy.PositionDeltas.y > 0.3f && currentEnemy.PositionDeltas.x > 0.3f && currentEnemy.PositionDeltas.y < 0.7f && currentEnemy.PositionDeltas.x < 0.7f /*&& currentEnemy.PrevTile != currentEnemy.Position*/) {
			if (currentEnemy.Dead == true) {
				getDirectionByTarget({ 15, 15 }, gameObject, currentEnemy);
			}
			else if (currentEnemy.Position.y == 14 && currentEnemy.Position.x == 13 || currentEnemy.Position.y == 14 && currentEnemy.Position.x == 16 || currentEnemy.Position.y == 26 && currentEnemy.Position.x == 13 || currentEnemy.Position.y == 26 && currentEnemy.Position.x == 16) {
				if (currentEnemy.CurrentDirection == ::wak::DOWN)
					getDirection(gameObject, currentEnemy);
			}
			else
				getDirection(gameObject, currentEnemy);
			currentEnemy.PrevTile = currentEnemy.Position;
		}

		if (gameObject.Player.Position == currentEnemy.Position && currentEnemy.Dead == false) {
			float deltasX = gameObject.Player.PositionDeltas.x - currentEnemy.PositionDeltas.x;
			float deltasY = gameObject.Player.PositionDeltas.y - currentEnemy.PositionDeltas.y;
			float finalDelta = deltasY - deltasX;
			if (finalDelta > -0.3f && finalDelta < 0.3f) {
				if (currentEnemy.CurrentMode == MODE_ESCAPE) {
					currentEnemy.Dead = true;
					currentEnemy.CurrentMode = currentEnemy.PrevtMode;
				}
				else
					lifeLost(gameObject);
			}
		}
		if (currentEnemy.Dead == true) {
			if (currentEnemy.Position.y == 14 && currentEnemy.Position.x == 15 /*&& currentEnemy.PositionDeltas.x < 0.3f*/) {
				currentEnemy.Position.y				= 15;
				currentEnemy.PositionDeltas.x		= 0;
				currentEnemy.PositionDeltas.y		= 0;
				currentEnemy.CurrentDirection = ::wak::DOWN;
			}
			else if (currentEnemy.Position.y == 15 && currentEnemy.Position.x == 15) {
				if (currentEnemy.PositionDeltas.y >= 0.9f) {
					currentEnemy.Position.y			= 16;
					currentEnemy.PositionDeltas.y	= 0;
					currentEnemy.Dead				= false;
					currentEnemy.InHouse			= true;
				}
				else
					currentEnemy.PositionDeltas.y += 0.9f * fLastFrameTime;
			}
			else
				moveInDirection(gameObject, currentEnemy, fLastFrameTime);
		}
		else if (currentEnemy.AlreadyOut == true && currentEnemy.InHouse == true) {
			if (currentEnemy.Position.y == 16 && currentEnemy.Position.x == 15) {
				currentEnemy.Position.y				= 15;
				currentEnemy.PositionDeltas.x		= 0;
				currentEnemy.PositionDeltas.y		= 0.9f;
				currentEnemy.CurrentDirection = ::wak::UP;
			}
			else if (currentEnemy.Position.y == 15 && currentEnemy.Position.x == 15) {
				if (currentEnemy.PositionDeltas.y <= 0.1f) {
					currentEnemy.Position.y			= 14;
					currentEnemy.PositionDeltas.y	= 0.9f;
					currentEnemy.InHouse			= false;
					currentEnemy.Reverse			= true;
				}
				else
					currentEnemy.PositionDeltas.y -= 0.9f * fLastFrameTime;
			}
			else
				moveInDirection(gameObject, currentEnemy, fLastFrameTime);
		}
		else
			moveInDirection(gameObject, currentEnemy, fLastFrameTime);

		refreshPosFromDeltas(currentEnemy);

		gameObject.Map.TilesEnemy[currentEnemy.Position.y][currentEnemy.Position.x] = 1;
	}
}
void																					::wak::update															(SGame& gameObject, float fLastFrameTime) {
	updatePlayer(gameObject, fLastFrameTime);
	updateEnemies(gameObject, fLastFrameTime);

	gameObject.AnimationTimeOther		+= fLastFrameTime;
	if(gameObject.AnimationTimeOther		 > 0.05) {
		++gameObject.CounterAnimation;
		gameObject.AnimationTimeOther	= 0;
	}

}