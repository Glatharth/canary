/**
 * Canary - A free and open-source MMORPG server emulator
 * Copyright (©) 2019-2024 OpenTibiaBR <opentibiabr@outlook.com>
 * Repository: https://github.com/opentibiabr/canary
 * License: https://github.com/opentibiabr/canary/blob/main/LICENSE
 * Contributors: https://github.com/opentibiabr/canary/graphs/contributors
 * Website: https://docs.opentibiabr.com/
 */

#include "lua/functions/items/weapon_functions.hpp"

#include "game/game.hpp"
#include "items/item.hpp"
#include "lua/scripts/lua_environment.hpp"
#include "utils/tools.hpp"

int WeaponFunctions::luaCreateWeapon(lua_State* L) {
	// Weapon(type)
	const WeaponType_t type = getNumber<WeaponType_t>(L, 2);
	switch (type) {
		case WEAPON_SWORD:
		case WEAPON_AXE:
		case WEAPON_CLUB: {
			if (const auto &weaponPtr = g_luaEnvironment().createWeaponObject<WeaponMelee>(getScriptEnv()->getScriptInterface())) {
				pushUserdata<WeaponMelee>(L, weaponPtr);
				setMetatable(L, -1, "Weapon");
				weaponPtr->weaponType = type;
			} else {
				lua_pushnil(L);
			}
			break;
		}
		case WEAPON_MISSILE:
		case WEAPON_DISTANCE:
		case WEAPON_AMMO: {
			if (const auto &weaponPtr = g_luaEnvironment().createWeaponObject<WeaponDistance>(getScriptEnv()->getScriptInterface())) {
				pushUserdata<WeaponDistance>(L, weaponPtr);
				setMetatable(L, -1, "Weapon");
				weaponPtr->weaponType = type;
			} else {
				lua_pushnil(L);
			}
			break;
		}
		case WEAPON_WAND: {
			if (const auto &weaponPtr = g_luaEnvironment().createWeaponObject<WeaponWand>(getScriptEnv()->getScriptInterface())) {
				pushUserdata<WeaponWand>(L, weaponPtr);
				setMetatable(L, -1, "Weapon");
				weaponPtr->weaponType = type;
			} else {
				lua_pushnil(L);
			}
			break;
		}
		default: {
			lua_pushnil(L);
			break;
		}
	}
	return 1;
}

