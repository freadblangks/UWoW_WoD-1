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

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "bloodmaul_slag_mines.h"

enum Texts
{
    NPC_TEXT_0                          = 77662, //< Да… он слабеет…
    NPC_TEXT_1                          = 77663, //< Убейте его! Я освобожусь!
    NPC_TEXT_2                          = 77653, //< Вы – щебень среди скал…
    NPC_TEXT_3                          = 77655, //< Я – горный обвал!
    NPC_TEXT_4                          = 84226, //< %s создает |cFFFF0404|Hspell:150045|h["Расплавление"]|h|r!
    NPC_TEXT_5                          = 77714, //< Обратитесь в прах!
    NPC_TEXT_6                          = 77671, //< Я сотру вас с лица земли.
    NPC_TEXT_7                          = 77868, //< Сотру в пыль!
    NPC_TEXT_8                          = 77651, //< Сотру в порошок…
    NPC_TEXT_9                          = 78568, //< Я возвращаюсь в землю…
    NPC_TEXT_START_BATTLE               = 77647,
    NPC_TEXT_10                         = 87603,
    NPC_TEXT_11                         = 78492,
};

enum Spells
{
    //< Shared
    SPELL_MAGMA_BARRAGE                 = 150004,
    SPELL_MAGMA_BARRAGE_2               = 150011,
    SPELL_ROUGH_SMASH                   = 149941,
    SPELL_DANCING_FLAMES                = 149975,
    SPELL_FIRE_STORM                    = 149997,
    SPELL_CAMERA_SHAKE                  = 143583,
    SPELL_CAMERA_SHAKE_HUGE             = 150209,
    SPELL_MAGMA_EXPLOSION_SUPREME       = 152864,
    SPELL_MAGMA_EXPLOSION_SUPREME_2     = 178676,
    
    //< Forgemaster Gog'duh
    SPELL_THROW_FLAGE                   = 150078,
    SPELL_THROW_EARTH                   = 150076,
    SPELL_SUMMON_EARTH_GUARDIAN         = 150061,
    SPELL_SUMMON_FLAME_GUARDIAN         = 150062,
    SPELL_EXPOSIVE_DEATH                = 150218,
    SPELL_MOLTEN_IMPACT                 = 150038,
    
    //< Magmolatus
    SPELL_SLAG_SMASH                    = 150023,
    SPELL_WITHERING_FLAMES              = 150032,
    SPELL_CHOKING_ASHES                 = 150034,
    SPELL_MOLTEN_IMPACT_2               = 150045,

    //< Molten Elemental
    SPELL_VOLCANIC_TANTRUM              = 150055, //< req script
    SPELL_VOLCANIC_TANTRUM_2            = 150048,

    //< Ruination <Servant of the Earth>
    SPELL_SHATTER_EARTH                 = 150324,

    //< Calamity <Servant of the Flame>
    SPELL_SCORCH                        = 150290,
};

enum NPCs
{
    NPC_MOLTEN_ELEMENTAL                = 74579,
    NPC_RUINATION                       = 74570,
    NPC_CALAMITY                        = 74571,
    NPC_LAVA_EXPLOSION_STALKER          = 75846,
};

class boss_forgemaster_gogduh : public CreatureScript
{
public:
    boss_forgemaster_gogduh() : CreatureScript("boss_forgemaster_gogduh") { }

    struct boss_forgemaster_gogduhoAI : public BossAI
    {
        boss_forgemaster_gogduhoAI(Creature* creature) : BossAI(creature, DATA_FORGEMASTER_GOGDUH)
        {
            instance = me->GetInstanceScript();
            me->StopMoving();
        }

        void Reset()
        {
            _Reset();

            enableMagmolatus = 0;
            magmas = 0;
        }

        void EnterCombat(Unit* /*who*/)
        {
            _EnterCombat();

            events.ScheduleEvent(EVENT_1, 3 * IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_4, 2 * IN_MILLISECONDS);

            Talk(uint8(NPC_TEXT_START_BATTLE));
        }

        void JustDied(Unit* /*killer*/)
        {
            _JustDied();
        }

        void EnterEvadeMode()
        {
            enableMagmolatus = 0;
            magmas = 0;

            _EnterEvadeMode();
        }

