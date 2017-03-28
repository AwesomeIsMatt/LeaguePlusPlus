#include "PluginSDK.h"
#include "Color.h"

PluginSetup("HelalsKayle");

IMenu* MainMenu;
IMenu* ComboMenu;
IMenu* HarassMenu;
IMenu* LaneClearMenu;
IMenu* JungleClearMenu;
IMenu* RSettings;
IMenu* Drawings;
IMenu* MiscMenu;

IMenuOption* ComboQ;
IMenuOption* ComboW;
IMenuOption* ComboE;
IMenuOption* ComboR;
IMenuOption* RKayle;

IMenuOption* HarassE;
IMenuOption* HarassManaManager;

IMenuOption* LaneClearE;
IMenuOption* LaneClearManaManager;

IMenuOption* JungleClearQ;
IMenuOption* JungleClearE;
IMenuOption* JungleClearManaManager;

IMenuOption* RforAlly;
IMenuOption* RAlly;
IMenuOption* WHeal;
IMenuOption* WHealer;

IMenuOption* DrawReady;
IMenuOption* DrawQ;
IMenuOption* DrawW;
IMenuOption* DrawE;
IMenuOption* DrawR;

ISpell2* Q;
ISpell2* W;
ISpell2* E;
ISpell2* R;

void Menu()
{
	MainMenu = GPluginSDK->AddMenu("HelalsKayle");

	ComboMenu = MainMenu->AddMenu("Combo");
	HarassMenu = MainMenu->AddMenu("Harass");
	LaneClearMenu = MainMenu->AddMenu("LaneClear");
	JungleClearMenu = MainMenu->AddMenu("JungleClear");
	Drawings = MainMenu->AddMenu("Drawings");
	MiscMenu = MainMenu->AddMenu("Miscs");

	ComboQ = ComboMenu->CheckBox("Use Q", true);
	ComboW = ComboMenu->CheckBox("Use W", true);
	ComboE = ComboMenu->CheckBox("Use E", true);
	ComboR = ComboMenu->CheckBox("Use R for Kayle only", true);
	RKayle = ComboMenu->AddFloat("Use R when % Percent HP", 0, 100, 20);

	HarassE = HarassMenu->CheckBox("Use E", true);
	HarassManaManager = HarassMenu->AddFloat("ManaManager for E", 0, 100, 65);

	LaneClearE = LaneClearMenu->CheckBox("Use E", true);
	LaneClearManaManager = LaneClearMenu->AddFloat("ManaManager for E", 0, 100, 65);

	JungleClearQ = JungleClearMenu->CheckBox("Use Q", true);
	JungleClearE = JungleClearMenu->CheckBox("Use E", true);
	JungleClearManaManager = JungleClearMenu->AddFloat("ManaManager", 0, 100, 65);

	RforAlly = MiscMenu->CheckBox("Use R for Ally", true);
	RAlly = MiscMenu->AddFloat("Use R when % Percent HP", 0, 100, 20);
	WHeal = MiscMenu->CheckBox("Use W for Heal", true);
	WHealer = MiscMenu->AddFloat("Use W when % Percent HP", 0, 100, 20);

	DrawReady = Drawings->CheckBox("Draw Ready Spells", true);
	DrawQ = Drawings->CheckBox("Draw Q", true);
	DrawW = Drawings->CheckBox("Draw W", true);
	DrawE = Drawings->CheckBox("Draw E", true);
	DrawR = Drawings->CheckBox("Draw R", true);

	RSettings = MainMenu->AddMenu("R Settings");

	// by Kornis
	for(auto ally : GEntityList->GetAllHeros(true, false))
	{
		RSettings->CheckBox(ally->ChampionName(), true);
	}

}

void LoadSpells()
{

	Q = GPluginSDK->CreateSpell2(kSlotQ, kTargetCast, false, false, kCollidesWithWalls);
	W = GPluginSDK->CreateSpell2(kSlotW, kTargetCast, false, false, kCollidesWithNothing);
	E = GPluginSDK->CreateSpell2(kSlotE, kTargetCast, false, true, kCollidesWithWalls);
	R = GPluginSDK->CreateSpell2(kSlotR, kTargetCast, false, false, kCollidesWithNothing);

	Q->SetOverrideRange(650);
	W->SetOverrideRange(900);
	E->SetOverrideRange(600);
}

