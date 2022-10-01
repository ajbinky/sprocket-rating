#define _USE_MATH_DEFINES

#include "pch.h"
#include "SprocketRatingPlugin.h"
#include "bakkesmod/wrappers//includes.h"
#include <iomanip>
#include <sstream>
#include <corecrt_math_defines.h>

using namespace std;


BAKKESMOD_PLUGIN(SprocketRatingPlugin, "Sprocket Rating Plugin", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

// 2s Constants
const float g_2s = 1.54;
const float a_2s = 0.79;
const float sv_2s = 1.75;
const float sh_2s = 3.93;
const float xopi_2s = 2.01;
const float yopi_2s = 1.34;
const float xdpi_2s = 1.94;
const float ydpi_2s = 1.09;

// 3s Constants
const float g_3s = 0.77;
const float a_3s = 0.53;
const float sv_3s = 1.24;
const float sh_3s = 2.43;
const float xopi_3s = 2.00;
const float yopi_3s = 1.69;
const float xdpi_3s = 1.96;
const float ydpi_3s = 0.93;

//
float g = 0.0;
float a = 0.0;
float sv = 0.0;
float sh = 0.0;
float xopi = 0.0;
float yopi = 0.0;
float xdpi = 0.0;
float ydpi = 0.0;

// Weightings
// Offense
const float weighting_goals = 1.0;
const float weighting_assists = 0.8;
const float weighting_shots = 0.2;
// Defense
const float weighting_goals_allowed = 1.1;
const float weighting_shots_allowed = 0.4;
const float weighting_saves = 0.5;

// 
bool isInOnlineGame = false;

// Do I need to copy TBJ exactly? Probably not. Oh well.
float raw_opi = 0.0;
float opi = 0.0;
float raw_dpi = 0.0;
float dpi = 0.0;
float goals = 0;
float assists = 0;
float shots = 0;
float saves = 0;
float rating = 0;
float goals_allowed = 0;
float shots_allowed = 0;

//
string playerName = "";
string playerNameShort = "";
string ratingString = "";

//
float userX = 0.0;
float userY = 0.0;
int backgroundX = 0;
int backgroundY = 0;
int backgroundWidth = 0;
int backgroundHeight = 0;

//
int playercount = 0;
int playerTeam = 0;
vector<string> players;
vector<float> ratings;
int team_goals[] = { 0, 0 };
int team_shots[] = { 0, 0 };


void SprocketRatingPlugin::onLoad()
{
	cvarManager->registerCvar("sr_enableBool", "1", "toggle plugin");
	cvarManager->registerCvar("sr_alwaysOn", "0", "toggle always on");
	cvarManager->registerCvar("sr_bgOpacity", "173", "set background opacity");
	cvarManager->registerCvar("sr_bgWidth", "300", "set background width");
	cvarManager->registerCvar("sr_bgHeight", "250", "set background width");
	cvarManager->registerCvar("sr_bgX", "87.0", "X Position %");
	cvarManager->registerCvar("sr_bgY", "0.0", "Y Position %");

	gameWrapper->HookEvent("Function TAGame.GFxData_GameEvent_TA.OnOpenScoreboard", std::bind(&SprocketRatingPlugin::openScoreboard, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.GFxData_GameEvent_TA.OnCloseScoreboard", std::bind(&SprocketRatingPlugin::closeScoreboard, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function ProjectX.GRI_X.EventGameStarted", std::bind(&SprocketRatingPlugin::gameStart, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.EventMatchEnded", std::bind(&SprocketRatingPlugin::gameEnd, this, std::placeholders::_1));
	//im hooking into the loading screen event to unregister drawables whenever you go to the loading screen. in theory, i dont need to hook into
	//exit to main menu or play freeplay map, as they will both take you to the loading screen, but i figure better safe than sorry
	gameWrapper->HookEvent("Function ProjectX.GFxShell_X.ExitToMainMenu", std::bind(&SprocketRatingPlugin::unregister, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.GFxData_Training_TA.PlayFreeplayMap", std::bind(&SprocketRatingPlugin::unregister, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.LoadingScreen_TA.GetProtipInputType", std::bind(&SprocketRatingPlugin::unregister, this, std::placeholders::_1));
}

void SprocketRatingPlugin::onUnload()
{
	gameWrapper->UnhookEvent("Function TAGame.GFxData_GameEvent_TA.OnOpenScoreboard");
	gameWrapper->UnhookEvent("Function TAGame.GFxData_GameEvent_TA.OnCloseScoreboard");
	gameWrapper->UnhookEvent("Function ProjectX.GRI_X.EventGameStarted");
	gameWrapper->UnhookEvent("Function TAGame.GameEvent_Soccar_TA.EventMatchEnded");
	gameWrapper->UnhookEvent("Function ProjectX.GFxShell_X.ExitToMainMenu");
	gameWrapper->UnhookEvent("Function TAGame.GFxData_Training_TA.PlayFreeplayMap");
	gameWrapper->UnhookEvent("Function TAGame.LoadingScreen_TA.GetProtipInputType");
}

void SprocketRatingPlugin::openScoreboard(string eventName) {
	if (cvarManager->getCvar("sr_enableBool").getBoolValue() && !cvarManager->getCvar("sr_alwaysOn").getBoolValue()) {
		gameWrapper->RegisterDrawable(std::bind(&SprocketRatingPlugin::render, this, std::placeholders::_1));
	}
}

void SprocketRatingPlugin::closeScoreboard(string eventName) {
	if (cvarManager->getCvar("sr_enableBool").getBoolValue() && !cvarManager->getCvar("sr_alwaysOn").getBoolValue()) {
		gameWrapper->UnregisterDrawables();
	}
}

void SprocketRatingPlugin::gameStart(string eventName) {
	if (cvarManager->getCvar("sr_enableBool").getBoolValue() && cvarManager->getCvar("sr_alwaysOn").getBoolValue()) {
		gameWrapper->RegisterDrawable(std::bind(&SprocketRatingPlugin::render, this, std::placeholders::_1));
	}
	else if (cvarManager->getCvar("sr_enableBool").getBoolValue() && !cvarManager->getCvar("sr_alwaysOn").getBoolValue()) {
		gameWrapper->UnregisterDrawables();
	}
}

void SprocketRatingPlugin::gameEnd(string eventName) {
	if (cvarManager->getCvar("sr_enableBool").getBoolValue() && !cvarManager->getCvar("sr_alwaysOn").getBoolValue()) {
		gameWrapper->RegisterDrawable(std::bind(&SprocketRatingPlugin::render, this, std::placeholders::_1));
	}
}

void SprocketRatingPlugin::unregister(string eventName) {
	if (cvarManager->getCvar("sr_enableBool").getBoolValue()) {
		gameWrapper->UnregisterDrawables();
	}
}

void SprocketRatingPlugin::render(CanvasWrapper canvas) {
	isInOnlineGame = gameWrapper->IsInOnlineGame(); // || gameWrapper->IsSpectatingInOnlineGame();
	if (isInOnlineGame) {
		ArrayWrapper<PriWrapper> pris = gameWrapper->GetOnlineGame().GetPRIs();

		//get X and Y as a percentage of the screen size
		//userX and userY will be between 0-1
		//get user defined width and height
		userX = cvarManager->getCvar("sr_bgX").getFloatValue() / 100;
		userY = cvarManager->getCvar("sr_bgY").getFloatValue() / 100;
		backgroundX = static_cast<int>(userX * canvas.GetSize().X);
		backgroundY = static_cast<int>(userY * canvas.GetSize().Y);
		backgroundWidth = cvarManager->getCvar("sr_bgWidth").getIntValue();
		backgroundHeight = cvarManager->getCvar("sr_bgHeight").getIntValue();

		//set up background and MVPR title.
		//bgopacity is set by user in settings
		//if OMVPR is true, then make title "MVPR/OMVPR"
		canvas.SetColor(0, 0, 0, cvarManager->getCvar("sr_bgOpacity").getIntValue());
		canvas.DrawRect(Vector2{ backgroundX, backgroundY }, Vector2{ backgroundX + backgroundWidth, backgroundY + backgroundHeight });
		canvas.SetPosition(Vector2{ backgroundX + 15, backgroundY + 15 });
		canvas.SetColor(255, 255, 255, 255);
		canvas.DrawString("Sprocket Rating", 2, 2);

		// init values
		playercount = 0;
		team_goals[0] = 0;
		team_goals[1] = 0;
		team_shots[0] = 0;
		team_shots[1] = 0;
		players.clear();
		ratings.clear();

		// Set team_goals and team_shots for dpi
		for (int i = 0; i < pris.Count(); i++) {
			playerTeam = pris.Get(i).GetTeamNum();
			if (playerTeam == 0 || playerTeam == 1) {
				playercount++;
			}
			team_goals[pris.Get(i).GetTeamNum()] += pris.Get(i).GetMatchGoals();
			team_shots[pris.Get(i).GetTeamNum()] += pris.Get(i).GetMatchShots();
		}

		// Set vars to correct constants - defaults to 2v2 variables
		g = playercount == 6 ? g_3s : g_2s;
		a = playercount == 6 ? a_3s : a_2s;
		sv = playercount == 6 ? sv_3s : sv_2s;
		sh = playercount == 6 ? sh_3s : sh_2s;
		xopi = playercount == 6 ? xopi_3s : xopi_2s;
		yopi = playercount == 6 ? yopi_3s : yopi_2s;
		xdpi = playercount == 6 ? xdpi_3s : xdpi_2s;
		ydpi = playercount == 6 ? ydpi_3s : ydpi_2s;

		// Set players' stats
		for (int i = 0; i < pris.Count(); i++) {
			playerTeam = pris.Get(i).GetTeamNum();
			if (playerTeam == 0 || playerTeam == 1) {
				playerName = pris.Get(i).GetPlayerName().ToString();
				goals = pris.Get(i).GetMatchGoals();
				goals_allowed = team_goals[1 - playerTeam];
				shots = pris.Get(i).GetMatchShots();
				shots_allowed = team_shots[1 - playerTeam];
				assists = pris.Get(i).GetMatchAssists();
				saves = pris.Get(i).GetMatchSaves();

				// Offensive Performance Index
				raw_opi = weighting_goals * (goals / g_2s) + weighting_assists * (assists / a_2s) + weighting_shots * (shots / sh_2s);
				opi = 100.0F / ((float) 1 + pow(M_E, -1 * log(9) * ((raw_opi - xopi_2s) / yopi_2s)));

				// Defensive Performance Index
				raw_dpi = weighting_goals_allowed * (2.0F - goals_allowed / 2.0F / g_2s) + weighting_saves * (saves / sv_2s) + weighting_shots * (2.0F - shots_allowed / 2.0F / sh_2s);
				dpi = 100.0F / ((float) 1 + pow(M_E, -1 * log(9) * ((raw_dpi - xdpi_2s) / ydpi_2s)));

				// Sprocket Rating
				rating = (opi + dpi) / 2.0;

				// Add Players to containers
				players.push_back(playerName);
				ratings.push_back(rating);
			}
		}

		// Show player names and rating
		for (int i = 0; i < playercount; i++) {
			playerName = players[i];
			rating = ratings[i];

			stringstream ss;
			ss << fixed << setprecision(2) << rating;
			ratingString = "";
			ratingString = ss.str();

			canvas.SetPosition(Vector2{ backgroundX + 15, backgroundY + ((i * 20) + 50) });

			if (playerName.length() > 13) {
				playerNameShort = playerName.substr(0, 12) + "...";
			}
			else {
				playerNameShort = playerName;
			}

			canvas.DrawString(playerNameShort + ": " + ratingString, 1.5, 1.5);

		}

	}


}