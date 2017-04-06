#include "PluginSDK.h"
#include "Color.h"

PluginSetup("HelalsKindred");

IMenu* MainMenu;
IMenu* ComboMenu;
IMenu* HarassMenu;
IMenu* LaneClearMenu;
IMenu* JungleClearMenu;
IMenu* KindredUlt;
IMenu* MiscsMenu;
IMenu* Drawings;

IMenuOption* ComboQ;
IMenuOption* ComboW;
IMenuOption* ComboE;

IMenuOption* HarassQ;

IMenuOption* LaneQ;
IMenuOption* LaneW;
IMenuOption* LaneMinion;

IMenuOption* JungleQ;
IMenuOption* JungleW;
IMenuOption* JungleE;
IMenuOption* JungleManager;

IMenuOption* KindredR;
IMenuOption* RHP;

IMenuOption* EGap;

IMenuOption* DrawReady;
IMenuOption* DrawQ;
IMenuOption* DrawW;
IMenuOption* DrawE;
IMenuOption* DrawR;

ISpell2* Q;
ISpell2* W;
ISpell2* E;
ISpell2* R;

IUnit* player;

void Menu()
{
	MainMenu = GPluginSDK->AddMenu("HelalsKindred");

	ComboMenu = MainMenu->AddMenu("Combo");
	{
		ComboQ = ComboMenu->CheckBox("Use Q", true);
		ComboW = ComboMenu->CheckBox("Use W", true);
		ComboE = ComboMenu->CheckBox("Use E", true);
	}

	HarassMenu = MainMenu->AddMenu("Harass");
	{
		HarassQ = HarassMenu->CheckBox("Use Q", true);

	}

	LaneClearMenu = MainMenu->AddMenu("LaneClear");
	{
		LaneQ = LaneClearMenu->CheckBox("Use Q", true);
		LaneW = LaneClearMenu->CheckBox("Use W", true);
		LaneMinion = LaneClearMenu->AddInteger("Min. Minions for casting W", 1, 5, 3);
	}

	JungleClearMenu = MainMenu->AddMenu("JungleClear");
	{
		JungleQ = JungleClearMenu->CheckBox("Use Q", true);
		JungleW = JungleClearMenu->CheckBox("Use W", true);
		JungleE = JungleClearMenu->CheckBox("Use E", true);
		JungleManager = JungleClearMenu->AddInteger("ManaManager", 0, 100, 30);
	}

	KindredUlt = MainMenu->AddMenu("Use R");
	{
		KindredR = KindredUlt->CheckBox("R WhiteList", true);
		RHP = KindredUlt->AddInteger("Use R < HP %", 0, 100, 20);
		for (auto ally : GEntityList->GetAllHeros(true, false))
		{
			KindredUlt->CheckBox(ally->ChampionName(), true);
		}
	}

	MiscsMenu = MainMenu->AddMenu("Miscs");
	{
		EGap = MiscsMenu->CheckBox("Use E for AntiGap", true);
	}

	Drawings = MainMenu->AddMenu("Drawings");
	{
		DrawReady = Drawings->CheckBox("Draw Ready Spells", true);
		DrawQ = Drawings->CheckBox("Draw Q", true);
		DrawW = Drawings->CheckBox("Draw W", true);
		DrawE = Drawings->CheckBox("Draw E", true);
		DrawR = Drawings->CheckBox("Draw R", true);
	}
}

void LoadSpells()
{
	Q = GPluginSDK->CreateSpell2(kSlotQ, kTargetCast, false, false, kCollidesWithYasuoWall);
	W = GPluginSDK->CreateSpell2(kSlotW, kTargetCast, false, false, kCollidesWithNothing);
	E = GPluginSDK->CreateSpell2(kSlotE, kTargetCast, false, false, kCollidesWithNothing);
	R = GPluginSDK->CreateSpell2(kSlotR, kTargetCast, false, false, kCollidesWithNothing);

	R->SetOverrideRange(500);
}

int GetMinionsInRange(float range)
{
	auto minions = GEntityList->GetAllMinions(false, true, false);
	auto minionsinrange = 0;

	for (auto minion : minions)
	{
		if (minion != nullptr && minion->GetTeam() != GEntityList->Player()->GetTeam())
		{
			auto flDistance = (minion->GetPosition() - GEntityList->Player()->GetPosition()).Length();
			if (flDistance < range)
			{
				minionsinrange++;
			}
		}
	}
	return minionsinrange;
}

void Combo()
{
	auto player = GEntityList->Player();
	auto target = GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, W->Range());

	if(ComboE->Enabled() && E->IsReady() && player->IsValidTarget(target, E->Range()))
	{
		E->CastOnUnit(target);
	}

	if(ComboW->Enabled() && W->IsReady() && player->IsValidTarget(target, W->Range()))
	{
		W->CastOnPlayer();
	}

	if(ComboQ->Enabled() && Q->IsReady() && player->IsValidTarget(target, W->Range()))
	{
		Q->CastOnPosition(GGame->CursorPosition());
	}

}

void Harass()
{
	auto player = GEntityList->Player();
	auto target = GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, Q->Range());

	if(HarassQ->Enabled() && Q->IsReady() && player->IsValidTarget(target, Q->Range()))
	{
		Q->CastOnPosition(GGame->CursorPosition());
	}
}


