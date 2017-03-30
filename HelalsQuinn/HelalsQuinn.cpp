#include "PluginSDK.h"
#include "Color.h"


PluginSetup("HelalsQuinn");

IMenu* MainMenu;
IMenu* ComboMenu;
IMenu* HarassMenu;
IMenu* JungleMenu;
IMenu* Drawings;
IMenu* AntiG;
IMenu* MiscMenu;

IMenuOption* ComboQ;
IMenuOption* ComboW;
IMenuOption* ComboE;

IMenuOption* HarassQ;
IMenuOption* HarassE;
IMenuOption* HarassManaManager;

IMenuOption* JungleQ;
IMenuOption* JungleE;
IMenuOption* JungleManaManager;

IMenuOption* AutoUlt;

IMenuOption* EGap;

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
	AntiG = MainMenu->AddMenu("Anti Gapcloser");
	MiscMenu = MainMenu->AddMenu("Auto R Settings");
	Drawings = MainMenu->AddMenu("Drawings");

	ComboQ = ComboMenu->CheckBox("Use Q", true);
	//ComboW = ComboMenu->CheckBox("Use W", true);
	ComboE = ComboMenu->CheckBox("Use E", true);

	HarassQ = HarassMenu->CheckBox("Use Q", true);
	HarassE = HarassMenu->CheckBox("Use E", true);
	HarassManaManager = HarassMenu->AddFloat("ManaManager", 0, 100, 65);

	JungleQ = JungleMenu->CheckBox("Use Q", true);
	JungleE = JungleMenu->CheckBox("Use E", true);
	JungleManaManager = JungleMenu->AddFloat("ManaManager", 0, 100, 50);

	AutoUlt = MiscMenu->CheckBox("Auto R in Base", true);

	EGap = AntiG->CheckBox("Use E for AntiGapclose", true);

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
	auto target = GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, Q->Range());

	if(ComboQ->Enabled() && Q->IsReady() && player->IsValidTarget(target, Q->Range()))
	{
		if(!target->HasBuff("QuinnW"))
		{
			Q->CastOnTarget(target, kHitChanceHigh);
		}
	}

	if(ComboE->Enabled() && E->IsReady() && player->IsValidTarget(target, E->Range()))
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
			{
				if(!junglemob->IsDead() && junglemob->IsValidTarget())
				{
					Q->CastOnTarget(junglemob);
				}
			}
			
		}

		if (junglemob != nullptr && junglemob->IsValidTarget(GEntityList->Player(), E->Range()) && JungleE->Enabled() && player->ManaPercent() >= JungleManaManager->GetFloat())
		{
			if(!junglemob->IsDead() && junglemob->IsValidTarget())
			{
				E->CastOnTarget(junglemob);
			}
			
		}
	}
}

void AutoR()
{
	if (!R->IsReady() && !AutoUlt->Enabled())
		return;

	auto player = GEntityList->Player();

	if(GUtility->IsPositionInFountain(player->GetPosition(), true, false) && std::string(GEntityList->Player()->GetSpellName(kSlotR)) == "QuinnR")
	{
		R->CastOnPlayer();
	}

}

void AutoW()
{


	for(auto target : GEntityList->GetAllHeros(false, true))
	{
		auto lastPos = target->GetPosition();
		if(!target->IsVisible())
		{
			if(W->IsReady() && (GEntityList->Player()->GetPosition() - lastPos).Length2D() <= E->Range())
			{
				W->CastOnPlayer();
			}
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

PLUGIN_EVENT(void) OnGapcloser(GapCloserSpell const& Args)
{
	if (Args.Sender != nullptr && Args.Sender != GEntityList->Player() && Args.Sender->IsEnemy(GEntityList->Player()) && GEntityList->Player()->IsValidTarget(Args.Sender, 525.f + Args.Sender->BoundingRadius()) && EGap->Enabled() && E->IsReady())
	{
		E->CastOnTarget(Args.Sender);
	}
}



PLUGIN_EVENT(void) OnRender()
{
	Drawing();
}


PLUGIN_EVENT(void) OnGameUpdate()
{
	AutoR();
	AutoW();

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
	GEventManager->AddEventHandler(kEventOnGapCloser, OnGapcloser);



	GRender->NotificationEx(Color::LightBlue().Get(), 2, true, true, "Helalmoneys Quinn v2.0 LOADED");

}

PLUGIN_API void OnUnload()
{
	MainMenu->Remove();
	GEventManager->RemoveEventHandler(kEventOnRender, OnRender);
	GEventManager->RemoveEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GEventManager->RemoveEventHandler(kEventOnGapCloser, OnGapcloser);

	GRender->NotificationEx(Color::LightBlue().Get(), 2, true, true, "Quinn UNLOADED");
}