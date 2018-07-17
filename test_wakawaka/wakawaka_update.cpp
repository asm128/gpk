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
	if (character.CurrentDirection == ::wak::UP) {
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

		 if (GetAsyncKeyState('D')	&&	map.TilesSolid[player.Position.y][player.Position.x + 1] != true && playerDeltas.y > 0.4f && playerDeltas.y < 0.6f) { player.CurrentDirection = RIGHT; playerDeltas.y = 0.5f; }
	else if (GetAsyncKeyState('A')	&&	map.TilesSolid[player.Position.y][player.Position.x - 1] != true && playerDeltas.y > 0.4f && playerDeltas.y < 0.6f) { player.CurrentDirection = LEFT ; playerDeltas.y = 0.5f; }
	else if (GetAsyncKeyState('W')	&&	map.TilesSolid[player.Position.y - 1][player.Position.x] != true && playerDeltas.x > 0.4f && playerDeltas.x < 0.6f && player.Position.x != 0 && player.Position.x != 29)	{player.CurrentDirection = UP;		playerDeltas.x = 0.5f;}
	else if (GetAsyncKeyState('S')	&&	map.TilesSolid[player.Position.y + 1][player.Position.x] != true && playerDeltas.x > 0.4f && playerDeltas.x < 0.6f && player.Position.x != 0 && player.Position.x != 29)	{player.CurrentDirection = DOWN;	playerDeltas.x = 0.5f;}

	if		( player.Position.x ==  0 && playerDeltas.x < 0.1f)	{ player.Position.x = 29; playerDeltas.x = 0.9f;}
	else if ( player.Position.x == 29 && playerDeltas.x > 0.9f)	{ player.Position.x =  0; playerDeltas.x = 0.1f;}

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

	if (map.TilesMap[player.Position.y][player.Position.x] == TILE_PELLET && playerDeltas.y < 0.6f && playerDeltas.x < 0.6f && playerDeltas.y > 0.4f && playerDeltas.x > 0.4f) {
		gameObject.Score											+= 10;
		gameObject.CounterPellet									++;
		map.TilesMap[player.Position.y][player.Position.x]			= TILE_VOID;
	}
	else if (map.TilesMap[player.Position.y][player.Position.x] == TILE_FRUIT && playerDeltas.x < 0.3f && playerDeltas.x > 0) {
		gameObject.Map.TilesMap[player.Position.y][player.Position.x] = TILE_VOID;
		gameObject.CounterFruit = 0;
	}
	else if (map.TilesMap[player.Position.y][player.Position.x] == TILE_ENERGYZER && playerDeltas.y < 0.6f && playerDeltas.x < 0.6f && playerDeltas.y > 0.4f && playerDeltas.x > 0.4f) {
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
				gameObject.Enemies[i].Vulnerability = true;
				gameObject.Enemies[i].Speed			= 3.0f;
			}
		}
	}
	for (uint32_t iEnemy = 0; iEnemy < GHOST_COUNT; ++iEnemy) {
		if (player.Position == gameObject.Enemies[iEnemy].Position && gameObject.Enemies[iEnemy].Dead == false) {
			float deltasX = playerDeltas.x - gameObject.Enemies[iEnemy].PositionDeltas.x;
			float deltasY = playerDeltas.y - gameObject.Enemies[iEnemy].PositionDeltas.y;
			float finalDelta = deltasY - deltasX;
			if (finalDelta > -0.3f && finalDelta < 0.3f) {
				if (gameObject.Enemies[iEnemy].Vulnerability == true) {
					gameObject.Enemies[iEnemy].Dead = true;
					gameObject.Enemies[iEnemy].Speed *= 2;
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
	if (ghost.CurrentMode == ::wak::MODE_ESCAPE || ghost.InHouse == true)
		ghost.CurrentDirection												= (::wak::DIRECTION)(rand() % 4);
	else if (ghost.CurrentMode == ::wak::MODE_SCATTER) {
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
		memset(gameObject.Map.TilesEnemy[y], INVALID_ENEMY, gameObject.Map.Size.x);

	if (gameObject.CounterPellet == 1) {
		gameObject.Enemies[ROSITA].Position.y = 14;
		gameObject.Enemies[ROSITA].Position.x = 15;
		gameObject.Enemies[ROSITA].InHouse = false;
		gameObject.Enemies[ROSITA].CurrentDirection = LEFT;
		gameObject.Enemies[ROSITA].AlreadyOut =
			gameObject.Enemies[ROSITA].Reverse = true;
	}
	else if (gameObject.CounterPellet == 30) {
		gameObject.Enemies[DONJAIME].Position.y = 14;
		gameObject.Enemies[DONJAIME].Position.x = 15;
		gameObject.Enemies[DONJAIME].InHouse = false;
		gameObject.Enemies[DONJAIME].CurrentDirection = LEFT;
		gameObject.Enemies[DONJAIME].AlreadyOut =
			gameObject.Enemies[DONJAIME].Reverse = true;
	}
	else if (gameObject.CounterPellet == 80) {
		gameObject.Enemies[COOLGUY].Position.y = 14;
		gameObject.Enemies[COOLGUY].Position.x = 15;
		gameObject.Enemies[COOLGUY].InHouse = false;
		gameObject.Enemies[COOLGUY].CurrentDirection = LEFT;
		gameObject.Enemies[COOLGUY].AlreadyOut =
			gameObject.Enemies[COOLGUY].Reverse = true;
	}

	::gpk::SCoord2<int32_t>										modeCheck = {};
	modeCheck.y = uint32_t(gameObject.Player.Position.y - gameObject.Enemies[COOLGUY].Position.y);
	modeCheck.x = uint32_t(gameObject.Player.Position.x - gameObject.Enemies[COOLGUY].Position.x);
	if (gameObject.Enemies[COOLGUY].CurrentMode != MODE_ESCAPE) {
		if (modeCheck.y < 8 && modeCheck.x < 8) {
			gameObject.Enemies[COOLGUY].CurrentMode = MODE_SCATTER;
			gameObject.Enemies[COOLGUY].PrevtMode = MODE_SCATTER;
		}
		else {
			gameObject.Enemies[COOLGUY].CurrentMode = MODE_PURSUIT;
			gameObject.Enemies[COOLGUY].PrevtMode = MODE_PURSUIT;
		}
	}
	if (gameObject.CounterFrightened == 0 && gameObject.CounterMode != 4) {
		if (gameObject.CounterChase == 0) {
			gameObject.CounterScatter += fLastFrameTime;
			if (gameObject.CounterScatter >= 7) {
				gameObject.CounterChase += fLastFrameTime;
				gameObject.CounterScatter = 0;
				gameObject.CounterMode++;
				gameObject.Enemies[REDDY].CurrentMode =
					gameObject.Enemies[ROSITA].CurrentMode =
					gameObject.Enemies[DONJAIME].CurrentMode = MODE_PURSUIT;
				gameObject.Enemies[REDDY].PrevtMode =
					gameObject.Enemies[ROSITA].PrevtMode =
					gameObject.Enemies[DONJAIME].PrevtMode = MODE_PURSUIT;
				gameObject.Enemies[REDDY].Reverse =
					gameObject.Enemies[ROSITA].Reverse =
					gameObject.Enemies[DONJAIME].Reverse = true;
			}
		}
		else {
			gameObject.CounterChase += fLastFrameTime;
			if (gameObject.CounterChase >= 20) {
				gameObject.CounterChase = 0;
				gameObject.Enemies[REDDY].CurrentMode =
					gameObject.Enemies[ROSITA].CurrentMode =
					gameObject.Enemies[DONJAIME].CurrentMode = MODE_SCATTER;
				gameObject.Enemies[REDDY].PrevtMode =
					gameObject.Enemies[ROSITA].PrevtMode =
					gameObject.Enemies[DONJAIME].PrevtMode = MODE_SCATTER;
				gameObject.Enemies[REDDY].Reverse =
					gameObject.Enemies[ROSITA].Reverse =
					gameObject.Enemies[DONJAIME].Reverse = true;
			}
		}
	}
	else {
		gameObject.CounterFrightened += fLastFrameTime;
		if (gameObject.CounterFrightened >= 7) {
			gameObject.CounterFrightened = 0;
			gameObject.Enemies[REDDY].CurrentMode = gameObject.Enemies[REDDY].PrevtMode;
			gameObject.Enemies[ROSITA].CurrentMode = gameObject.Enemies[ROSITA].PrevtMode;
			gameObject.Enemies[DONJAIME].CurrentMode = gameObject.Enemies[DONJAIME].PrevtMode;
			gameObject.Enemies[COOLGUY].CurrentMode = gameObject.Enemies[COOLGUY].PrevtMode;
		}
	}

	for (uint32_t enemyC = 0; enemyC < ::wak::GHOST_COUNT; ++enemyC) {
		::wak::SEnemy								& currentEnemy = gameObject.Enemies[enemyC];

		if (currentEnemy.Dead == true)
			currentEnemy.Speed = 9.0f;
		else if (currentEnemy.Vulnerability == true || currentEnemy.Position.x < 10 && currentEnemy.Position.y == 17 || currentEnemy.Position.x > 19 && currentEnemy.Position.y == 17)
			currentEnemy.Speed = 3.0f;
		else if (currentEnemy.CurrentGhost == ::wak::REDDY && currentEnemy.CurrentMode == ::wak::MODE_PURSUIT)
			currentEnemy.Speed = 4.5f;
		else
			currentEnemy.Speed = 4.0f;

		if (currentEnemy.Stand == true)
			getDirectionByCollission(currentEnemy, gameObject);
		else if (gameObject.Map.TilesDecision[currentEnemy.Position.y][currentEnemy.Position.x] == true && currentEnemy.PositionDeltas.y > 0.4f && currentEnemy.PositionDeltas.x > 0.4f && currentEnemy.PositionDeltas.y < 0.6f && currentEnemy.PositionDeltas.x < 0.6f && currentEnemy.PrevTile != currentEnemy.Position) {
			if (currentEnemy.Dead == true) {
				getDirectionByTarget({ 15, 15 }, gameObject, currentEnemy);
			}
			else if (currentEnemy.Position.y == 14 && currentEnemy.Position.x == 13 || currentEnemy.Position.y == 14 && currentEnemy.Position.x == 16 || currentEnemy.Position.y == 26 && currentEnemy.Position.x == 13 || currentEnemy.Position.y == 26 && currentEnemy.Position.x == 16) {
				if (currentEnemy.CurrentDirection == ::wak::DOWN)
					getDirection(gameObject, currentEnemy);
			}
			else if (currentEnemy.Reverse == true) {
				reverse(currentEnemy);
				currentEnemy.Reverse = false;
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
				if (currentEnemy.Vulnerability == true) {
					currentEnemy.Dead = true;
					currentEnemy.CurrentMode = currentEnemy.PrevtMode;
				}
				else
					lifeLost(gameObject);
			}
		}
		if (currentEnemy.Dead == true) {
			if (currentEnemy.Position.y == 14 && currentEnemy.Position.x == 15 && currentEnemy.PositionDeltas.x < 0.2f) {
				currentEnemy.Position.y = 15;
				currentEnemy.PositionDeltas.x = 0;
				currentEnemy.PositionDeltas.y = 0;
			}
			else if (currentEnemy.Position.y == 15 && currentEnemy.Position.x == 15) {
				if (currentEnemy.PositionDeltas.y >= 0.9f) {
					currentEnemy.Position.y = 16;
					currentEnemy.PositionDeltas.y = 0;
					currentEnemy.Dead = false;
				}
				else
					currentEnemy.PositionDeltas.y += 0.9f * fLastFrameTime;
			}
			else
				moveInDirection(gameObject, currentEnemy, fLastFrameTime);
		}
		else
			moveInDirection(gameObject, currentEnemy, fLastFrameTime);

		refreshPosFromDeltas(currentEnemy);

		if (currentEnemy.Position.x == 0 && currentEnemy.PositionDeltas.x < 0.1f) { currentEnemy.Position.x = 29;				currentEnemy.PositionDeltas.x = 0.9f; }
		else if (currentEnemy.Position.x == 29 && currentEnemy.PositionDeltas.x > 0.9f) { currentEnemy.Position.x = 0;				currentEnemy.PositionDeltas.x = 0.1f; }

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