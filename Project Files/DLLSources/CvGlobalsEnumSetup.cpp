
#include "CvGameCoreDLL.h"
#include "CvXMLLoadUtility.h"
#include "CvDLLXMLIFaceBase.h"

#include "UserSettings.h"

#include "autogenerated/AutoXmlDeclare.h"

void TestEnumMap();


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



#include <iostream>
#include <sstream>
#include <fstream>

static void checkPublicMapSetting()
{
	const std::string modPath = gDLL->getModName();
	const std::string modName = modPath.substr(5, modPath.length() - 6);
	const std::string iniFile = modPath + modName + ".ini";

	std::vector<std::string> file_content;

	try
	{
		std::ifstream input(iniFile.c_str());
		std::stringstream buffer;
		buffer << input.rdbuf();
		input.close();

		for (std::string line; std::getline(buffer, line); )
		{
			file_content.push_back(line);
			if (line.substr(0, 15) == "AllowPublicMaps")
			{
				if (line == "AllowPublicMaps = 0")
				{
					return;
				}
			}
		}
	}
	catch (const std::exception&)
	{
		// make the game silently ignore file read crashes
		return;
	}

	try
	{
		std::ofstream output(iniFile.c_str());
		for (unsigned i = 0; i < file_content.size(); i++)
		{
			const std::string& line = file_content[i];
			if (line.substr(0, 15) == "AllowPublicMaps")
			{
				output << "AllowPublicMaps = 0" << std::endl;
			}
			else
			{
				output << line << std::endl;
			}
		}
		output.close();
	}
	catch (const std::exception&)
	{
		char szMessage[1024];

		// TODO: figure out why trying to use TXT_KEYs (hence translations) crashes the game.
		CvString message;
		message
			.append("The game tried to set AllowPublicMaps to 0 in ")
			.append(modName)
			.append(".ini\n")
			.append("Something went wrong while doing so and now it's possible that incompatible public maps can be selected ingame.\nPlease manually open this file and set AllowPublicMaps = 0\n")
			.append("Restart the game once this is done.")
			;
		CvString header = "Configuration error";

		sprintf(szMessage, message);
		gDLL->MessageBoxA(szMessage, header);
		return;
	}

	{
		char szMessage[1024];

		// TODO: figure out why trying to use TXT_KEYs (hence translations) crashes the game.
		CvString message = "The mod has changed configuration to remove known incompatible maps from the menus.\nPlease restart the game for the new settings to take effect.";
		CvString header = "Restart required";

		sprintf(szMessage, message);
		gDLL->MessageBoxA(szMessage, header);

	}
}


