#include "GameState.hpp"

#include <iostream>

#include "Common.hpp"
#include "Game\Game.hpp"
#include "Entities\Player\Player.hpp"


void GameState::load() {
	// first load the proper map
	Game::instance().load_map(current_level_);

	// restore the saved values in that map
	Common::player->set_health(player_health_);
	Common::player->set_position(player_position_);
	Game::instance().set_time_left(time_left_);
	Common::player->set_score(score_);
	Common::player->get_node()->setOrientation(player_orientation_);
}

void GameState::set_current_level(int level) {
	current_level_ = level;
}

void GameState::set_player_health(int health) {
	player_health_ = health;
}

void GameState::set_player_location(Ogre::Vector3 position) {
	player_position_ = position;
}

void GameState::set_player_orientation(Ogre::Quaternion orientation) {
	player_orientation_ = orientation;
}

void GameState::set_time_left(float time_left) {
	time_left_ = time_left;
}

void GameState::set_score(int score) {
	score_ = score;
}

void GameState::add_dead_enemy(std::string enemy_id) {
	dead_enemies_.push_back(enemy_id);
}

bool GameState::is_dead(std::string target_enemy_id) {
	return std::ranges::find(dead_enemies_, target_enemy_id) != dead_enemies_.end();
}

void GameState::clear_dead_enemies() {
	dead_enemies_.clear();
}
