#pragma once

class EventTriggerUnitCount
{
public:
	EventTriggerUnitCount();

	UnitClassTypes getUnitClass() const;
	int getCount() const;
	bool getCountCitizens() const;
	bool getCountUnitsOnMap() const;
	bool getPlotNoOwner() const;

	bool read(CvXMLLoadUtility* pXML, const char* szType);

private:
	UnitClassTypes m_eType;
	int m_iCount;
	bool m_bCountCitizens;
	bool m_bCountUnitsOnMap;
	bool m_bPlotNoOwner;
};

