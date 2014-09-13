/*
 * MySqlReader.h
 *
 *  Created on: Sep 12, 2014
 *      Author: steve
 */

#ifndef MYSQLREADER_H_
#define MYSQLREADER_H_

#include <string>
#include <unordered_set>
#include <unordered_map>
#include <mysql/mysql.h>

namespace blacklist
{

const int MAX_STRING_LEN = 2048;

class MySqlReader
{
public:
	MySqlReader(const std::string& dbhost, const std::string& dbuser,
			const std::string& dbpass, const std::string& dbname);
	virtual ~MySqlReader();

	void readBlockedCategories(
			std::unordered_set<std::string>& categories);

	void readCategories(
			std::unordered_map<std::string, std::string>& categories);

	void prepareQueries();

	bool queryWhitelistDomain(const std::string& domain);
	bool queryWhitelistUrl(const std::string& url);
	bool queryDomain(const std::string& domain, std::string& categories);
	bool queryUrl(const std::string& url, std::string& categories);

private:
	MYSQL * m_conn;
	MYSQL_STMT * m_stmtqueryDomain;
	MYSQL_STMT * m_stmtqueryUrl;
	MYSQL_STMT * m_stmtqueryWlDomain;
	MYSQL_STMT * m_stmtqueryWlUrl;

	char m_stringin[MAX_STRING_LEN];
	long unsigned int m_stringinlen;
	char m_stringout[MAX_STRING_LEN];
	long unsigned int m_stringoutlen;
	my_bool m_stringoutnul;
	my_bool m_stringouterr;

};

} /* namespace blacklist */

#endif /* MYSQLQUERY_H_ */