        void DamageTaken(Unit* /*attacker*/, uint32& damage)
        {
            if (enableMagmolatus == 0 && me->HealthBelowPctDamaged(50, damage))
            {
                ++enableMagmolatus;

                float x = 0.0f, y = 0.0f;
                GetRandPosFromCenterInDist(me->GetPositionX(), me->GetPositionY(), 5.0f, x, y);
                me->CastSpell(x, y - 4.0f, me->GetPositionZ(), SPELL_SUMMON_EARTH_GUARDIAN);
                me->CastSpell(x + 4.0f, y, me->GetPositionZ(), SPELL_SUMMON_FLAME_GUARDIAN);

                if (Creature* magmolatus = me->FindNearestCreature(NPC_LAVA_EXPLOSION_STALKER, 100))
                    magmolatus->AI()->DoAction(ACTION_2);

                Talk(uint8(NPC_TEXT_10));
            }

            if (enableMagmolatus == 1 && me->HealthBelowPctDamaged(20, damage))
            {
                ++enableMagmolatus;
                if (Creature* magmolatus = me->FindNearestCreature(BOSS_MAGMOLATUS, 50))
                    magmolatus->AI()->Talk(uint8(NPC_TEXT_1));
            }

            if (enableMagmolatus == 2 && me->HealthBelowPctDamaged(10, damage))
            {
                ++enableMagmolatus;

                if (Creature* magmolatus = me->FindNearestCreature(BOSS_MAGMOLATUS, 50))
                    magmolatus->AI()->DoAction(ACTION_1);

                me->SetReactState(REACT_PASSIVE);
                me->AttackStop();
                me->SetAIAnimKitId(1567);
                events.CancelEvent(EVENT_1);
                events.CancelEvent(EVENT_3);

                Talk(uint8(NPC_TEXT_11));
            }

            if (me->HealthBelowPctDamaged(1, damage))
                damage = 0;
        }

        void UpdateAI(uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (me->GetDistance(me->GetHomePosition()) >= 120.0f)
            {
                EnterEvadeMode();
                return;
            }

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_1:
                        events.ScheduleEvent(EVENT_1, 4 * IN_MILLISECONDS);
                        events.ScheduleEvent(EVENT_3, 1 * IN_MILLISECONDS);
                        
                        if (Creature* magmolatus = me->FindNearestCreature(BOSS_MAGMOLATUS, 100))
                            magmolatus->AI()->DoAction(ACTION_2);
                        
                        me->CastSpell(me, SPELL_MAGMA_BARRAGE);
                        break;
                    case EVENT_3:
                        if (magmas < 6)
                        {
                            events.ScheduleEvent(EVENT_3, 1 * IN_MILLISECONDS);
                            if (Creature* stalker = me->FindNearestCreature(NPC_LAVA_EXPLOSION_STALKER, 100))
                                stalker->AI()->DoAction(ACTION_3);

                            ++magmas;
                        }
                        else
                        {
                            magmas = 0;
                            events.CancelEvent(EVENT_3);
                        }
                        break;
                    case EVENT_4:
                        events.ScheduleEvent(EVENT_4, 10 * IN_MILLISECONDS);
                        if (Creature* magmolatus = me->FindNearestCreature(NPC_LAVA_EXPLOSION_STALKER, 100))
                            magmolatus->AI()->DoAction(ACTION_4);
                        break;
                    default:
                        break;
                }
            }
        }

    private:
        InstanceScript* instance;
        EventMap events;
        int8 enableMagmolatus;
        int8 magmas;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_forgemaster_gogduhoAI(creature);
    }
};

class boss_magmolatus : public CreatureScript
{
public:
    boss_magmolatus() : CreatureScript("boss_magmolatus") { }

    struct boss_magmolatusoAI : public BossAI
    {
        boss_magmolatusoAI(Creature* creature) : BossAI(creature, DATA_MAGMOLATUS)
        {
            instance = me->GetInstanceScript();

            me->SetReactState(REACT_PASSIVE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
        }

        void DoAction(int32 const action)
        {
            switch (action)
            {
                case ACTION_1:
                    me->SetReactState(REACT_AGGRESSIVE);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                    DoZoneInCombat();
                    Talk(uint8(NPC_TEXT_2));

                    if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                        AttackStart(target);

                    break;
                case ACTION_2:
                    Talk(uint8(NPC_TEXT_0));
                    break;
                default:
                    break;
            }
        }

        void Reset()
        {
            _Reset();

            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);

            events.ScheduleEvent(EVENT_5, 5 * IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_6, 12 * IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_7, 7 * IN_MILLISECONDS);
        }

        void EnterCombat(Unit* /*who*/)
        {
            _EnterCombat();

            events.ScheduleEvent(EVENT_5, 5 * IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_6, 12 * IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_7, 7 * IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_14, 5 * IN_MILLISECONDS);
        }

        void JustDied(Unit* /*killer*/)
        {
            _JustDied();

            Talk(uint8(NPC_TEXT_9));
        }

