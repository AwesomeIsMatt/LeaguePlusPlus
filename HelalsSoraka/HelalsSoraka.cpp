#include "PluginSDK.h"
#include "Color.h"

PluginSetup("HelalsSoraka");

IMenu* MainMenu;
IMenu* ComboMenu;
IMenu* HealingMenu;
IMenu* HarassMenu;
IMenu* RSettings;
IMenu* RWhitelist;
IMenu* Drawings;

IMenuOption* ComboQ;
IMenuOption* ComboE;

IMenuOption* HarassQ;
IMenuOption* HarassE;

IMenuOption* HealW;
IMenuOption* HealManager;

IMenuOption* HealR;
IMenuOption* RHealManager;

IMenuOption* DrawReady;
IMenuOption* DrawQ;
IMenuOption* DrawW;
IMenuOption* DrawE;

ISpell2* Q;
ISpell2* W;
ISpell2* E;
ISpell2* R;

void Menu()
{
	MainMenu = GPluginSDK->AddMenu("HelalsSoraka");

	ComboMenu = MainMenu->AddMenu("Combo");
	{
		ComboQ = ComboMenu->CheckBox("Use Q", true);
		ComboE = ComboMenu->CheckBox("Use E", true);

	}

	HealingMenu = MainMenu->AddMenu("Healing");
	{
		HealW = HealingMenu->CheckBox("Use W", true);
		HealManager = HealingMenu->AddInteger("Heal", 0, 100, 65);
	}

	HarassMenu = MainMenu->AddMenu("Harass");
	{
		HarassQ = HarassMenu->CheckBox("Use Q", true);
		HarassE = HarassMenu->CheckBox("Use E", true);
	}

	RSettings = MainMenu->AddMenu("R Heal Settings");
	{
		HealR = RSettings->CheckBox("Use R", true);
		RHealManager = RSettings->AddInteger("Use R if HP % <", 0, 100, 30);

		RWhitelist = RSettings->AddMenu("R Whitelist");
		for (auto ally : GEntityList->GetAllHeros(true, false))
		{
			RWhitelist->CheckBox(ally->ChampionName(), true);
		}
	}

	Drawings = MainMenu->AddMenu("Drawings");
	{
		DrawReady = Drawings->CheckBox("Draw Ready Spells", true);
		DrawQ = Drawings->CheckBox("Draw Q", true);
		DrawW = Drawings->CheckBox("Draw W", true);
		DrawE = Drawings->CheckBox("Draw E", true);
	}



}

void LoadSpells()
{
	Q = GPluginSDK->CreateSpell2(kSlotQ, kCircleCast, false, false, kCollidesWithNothing);
	W = GPluginSDK->CreateSpell2(kSlotW, kTargetCast, false, false, kCollidesWithNothing);
	E = GPluginSDK->CreateSpell2(kSlotE, kCircleCast, false, false, kCollidesWithNothing);
	R = GPluginSDK->CreateSpell2(kSlotR, kTargetCast, false, false, kCollidesWithNothing);
}

void Combo()
{
	auto player = GEntityList->Player();
	auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());

	if (ComboQ->Enabled() && Q->IsReady() && player->IsValidTarget(target, Q->Range()))
	{
		Q->CastOnTarget(target, kHitChanceHigh);
	}

	if (ComboE->Enabled() && E->IsReady() && player->IsValidTarget(target, E->Range()))
	{
		E->CastOnTarget(target, kHitChanceHigh);
	}

}

void Harass()
{
	auto player = GEntityList->Player();
	auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());

	if (HarassQ->Enabled() && Q->IsReady() && player->IsValidTarget(target, Q->Range()))
	{
		Q->CastOnTarget(target, kHitChanceHigh);
	}

	if (HarassE->Enabled() && E->IsReady() && player->IsValidTarget(target, E->Range()))
	{
		E->CastOnTarget(target, kHitChanceHigh);
	}

}

void Healings()
{

	auto player = GEntityList->Player();

	for(auto ally : GEntityList->GetAllHeros(true, false))
	{
		if(ally != GEntityList->Player())
		{
			if(HealW->Enabled() && W->IsReady() && ally->IsValidTarget(GEntityList->Player(), W->Range()))
			{
				if(ally->HealthPercent() < HealManager->GetInteger())
				{
					if(!GUtility->IsPositionInFountain(ally->GetPosition(), true, false))
					{
						W->CastOnUnit(ally);
					}
					
				}
			}
		}
	}


}

void RGlobal()
{
	auto player = GEntityList->Player();

	for (auto ally : GEntityList->GetAllHeros(true, false))
	{
		IMenuOption * temp = RWhitelist->GetOption(ally->ChampionName());

		if (HealR->Enabled() && R->IsReady() && player->HealthPercent() <= RHealManager->GetInteger() || ally->HealthPercent() <= RHealManager->GetInteger())
		{
			if (temp->Enabled())
			{
				if (!GUtility->IsPositionInFountain(ally->GetPosition(), true, false) && !GUtility->IsPositionInFountain(player->GetPosition(), true, false))
				{
					R->CastOnPlayer();
				}
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
	}
}

PLUGIN_EVENT(void) OnGameUpdate()
{

	RGlobal();
	Healings();

	if (GOrbwalking->GetOrbwalkingMode() == kModeCombo)
	{
		Combo(); 
		Healings();
	}

	if (GOrbwalking->GetOrbwalkingMode() == kModeMixed)
	{
		Harass();
	}


}

PLUGIN_EVENT(void) OnRender()
{
	Drawing();
}


PLUGIN_API void OnLoad(IPluginSDK* PluginSDK)
{
	PluginSDKSetup(PluginSDK);
	Menu();
	LoadSpells();

	GEventManager->AddEventHandler(kEventOnRender, OnRender);
	GEventManager->AddEventHandler(kEventOnGameUpdate, OnGameUpdate);

	GRender->NotificationEx(Color::LightBlue().Get(), 2, true, true, "Helalmoneys Soraka v1.0 LOADED");
}

PLUGIN_API void OnUnload()
{
	MainMenu->Remove();

	GEventManager->RemoveEventHandler(kEventOnRender, OnRender);
	GEventManager->RemoveEventHandler(kEventOnGameUpdate, OnGameUpdate);

	GRender->NotificationEx(Color::LightRed().Get(), 2, true, true, "Soraka UNLOADED");
}