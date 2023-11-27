#include "CvGameCoreDLL.h"
#include "CvEnums.h"
#include "CvGameCoreDLLUndefNew.h"
#include <boost/python/enum.hpp>
#include "CvGameCoreDLLDefNew.h"

namespace python = boost::python;
//
// Python interface for enums
//

// How to read what can be accessed from python
//
// The file is split into 3 sections
//
// Section 1: Value setup
//   void EnumContainer<  TYPE  >::apply()
//      this one adds extra values to TYPE
//      each new value is added with enumTable.value(python name, C++ name)
//   WARNING: section 1 values doesn't exist during python's first init during startup as the data haven't been loaded yet (usually not a problem)
//      Section 2 and 3 are however available
//
// Section 2: XML Types
//    At the start of CyEnumsPythonInterface(), a number of xml files are listed
//    Any file listed here will have types from xml, NO_ and NUM_, even in a dynamic dll
//
// Section 2: Vanilla approach
// python::enum_<AAA>("BBB")
//      .value("NO_COLOR", NO_COLOR)
//     ;
// This adds BBB to python and it returns value AAA (usually the same)
// Nothing automatic happens here. It adds values like Section 1
//
// Note Section 3 will slowly be replaced by Section 2 as Section 3 needs manual updating to match xml while section 2 is automatic


class EnumContainerBase
{
protected:
	EnumContainerBase() {}
public:
	virtual void apply() = 0;
};

template<typename T>
class EnumContainer : public EnumContainerBase
{
public:
	EnumContainer(boost::python::enum_<T> enumTableInit) : enumTable(enumTableInit) {}

	void apply();

protected:
	boost::python::enum_<T> enumTable;
};

// There is a problem with python exposure during startup.
// The exe is hardcoded to expose everything prior to being done loading xml files.
// To get around this limitation, only expose what is available (NO_, NUM_, Type tag from xml and enums)
// While this takes place, store the enum instance in the vector.
// Once xml is done loading, the vector is looped to call apply() and the rest of the values can be added.
static std::vector<EnumContainerBase*> EnumVector;


template<typename T>
void EnumContainer<T>::apply()
{
	// intentionally empty. Fallback for any type, which doesn't have extra values to add
}

// python_enum_check.pl NEXT STATE
template<>
void EnumContainer<int>::apply()
{
	// Python name: GlobalDefines

	enumTable.value("BASE_CAPTURE_GOLD", GLOBAL_DEFINE_BASE_CAPTURE_GOLD);
	enumTable.value("BUILDING_PRODUCTION_PERCENT", GLOBAL_DEFINE_BUILDING_PRODUCTION_PERCENT);
	enumTable.value("CAPTURE_GOLD_MAX_TURNS", GLOBAL_DEFINE_CAPTURE_GOLD_MAX_TURNS);
	enumTable.value("CAPTURE_GOLD_PER_POPULATION", GLOBAL_DEFINE_CAPTURE_GOLD_PER_POPULATION);
	enumTable.value("CAPTURE_GOLD_RAND1", GLOBAL_DEFINE_CAPTURE_GOLD_RAND1);
	enumTable.value("CAPTURE_GOLD_RAND2", GLOBAL_DEFINE_CAPTURE_GOLD_RAND2);
	enumTable.value("CIVILOPEDIA_SHOW_ACTIVE_CIVS_ONLY", GLOBAL_DEFINE_CIVILOPEDIA_SHOW_ACTIVE_CIVS_ONLY);
	enumTable.value("DECREASE_MAX_TAX_RATE", GLOBAL_DEFINE_DECREASE_MAX_TAX_RATE);
	enumTable.value("INCREASE_MAX_TAX_RATE", GLOBAL_DEFINE_INCREASE_MAX_TAX_RATE);
	enumTable.value("MAX_TAX_RATE", GLOBAL_DEFINE_MAX_TAX_RATE);
	enumTable.value("NEW_CAPACITY", 1);
	enumTable.value("PORT_ROYAL_PORT_TAX", GLOBAL_DEFINE_PORT_ROYAL_PORT_TAX);
	enumTable.value("SCORE_FATHER_FACTOR", GLOBAL_DEFINE_SCORE_FATHER_FACTOR);
	enumTable.value("SCORE_FREE_PERCENT", GLOBAL_DEFINE_SCORE_FREE_PERCENT);
	enumTable.value("SCORE_HANDICAP_PERCENT_OFFSET", GLOBAL_DEFINE_SCORE_HANDICAP_PERCENT_OFFSET);
	enumTable.value("SCORE_HANDICAP_PERCENT_PER", GLOBAL_DEFINE_SCORE_HANDICAP_PERCENT_PER);
	enumTable.value("SCORE_LAND_FACTOR", GLOBAL_DEFINE_SCORE_LAND_FACTOR);
	enumTable.value("SCORE_POPULATION_FACTOR", GLOBAL_DEFINE_SCORE_POPULATION_FACTOR);
	enumTable.value("SCORE_VICTORY_PERCENT", GLOBAL_DEFINE_SCORE_VICTORY_PERCENT);
	enumTable.value("SHOW_LANDMARKS", GLOBAL_DEFINE_SHOW_LANDMARKS);
	enumTable.value("START_YEAR", GLOBAL_DEFINE_START_YEAR);
	enumTable.value("UNIT_PRODUCTION_PERCENT", GLOBAL_DEFINE_UNIT_PRODUCTION_PERCENT);
	enumTable.value("USE_MODDERS_PLAYEROPTION_1", GLOBAL_DEFINE_USE_MODDERS_PLAYEROPTION_1);
	enumTable.value("USE_MODDERS_PLAYEROPTION_2", GLOBAL_DEFINE_USE_MODDERS_PLAYEROPTION_2);
	enumTable.value("USE_MODDERS_PLAYEROPTION_3", GLOBAL_DEFINE_USE_MODDERS_PLAYEROPTION_3);
}

template<>
void EnumContainer<HandicapTypes>::apply()
{
	enumTable.value("STANDARD_HANDICAP", GLOBAL_DEFINE_STANDARD_HANDICAP);
}

template<>
void EnumContainer<UnitClassTypes>::apply()
{
	enumTable.value("DEFAULT_POPULATION_UNIT", GLOBAL_DEFINE_DEFAULT_POPULATION_UNIT);
}

template<>
void EnumContainer<YieldTypes>::apply()
{
	enumTable.value("NUM_CARGO_YIELD_TYPES", NUM_CARGO_YIELD_TYPES);
}

// python_enum_check.pl NEXT STATE

template<typename T>
static void addEnumValues(T value, const char* szName, const char* szNoType, const char* szNumTypes)
{
	boost::python::enum_<T> enumTable = python::enum_<T>(szName)
		.value(szNumTypes, VARINFO<T>::END);
	if (szNoType != NULL)
	{
		enumTable.value(szNoType, static_cast<T>(-1));
	}
	for (T eLoopVar = VARINFO<T>::FIRST; eLoopVar < VARINFO<T>::END; ++eLoopVar)
	{
		enumTable.value(getTypeStr(eLoopVar), eLoopVar);
	}
	EnumVector.push_back(new EnumContainer<T>(enumTable));
}

void CyEnumsPythonInterfacePostApply()
{
	// Apply the xml data, which wasn't available during the first run.
	const unsigned int iLength = EnumVector.size();
	for (unsigned int i = 0; i < iLength; ++i)
	{
		EnumVector[i]->apply();
		SAFE_DELETE(EnumVector[i]);
	}
	EnumVector.clear();

	// End of python enum setup.
	// Should any post setup activity ever be needed, then it can be added after this comment.
}

