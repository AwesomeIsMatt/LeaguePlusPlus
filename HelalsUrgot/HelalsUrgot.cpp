#include "PluginSDK.h"
#include "Color.h"

PluginSetup("HelalsUrgot");

IMenu* MainMenu;
IMenu* ComboMenu;
IMenu* HarassMenu;
IMenu* LaneClearMenu;
IMenu* LastHitMenu;;
IMenu* JungleClearMenu;
IMenu* MiscsMenu;
IMenu* Drawings;

IMenuOption* ComboQ;
IMenuOption* ComboW;
IMenuOption* ComboE;

IMenuOption* HarassQ;
IMenuOption* HarassManaManager;

IMenuOption* LaneClearQ;
IMenuOption* LaneClearManaManager;

IMenuOption* LastQ;

IMenuOption* AutoTear;
IMenuOption* AutoTearManager;

IMenuOption* DrawReady;
IMenuOption* DrawQ;
IMenuOption* DrawW;
IMenuOption* DrawE;
IMenuOption* DrawR;

ISpell2* Q;
ISpell2* Q2;
ISpell2* W;
ISpell2* E;
ISpell2* R;

void Menu()
{
	MainMenu = GPluginSDK->AddMenu("HelalsUrgot");
	
	ComboMenu = MainMenu->AddMenu("Combo");
	HarassMenu = MainMenu->AddMenu("Harass");
	LaneClearMenu = MainMenu->AddMenu("LaneClear");
	LastHitMenu = MainMenu->AddMenu("Lasthit");
	MiscsMenu = MainMenu->AddMenu("Miscs");
	Drawings = MainMenu->AddMenu("Drawings");

	ComboQ = ComboMenu->CheckBox("Use Q", true);
	ComboW = ComboMenu->CheckBox("Use W", true);
	ComboE = ComboMenu->CheckBox("Use E", true);
	
	HarassQ = HarassMenu->CheckBox("Use Q", true);
	HarassManaManager = HarassMenu->AddInteger("ManaManager", 0, 100, 65);

	LaneClearQ = LaneClearMenu->CheckBox("Use Q", true);
	LaneClearManaManager = LaneClearMenu->AddInteger("ManaManager", 0, 100, 50);

	LastQ = LastHitMenu->CheckBox("Use Q", true);

	AutoTear = MiscsMenu->CheckBox("Stack Tear", true);
	AutoTearManager = MiscsMenu->AddInteger("Stack if Mana > ", 0, 100, 50);

	DrawReady = Drawings->CheckBox("Draw Ready Spells", true);
	DrawQ = Drawings->CheckBox("Draw Q", true);
	DrawE = Drawings->CheckBox("Draw E", true);
	DrawR = Drawings->CheckBox("Draw R", true);
}

void LoadSpells()
{
	Q = GPluginSDK->CreateSpell2(kSlotQ, kLineCast, true, false, static_cast<eCollisionFlags>(kCollidesWithMinions | kCollidesWithYasuoWall));
	W = GPluginSDK->CreateSpell2(kSlotW, kTargetCast, true, false, kCollidesWithNothing);
	E = GPluginSDK->CreateSpell2(kSlotE, kCircleCast, false, true, kCollidesWithYasuoWall);
	R = GPluginSDK->CreateSpell2(kSlotR, kTargetCast, false, false, kCollidesWithNothing);

	Q2 = GPluginSDK->CreateSpell2(kSlotQ, kTargetCast, true, false, kCollidesWithYasuoWall);
	Q2->SetOverrideRange(1200);
}

void Combo()
{
	auto player = GEntityList->Player();
	auto target = GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, Q->Range());
	auto target2 = GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, Q2->Range());
	
	if(ComboQ->Enabled() && Q->IsReady() && player->IsValidTarget(target2, Q2->Range()) && target2->HasBuff("UrgotCorrosiveDebuff"))
	{

		Q2->CastOnTarget(target2);
		
	}

	if(ComboQ->Enabled() && Q->IsReady() && player->IsValidTarget(target, Q->Range()))
	{
		Q->CastOnTarget(target, kHitChanceHigh);
	}

	if(ComboW->Enabled() && W->IsReady() && player->IsValidTarget(target, E->Range()))
	{
		W->CastOnPlayer();
	}

	if(ComboE->Enabled() && E->IsReady() && player->IsValidTarget(target, E->Range()))
	{
		E->CastOnTarget(target, kHitChanceHigh);
	}
}

void Harass()
{
	auto player = GEntityList->Player();
	auto target = GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, Q->Range());
	auto target2 = GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, Q2->Range());


	if (ComboE->Enabled() && E->IsReady() && player->IsValidTarget(target, E->Range()))
	{
		E->CastOnTarget(target, kHitChanceHigh);
	}

	if(HarassQ->Enabled() && Q->IsReady() && player->IsValidTarget(target, Q2->Range()) && target2->HasBuff("UrgotCorrosiveDebuff")){}
	{
		if(player->ManaPercent() >= HarassManaManager->GetInteger())
		{
			Q2->CastOnTarget(target2);
		}
	}
}

