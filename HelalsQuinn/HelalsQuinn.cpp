#include "PluginSDK.h"
#include "Color.h"


PluginSetup("HelalsQuinn");

IMenu* MainMenu;
IMenu* ComboMenu;
IMenu* HarassMenu;
IMenu* JungleMenu;
IMenu* Drawings;
IMenu* MiscMenu;

IMenuOption* ComboQ;
IMenuOption* ComboE;
IMenuOption* ComboR;

IMenuOption* HarassQ;
IMenuOption* HarassE;
IMenuOption* HarassManaManager;

IMenuOption* JungleQ;
IMenuOption* JungleE;
IMenuOption* JungleManaManager;

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
	MainMenu = GPluginSDK->AddMenu("HelalsQuinn");

	ComboMenu = MainMenu->AddMenu("Combo");
	HarassMenu = MainMenu->AddMenu("Harass");
	JungleMenu = MainMenu->AddMenu("Jungle Clear");
	Drawings = MainMenu->AddMenu("Drawings");

	ComboQ = ComboMenu->CheckBox("Use Q", true);
	ComboE = ComboMenu->CheckBox("Use E", true);
	ComboR = ComboMenu->CheckBox("Use R", true);

	HarassQ = HarassMenu->CheckBox("Use Q", true);
	HarassE = HarassMenu->CheckBox("Use E", true);
	HarassManaManager = HarassMenu->AddFloat("ManaManager", 0, 100, 65);

	JungleQ = JungleMenu->CheckBox("Use Q", true);
	JungleE = JungleMenu->CheckBox("Use E", true);
	JungleManaManager = JungleMenu->AddFloat("ManaManager", 0, 100, 50);

	DrawReady = Drawings->CheckBox("Draw Ready Spells", true);
	DrawQ = Drawings->CheckBox("Draw Q", true);
	DrawW = Drawings->CheckBox("Draw W", true);
	DrawE = Drawings->CheckBox("Draw E", true);
}


void LoadSpells()
{
	
	
	Q = GPluginSDK->CreateSpell2(kSlotQ, kLineCast, false, false, static_cast<eCollCollisionFlags>(kCollidesWithYasuoWall, kCollidesWithMinions));
	W = GPluginSDK->CreateSpell2(kSlotW, kTargetCast, false, false, kCollidesWithNothing);
	E = GPluginSDK->CreateSpell2(kSlotE, kTargetCast, false, false, kCollidesWithNothing);
	R = GPluginSDK->CreateSpell2(kSlotR, kTargetCast, false, false, kCollidesWithNothing);
	
}

void Combo()
{
	auto player = GEntityList->Player();
	auto target = GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, E->Range());

	if (ComboQ->Enabled() && Q->IsReady() && player->IsValidTarget(target, Q->Range()))
	{
		if(!target->HasBuff("QuinnW"))
		{
			Q->CastOnTarget(target, kHitChanceHigh);
		}
	}


	if (ComboE->Enabled() && E->IsReady() && player->IsValidTarget(target, E->Range()))
	{
		if(!target->HasBuff("QuinnW"))
		{
			E->CastOnTarget(target);
		}

	}


}

void Harass()
{
	auto player = GEntityList->Player();
	auto target = GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, Q->Range());

	if(HarassQ->Enabled() && Q->IsReady() && player->IsValidTarget(target, Q->Range()) && player->ManaPercent() >= HarassManaManager->GetFloat())
	{
		if(!target->HasBuff("QuinnW"))
		{
			Q->CastOnTarget(target, kHitChanceHigh);
		}
	}

	if(HarassE->Enabled() && E->IsReady() && player->IsValidTarget(target, E->Range()) && player->ManaPercent() >= HarassManaManager->GetFloat())
	{
		if(!target->HasBuff("QuinnW"))
		{
			E->CastOnTarget(target);
		}
	}
}

void JungleClear()
{
	auto player = GEntityList->Player();

	for (auto junglemob : GEntityList->GetAllMinions(false, false, true))
	{
		if(junglemob !=nullptr && junglemob->IsValidTarget(GEntityList->Player(), Q->Range()) && JungleQ->Enabled() && player->ManaPercent() >= JungleManaManager->GetFloat())
		{
			Q->CastOnTarget(junglemob);
		}

		if (junglemob != nullptr && junglemob->IsValidTarget(GEntityList->Player(), E->Range()) && JungleE->Enabled() && player->ManaPercent() >= JungleManaManager->GetFloat())
		{
			E->CastOnTarget(junglemob);
		}
	}
}

void Drawing()
{
	auto player = GEntityList->Player();
	
	if(DrawReady->Enabled())
	{
		if(Q->IsReady() && DrawQ->Enabled())
		{
			GRender->DrawOutlinedCircle(player->GetPosition(), Vec4(225, 225, 0, 225), Q->Range());
		}

		if(W->IsReady() && DrawW->Enabled())
		{
			GRender->DrawOutlinedCircle(player->GetPosition(), Vec4(225, 225, 0, 225), W->Range());
		}

		if(E->IsReady() && DrawE->Enabled())
		{
			GRender->DrawOutlinedCircle(player->GetPosition(), Vec4(225, 225, 0, 225), E->IsReady());
		}
	}
	else
	{
		if(DrawQ->Enabled())
		{
			GRender->DrawOutlinedCircle(player->GetPosition(), Vec4(225, 225, 0, 225), Q->Range());
		}

		if(DrawW->Enabled())
		{
			GRender->DrawOutlinedCircle(player->GetPosition(), Vec4(225, 225, 0, 225), W->Range());
		}
		if(DrawE->Enabled())
		{
			GRender->DrawOutlinedCircle(player->GetPosition(), Vec4(225, 225, 0, 225), E->Range());
		}
	}
}

PLUGIN_EVENT(void) OnRender()
{
	Drawing();
}


PLUGIN_EVENT(void) OnGameUpdate()
{
	if(GOrbwalking->GetOrbwalkingMode() == kModeCombo)
	{
		Combo();
	}
	if(GOrbwalking->GetOrbwalkingMode() == kModeMixed)
	{
		Harass();
	}

	if(GOrbwalking->GetOrbwalkingMode() == kModeLaneClear)
	{
		JungleClear();
	}

}

PLUGIN_API void OnLoad(IPluginSDK* PluginSDK)
{
	PluginSDKSetup(PluginSDK);
	Menu();
	LoadSpells();
	GEventManager->AddEventHandler(kEventOnRender, OnRender);
	GEventManager->AddEventHandler(kEventOnGameUpdate, OnGameUpdate);

	GRender->NotificationEx(Color::LightBlue().Get(), 2, true, true, "Helalmoneys Quinn v1.0 LOADED");

}

PLUGIN_API void OnUnload()
{
	MainMenu->Remove();
	GEventManager->RemoveEventHandler(kEventOnRender, OnRender);
	GEventManager->RemoveEventHandler(kEventOnGameUpdate, OnGameUpdate);

	GRender->NotificationEx(Color::LightBlue().Get(), 2, true, true, "Helalmoneys Quinn v1.0 UNLOADED");
}