void CyEnumsPythonInterface()
{
	OutputDebugString("Python Extension Module - CyEnumsPythonInterface\n");

	// list all the enums, which autogenerate the python interface based on xml
	// arguments:
	// 1: any variable of the enum type. Which one doesn't matter as it is only used to set the template type
	// 2: name of enum in python
	// 3: name of NO_ type. Skipped if it is set to NULL
	// 4 name of NUM_

// python_enum_check.pl NEXT STATE
	addEnumValues(NO_BUTTON_POPUP      , "ButtonPopupTypes"   , "NO_BUTTON_POPUP"  , "NUM_BUTTONPOPUP_TYPES"    );
	addEnumValues(NO_COLOR             , "ColorTypes"         , "NO_COLOR"         , "NUM_COLOR_TYPES"          );
	addEnumValues(NO_CONCEPT           , "ConceptTypes"       , "NO_CONCEPT"       , "NUM_CONCEPT_TYPES"        );
	addEnumValues(NO_EUROPE            , "EuropeTypes"        , "NO_EUROPE"        , "NUM_EUROPE_TYPES"         );
	addEnumValues(NO_FEATURE           , "FeatureTypes"       , "NO_FEATURE"       , "NUM_FEATURE_TYPES"        );
	addEnumValues(MAX_NUM_SYMBOLS      , "FontSymbols"        , NULL               , "MAX_NUM_SYMBOLS"          );
	addEnumValues(NO_HANDICAP          , "HandicapTypes"      , "NO_HANDICAP"      , "NUM_HANDICAP_TYPES"       );
	addEnumValues(NO_HURRY             , "HurryTypes"         , "NO_HURRY"         , "NUM_HURRY_TYPES"          );
	addEnumValues(NO_PROFESSION        , "ProfessionTypes"    , "NO_PROFESSION"    , "NUM_PROFESSION_TYPES"     );
	addEnumValues(NO_SPECIALBUILDING   , "SpecialBuildingTypes","NO_SPECIALBUILDING", "NUM_SPECIALBUILDING_TYPES");
	addEnumValues(NO_TERRAIN           , "TerrainTypes"       , "NO_TERRAIN"       , "NUM_TERRAIN_TYPES"        );
	addEnumValues(NO_UNIT              , "UnitTypes"          , "NO_UNIT"          , "NUM_UNIT_TYPES"           );
	addEnumValues(NO_UNITCLASS         , "UnitClassTypes"     , "NO_UNITCLASS"     , "NUM_UNITCLASS_TYPES"      );
	addEnumValues(NO_WORLDSIZE         , "WorldSizeTypes"     , "NO_WORLDSIZE"     , "NUM_WORLDSIZE_TYPES"      );
	addEnumValues(NO_WIDGET            , "WidgetTypes"        , "NO_WIDGET"        , "NUM_WIDGET_TYPES"         );
	addEnumValues(NO_YIELD             , "YieldTypes"         , "NO_YIELD"         , "NUM_YIELD_TYPES"          );
// python_enum_check.pl NEXT STATE
	
	EnumVector.push_back(new EnumContainer<int>(python::enum_<int>("GlobalDefines")));

// python_enum_check.pl NEXT STATE
	python::enum_<GameStateTypes>("GameStateTypes")
		.value("GAMESTATE_ON", GAMESTATE_ON)
		.value("GAMESTATE_OVER", GAMESTATE_OVER)
		.value("GAMESTATE_EXTENDED", GAMESTATE_EXTENDED)
		;
	python::enum_<PopupStates>("PopupStates")
		.value("POPUPSTATE_IMMEDIATE", POPUPSTATE_IMMEDIATE)
		.value("POPUPSTATE_QUEUED", POPUPSTATE_QUEUED)
		.value("POPUPSTATE_MINIMIZED", POPUPSTATE_MINIMIZED)
		;
	python::enum_<CameraLookAtTypes>("CameraLookAtTypes")
		.value("CAMERALOOKAT_NORMAL", CAMERALOOKAT_NORMAL)
		.value("CAMERALOOKAT_CITY_ZOOM_IN", CAMERALOOKAT_CITY_ZOOM_IN)
		.value("CAMERALOOKAT_BATTLE", CAMERALOOKAT_BATTLE)
		.value("CAMERALOOKAT_BATTLE_ZOOM_IN", CAMERALOOKAT_BATTLE_ZOOM_IN)
		.value("CAMERALOOKAT_IMMEDIATE", CAMERALOOKAT_IMMEDIATE)
		;
	python::enum_<CameraMovementSpeeds>("CameraMovementSpeeds")
		.value("CAMERAMOVEMENTSPEED_NORMAL", CAMERAMOVEMENTSPEED_NORMAL)
		.value("CAMERAMOVEMENTSPEED_SLOW", CAMERAMOVEMENTSPEED_SLOW)
		.value("CAMERAMOVEMENTSPEED_FAST", CAMERAMOVEMENTSPEED_FAST)
		;
	python::enum_<ZoomLevelTypes>("ZoomLevelTypes")
		.value("ZOOM_UNKNOWN", ZOOM_UNKNOWN)
		.value("ZOOM_GLOBEVIEW", ZOOM_GLOBEVIEW)
		.value("ZOOM_NORMAL", ZOOM_NORMAL)
		.value("ZOOM_DETAIL", ZOOM_DETAIL)
		;
	python::enum_<DirectionTypes>("DirectionTypes")
		.value("NO_DIRECTION", NO_DIRECTION)
		.value("DIRECTION_NORTH", DIRECTION_NORTH)
		.value("DIRECTION_NORTHEAST", DIRECTION_NORTHEAST)
		.value("DIRECTION_EAST", DIRECTION_EAST)
		.value("DIRECTION_SOUTHEAST", DIRECTION_SOUTHEAST)
		.value("DIRECTION_SOUTH", DIRECTION_SOUTH)
		.value("DIRECTION_SOUTHWEST", DIRECTION_SOUTHWEST)
		.value("DIRECTION_WEST", DIRECTION_WEST)
		.value("DIRECTION_NORTHWEST", DIRECTION_NORTHWEST)
		.value("NUM_DIRECTION_TYPES", NUM_DIRECTION_TYPES)
		;
	python::enum_<CardinalDirectionTypes>("CardinalDirectionTypes")
		.value("NO_CARDINALDIRECTION", NO_CARDINALDIRECTION)
		.value("CARDINALDIRECTION_NORTH", CARDINALDIRECTION_NORTH)
		.value("CARDINALDIRECTION_EAST", CARDINALDIRECTION_EAST)
		.value("CARDINALDIRECTION_SOUTH", CARDINALDIRECTION_SOUTH)
		.value("CARDINALDIRECTION_WEST", CARDINALDIRECTION_WEST)
		.value("NUM_CARDINALDIRECTION_TYPES", NUM_CARDINALDIRECTION_TYPES)
		;
	python::enum_<PlayerColorTypes>("PlayerColorTypes")
		.value("NO_PLAYERCOLOR", NO_PLAYERCOLOR)
		;
	python::enum_<PlotStyles>("PlotStyles")
		.value("PLOT_STYLE_NONE", PLOT_STYLE_NONE)
		.value("PLOT_STYLE_NUMPAD_1", PLOT_STYLE_NUMPAD_1)
		.value("PLOT_STYLE_NUMPAD_2", PLOT_STYLE_NUMPAD_2)
		.value("PLOT_STYLE_NUMPAD_3", PLOT_STYLE_NUMPAD_3)
		.value("PLOT_STYLE_NUMPAD_4", PLOT_STYLE_NUMPAD_4)
		.value("PLOT_STYLE_NUMPAD_6", PLOT_STYLE_NUMPAD_6)
		.value("PLOT_STYLE_NUMPAD_7", PLOT_STYLE_NUMPAD_7)
		.value("PLOT_STYLE_NUMPAD_8", PLOT_STYLE_NUMPAD_8)
		.value("PLOT_STYLE_NUMPAD_9", PLOT_STYLE_NUMPAD_9)
		.value("PLOT_STYLE_NUMPAD_1_ANGLED", PLOT_STYLE_NUMPAD_1_ANGLED)
		.value("PLOT_STYLE_NUMPAD_2_ANGLED", PLOT_STYLE_NUMPAD_2_ANGLED)
		.value("PLOT_STYLE_NUMPAD_3_ANGLED", PLOT_STYLE_NUMPAD_3_ANGLED)
		.value("PLOT_STYLE_NUMPAD_4_ANGLED", PLOT_STYLE_NUMPAD_4_ANGLED)
		.value("PLOT_STYLE_NUMPAD_6_ANGLED", PLOT_STYLE_NUMPAD_6_ANGLED)
		.value("PLOT_STYLE_NUMPAD_7_ANGLED", PLOT_STYLE_NUMPAD_7_ANGLED)
		.value("PLOT_STYLE_NUMPAD_8_ANGLED", PLOT_STYLE_NUMPAD_8_ANGLED)
		.value("PLOT_STYLE_NUMPAD_9_ANGLED", PLOT_STYLE_NUMPAD_9_ANGLED)
		.value("PLOT_STYLE_BOX_FILL", PLOT_STYLE_BOX_FILL)
		.value("PLOT_STYLE_BOX_OUTLINE", PLOT_STYLE_BOX_OUTLINE)
		.value("PLOT_STYLE_RIVER_SOUTH", PLOT_STYLE_RIVER_SOUTH)
		.value("PLOT_STYLE_RIVER_EAST", PLOT_STYLE_RIVER_EAST)
		.value("PLOT_STYLE_SIDE_ARROWS", PLOT_STYLE_SIDE_ARROWS)
		.value("PLOT_STYLE_CIRCLE", PLOT_STYLE_CIRCLE)
		.value("PLOT_STYLE_TARGET", PLOT_STYLE_TARGET)
		.value("PLOT_STYLE_DOT_TARGET", PLOT_STYLE_DOT_TARGET)
		.value("PLOT_STYLE_WAVES", PLOT_STYLE_WAVES)
		.value("PLOT_STYLE_DOTS", PLOT_STYLE_DOTS)
		.value("PLOT_STYLE_CIRCLES", PLOT_STYLE_CIRCLES)
		;
	python::enum_<PlotLandscapeLayers>("PlotLandscapeLayers")
		.value("PLOT_LANDSCAPE_LAYER_ALL", PLOT_LANDSCAPE_LAYER_ALL)
		.value("PLOT_LANDSCAPE_LAYER_BASE", PLOT_LANDSCAPE_LAYER_BASE)
		.value("PLOT_LANDSCAPE_LAYER_RECOMMENDED_PLOTS", PLOT_LANDSCAPE_LAYER_RECOMMENDED_PLOTS)
		.value("PLOT_LANDSCAPE_LAYER_NUMPAD_HELP", PLOT_LANDSCAPE_LAYER_NUMPAD_HELP)
		.value("PLOT_LANDSCAPE_LAYER_REVEALED_PLOTS", PLOT_LANDSCAPE_LAYER_REVEALED_PLOTS)
		;
	python::enum_<AreaBorderLayers>("AreaBorderLayers")
		.value("AREA_BORDER_LAYER_REVEALED_PLOTS", AREA_BORDER_LAYER_REVEALED_PLOTS)
		.value("AREA_BORDER_LAYER_WORLD_BUILDER", AREA_BORDER_LAYER_WORLD_BUILDER)
		.value("AREA_BORDER_LAYER_FOUNDING_BORDER", AREA_BORDER_LAYER_FOUNDING_BORDER)
		.value("AREA_BORDER_LAYER_CITY_RADIUS", AREA_BORDER_LAYER_CITY_RADIUS)
		.value("AREA_BORDER_LAYER_RANGED", AREA_BORDER_LAYER_RANGED)
		.value("AREA_BORDER_LAYER_HIGHLIGHT_PLOT", AREA_BORDER_LAYER_HIGHLIGHT_PLOT)
		.value("AREA_BORDER_LAYER_EUROPE", AREA_BORDER_LAYER_EUROPE)
		.value("NUM_AREA_BORDER_LAYERS", NUM_AREA_BORDER_LAYERS)
		;
	python::enum_<InterfaceModeTypes>("InterfaceModeTypes")
		.value("NO_INTERFACEMODE", NO_INTERFACEMODE)
		.value("INTERFACEMODE_SELECTION", INTERFACEMODE_SELECTION)
		.value("INTERFACEMODE_PING", INTERFACEMODE_PING)
		.value("INTERFACEMODE_SIGN", INTERFACEMODE_SIGN)
		.value("INTERFACEMODE_GRIP", INTERFACEMODE_GRIP)
		.value("INTERFACEMODE_GLOBELAYER_INPUT", INTERFACEMODE_GLOBELAYER_INPUT)
		.value("INTERFACEMODE_GO_TO", INTERFACEMODE_GO_TO)
		.value("INTERFACEMODE_GO_TO_TYPE", INTERFACEMODE_GO_TO_TYPE)
		.value("INTERFACEMODE_GO_TO_ALL", INTERFACEMODE_GO_TO_ALL)
		.value("INTERFACEMODE_ROUTE_TO", INTERFACEMODE_ROUTE_TO)
		.value("INTERFACEMODE_PYTHON_PICK_PLOT", INTERFACEMODE_PYTHON_PICK_PLOT)
		.value("INTERFACEMODE_SAVE_PLOT_NIFS", INTERFACEMODE_SAVE_PLOT_NIFS)
		.value("INTERFACEMODE_ROUTE_TO_ROAD", INTERFACEMODE_ROUTE_TO_ROAD)
		.value("INTERFACEMODE_ROUTE_TO_COUNTRY_ROAD", INTERFACEMODE_ROUTE_TO_COUNTRY_ROAD)
		.value("NUM_INTERFACEMODE_TYPES", NUM_INTERFACEMODE_TYPES)
		;
	python::enum_<InterfaceMessageTypes>("InterfaceMessageTypes")
		.value("NO_MESSAGE_TYPE", NO_MESSAGE_TYPE)
		.value("MESSAGE_TYPE_INFO", MESSAGE_TYPE_INFO)
		.value("MESSAGE_TYPE_LOG_ONLY", MESSAGE_TYPE_LOG_ONLY)
		.value("MESSAGE_TYPE_DISPLAY_ONLY", MESSAGE_TYPE_DISPLAY_ONLY)
		.value("MESSAGE_TYPE_MAJOR_EVENT", MESSAGE_TYPE_MAJOR_EVENT)
		.value("MESSAGE_TYPE_MINOR_EVENT", MESSAGE_TYPE_MINOR_EVENT)
		.value("MESSAGE_TYPE_CHAT", MESSAGE_TYPE_CHAT)
		.value("MESSAGE_TYPE_COMBAT_MESSAGE", MESSAGE_TYPE_COMBAT_MESSAGE)
		.value("MESSAGE_TYPE_QUEST", MESSAGE_TYPE_QUEST)
		.value("MESSAGE_TYPE_TUTORIAL", MESSAGE_TYPE_TUTORIAL)
		.value("NUM_INTERFACE_MESSAGE_TYPES", NUM_INTERFACE_MESSAGE_TYPES)
		;
	python::enum_<MinimapModeTypes>("MinimapModeTypes")
		.value("NO_MINIMAPMODE", NO_MINIMAPMODE)
		.value("MINIMAPMODE_TERRITORY", MINIMAPMODE_TERRITORY)
		.value("MINIMAPMODE_TERRAIN", MINIMAPMODE_TERRAIN)
		.value("MINIMAPMODE_REPLAY", MINIMAPMODE_REPLAY)
		.value("MINIMAPMODE_MILITARY", MINIMAPMODE_MILITARY)
		.value("NUM_MINIMAPMODE_TYPES", NUM_MINIMAPMODE_TYPES)
		;
	python::enum_<EngineDirtyBits>("EngineDirtyBits")
		.value("GlobeTexture_DIRTY_BIT", GlobeTexture_DIRTY_BIT)
		.value("MinimapTexture_DIRTY_BIT", MinimapTexture_DIRTY_BIT)
		.value("CultureBorders_DIRTY_BIT", CultureBorders_DIRTY_BIT)
		.value("NUM_ENGINE_DIRTY_BITS", NUM_ENGINE_DIRTY_BITS)
		;
	python::enum_<InterfaceDirtyBits>("InterfaceDirtyBits")
		.value("SelectionCamera_DIRTY_BIT", SelectionCamera_DIRTY_BIT)
		.value("Fog_DIRTY_BIT", Fog_DIRTY_BIT)
		.value("GlobeLayer_DIRTY_BIT", GlobeLayer_DIRTY_BIT)
		.value("GlobeInfo_DIRTY_BIT", GlobeInfo_DIRTY_BIT)
		.value("Waypoints_DIRTY_BIT", Waypoints_DIRTY_BIT)
		.value("MiscButtons_DIRTY_BIT", MiscButtons_DIRTY_BIT)
		.value("PlotListButtons_DIRTY_BIT", PlotListButtons_DIRTY_BIT)
		.value("SelectionButtons_DIRTY_BIT", SelectionButtons_DIRTY_BIT)
		.value("CitizenButtons_DIRTY_BIT", CitizenButtons_DIRTY_BIT)
		.value("Event_DIRTY_BIT", Event_DIRTY_BIT)
		.value("Center_DIRTY_BIT", Center_DIRTY_BIT)
		.value("GameData_DIRTY_BIT", GameData_DIRTY_BIT)
		.value("Score_DIRTY_BIT", Score_DIRTY_BIT)
		.value("TurnTimer_DIRTY_BIT", TurnTimer_DIRTY_BIT)
		.value("Help_DIRTY_BIT", Help_DIRTY_BIT)
		.value("MinimapSection_DIRTY_BIT", MinimapSection_DIRTY_BIT)
		.value("SelectionSound_DIRTY_BIT", SelectionSound_DIRTY_BIT)
		.value("Cursor_DIRTY_BIT", Cursor_DIRTY_BIT)
		.value("CityInfo_DIRTY_BIT", CityInfo_DIRTY_BIT)
		.value("UnitInfo_DIRTY_BIT", UnitInfo_DIRTY_BIT)
		.value("Popup_DIRTY_BIT", Popup_DIRTY_BIT)
		.value("CityScreen_DIRTY_BIT", CityScreen_DIRTY_BIT)
		.value("InfoPane_DIRTY_BIT", InfoPane_DIRTY_BIT)
		.value("Flag_DIRTY_BIT", Flag_DIRTY_BIT)
		.value("HighlightPlot_DIRTY_BIT", HighlightPlot_DIRTY_BIT)
		.value("ColoredPlots_DIRTY_BIT", ColoredPlots_DIRTY_BIT)
		.value("Financial_Screen_DIRTY_BIT", Financial_Screen_DIRTY_BIT)
		.value("Foreign_Screen_DIRTY_BIT", Foreign_Screen_DIRTY_BIT)
		.value("Soundtrack_DIRTY_BIT", Soundtrack_DIRTY_BIT)
		.value("Domestic_Advisor_DIRTY_BIT", Domestic_Advisor_DIRTY_BIT)
		.value("Advanced_Start_DIRTY_BIT", Advanced_Start_DIRTY_BIT)
		.value("EuropeScreen_DIRTY_BIT", EuropeScreen_DIRTY_BIT)
		.value("ResourceTable_DIRTY_BIT", ResourceTable_DIRTY_BIT)
		.value("TradeRoutesAdvisor_DIRTY_BIT", TradeRoutesAdvisor_DIRTY_BIT)	// TAC - Trade Routes Advisor - koma13
		.value("AfricaScreen_DIRTY_BIT", AfricaScreen_DIRTY_BIT) /*** TRIANGLETRADE 10/24/08 by DPII ***/
		.value("PortRoyalScreen_DIRTY_BIT", PortRoyalScreen_DIRTY_BIT) // R&R, ray, Port Royal
		.value("NUM_INTERFACE_DIRTY_BITS", NUM_INTERFACE_DIRTY_BITS)
		;
	python::enum_<ClimateTypes>("ClimateTypes")
		.value("NO_CLIMATE", NO_CLIMATE)
		;
	python::enum_<SeaLevelTypes>("SeaLevelTypes")
		.value("NO_SEALEVEL", NO_SEALEVEL)
		;
	python::enum_<CustomMapOptionTypes>("CustomMapOptionTypes")
		.value("NO_CUSTOM_MAPOPTION", NO_CUSTOM_MAPOPTION)
		;
	python::enum_<PlotTypes>("PlotTypes")
		.value("NO_PLOT", NO_PLOT)
		.value("PLOT_PEAK", PLOT_PEAK)
		.value("PLOT_HILLS", PLOT_HILLS)
		.value("PLOT_LAND", PLOT_LAND)
		.value("PLOT_OCEAN", PLOT_OCEAN)
		.value("NUM_PLOT_TYPES", NUM_PLOT_TYPES)
		;
	python::enum_<EmphasizeTypes>("EmphasizeTypes")
		.value("NO_EMPHASIZE", NO_EMPHASIZE)
		;
	python::enum_<GameOptionTypes>("GameOptionTypes")
		.value("NO_GAMEOPTION", NO_GAMEOPTION)
		.value("GAMEOPTION_ADVANCED_START", GAMEOPTION_ADVANCED_START)
		.value("GAMEOPTION_NO_CITY_RAZING", GAMEOPTION_NO_CITY_RAZING)
		.value("GAMEOPTION_AGGRESSIVE_AI", GAMEOPTION_AGGRESSIVE_AI)
		.value("GAMEOPTION_LEAD_ANY_CIV", GAMEOPTION_LEAD_ANY_CIV)
		.value("GAMEOPTION_PERMANENT_ALLIANCES", GAMEOPTION_PERMANENT_ALLIANCES)
		.value("GAMEOPTION_ALWAYS_WAR", GAMEOPTION_ALWAYS_WAR)
		.value("GAMEOPTION_ALWAYS_PEACE", GAMEOPTION_ALWAYS_PEACE)
		.value("GAMEOPTION_ONE_CITY_CHALLENGE", GAMEOPTION_ONE_CITY_CHALLENGE)
		.value("GAMEOPTION_NO_CHANGING_WAR_PEACE", GAMEOPTION_NO_CHANGING_WAR_PEACE)
		.value("GAMEOPTION_NEW_RANDOM_SEED", GAMEOPTION_NEW_RANDOM_SEED)
		.value("GAMEOPTION_LOCK_MODS", GAMEOPTION_LOCK_MODS)
		.value("GAMEOPTION_NO_GOODY_HUTS", GAMEOPTION_NO_GOODY_HUTS)
		.value("GAMEOPTION_NO_EVENTS", GAMEOPTION_NO_EVENTS)
		.value("GAMEOPTION_REDUCED_REF", GAMEOPTION_REDUCED_REF)	// TAC - Reduced REF Option - koma13
		.value("GAMEOPTION_RANDOM_SETTLEMENT_AREAS", GAMEOPTION_RANDOM_SETTLEMENT_AREAS)	// TAC - Correct Geographical Placement of Natives - koma13
		.value("GAMEOPTION_USE_OLD_FOUNDING_FATHER_SYSTEM", GAMEOPTION_USE_OLD_FOUNDING_FATHER_SYSTEM)	// R&R, ray, Founding Fathers Available again, when Civ destroyed
		.value("GAMEOPTION_NO_MORE_VARIABLES_HIDDEN", GAMEOPTION_NO_MORE_VARIABLES_HIDDEN)	// R&R, Robert Surcouf, No More Variables Hidden game option
		.value("GAMEOPTION_NO_WILD_LAND_ANIMALS", GAMEOPTION_NO_WILD_LAND_ANIMALS) // < JAnimals Mod Start >
		.value("GAMEOPTION_NO_WILD_SEA_ANIMALS", GAMEOPTION_NO_WILD_SEA_ANIMALS) // < JAnimals Mod Start >
		.value("GAMEOPTION_REDUCED_CITY_DISTANCE", GAMEOPTION_REDUCED_CITY_DISTANCE) /// reduced city distance - Nightinggale
		.value("GAMEOPTION_DEACTIVATE_HEMISPHERE_RESTRICTIONS", GAMEOPTION_DEACTIVATE_HEMISPHERE_RESTRICTIONS) // ray, deactivate Hemisphere Restrictions
		.value("GAMEOPTION_ONLY_ONE_COLONIST_PER_VILLAGE", GAMEOPTION_ONLY_ONE_COLONIST_PER_VILLAGE) // WTP, ray, Game Option only 1 Colonist living in Village - START
		.value("NUM_GAMEOPTION_TYPES", NUM_GAMEOPTION_TYPES)
		;
	python::enum_<MultiplayerOptionTypes>("MultiplayerOptionTypes")
		.value("NO_MPOPTION", NO_MPOPTION)
		.value("MPOPTION_SIMULTANEOUS_TURNS", MPOPTION_SIMULTANEOUS_TURNS)
		.value("MPOPTION_TAKEOVER_AI", MPOPTION_TAKEOVER_AI)
		.value("MPOPTION_SHUFFLE_TEAMS", MPOPTION_SHUFFLE_TEAMS)
		.value("MPOPTION_ANONYMOUS", MPOPTION_ANONYMOUS)
		.value("MPOPTION_TURN_TIMER", MPOPTION_TURN_TIMER)
		.value("NUM_MPOPTION_TYPES", NUM_MPOPTION_TYPES)
		;
	python::enum_<SpecialOptionTypes>("SpecialOptionTypes")
		.value("NO_SPECIALOPTION", NO_SPECIALOPTION)
		.value("SPECIALOPTION_REPORT_STATS", SPECIALOPTION_REPORT_STATS)
		.value("NUM_SPECIALOPTION_TYPES", NUM_SPECIALOPTION_TYPES)
		;
	python::enum_<PlayerOptionTypes>("PlayerOptionTypes")
		.value("NO_PLAYEROPTION", NO_PLAYEROPTION)
		.value("PLAYEROPTION_TUTORIAL", PLAYEROPTION_TUTORIAL)
		.value("PLAYEROPTION_ADVISOR_POPUPS", PLAYEROPTION_ADVISOR_POPUPS)
		.value("PLAYEROPTION_ADVISOR_HELP", PLAYEROPTION_ADVISOR_HELP)
		.value("PLAYEROPTION_WAIT_END_TURN", PLAYEROPTION_WAIT_END_TURN)
		.value("PLAYEROPTION_MINIMIZE_POP_UPS", PLAYEROPTION_MINIMIZE_POP_UPS)
		.value("PLAYEROPTION_SHOW_FRIENDLY_MOVES", PLAYEROPTION_SHOW_FRIENDLY_MOVES)
		.value("PLAYEROPTION_SHOW_ENEMY_MOVES", PLAYEROPTION_SHOW_ENEMY_MOVES)
		.value("PLAYEROPTION_QUICK_MOVES", PLAYEROPTION_QUICK_MOVES)
		.value("PLAYEROPTION_QUICK_ATTACK", PLAYEROPTION_QUICK_ATTACK)
		.value("PLAYEROPTION_QUICK_DEFENSE", PLAYEROPTION_QUICK_DEFENSE)
		.value("PLAYEROPTION_STACK_ATTACK", PLAYEROPTION_STACK_ATTACK)
		.value("PLAYEROPTION_AUTO_PROMOTION", PLAYEROPTION_AUTO_PROMOTION)
		.value("PLAYEROPTION_SAFE_AUTOMATION", PLAYEROPTION_SAFE_AUTOMATION)
		.value("PLAYEROPTION_NUMPAD_HELP", PLAYEROPTION_NUMPAD_HELP)
		.value("PLAYEROPTION_NO_UNIT_CYCLING", PLAYEROPTION_NO_UNIT_CYCLING)
		.value("PLAYEROPTION_NO_UNIT_RECOMMENDATIONS", PLAYEROPTION_NO_UNIT_RECOMMENDATIONS)
		.value("PLAYEROPTION_RIGHT_CLICK_MENU", PLAYEROPTION_RIGHT_CLICK_MENU)
		.value("PLAYEROPTION_LEAVE_FORESTS", PLAYEROPTION_LEAVE_FORESTS)
		.value("PLAYEROPTION_MODDER_1", PLAYEROPTION_MODDER_1)
		.value("PLAYEROPTION_MODDER_2", PLAYEROPTION_MODDER_2)
		.value("PLAYEROPTION_MODDER_3", PLAYEROPTION_MODDER_3)
		.value("NUM_PLAYEROPTION_TYPES", NUM_PLAYEROPTION_TYPES)
		;
	python::enum_<GraphicOptionTypes>("GraphicOptionTypes")
		.value("NO_GRAPHICOPTION", NO_GRAPHICOPTION)
		.value("GRAPHICOPTION_HEALTH_BARS", GRAPHICOPTION_HEALTH_BARS)
		.value("GRAPHICOPTION_NO_COMBAT_ZOOM", GRAPHICOPTION_NO_COMBAT_ZOOM)
		.value("GRAPHICOPTION_NO_ENEMY_GLOW", GRAPHICOPTION_NO_ENEMY_GLOW)
		.value("GRAPHICOPTION_FROZEN_ANIMATIONS", GRAPHICOPTION_FROZEN_ANIMATIONS)
		.value("GRAPHICOPTION_EFFECTS_DISABLED", GRAPHICOPTION_EFFECTS_DISABLED)
		.value("GRAPHICOPTION_GLOBE_VIEW_BUILDINGS_DISABLED", GRAPHICOPTION_GLOBE_VIEW_BUILDINGS_DISABLED)
		.value("GRAPHICOPTION_FULLSCREEN", GRAPHICOPTION_FULLSCREEN)
		.value("GRAPHICOPTION_LOWRES_TEXTURES", GRAPHICOPTION_LOWRES_TEXTURES)
		.value("GRAPHICOPTION_HIRES_TERRAIN", GRAPHICOPTION_HIRES_TERRAIN)
		.value("GRAPHICOPTION_NO_MOVIES", GRAPHICOPTION_NO_MOVIES)
		.value("NUM_GRAPHICOPTION_TYPES", NUM_GRAPHICOPTION_TYPES)
		;
	python::enum_<ForceControlTypes>("ForceControlTypes")
		.value("NO_FORCECONTROL", NO_FORCECONTROL)
		.value("FORCECONTROL_SPEED", FORCECONTROL_SPEED)
		.value("FORCECONTROL_HANDICAP", FORCECONTROL_HANDICAP)
		.value("FORCECONTROL_OPTIONS", FORCECONTROL_OPTIONS)
		.value("FORCECONTROL_VICTORIES", FORCECONTROL_VICTORIES)
		.value("FORCECONTROL_MAX_TURNS", FORCECONTROL_MAX_TURNS)
		.value("FORCECONTROL_MAX_CITY_ELIMINATIONS", FORCECONTROL_MAX_CITY_ELIMINATION)
		.value("FORCECONTROL_ADVANCED_START", FORCECONTROL_ADVANCED_START)
		.value("NUM_FORCECONTROL_TYPES", NUM_FORCECONTROL_TYPES)
		;
	python::enum_<VictoryTypes>("VictoryTypes")
		.value("NO_VICTORY", NO_VICTORY)
		;
	python::enum_<BonusTypes>("BonusTypes")
		.value("NO_BONUS", NO_BONUS)
		;
	python::enum_<ImprovementTypes>("ImprovementTypes")
		.value("NO_IMPROVEMENT", NO_IMPROVEMENT)
		;
	python::enum_<RouteTypes>("RouteTypes")
		.value("NO_ROUTE", NO_ROUTE)
		;
	python::enum_<GoodyTypes>("GoodyTypes")
		.value("NO_GOODY", NO_GOODY)
		;
	python::enum_<BuildTypes>("BuildTypes")
		.value("NO_BUILD", NO_BUILD)
		;
	python::enum_<GameSpeedTypes>("GameSpeedTypes")
		.value("NO_GAMESPEED", NO_GAMESPEED)
		;
	python::enum_<TurnTimerTypes>("TurnTimerTypes")
		.value("NO_TURNTIMER", NO_TURNTIMER)
		;
	python::enum_<EraTypes>("EraTypes")
		.value("NO_ERA", NO_ERA)
		;
	python::enum_<CivilizationTypes>("CivilizationTypes")
		.value("NO_CIVILIZATION", NO_CIVILIZATION)
		.value("CIVILIZATION_BARBARIAN", CIVILIZATION_BARBARIAN)
		.value("CIVILIZATION_CHURCH", CIVILIZATION_CHURCH)
		;
	python::enum_<LeaderHeadTypes>("LeaderHeadTypes")
		.value("NO_LEADER", NO_LEADER)
		;
	python::enum_<ArtStyleTypes>("ArtStyleTypes")
		.value("NO_ARTSTYLE", NO_ARTSTYLE)
		;
	//Androrc UnitArtStyles
	python::enum_<UnitArtStyleTypes>("UnitArtStyleTypes")
		.value("NO_UNIT_ARTSTYLE", NO_UNIT_ARTSTYLE)
		;
	//Androrc End
	python::enum_<CitySizeTypes>("CitySizeTypes")
		.value("NO_CITYSIZE", NO_CITYSIZE)
		.value("CITYSIZE_SMALL", CITYSIZE_SMALL)
		.value("CITYSIZE_MEDIUM", CITYSIZE_MEDIUM)
		.value("CITYSIZE_LARGE", CITYSIZE_LARGE)
		.value("NUM_CITYSIZE_TYPES", NUM_CITYSIZE_TYPES)
		;
	python::enum_<FootstepAudioTypes>("FootstepAudioTypes")
		.value("NO_FOOTSTEPAUDIO", NO_FOOTSTEPAUDIO)
		;
	python::enum_<FootstepAudioTags>("FootstepAudioTags")
		.value("NO_FOOTSTEPAUDIO_TAG", NO_FOOTSTEPAUDIO_TAG)
		;
	python::enum_<ChatTargetTypes>("ChatTargetTypes")
		.value("NO_CHATTARGET", NO_CHATTARGET)
		.value("CHATTARGET_ALL", CHATTARGET_ALL)
		.value("CHATTARGET_TEAM", CHATTARGET_TEAM)
		;
	python::enum_<VoiceTargetTypes>("VoiceTargetTypes")
		.value("NO_VOICETARGET", NO_VOICETARGET)
		.value("VOICETARGET_DIPLO", VOICETARGET_DIPLO)
		.value("VOICETARGET_TEAM", VOICETARGET_TEAM)
		.value("VOICETARGET_ALL", VOICETARGET_ALL)
		.value("NUM_VOICETARGETS", NUM_VOICETARGETS)
		;
	python::enum_<TeamTypes>("TeamTypes")
		.value("UNKNOWN_TEAM", UNKNOWN_TEAM)
		.value("NO_TEAM", NO_TEAM)
		;
	python::enum_<PlayerTypes>("PlayerTypes")
		.value("UNKNOWN_PLAYER", UNKNOWN_PLAYER)
		.value("NO_PLAYER", NO_PLAYER)
		;
	python::enum_<TraitTypes>("TraitTypes")
		.value("NO_TRAIT", NO_TRAIT)
		;
	python::enum_<OrderTypes>("OrderTypes")
		.value("NO_ORDER", NO_ORDER)
		.value("ORDER_TRAIN", ORDER_TRAIN)
		.value("ORDER_CONSTRUCT", ORDER_CONSTRUCT)
		.value("ORDER_CONVINCE", ORDER_CONVINCE)
		.value("NUM_ORDER_TYPES", NUM_ORDER_TYPES)
		;
	python::enum_<TaskTypes>("TaskTypes")
		.value("TASK_RAZE", TASK_RAZE)
		.value("TASK_GIFT", TASK_GIFT)
		.value("TASK_SET_ALL_CITIZENS_AUTOMATED", TASK_SET_ALL_CITIZENS_AUTOMATED)
		.value("TASK_SET_CITIZEN_AUTOMATED", TASK_SET_CITIZEN_AUTOMATED)
		.value("TASK_SET_AUTOMATED_PRODUCTION", TASK_SET_AUTOMATED_PRODUCTION)
		.value("TASK_SET_EMPHASIZE", TASK_SET_EMPHASIZE)
		.value("TASK_CHANGE_WORKING_PLOT", TASK_CHANGE_WORKING_PLOT)
		.value("TASK_CHANGE_PROFESSION", TASK_CHANGE_PROFESSION)
		.value("TASK_REPLACE_CITIZEN", TASK_REPLACE_CITIZEN)
		.value("TASK_EJECT_TO_TRANSPORT", TASK_EJECT_TO_TRANSPORT)
		.value("TASK_CLEAR_WORKING_OVERRIDE", TASK_CLEAR_WORKING_OVERRIDE)
		.value("TASK_HURRY", TASK_HURRY)
		.value("TASK_CLEAR_ORDERS", TASK_CLEAR_ORDERS)
		.value("TASK_RALLY_PLOT", TASK_RALLY_PLOT)
		.value("TASK_CLEAR_RALLY_PLOT", TASK_CLEAR_RALLY_PLOT)
		.value("TASK_LIBERATE", TASK_LIBERATE)
		.value("TASK_YIELD_IMPORT", TASK_YIELD_IMPORT)
		.value("TASK_YIELD_EXPORT", TASK_YIELD_EXPORT)
		.value("TASK_YIELD_LEVEL", TASK_YIELD_LEVEL)
		.value("TASK_YIELD_TRADEROUTE", TASK_YIELD_TRADEROUTE) // transport feeder - Nightinggale
		.value("TASK_CLEAR_SPECIALTY", TASK_CLEAR_SPECIALTY)
		.value("TASK_EDUCATE", TASK_EDUCATE)
		.value("TASK_PUSH_TRAIN_UNIT", TASK_PUSH_TRAIN_UNIT)
		.value("TASK_PUSH_CONSTRUCT_BUILDING", TASK_PUSH_CONSTRUCT_BUILDING)
		.value("TASK_CHEAT", TASK_CHEAT)
		//Androrc Multiple Professions per Building
		.value("TASK_CHANGE_WORKING_BUILDING", TASK_CHANGE_WORKING_BUILDING)
		//Androrc End
		.value("NUM_TASK_TYPES", NUM_TASK_TYPES)
		;
	python::enum_<PlayerActionTypes>("PlayerActionTypes")
		.value("PLAYER_ACTION_BUY_EUROPE_UNIT", PLAYER_ACTION_BUY_EUROPE_UNIT)
		.value("PLAYER_ACTION_SELL_YIELD_UNIT", PLAYER_ACTION_SELL_YIELD_UNIT)
		.value("PLAYER_ACTION_BUY_YIELD_UNIT", PLAYER_ACTION_BUY_YIELD_UNIT)
		.value("PLAYER_ACTION_LOAD_UNIT_FROM_EUROPE", PLAYER_ACTION_LOAD_UNIT_FROM_EUROPE)
		.value("PLAYER_ACTION_UNLOAD_UNIT_TO_EUROPE", PLAYER_ACTION_UNLOAD_UNIT_TO_EUROPE)
		.value("PLAYER_ACTION_TRANSFER_UNIT_IN_EUROPE", PLAYER_ACTION_TRANSFER_UNIT_IN_EUROPE)
		.value("PLAYER_ACTION_SET_OPTION", PLAYER_ACTION_SET_OPTION)
		.value("PLAYER_ACTION_AUTO_MOVES", PLAYER_ACTION_AUTO_MOVES)
		.value("PLAYER_ACTION_TURN_COMPLETE", PLAYER_ACTION_TURN_COMPLETE)
		.value("PLAYER_ACTION_APPLY_EVENT", PLAYER_ACTION_APPLY_EVENT)
		.value("PLAYER_ACTION_CONVINCE_FATHER", PLAYER_ACTION_CONVINCE_FATHER)
		.value("PLAYER_ACTION_SET_CIVIC", PLAYER_ACTION_SET_CIVIC)
		.value("PLAYER_ACTION_RECEIVE_GOODY", PLAYER_ACTION_RECEIVE_GOODY)
		.value("PLAYER_ACTION_FEAT", PLAYER_ACTION_FEAT)
		.value("PLAYER_ACTION_BUY_UNITS_FROM_KING", PLAYER_ACTION_BUY_UNITS_FROM_KING)
		.value("PLAYER_ACTION_HURRY", PLAYER_ACTION_HURRY)
		.value("PLAYER_ACTION_EUROPE_CHANGE_PROFESSION", PLAYER_ACTION_EUROPE_CHANGE_PROFESSION)
		/*** TRIANGLETRADE 10/24/08 by DPII ***/
		.value("PLAYER_ACTION_BUY_AFRICA_UNIT", PLAYER_ACTION_BUY_AFRICA_UNIT)
		.value("PLAYER_ACTION_SELL_YIELD_UNIT_AFRICA", PLAYER_ACTION_SELL_YIELD_UNIT_AFRICA)
		.value("PLAYER_ACTION_BUY_YIELD_UNIT_AFRICA", PLAYER_ACTION_BUY_YIELD_UNIT_AFRICA)
		.value("PLAYER_ACTION_LOAD_UNIT_FROM_AFRICA", PLAYER_ACTION_LOAD_UNIT_FROM_AFRICA)
		.value("PLAYER_ACTION_UNLOAD_UNIT_TO_AFRICA", PLAYER_ACTION_UNLOAD_UNIT_TO_AFRICA)
		.value("PLAYER_ACTION_TRANSFER_UNIT_IN_AFRICA", PLAYER_ACTION_TRANSFER_UNIT_IN_AFRICA)
		// R&R, ray, Port Royal
		.value("PLAYER_ACTION_BUY_PORT_ROYAL_UNIT", PLAYER_ACTION_BUY_PORT_ROYAL_UNIT)
		.value("PLAYER_ACTION_SELL_YIELD_UNIT_PORT_ROYAL", PLAYER_ACTION_SELL_YIELD_UNIT_PORT_ROYAL)
		.value("PLAYER_ACTION_BUY_YIELD_UNIT_PORT_ROYAL", PLAYER_ACTION_BUY_YIELD_UNIT_PORT_ROYAL)
		.value("PLAYER_ACTION_LOAD_UNIT_FROM_PORT_ROYAL", PLAYER_ACTION_LOAD_UNIT_FROM_PORT_ROYAL)
		.value("PLAYER_ACTION_UNLOAD_UNIT_TO_PORT_ROYAL", PLAYER_ACTION_UNLOAD_UNIT_TO_PORT_ROYAL)
		.value("PLAYER_ACTION_TRANSFER_UNIT_IN_PORT_ROYAL", PLAYER_ACTION_TRANSFER_UNIT_IN_PORT_ROYAL)
		.value("NUM_PLAYER_ACTION_TYPES", NUM_PLAYER_ACTION_TYPES)
		;
	python::enum_<BuildingClassTypes>("BuildingClassTypes")
		.value("NO_BUILDINGCLASS", NO_BUILDINGCLASS)
		;
	python::enum_<BuildingTypes>("BuildingTypes")
		.value("NO_BUILDING", NO_BUILDING)
		.value("BUILDING_CHICKEE", BUILDING_CHICKEE)
		;
	python::enum_<InfoBarTypes>("InfoBarTypes")
		.value("INFOBAR_STORED", INFOBAR_STORED)
		.value("INFOBAR_RATE", INFOBAR_RATE)
		.value("INFOBAR_RATE_EXTRA", INFOBAR_RATE_EXTRA)
		.value("INFOBAR_EMPTY", INFOBAR_EMPTY)
		.value("NUM_INFOBAR_TYPES", NUM_INFOBAR_TYPES)
		;
	python::enum_<HealthBarTypes>("HealthBarTypes")
		.value("HEALTHBAR_ALIVE_ATTACK", HEALTHBAR_ALIVE_ATTACK)
		.value("HEALTHBAR_ALIVE_DEFEND", HEALTHBAR_ALIVE_DEFEND)
		.value("HEALTHBAR_DEAD", HEALTHBAR_DEAD)
		.value("NUM_HEALTHBAR_TYPES", NUM_HEALTHBAR_TYPES)
		;
	python::enum_<CalendarTypes>("CalendarTypes")
		.value("CALENDAR_DEFAULT", CALENDAR_DEFAULT)
		.value("CALENDAR_BI_YEARLY", CALENDAR_BI_YEARLY)
		.value("CALENDAR_YEARS", CALENDAR_YEARS)
		.value("CALENDAR_TURNS", CALENDAR_TURNS)
		.value("CALENDAR_SEASONS", CALENDAR_SEASONS)
		.value("CALENDAR_MONTHS", CALENDAR_MONTHS)
		.value("CALENDAR_WEEKS", CALENDAR_WEEKS)
		;
	python::enum_<SeasonTypes>("SeasonTypes")
		.value("NO_SEASON", NO_SEASON)
		;
	python::enum_<MonthTypes>("MonthTypes")
		.value("NO_MONTH", NO_MONTH)
		;
	python::enum_<DenialTypes>("DenialTypes")
		.value("NO_DENIAL", NO_DENIAL)
		.value("DENIAL_UNKNOWN", DENIAL_UNKNOWN)
		.value("DENIAL_NEVER", DENIAL_NEVER)
		.value("DENIAL_TOO_MUCH", DENIAL_TOO_MUCH)
		.value("DENIAL_MYSTERY", DENIAL_MYSTERY)
		.value("DENIAL_JOKING", DENIAL_JOKING)
		.value("DENIAL_CONTACT_THEM", DENIAL_CONTACT_THEM)
		.value("DENIAL_VICTORY", DENIAL_VICTORY)
		.value("DENIAL_ATTITUDE", DENIAL_ATTITUDE)
		.value("DENIAL_ATTITUDE_THEM", DENIAL_ATTITUDE_THEM)
		.value("DENIAL_POWER_US", DENIAL_POWER_US)
		.value("DENIAL_POWER_YOU", DENIAL_POWER_YOU)
		.value("DENIAL_POWER_THEM", DENIAL_POWER_THEM)
		.value("DENIAL_TOO_MANY_WARS", DENIAL_TOO_MANY_WARS)
		.value("DENIAL_NO_GAIN", DENIAL_NO_GAIN)
		.value("DENIAL_NOT_INTERESTED", DENIAL_NOT_INTERESTED)
		.value("DENIAL_NOT_ALLIED", DENIAL_NOT_ALLIED)
		.value("DENIAL_RECENT_CANCEL", DENIAL_RECENT_CANCEL)
		.value("DENIAL_WORST_ENEMY", DENIAL_WORST_ENEMY)
		.value("DENIAL_POWER_YOUR_ENEMIES", DENIAL_POWER_YOUR_ENEMIES)
		.value("DENIAL_TOO_FAR", DENIAL_TOO_FAR)
		.value("NUM_DENIAL_TYPES", NUM_DENIAL_TYPES)
		;
	python::enum_<DomainTypes>("DomainTypes")
		.value("DOMAIN_SEA", DOMAIN_SEA)
		.value("DOMAIN_LAND", DOMAIN_LAND)
		.value("DOMAIN_IMMOBILE", DOMAIN_IMMOBILE)
		.value("NUM_DOMAIN_TYPES", NUM_DOMAIN_TYPES)
		;
	python::enum_<SpecialUnitTypes>("SpecialUnitTypes")
		.value("NO_SPECIALUNIT", NO_SPECIALUNIT)
		;
	python::enum_<UnitCombatTypes>("UnitCombatTypes")
		.value("NO_UNITCOMBAT", NO_UNITCOMBAT)
		;
	python::enum_<UnitAITypes>("UnitAITypes")
		.value("NO_UNITAI", NO_UNITAI)
		.value("UNITAI_UNKNOWN", UNITAI_UNKNOWN)
		.value("UNITAI_ANIMAL", UNITAI_ANIMAL) // < JAnimals Mod Start >
		.value("UNITAI_ANIMAL_SEA", UNITAI_ANIMAL_SEA) // R&R, ray, Wild Animal
		.value("UNITAI_FLEEING", UNITAI_FLEEING) // R&R, ray, Fleeing Units
		.value("UNITAI_COLONIST", UNITAI_COLONIST)
		.value("UNITAI_SETTLER", UNITAI_SETTLER)
		.value("UNITAI_WORKER", UNITAI_WORKER)
		.value("UNITAI_MISSIONARY", UNITAI_MISSIONARY)
		.value("UNITAI_TRADER", UNITAI_TRADER) // WTP, ray, Native Trade Posts - START
		.value("UNITAI_SCOUT", UNITAI_SCOUT)
		.value("UNITAI_WAGON", UNITAI_WAGON)
		.value("UNITAI_TREASURE", UNITAI_TREASURE)
		.value("UNITAI_DEFENSIVE", UNITAI_DEFENSIVE)
		.value("UNITAI_OFFENSIVE", UNITAI_DEFENSIVE)
		.value("UNITAI_COUNTER", UNITAI_COUNTER)
		.value("UNITAI_WORKER_SEA", UNITAI_WORKER_SEA) //TAC Whaling, ray
		.value("UNITAI_TRANSPORT_SEA", UNITAI_TRANSPORT_SEA)
		.value("UNITAI_ASSAULT_SEA", UNITAI_ASSAULT_SEA)
		.value("UNITAI_COMBAT_SEA", UNITAI_COMBAT_SEA)
		.value("UNITAI_PIRATE_SEA", UNITAI_PIRATE_SEA)
		.value("UNITAI_ESCORT_SEA", UNITAI_ESCORT_SEA) // TAC - AI Escort Sea - koma13
		.value("UNITAI_TRANSPORT_COAST", UNITAI_TRANSPORT_COAST)
		.value("NUM_UNITAI_TYPES", NUM_UNITAI_TYPES)
		;
	python::enum_<InvisibleTypes>("InvisibleTypes")
		.value("NO_INVISIBLE", NO_INVISIBLE)
		.value("INVISIBLE_RANGER", INVISIBLE_RANGER) // R&R, ray, Rangers
		;
	python::enum_<ProbabilityTypes>("ProbabilityTypes")
		.value("NO_PROBABILITY", NO_PROBABILITY)
		.value("PROBABILITY_LOW", PROBABILITY_LOW)
		.value("PROBABILITY_REAL", PROBABILITY_REAL)
		.value("PROBABILITY_HIGH", PROBABILITY_HIGH)
		.value("NUM_PROBABILITY_TYPES", NUM_PROBABILITY_TYPES)
		;
	python::enum_<ActivityTypes>("ActivityTypes")
		.value("NO_ACTIVITY", NO_ACTIVITY)
		.value("ACTIVITY_AWAKE", ACTIVITY_AWAKE)
		.value("ACTIVITY_HOLD", ACTIVITY_HOLD)
		.value("ACTIVITY_SLEEP", ACTIVITY_SLEEP)
		.value("ACTIVITY_HEAL", ACTIVITY_HEAL)
		.value("ACTIVITY_SENTRY", ACTIVITY_SENTRY)
		.value("ACTIVITY_MISSION", ACTIVITY_MISSION)
		.value("NUM_ACTIVITY_TYPES", NUM_ACTIVITY_TYPES)
		;
	python::enum_<AutomateTypes>("AutomateTypes")
		.value("NO_AUTOMATE", NO_AUTOMATE)
		.value("AUTOMATE_BUILD", AUTOMATE_BUILD)
		.value("AUTOMATE_CITY", AUTOMATE_CITY)
		.value("AUTOMATE_EXPLORE", AUTOMATE_EXPLORE)
		.value("AUTOMATE_SAIL", AUTOMATE_SAIL_TO_EUROPE)
		.value("AUTOMATE_TRANSPORT_ROUTES", AUTOMATE_TRANSPORT_ROUTES)
		.value("AUTOMATE_TRANSPORT_FULL", AUTOMATE_TRANSPORT_FULL)
		.value("AUTOMATE_WORKER_SEA", AUTOMATE_WORKER_SEA) //TAC Whaling, ray
		.value("AUTOMATE_FULL", AUTOMATE_FULL)
		.value("NUM_AUTOMATE_TYPES", NUM_AUTOMATE_TYPES)
		;
	python::enum_<MissionTypes>("MissionTypes")
		.value("NO_MISSION", NO_MISSION)
		.value("MISSION_MOVE_TO", MISSION_MOVE_TO)
		.value("MISSION_ROUTE_TO", MISSION_ROUTE_TO)
		.value("MISSION_ROUTE_TO_ROAD", MISSION_ROUTE_TO)
		.value("MISSION_ROUTE_TO_COUNTRY_ROAD", MISSION_ROUTE_TO)
		.value("MISSION_MOVE_TO_UNIT", MISSION_MOVE_TO_UNIT)
		.value("MISSION_SKIP", MISSION_SKIP)
		.value("MISSION_SLEEP", MISSION_SLEEP)
		.value("MISSION_FORTIFY", MISSION_FORTIFY)
		.value("MISSION_HEAL", MISSION_HEAL)
		.value("MISSION_SENTRY", MISSION_SENTRY)
		.value("MISSION_BOMBARD", MISSION_BOMBARD)
		.value("MISSION_PILLAGE", MISSION_PILLAGE)
		.value("MISSION_FOUND", MISSION_FOUND)
		.value("MISSION_JOIN_CITY", MISSION_JOIN_CITY)
		.value("MISSION_LEAD", MISSION_LEAD)
		.value("MISSION_BUILD", MISSION_BUILD)
		.value("MISSION_BEGIN_COMBAT", MISSION_BEGIN_COMBAT )
		.value("MISSION_END_COMBAT", MISSION_END_COMBAT )
		.value("MISSION_SURRENDER", MISSION_SURRENDER )
		.value("MISSION_CAPTURED", MISSION_CAPTURED )
		.value("MISSION_IDLE", MISSION_IDLE )
		.value("MISSION_DIE", MISSION_DIE )
		.value("MISSION_DAMAGE", MISSION_DAMAGE )
		.value("MISSION_MULTI_SELECT", MISSION_MULTI_SELECT )
		.value("MISSION_MULTI_DESELECT", MISSION_MULTI_DESELECT )
		.value("MISSION_WHALING", MISSION_WHALING ) //TAC Whaling, ray
		.value("MISSION_FISHING", MISSION_FISHING ) // R&R, ray, High Sea Fishing
		.value("NUM_MISSION_TYPES", NUM_MISSION_TYPES )
		;
	python::enum_<MissionAITypes>("MissionAITypes")
		.value("NO_MISSIONAI", NO_MISSIONAI)
		.value("MISSIONAI_SHADOW", MISSIONAI_SHADOW)
		.value("MISSIONAI_GROUP", MISSIONAI_GROUP)
		.value("MISSIONAI_LOAD_ASSAULT", MISSIONAI_LOAD_ASSAULT)
		.value("MISSIONAI_LOAD_SETTLER", MISSIONAI_LOAD_SETTLER)
		.value("MISSIONAI_LOAD_SPECIAL", MISSIONAI_LOAD_SPECIAL)
		.value("MISSIONAI_GUARD_CITY", MISSIONAI_GUARD_CITY)
		.value("MISSIONAI_GUARD_BONUS", MISSIONAI_GUARD_BONUS)
		.value("MISSIONAI_GUARD_TRADE_NET", MISSIONAI_GUARD_TRADE_NET)
		.value("MISSIONAI_SPREAD", MISSIONAI_SPREAD)
		.value("MISSIONAI_EXPLORE", MISSIONAI_EXPLORE)
		.value("MISSIONAI_PILLAGE", MISSIONAI_PILLAGE)
		.value("MISSIONAI_FOUND", MISSIONAI_FOUND)
		.value("MISSIONAI_BUILD", MISSIONAI_BUILD)
		.value("MISSIONAI_ASSAULT", MISSIONAI_ASSAULT)
		.value("MISSIONAI_PICKUP", MISSIONAI_PICKUP)
		.value("MISSIONAI_AWAIT_PICKUP", MISSIONAI_AWAIT_PICKUP)
		.value("MISSIONAI_SAIL_TO_EUROPE", MISSIONAI_SAIL_TO_EUROPE)
		.value("MISSIONAI_SAIL_FROM_EUROPE", MISSIONAI_SAIL_FROM_EUROPE)
		.value("MISSIONAI_WORKER_SEA", MISSIONAI_WORKER_SEA)//TAC Whaling, ray
		.value("MISSIONAI_TRANSPORT", MISSIONAI_TRANSPORT)
		.value("MISSIONAI_TRANSPORT_SEA", MISSIONAI_TRANSPORT_SEA)
		.value("MISSIONAI_PIRACY", MISSIONAI_PIRACY)
		.value("MISSIONAI_BOMBARD", MISSIONAI_BOMBARD)
		.value("MISSIONAI_LEARN", MISSIONAI_LEARN)
		/*** TRIANGLETRADE 10/24/08 by DPII ***/
		.value("MISSIONAI_SAIL_TO_AFRICA", MISSIONAI_SAIL_TO_AFRICA)
		.value("MISSIONAI_SAIL_FROM_AFRICA", MISSIONAI_SAIL_FROM_AFRICA)
		// R&R, ray, Port Royal
		.value("MISSIONAI_SAIL_TO_PORT_ROYAL", MISSIONAI_SAIL_TO_PORT_ROYAL)
		.value("MISSIONAI_SAIL_FROM_PORT_ROYAL", MISSIONAI_SAIL_FROM_PORT_ROYAL)
		.value("NUM_MISSIONAI_TYPES", NUM_MISSIONAI_TYPES)
		;
	// any additions need to be reflected in GlobalTypes.xml
	python::enum_<CommandTypes>("CommandTypes")
		.value("NO_COMMAND", NO_COMMAND)
		.value("COMMAND_PROMOTION", COMMAND_PROMOTION)
		.value("COMMAND_UPGRADE", COMMAND_UPGRADE)
		.value("COMMAND_AUTOMATE", COMMAND_AUTOMATE)
		.value("COMMAND_WAKE", COMMAND_WAKE)
		.value("COMMAND_CANCEL", COMMAND_CANCEL)
		.value("COMMAND_CANCEL_ALL", COMMAND_CANCEL_ALL)
		.value("COMMAND_STOP_AUTOMATION", COMMAND_STOP_AUTOMATION)
		.value("COMMAND_DELETE", COMMAND_DELETE)
		.value("COMMAND_GIFT", COMMAND_GIFT)
		.value("COMMAND_LOAD", COMMAND_LOAD)
		.value("COMMAND_LOAD_UNIT", COMMAND_LOAD_UNIT)
		.value("COMMAND_LOAD_CARGO", COMMAND_LOAD_CARGO)
		.value("COMMAND_LOAD_YIELD", COMMAND_LOAD_YIELD)
		.value("COMMAND_UNLOAD", COMMAND_UNLOAD)
		.value("COMMAND_UNLOAD_ALL", COMMAND_UNLOAD_ALL)
		.value("COMMAND_LEARN", COMMAND_LEARN)
		.value("COMMAND_KING_TRANSPORT", COMMAND_KING_TRANSPORT)
		.value("COMMAND_ESTABLISH_MISSION", COMMAND_ESTABLISH_MISSION)
		.value("COMMAND_STIR_UP_NATIVES", COMMAND_STIR_UP_NATIVES) // R&R, ray , Stirring Up Natives - START
		.value("COMMAND_SPEAK_WITH_CHIEF", COMMAND_SPEAK_WITH_CHIEF)
		.value("COMMAND_YIELD_TRADE", COMMAND_YIELD_TRADE)
		.value("COMMAND_SAIL_TO_EUROPE", COMMAND_SAIL_TO_EUROPE)
		.value("COMMAND_CHOOSE_TRADE_ROUTES", COMMAND_CHOOSE_TRADE_ROUTES)
		.value("COMMAND_ASSIGN_TRADE_ROUTE", COMMAND_ASSIGN_TRADE_ROUTE)
		.value("COMMAND_PROMOTE", COMMAND_PROMOTE)
		.value("COMMAND_PROFESSION", COMMAND_PROFESSION)
		.value("COMMAND_CLEAR_SPECIALTY", COMMAND_CLEAR_SPECIALTY)
		.value("COMMAND_HOTKEY", COMMAND_HOTKEY)
		.value("COMMAND_GOTO_MENU", COMMAND_GOTO_MENU)	// TAC - Goto Menu - koma13
		.value("COMMAND_IGNORE_DANGER", COMMAND_IGNORE_DANGER)	// TAC - Trade Routes Advisor - koma13
		.value("COMMAND_SAIL_TO_AFRICA", COMMAND_SAIL_TO_AFRICA) /*** TRIANGLETRADE 10/15/08 by DPII ***/
		.value("COMMAND_SAIL_TO_PORT_ROYAL", COMMAND_SAIL_TO_PORT_ROYAL) // R&R, ray, Port Royal
		.value("COMMAND_MERGE_TREASURES", COMMAND_MERGE_TREASURES) // WTP, merge Treasures, of Raubwuerger
		.value("COMMAND_ESTABLISH_TRADE_POST", COMMAND_ESTABLISH_TRADE_POST) // WTP, ray, Native Trade Posts - START
		.value("COMMAND_USE_CONSTRUCTION_SUPPLIES", COMMAND_USE_CONSTRUCTION_SUPPLIES) // WTP, ray, Construction Supplies - START
		.value("NUM_COMMAND_TYPES", NUM_COMMAND_TYPES)
		;
	python::enum_<ControlTypes>("ControlTypes")
		.value("NO_CONTROL", NO_CONTROL)
		.value("CONTROL_CENTERONSELECTION", CONTROL_CENTERONSELECTION)
		.value("CONTROL_SELECTYUNITTYPE", CONTROL_SELECTYUNITTYPE)
		.value("CONTROL_SELECTYUNITALL", CONTROL_SELECTYUNITALL)
		.value("CONTROL_SELECTCITY", CONTROL_SELECTCITY)
		.value("CONTROL_SELECTCAPITAL", CONTROL_SELECTCAPITAL)
		.value("CONTROL_NEXTCITY", CONTROL_NEXTCITY)
		.value("CONTROL_PREVCITY", CONTROL_PREVCITY)
		.value("CONTROL_NEXTUNIT", CONTROL_NEXTUNIT)
		.value("CONTROL_PREVUNIT", CONTROL_PREVUNIT)
		.value("CONTROL_CYCLEUNIT", CONTROL_CYCLEUNIT)
		.value("CONTROL_CYCLEUNIT_ALT", CONTROL_CYCLEUNIT_ALT)
		.value("CONTROL_LASTUNIT", CONTROL_LASTUNIT)
		.value("CONTROL_ENDTURN", CONTROL_ENDTURN)
		.value("CONTROL_ENDTURN_ALT", CONTROL_ENDTURN_ALT)
		.value("CONTROL_FORCEENDTURN", CONTROL_FORCEENDTURN)
		.value("CONTROL_AUTOMOVES", CONTROL_AUTOMOVES)
		.value("CONTROL_PING", CONTROL_PING)
		.value("CONTROL_SIGN", CONTROL_SIGN)
		.value("CONTROL_GRID", CONTROL_GRID)
		.value("CONTROL_BARE_MAP", CONTROL_BARE_MAP)
		.value("CONTROL_YIELDS", CONTROL_YIELDS)
		.value("CONTROL_RESOURCE_ALL", CONTROL_RESOURCE_ALL)
		.value("CONTROL_UNIT_ICONS", CONTROL_UNIT_ICONS)
		.value("CONTROL_GLOBELAYER", CONTROL_GLOBELAYER)
		.value("CONTROL_SCORES", CONTROL_SCORES)
		.value("CONTROL_LOAD_GAME", CONTROL_LOAD_GAME)
		.value("CONTROL_OPTIONS_SCREEN", CONTROL_OPTIONS_SCREEN)
		.value("CONTROL_RETIRE", CONTROL_RETIRE)
		.value("CONTROL_SAVE_GROUP", CONTROL_SAVE_GROUP)
		.value("CONTROL_SAVE_NORMAL", CONTROL_SAVE_NORMAL)
		.value("CONTROL_QUICK_SAVE", CONTROL_QUICK_SAVE)
		.value("CONTROL_QUICK_LOAD", CONTROL_QUICK_LOAD)
		.value("CONTROL_ORTHO_CAMERA", CONTROL_ORTHO_CAMERA)
		.value("CONTROL_CYCLE_CAMERA_FLYING_MODES", CONTROL_CYCLE_CAMERA_FLYING_MODES)
		.value("CONTROL_ISOMETRIC_CAMERA_LEFT", CONTROL_ISOMETRIC_CAMERA_LEFT)
		.value("CONTROL_ISOMETRIC_CAMERA_RIGHT", CONTROL_ISOMETRIC_CAMERA_RIGHT)
		.value("CONTROL_FLYING_CAMERA", CONTROL_FLYING_CAMERA)
		.value("CONTROL_MOUSE_FLYING_CAMERA", CONTROL_MOUSE_FLYING_CAMERA)
		.value("CONTROL_TOP_DOWN_CAMERA", CONTROL_TOP_DOWN_CAMERA)
		.value("CONTROL_CIVILOPEDIA", CONTROL_CIVILOPEDIA)
		.value("CONTROL_FOREIGN_SCREEN", CONTROL_FOREIGN_SCREEN)
		.value("CONTROL_CONGRESS_SCREEN", CONTROL_CONGRESS_SCREEN)
		.value("CONTROL_REVOLUTION_SCREEN", CONTROL_REVOLUTION_SCREEN)
		.value("CONTROL_EUROPE_SCREEN", CONTROL_EUROPE_SCREEN)
		.value("CONTROL_MILITARY_SCREEN", CONTROL_MILITARY_SCREEN)
		.value("CONTROL_FATHER_SCREEN", CONTROL_FATHER_SCREEN)
		.value("CONTROL_TURN_LOG", CONTROL_TURN_LOG)
		.value("CONTROL_CHAT_ALL", CONTROL_CHAT_ALL)
		.value("CONTROL_CHAT_TEAM", CONTROL_CHAT_TEAM)
		.value("CONTROL_DOMESTIC_SCREEN", CONTROL_DOMESTIC_SCREEN)
		.value("CONTROL_VICTORY_SCREEN", CONTROL_VICTORY_SCREEN)
		.value("CONTROL_INFO", CONTROL_INFO)
		.value("CONTROL_GLOBE_VIEW", CONTROL_GLOBE_VIEW)
		.value("CONTROL_DETAILS", CONTROL_DETAILS)
		.value("CONTROL_ADMIN_DETAILS", CONTROL_ADMIN_DETAILS)
		.value("CONTROL_HALL_OF_FAME", CONTROL_HALL_OF_FAME)
		.value("CONTROL_WORLD_BUILDER", CONTROL_WORLD_BUILDER)
		.value("CONTROL_DIPLOMACY", CONTROL_DIPLOMACY)
		.value("CONTROL_SELECT_HEALTHY", CONTROL_SELECT_HEALTHY)
		.value("CONTROL_FREE_COLONY", CONTROL_FREE_COLONY)
		.value("CONTROL_AFRICA_SCREEN", CONTROL_AFRICA_SCREEN) /*** TRIANGLETRADE 10/15/08 by DPII ***/
		.value("CONTROL_PORT_ROYAL_SCREEN", CONTROL_PORT_ROYAL_SCREEN) // R&R, ray, Port Royal
		.value("NUM_CONTROL_TYPES", NUM_CONTROL_TYPES)
		;
	python::enum_<PromotionTypes>("PromotionTypes")
		.value("NO_PROMOTION", NO_PROMOTION)
		;
	python::enum_<CultureLevelTypes>("CultureLevelTypes")
		.value("NO_CULTURELEVEL", NO_CULTURELEVEL)
		;
	python::enum_<CivicOptionTypes>("CivicOptionTypes")
		.value("NO_CIVICOPTION", NO_CIVICOPTION)
		;
	python::enum_<CivicTypes>("CivicTypes")
		.value("NO_CIVIC", NO_CIVIC)
		;
	python::enum_<WarPlanTypes>("WarPlanTypes")
		.value("NO_WARPLAN", NO_WARPLAN)
		.value("WARPLAN_ATTACKED_RECENT", WARPLAN_ATTACKED_RECENT)
		.value("WARPLAN_ATTACKED", WARPLAN_ATTACKED)
		.value("WARPLAN_PREPARING_LIMITED", WARPLAN_PREPARING_LIMITED)
		.value("WARPLAN_PREPARING_TOTAL", WARPLAN_PREPARING_TOTAL)
		.value("WARPLAN_LIMITED", WARPLAN_LIMITED)
		.value("WARPLAN_TOTAL", WARPLAN_TOTAL)
		.value("WARPLAN_DOGPILE", WARPLAN_DOGPILE)
		.value("WARPLAN_EXTORTION", WARPLAN_EXTORTION)
		.value("NUM_WARPLAN_TYPES", NUM_WARPLAN_TYPES)
		;
	python::enum_<AreaAITypes>("AreaAITypes")
		.value("NO_AREAAI", NO_AREAAI)
		.value("AREAAI_OFFENSIVE", AREAAI_OFFENSIVE)
		.value("AREAAI_BALANCED", AREAAI_BALANCED)
		.value("AREAAI_DEFENSIVE", AREAAI_DEFENSIVE)
		.value("AREAAI_MASSING", AREAAI_MASSING)
		.value("AREAAI_ASSAULT", AREAAI_ASSAULT)
		.value("AREAAI_ASSAULT_MASSING", AREAAI_ASSAULT_MASSING)
		.value("AREAAI_NEUTRAL", AREAAI_NEUTRAL)
		.value("NUM_AREAAI_TYPES", NUM_AREAAI_TYPES)
		;
	python::enum_<EmotionTypes>("EmotionTypes")
		.value("NO_EMOTION", NO_EMOTION)
		.value("EMOTION_GREED", EMOTION_GREED)
		.value("EMOTION_ANGER", EMOTION_ANGER)
		.value("EMOTION_ENVY", EMOTION_ENVY)
		.value("EMOTION_ANXIETY", EMOTION_ANXIETY)
		.value("EMOTION_SUBMISSION", EMOTION_SUBMISSION)
		.value("NUM_EMOTION_TYPES", NUM_EMOTION_TYPES)
		;
	python::enum_<EndTurnButtonStates>("EndTurnButtonStates")
		.value("END_TURN_GO", END_TURN_GO)
		.value("END_TURN_OVER_HIGHLIGHT", END_TURN_OVER_HIGHLIGHT)
		.value("END_TURN_OVER_DARK", END_TURN_OVER_DARK)
		.value("NUM_END_TURN_STATES", NUM_END_TURN_STATES)
		;
	python::enum_<FogOfWarModeTypes>("FogOfWarModeTypes")
		.value("FOGOFWARMODE_OFF", FOGOFWARMODE_OFF)
		.value("FOGOFWARMODE_UNEXPLORED", FOGOFWARMODE_UNEXPLORED)
		.value("NUM_FOGOFWARMODE_TYPES", NUM_FOGOFWARMODE_TYPES)
		;
	python::enum_<AnimationTypes>("AnimationTypes")
		.value("NONE_ANIMATION", NONE_ANIMATION)
		.value("BONUSANIMATION_UNIMPROVED", BONUSANIMATION_UNIMPROVED)
		.value("BONUSANIMATION_NOT_WORKED", BONUSANIMATION_NOT_WORKED)
		.value("BONUSANIMATION_WORKED", BONUSANIMATION_WORKED)
		.value("IMPROVEMENTANIMATION_OFF", IMPROVEMENTANIMATION_OFF)
		.value("IMPROVEMENTANIMATION_ON", IMPROVEMENTANIMATION_ON)
		.value("IMPROVEMENTANIMATION_OFF_EXTRA", IMPROVEMENTANIMATION_OFF_EXTRA)
		.value("IMPROVEMENTANIMATION_ON_EXTRA_1", IMPROVEMENTANIMATION_ON_EXTRA_1)
		.value("IMPROVEMENTANIMATION_ON_EXTRA_2", IMPROVEMENTANIMATION_ON_EXTRA_2)
		.value("IMPROVEMENTANIMATION_ON_EXTRA_3", IMPROVEMENTANIMATION_ON_EXTRA_3)
		.value("IMPROVEMENTANIMATION_ON_EXTRA_4", IMPROVEMENTANIMATION_ON_EXTRA_4)
		;
	python::enum_<EntityEventTypes>("EntityEventTypes")
		.value( "ENTITY_EVENT_NONE", ENTITY_EVENT_NONE )
		;
	python::enum_<AnimationPathTypes>("AnimationPathTypes")
		.value( "ANIMATIONPATH_NONE", ANIMATIONPATH_NONE )
		.value( "ANIMATIONPATH_IDLE", ANIMATIONPATH_IDLE )
		.value( "ANIMATIONPATH_MOVE", ANIMATIONPATH_MOVE )
		.value( "ANIMATIONPATH_RANDOMIZE_ANIMATION_SET", ANIMATIONPATH_RANDOMIZE_ANIMATION_SET )
		.value( "ANIMATIONPATH_MELEE_STRIKE", ANIMATIONPATH_MELEE_STRIKE )
		.value( "ANIMATIONPATH_MELEE_HURT", ANIMATIONPATH_MELEE_HURT )
		.value( "ANIMATIONPATH_MELEE_DIE", ANIMATIONPATH_MELEE_DIE )
		.value( "ANIMATIONPATH_MELEE_FORTIFIED", ANIMATIONPATH_MELEE_FORTIFIED )
		.value( "ANIMATIONPATH_MELEE_DIE_FADE", ANIMATIONPATH_MELEE_DIE_FADE )
		.value( "ANIMATIONPATH_RANGED_STRIKE", ANIMATIONPATH_RANGED_STRIKE )
		.value( "ANIMATIONPATH_RANGED_DIE", ANIMATIONPATH_RANGED_DIE )
		.value( "ANIMATIONPATH_RANGED_FORTIFIED", ANIMATIONPATH_RANGED_FORTIFIED )
		.value( "ANIMATIONPATH_RANGED_RUNHIT", ANIMATIONPATH_RANGED_RUNHIT )
		.value( "ANIMATIONPATH_RANGED_RUNDIE", ANIMATIONPATH_RANGED_RUNDIE )
		.value( "ANIMATIONPATH_RANGED_DIE_FADE", ANIMATIONPATH_RANGED_DIE_FADE )
		.value( "ANIMATIONPATH_LEADER_COMMAND", ANIMATIONPATH_LEADER_COMMAND )
		;
	python::enum_<AnimationCategoryTypes>("AnimationCategoryTypes")
		.value("ANIMCAT_NONE", ANIMCAT_NONE)
		;
	python::enum_<CursorTypes>("CursorTypes")
		.value("NO_CURSOR", NO_CURSOR)
		;
	python::enum_<TradeableItems>("TradeableItems")
		.value("NO_TRADEABLE_ITEMS", TRADE_ITEM_NONE)
		.value("TRADE_GOLD", TRADE_GOLD)
		.value("TRADE_MAPS", TRADE_MAPS)
		.value("TRADE_OPEN_BORDERS", TRADE_OPEN_BORDERS)
		.value("TRADE_DEFENSIVE_PACT", TRADE_DEFENSIVE_PACT)
		.value("TRADE_PERMANENT_ALLIANCE", TRADE_PERMANENT_ALLIANCE)
		.value("TRADE_PEACE_TREATY", TRADE_PEACE_TREATY)
		.value("NUM_BASIC_ITEMS", NUM_BASIC_ITEMS)
		.value("TRADE_YIELD", TRADE_YIELD)
		.value("TRADE_CITIES", TRADE_CITIES)
		.value("TRADE_PEACE", TRADE_PEACE)
		.value("TRADE_WAR", TRADE_WAR)
		.value("TRADE_EMBARGO", TRADE_EMBARGO)
		.value("NUM_TRADEABLE_HEADINGS", NUM_TRADEABLE_HEADINGS)
		.value("NUM_TRADEABLE_ITEMS", NUM_TRADEABLE_ITEMS)
		;
	python::enum_<DiploEventTypes>("DiploEventTypes")
		.value("NO_DIPLOEVENT", NO_DIPLOEVENT)
		.value("DIPLOEVENT_CONTACT", DIPLOEVENT_CONTACT)
		.value("DIPLOEVENT_AI_CONTACT", DIPLOEVENT_AI_CONTACT)
		.value("DIPLOEVENT_FAILED_CONTACT", DIPLOEVENT_FAILED_CONTACT)
		.value("DIPLOEVENT_GIVE_HELP", DIPLOEVENT_GIVE_HELP)
		.value("DIPLOEVENT_REFUSED_HELP", DIPLOEVENT_REFUSED_HELP)
		.value("DIPLOEVENT_ACCEPT_DEMAND", DIPLOEVENT_ACCEPT_DEMAND)
		.value("DIPLOEVENT_REJECTED_DEMAND", DIPLOEVENT_REJECTED_DEMAND)
		.value("DIPLOEVENT_DEMAND_WAR", DIPLOEVENT_DEMAND_WAR)
		.value("DIPLOEVENT_JOIN_WAR", DIPLOEVENT_JOIN_WAR)
		.value("DIPLOEVENT_NO_JOIN_WAR", DIPLOEVENT_NO_JOIN_WAR)
		.value("DIPLOEVENT_STOP_TRADING", DIPLOEVENT_STOP_TRADING)
		.value("DIPLOEVENT_NO_STOP_TRADING", DIPLOEVENT_NO_STOP_TRADING)
		.value("DIPLOEVENT_ASK_HELP", DIPLOEVENT_ASK_HELP)
		.value("DIPLOEVENT_MADE_DEMAND", DIPLOEVENT_MADE_DEMAND)
		.value("DIPLOEVENT_TARGET_CITY", DIPLOEVENT_TARGET_CITY)
		.value("DIPLOEVENT_ACCEPT_TAX_RATE", DIPLOEVENT_ACCEPT_TAX_RATE)
		.value("DIPLOEVENT_REFUSE_TAX_RATE", DIPLOEVENT_REFUSE_TAX_RATE)
		.value("DIPLOEVENT_ACCEPT_KING_GOLD", DIPLOEVENT_ACCEPT_KING_GOLD)
		.value("DIPLOEVENT_REFUSE_KING_GOLD", DIPLOEVENT_REFUSE_KING_GOLD)
		.value("DIPLOEVENT_LIVE_AMONG_NATIVES", DIPLOEVENT_LIVE_AMONG_NATIVES)
		.value("DIPLOEVENT_TRANSPORT_TREASURE", DIPLOEVENT_TRANSPORT_TREASURE)
		.value("DIPLOEVENT_FOUND_CITY", DIPLOEVENT_FOUND_CITY)
		.value("DIPLOEVENT_ACQUIRE_CITY", DIPLOEVENT_ACQUIRE_CITY) //ray18
		.value("DIPLOEVENT_ACQUIRE_MERC", DIPLOEVENT_ACQUIRE_MERC) //TAC Native Mercs
		.value("DIPLOEVENT_ACQUIRE_NATIVE_SLAVE", DIPLOEVENT_ACQUIRE_NATIVE_SLAVE) // R&R, ray, Native Slave
		.value("DIPLOEVENT_ACQUIRE_AFRICAN_SLAVES", DIPLOEVENT_ACQUIRE_AFRICAN_SLAVES) // R&R, ray, African Slaves
		.value("DIPLOEVENT_ACQUIRE_PRISONERS", DIPLOEVENT_ACQUIRE_PRISONERS) // R&R, ray, Prisons Crowded
		.value("DIPLOEVENT_ACQUIRE_NOBLE", DIPLOEVENT_ACQUIRE_NOBLE) // R&R, ray, Revolutionary Noble
		.value("DIPLOEVENT_ACQUIRE_BISHOP", DIPLOEVENT_ACQUIRE_BISHOP) // R&R, ray, Bishop
		.value("DIPLOEVENT_PAY_CHURCH_DEMAND", DIPLOEVENT_PAY_CHURCH_DEMAND) // R&R, ray, the Church
		.value("DIPLOEVENT_REFUSE_CHURCH_DEMAND", DIPLOEVENT_REFUSE_CHURCH_DEMAND) // R&R, ray, the Church
		.value("DIPLOEVENT_CHURCH_FAVOUR", DIPLOEVENT_CHURCH_FAVOUR) // R&R, ray, Church Favours
		.value("DIPLOEVENT_CHURCH_WAR", DIPLOEVENT_CHURCH_WAR) // R&R, ray, Church War
		.value("DIPLOEVENT_COLONIAL_INTERVENTION_NATIVE_WAR", DIPLOEVENT_COLONIAL_INTERVENTION_NATIVE_WAR) //WTP, ray, Colonial Intervention In Native War - START
		.value("DIPLOEVENT_COLONIES_AND_NATIVE_ALLIES_WAR_REFUSE", DIPLOEVENT_COLONIES_AND_NATIVE_ALLIES_WAR_REFUSE) // WTP, ray, Big Colonies and Native Allies War - START
		.value("DIPLOEVENT_COLONIES_AND_NATIVE_ALLIES_WAR_ACCEPT", DIPLOEVENT_COLONIES_AND_NATIVE_ALLIES_WAR_ACCEPT) // WTP, ray, Big Colonies and Native Allies War - START
		.value("DIPLOEVENT_ACQUIRE_SMUGGLERS", DIPLOEVENT_ACQUIRE_SMUGGLERS) // R&R, ray, Smuggling
		.value("DIPLOEVENT_ACQUIRE_RANGERS", DIPLOEVENT_ACQUIRE_RANGERS) // R&R, ray, Rangers
		.value("DIPLOEVENT_ACQUIRE_CONQUISTADORS", DIPLOEVENT_ACQUIRE_CONQUISTADORS) // // R&R, ray, Conquistadors
		.value("DIPLOEVENT_ACQUIRE_PIRATES", DIPLOEVENT_ACQUIRE_PIRATES) // R&R, ray, Pirates
		.value("DIPLOEVENT_ACQUIRE_USED_SHIPS", DIPLOEVENT_ACQUIRE_USED_SHIPS) //WTP, ray Kings Used Ship - START
		.value("DIPLOEVENT_ACQUIRE_FOREIGN_IMMIGRANTS", DIPLOEVENT_ACQUIRE_FOREIGN_IMMIGRANTS) // WTP, ray, Foreign Kings, buy Immigrants - START
		.value("DIPLOEVENT_BRIBE_PIRATES", DIPLOEVENT_BRIBE_PIRATES) // R&R, ray, Pirates
		.value("DIPLOEVENT_CREATE_ENEMY_PIRATES", DIPLOEVENT_CREATE_ENEMY_PIRATES) // R&R, ray, Pirates
		.value("DIPLOEVENT_ACQUIRE_CONTINENTAL_GUARD", DIPLOEVENT_ACQUIRE_CONTINENTAL_GUARD) // R&R, ray, Continental Guard
		.value("DIPLOEVENT_ACQUIRE_MORTAR", DIPLOEVENT_ACQUIRE_MORTAR) // R&R, ray, Mortar
		.value("DIPLOEVENT_REV_SUPPORT", DIPLOEVENT_REV_SUPPORT) //TAC Revolution Support
		.value("DIPLOEVENT_EUROPE_WAR", DIPLOEVENT_EUROPE_WAR) //TAC European Wars
		.value("DIPLOEVENT_STEALING_IMMIGRANT", DIPLOEVENT_STEALING_IMMIGRANT) // R&R, Stealing Immigrant
		.value("DIPLOEVENT_EUROPE_PEACE", DIPLOEVENT_EUROPE_PEACE) // R&R, ray, European Peace
		.value("DIPLOEVENT_ROYAL_INTERVENTION", DIPLOEVENT_ROYAL_INTERVENTION) // WTP, ray, Royal Intervention, START
		.value("DIPLOEVENT_PRIVATEERS_ACCUSATION", DIPLOEVENT_PRIVATEERS_ACCUSATION) // WTP, ray, Privateers DLL Diplo Event - START
		.value("DIPLOEVENT_NATIVE_TRADE", DIPLOEVENT_NATIVE_TRADE) // R&R, ray, Natives Trading
		.value("DIPLOEVENT_FOUND_CITY_CHECK_NATIVES", DIPLOEVENT_FOUND_CITY_CHECK_NATIVES)
		.value("NUM_DIPLOEVENT_TYPES", NUM_DIPLOEVENT_TYPES)
		;
	python::enum_<DiploCommentTypes>("DiploCommentTypes")
		.value("NO_DIPLOCOMMENT", NO_DIPLOCOMMENT)
		;
	python::enum_<NetContactTypes>("NetContactTypes")
		.value("NO_NETCONTACT", NO_NETCONTACT)
		.value("NETCONTACT_INITIAL", NETCONTACT_INITIAL)
		.value("NETCONTACT_RESPONSE", NETCONTACT_RESPONSE)
		.value("NETCONTACT_ESTABLISHED", NETCONTACT_ESTABLISHED)
		.value("NETCONTACT_BUSY", NETCONTACT_BUSY)
		.value("NUM_NETCONTACT_TYPES", NUM_NETCONTACT_TYPES)
		;
	python::enum_<ContactTypes>("ContactTypes")
		.value("CONTACT_JOIN_WAR", CONTACT_JOIN_WAR)
		.value("CONTACT_STOP_TRADING", CONTACT_STOP_TRADING)
		.value("CONTACT_GIVE_HELP", CONTACT_GIVE_HELP)
		.value("CONTACT_ASK_FOR_HELP", CONTACT_ASK_FOR_HELP)
		.value("CONTACT_DEMAND_TRIBUTE", CONTACT_DEMAND_TRIBUTE)
		.value("CONTACT_OPEN_BORDERS", CONTACT_OPEN_BORDERS)
		.value("CONTACT_DEFENSIVE_PACT", CONTACT_DEFENSIVE_PACT)
		.value("CONTACT_PERMANENT_ALLIANCE", CONTACT_PERMANENT_ALLIANCE)
		.value("CONTACT_PEACE_TREATY", CONTACT_PEACE_TREATY)
		.value("CONTACT_TRADE_MAP", CONTACT_TRADE_MAP)
		.value("CONTACT_YIELD_GIFT", CONTACT_YIELD_GIFT)
		// R&R, ray, Natives raiding party - START
		.value("CONTACT_NATIVE_RAID", CONTACT_NATIVE_RAID)
		// R&R, ray, Natives raiding party - END
		.value("NUM_CONTACT_TYPES", NUM_CONTACT_TYPES)
		;
	python::enum_<MemoryTypes>("MemoryTypes")
		.value("MEMORY_DECLARED_WAR", MEMORY_DECLARED_WAR)
		.value("MEMORY_DECLARED_WAR_ON_FRIEND", MEMORY_DECLARED_WAR_ON_FRIEND)
		.value("MEMORY_HIRED_WAR_ALLY", MEMORY_HIRED_WAR_ALLY)
		.value("MEMORY_RAZED_CITY", MEMORY_RAZED_CITY)
		.value("MEMORY_GIVE_HELP", MEMORY_GIVE_HELP)
		.value("MEMORY_REFUSED_HELP", MEMORY_REFUSED_HELP)
		.value("MEMORY_ACCEPT_DEMAND", MEMORY_ACCEPT_DEMAND)
		.value("MEMORY_REJECTED_DEMAND", MEMORY_REJECTED_DEMAND)
		.value("MEMORY_ACCEPTED_JOIN_WAR", MEMORY_ACCEPTED_JOIN_WAR)
		.value("MEMORY_DENIED_JOIN_WAR", MEMORY_DENIED_JOIN_WAR)
		.value("MEMORY_ACCEPTED_STOP_TRADING", MEMORY_ACCEPTED_STOP_TRADING)
		.value("MEMORY_DENIED_STOP_TRADING", MEMORY_DENIED_STOP_TRADING)
		.value("MEMORY_STOPPED_TRADING", MEMORY_STOPPED_TRADING)
		.value("MEMORY_STOPPED_TRADING_RECENT", MEMORY_STOPPED_TRADING_RECENT)
		.value("MEMORY_HIRED_TRADE_EMBARGO", MEMORY_HIRED_TRADE_EMBARGO)
		.value("MEMORY_MADE_DEMAND", MEMORY_MADE_DEMAND)
		.value("MEMORY_MADE_DEMAND_RECENT", MEMORY_MADE_DEMAND_RECENT)
		.value("MEMORY_CANCELLED_OPEN_BORDERS", MEMORY_CANCELLED_OPEN_BORDERS)
		.value("MEMORY_EVENT_GOOD_TO_US", MEMORY_EVENT_GOOD_TO_US)
		.value("MEMORY_EVENT_BAD_TO_US", MEMORY_EVENT_BAD_TO_US)
		.value("MEMORY_LIBERATED_CITIES", MEMORY_LIBERATED_CITIES)
		.value("MEMORY_MISSIONARY_FAIL", MEMORY_MISSIONARY_FAIL)
		.value("NUM_MEMORY_TYPES", NUM_MEMORY_TYPES)
		;
	python::enum_<AttitudeTypes>("AttitudeTypes")
		.value("NO_ATTITUDE", NO_ATTITUDE)
		.value("ATTITUDE_FURIOUS", ATTITUDE_FURIOUS)
		.value("ATTITUDE_ANNOYED", ATTITUDE_ANNOYED)
		.value("ATTITUDE_CAUTIOUS", ATTITUDE_CAUTIOUS)
		.value("ATTITUDE_PLEASED", ATTITUDE_PLEASED)
		.value("ATTITUDE_FRIENDLY", ATTITUDE_FRIENDLY)
		.value("NUM_ATTITUDE_TYPES", NUM_ATTITUDE_TYPES)
		;
	python::enum_<LeaderheadAction>("LeaderheadAction")
		.value( "NO_LEADERANIM", NO_LEADERANIM )
		.value( "LEADERANIM_GREETING", LEADERANIM_GREETING )
		.value( "LEADERANIM_FRIENDLY", LEADERANIM_FRIENDLY )
		.value( "LEADERANIM_PLEASED", LEADERANIM_PLEASED )
		.value( "LEADERANIM_CAUTIOUS", LEADERANIM_CAUTIOUS )
		.value( "LEADERANIM_ANNOYED", LEADERANIM_ANNOYED )
		.value( "LEADERANIM_FURIOUS", LEADERANIM_FURIOUS )
		.value( "LEADERANIM_DISAGREE", LEADERANIM_DISAGREE )
		.value( "LEADERANIM_AGREE", LEADERANIM_AGREE )
		.value( "LEADERANIM_OFFER_PINKY", LEADERANIM_OFFER_PINKY )
		.value( "LEADERANIM_PINKY_IDLE", LEADERANIM_PINKY_IDLE )
		.value( "LEADERANIM_PINKY_ACCEPT", LEADERANIM_PINKY_ACCEPT )
		.value( "LEADERANIM_PINKY_REJECT", LEADERANIM_PINKY_REJECT )
		.value( "NUM_LEADERANIM_TYPES", NUM_LEADERANIM_TYPES )
		;
	python::enum_<DiplomacyPowerTypes>("DiplomacyPowerTypes")
		.value("NO_DIPLOMACYPOWER", NO_DIPLOMACYPOWER)
		.value("DIPLOMACYPOWER_WEAKER", DIPLOMACYPOWER_WEAKER)
		.value("DIPLOMACYPOWER_EQUAL", DIPLOMACYPOWER_EQUAL)
		.value("DIPLOMACYPOWER_STRONGER", DIPLOMACYPOWER_STRONGER)
		.value("NUM_DIPLOMACYPOWER_TYPES", NUM_DIPLOMACYPOWER_TYPES)
		;
	python::enum_<FeatTypes>("FeatTypes")
		.value("FEAT_TREASURE", FEAT_TREASURE)
		.value("FEAT_TREASURE_IN_PORT", FEAT_TREASURE_IN_PORT)
		.value("FEAT_CITY_MISSING_YIELD", FEAT_CITY_MISSING_YIELD)
		.value("FEAT_CITY_NO_FOOD", FEAT_CITY_NO_FOOD)
		.value("FEAT_CITY_INLAND", FEAT_CITY_INLAND)
		.value("FEAT_EUROPE_SHIPS", FEAT_EUROPE_SHIPS)
		.value("FEAT_GOTO_EUROPE", FEAT_GOTO_EUROPE)
		.value("FEAT_TALK_NATIVES", FEAT_TALK_NATIVES)
		.value("FEAT_CITY_SCREEN", FEAT_CITY_SCREEN)
		.value("FEAT_PAD", FEAT_PAD)
		.value("NUM_FEAT_TYPES", NUM_FEAT_TYPES)
		;
	python::enum_<SaveGameTypes>("SaveGameTypes")
		.value("SAVEGAME_NONE", SAVEGAME_NONE)
		.value("SAVEGAME_AUTO", SAVEGAME_AUTO)
		.value("SAVEGAME_RECOVERY", SAVEGAME_RECOVERY)
		.value("SAVEGAME_QUICK", SAVEGAME_QUICK)
		.value("SAVEGAME_NORMAL", SAVEGAME_NORMAL)
		.value("SAVEGAME_GROUP", SAVEGAME_GROUP)
		.value("SAVEGAME_DROP_QUIT", SAVEGAME_DROP_QUIT)
		.value("SAVEGAME_DROP_CONTINUE", SAVEGAME_DROP_CONTINUE)
		.value("SAVEGAME_PBEM", SAVEGAME_PBEM)
		.value("SAVEGAME_REPLAY", SAVEGAME_REPLAY)
		.value("NUM_SAVEGAME_TYPES", NUM_SAVEGAME_TYPES)
		;
	python::enum_<GameType>("GameType")
		.value("GAME_NONE", GAME_NONE)
		.value("GAME_SP_NEW", GAME_SP_NEW)
		.value("GAME_SP_SCENARIO", GAME_SP_SCENARIO)
		.value("GAME_SP_LOAD", GAME_SP_LOAD)
		.value("GAME_MP_NEW", GAME_MP_NEW)
		.value("GAME_MP_SCENARIO", GAME_MP_SCENARIO)
		.value("GAME_MP_LOAD", GAME_MP_LOAD)
		.value("GAME_HOTSEAT_NEW", GAME_HOTSEAT_NEW)
		.value("GAME_HOTSEAT_SCENARIO", GAME_HOTSEAT_SCENARIO)
		.value("GAME_HOTSEAT_LOAD", GAME_HOTSEAT_LOAD)
		.value("GAME_PBEM_NEW", GAME_PBEM_NEW)
		.value("GAME_PBEM_SCENARIO", GAME_PBEM_SCENARIO)
		.value("GAME_PBEM_LOAD", GAME_PBEM_LOAD)
		.value("GAME_REPLAY", GAME_REPLAY)
		.value("NUM_GAMETYPES", NUM_GAMETYPES)
		;
	python::enum_<GameMode>("GameMode")
		.value("NO_GAMEMODE", NO_GAMEMODE)
		.value("GAMEMODE_NORMAL", GAMEMODE_NORMAL)
		.value("GAMEMODE_PITBOSS", GAMEMODE_PITBOSS)
		.value("NUM_GAMEMODES", NUM_GAMEMODES)
		;
	python::enum_<InterfaceVisibility>("InterfaceVisibility")
		.value("INTERFACE_SHOW", INTERFACE_SHOW)
		.value("INTERFACE_HIDE", INTERFACE_HIDE)
		.value("INTERFACE_HIDE_ALL", INTERFACE_HIDE_ALL)
		.value("INTERFACE_MINIMAP_ONLY", INTERFACE_MINIMAP_ONLY)
		.value("INTERFACE_ADVANCED_START", INTERFACE_ADVANCED_START)
		;
	python::enum_<GenericButtonSizes>("GenericButtonSizes")
		.value("BUTTON_SIZE_46", BUTTON_SIZE_46)
		.value("BUTTON_SIZE_32", BUTTON_SIZE_32)
		.value("BUTTON_SIZE_24", BUTTON_SIZE_24)
		.value("BUTTON_SIZE_16", BUTTON_SIZE_16)
		.value("BUTTON_SIZE_CUSTOM", BUTTON_SIZE_CUSTOM)
		;
	python::enum_<WorldBuilderPopupTypes>("WorldBuilderPopupTypes")
		.value("WBPOPUP_NONE", WBPOPUP_NONE)
		.value("WBPOPUP_START", WBPOPUP_START)
		.value("WBPOPUP_CITY", WBPOPUP_CITY)
		.value("WBPOPUP_UNIT", WBPOPUP_UNIT)
		.value("WBPOPUP_PLAYER", WBPOPUP_PLAYER)
		.value("WBPOPUP_PLOT", WBPOPUP_PLOT)
		.value("WBPOPUP_TERRAIN", WBPOPUP_TERRAIN)
		.value("WBPOPUP_FEATURE", WBPOPUP_FEATURE)
		.value("WBPOPUP_IMPROVEMENT", WBPOPUP_IMPROVEMENT)
		.value("WBPOPUP_GAME", WBPOPUP_GAME)
		.value("NUM_WBPOPUP", NUM_WBPOPUP)
		;
	python::enum_<EventType>("EventType")
		.value("EVT_LBUTTONDOWN", EVT_LBUTTONDOWN)
		.value("EVT_LBUTTONDBLCLICK", EVT_LBUTTONDBLCLICK)
		.value("EVT_RBUTTONDOWN", EVT_RBUTTONDOWN)
		.value("EVT_BACK", EVT_BACK)
		.value("EVT_FORWARD", EVT_FORWARD)
		.value("EVT_KEYDOWN", EVT_KEYDOWN)
		.value("EVT_KEYUP", EVT_KEYUP)
		;
	python::enum_<LoadType>("LoadType")
		.value("LOAD_NORMAL", LOAD_NORMAL)
		.value("LOAD_INIT", LOAD_INIT)
		.value("LOAD_SETUP", LOAD_SETUP)
		.value("LOAD_GAMETYPE", LOAD_GAMETYPE)
		.value("LOAD_REPLAY", LOAD_REPLAY)
		;
	python::enum_<FontTypes>("FontTypes")
		.value("TITLE_FONT", TITLE_FONT)
		.value("GAME_FONT", GAME_FONT)
		.value("SMALL_FONT", SMALL_FONT)
		.value("MENU_FONT", MENU_FONT)
		.value("MENU_HIGHLIGHT_FONT", MENU_HIGHLIGHT_FONT)
		;
	python::enum_<PanelStyles>("PanelStyles")
		.value("PANEL_STYLE_STANDARD",PANEL_STYLE_STANDARD)
		.value("PANEL_STYLE_SOLID",PANEL_STYLE_SOLID)
		.value("PANEL_STYLE_EMPTY",PANEL_STYLE_EMPTY)
		.value("PANEL_STYLE_FLAT",PANEL_STYLE_FLAT)
		.value("PANEL_STYLE_IN",PANEL_STYLE_IN)
		.value("PANEL_STYLE_OUT",PANEL_STYLE_OUT)
		.value("PANEL_STYLE_EXTERNAL",PANEL_STYLE_EXTERNAL)
		.value("PANEL_STYLE_DEFAULT",PANEL_STYLE_DEFAULT)
		.value("PANEL_STYLE_CIVILPEDIA",PANEL_STYLE_CIVILPEDIA)
		.value("PANEL_STYLE_STONE",PANEL_STYLE_STONE)
		.value("PANEL_STYLE_BLUELARGE",PANEL_STYLE_BLUELARGE)
		.value("PANEL_STYLE_UNITSTAT",PANEL_STYLE_UNITSTAT)
		.value("PANEL_STYLE_BLUE50",PANEL_STYLE_BLUE50)
		.value("PANEL_STYLE_TOPBAR",PANEL_STYLE_TOPBAR)
		.value("PANEL_STYLE_BOTTOMBAR",PANEL_STYLE_BOTTOMBAR)
		.value("PANEL_STYLE_TECH",PANEL_STYLE_TECH)
		.value("PANEL_STYLE_GAMEHUD_LEFT",PANEL_STYLE_GAMEHUD_LEFT)
		.value("PANEL_STYLE_GAMEHUD_RIGHT",PANEL_STYLE_GAMEHUD_RIGHT)
		.value("PANEL_STYLE_GAMEHUD_CENTER",PANEL_STYLE_GAMEHUD_CENTER)
		.value("PANEL_STYLE_GAMEHUD_STATS",PANEL_STYLE_GAMEHUD_STATS)
		.value("PANEL_STYLE_GAME_MAP",PANEL_STYLE_GAME_MAP)
		.value("PANEL_STYLE_GAME_TOPBAR",PANEL_STYLE_GAME_TOPBAR)
		.value("PANEL_STYLE_HUD_HELP", PANEL_STYLE_HUD_HELP)
		.value("PANEL_STYLE_CITY_LEFT",PANEL_STYLE_CITY_LEFT)
		.value("PANEL_STYLE_CITY_RIGHT",PANEL_STYLE_CITY_RIGHT)
		.value("PANEL_STYLE_CITY_TOP",PANEL_STYLE_CITY_TOP)
		.value("PANEL_STYLE_CITY_TANSHADE",PANEL_STYLE_CITY_TANSHADE)
		.value("PANEL_STYLE_CITY_INFO",PANEL_STYLE_CITY_INFO)
		.value("PANEL_STYLE_CITY_TANTL",PANEL_STYLE_CITY_TANTL)
		.value("PANEL_STYLE_CITY_TANTR",PANEL_STYLE_CITY_TANTR)
		.value("PANEL_STYLE_CITY_COLUMNL",PANEL_STYLE_CITY_COLUMNL)
		.value("PANEL_STYLE_CITY_COLUMNC",PANEL_STYLE_CITY_COLUMNC)
		.value("PANEL_STYLE_CITY_COLUMNR",PANEL_STYLE_CITY_COLUMNR)
		.value("PANEL_STYLE_CITY_TITLE",PANEL_STYLE_CITY_TITLE)
		.value("PANEL_STYLE_DAWN",PANEL_STYLE_DAWN)
		.value("PANEL_STYLE_DAWNTOP",PANEL_STYLE_DAWNTOP)
		.value("PANEL_STYLE_DAWNBOTTOM",PANEL_STYLE_DAWNBOTTOM)
		.value("PANEL_STYLE_MAIN",PANEL_STYLE_MAIN)
		.value("PANEL_STYLE_MAIN_BLACK25",PANEL_STYLE_MAIN_BLACK25)
		.value("PANEL_STYLE_MAIN_BLACK50",PANEL_STYLE_MAIN_BLACK50)
		.value("PANEL_STYLE_MAIN_WHITE",PANEL_STYLE_MAIN_WHITE)
		.value("PANEL_STYLE_MAIN_WHITETAB",PANEL_STYLE_MAIN_WHITETAB)
		.value("PANEL_STYLE_MAIN_TAN",PANEL_STYLE_MAIN_TAN)
		.value("PANEL_STYLE_MAIN_TAN15",PANEL_STYLE_MAIN_TAN15)
		.value("PANEL_STYLE_MAIN_TANL",PANEL_STYLE_MAIN_TANL)
		.value("PANEL_STYLE_MAIN_TANR",PANEL_STYLE_MAIN_TANR)
		.value("PANEL_STYLE_MAIN_TANT",PANEL_STYLE_MAIN_TANT)
		.value("PANEL_STYLE_MAIN_TANB",PANEL_STYLE_MAIN_TANB)
		.value("PANEL_STYLE_MAIN_BOTTOMBAR",PANEL_STYLE_MAIN_BOTTOMBAR)
		.value("PANEL_STYLE_MAIN_SELECT",PANEL_STYLE_MAIN_SELECT)
		;
	python::enum_<ButtonStyles>("ButtonStyles")
		.value("BUTTON_STYLE_STANDARD", BUTTON_STYLE_STANDARD)
		.value("BUTTON_STYLE_ETCHED", BUTTON_STYLE_ETCHED)
		.value("BUTTON_STYLE_FLAT", BUTTON_STYLE_FLAT)
		.value("BUTTON_STYLE_IMAGE", BUTTON_STYLE_IMAGE)
		.value("BUTTON_STYLE_LABEL", BUTTON_STYLE_LABEL)
		.value("BUTTON_STYLE_LINK", BUTTON_STYLE_LINK)
		.value("BUTTON_STYLE_SQUARE", BUTTON_STYLE_SQUARE)
		.value("BUTTON_STYLE_TOOL", BUTTON_STYLE_TOOL)
		.value("BUTTON_STYLE_DEFAULT", BUTTON_STYLE_DEFAULT)
		.value("BUTTON_STYLE_CIRCLE", BUTTON_STYLE_CIRCLE)
		.value("BUTTON_STYLE_CITY_B01", BUTTON_STYLE_CITY_B01)
		.value("BUTTON_STYLE_CITY_B02TL", BUTTON_STYLE_CITY_B02TL)
		.value("BUTTON_STYLE_CITY_B02TR", BUTTON_STYLE_CITY_B02TR)
		.value("BUTTON_STYLE_CITY_B02BL", BUTTON_STYLE_CITY_B02BL)
		.value("BUTTON_STYLE_CITY_B02BR", BUTTON_STYLE_CITY_B02BR)
		.value("BUTTON_STYLE_CITY_B03TL", BUTTON_STYLE_CITY_B03TL)
		.value("BUTTON_STYLE_CITY_B03TC", BUTTON_STYLE_CITY_B03TC)
		.value("BUTTON_STYLE_CITY_B03TR", BUTTON_STYLE_CITY_B03TR)
		.value("BUTTON_STYLE_CITY_B03BL", BUTTON_STYLE_CITY_B03BL)
		.value("BUTTON_STYLE_CITY_B03BC", BUTTON_STYLE_CITY_B03BC)
		.value("BUTTON_STYLE_CITY_B03BR", BUTTON_STYLE_CITY_B03BR)
		.value("BUTTON_STYLE_CITY_FLAT", BUTTON_STYLE_CITY_FLAT)
		.value("BUTTON_STYLE_CITY_PLUS", BUTTON_STYLE_CITY_PLUS)
		.value("BUTTON_STYLE_CITY_MINUS", BUTTON_STYLE_CITY_MINUS)
		.value("BUTTON_STYLE_ARROW_LEFT", BUTTON_STYLE_ARROW_LEFT)
		.value("BUTTON_STYLE_ARROW_RIGHT", BUTTON_STYLE_ARROW_RIGHT)
		.value("BUTTON_STYLE_ARROW_UP", BUTTON_STYLE_ARROW_UP)
		.value("BUTTON_STYLE_ARROW_DOWN", BUTTON_STYLE_ARROW_DOWN)
		;
	python::enum_<ImageShapes>("ImageShapes")
		.value("IMAGE_SHAPE_RECTANGLE", IMAGE_SHAPE_RECTANGLE)
		.value("IMAGE_SHAPE_ELLIPSE", IMAGE_SHAPE_ELLIPSE)
		.value("IMAGE_SHAPE_PARABOLA_TOP", IMAGE_SHAPE_PARABOLA_TOP)
		;
	python::enum_<TableStyles>("TableStyles")
		.value("TABLE_STYLE_STANDARD", TABLE_STYLE_STANDARD)
		.value("TABLE_STYLE_EMPTY", TABLE_STYLE_EMPTY)
		.value("TABLE_STYLE_ALTEMPTY", TABLE_STYLE_ALTEMPTY)
		.value("TABLE_STYLE_CITY", TABLE_STYLE_CITY)
		.value("TABLE_STYLE_EMPTYSELECTINACTIVE", TABLE_STYLE_EMPTYSELECTINACTIVE)
		.value("TABLE_STYLE_ALTDEFAULT", TABLE_STYLE_ALTDEFAULT)
		.value("TABLE_STYLE_STAGINGROOM", TABLE_STYLE_STAGINGROOM)
		;
	python::enum_<EventContextTypes>("EventContextTypes")
		.value("NO_EVENTCONTEXT", NO_EVENTCONTEXT)
		.value("EVENTCONTEXT_SELF", EVENTCONTEXT_SELF)
		.value("EVENTCONTEXT_ALL", EVENTCONTEXT_ALL)
		;
	python::enum_<TabGroupTypes>("TabGroupTypes")
		.value("NO_TABGROUP", NO_TABGROUP)
		.value("TABGROUP_GAME", TABGROUP_GAME)
		.value("TABGROUP_INPUT", TABGROUP_INPUT)
		.value("TABGROUP_GRAPHICS", TABGROUP_GRAPHICS)
		.value("TABGROUP_AUDIO", TABGROUP_AUDIO)
		.value("TABGROUP_CLOCK", TABGROUP_CLOCK)
		.value("NUM_TABGROUPS", NUM_TABGROUPS)
		;
	python::enum_<ReplayMessageTypes>("ReplayMessageTypes")
		.value("NO_REPLAY_MESSAGE", NO_REPLAY_MESSAGE)
		.value("REPLAY_MESSAGE_MAJOR_EVENT", REPLAY_MESSAGE_MAJOR_EVENT)
		.value("REPLAY_MESSAGE_CITY_FOUNDED", REPLAY_MESSAGE_CITY_FOUNDED)
		.value("REPLAY_MESSAGE_PLOT_OWNER_CHANGE", REPLAY_MESSAGE_PLOT_OWNER_CHANGE)
		.value("NUM_REPLAY_MESSAGE_TYPES", NUM_REPLAY_MESSAGE_TYPES)
		;
	python::enum_<AudioTag>("AudioTag")
		.value("AUDIOTAG_NONE", AUDIOTAG_NONE)
		.value("AUDIOTAG_SOUNDID", AUDIOTAG_SOUNDID)
		.value("AUDIOTAG_CONTEXTID", AUDIOTAG_CONTEXTID)
		.value("AUDIOTAG_SOUNDTYPE", AUDIOTAG_SOUNDTYPE)
		.value("AUDIOTAG_2DSCRIPT", AUDIOTAG_2DSCRIPT)
		.value("AUDIOTAG_3DSCRIPT", AUDIOTAG_3DSCRIPT)
		.value("AUDIOTAG_SOUNDSCAPE", AUDIOTAG_SOUNDSCAPE)
		.value("AUDIOTAG_POSITION", AUDIOTAG_POSITION)
		.value("AUDIOTAG_SCRIPTTYPE", AUDIOTAG_SCRIPTTYPE)
		.value("AUDIOTAG_LOADTYPE", AUDIOTAG_LOADTYPE)
		.value("AUDIOTAG_COUNT", AUDIOTAG_COUNT)
		;
	python::enum_<CivilopediaPageTypes>("CivilopediaPageTypes")
		.value("NO_CIVILOPEDIA_PAGE", NO_CIVILOPEDIA_PAGE)
		.value("CIVILOPEDIA_PAGE_UNIT", CIVILOPEDIA_PAGE_UNIT)
		.value("CIVILOPEDIA_PAGE_PROFESSION", CIVILOPEDIA_PAGE_PROFESSION)
		.value("CIVILOPEDIA_PAGE_BUILDING", CIVILOPEDIA_PAGE_BUILDING)
		.value("CIVILOPEDIA_PAGE_FATHER", CIVILOPEDIA_PAGE_FATHER)
		.value("CIVILOPEDIA_PAGE_BONUS", CIVILOPEDIA_PAGE_BONUS)
		.value("CIVILOPEDIA_PAGE_IMPROVEMENT", CIVILOPEDIA_PAGE_IMPROVEMENT)
		.value("CIVILOPEDIA_PAGE_PROMOTION", CIVILOPEDIA_PAGE_PROMOTION)
		.value("CIVILOPEDIA_PAGE_CIV", CIVILOPEDIA_PAGE_CIV)
		.value("CIVILOPEDIA_PAGE_LEADER", CIVILOPEDIA_PAGE_LEADER)
		.value("CIVILOPEDIA_PAGE_CIVIC", CIVILOPEDIA_PAGE_CIVIC)
		.value("CIVILOPEDIA_PAGE_CONCEPT", CIVILOPEDIA_PAGE_CONCEPT)
		.value("CIVILOPEDIA_PAGE_TERRAIN", CIVILOPEDIA_PAGE_TERRAIN)
		.value("CIVILOPEDIA_PAGE_YIELDS", CIVILOPEDIA_PAGE_YIELDS)
		.value("CIVILOPEDIA_PAGE_FEATURE", CIVILOPEDIA_PAGE_FEATURE)
		.value("CIVILOPEDIA_PAGE_HINTS", CIVILOPEDIA_PAGE_HINTS)
		.value("NUM_CIVILOPEDIA_PAGE_TYPES", NUM_CIVILOPEDIA_PAGE_TYPES)
		;
	python::enum_<ActionSubTypes>("ActionSubTypes")
		.value("NO_ACTIONSUBTYPE", NO_ACTIONSUBTYPE)
		.value("ACTIONSUBTYPE_INTERFACEMODE", ACTIONSUBTYPE_INTERFACEMODE)
		.value("ACTIONSUBTYPE_COMMAND", ACTIONSUBTYPE_COMMAND)
		.value("ACTIONSUBTYPE_BUILD", ACTIONSUBTYPE_BUILD)
		.value("ACTIONSUBTYPE_PROMOTION", ACTIONSUBTYPE_PROMOTION)
		.value("ACTIONSUBTYPE_UNIT", ACTIONSUBTYPE_UNIT)
		.value("ACTIONSUBTYPE_CONTROL", ACTIONSUBTYPE_CONTROL)
		.value("ACTIONSUBTYPE_AUTOMATE", ACTIONSUBTYPE_AUTOMATE)
		.value("ACTIONSUBTYPE_MISSION", ACTIONSUBTYPE_MISSION)
		.value("NUM_ACTIONSUBTYPES", NUM_ACTIONSUBTYPES)
		;
	python::enum_<GameMessageTypes>("GameMessageTypes")
		.value("GAMEMESSAGE_NETWORK_READY", GAMEMESSAGE_NETWORK_READY)
		.value("GAMEMESSAGE_SAVE_GAME_FLAG", GAMEMESSAGE_SAVE_GAME_FLAG)
		.value("GAMEMESSAGE_SAVE_FLAG_ACK", GAMEMESSAGE_SAVE_FLAG_ACK)
		.value("GAMEMESSAGE_VERIFY_VERSION", GAMEMESSAGE_VERIFY_VERSION)
		.value("GAMEMESSAGE_VERSION_NACK", GAMEMESSAGE_VERSION_NACK)
		.value("GAMEMESSAGE_VERSION_WARNING", GAMEMESSAGE_VERSION_WARNING)
		.value("GAMEMESSAGE_GAME_TYPE", GAMEMESSAGE_GAME_TYPE)
		.value("GAMEMESSAGE_ID_ASSIGNMENT", GAMEMESSAGE_ID_ASSIGNMENT)
		.value("GAMEMESSAGE_FILE_INFO", GAMEMESSAGE_FILE_INFO)
		.value("GAMEMESSAGE_PICK_YOUR_CIV", GAMEMESSAGE_PICK_YOUR_CIV)
		.value("GAMEMESSAGE_CIV_CHOICE", GAMEMESSAGE_CIV_CHOICE)
		.value("GAMEMESSAGE_CONFIRM_CIV_CLAIM", GAMEMESSAGE_CONFIRM_CIV_CLAIM)
		.value("GAMEMESSAGE_CLAIM_INFO", GAMEMESSAGE_CLAIM_INFO)
		.value("GAMEMESSAGE_CIV_CHOICE_ACK", GAMEMESSAGE_CIV_CHOICE_ACK)
		.value("GAMEMESSAGE_CIV_CHOICE_NACK", GAMEMESSAGE_CIV_CHOICE_NACK)
		.value("GAMEMESSAGE_CIV_CHOSEN", GAMEMESSAGE_CIV_CHOSEN)
		.value("GAMEMESSAGE_INTERIM_NOTICE", GAMEMESSAGE_INTERIM_NOTICE)
		.value("GAMEMESSAGE_INIT_INFO", GAMEMESSAGE_INIT_INFO)
		.value("GAMEMESSAGE_MAPSCRIPT_CHECK", GAMEMESSAGE_MAPSCRIPT_CHECK)
		.value("GAMEMESSAGE_MAPSCRIPT_ACK", GAMEMESSAGE_MAPSCRIPT_ACK)
		.value("GAMEMESSAGE_LOAD_GAME", GAMEMESSAGE_LOAD_GAME)
		.value("GAMEMESSAGE_PLAYER_ID", GAMEMESSAGE_PLAYER_ID)
		.value("GAMEMESSAGE_SLOT_REASSIGNMENT", GAMEMESSAGE_SLOT_REASSIGNMENT)
		.value("GAMEMESSAGE_PLAYER_INFO", GAMEMESSAGE_PLAYER_INFO)
		.value("GAMEMESSAGE_GAME_INFO", GAMEMESSAGE_GAME_INFO)
		.value("GAMEMESSAGE_REASSIGN_PLAYER", GAMEMESSAGE_REASSIGN_PLAYER)
		.value("GAMEMESSAGE_PITBOSS_INFO", GAMEMESSAGE_PITBOSS_INFO)
		.value("GAMEMESSAGE_LAUNCHING_INFO", GAMEMESSAGE_LAUNCHING_INFO)
		.value("GAMEMESSAGE_INIT_GAME", GAMEMESSAGE_INIT_GAME)
		.value("GAMEMESSAGE_INIT_PLAYERS", GAMEMESSAGE_INIT_PLAYERS)
		.value("GAMEMESSAGE_AUTH_REQUEST", GAMEMESSAGE_AUTH_REQUEST)
		.value("GAMEMESSAGE_AUTH_RESPONSE", GAMEMESSAGE_AUTH_RESPONSE)
		.value("GAMEMESSAGE_SYNCH_START", GAMEMESSAGE_SYNCH_START)
		.value("GAMEMESSAGE_EXTENDED_GAME", GAMEMESSAGE_EXTENDED_GAME)
		.value("GAMEMESSAGE_JOIN_GROUP", GAMEMESSAGE_JOIN_GROUP)
		.value("GAMEMESSAGE_PUSH_MISSION", GAMEMESSAGE_PUSH_MISSION)
		.value("GAMEMESSAGE_AUTO_MISSION", GAMEMESSAGE_AUTO_MISSION)
		.value("GAMEMESSAGE_DO_COMMAND", GAMEMESSAGE_DO_COMMAND)
		.value("GAMEMESSAGE_PUSH_ORDER", GAMEMESSAGE_PUSH_ORDER)
		.value("GAMEMESSAGE_POP_ORDER", GAMEMESSAGE_POP_ORDER)
		.value("GAMEMESSAGE_DO_TASK", GAMEMESSAGE_DO_TASK)
		.value("GAMEMESSAGE_CHAT", GAMEMESSAGE_CHAT)
		.value("GAMEMESSAGE_PING", GAMEMESSAGE_PING)
		.value("GAMEMESSAGE_SIGN", GAMEMESSAGE_SIGN)
		.value("GAMEMESSAGE_LINE_ENTITY", GAMEMESSAGE_LINE_ENTITY)
		.value("GAMEMESSAGE_SIGN_DELETE", GAMEMESSAGE_SIGN_DELETE)
		.value("GAMEMESSAGE_LINE_ENTITY_DELETE", GAMEMESSAGE_LINE_ENTITY_DELETE)
		.value("GAMEMESSAGE_LINE_GROUP_DELETE", GAMEMESSAGE_LINE_GROUP_DELETE)
		.value("GAMEMESSAGE_PAUSE", GAMEMESSAGE_PAUSE)
		.value("GAMEMESSAGE_MP_KICK", GAMEMESSAGE_MP_KICK)
		.value("GAMEMESSAGE_MP_RETIRE", GAMEMESSAGE_MP_RETIRE)
		.value("GAMEMESSAGE_CLOSE_CONNECTION", GAMEMESSAGE_CLOSE_CONNECTION)
		.value("GAMEMESSAGE_NEVER_JOINED", GAMEMESSAGE_NEVER_JOINED)
		.value("GAMEMESSAGE_MP_DROP_INIT", GAMEMESSAGE_MP_DROP_INIT)
		.value("GAMEMESSAGE_MP_DROP_VOTE", GAMEMESSAGE_MP_DROP_VOTE)
		.value("GAMEMESSAGE_MP_DROP_UPDATE", GAMEMESSAGE_MP_DROP_UPDATE)
		.value("GAMEMESSAGE_MP_DROP_RESULT", GAMEMESSAGE_MP_DROP_RESULT)
		.value("GAMEMESSAGE_MP_DROP_SAVE", GAMEMESSAGE_MP_DROP_SAVE)
		.value("GAMEMESSAGE_TOGGLE_TRADE", GAMEMESSAGE_TOGGLE_TRADE)
		.value("GAMEMESSAGE_IMPLEMENT_OFFER", GAMEMESSAGE_IMPLEMENT_OFFER)
		.value("GAMEMESSAGE_CHANGE_WAR", GAMEMESSAGE_CHANGE_WAR)
		.value("GAMEMESSAGE_APPLY_EVENT", GAMEMESSAGE_APPLY_EVENT)
		.value("GAMEMESSAGE_CONTACT_CIV", GAMEMESSAGE_CONTACT_CIV)
		.value("GAMEMESSAGE_DIPLO_CHAT", GAMEMESSAGE_DIPLO_CHAT)
		.value("GAMEMESSAGE_SEND_OFFER", GAMEMESSAGE_SEND_OFFER)
		.value("GAMEMESSAGE_DIPLO_EVENT", GAMEMESSAGE_DIPLO_EVENT)
		.value("GAMEMESSAGE_RENEGOTIATE", GAMEMESSAGE_RENEGOTIATE)
		.value("GAMEMESSAGE_RENEGOTIATE_ITEM", GAMEMESSAGE_RENEGOTIATE_ITEM)
		.value("GAMEMESSAGE_EXIT_TRADE", GAMEMESSAGE_EXIT_TRADE)
		.value("GAMEMESSAGE_KILL_DEAL", GAMEMESSAGE_KILL_DEAL)
		.value("GAMEMESSAGE_SAVE_GAME", GAMEMESSAGE_SAVE_GAME)
		.value("GAMEMESSAGE_UPDATE_CIVICS", GAMEMESSAGE_UPDATE_CIVICS)
		.value("GAMEMESSAGE_CLEAR_TABLE", GAMEMESSAGE_CLEAR_TABLE)
		.value("GAMEMESSAGE_POPUP_PROCESSED", GAMEMESSAGE_POPUP_PROCESSED)
		.value("GAMEMESSAGE_DIPLOMACY_PROCESSED", GAMEMESSAGE_DIPLOMACY_PROCESSED)
		.value("GAMEMESSAGE_HOT_JOIN_NOTICE", GAMEMESSAGE_HOT_JOIN_NOTICE)
		.value("GAMEMESSAGE_HOT_DROP_NOTICE", GAMEMESSAGE_HOT_DROP_NOTICE)
		.value("GAMEMESSAGE_DIPLOMACY", GAMEMESSAGE_DIPLOMACY)
		.value("GAMEMESSAGE_POPUP", GAMEMESSAGE_POPUP)
		.value("GAMEMESSAGE_ADVANCED_START_ACTION", GAMEMESSAGE_ADVANCED_START_ACTION)
		.value("GAMEMESSAGE_PLAYER_ACTION", GAMEMESSAGE_PLAYER_ACTION)
		.value("GAMEMESSAGE_MOD_NET_MESSAGE", GAMEMESSAGE_MOD_NET_MESSAGE)
		;
	python::enum_<PopupControlLayout>("PopupControlLayout")
		.value("POPUP_LAYOUT_LEFT", POPUP_LAYOUT_LEFT)
		.value("POPUP_LAYOUT_CENTER", POPUP_LAYOUT_CENTER)
		.value("POPUP_LAYOUT_RIGHT", POPUP_LAYOUT_RIGHT)
		.value("POPUP_LAYOUT_STRETCH", POPUP_LAYOUT_STRETCH)
		.value("POPUP_LAYOUT_TOP", POPUP_LAYOUT_TOP)
		.value("POPUP_LAYOUT_NUMLAYOUTS", POPUP_LAYOUT_NUMLAYOUTS)
		;
	python::enum_<JustificationTypes>("JustificationTypes")
		.value("DLL_FONT_LEFT_JUSTIFY", DLL_FONT_LEFT_JUSTIFY)
		.value("DLL_FONT_RIGHT_JUSTIFY", DLL_FONT_RIGHT_JUSTIFY)
		.value("DLL_FONT_CENTER_JUSTIFY", DLL_FONT_CENTER_JUSTIFY)
		.value("DLL_FONT_CENTER_VERTICALLY", DLL_FONT_CENTER_VERTICALLY)
		.value("DLL_FONT_ADDITIVE", DLL_FONT_ADDITIVE)
		;
	python::enum_<ToolTipAlignTypes>("ToolTipAlignTypes")
		.value("TOOLTIP_TOP_LEFT", TOOLTIP_TOP_LEFT)
		.value("TOOLTIP_TOP_INLEFT", TOOLTIP_TOP_INLEFT)
		.value("TOOLTIP_TOP_CENTER", TOOLTIP_TOP_CENTER)
		.value("TOOLTIP_TOP_INRIGHT", TOOLTIP_TOP_INRIGHT)
		.value("TOOLTIP_TOP_RIGHT", TOOLTIP_TOP_RIGHT)
		.value("TOOLTIP_INTOP_RIGHT", TOOLTIP_INTOP_RIGHT)
		.value("TOOLTIP_CENTER_RIGHT", TOOLTIP_CENTER_RIGHT)
		.value("TOOLTIP_INBOTTOM_RIGHT", TOOLTIP_INBOTTOM_RIGHT)
		.value("TOOLTIP_BOTTOM_RIGHT", TOOLTIP_BOTTOM_RIGHT)
		.value("TOOLTIP_BOTTOM_INRIGHT", TOOLTIP_BOTTOM_INRIGHT)
		.value("TOOLTIP_BOTTOM_CENTER", TOOLTIP_BOTTOM_CENTER)
		.value("TOOLTIP_BOTTOM_INLEFT", TOOLTIP_BOTTOM_INLEFT)
		.value("TOOLTIP_BOTTOM_LEFT", TOOLTIP_BOTTOM_LEFT)
		.value("TOOLTIP_INBOTTOM_LEFT", TOOLTIP_INBOTTOM_LEFT)
		.value("TOOLTIP_CENTER_LEFT", TOOLTIP_CENTER_LEFT)
		.value("TOOLTIP_INTOP_LEFT", TOOLTIP_INTOP_LEFT)
		;
	python::enum_<ActivationTypes>("ActivationTypes")
		.value("ACTIVATE_NORMAL", ACTIVATE_NORMAL)
		.value("ACTIVATE_CHILDFOCUS", ACTIVATE_CHILDFOCUS)
		.value("ACTIVATE_MIMICPARENT", ACTIVATE_MIMICPARENT)
		.value("ACTIVATE_MIMICPARENTFOCUS", ACTIVATE_MIMICPARENTFOCUS)
		;
	python::enum_<HitTestTypes>("HitTestTypes")
		.value("HITTEST_DEFAULT", HITTEST_DEFAULT)
		.value("HITTEST_NOHIT", HITTEST_NOHIT)
		.value("HITTEST_SOLID", HITTEST_SOLID)
		.value("HITTEST_ON", HITTEST_ON)
		.value("HITTEST_CHILDREN", HITTEST_CHILDREN)
		;
	python::enum_<GraphicLevelTypes>("GraphicLevelTypes")
		.value("GRAPHICLEVEL_HIGH", GRAPHICLEVEL_HIGH)
		.value("GRAPHICLEVEL_MEDIUM", GRAPHICLEVEL_MEDIUM)
		.value("GRAPHICLEVEL_LOW", GRAPHICLEVEL_LOW)
		.value("GRAPHICLEVEL_CURRENT", GRAPHICLEVEL_CURRENT)
		.value("GRAPHICLEVEL_BELOW_SPEC", GRAPHICLEVEL_BELOW_SPEC)
		.value("NUM_GRAPHICLEVELS", NUM_GRAPHICLEVELS)
		;
	python::enum_<EventTypes>("EventTypes")
		.value("NO_EVENT", NO_EVENT)
		;
	python::enum_<EventTriggerTypes>("EventTriggerTypes")
		.value("NO_EVENTTRIGGER", NO_EVENTTRIGGER)
		;
	python::enum_<AdvancedStartActionTypes>("AdvancedStartActionTypes")
		.value("NO_ADVANCEDSTARTACTION", NO_ADVANCEDSTARTACTION)
		.value("ADVANCEDSTARTACTION_EXIT", ADVANCEDSTARTACTION_EXIT)
		.value("ADVANCEDSTARTACTION_UNIT", ADVANCEDSTARTACTION_UNIT)
		.value("ADVANCEDSTARTACTION_CITY", ADVANCEDSTARTACTION_CITY)
		.value("ADVANCEDSTARTACTION_POP", ADVANCEDSTARTACTION_POP)
		.value("ADVANCEDSTARTACTION_CULTURE", ADVANCEDSTARTACTION_CULTURE)
		.value("ADVANCEDSTARTACTION_BUILDING", ADVANCEDSTARTACTION_BUILDING)
		.value("ADVANCEDSTARTACTION_IMPROVEMENT", ADVANCEDSTARTACTION_IMPROVEMENT)
		.value("ADVANCEDSTARTACTION_ROUTE", ADVANCEDSTARTACTION_ROUTE)
		.value("ADVANCEDSTARTACTION_VISIBILITY", ADVANCEDSTARTACTION_VISIBILITY)
		.value("ADVANCEDSTARTACTION_AUTOMATE", ADVANCEDSTARTACTION_AUTOMATE)
		.value("NUM_ADVANCEDSTARTACTIONS", NUM_ADVANCEDSTARTACTIONS)
		;
	python::enum_<UnitTravelStates>("UnitTravelStates")
		.value("NO_UNIT_TRAVEL_STATE", NO_UNIT_TRAVEL_STATE)
		.value("UNIT_TRAVEL_STATE_TO_EUROPE", UNIT_TRAVEL_STATE_TO_EUROPE)
		.value("UNIT_TRAVEL_STATE_IN_EUROPE", UNIT_TRAVEL_STATE_IN_EUROPE)
		.value("UNIT_TRAVEL_STATE_FROM_EUROPE", UNIT_TRAVEL_STATE_FROM_EUROPE)
		.value("UNIT_TRAVEL_STATE_LIVE_AMONG_NATIVES", UNIT_TRAVEL_STATE_LIVE_AMONG_NATIVES)
		/*** TRIANGLETRADE 10/24/08 by DPII ***/
		.value("UNIT_TRAVEL_STATE_TO_AFRICA", UNIT_TRAVEL_STATE_TO_AFRICA)
		.value("UNIT_TRAVEL_STATE_IN_AFRICA", UNIT_TRAVEL_STATE_IN_AFRICA)
		.value("UNIT_TRAVEL_STATE_FROM_AFRICA", UNIT_TRAVEL_STATE_FROM_AFRICA)
		// R&R, ray, Port Royal
		.value("UNIT_TRAVEL_STATE_TO_PORT_ROYAL", UNIT_TRAVEL_STATE_TO_PORT_ROYAL)
		.value("UNIT_TRAVEL_STATE_IN_PORT_ROYAL", UNIT_TRAVEL_STATE_IN_PORT_ROYAL)
		.value("UNIT_TRAVEL_STATE_FROM_PORT_ROYAL", UNIT_TRAVEL_STATE_FROM_PORT_ROYAL)
		.value("NUM_UNIT_TRAVEL_STATES", NUM_UNIT_TRAVEL_STATES)
		;
	python::enum_<FatherCategoryTypes>("FatherCategoryTypes")
		.value("NO_FATHERCATEGORY", NO_FATHERCATEGORY)
		;
	python::enum_<FatherTypes>("FatherTypes")
		.value("NO_FATHER", NO_FATHER)
		;
	python::enum_<FatherPointTypes>("FatherPointTypes")
		.value("NO_FATHER_POINT_TYPE", NO_FATHER_POINT_TYPE)
		;
	python::enum_<AlarmTypes>("AlarmTypes")
		.value("NO_ALARM", NO_ALARM)
		;

	// TAC - Trade Messages - koma13 - START
	python::enum_<TradeMessageTypes>("TradeMessageTypes")
		.value("NO_TRADE_MESSAGE", NO_TRADE_MESSAGE)
		.value("TRADE_MESSAGE_EUROPE_YIELD_SOLD", TRADE_MESSAGE_EUROPE_YIELD_SOLD)
		.value("TRADE_MESSAGE_EUROPE_YIELD_BOUGHT", TRADE_MESSAGE_EUROPE_YIELD_BOUGHT)
		.value("TRADE_MESSAGE_TREASURE", TRADE_MESSAGE_TREASURE)
		.value("TRADE_MESSAGE_LACK_FUNDS", TRADE_MESSAGE_LACK_FUNDS)
		.value("NUM_TRADE_MESSAGES", NUM_TRADE_MESSAGES)
		;
	// TAC - Trade Messages - koma13 - END

	python::enum_<JITarrayTypes>("JITarrayTypes")
		.value("NO_JIT_ARRAY_TYPE", NO_JIT_ARRAY_TYPE)
		.value("JIT_ARRAY_ACHIEVE", JIT_ARRAY_ACHIEVE)
		.value("JIT_ARRAY_ART_STYLE", JIT_ARRAY_ARTSTYLE)
		.value("JIT_ARRAY_BONUS", JIT_ARRAY_BONUS)
		.value("JIT_ARRAY_BUILD", JIT_ARRAY_BUILD)
		.value("JIT_ARRAY_BUILDING", JIT_ARRAY_BUILDING)
		.value("JIT_ARRAY_BUILDING_CLASS", JIT_ARRAY_BUILDINGCLASS)
		.value("JIT_ARRAY_BUILDING_SPECIAL", JIT_ARRAY_BUILDING_SPECIAL)
		.value("JIT_ARRAY_CIV_EFFECT", JIT_ARRAY_CIV_EFFECT)
		.value("JIT_ARRAY_CIVIC", JIT_ARRAY_CIVIC)
		.value("JIT_ARRAY_CIVIC_OPTION", JIT_ARRAY_CIVICOPTION)
		.value("JIT_ARRAY_CIVILIZATION", JIT_ARRAY_CIVILIZATION)
		.value("JIT_ARRAY_CLIMATE", JIT_ARRAY_CLIMATE)
		.value("JIT_ARRAY_COLOR", JIT_ARRAY_COLOR)
		.value("JIT_ARRAY_CULTURE", JIT_ARRAY_CULTURE)
		.value("JIT_ARRAY_DIPLO", JIT_ARRAY_DIPLOMACY)
		.value("JIT_ARRAY_DOMAIN", JIT_ARRAY_DOMAIN)
		.value("JIT_ARRAY_EMPHASIZE", JIT_ARRAY_EMPHASIZE)
		.value("JIT_ARRAY_ERA", JIT_ARRAY_ERA)
		.value("JIT_ARRAY_EUROPE", JIT_ARRAY_EUROPE)
		.value("JIT_ARRAY_EVENT", JIT_ARRAY_EVENT)
		.value("JIT_ARRAY_EVENT_TRIGGER", JIT_ARRAY_EVENT_TRIGGER)
		.value("JIT_ARRAY_FATHER", JIT_ARRAY_FATHER)
		.value("JIT_ARRAY_FATHER_POINT", JIT_ARRAY_FATHER_POINT)
		.value("JIT_ARRAY_FEATURE", JIT_ARRAY_FEATURE)
		.value("JIT_ARRAY_GAME_OPTION", JIT_ARRAY_GAME_OPTION)
		.value("JIT_ARRAY_GAME_SPEED", JIT_ARRAY_GAME_SPEED)
		.value("JIT_ARRAY_GOODY", JIT_ARRAY_GOODY)
		.value("JIT_ARRAY_HANDICAP", JIT_ARRAY_HANDICAP)
		.value("JIT_ARRAY_HURRY", JIT_ARRAY_HURRY)
		.value("JIT_ARRAY_IMPROVEMENT", JIT_ARRAY_IMPROVEMENT)
		.value("JIT_ARRAY_INVISIBLE", JIT_ARRAY_INVISIBLE)
		.value("JIT_ARRAY_LEADER_HEAD", JIT_ARRAY_LEADER)
		.value("JIT_ARRAY_MEMORY", JIT_ARRAY_MEMORY)
		.value("JIT_ARRAY_PLAYER_COLOR", JIT_ARRAY_PLAYER_COLOR)
		.value("JIT_ARRAY_PLAYER_OPTION", JIT_ARRAY_PLAYER_OPTION)
		.value("JIT_ARRAY_PROFESSION", JIT_ARRAY_PROFESSION)
		.value("JIT_ARRAY_PROMOTION", JIT_ARRAY_PROMOTION)
		.value("JIT_ARRAY_ROUTE", JIT_ARRAY_ROUTE)
		.value("JIT_ARRAY_SEA_LEVEL", JIT_ARRAY_SEA_LEVEL)
		.value("JIT_ARRAY_TERRAIN", JIT_ARRAY_TERRAIN)
		.value("JIT_ARRAY_TRAIT", JIT_ARRAY_TRAIT)
		.value("JIT_ARRAY_UNIT", JIT_ARRAY_UNIT)
		.value("JIT_ARRAY_UNIT_AI", JIT_ARRAY_UNITAI)
		.value("JIT_ARRAY_UNIT_CLASS", JIT_ARRAY_UNITCLASS)
		.value("JIT_ARRAY_UNIT_COMBAT", JIT_ARRAY_UNITCOMBAT)
		.value("JIT_ARRAY_UNIT_SPECIAL", JIT_ARRAY_UNIT_SPECIAL)
		.value("JIT_ARRAY_VICTORY", JIT_ARRAY_VICTORY)
		.value("JIT_ARRAY_WORLD_SIZE", JIT_ARRAY_WORLD_SIZE)
		.value("JIT_ARRAY_YIELD", JIT_ARRAY_YIELD)

		.value("NUM_JITarrayTypes", NUM_JITarrayTypes)

		.value("JIT_ARRAY_CONTACT", JIT_ARRAY_CONTACT)
		.value("JIT_ARRAY_EMOTION", JIT_ARRAY_EMOTION)
		.value("JIT_ARRAY_FEAT", JIT_ARRAY_FEAT)
		.value("JIT_ARRAY_MISSION_AI", JIT_ARRAY_MISSION_AI)
		.value("JIT_ARRAY_STRATEGY", JIT_ARRAY_STRATEGY)

		.value("JIT_ARRAY_NO_TYPE", JIT_ARRAY_NO_TYPE)
		.value("JIT_ARRAY_MODIFIER", JIT_ARRAY_MODIFIER)
		.value("JIT_ARRAY_MODIFIER_FLOAT", JIT_ARRAY_MODIFIER_FLOAT)
		.value("JIT_ARRAY_ALLOW", JIT_ARRAY_ALLOW)
		.value("JIT_ARRAY_INT", JIT_ARRAY_INT)
		.value("JIT_ARRAY_UNSIGNED_INT", JIT_ARRAY_UNSIGNED_INT)
		.value("JIT_ARRAY_FLOAT", JIT_ARRAY_FLOAT)
		;
// python_enum_check.pl NEXT STATE
	static bool bFirstRun = true;
	if (bFirstRun)
	{
		bFirstRun = false;
	}
	else
	{
		// run this function on reloads, thorugh skip it the first time as it is handled later in the xml reading order
		CyEnumsPythonInterfacePostApply();
	}
}
