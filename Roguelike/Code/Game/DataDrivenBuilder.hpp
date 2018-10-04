#pragma once
#include "Game/Stats.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "ThirdParty/IXMLParser/IXMLParser.h"
#include <string>
#include <map>

template <typename T>
class DataDrivenBuilder
{
public:
	std::string m_name;
	Stats m_stats;
	static std::map<std::string, T*> s_registry;

	DataDrivenBuilder();
	~DataDrivenBuilder();

	static void RegisterNewType(const ITCXMLNode& xmlNode);
	static void DestroyRegistry();
};

template <typename T>
std::map<std::string, T*> DataDrivenBuilder<T>::s_registry;

template <typename T>
DataDrivenBuilder<T>::DataDrivenBuilder()
{

}

template <typename T>
DataDrivenBuilder<T>::~DataDrivenBuilder()
{

}

template <typename T>
void DataDrivenBuilder<T>::RegisterNewType(const ITCXMLNode& xmlNode)
{
	std::string name = ParseXmlAttribute(xmlNode, "name", (std::string)"ERROR_NO_NAME_FOUND");
	s_registry[name] = new T(xmlNode);
}

template <typename T>
void DataDrivenBuilder<T>::DestroyRegistry()
{
	std::map<std::string, T*>::iterator builderIter;

	for (builderIter = s_registry.begin(); builderIter != s_registry.end(); ++builderIter)
	{
		delete builderIter->second;
		builderIter->second = nullptr;
	}
	s_registry.clear();
}