        void EnterEvadeMode()
        {
            BossAI::EnterEvadeMode();
            _EnterEvadeMode();

            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);

            if (Creature* gogduh = me->FindNearestCreature(BOSS_FORGEMASTER_GOGDUH, 100, false))
                gogduh->Respawn(true);
        }

        void UpdateAI(uint32 diff)
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_5:
                        events.ScheduleEvent(EVENT_5, 24 * IN_MILLISECONDS);
                        me->CastSpell(SelectTarget(SELECT_TARGET_RANDOM), SPELL_WITHERING_FLAMES);
                        break;
                    case EVENT_6:
                        events.ScheduleEvent(EVENT_6, 20 * IN_MILLISECONDS);
                        me->CastSpell(SelectTarget(SELECT_TARGET_RANDOM), SPELL_MOLTEN_IMPACT);
                        Talk(uint8(NPC_TEXT_4));
                        break;
                    case EVENT_7:
                        events.ScheduleEvent(EVENT_7, 16 * IN_MILLISECONDS);
                        me->CastSpell(me, SPELL_SLAG_SMASH);
                        break;
                    case EVENT_14:
                        events.ScheduleEvent(EVENT_15, 2 * IN_MILLISECONDS);
                        Talk(uint8(NPC_TEXT_3));
                        break;
                    case EVENT_15:
                        events.ScheduleEvent(EVENT_16, 3 * IN_MILLISECONDS);
                        Talk(uint8(NPC_TEXT_6));
                        break;
                    case EVENT_16:
                        Talk(uint8(NPC_TEXT_5));
                        break;
                    default:
                        break;
                }
            }

            DoMeleeAttackIfReady();
        }

    private:
        InstanceScript* instance;
        EventMap events;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_magmolatusoAI(creature);
    }
};

class npc_ruination : public CreatureScript
{
public:
    npc_ruination() : CreatureScript("npc_ruination") { }

    struct npc_ruinationAI : public ScriptedAI
    {
        npc_ruinationAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        void Reset()
        {
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

            me->SetInCombatWithZone();

            events.ScheduleEvent(EVENT_8, 3 * IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_9, 6 * IN_MILLISECONDS);
        }

        void EnterCombat(Unit* /*who*/)
        {
            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
            events.ScheduleEvent(EVENT_8, 3 * IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_9, 6 * IN_MILLISECONDS);
        }

        void JustDied(Unit* /*killer*/)
        {
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            me->DespawnOrUnsummon(3 * IN_MILLISECONDS);
        }

        void EnterEvadeMode()
        {
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            me->DespawnOrUnsummon(3 * IN_MILLISECONDS);
        }

        void UpdateAI(uint32 diff)
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_8:
                        events.ScheduleEvent(EVENT_8, 5 * IN_MILLISECONDS);
                        me->CastSpell(SelectTarget(SELECT_TARGET_TOPAGGRO), SPELL_ROUGH_SMASH);
                        break;
                    case EVENT_9:
                        events.ScheduleEvent(EVENT_8, 10 * IN_MILLISECONDS);
                        me->CastSpell(me, SPELL_SHATTER_EARTH);
                        break;
                    default:
                        break;
                }
            }
        }

    private:
        InstanceScript* instance;
        EventMap events;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
       return new npc_ruinationAI(creature);
    }
};

class npc_calamity : public CreatureScript
{
public:
    npc_calamity() : CreatureScript("npc_calamity") { }

    struct npc_calamityAI : public ScriptedAI
    {
        npc_calamityAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        void Reset()
        {
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

            events.ScheduleEvent(EVENT_10, 3 * IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_11, 7 * IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_12, 15 * IN_MILLISECONDS);
        }

        void EnterCombat(Unit* /*who*/)
        {
            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

            me->SetInCombatWithZone();

            events.ScheduleEvent(EVENT_10, 3 * IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_11, 7 * IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_12, 15 * IN_MILLISECONDS);
        }

        void JustDied(Unit* /*killer*/)
        {
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            me->DespawnOrUnsummon(3 * IN_MILLISECONDS);
        }

        void EnterEvadeMode()
        {
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            me->DespawnOrUnsummon(3 * IN_MILLISECONDS);
        }

        void UpdateAI(uint32 diff)
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_10:
                        events.ScheduleEvent(EVENT_10, 5 * IN_MILLISECONDS);
                        me->CastSpell(SelectTarget(SELECT_TARGET_TOPAGGRO), SPELL_SCORCH);
                        break;
                    case EVENT_11:
                        events.ScheduleEvent(EVENT_10, 10 * IN_MILLISECONDS);
                        me->CastSpell(me, SPELL_DANCING_FLAMES);
                        break;
                    case EVENT_12:
                        events.ScheduleEvent(EVENT_10, 20 * IN_MILLISECONDS);
                        me->CastSpell(me, SPELL_FIRE_STORM);
                        break;
                    default:
                        break;
                }
            }
        }

    private:
        InstanceScript* instance;
        EventMap events;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_calamityAI(creature);
    }
};

