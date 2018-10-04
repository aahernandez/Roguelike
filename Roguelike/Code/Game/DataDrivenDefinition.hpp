#pragma once
#include "ThirdParty/IXMLParser/IXMLParser.h"
#include "Engine/Core/FileUtils.hpp"
#include <string>
#include <map>

template <typename T>
class DataDrivenDefinition
{
public:
	std::string m_name;
	static std::map<std::string, T*> s_registry;

	DataDrivenDefinition();
	~DataDrivenDefinition();

	static void RegisterNewType(const ITCXMLNode& xmlNode);
	static void DestroyRegistry();
};

template <typename T>
std::map<std::string, T*> DataDrivenDefinition<T>::s_registry;

template <typename T>
DataDrivenDefinition<T>::DataDrivenDefinition() {}

template <typename T>
DataDrivenDefinition<T>::~DataDrivenDefinition() {}

template <typename T>
void DataDrivenDefinition<T>::RegisterNewType(const ITCXMLNode& xmlNode)
{
	std::string name = ParseXmlAttribute(xmlNode, "name", (std::string)"ERROR_NO_NAME_FOUND");
	s_registry[name] = new T(xmlNode);
}

template <typename T>
void DataDrivenDefinition<T>::DestroyRegistry()
{
	std::map<std::string, T*>::iterator definitionIter;

	for (definitionIter = s_registry.begin(); definitionIter != s_registry.end(); ++definitionIter)
	{
		delete definitionIter->second;
		definitionIter->second = nullptr;
	}
	s_registry.clear();
}