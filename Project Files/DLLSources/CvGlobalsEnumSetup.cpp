
#include "CvGameCoreDLL.h"

#include "UserSettings.h"

#include "autogenerated/AutoXmlDeclare.h"

void TestEnumMap();

#ifndef HARDCODE_XML_VALUES
CivEffectTypes   CIV_EFFECT_DEFAULT_ALL      = NO_CIV_EFFECT;
CivEffectTypes   CIV_EFFECT_DEFAULT_EUROPEAN = NO_CIV_EFFECT;
CivEffectTypes   CIV_EFFECT_DEFAULT_NATIVE   = NO_CIV_EFFECT;
CivEffectTypes   CIV_EFFECT_DEFAULT_KING     = NO_CIV_EFFECT;
CivEffectTypes   CIV_EFFECT_DEFAULT_BARBARIAN= NO_CIV_EFFECT;
CivEffectTypes   CIV_EFFECT_DEFAULT_CHURCH   = NO_CIV_EFFECT;
CivEffectTypes   CIV_EFFECT_DEFAULT_HUMAN    = NO_CIV_EFFECT;
CivEffectTypes   CIV_EFFECT_DEFAULT_AI       = NO_CIV_EFFECT;

HurryTypes       HURRY_GOLD                  = NO_HURRY;
HurryTypes       HURRY_IMMIGRANT             = NO_HURRY;

SpecialUnitTypes SPECIALUNIT_COLONIST_UNIT   = NO_SPECIALUNIT;
#endif


static void DisplayXMLmissingError(bool bSuccess, const char* szName)
{
	if (!bSuccess)
	{
		char szMessage[1024];

		sprintf(szMessage, "XML is missing the mandatory entry: %s", szName);
		gDLL->MessageBox(szMessage, "Missing XML entry Error");
	}
}

static void DisplayXMLhardcodingError(bool bSuccess, const char* szName, bool bAlwaysHardcoded)
{
	if (!bSuccess)
	{
		char szMessage[1024];

		sprintf(szMessage, "DLL hardcoding error: %s\n%s", szName,
			bAlwaysHardcoded ? "This is always hardcoded and changing it in XML requires recompiling the DLL." : "Use a non-hardcoded DLL or recompile the DLL to match your new XML setting."
			);
		gDLL->MessageBox(szMessage, "XML mismatch hardcoded DLL Error");
	}
}

static void DisplayXMLhardcodingError(const char* szName, const char* szAssumedName)
{
	DisplayXMLhardcodingError(strcmp(szName, szAssumedName) == 0, szName, false);
}

