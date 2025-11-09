#include "GameState.hpp"

#include <iostream>

#include "Common.hpp"
#include "Game\Game.hpp"
#include "Entities\Player\Player.hpp"


void GameState::load() {
	// first load the proper map
	Game::instance()->load_map(current_level_);

	// restore the saved values in that map
	Common::player->set_health(player_health_);
	Common::player->set_position(player_position_);
	Game::instance()->set_time_left(time_left_);
	Common::player->set_score(score_);
	Common::player->get_node()->setOrientation(player_orientation_);
}

void GameState::set_current_level(int currentLevel) {
	current_level_ = currentLevel;
}

void GameState::set_player_health(int playerHealth) {
	player_health_ = playerHealth;
}

void GameState::set_player_location(Ogre::Vector3 playerPosition) {
	player_position_ = playerPosition;
}

void GameState::set_player_orientation(Ogre::Quaternion playerOrientation) {
	player_orientation_ = playerOrientation;
}

void GameState::set_time_left(float timeLeft) {
	time_left_ = timeLeft;
}

void GameState::set_score(int score) {
	score_ = score;
}

void GameState::add_dead_enemy(std::string deadEnemyEntityName) {
	dead_enemies_.push_back(deadEnemyEntityName);
}

bool GameState::is_dead(std::string enemyId) {
	for (auto& dead_enemy_id : dead_enemies_) {
		if (dead_enemy_id == enemyId) {
			return true;
		}
	}
	return false;
}

void GameState::clear_dead_enemies() {
	dead_enemies_.clear();
}