void Combo()
{
	auto player = GEntityList->Player();
	auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, W->Range());

	if (ComboQ->Enabled() && Q->IsReady() && player->IsValidTarget(target, Q->Range()))
	{
		Q->CastOnTarget(target);
	}


	if (ComboW->Enabled() && W->IsReady() && player->IsValidTarget(target, W->Range()))
	{
		W->CastOnPlayer();
	}

	if (ComboE->Enabled() && E->IsReady() && player->IsValidTarget(target, E->Range()))
	{
		E->CastOnTarget(target);
	}


}

void Harass()
{
	auto player = GEntityList->Player();
	auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());

	if (HarassE->Enabled() && E->IsReady() && player->IsValidTarget(target, E->Range()) && player->ManaPercent() >= HarassManaManager->GetFloat())
	{
		E->CastOnTarget(target);
	}
}

void LaneClear()
{
	auto player = GEntityList->Player();

	for (auto minion : GEntityList->GetAllMinions(false, true, false))
	{
		if (minion != nullptr && minion->IsValidTarget(GEntityList->Player(), E->Range()) && LaneClearE->Enabled() && player->ManaPercent() >= LaneClearManaManager->GetFloat())
		{
			E->CastOnTarget(minion);
		}
	}

}

void JungleClear() {

	auto player = GEntityList->Player();

	for (auto junglemob : GEntityList->GetAllMinions(false, false, true)) {

		if(junglemob != nullptr && junglemob->IsValidTarget(GEntityList->Player(), E->Range()) && JungleClearE->Enabled() && player->ManaPercent() >= JungleClearManaManager->GetFloat())
		{
			E->CastOnTarget(junglemob);
		}

		if(junglemob !=nullptr && junglemob->IsValidTarget(GEntityList->Player(), Q->Range()) && JungleClearQ->Enabled() && player->ManaPercent() >= JungleClearManaManager->GetFloat())
		{
			Q->CastOnTarget(junglemob);
		}

	}

}

void KayleR()
{

	if (!R->IsReady() && !RKayle->Enabled())
		return;

	auto player = GEntityList->Player();

	for(auto ally : GEntityList->GetAllHeros(true, false))
	{

		IMenuOption * temp = RSettings->GetOption(ally->ChampionName());

			if (ComboR->Enabled() && R->IsReady() && player->HealthPercent() <= RKayle->GetFloat() && temp ->Enabled())
			{
				if (!GUtility->IsPositionInFountain(ally->GetPosition(), true, false) && !GUtility->IsPositionInFountain(player->GetPosition(), true, false))
				{
					R->CastOnPlayer();
				}
				
			}
		
	}

}


void AllyR()
{
	if (!R->IsReady() && !RAlly->Enabled())
		return;

	auto player = GEntityList->Player();

	for (auto ally : GEntityList->GetAllHeros(true, false))
	{
		for (auto allyR : GEntityList->GetAllHeros(true, false))
		{

			IMenuOption * temp = RSettings->GetOption(ally->ChampionName());

			if (player->IsValidTarget(ally, R->Range()) && R->IsReady() && RforAlly->Enabled() && ally->HealthPercent() <= RAlly->GetFloat() && temp->Enabled())
			{
				if(!GUtility->IsPositionInFountain(ally->GetPosition(), true, false) && !GUtility->IsPositionInFountain(player->GetPosition(), true, false))
				{
					R->CastOnTarget(ally);
				}
				
			}
		}

	}
}

void WHeals()
{
	auto player = GEntityList->Player();

	for (auto ally : GEntityList->GetAllHeros(true, false))
	{
		if (player->IsValidTarget(ally, W->Range()) && W->IsReady() && WHeal->Enabled() && ally->HealthPercent() <= WHealer->GetFloat())
		{
			if (!GUtility->IsPositionInFountain(ally->GetPosition(), true, false) && !GUtility->IsPositionInFountain(player->GetPosition(), true, false))
			{
				W->CastOnTarget(ally);
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


	KayleR();
	AllyR();
	WHeals();
}


PLUGIN_API void OnLoad(IPluginSDK* PluginSDK)
{
	PluginSDKSetup(PluginSDK);
	Menu();
	LoadSpells();
	GEventManager->AddEventHandler(kEventOnRender, OnRender);
	GEventManager->AddEventHandler(kEventOnGameUpdate, OnGameUpdate);

	GRender->NotificationEx(Color::LightBlue().Get(), 2, true, true, "Helalmoneys Kayle v2.1 LOADED");
}

PLUGIN_API void OnUnload()
{
	MainMenu->Remove();
	GEventManager->RemoveEventHandler(kEventOnRender, OnRender);
	GEventManager->RemoveEventHandler(kEventOnGameUpdate, OnGameUpdate);

	GRender->NotificationEx(Color::LightRed().Get(), 2, true, true, "Helalmoneys Kayle UNLOADED");

}