void LaneClear()
{
	auto player = GEntityList->Player();

	for (auto minion : GEntityList->GetAllMinions(false, true, false))
	{
		if (minion != nullptr && minion->IsValidTarget(GEntityList->Player(), Q->Range()) && LaneClearQ->Enabled() && player->ManaPercent() >= LaneClearManaManager->GetInteger())
		{
			if(!minion->IsDead())
			{
				Q->CastOnTarget(minion, kHitChanceLow);
			}
			
		}
	}
}

void LastHit()
{
	if (!Q->IsReady() || !LastQ->Enabled())
		return;


	for (auto minion : GEntityList->GetAllMinions(false, true, false))
	{
		if(minion != nullptr && !minion->IsDead())
		{
			if(GEntityList->Player()->IsValidTarget(minion, Q->Range()))
			{
				if (GDamage->GetSpellDamage(GEntityList->Player(), minion, kSlotQ) > GHealthPrediction->GetPredictedHealth(minion, kLastHitPrediction, static_cast<int>(((minion->ServerPosition() - GEntityList->Player()->GetPosition()).Length2D() * 1000) / Q->Speed()) - 125, static_cast<int>(E->GetDelay() * 1000)))
				{
					auto minionDistance = (minion->GetPosition() - GEntityList->Player()->GetPosition()).Length();
					if(minionDistance > 425)
					{
						Q->CastOnUnit(minion);
					}
					
				}
			}
		}
	}
}

void _OnOrbwalkPreAttack(IUnit* minion)
{
	auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), minion, kSlotQ);
	for(auto minion : GEntityList->GetAllMinions(false,true, false))
	{
		if(LastQ->Enabled() && GEntityList->Player()->GetSpellLevel(kSlotQ) > 0)
		{
			if(minion->IsValidTarget(GEntityList->Player(), Q->Range()))
			{
				if(minion->GetHealth() < QDamage + 100 && GEntityList->Player()->GetMana() > Q->ManaCost())
				{
					if(minion != nullptr && minion->IsCreep())
					{
						GOrbwalking->DisableNextAttack();
					}
				}
			}
		}
	}
}

void AutoStackTear()
{
	auto tear = GPluginSDK->CreateItemForId(3070, 0);
	auto manamune = GPluginSDK->CreateItemForId(3004, 0);
	auto player = GEntityList->Player();

	if (!tear->IsOwned() && !AutoTear->Enabled())
		return;

	if(tear->IsOwned() && AutoTear->Enabled() && GUtility->IsPositionInFountain(player->GetPosition(), true, false))
	{
		if(tear->IsReady())
		{
			Q->CastOnPosition(GGame->CursorPosition());
		}
		Q->CastOnPosition(GGame->CursorPosition());
	}

	if (!manamune->IsOwned() && !AutoTear->Enabled())
		return;

	if(manamune->IsOwned() && AutoTear->Enabled() && GUtility->IsPositionInFountain(player->GetPosition(), true, false))
	{
		if(manamune->IsReady())
		{
			Q->CastOnPosition(GGame->CursorPosition());
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

		if (E->IsReady() && DrawE->Enabled())
		{
			GRender->DrawOutlinedCircle(player->GetPosition(), Vec4(225, 225, 0, 225), E->Range());
		}

		if (R->IsReady() && DrawR->Enabled())
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

	AutoStackTear();

	if(GOrbwalking->GetOrbwalkingMode() == kModeCombo)
	{
		Combo();
	}

	if(GOrbwalking->GetOrbwalkingMode() == kModeMixed)
	{
		Harass();
		LastHit();
	}

	if(GOrbwalking->GetOrbwalkingMode() == kModeLastHit)
	{
		LastHit();
	}

	if(GOrbwalking->GetOrbwalkingMode() == kModeLaneClear)
	{
		LaneClear();
	}
}

PLUGIN_API void OnLoad(IPluginSDK* PluginSDK)
{
	PluginSDKSetup(PluginSDK);
	Menu();
	LoadSpells();

	GEventManager->AddEventHandler(kEventOnRender, OnRender);
	GEventManager->AddEventHandler(kEventOnGameUpdate, OnGameUpdate);

	GRender->NotificationEx(Color::LightBlue().Get(), 2, true, true, "Helalmoneys Urgot v2.0 LOADED");

}

PLUGIN_API void OnUnload()
{
	MainMenu->Remove();

	GEventManager->RemoveEventHandler(kEventOnRender, OnRender);
	GEventManager->RemoveEventHandler(kEventOnGameUpdate, OnGameUpdate);

	GRender->NotificationEx(Color::LightBlue().Get(), 2, true, true, "Urgot UNLOADED");
}