void CvGlobals::postXMLLoad(bool bFirst)
{
	if (bFirst)
	{
		// create/update settings file if needed
		UserSettings testUserSettings;
	}

#include "autogenerated/AutoXmlInit.h"

	if (bFirst)
	{
#ifndef CHECK_GLOBAL_CONSTANTS
		NUM_ARTSTYLE_TYPES    = static_cast<ArtStyleTypes   >(GC.getNumArtStyleTypes   ());
#endif

#ifndef HARDCODE_XML_VALUES

		int iCounter = 0;
#endif
	}
	else // bFirst
	{
		// Now all xml data has been loaded

		checkPublicMapSetting();

		// first test if DLL hardcoding and xml are out of sync
#include "autogenerated/AutoXmlTest.h"

		// Generate the default CivEffect.
		// It's given to all players and enables everything, which can't be enabled by CivEffect.
		// If it isn't explicitly enabled, it's assumed to be available from the start.
		this->m_pAutogeneratedCivEffect = new CivEffectInfo(true);
		
		CvSpecialBuildingInfo::postXmlReadSetup();

		// set up consumed yields for fast looping
		{
			EnumMap<YieldTypes, int> aYields;

			for (BuildingTypes eBuilding = FIRST_BUILDING; eBuilding < NUM_BUILDING_TYPES; ++eBuilding)
			{
				CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);
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

template<typename T>
static void setupGameFontCharsOffset(const T eOffset)
{
	if (eOffset == 0)
	{
		return;
	}

	const T eNumInfos = (T)getArrayLength(getJITarrayType(eOffset));
	for (T eLoop = (T)0; eLoop < eNumInfos; ++eLoop)
	{
		if (GC.getInfo(eLoop).getChar() != -1)
		{
			GC.getInfo(eLoop).setChar(GC.getInfo(eLoop).getChar() + eOffset);
		}
	}
}

static void setupGameFontCharsOffset(const CivilizationTypes eOffset)
{
	const int iNoChar = GC.getCivilizationInfo(FIRST_CIVILIZATION).getChar() + eOffset;
	int iIndex = iNoChar;

	for (CivilizationTypes eLoop = FIRST_CIVILIZATION; eLoop < NUM_CIVILIZATION_TYPES; ++eLoop)
	{
		CvCivilizationInfo& kInfo = GC.getCivilizationInfo(eLoop);
		if (kInfo.getCivCategoryTypes() == CIV_CATEGORY_EUROPEAN)
		{
			iIndex += 2;
			kInfo.setChar(iIndex);
		}
		else
		{
			kInfo.setChar(iNoChar);
		}
	}
}


void CvGlobals::setupGameFontChars()
{
	// call setChar after the exe is done setting GameFont chars
	// while the exe generally does a good job at assigning GameFont IDs, it has limitations and it can fail

	CvXMLLoadUtility util;
	int iStart = 0;

	util.CreateFXml();

	FXml* pXML = util.GetXML();
	CvDLLXmlIFaceBase* pInterface = gDLL->getXMLIFace();

	bool bLoaded = util.LoadCivXml(pXML, "xml\\Interface\\GameFontIDs.xml");
	if (!bLoaded)
	{
		return;
	}

	if (!pInterface->LocateNode(pXML, "GameFontIDs"))
	{
		return;
	}

	FontSymbols eFontSymbolOffset = FIRST_FONTSYMBOL;
	pInterface->SetToChildByTagName(pXML, "Addresses");
	util.GetChildXmlValByName(&(int&)eFontSymbolOffset, "BillboardFontSymbolStart");
	pInterface->SetToParent(pXML);

	for (FontSymbols eSymbol = FIRST_FONTSYMBOL; eSymbol < MAX_NUM_SYMBOLS; ++eSymbol)
	{
		m_aiGameFontCustomSymbolID[eSymbol] = static_cast<FontSymbols>(GC.getDLLIFace()->getSymbolID(eSymbol));
	}

	if (pInterface->SetToChildByTagName(pXML, "BillboardSymbols"))
	{
		bool bSuccess = pInterface->SetToChildByTagName(pXML, "BillboardSymbol");
		if (bSuccess)
		{
			CvString szName;
			while (bSuccess)
			{
				util.GetXmlVal(szName);
				FontSymbols eIndex = getIndexOfType(eIndex, szName);
				m_aiGameFontCustomSymbolID[eIndex] = eFontSymbolOffset;
				++eFontSymbolOffset;
				bSuccess = pInterface->LocateNextSiblingNodeByTagName(pXML, "BillboardSymbol");
			}
			pInterface->SetToParent(pXML);
		}
		pInterface->SetToParent(pXML);
	}

	pInterface->SetToChildByTagName(pXML, "Offsets");
	util.GetChildXmlValByName(&iStart, "Bonus");
	setupGameFontCharsOffset(static_cast<BonusTypes>(iStart));
	util.GetChildXmlValByName(&iStart, "Building");
	setupGameFontCharsOffset(static_cast<SpecialBuildingTypes>(iStart));
	util.GetChildXmlValByName(&iStart, "FatherPoint");
	setupGameFontCharsOffset(static_cast<FatherPointTypes>(iStart));
	util.GetChildXmlValByName(&iStart, "Mission");
	setupGameFontCharsOffset(static_cast<CivilizationTypes>(iStart));
	util.GetChildXmlValByName(&iStart, "Yield");
	setupGameFontCharsOffset(static_cast<YieldTypes>(iStart));

	util.DestroyFXml();
}
/// GameFont XML control - end - Nightinggale
