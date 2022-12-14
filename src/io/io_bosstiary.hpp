/**
 * Canary - A free and open-source MMORPG server emulator
 * Copyright (©) 2019-2022 OpenTibiaBR <opentibiabr@outlook.com>
 * Repository: https://github.com/opentibiabr/canary
 * License: https://github.com/opentibiabr/canary/blob/main/LICENSE
 * Contributors: https://github.com/opentibiabr/canary/graphs/contributors
 * Website: https://docs.opentibiabr.org/
*/

#ifndef SRC_IO_IO_BOSSTIARY_HPP_
#define SRC_IO_IO_BOSSTIARY_HPP_

#include <map>
#include <string>

enum class BosstiaryRarity_t : uint8_t {
	RARITY_BANE = 0,
	RARITY_ARCHFOE = 1,
	RARITY_NEMESIS = 2
};

class MonsterType;

class IOBosstiary {
public:
	// Non copyable
	IOBosstiary(IOBosstiary const&) = delete;
	void operator=(IOBosstiary const&) = delete;

	static IOBosstiary& getInstance() {
		// Guaranteed to be destroyed
		static IOBosstiary instance;
		// Instantiated on first use
		return instance;
	}

	void loadBoostedBoss();

	void addBosstiaryMonster(uint16_t raceId, const std::string & name);
	const std::map<uint16_t, std::string>& getBosstiaryMap() const;

	void setBossBoostedName(const std::string &name);
	std::string getBoostedBossName() const;
	MonsterType* getMonsterTypeByBossRaceId(uint16_t raceId) const;
private:
	IOBosstiary() = default;
	~IOBosstiary() = default;

	std::map<uint16_t, std::string> bosstiaryMap;
	std::string boostedBoss;
};

constexpr auto g_ioBosstiary = &IOBosstiary::getInstance;

#endif  // SRC_IO_IO_BOSSTIARY_HPP_