void LaneClear()
{
	auto player = GEntityList->Player();

	for (auto minion : GEntityList->GetAllMinions(false, true, false))
	{
		if(minion != nullptr && minion->IsValidTarget(GEntityList->Player(), W->Range()) && LaneQ->Enabled() && Q->IsReady())
		{
			if(!minion->IsDead() && minion->IsCreep())
			{
				Q->CastOnPosition(GGame->CursorPosition());
			}
			
		}

		if(minion != nullptr && minion->IsValidTarget(GEntityList->Player(), W->Range()) && LaneW->Enabled() && W->IsReady())
		{
			if(!minion->IsDead() && minion->IsCreep())
			{
				Vec3 pos;
				int minions;
				W->FindBestCastPosition(true, false, pos, minions);
				if(minions >= LaneMinion->GetInteger())
				{
					W->CastOnPosition(pos);
				}
			}
		}
	}
}

void JungleClear()
{
	auto player = GEntityList->Player();

	for(auto junglemob : GEntityList->GetAllMinions(false, false, true))
	{
		if(junglemob != nullptr && junglemob->IsValidTarget(player, E->Range()) && JungleE->Enabled() && E->IsReady())
		{
			if(!junglemob->IsDead() && junglemob->IsValidTarget() && player->ManaPercent() >= JungleManager->GetInteger())
			{
				E->CastOnTarget(junglemob);
			}
		}

		if (junglemob != nullptr && junglemob->IsValidTarget(player, W->Range()) && JungleW->Enabled() && W->IsReady() && player->ManaPercent() >= JungleManager->GetInteger())
		{
			if (!junglemob->IsDead() && junglemob->IsValidTarget())
			{
				W->CastOnPlayer();
			}
		}

		if (junglemob != nullptr && junglemob->IsValidTarget(player, W->Range()) && JungleQ->Enabled() && Q->IsReady() && player->ManaPercent() >= JungleManager->GetInteger())
		{
			if (!junglemob->IsDead() && junglemob->IsValidTarget())
			{
				Q->CastOnPosition(GGame->CursorPosition());
			}
		}

	}
}

void KindredUltimate()
{
	if (!R->IsReady() && !KindredR->Enabled())
		return;

	auto player = GEntityList->Player();

	for(auto ally : GEntityList->GetAllHeros(true, false))
	{
		IMenuOption * temp = KindredUlt->GetOption(ally->ChampionName());

		if(KindredR->Enabled() && R->IsReady() && player->HealthPercent() <= RHP->GetInteger() && temp->Enabled() && player->IsValidTarget(ally, R->Range()))
		{
			if (!GUtility->IsPositionInFountain(ally->GetPosition(), true, false) && !GUtility->IsPositionInFountain(player->GetPosition(), true, false))
			{
				R->CastOnUnit(ally);
			}
		}
	}

}



void Drawing()
{
	auto player = GEntityList->Player();
	if (DrawReady->Enabled())
	{
		if (Q->IsReady() && DrawQ->Enabled())
		{
			GRender->DrawOutlinedCircle(player->GetPosition(), Vec4(225, 225, 0, 225), Q->Range());
		}

		if (W->IsReady() && DrawW->Enabled())
		{
			GRender->DrawOutlinedCircle(player->GetPosition(), Vec4(225, 225, 0, 225), W->Range());
		}

		if (E->IsReady() && DrawE->Enabled())
		{
			GRender->DrawOutlinedCircle(player->GetPosition(), Vec4(225, 225, 0, 225), E->Range());
		}

		if (R->IsReady() && DrawR->Enabled())
		{
			GRender->DrawOutlinedCircle(player->GetPosition(), Vec4(225, 225, 0, 225), R->Range());
		}
	}

	else
	{
		if (DrawQ->Enabled())
		{
			GRender->DrawOutlinedCircle(player->GetPosition(), Vec4(225, 225, 0, 225), Q->Range());
		}

		if (DrawW->Enabled())
		{
			GRender->DrawOutlinedCircle(player->GetPosition(), Vec4(225, 225, 0, 225), W->Range());
		}

		if (DrawE->Enabled())
		{
			GRender->DrawOutlinedCircle(player->GetPosition(), Vec4(225, 225, 0, 225), E->Range());
		}

		if (DrawR->Enabled())
		{
			GRender->DrawOutlinedCircle(player->GetPosition(), Vec4(225, 225, 0, 225), R->Range());
		}
	}
}

PLUGIN_EVENT(void) OnRender()
{
	Drawing();
}

PLUGIN_EVENT(void) OnGameUpdate()
{

	KindredUltimate();

	if (GOrbwalking->GetOrbwalkingMode() == kModeCombo)
	{
		Combo();
	}

	if (GOrbwalking->GetOrbwalkingMode() == kModeMixed)
	{
		Harass();
	}

	if (GOrbwalking->GetOrbwalkingMode() == kModeLaneClear)
	{
		LaneClear();
		JungleClear();
	}

}

PLUGIN_API void OnLoad(IPluginSDK* PluginSDK)
{
	PluginSDKSetup(PluginSDK);
	player = GEntityList->Player();
	Menu();
	LoadSpells();
	GEventManager->AddEventHandler(kEventOnRender, OnRender);
	GEventManager->AddEventHandler(kEventOnGameUpdate, OnGameUpdate);

	if (strcmp(player->ChampionName(), "Kindred") == 0) {
		GRender->NotificationEx(Color::Green().Get(), 2, true, true, "Helalmoneys Kindred v1.0 LOADED");
	}
	else {
		GRender->NotificationEx(Color::Red().Get(), 2, true, true, "Kindred unloaded. You dont play the Champion!");
	}

}

PLUGIN_API void OnUnload()
{
	MainMenu->Remove();
	GEventManager->RemoveEventHandler(kEventOnRender, OnRender);
	GEventManager->RemoveEventHandler(kEventOnGameUpdate, OnGameUpdate);

	GRender->NotificationEx(Color::LightRed().Get(), 2, true, true, "Kindred UNLOADED");

}