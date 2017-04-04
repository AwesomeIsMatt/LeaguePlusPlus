#include "PluginSDK.h"
#include "Color.h"

PluginSetup("HelalsTeemo");

IMenu* MainMenu;
IMenu* ComboMenu;
IMenu* HarassMenu;
IMenu* MiscsMenu;
IMenu* Drawings;

IMenuOption* ComboQ;
IMenuOption* ComboW;
IMenuOption* ComboR;

IMenuOption* HarrasQ;
IMenuOption* HarassManager;

IMenuOption* AutoR;

IMenuOption* DrawReady;
IMenuOption* DrawQ;
IMenuOption* DrawR;

ISpell2* Q;
ISpell2* W;
ISpell2* R;

int lastcast;

void Menu()
{
	MainMenu = GPluginSDK->AddMenu("HelalsTeemo");

	ComboMenu = MainMenu->AddMenu("Combo");
	{
		ComboQ = ComboMenu->CheckBox("Use Q", true);
		ComboW = ComboMenu->CheckBox("Use W", true);
		ComboR = ComboMenu->CheckBox("Use R", true);
	}

	HarassMenu = MainMenu->AddMenu("Harass");
	{
		HarrasQ = HarassMenu->CheckBox("Use Q", true);
		HarassManager = HarassMenu->AddInteger("ManaManager", 0, 100, 50);
	}

	/*MiscsMenu = MainMenu->AddMenu("Miscs");
	{
		AutoR = MiscsMenu->CheckBox("Use R", true);
	}*/

	Drawings = MainMenu->AddMenu("Drawings");
	{
		DrawReady = Drawings->CheckBox("Draw Ready Spells", true);
		DrawQ = Drawings->CheckBox("Draw Q", true);
		DrawR = Drawings->CheckBox("Draw R", true);
	}
}

void LoadSpells()
{
	Q = GPluginSDK->CreateSpell2(kSlotQ, kTargetCast, false, false, kCollidesWithWalls);
	W = GPluginSDK->CreateSpell2(kSlotW, kCircleCast, false, false, kCollidesWithNothing);
	R = GPluginSDK->CreateSpell2(kSlotR, kCircleCast, false, true, kCollidesWithNothing);
	Q->SetOverrideRange(580);
	W->SetOverrideRange(850);

}

void CastR()
{
	auto target = GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, Q->Range());

	if (target->IsValidTarget(GEntityList->Player(), Q->Range()) && R->IsReady())
	{
		Vec3 pred;
		GPrediction->GetFutureUnitPosition(target, 0.15f, true, pred);

		R->CastOnPosition(pred);
		lastcast = GGame->TickCount() + 1500;
	}
}

void Combo()
{
	auto player = GEntityList->Player();
	auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());

	if (ComboQ->Enabled() && Q->IsReady() && player->IsValidTarget(target, Q->Range()))
	{
		Q->CastOnUnit(target);
	}

	if (ComboW->Enabled() && W->IsReady() && player->IsValidTarget(target, W->Range()))
	{
		W->CastOnPlayer();
	}

	if (ComboR->Enabled() && R->IsReady() && player->IsValidTarget(target, R->Range()))
	{
		if (lastcast < GGame->TickCount())
		{

			CastR();
		}
	}

}

void Harass()
{
	auto player = GEntityList->Player();
	auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());

	if(HarrasQ->Enabled() && Q->IsReady() && player->IsValidTarget(target, Q->Range()))
	{
		Q->CastOnUnit(target);
		
	}
}

/*
void AutoMushRoom()
{

	
	for(auto target : GEntityList->GetAllHeros(false, true))
	{
		auto lastPos = target->GetPosition();
		if(!target->IsVisible() && !target->IsDead() && target != nullptr)
		{
			if (R->IsReady() && (GEntityList->Player()->GetPosition() - lastPos).Length2D() <= Q->Range())
			{
				if (lastcast < GGame->TickCount())
				{
					CastR();
				}

			}
		}
	}
}*/

void Drawing()
{
	auto player = GEntityList->Player();
	if (DrawReady->Enabled())
	{
		if (Q->IsReady() && DrawQ->Enabled())
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

	//AutoMushRoom();

	if (GOrbwalking->GetOrbwalkingMode() == kModeCombo)
	{
		Combo();
	}

	if (GOrbwalking->GetOrbwalkingMode() == kModeMixed)
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

	GRender->NotificationEx(Color::Green().Get(), 2, true, true, "Helalmoneys Teemo v2.0 - LOADED");





}

PLUGIN_API void OnUnload(IPluginSDK* IPluginSDK)
{
	MainMenu->Remove();
	GEventManager->RemoveEventHandler(kEventOnRender, OnRender);
	GEventManager->RemoveEventHandler(kEventOnGameUpdate, OnGameUpdate);

	GRender->NotificationEx(Color::Green().Get(), 2, true, true, "Teemo - UNLOADED");

}