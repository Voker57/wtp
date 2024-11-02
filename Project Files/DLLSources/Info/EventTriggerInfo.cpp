#include "../CvGameCoreDLL.h"
#include "EventTriggerInfo.h"
#include "../CvXMLLoadUtility.h"

EventTriggerUnitCount::EventTriggerUnitCount()
	: m_eType(NO_UNITCLASS)
	, m_iCount(0)
	, m_bCountCitizens(false)
	, m_bCountUnitsOnMap(false)
	, m_bPlotNoOwner(false)
{
}

UnitClassTypes EventTriggerUnitCount::getUnitClass() const
{
	return m_eType;
}

int EventTriggerUnitCount::getCount() const
{
	return m_iCount;
}

bool EventTriggerUnitCount::getCountCitizens() const
{
	return m_bCountCitizens;
}

bool EventTriggerUnitCount::getCountUnitsOnMap() const
{
	return m_bCountUnitsOnMap;
}

bool EventTriggerUnitCount::getPlotNoOwner() const
{
	return m_bPlotNoOwner;
}

bool EventTriggerUnitCount::read(CvXMLLoadUtility* pXML, const char* szType)
{
	pXML->GetEnum(szType, m_eType, "RequiredUnitClass");
	pXML->GetChildXmlValByName(&m_iCount, "iRequiredCount");
	pXML->GetChildXmlValByName(&m_bCountCitizens, "bCountCitizens", false);
	pXML->GetChildXmlValByName(&m_bCountUnitsOnMap, "bCountUnitsOnMap", false);
	pXML->GetChildXmlValByName(&m_bPlotNoOwner, "bUnownedPlot", false);

	return true;
}