void CvGlobals::postXMLLoad(bool bFirst)
{
	if (bFirst)
	{
		// create/update settings file if needed
		UserSettings testUserSettings;
	}

	if (bFirst)
	{
#ifndef CHECK_GLOBAL_CONSTANTS
		NUM_ARTSTYLE_TYPES    = static_cast<ArtStyleTypes   >(GC.getNumArtStyleTypes   ());
		NUM_COLOR_TYPES       = static_cast<ColorTypes      >(GC.getNumColorInfos      ());
		NUM_PLAYERCOLOR_TYPES = static_cast<PlayerColorTypes>(GC.getNumPlayerColorInfos());
#endif

#include "autogenerated/AutoXmlInit.h"

#ifndef HARDCODE_XML_VALUES

		for (HurryTypes eHurry = FIRST_HURRY; eHurry < NUM_HURRY_TYPES; ++eHurry)
		{
			const char *type = GC.getHurryInfo(eHurry).getType();
			if (strcmp(type, "HURRY_GOLD") == 0)
			{
				HURRY_GOLD = eHurry;
			}
			else if (strcmp(type, "HURRY_IMMIGRANT") == 0)
			{
				HURRY_IMMIGRANT = eHurry;
			}
		}

		SPECIALUNIT_COLONIST_UNIT = static_cast<SpecialUnitTypes>(getIndexForType(JIT_ARRAY_UNIT_SPECIAL, "SPECIALUNIT_COLONIST_UNIT"));

		int iCounter = 0;

		for (CivEffectTypes eCivEffect = FIRST_CIV_EFFECT; eCivEffect < NUM_CIV_EFFECT_TYPES; ++eCivEffect)
		{
			const char *szType = this->getCivEffectInfo(eCivEffect).getType();

			if (strcmp(szType, "CIV_EFFECT_DEFAULT_ALL") == 0)
			{
				CIV_EFFECT_DEFAULT_ALL = eCivEffect;
			}
			else if (strcmp(szType, "CIV_EFFECT_DEFAULT_EUROPEAN") == 0)
			{
				CIV_EFFECT_DEFAULT_EUROPEAN = eCivEffect;
			}
			else if (strcmp(szType, "CIV_EFFECT_DEFAULT_NATIVE") == 0)
			{
				CIV_EFFECT_DEFAULT_NATIVE = eCivEffect;
			}
			else if (strcmp(szType, "CIV_EFFECT_DEFAULT_KING") == 0)
			{
				CIV_EFFECT_DEFAULT_KING = eCivEffect;
			}
			else if (strcmp(szType, "CIV_EFFECT_DEFAULT_BARBARIAN") == 0)
			{
				CIV_EFFECT_DEFAULT_BARBARIAN = eCivEffect;
			}
			else if (strcmp(szType, "CIV_EFFECT_DEFAULT_CHURCH") == 0)
			{
				CIV_EFFECT_DEFAULT_CHURCH = eCivEffect;
			}
			else if (strcmp(szType, "CIV_EFFECT_DEFAULT_HUMAN") == 0)
			{
				CIV_EFFECT_DEFAULT_HUMAN = eCivEffect;
			}
			else if (strcmp(szType, "CIV_EFFECT_DEFAULT_AI") == 0)
			{
				CIV_EFFECT_DEFAULT_AI = eCivEffect;
			}
			else
			{
				continue;
			}
			++iCounter;
			if (iCounter == 8)
			{
				// all found. No need to loop the rest
				break;
			}
		}
		DisplayXMLmissingError(CIV_EFFECT_DEFAULT_ALL        != NO_CIV_EFFECT, "CIV_EFFECT_DEFAULT_ALL"       );
		DisplayXMLmissingError(CIV_EFFECT_DEFAULT_EUROPEAN   != NO_CIV_EFFECT, "CIV_EFFECT_DEFAULT_EUROPEAN"  );
		DisplayXMLmissingError(CIV_EFFECT_DEFAULT_NATIVE     != NO_CIV_EFFECT, "CIV_EFFECT_DEFAULT_NATIVE"    );
		DisplayXMLmissingError(CIV_EFFECT_DEFAULT_KING       != NO_CIV_EFFECT, "CIV_EFFECT_DEFAULT_KING"      );
		DisplayXMLmissingError(CIV_EFFECT_DEFAULT_BARBARIAN  != NO_CIV_EFFECT, "CIV_EFFECT_DEFAULT_BARBARIAN" );
		DisplayXMLmissingError(CIV_EFFECT_DEFAULT_CHURCH     != NO_CIV_EFFECT, "CIV_EFFECT_DEFAULT_CHURCH"    );
		DisplayXMLmissingError(CIV_EFFECT_DEFAULT_HUMAN      != NO_CIV_EFFECT, "CIV_EFFECT_DEFAULT_HUMAN"     );
		DisplayXMLmissingError(CIV_EFFECT_DEFAULT_AI         != NO_CIV_EFFECT, "CIV_EFFECT_DEFAULT_AI"        );

#else

		DisplayXMLhardcodingError("CIV_EFFECT_DEFAULT_ALL"       , this->getCivEffectInfo(CIV_EFFECT_DEFAULT_ALL       ).getType());
		DisplayXMLhardcodingError("CIV_EFFECT_DEFAULT_EUROPEAN"  , this->getCivEffectInfo(CIV_EFFECT_DEFAULT_EUROPEAN  ).getType());
		DisplayXMLhardcodingError("CIV_EFFECT_DEFAULT_NATIVE"    , this->getCivEffectInfo(CIV_EFFECT_DEFAULT_NATIVE    ).getType());
		DisplayXMLhardcodingError("CIV_EFFECT_DEFAULT_KING"      , this->getCivEffectInfo(CIV_EFFECT_DEFAULT_KING      ).getType());
		DisplayXMLhardcodingError("CIV_EFFECT_DEFAULT_HUMAN"     , this->getCivEffectInfo(CIV_EFFECT_DEFAULT_HUMAN     ).getType());
		DisplayXMLhardcodingError("CIV_EFFECT_DEFAULT_AI"        , this->getCivEffectInfo(CIV_EFFECT_DEFAULT_AI        ).getType());

		DisplayXMLhardcodingError("HURRY_GOLD"                   , this->getHurryInfo(HURRY_GOLD                       ).getType());
		DisplayXMLhardcodingError("HURRY_IMMIGRANT"              , this->getHurryInfo(HURRY_IMMIGRANT                  ).getType());

#endif
		DisplayXMLmissingError(HURRY_GOLD      != NO_HURRY, "HURRY_GOLD");
		DisplayXMLmissingError(HURRY_IMMIGRANT != NO_HURRY, "HURRY_IMMIGRANT");

	}
	else // bFirst
	{
		// Now all xml data has been loaded

		// first test if DLL hardcoding and xml are out of sync
#include "autogenerated/AutoXmlTest.h"

		// Generate the default CivEffect.
		// It's given to all players and enables everything, which can't be enabled by CivEffect.
		// If it isn't explicitly enabled, it's assumed to be available from the start.
		this->m_pAutogeneratedCivEffect = new CivEffectInfo(true);
		
		// set up consumed yields for fast looping
		{
			EnumMap<YieldTypes, int> aYields;

			for (BuildingTypes eBuilding = FIRST_BUILDING; eBuilding < NUM_BUILDING_TYPES; ++eBuilding)
			{
				CvBuildingInfo &kBuilding = GC.getBuildingInfo(eBuilding);
				kBuilding.getYieldDemands().addTo(aYields);
			}
			for (UnitTypes eUnit = FIRST_UNIT; eUnit < NUM_UNIT_TYPES; ++eUnit)
			{
				CvUnitInfo &kUnit = GC.getUnitInfo(eUnit);
				kUnit.getYieldDemands().addTo(aYields);
			}
			m_iaDomesticDemandYieldTypes.assignFrom(aYields);
		}

		for (EventTriggerTypes eTrigger = FIRST_EVENTTRIGGER; eTrigger < NUM_EVENTTRIGGER_TYPES; ++eTrigger)
		{
			GC.getEventTriggerInfo(eTrigger).verifyTriggerSettings();
		}
	}

#ifdef _DEBUG
	if (bFirst)
	{
		TestEnumMap();
	}
#endif
}


/// GameFont XML control - start - Nightinggale
void CvGlobals::setupGameFontChars()
{
	// call setChar after the exe is done setting GameFont chars
	// while the exe generally does a good job at assigning GameFont IDs, it has limitations and it can fail

	const int bonusBaseID = GC.getFontSymbolBonusOffset();

	for (BonusTypes eBonus = FIRST_BONUS; eBonus < NUM_BONUS_TYPES; ++eBonus)
	{
		// Bonus icon order is defined in bonus art. The "read" index is calculated like this in BTS.
		int bonusID = bonusBaseID + getInfo(eBonus).getArtInfo()->getFontButtonIndex();
		getInfo(eBonus).setChar(bonusID);
	}

}
/// GameFont XML control - end - Nightinggale
