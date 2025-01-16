from CvPythonExtensions import *
import CvUtil
import ScreenInput
import CvScreenEnums

import DomesticAdvisorTable
import BaseAdvisorWindow


# globals
gc = CyGlobalContext()
ArtFileMgr = CyArtFileMgr()
localText = CyTranslator()

class TradeCountAdvisor(BaseAdvisorWindow.BaseAdvisorWindow):
	def __init__(self, parent):
		BaseAdvisorWindow.BaseAdvisorWindow.__init__(self, parent, "TradeCountStateClass")

	def drawColonyCell(self, iRow, pCity, iYield, pYieldInfo):
		player = gc.getPlayer(gc.getGame().getActivePlayer())
		
		iCount = 0
		
		if iRow == 0:
			iCount = player.getYieldTradedTotalEurope(iYield)
		elif iRow == 1:
			iCount = player.getYieldTradedTotalAfrica(iYield)
		elif iRow == 2:
			iCount = player.getYieldTradedTotalPortRoyal(iYield)
		
		self.tableManager.addInt(iCount, iCount, -1, WidgetTypes.WIDGET_HELP_DOMESTIC_TRADED_COUNT)

	def getWidgetHelp(self, argsList):
		iScreen, eWidgetType, iData1, iData2, bOption = argsList
		
		if eWidgetType == WidgetTypes.WIDGET_HELP_DOMESTIC_TRADED_COUNT:
			return unicode(iData1)
		
		return None

	def getRowName(self, iRow):
		if iRow == 0:
			return localText.getText("TXT_KEY_CONCEPT_EUROPE", ())
		elif iRow == 1:
			return localText.getText("TXT_KEY_CONCEPT_AFRICA", ())
		elif iRow == 2:
			return localText.getText("TXT_KEY_CONCEPT_PORT_ROYAL", ())

	def drawEachColonyLine(self):
		self.tableManager.clearRows()
		iNumRows = 3
		
		self.tableManager.setNumRows(iNumRows)
		for iRow in range(iNumRows):
			self.tableManager.skipCell()
			self.tableManager.addText(self.getRowName(iRow))
			self.drawColonyRow(iRow, None)

	def createTableHeader(self):
		# create table headers
		self.tableManager.addHeaderTxt("", self.tableManager.MainAdvisor.ROW_HIGHT)
		self.tableManager.addHeaderTxt("", self.tableManager.MainAdvisor.CITY_NAME_COLUMN_WIDTH)
		self.tableManager.addHeaderArrayYields()
