#include "PluginSDK.h"
#include "Color.h"

PluginSetup("HelalsAshe");

IMenu* MainMenu;
IMenu* ComboMenu;
IMenu* HarassMenu;
IMenu* LanceClearMenu;
IMenu* JungleClearMenu;
IMenu* Drawings;
IMenu* MiscMenu;

IMenuOption* ComboQ;
IMenuOption* ComboW;
IMenuOption* ComboR;

IMenuOption* HarassW;
IMenuOption* HarassManaManager;

IMenuOption* LaneClearW;
IMenuOption* LaneManaManager;

IMenuOption* JungleQ;
IMenuOption* JungleW;

IMenuOption* SemiR;
IMenuOption* SemiRToggle;

ISpell2* Q;
ISpell2* W;
ISpell2* E;
ISpell2* R;

void Menu()
{
	MainMenu = GPluginSDK->AddMenu("HelalsAshe");

	ComboMenu = MainMenu->AddMenu("Combo");
	HarassMenu = MainMenu->AddMenu("Harass");
	LanceClearMenu = MainMenu->AddMenu("LaneClear");
	JungleClearMenu = MainMenu->AddMenu("JungleClear");
	
	
	ComboQ = ComboMenu->CheckBox("Use Q", true);
	ComboW = ComboMenu->CheckBox("Use W", true);
	ComboR = ComboMenu->CheckBox("Use R", true);

	HarassW = HarassMenu->CheckBox("Use W", true);
	HarassManaManager = HarassMenu->AddFloat("ManaManager", 0, 100, 65);

	LaneClearW = LanceClearMenu->CheckBox("Use W", true);
	LaneManaManager = LanceClearMenu->AddFloat("ManaManager", 0, 100, 65);

	JungleQ = JungleClearMenu->CheckBox("Use Q", true);
	JungleW = JungleClearMenu->CheckBox("Use W", true);

	MiscMenu = MainMenu->AddMenu("R Settings");
	{
		SemiR = MiscMenu->CheckBox("Use Semi R", true);
		SemiRToggle = MiscMenu->AddKey("Toggle", 71);
	}
	
}

void LoadSpells()
{
	Q = GPluginSDK->CreateSpell2(kSlotQ, kTargetCast, false, false, kCollidesWithNothing);
	Q->SetOverrideRange(600);
	W = GPluginSDK->CreateSpell2(kSlotW, kConeCast, false, false, kCollidesWithYasuoWall);
	E = GPluginSDK->CreateSpell2(kSlotE, kLineCast, false, false, kCollidesWithNothing);
	R = GPluginSDK->CreateSpell2(kSlotR, kLineCast, false, false, kCollidesWithYasuoWall);
}

void Combo()
{
	auto player = GEntityList->Player();
	auto target = GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, Q->Range());

	if(ComboQ->Enabled() && Q->IsReady() && player->IsValidTarget(target, Q->Range()))
	{
		Q->CastOnPlayer();
	}

	if(ComboW->Enabled() && W->IsReady() && player->IsValidTarget(target, W->Range()))
	{
		W->CastOnTarget(target, kHitChanceHigh);
	}

	if(ComboR->Enabled() && R->IsReady() && player->IsValidTarget(target, 1500))
	{
		R->CastOnTarget(target, kHitChanceHigh);
	}
}


void Harass()
{
	auto player = GEntityList->Player();
	auto target = GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, W->Range());

	if(HarassW->Enabled() && W->IsReady() && player->IsValidTarget(target, W->Range()) && player->ManaPercent() >= HarassManaManager->GetFloat())
	{
		W->CastOnTarget(target, kHitChanceHigh);
	}
}

void LaneClear()
{
	auto player = GEntityList->Player();

	for (auto minion : GEntityList->GetAllMinions(false, true, false))
	{
		if(minion != nullptr && minion->IsValidTarget(GEntityList->Player(), W->Range()) && LaneClearW->Enabled() && W->IsReady() && player->ManaPercent() >= LaneManaManager->GetFloat())
		{
			if(!minion->IsDead())
			{
				W->CastOnTarget(minion);
			}
		}
	}
}

void JungleClear()
{
	auto player = GEntityList->Player();

	for (auto junglemob : GEntityList->GetAllMinions(false, false, true)) {

		if(junglemob != nullptr && junglemob->IsValidTarget(GEntityList->Player(), 600) && JungleQ->Enabled() && Q->IsReady())
		{
			if(!junglemob->IsDead() && junglemob->IsValidTarget())
			{
				Q->CastOnPlayer();
			}
			
		}

		if (junglemob != nullptr && junglemob->IsValidTarget(GEntityList->Player(), W->Range()) && JungleW->Enabled() && W->IsReady())
		{
			if(!junglemob->IsDead() && junglemob->IsValidTarget())
			{
				W->CastOnTarget(junglemob);
			}
			
		}


	}
}

void ManualR()
{

	auto player = GEntityList->Player();
	auto target = GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, 1500);

	if(R->IsReady())
	{
		if(GetAsyncKeyState(SemiRToggle->GetInteger()))
		{
			if (GGame->IssueOrder(GEntityList->Player(), kMoveTo, GGame->CursorPosition()))
			{
				if(SemiR->Enabled() && R->IsReady() && player->IsValidTarget(target, 1500))
				{
					R->CastOnTarget(target, kHitChanceHigh);
				}
			}
		}
	}
}

PLUGIN_EVENT(void) OnGameUpdate()
{

	ManualR();

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
		LaneClear();
		JungleClear();
	}

	
}

PLUGIN_API void OnLoad(IPluginSDK* PluginSDK)
{
	PluginSDKSetup(PluginSDK);
	Menu();
	LoadSpells();

	GEventManager->AddEventHandler(kEventOnGameUpdate, OnGameUpdate);

	GRender->NotificationEx(Color::LightBlue().Get(), 2, true, true, "Helalmoneys Ashe v1.0 LOADED");
}

PLUGIN_API void OnUnload()
{
	MainMenu->Remove();

	GEventManager->RemoveEventHandler(kEventOnGameUpdate, OnGameUpdate);

	GRender->NotificationEx(Color::LightBlue().Get(), 2, true, true, "Ashe UNLOADED");
}