/*
 * MySqlController.h
 *
 *  Created on: Sep 11, 2014
 *      Author: steve
 */

#ifndef MYSQLCONTROLLER_H_
#define MYSQLCONTROLLER_H_

#include <string>
#include <mysql/mysql.h>

namespace blacklist
{
namespace mysql
{

const int MAX_STRING_LEN = 2048;

class MySqlController
{
public:
	MySqlController(const std::string& dbhost, const std::string& dbuser,
			const std::string& dbpass, const std::string& dbname);
	virtual ~MySqlController();

	void clearLists();

	void prepareInserts();
	void insertCategory(const std::string& name, const std::string& description);
	void insertDomain(const std::string& name, const std::string& categories);
	void insertUrl(const std::string& name, const std::string& categories);
private:
	MYSQL * m_conn;
	MYSQL_STMT * m_stmtinsCategory;
	MYSQL_STMT * m_stmtinsDomain;
	MYSQL_STMT * m_stmtinsUrl;

	MYSQL_STMT * m_stmtlookCategory;
	MYSQL_STMT * m_stmtlookDomain;
	MYSQL_STMT * m_stmtlookUrl;

	char m_string1[MAX_STRING_LEN];
	long unsigned int m_stringlen1;
	char m_string2[MAX_STRING_LEN];
	long unsigned int m_stringlen2;
};

} /* namespace mysql */
} /* namespace blacklist */

#endif /* MYSQLCONTROLLER_H_ */
