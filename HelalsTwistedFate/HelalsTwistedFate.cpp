#include "PluginSDK.h"
#include "Color.h"


PluginSetup("HelalsTwistedFate");

IMenu* MainMenu;
IMenu* ComboMenu;
IMenu* LaneClearMenu;
IMenu* CardSelector;
IMenu* MiscMenu;

IMenuOption* ComboQ;
IMenuOption* ComboW;

IMenuOption* GapW;
IMenuOption* PingR;

IMenuOption* PickGold;
IMenuOption* PickBlue;
IMenuOption* PickRed;


ISpell2* Q;
ISpell2* W;
ISpell2* E;
ISpell2* R;


void Menu()
{
	MainMenu = GPluginSDK->AddMenu("HelalsTwistedFate");
	ComboMenu = MainMenu->AddMenu("Combo");
	MiscMenu = MainMenu->AddMenu("Miscs");

	ComboQ = ComboMenu->CheckBox("Use Q", true);
	ComboW = ComboMenu->CheckBox("Use W", true);

	CardSelector = MainMenu->AddMenu("CardSelector");
	{
		PickGold = CardSelector->AddKey("Pick Gold Card[HOLD KEY]", 87);
		PickBlue = CardSelector->AddKey("Pick Blue Card[HOLD KEY]", 69);
		PickRed = CardSelector->AddKey("Pick Red Card[HOLD KEY]", 84);
	}

	//GapW = MiscMenu->CheckBox("Anti Gapcloser", true);
	PingR = MiscMenu->CheckBox("Ping if Killable", true);
}

void LoadSpells()
{
	Q = GPluginSDK->CreateSpell2(kSlotQ, kLineCast, false, true, kCollidesWithYasuoWall);
	Q->SetSkillshot(0.25f, 40.f, 1000.f, 1500.f);
	W = GPluginSDK->CreateSpell2(kSlotW, kTargetCast, false, false, kCollidesWithYasuoWall);
	R = GPluginSDK->CreateSpell2(kSlotR, kCircleCast, false, false, kCollidesWithNothing);
}

void AlwaysCard()
{

	static int CardTick;

	if (!W->IsReady())
	{
		return;
	}
	if (GGame->TickCount() - CardTick < 250)
	{
		return;
	}
	CardTick = GGame->TickCount();
	if (std::string(GEntityList->Player()->GetSpellBook()->GetName(kSlotW)) == "PickACard")
	{
		W->CastOnPlayer();
	}
}

void PickGoldCard()
{

	if(W->IsReady())
	{

		if (GetAsyncKeyState(PickGold->GetInteger()))

		{
			if (GGame->IssueOrder(GEntityList->Player(), kMoveTo, GGame->CursorPosition()))
			{

				if (std::string(GEntityList->Player()->GetSpellBook()->GetName(kSlotW)) == "PickACard")
				{
					W->CastOnPlayer();
				}

				else if (std::string(GEntityList->Player()->GetSpellBook()->GetName(kSlotW)) == "GoldCardLock")
				{
					W->CastOnPlayer();
				}

			}

		}
		
	}



}

//Für Combo und Anti Gapcloser
void PickGoldCardCombo()
{

	if(W->IsReady())
	{

		if (std::string(GEntityList->Player()->GetSpellBook()->GetName(kSlotW)) == "PickACard")
		{
			W->CastOnPlayer();
		}

		else if (std::string(GEntityList->Player()->GetSpellBook()->GetName(kSlotW)) == "GoldCardLock")
		{
			W->CastOnPlayer();
		}
		
	}

}

void PickBlueCard()
{

	if(W->IsReady())
	{

		if (GetAsyncKeyState(PickBlue->GetInteger()))

		{
			if (GGame->IssueOrder(GEntityList->Player(), kMoveTo, GGame->CursorPosition()))
			{

				if (std::string(GEntityList->Player()->GetSpellBook()->GetName(kSlotW)) == "PickACard")
				{
					W->CastOnPlayer();
				}

				else if (std::string(GEntityList->Player()->GetSpellBook()->GetName(kSlotW)) == "BlueCardLock")
				{
					W->CastOnPlayer();
				}

			}

		}
		
	}



}

void PickRedCard()
{

	if(W->IsReady())
	{

		if (GetAsyncKeyState(PickRed->GetInteger()))

		{
			if (GGame->IssueOrder(GEntityList->Player(), kMoveTo, GGame->CursorPosition()))
			{

				if (std::string(GEntityList->Player()->GetSpellBook()->GetName(kSlotW)) == "PickACard")
				{
					W->CastOnPlayer();
				}

				else if (std::string(GEntityList->Player()->GetSpellBook()->GetName(kSlotW)) == "RedCardLock")
				{
					W->CastOnPlayer();
				}

			}

		}
		
	}



}


void Combo()
{
	auto player = GEntityList->Player();
	auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());

	if(ComboQ->Enabled() && Q->IsReady() && player->IsValidTarget(target, Q->Range()) && !target->IsMoving())
	{
		Q->CastOnTarget(target, kHitChanceHigh);
	}

	if(ComboW->Enabled() && W->IsReady() && player->IsValidTarget(target, Q->Range()))
	{
		AlwaysCard();
		{
			PickGoldCardCombo();
		}

	}
}

/*
void AntiGapCloser()
{
	auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());

	if(target->IsDashing() && GapW->Enabled() && W->IsReady() && !target->IsCreep() && !target->IsJungleCreep())
	{
		AlwaysCard();
		{
			PickGoldCardCombo();
		}
	}
}
*/

void RPing()
{

	static int LastPingTime2;

	for (auto target : GEntityList->GetAllHeros(false, true))
	{
		if(!target->IsDead() && target->IsValidTarget())
		{
			if(PingR->Enabled())
			{
				auto ComboDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotQ) * 2 + GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotW);
				if(W->IsReady() && Q->IsReady() && R->IsReady())
				{
					if(target->GetHealth() < ComboDamage)
					{
						if(GGame->Time() - LastPingTime2 >= 1)
						{
							GGame->ShowPing(1, target->GetPosition(), true);
							GGame->ShowPing(1, target->GetPosition(), true);
							GGame->ShowPing(1, target->GetPosition(), true);
							LastPingTime2 = GGame->Time();

						}
					}
				}
			}
		}
	}
}

void GoldCardAfterR()
{

	if (!R->IsReady())
		return;

	auto player = GEntityList->Player();

	if (player->HasBuff("destiny_marker") && R->IsReady())
	{
		AlwaysCard();
		{
			PickGoldCardCombo();
		}
	}

}

PLUGIN_EVENT(void) OnGameUpdate()
{


	PickRedCard();
	PickBlueCard();
	PickGoldCard();
	//AntiGapCloser();
	RPing();
	GoldCardAfterR();
	

	if(GOrbwalking->GetOrbwalkingMode() == kModeCombo)
	{
		Combo();
		
	}



}

PLUGIN_API void OnLoad(IPluginSDK* PluginSDK)
{
	PluginSDKSetup(PluginSDK);
	Menu();
	LoadSpells();
	GEventManager->AddEventHandler(kEventOnGameUpdate, OnGameUpdate);

	GRender->NotificationEx(Color::LightBlue().Get(), 2, true, true, "Helalmoneys Twisted Fate v1.0 LOADED");
}

PLUGIN_API void OnUnload()
{
	MainMenu->Remove();
	GEventManager->RemoveEventHandler(kEventOnGameUpdate, OnGameUpdate);

	GRender->NotificationEx(Color::LightRed().Get(), 2, true, true, "TF UNLOADED");

}