class npc_molten_elemental : public CreatureScript
{
public:
    npc_molten_elemental() : CreatureScript("npc_molten_elemental") { }

    struct npc_molten_elementalAI : public ScriptedAI
    {
        npc_molten_elementalAI(Creature* creature) : ScriptedAI(creature)
        {
            Instance = creature->GetInstanceScript();
        }

        void Reset()
        {
            events.ScheduleEvent(EVENT_10, 3 * IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_1, 7 * IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_2, 5 * IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_13, 12 * IN_MILLISECONDS);
        }

        void EnterCombat(Unit* /*who*/)
        {
            me->SetInCombatWithZone();

            events.ScheduleEvent(EVENT_10, 3 * IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_1, 7 * IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_2, 5 * IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_13, 12 * IN_MILLISECONDS);
        }

        void JustDied(Unit* /*killer*/)
        {
            me->DespawnOrUnsummon(3 * IN_MILLISECONDS);
        }

        void EnterEvadeMode()
        {
            me->DespawnOrUnsummon(3 * IN_MILLISECONDS);
        }

        void UpdateAI(uint32 diff)
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_10:
                        events.ScheduleEvent(EVENT_10, 3 * IN_MILLISECONDS);
                        me->CastSpell(SelectTarget(SELECT_TARGET_NEAREST), SPELL_SCORCH);
                        break;
                    case EVENT_1:
                        events.ScheduleEvent(EVENT_1, 8 * IN_MILLISECONDS);
                        me->CastSpell(me, SPELL_MAGMA_BARRAGE);
                        break;
                    case EVENT_2:
                        events.ScheduleEvent(EVENT_2, 12 * IN_MILLISECONDS);
                        me->AddAura(SPELL_MAGMA_BARRAGE_2, me);
                        break;
                    case EVENT_13:
                        events.ScheduleEvent(EVENT_13, 12 * IN_MILLISECONDS);
                        me->CastSpell(me, SPELL_VOLCANIC_TANTRUM_2);
                        break;
                    default:
                        break;
                }
            }
        }

    private:
        InstanceScript* Instance;
        EventMap events;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_molten_elementalAI(creature);
    }
};

class npc_lava_explosion_stalker : public CreatureScript
{
public:
    npc_lava_explosion_stalker() : CreatureScript("npc_lava_explosion_stalker") { }

    struct npc_lava_explosion_stalkerAI : public ScriptedAI
    {
        npc_lava_explosion_stalkerAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        void DoAction(int32 const action)
        {
            switch (action)
            {
                case ACTION_2:
                    me->CastSpell(me, SPELL_CAMERA_SHAKE);
                    break;
                case ACTION_3:
                    me->AddAura(SPELL_MAGMA_BARRAGE_2, me);
                    break;
                case ACTION_4:
                    me->CastSpell(me, SPELL_MAGMA_EXPLOSION_SUPREME);
                    break;
                default:
                    break;
            }
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (who->GetTypeId() != TYPEID_PLAYER || me->GetDistance(who) > 50.0f)
                return;

            if (instance->GetData(DATA_FORGEMASTER_GOGDUH_EVENT) == NOT_STARTED)
                instance->SetData(DATA_FORGEMASTER_GOGDUH_EVENT, DONE);
        }

        void Reset() { }

        void UpdateAI(uint32 /*diff*/) { }

    private:
        InstanceScript* instance;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lava_explosion_stalkerAI(creature);
    }
};

class spell_molten_impact : public SpellScriptLoader
{
public:
    spell_molten_impact() : SpellScriptLoader("spell_molten_impact") { }

    class spell_molten_impact_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_molten_impact_SpellScript);

        void HandleScriptEffect(SpellEffIndex effIndex)
        {
            if (Unit* caster = GetCaster())
                if (Unit* target = GetHitUnit())
                    caster->CastSpell(target, GetSpellInfo()->GetEffect(effIndex)->BasePoints, true);
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_molten_impact_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_molten_impact_SpellScript();
    }
};

void AddSC_boss_forgemaster_gogduh()
{
    new boss_forgemaster_gogduh();
    new boss_magmolatus();

    new npc_ruination();
    new npc_calamity();
    new npc_molten_elemental();
    new npc_lava_explosion_stalker();

    new spell_molten_impact();
}