int WeaponFunctions::luaWeaponAction(lua_State* L) {
	// weapon:action(callback)
	const WeaponShared_ptr &weapon = getUserdataShared<Weapon>(L, 1);
	if (weapon) {
		std::string typeName = getString(L, 2);
		const std::string tmpStr = asLowerCaseString(typeName);
		if (tmpStr == "removecount") {
			weapon->action = WEAPONACTION_REMOVECOUNT;
		} else if (tmpStr == "removecharge") {
			weapon->action = WEAPONACTION_REMOVECHARGE;
		} else if (tmpStr == "move") {
			weapon->action = WEAPONACTION_MOVE;
		} else {
			g_logger().error("[WeaponFunctions::luaWeaponAction] - "
			                 "No valid action {}",
			                 typeName);
			pushBoolean(L, false);
		}
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int WeaponFunctions::luaWeaponRegister(lua_State* L) {
	// weapon:register()
	const WeaponShared_ptr* weaponPtr = getRawUserDataShared<Weapon>(L, 1);
	if (weaponPtr && *weaponPtr) {
		WeaponShared_ptr weapon = *weaponPtr;
		if (weapon->weaponType == WEAPON_DISTANCE || weapon->weaponType == WEAPON_AMMO || weapon->weaponType == WEAPON_MISSILE) {
			weapon = getUserdataShared<WeaponDistance>(L, 1);
		} else if (weapon->weaponType == WEAPON_WAND) {
			weapon = getUserdataShared<WeaponWand>(L, 1);
		} else {
			weapon = getUserdataShared<WeaponMelee>(L, 1);
		}

		const uint16_t id = weapon->getID();
		ItemType &it = Item::items.getItemType(id);
		it.weaponType = weapon->weaponType;

		if (weapon->getWieldInfo() != 0) {
			it.wieldInfo = weapon->getWieldInfo();
			it.vocationString = weapon->getVocationString();
			it.minReqLevel = weapon->getReqLevel();
			it.minReqMagicLevel = weapon->getReqMagLv();
		}

		weapon->configureWeapon(it);
		pushBoolean(L, g_weapons().registerLuaEvent(weapon));
		weapon = nullptr; // Releases weapon, removing the luascript reference
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int WeaponFunctions::luaWeaponOnUseWeapon(lua_State* L) {
	// weapon:onUseWeapon(callback)
	const WeaponShared_ptr &weapon = getUserdataShared<Weapon>(L, 1);
	if (weapon) {
		if (!weapon->loadCallback()) {
			pushBoolean(L, false);
			return 1;
		}

		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int WeaponFunctions::luaWeaponUnproperly(lua_State* L) {
	// weapon:wieldedUnproperly(bool)
	const WeaponShared_ptr &weapon = getUserdataShared<Weapon>(L, 1);
	if (weapon) {
		weapon->setWieldUnproperly(getBoolean(L, 2));
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int WeaponFunctions::luaWeaponLevel(lua_State* L) {
	// weapon:level(lvl)
	const WeaponShared_ptr &weapon = getUserdataShared<Weapon>(L, 1);
	if (weapon) {
		weapon->setRequiredLevel(getNumber<uint32_t>(L, 2));
		weapon->setWieldInfo(WIELDINFO_LEVEL);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int WeaponFunctions::luaWeaponMagicLevel(lua_State* L) {
	// weapon:magicLevel(lvl)
	const WeaponShared_ptr &weapon = getUserdataShared<Weapon>(L, 1);
	if (weapon) {
		weapon->setRequiredMagLevel(getNumber<uint32_t>(L, 2));
		weapon->setWieldInfo(WIELDINFO_MAGLV);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int WeaponFunctions::luaWeaponMana(lua_State* L) {
	// weapon:mana(mana)
	const WeaponShared_ptr &weapon = getUserdataShared<Weapon>(L, 1);
	if (weapon) {
		weapon->setMana(getNumber<uint32_t>(L, 2));
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int WeaponFunctions::luaWeaponManaPercent(lua_State* L) {
	// weapon:manaPercent(percent)
	const WeaponShared_ptr &weapon = getUserdataShared<Weapon>(L, 1);
	if (weapon) {
		weapon->setManaPercent(getNumber<uint32_t>(L, 2));
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int WeaponFunctions::luaWeaponHealth(lua_State* L) {
	// weapon:health(health)
	const WeaponShared_ptr &weapon = getUserdataShared<Weapon>(L, 1);
	if (weapon) {
		weapon->setHealth(getNumber<int32_t>(L, 2));
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int WeaponFunctions::luaWeaponHealthPercent(lua_State* L) {
	// weapon:healthPercent(percent)
	const WeaponShared_ptr &weapon = getUserdataShared<Weapon>(L, 1);
	if (weapon) {
		weapon->setHealthPercent(getNumber<uint32_t>(L, 2));
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int WeaponFunctions::luaWeaponSoul(lua_State* L) {
	// weapon:soul(soul)
	const WeaponShared_ptr &weapon = getUserdataShared<Weapon>(L, 1);
	if (weapon) {
		weapon->setSoul(getNumber<uint32_t>(L, 2));
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int WeaponFunctions::luaWeaponBreakChance(lua_State* L) {
	// weapon:breakChance(percent)
	const WeaponShared_ptr &weapon = getUserdataShared<Weapon>(L, 1);
	if (weapon) {
		weapon->setBreakChance(getNumber<uint32_t>(L, 2));
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int WeaponFunctions::luaWeaponWandDamage(lua_State* L) {
	// weapon:damage(damage[min, max]) only use this if the weapon is a wand!
	const auto &weapon = getUserdataShared<WeaponWand>(L, 1);
	if (weapon) {
		weapon->setMinChange(getNumber<uint32_t>(L, 2));
		if (lua_gettop(L) > 2) {
			weapon->setMaxChange(getNumber<uint32_t>(L, 3));
		} else {
			weapon->setMaxChange(getNumber<uint32_t>(L, 2));
		}
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int WeaponFunctions::luaWeaponElement(lua_State* L) {
	// weapon:element(combatType)
	const WeaponShared_ptr &weapon = getUserdataShared<Weapon>(L, 1);
	if (weapon) {
		if (!getNumber<CombatType_t>(L, 2)) {
			std::string element = getString(L, 2);
			const std::string tmpStrValue = asLowerCaseString(element);
			if (tmpStrValue == "earth") {
				weapon->params.combatType = COMBAT_EARTHDAMAGE;
			} else if (tmpStrValue == "ice") {
				weapon->params.combatType = COMBAT_ICEDAMAGE;
			} else if (tmpStrValue == "energy") {
				weapon->params.combatType = COMBAT_ENERGYDAMAGE;
			} else if (tmpStrValue == "fire") {
				weapon->params.combatType = COMBAT_FIREDAMAGE;
			} else if (tmpStrValue == "death") {
				weapon->params.combatType = COMBAT_DEATHDAMAGE;
			} else if (tmpStrValue == "holy") {
				weapon->params.combatType = COMBAT_HOLYDAMAGE;
			} else {
				g_logger().warn("[WeaponFunctions:luaWeaponElement] - "
				                "Type {} does not exist",
				                element);
			}
		} else {
			weapon->params.combatType = getNumber<CombatType_t>(L, 2);
		}
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int WeaponFunctions::luaWeaponPremium(lua_State* L) {
	// weapon:premium(bool)
	const WeaponShared_ptr &weapon = getUserdataShared<Weapon>(L, 1);
	if (weapon) {
		weapon->setNeedPremium(getBoolean(L, 2));
		weapon->setWieldInfo(WIELDINFO_PREMIUM);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int WeaponFunctions::luaWeaponVocation(lua_State* L) {
	// weapon:vocation(vocName[, showInDescription = false, lastVoc = false])
	const WeaponShared_ptr &weapon = getUserdataShared<Weapon>(L, 1);
	if (weapon) {
		weapon->addVocWeaponMap(getString(L, 2));
		weapon->setWieldInfo(WIELDINFO_VOCREQ);
		std::string tmp;
		bool showInDescription = false;
		bool lastVoc = false;
		if (getBoolean(L, 3)) {
			showInDescription = getBoolean(L, 3);
		}
		if (getBoolean(L, 4)) {
			lastVoc = getBoolean(L, 4);
		}
		if (showInDescription) {
			if (weapon->getVocationString().empty()) {
				tmp = asLowerCaseString(getString(L, 2));
				tmp += "s";
				weapon->setVocationString(tmp);
			} else {
				tmp = weapon->getVocationString();
				if (lastVoc) {
					tmp += " and ";
				} else {
					tmp += ", ";
				}
				tmp += asLowerCaseString(getString(L, 2));
				tmp += "s";
				weapon->setVocationString(tmp);
			}
		}
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int WeaponFunctions::luaWeaponId(lua_State* L) {
	// weapon:id(id)
	const WeaponShared_ptr &weapon = getUserdataShared<Weapon>(L, 1);
	if (weapon) {
		weapon->setID(getNumber<uint16_t>(L, 2));
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int WeaponFunctions::luaWeaponAttack(lua_State* L) {
	// weapon:attack(atk)
	const WeaponShared_ptr &weapon = getUserdataShared<Weapon>(L, 1);
	if (weapon) {
		const uint16_t id = weapon->getID();
		ItemType &it = Item::items.getItemType(id);
		it.attack = getNumber<int32_t>(L, 2);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int WeaponFunctions::luaWeaponDefense(lua_State* L) {
	// weapon:defense(defense[, extraDefense])
	const WeaponShared_ptr &weapon = getUserdataShared<Weapon>(L, 1);
	if (weapon) {
		const uint16_t id = weapon->getID();
		ItemType &it = Item::items.getItemType(id);
		it.defense = getNumber<int32_t>(L, 2);
		if (lua_gettop(L) > 2) {
			it.extraDefense = getNumber<int32_t>(L, 3);
		}
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int WeaponFunctions::luaWeaponRange(lua_State* L) {
	// weapon:range(range)
	const WeaponShared_ptr &weapon = getUserdataShared<Weapon>(L, 1);
	if (weapon) {
		const uint16_t id = weapon->getID();
		ItemType &it = Item::items.getItemType(id);
		it.shootRange = getNumber<uint8_t>(L, 2);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int WeaponFunctions::luaWeaponCharges(lua_State* L) {
	// weapon:charges(charges[, showCharges = true])
	const WeaponShared_ptr &weapon = getUserdataShared<Weapon>(L, 1);
	if (weapon) {
		bool showCharges = true;
		if (lua_gettop(L) > 2) {
			showCharges = getBoolean(L, 3);
		}
		const uint16_t id = weapon->getID();
		ItemType &it = Item::items.getItemType(id);
		it.charges = getNumber<uint8_t>(L, 2);
		it.showCharges = showCharges;
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int WeaponFunctions::luaWeaponDuration(lua_State* L) {
	// weapon:duration(duration[, showDuration = true])
	const WeaponShared_ptr &weapon = getUserdataShared<Weapon>(L, 1);
	if (weapon) {
		bool showDuration = true;
		if (lua_gettop(L) > 2) {
			showDuration = getBoolean(L, 3);
		}
		const uint16_t id = weapon->getID();
		ItemType &it = Item::items.getItemType(id);
		it.decayTime = getNumber<uint8_t>(L, 2);
		it.showDuration = showDuration;
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int WeaponFunctions::luaWeaponDecayTo(lua_State* L) {
	// weapon:decayTo([itemid = 0]
	const WeaponShared_ptr &weapon = getUserdataShared<Weapon>(L, 1);
	if (weapon) {
		uint16_t itemid = 0;
		if (lua_gettop(L) > 1) {
			itemid = getNumber<uint16_t>(L, 2);
		}
		const uint16_t id = weapon->getID();
		ItemType &it = Item::items.getItemType(id);
		it.decayTo = itemid;
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int WeaponFunctions::luaWeaponTransformEquipTo(lua_State* L) {
	// weapon:transformEquipTo(itemid)
	const WeaponShared_ptr &weapon = getUserdataShared<Weapon>(L, 1);
	if (weapon) {
		const uint16_t id = weapon->getID();
		ItemType &it = Item::items.getItemType(id);
		it.transformEquipTo = getNumber<uint16_t>(L, 2);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int WeaponFunctions::luaWeaponTransformDeEquipTo(lua_State* L) {
	// weapon:transformDeEquipTo(itemid)
	const WeaponShared_ptr &weapon = getUserdataShared<Weapon>(L, 1);
	if (weapon) {
		const uint16_t id = weapon->getID();
		ItemType &it = Item::items.getItemType(id);
		it.transformDeEquipTo = getNumber<uint16_t>(L, 2);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int WeaponFunctions::luaWeaponShootType(lua_State* L) {
	// weapon:shootType(type)
	const WeaponShared_ptr &weapon = getUserdataShared<Weapon>(L, 1);
	if (weapon) {
		const uint16_t id = weapon->getID();
		ItemType &it = Item::items.getItemType(id);
		it.shootType = getNumber<ShootType_t>(L, 2);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int WeaponFunctions::luaWeaponSlotType(lua_State* L) {
	// weapon:slotType(slot)
	const WeaponShared_ptr &weapon = getUserdataShared<Weapon>(L, 1);
	if (weapon) {
		const uint16_t id = weapon->getID();
		ItemType &it = Item::items.getItemType(id);
		const std::string slot = getString(L, 2);

		if (slot == "two-handed") {
			it.slotPosition = SLOTP_TWO_HAND;
		} else {
			it.slotPosition = SLOTP_HAND;
		}
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int WeaponFunctions::luaWeaponAmmoType(lua_State* L) {
	// weapon:ammoType(type)
	const auto &weapon = getUserdataShared<WeaponDistance>(L, 1);
	if (weapon) {
		const uint16_t id = weapon->getID();
		ItemType &it = Item::items.getItemType(id);
		std::string type = getString(L, 2);

		if (type == "arrow") {
			it.ammoType = AMMO_ARROW;
		} else if (type == "bolt") {
			it.ammoType = AMMO_BOLT;
		} else {
			g_logger().warn("[WeaponFunctions:luaWeaponAmmoType] - "
			                "Type {} does not exist",
			                type);
			lua_pushnil(L);
			return 1;
		}
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int WeaponFunctions::luaWeaponHitChance(lua_State* L) {
	// weapon:hitChance(chance)
	const WeaponShared_ptr &weapon = getUserdataShared<Weapon>(L, 1);
	if (weapon) {
		const uint16_t id = weapon->getID();
		ItemType &it = Item::items.getItemType(id);
		it.hitChance = getNumber<int8_t>(L, 2);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int WeaponFunctions::luaWeaponMaxHitChance(lua_State* L) {
	// weapon:maxHitChance(max)
	const WeaponShared_ptr &weapon = getUserdataShared<Weapon>(L, 1);
	if (weapon) {
		const uint16_t id = weapon->getID();
		ItemType &it = Item::items.getItemType(id);
		it.maxHitChance = getNumber<int32_t>(L, 2);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int WeaponFunctions::luaWeaponExtraElement(lua_State* L) {
	// weapon:extraElement(atk, combatType)
	const WeaponShared_ptr &weapon = getUserdataShared<Weapon>(L, 1);
	if (weapon) {
		const uint16_t id = weapon->getID();
		const ItemType &it = Item::items.getItemType(id);
		it.abilities->elementDamage = getNumber<uint16_t>(L, 2);

		if (!getNumber<CombatType_t>(L, 3)) {
			std::string element = getString(L, 3);
			const std::string tmpStrValue = asLowerCaseString(element);
			if (tmpStrValue == "earth") {
				it.abilities->elementType = COMBAT_EARTHDAMAGE;
			} else if (tmpStrValue == "ice") {
				it.abilities->elementType = COMBAT_ICEDAMAGE;
			} else if (tmpStrValue == "energy") {
				it.abilities->elementType = COMBAT_ENERGYDAMAGE;
			} else if (tmpStrValue == "fire") {
				it.abilities->elementType = COMBAT_FIREDAMAGE;
			} else if (tmpStrValue == "death") {
				it.abilities->elementType = COMBAT_DEATHDAMAGE;
			} else if (tmpStrValue == "holy") {
				it.abilities->elementType = COMBAT_HOLYDAMAGE;
			} else {
				g_logger().warn("[WeaponFunctions:luaWeaponExtraElement] - "
				                "Type {} does not exist",
				                element);
			}
		} else {
			it.abilities->elementType = getNumber<CombatType_t>(L, 3);
		}
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}
