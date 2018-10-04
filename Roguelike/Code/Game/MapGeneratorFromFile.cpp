#include "Game/MapGeneratorFromFile.hpp"
#include "Game/Map.hpp"
#include "Game/TileDefinition.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "ThirdParty/IXMLParser/IXMLParser.h"

MapGeneratorFromFile::MapGeneratorFromFile(const ITCXMLNode& xmlNode)
{
	ValidateXmlElement(xmlNode, "Key", "name,filePath,offsetX,offsetY,isMirrored,rotation");
	m_name = ParseXmlAttribute(xmlNode, "name", (std::string)"ERROR_NO_NAME_FOUND");
	m_filePath = ParseXmlAttribute(xmlNode, "filePath", (std::string)"ERROR_NO_FILEPATH_FOUND");
	m_offsetX = ParseXmlAttribute(xmlNode, "offsetX", IntRange(0,0));
	m_offsetY = ParseXmlAttribute(xmlNode, "offsetY", IntRange(0,0));
	m_isMirrored = ParseXmlAttribute(xmlNode, "isMirrored", false);
	m_numRightAngleRotations = ParseXmlAttribute(xmlNode, "rotation", IntRange(0,0));

	LoadKeys(xmlNode);
}

MapGeneratorFromFile::~MapGeneratorFromFile()
{

}

void MapGeneratorFromFile::PopulateMap(Map *mapToPopulate)
{ 
	IXMLDomParser iDom;
	iDom.setRemoveClears(false);
	const char* filePathChar = m_filePath.c_str();
	ITCXMLNode xMainNode = iDom.openFileHelper(filePathChar, "Map");
	std::vector<std::string> fileMap;

	const char* mapCharacters = xMainNode.getText(0);
	std::string fileMapLine = "";
	while (*mapCharacters != '\0')
	{
		if (*mapCharacters != '\n' && *mapCharacters != '\t')
		{
			fileMapLine += *mapCharacters;
		}
		else if (*mapCharacters == '\n')
		{
			fileMap.push_back(fileMapLine);
			fileMapLine = "";
		}
		*mapCharacters++;
	}
	fileMap.push_back(fileMapLine);

	int heightOfFileMap = (int)fileMap.size();
	IntVector2 startPos = IntVector2(m_offsetX.GetIntInRange(), m_offsetY.GetIntInRange() + heightOfFileMap - 1);

	int rotation = m_numRightAngleRotations.GetIntInRange();

	int minStringLength = INT_MAX;
	for (std::vector<std::string>::iterator fileMapIter = fileMap.begin(); fileMapIter != fileMap.end(); ++fileMapIter)
	{
		if ((int)(*fileMapIter).length() < minStringLength)
			minStringLength = (int)(*fileMapIter).length();
	}

	int currentY = 0;
	for (std::vector<std::string>::iterator fileMapIter = fileMap.begin(); fileMapIter != fileMap.end(); ++fileMapIter)
	{
		mapCharacters = (*fileMapIter).c_str();
		int stringLength = (int)(*fileMapIter).length();

		if (!m_isMirrored)
		{
			int currentX = 0;
			for (int characterCount = 0; characterCount < stringLength; characterCount++)
			{
				if (mapCharacters[characterCount] != ' ' && mapCharacters[characterCount] != '\0')
				{
					IntVector2 tilePos = GetRotatedPosition(rotation, IntVector2(startPos.x + currentX, startPos.y + currentY));
					Tile *currentTile = mapToPopulate->GetTileAtPosition(tilePos);
					if (currentTile != nullptr)
						currentTile->SetTileDefinition(m_keys[mapCharacters[characterCount]]);
				}
				currentX++;
			}
		}
		else 
		{
			int currentX = stringLength;
			for (int characterCount = stringLength; characterCount >= 0; characterCount--)
			{
				if (mapCharacters[characterCount] != ' ' && mapCharacters[characterCount] != '\0')
				{
					IntVector2 tilePos = GetRotatedPosition(rotation, IntVector2(startPos.x - currentX + minStringLength, startPos.y + currentY));
					Tile *currentTile = mapToPopulate->GetTileAtPosition(IntVector2(tilePos));
					if (currentTile != nullptr)
						currentTile->SetTileDefinition(m_keys[mapCharacters[characterCount]]);
				}
				currentX--;
			}
		}

		currentY--;
	}

}

void MapGeneratorFromFile::LoadKeys(const ITCXMLNode& xmlNode)
{
	for (int keyCount = 0; keyCount < xmlNode.nChildNode(); keyCount++)
	{
		ITCXMLNode keyNode = xmlNode.getChildNode(keyCount);
		ValidateXmlElement(keyNode, "", "symbol,tile");
		std::string keySymbol = ParseXmlAttribute(keyNode, "symbol", (std::string)"ERROR_NO_SYMBOL_FOUND");
		std::string keyTileName = ParseXmlAttribute(keyNode, "tile", (std::string)"ERROR_NO_TILE_FOUND");
		m_keys[*keySymbol.c_str()] = TileDefinition::s_registry[keyTileName];
	}
}

IntVector2 MapGeneratorFromFile::GetRotatedPosition(int numRotation, const IntVector2& position)
{
	IntVector2 newPosition;
	switch (numRotation)
	{
	case 0:
		newPosition = position;
		break;
	case 1:
		newPosition.x = position.y;
		newPosition.y = position.x;
		break;
	case 2:
		newPosition.x = -position.x;
		newPosition.y = -position.y;
		break;
	case 3:
		newPosition.x = -position.y;
		newPosition.y = position.x;
		break;
	}

	return newPosition;
}

