#include "ReusablePotion.h"
#include "GameTime.h"

void SetPlayerPvPState(Player* player, bool state)
{
    uint32 playerGuid = player->GetGUID().GetRawValue();
    auto mapKey = playerPvpMap.find(playerGuid);
    if (mapKey != playerPvpMap.end())
    {
        mapKey->second = state;
    }
    else
    {
        playerPvpMap.emplace(playerGuid, state);
    }
}

bool GetPlayerPvPState(Player* player)
{
    uint32 playerGuid = player->GetGUID().GetRawValue();
    auto mapKey = playerPvpMap.find(playerGuid);

    if (mapKey != playerPvpMap.end())
    {
        return mapKey->second;
    }

    return false;
}

void ReusablePotionPlayerScript::OnPlayerLeaveCombat(Player* player)
{
    if (!player)
    {
        return;
    }

    if (!sConfigMgr->GetOption<bool>("ReusablePotion.Enable", false))
    {
        return;
    }

    if (sConfigMgr->GetOption<bool>("ReusablePotion.Verbose", false))
    {
        LOG_INFO("module", "ReusablePotion(OnPlayerLeaveCombat)");
    }

    SetPlayerPvPState(player, false);
}

void ReusablePotionUnitScript::OnDamage(Unit* attacker, Unit* victim, uint32& /*damage*/)
{
    if (!sConfigMgr->GetOption<bool>("ReusablePotion.Enable", false))
    {
        return;
    }

    if (!attacker)
    {
        return;
    }

    auto attPlayer = attacker->ToPlayer();
    if (!attPlayer)
    {
        return;
    }

    if (!victim)
    {
        return;
    }

    auto victPlayer = victim->ToPlayer();
    if (!victPlayer)
    {
        return;
    }

    if (sConfigMgr->GetOption<bool>("ReusablePotion.Verbose", false))
    {
        LOG_INFO("module", "ReusablePotion(OnDamage)");
    }

    if (attPlayer->GetGUID() == victPlayer->GetGUID())
    {
        if (sConfigMgr->GetOption<bool>("ReusablePotion.Verbose", false))
        {
            LOG_INFO("module", "ReusablePotion(OnDamage:DamageSelf)");
        }
        return;
    }

    SetPlayerPvPState(attPlayer, true);
    SetPlayerPvPState(victPlayer, true);
}
bool ReusablePotionItemScript::OnUse(Player* player, Item* item, SpellCastTargets const& /*targets*/){
    if (!sConfigMgr->GetOption<bool>("ReusablePotion.Enable", false))
    {
        return false;
    }

    if (!sConfigMgr->GetOption<bool>("ReusablePotion.PvPEnable", false))
    {
        bool isInPvPCombat = GetPlayerPvPState(player);

        if (isInPvPCombat)
        {
            return false;
        }
    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(item->GetTemplate()->Spells[0].SpellId);
    auto effect1 = spellInfo->Effects[0].Effect;
    bool many_effects = false;

    if (effect1 == REUSE_SPELL_EFFECT_HEAL)
    {
        many_effects = true;
    }
    if (effect1 == REUSE_SPELL_EFFECT_MANA)
    {
        many_effects = true;
    }
    if (effect1 == REUSE_SPELL_EFFECT_AURA)
    {
        many_effects = true;
    }

    if (!many_effects){
        return false;
    }
    bool many_visuals = false;
    auto visual1 = spellInfo->SpellVisual[0];
    if (visual1 == REUSE_SPELL_VISUAL_POTION)
    {
        many_visuals = true;
    }
    if (visual1 == REUSE_SPELL_VISUAL_POTION_OTHER_ONE){
        many_visuals = true;
    }
    if(!many_visuals){
        return false;
    }

    player->SendCooldownEvent(spellInfo);
    player->SetLastPotionId(0);
    return true;
}
//
//void ReusablePotionUnitScript::ModifyHealReceived(Unit* target, Unit* /*healer*/, uint32& /*addHealth*/, SpellInfo const* spellInfo)
/*{
    if (!sConfigMgr->GetOption<bool>("ReusablePotion.Enable", false))
    {
        return;
    }

    if (!target)
    {
        return;
    }

    Player* player = target->ToPlayer();
    if (!player)
    {
        return;
    }

    if (!sConfigMgr->GetOption<bool>("ReusablePotion.PvPEnable", false))
    {
        bool isInPvPCombat = GetPlayerPvPState(player);

        if (isInPvPCombat)
        {
            return;
        }
    }

    auto effect1 = spellInfo->Effects[0].Effect;
    bool many_effects = false;

    if (effect1 == REUSE_SPELL_EFFECT_HEAL)
    {
        many_effects = true;
    }
    if (effect1 == REUSE_SPELL_EFFECT_MANA)
    {
        many_effects = true;
    }
    if (effect1 == REUSE_SPELL_EFFECT_AURA)
    {
        many_effects = true;
    }

    if (!many_effects){
        return;
    }
    bool many_visuals = false;
    auto visual1 = spellInfo->SpellVisual[0];
    if (visual1 == REUSE_SPELL_VISUAL_POTION)
    {
        many_visuals = true;
    }
    if (visual1 == REUSE_SPELL_VISUAL_POTION_OTHER_ONE){
        many_visuals = true;
    }
    if(!many_visuals){
        return;
    }

    player->SendCooldownEvent(spellInfo);
    player->SetLastPotionId(0);
}
*/
void AddMyReusablePotionScripts()
{
    new ReusablePotionPlayerScript();
    new ReusablePotionUnitScript();
    new ReusablePotionItemScript();
}
