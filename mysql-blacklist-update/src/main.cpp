/*
 * main.cpp
 *
 *  Created on: Aug 20, 2014
 *      Author: Steve Mc Gregor <steve@smcgregor.info>
 */

#include "blacklist/BlacklistReader.h"
#include "blacklist/MySqlWriter.h"

#include <iostream>

int main(int argc, char *argv[])
{
	// EDITAR
	blacklist::BlacklistReader bl("/home/steve/squidblacklist/blacklists");

	blacklist::MySqlWriter db("localhost", "root", "root", "squidblacklist");
	db.clearLists();
	db.prepareInserts();

	std::map<std::string, std::string>::const_iterator it;

	for (it = bl.getCategories().begin(); it != bl.getCategories().end(); ++it)
	{
		db.insertCategory(it->first, it->second);
	}

	for (it = bl.getDomains().begin(); it != bl.getDomains().end(); ++it)
	{
		db.insertDomain(it->first, it->second);
	}

	for (it = bl.getUrls().begin(); it != bl.getUrls().end(); ++it)
	{
		db.insertUrl(it->first, it->second);
	}
}



