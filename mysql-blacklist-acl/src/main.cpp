/*
 * main.cpp
 *
 *  Created on: Sep 12, 2014
 *      Author: steve
 */

#include <iostream>
#include "blacklist/MySqlReader.h"
#include <unordered_set>
#include <boost/algorithm/string.hpp>

/* Converts a hex character to its integer value */
char from_hex(char ch)
{
	return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
}

/* Converts an integer value to its hex character*/
char to_hex(char code)
{
	static char hex[] = "0123456789abcdef";
	return hex[code & 15];
}

/* Returns a url-encoded version of str */
/* IMPORTANT: be sure to free() the returned string after use */
void url_encode(const std::string& in, std::string& out)
{
	out.clear();
	out.reserve(in.size() * 3);

	std::size_t i = 0;
	while (i < in.size())
	{
		if (isalnum(in[i]) || in[i] == '-' || in[i] == '_' || in[i] == '.'
				|| in[i] == '~')
		{
			out += in[i];
		}
		else if (in[i] == ' ')
		{
			out += '+';
		}
		else
		{
			out += '%';
			out += to_hex(in[i] >> 4);
			out += to_hex(in[i] & 15);
		}
		++i;
	}
}

/* Returns a url-decoded version of str */
/* IMPORTANT: be sure to free() the returned string after use */
void url_decode(const std::string& in, std::string& out)
{
	out.clear();
	out.reserve(in.size());

	std::size_t i = 0;
	while (i < in.size())
	{
		if (in[i] == '%')
		{
			if (in[i + 1] && in[i + 2])
			{
				out += from_hex(in[i + 1]) << 4 | from_hex(in[i + 2]);
				i = i + 2;
			}
		}
		else if (in[i] == '+')
		{
			out += ' ';
		}
		else
		{
			out += in[i];
		}
		++i;
	}
}

int main(int argc, char *argv[])
{
	blacklist::MySqlReader db("192.168.0.1", "soporte", "UPronim123",
			"squidblacklist");

	std::unordered_set<std::string> blockcategories;
	db.readBlockedCategories(blockcategories);

	std::unordered_map<std::string, std::string> categories;
	db.readCategories(categories);

	std::unordered_map<std::string, std::string>::iterator it =
			categories.begin();
	while (it != categories.end())
	{
		if (blockcategories.find(it->first) == blockcategories.end())
		{
			it = categories.erase(it);
		}
		else
		{
			++it;
		}
	}

	db.prepareQueries();

	while (true)
	{
		bool retValue = false;
		std::string messageout;

		std::string domain_encoded;
		std::string url_encoded;
		std::cin >> domain_encoded >> url_encoded;

		std::string domain;
		std::string url;
		url_decode(domain_encoded, domain);
		url_decode(url_encoded, url);

		if (db.queryWhitelistDomain(domain) || db.queryWhitelistUrl(url))
		{
			retValue = true;
		}
		else
		{
			std::string categorylist;
			bool badDomain = false;

			while (domain.find_first_of('.') < domain.npos && !db.queryDomain(domain, categorylist))
			{
				domain = domain.substr(domain.find_first_of('.') + 1);
			}

			if (categorylist.size() > 0)
			{
				std::vector<std::string> catlist;
				boost::algorithm::split(catlist, categorylist, boost::is_any_of(","));
				for (std::size_t i = 0; i < catlist.size(); i++)
				{
					if (categories.find(catlist[i]) != categories.end())
					{
						messageout += "Dominio bloqueado por: \"" + catlist[i] + "\": " + categories[catlist[i]];
						badDomain = true;
						break;
					}
				}
			}

			bool badUrl = false;

			if (badDomain == false)
			{
				if (db.queryUrl(url, categorylist))
				{
					std::vector<std::string> catlist;
					boost::algorithm::split(catlist, categorylist, boost::is_any_of(","));
					for (std::size_t i = 0; i < catlist.size(); i++)
					{
						if (categories.find(catlist[i]) != categories.end())
						{
							messageout += "Url bloqueado por: \"" + catlist[i] + "\": " + categories[catlist[i]];
							badUrl = true;
							break;
						}
					}
				}
			}

			if (badDomain == false and badUrl == false)
			{
				retValue = true;
			}
		}

		if (retValue)
		{
			std::cout << "OK" << std::endl;
		}
		else
		{
			std::cout << "ERR";
			if (messageout.size() > 0)
			{
				std::string messageenc;
				url_encode(messageout, messageenc);
				std::cout << " message=\"" << messageenc;
				std::cout << std::endl;
			}
		}
	}
}

