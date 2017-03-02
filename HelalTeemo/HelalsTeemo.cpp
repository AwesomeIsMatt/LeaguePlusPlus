#include "PluginSDK.h"
#include "Color.h"

PluginSetup("HelalsTeemo");

IMenu* MainMenu;
IMenu* ComboMenu;
IMenu* HarassMenu;
IMenu* LastHitMenu;
IMenu* JungleClear;
IMenu* Drawings;

IMenuOption* ComboQ;
IMenuOption* ComboW;
IMenuOption* HarrasQ;
IMenuOption* HarrasW;
IMenuOption* WManaManager;
IMenuOption* LastHitQ;
IMenuOption* JungleQ;

IMenuOption* DrawReady;
IMenuOption* DrawQ;
IMenuOption* DrawR;

ISpell2* Q;
ISpell2* W;
ISpell2* R;

void Menu()
{
	MainMenu = GPluginSDK->AddMenu("HelalsTeemo");

	ComboMenu = MainMenu->AddMenu("Combo");
	HarassMenu = MainMenu->AddMenu("Harras");
	LastHitMenu = MainMenu->AddMenu("LastHit");
	JungleClear = MainMenu->AddMenu("JungleClear");
	Drawings = MainMenu->AddMenu("Drawings");
	

	ComboQ = ComboMenu->CheckBox("Use Q", true);
	ComboW = ComboMenu->CheckBox("Use W", true);

	HarrasQ = HarassMenu->CheckBox("Use Q", true);
	//HarrasW = HarassMenu->CheckBox("Use W", true);
	WManaManager = HarassMenu->AddFloat("ManaManager for Q", 0, 100, 65);

	// Muss noch gemacht werden
	LastHitQ = LastHitMenu->CheckBox("Use Q for LastHit (soonTM)", true);

	// Muss noch gemacht werden
	JungleQ = JungleClear->CheckBox("Use Q for Jungle (soonTM)", true);

	DrawReady = Drawings->CheckBox("Draw Ready Spells", true);
	DrawQ = Drawings->CheckBox("Draw Q", true);
	DrawR = Drawings->CheckBox("Draw R", true);


}

void LoadSpells()
{
	Q = GPluginSDK->CreateSpell2(kSlotQ, kTargetCast, false, false, kCollidesWithWalls);
	W = GPluginSDK->CreateSpell2(kSlotW, kCircleCast, false, false, kCollidesWithNothing);
	R = GPluginSDK->CreateSpell2(kSlotR, kCircleCast, false, true, kCollidesWithNothing);
	Q->SetOverrideRange(680);
	W->SetOverrideRange(850);
	R->SetOverrideRange(400);
	
}

void Combo()
{
	auto player = GEntityList->Player();
	auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
	
	if(ComboQ->Enabled() && Q->IsReady() && player->IsValidTarget(target, Q->Range()))
	{
		Q->CastOnTarget(target);
	}

	if(ComboW->Enabled() && W->IsReady() && player->IsValidTarget(target, W->Range()))
	{
		W->CastOnTarget(target);
	}
}

void Harass()
{
	auto player = GEntityList->Player();
	auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());

	if (HarrasQ->Enabled() && Q->IsReady() && player->IsValidTarget(target, Q->Range()) && player->ManaPercent() >= WManaManager->GetFloat())
	{
		Q->CastOnTarget(target);
	}

	//Checken ob das sinnvoll ist
	/*if (HarrasW->Enabled() && W->IsReady() && player->IsValidTarget(target, W->Range()))
	{
		W->CastOnPlayer();
	}*/
}

//TOMORROW BIK
void Lasthit()
{

}

//MUSS NOCH GEMACHT WERDEN
void JungleC()
{

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
	if(GOrbwalking->GetOrbwalkingMode() == kModeCombo)
	{
		Combo();
	}

	if(GOrbwalking->GetOrbwalkingMode() == kModeMixed)
	{
		Harass();
	}
}



PLUGIN_API void OnLoad(IPluginSDK* PluginSDK)
{
	PluginSDKSetup(PluginSDK);
	Menu();
	LoadSpells();
	GEventManager->AddEventHandler(kEventOnRender, OnRender);
	GEventManager->AddEventHandler(kEventOnGameUpdate, OnGameUpdate);

	GRender->NotificationEx(Color::Green().Get(), 2, true, true, "Helalmoneys Teemo v0.5 LOADED");
	GGame->PrintChat("LastHit, JungleClear... soonTM");
	
	
}

PLUGIN_API void OnUnload(IPluginSDK* IPluginSDK)
{
	MainMenu->Remove();
	GEventManager->RemoveEventHandler(kEventOnRender, OnRender);
	GEventManager->RemoveEventHandler(kEventOnGameUpdate, OnGameUpdate);

	GRender->NotificationEx(Color::Green().Get(), 2, true, true, "Helalmoneys Teemo v0.5 UNLOADED");
	
}
