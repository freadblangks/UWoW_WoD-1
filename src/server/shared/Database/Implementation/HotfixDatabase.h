/*
 * Copyright (C) 2008-2015 TrinityCore <http://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _HOTFIXDATABASE_H
#define _HOTFIXDATABASE_H

#include "DatabaseWorkerPool.h"
#include "MySQLConnection.h"

enum HotfixDatabaseStatements
{
    /*  Naming standard for defines:
        {DB}_{SEL/INS/UPD/DEL/REP}_{Summary of data changed}
        When updating more than one field, consider looking at the calling function
        name for a suiting suffix.
    */

    HOTFIX_SEL_AREA_GROUP,

    HOTFIX_SEL_AREA_GROUP_MEMBER,

    HOTFIX_SEL_BROADCAST_TEXT,
    HOTFIX_SEL_BROADCAST_TEXT_LOCALE,

    HOTFIX_SEL_CURVE_POINT,

    HOTFIX_SEL_HOLIDAYS,
    HOTFIX_SEL_HOLIDAYS_LOCALE,

    HOTFIX_SEL_ITEM,

    HOTFIX_SEL_ITEM_APPEARANCE,

    HOTFIX_SEL_ITEM_BONUS,

    HOTFIX_SEL_ITEM_BONUS_TREE_NODE,

    HOTFIX_SEL_ITEM_CURRENCY_COST,

    HOTFIX_SEL_ITEM_EFFECT,

    HOTFIX_SEL_ITEM_MODIFIED_APPEARANCE,

    HOTFIX_SEL_ITEM_SPARSE,
    HOTFIX_SEL_ITEM_SPARSE_LOCALE,

    HOTFIX_SEL_ITEM_EXTENDED_COST,

    HOTFIX_SEL_BATTLE_PET_ABILITY,
    HOTFIX_SEL_BATTLE_PET_ABILITY_LOCALE,

    HOTFIX_SEL_BATTLE_PET_ABILITY_EFFECT,

    HOTFIX_SEL_BATTLE_PET_EFFECT_PROPERTIES,
    HOTFIX_SEL_BATTLE_PET_EFFECT_PROPERTIES_LOCALE,

    HOTFIX_SEL_BATTLE_PET_ABILITY_TURN,

    HOTFIX_SEL_BATTLE_PET_ABILITY_STATE,

    HOTFIX_SEL_BATTLE_PET_STATE,
    HOTFIX_SEL_BATTLE_PET_STATE_LOCALE,

    HOTFIX_SEL_BATTLE_PET_SPECIES,
    HOTFIX_SEL_BATTLE_PET_SPECIES_LOCALE,

    HOTFIX_SEL_BATTLE_PET_SPECIES_STATE,

    HOTFIX_SEL_BATTLE_PET_SPECIES_X_ABILITY,

    HOTFIX_SEL_BATTLE_PET_BREED_QUALITY,

    HOTFIX_SEL_BATTLE_PET_BREED_STATE,

    HOTFIX_SEL_GARR_ABILITY,
    HOTFIX_SEL_GARR_ABILITY_LOCALE,

    HOTFIX_SEL_GARRISON_ABILITY_EFFECT,

    HOTFIX_SEL_GARR_BUILDING,
    HOTFIX_SEL_GARR_BUILDING_LOCALE,

    HOTFIX_SEL_GARR_BUILDING_PLOT_INST,

    HOTFIX_SEL_GARR_CLASS_SPEC,
    HOTFIX_SEL_GARR_CLASS_SPEC_LOCALE,

    HOTFIX_SEL_GARRISON_ENCOUNTER,

    HOTFIX_SEL_GARRISON_ENCOUNTER_X_MECHANIC,

    HOTFIX_SEL_GARR_FOLLOWER,
    HOTFIX_SEL_GARR_FOLLOWER_LOCALE,

    HOTFIX_SEL_GARR_FOLLOWER_X_ABILITY,

    HOTFIX_SEL_GARRISON_MECHANIC,

    HOTFIX_SEL_GARRISON_MISSION,

    HOTFIX_SEL_GARRISON_MISSION_REWARD,

    HOTFIX_SEL_GARRISON_MISSION_X_ENCOUNTER,

    HOTFIX_SEL_GARR_PLOT,
    HOTFIX_SEL_GARR_PLOT_LOCALE,

    HOTFIX_SEL_GARR_PLOT_BUILDING,

    HOTFIX_SEL_GARR_PLOT_INSTANCE,
    HOTFIX_SEL_GARR_PLOT_INSTANCE_LOCALE,

    HOTFIX_SEL_GARR_SITE_LEVEL,

    HOTFIX_SEL_GARR_SITE_LEVEL_PLOT_INST,

    HOTFIX_SEL_CURRENCY_TYPES,
    HOTFIX_SEL_CURRENCY_TYPES_LOCALE,

    HOTFIX_SEL_QUEST_PACKAGE_ITEM,

    HOTFIX_SEL_MOUNT,
    HOTFIX_SEL_MOUNT_LOCALE,

    HOTFIX_SEL_LANGUAGE_WORDS,
    HOTFIX_SEL_LANGUAGE_WORDS_LOCALE,

    HOTFIX_SEL_KEY_CHAIN,

    HOTFIX_SEL_OVERRIDE_SPELL_DATA,

    HOTFIX_SEL_PHASE_GROUP,

    HOTFIX_SEL_SOUND_ENTRIES,
    HOTFIX_SEL_SOUND_ENTRIES_LOCALE,

    HOTFIX_SEL_SPELL_AURA_RESTRICTIONS,

    HOTFIX_SEL_SPELL_CASTING_REQUIREMENTS,

    HOTFIX_SEL_SPELL_CLASS_OPTIONS,

    HOTFIX_SEL_SPELL_MISC,

    HOTFIX_SEL_SPELL_POWER,

    HOTFIX_SEL_SPELL_REAGENTS,

    HOTFIX_SEL_SPELL_RUNE_COST,

    HOTFIX_SEL_TOY,
    HOTFIX_SEL_TOY_LOCALE,

    HOTFIX_SEL_SPELL_TOTEMS,

    HOTFIX_SEL_TAXI_NODES,
    HOTFIX_SEL_TAXI_NODES_LOCALE,

    HOTFIX_SEL_TAXI_PATH,

    HOTFIX_SEL_TAXI_PATH_NODE,

    HOTFIX_SEL_ITEM_UPGRADE,

    HOTFIX_SEL_RULE_SET_ITEM_UPGRADE,

    HOTFIX_SEL_GAME_OBJECTS,
    HOTFIX_SEL_GAME_OBJECTS_LOCALE,

    HOTFIX_SEL_MAP_CHALLENGE_MODE,

    HOTFIX_SEL_SPELL_VISUAL,

    HOTFIX_SEL_ITEM_X_BONUS_TREE,

    MAX_HOTFIXDATABASE_STATEMENTS
};

class HotfixDatabaseConnection : public MySQLConnection
{
public:
    typedef HotfixDatabaseStatements Statements;

    //- Constructors for sync and async connections
    HotfixDatabaseConnection(MySQLConnectionInfo& connInfo) : MySQLConnection(connInfo) { }
    HotfixDatabaseConnection(ProducerConsumerQueue<SQLOperation*>* q, MySQLConnectionInfo& connInfo) : MySQLConnection(q, connInfo) { }

    //- Loads database type specific prepared statements
    void DoPrepareStatements() override;
};

typedef DatabaseWorkerPool<HotfixDatabaseConnection> HotfixDatabaseWorkerPool;

#endif