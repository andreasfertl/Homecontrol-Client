#ifndef _XMLPARSER_INCLUDED
#define _XMLPARSER_INCLUDED

#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>



class XMLParser
{
	private:
		int GetConfigurationFromXML(boost::property_tree::ptree &pt, class Configuration &Conf);
		unsigned int GetAllSubKeysOf(std::vector<std::string> &FoundSubkeys, std::string Str, boost::property_tree::ptree::value_type const& v);
		
		
	public:
		XMLParser();
		~XMLParser();
		int ReadFile(std::string FileName, class Configuration &Conf); //read the XML file to Configuration
};



#endif //_XMLPARSER_INCLUDED