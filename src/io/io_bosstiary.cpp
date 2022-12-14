/**
 * Canary - A free and open-source MMORPG server emulator
 * Copyright (©) 2019-2022 OpenTibiaBR <opentibiabr@outlook.com>
 * Repository: https://github.com/opentibiabr/canary
 * License: https://github.com/opentibiabr/canary/blob/main/LICENSE
 * Contributors: https://github.com/opentibiabr/canary/graphs/contributors
 * Website: https://docs.opentibiabr.org/
*/

#include "pch.hpp"

#include "io/io_bosstiary.hpp"

#include "creatures/monsters/monsters.h"
#include "utils/tools.h"

#include <chrono>

void IOBosstiary::loadBoostedBoss()
{
	Database& database = Database::getInstance();
	std::ostringstream query;
	query << "SELECT * FROM `boosted_boss`";
	DBResult_ptr result = database.storeQuery(query.str());
	if (!result) {
		SPDLOG_ERROR("[{}] Failed to detect boosted boss database. (CODE 01)", __FUNCTION__);
		return;
	}

	uint16_t date = result->getNumber<uint16_t>("date");
	auto timeNow = getTimeNow();
	auto time = localtime(&timeNow);
	auto today = time->tm_mday;

	auto bossMap = getBosstiaryMap();
	if (bossMap.size() <= 1) {
		SPDLOG_ERROR("[{}] It is not possible to create a boosted boss with only one registered boss. (CODE 02)", __FUNCTION__);
		return;
	}

	std::string bossName;
	if (date == today) {
		bossName = result->getString("boostname");
		setBossBoostedName(bossName);
		SPDLOG_INFO("Boosted boss: {}", bossName);
		return;
	}

	uint16_t oldBossRace = result->getNumber<uint16_t>("raceid");
	uint16_t bossRace = 0;
	uint8_t size = 0;
	uint16_t bossrandom = uniform_random(0, bossMap.size());
	do {
		for (const auto& [mapBossRaceId, mapBossName] : bossMap) {
			if (oldBossRace == mapBossRaceId) {
				continue;
			}

			if (size == bossrandom) {
				bossRace = mapBossRaceId;
				bossName = mapBossName;
			}
			size++;
		}
	} while (size < bossMap.size());

	query.str(std::string());
	query << "UPDATE `boosted_boss` SET ";
	query << "`date` = '" << today << "',";
	query << "`boostname` = " << database.escapeString(bossName) << ",";
	SPDLOG_INFO("5");
	const MonsterType* bossType = getMonsterTypeByBossRaceId(bossRace);
	if (bossType) {
		query << "`looktype` = " << static_cast<int>(bossType->info.outfit.lookType) << ",";
		query << "`lookfeet` = " << static_cast<int>(bossType->info.outfit.lookFeet) << ",";
		query << "`looklegs` = " << static_cast<int>(bossType->info.outfit.lookLegs) << ",";
		query << "`lookhead` = " << static_cast<int>(bossType->info.outfit.lookHead) << ",";
		query << "`lookbody` = " << static_cast<int>(bossType->info.outfit.lookBody) << ",";
		query << "`lookaddons` = " << static_cast<int>(bossType->info.outfit.lookAddons) << ",";
		query << "`lookmount` = " << static_cast<int>(bossType->info.outfit.lookMount) << ",";
	}
	query << "`raceid` = '" << bossRace << "'";
	if (!database.executeQuery(query.str())) {
		SPDLOG_ERROR("[{}] Failed to detect boosted boss database. (CODE 03)", __FUNCTION__);
		return;
	}

	setBossBoostedName(bossName);
	SPDLOG_INFO("Boosted boss: {}", bossName);
}

void IOBosstiary::addBosstiaryMonster(uint16_t raceId, const std::string & name)
{
	auto it = bosstiaryMap.find(raceId);
	if (it != bosstiaryMap.end()) {
		return;
	}

	bosstiaryMap.insert(std::pair<uint16_t, std::string>(raceId, name));
}

const std::map<uint16_t, std::string>& IOBosstiary::getBosstiaryMap() const {
	return bosstiaryMap;
}

void IOBosstiary::setBossBoostedName(const std::string &name) {
	boostedBoss = name;
}

std::string IOBosstiary::getBoostedBossName() const {
	return boostedBoss;
}

MonsterType* IOBosstiary::getMonsterTypeByBossRaceId(uint16_t raceId) const {
	for (const auto&[bossRaceId, bossName] : getBosstiaryMap()) {
		if (bossRaceId == raceId) {
			MonsterType* monsterType = g_monsters().getMonsterType(bossName);
			if (!monsterType) {
				SPDLOG_ERROR("[{}] Boss with id not found in boss map", raceId);
				continue;
			}

			return monsterType;
		}
	}

	return nullptr;
}
