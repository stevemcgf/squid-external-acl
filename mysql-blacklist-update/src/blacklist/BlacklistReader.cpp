/*
 * BlacklistReader.cpp
 *
 *  Created on: Sep 11, 2014
 *      Author: steve
 */

#include "BlacklistReader.h"

#include <fstream>
#include <iostream>

namespace blacklist
{

const int MAX_STRING_LEN = 2048;

BlacklistReader::BlacklistReader(const std::string& basedir) :
		m_basedir(basedir)
{
	readCategories();
	std::cout << "Categorias: " << m_categories.size() << std::endl;
	readDomains();
	std::cout << "Dominios: " << m_domains.size() << std::endl;
	readUrls();
	std::cout << "Urls: " << m_urls.size() << std::endl;

}

BlacklistReader::~BlacklistReader()
{

}

void BlacklistReader::readCategories()
{
	std::string catfile = m_basedir + "/CATEGORIES";
	std::ifstream intxtCat;

	intxtCat.open(catfile.c_str());
	char str2[MAX_STRING_LEN];

	if (!intxtCat.fail())
	{
		while (!intxtCat.eof())
		{
			std::string str1;
			std::string aux;

			intxtCat >> str1;
			intxtCat >> aux;
			if (aux == "-")
			{
				intxtCat.getline(str2, MAX_STRING_LEN);
				m_categories.insert(
						std::pair<std::string, std::string>(str1, str2));
			}
			else
			{
				intxtCat.getline(str2, MAX_STRING_LEN);
			}
		}
	}
	else
	{
		std::cerr << "No se pudo abrir archivo de categorias" << std::endl;
	}
}

void BlacklistReader::readDomains()
{
	std::map<std::string, std::string>::iterator it = m_categories.begin();
	char str[MAX_STRING_LEN];

	while (it != m_categories.end())
	{
		std::string domfile = m_basedir + "/" + it->first + "/domains";
		std::ifstream intxtDom;

		intxtDom.open(domfile.c_str());

		if (!intxtDom.fail())
		{
			while (!intxtDom.eof())
			{
				intxtDom.getline(str, MAX_STRING_LEN);
				std::string name = str;
				if (name.length() > 0)
				{
					if (m_domains.find(name) == m_domains.end())
					{
						m_domains[name] = it->first;
					}
					else
					{
						m_domains[name] = m_domains[name] + "," + it->first;
					}
				}
			}
		}
		else
		{
			std::cerr << "No se encontro: " << domfile << std::endl;
		}
		++it;
	}
}

void BlacklistReader::readUrls()
{
	std::map<std::string, std::string>::iterator it = m_categories.begin();
	char str[MAX_STRING_LEN];

	while (it != m_categories.end())
	{
		std::string urlfile = m_basedir + "/" + it->first + "/urls";
		std::ifstream intxtUrl;

		intxtUrl.open(urlfile.c_str());

		if (!intxtUrl.fail())
		{
			while (!intxtUrl.eof())
			{
				intxtUrl.getline(str, MAX_STRING_LEN);
				std::string name = str;
				if (name.length() > 0)
				{
					if (m_urls.find(name) == m_urls.end())
					{
						m_urls[name] = it->first;
					}
					else
					{
						m_urls[name] = m_urls[name] + "," + it->first;
					}
				}
			}
		}
		++it;
	}
}

} /* namespace blacklist */
