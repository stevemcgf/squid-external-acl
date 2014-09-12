/*
 * MySqlController.cpp
 *
 *  Created on: Sep 11, 2014
 *      Author: steve
 */

#include "MySqlController.h"
#include <iostream>
#include <exception>
#include <cstring>

namespace blacklist
{
namespace mysql
{

MySqlController::MySqlController(const std::string& dbhost,
		const std::string& dbuser, const std::string& dbpass,
		const std::string& dbname)
{
	m_conn = mysql_init(NULL);
	if (!m_conn)
	{
		std::cerr << "mysql_init error: " << mysql_error(m_conn) << std::endl;
		throw std::exception();
	}

	if (mysql_real_connect(m_conn, dbhost.c_str(), dbuser.c_str(),
			dbpass.c_str(), dbname.c_str(), 0, NULL, 0) == NULL)
	{
		std::cerr << "mysql_real_connect error: " << mysql_error(m_conn) << std::endl;
		throw std::exception();
	}

	m_stringlen1 = 0;
	m_stringlen2 = 0;

	m_stmtinsCategory = NULL;
	m_stmtinsDomain = NULL;
	m_stmtinsUrl = NULL;

	m_stmtlookCategory = NULL;
	m_stmtlookDomain = NULL;
	m_stmtlookUrl = NULL;
}

MySqlController::~MySqlController()
{
	if (m_conn)
	{
		mysql_close(m_conn);
	}

	if (m_stmtinsCategory)
	{
		mysql_stmt_close(m_stmtinsCategory);
		mysql_stmt_close(m_stmtinsDomain);
		mysql_stmt_close(m_stmtinsUrl);
	}

	if (m_stmtlookCategory)
	{
		mysql_stmt_close(m_stmtlookCategory);
		mysql_stmt_close(m_stmtlookDomain);
		mysql_stmt_close(m_stmtlookUrl);
	}
}

void MySqlController::clearLists()
{
	mysql_query(m_conn, "DELETE FROM category");
	mysql_query(m_conn, "DELETE FROM domain");
	mysql_query(m_conn, "ALTER TABLE domain AUTO_INCREMENT = 1");
	mysql_query(m_conn, "DELETE FROM url");
	mysql_query(m_conn, "ALTER TABLE url AUTO_INCREMENT = 1");
}

void MySqlController::prepareInserts()
{
	// BIND MYSQL
	MYSQL_BIND bind[2];
	bind[0].buffer_type = MYSQL_TYPE_STRING;
	bind[0].buffer = m_string1;
	bind[0].buffer_length = MAX_STRING_LEN;
	bind[0].is_null = 0;
	bind[0].length = &m_stringlen1;

	bind[1].buffer_type = MYSQL_TYPE_STRING;
	bind[1].buffer = m_string2;
	bind[1].buffer_length = MAX_STRING_LEN;
	bind[1].is_null = 0;
	bind[1].length = &m_stringlen2;

	// CATEGORY
	m_stmtinsCategory = mysql_stmt_init(m_conn);
	const std::string strInsCategory =
			"INSERT INTO category (name, description) VALUES (?,?)";

	if (!m_stmtinsCategory)
	{
		std::cerr << "mysql_stmt_init error: " << mysql_error(m_conn) << std::endl;
		throw std::exception();
	}

	if (mysql_stmt_prepare(m_stmtinsCategory, strInsCategory.c_str(),
			strInsCategory.length()))
	{
		std::cerr << "mysql_stmt_prepare error: " << mysql_error(m_conn) << std::endl;
	}

	mysql_stmt_bind_param(m_stmtinsCategory, bind);

	// DOMAIN
	m_stmtinsDomain = mysql_stmt_init(m_conn);
	const std::string strInsDomain =
			"INSERT INTO domain (name, categories) VALUES (?,?)";

	if (!m_stmtinsDomain)
	{
		std::cerr << "mysql_stmt_init error: " << mysql_error(m_conn) << std::endl;
		throw std::exception();
	}

	if (mysql_stmt_prepare(m_stmtinsDomain, strInsDomain.c_str(),
			strInsDomain.length()))
	{
		std::cerr << "mysql_stmt_prepare error: " << mysql_error(m_conn) << std::endl;
	}

	mysql_stmt_bind_param(m_stmtinsDomain, bind);

	// URL
	m_stmtinsUrl = mysql_stmt_init(m_conn);
	const std::string strInsUrl =
			"INSERT INTO url (name, categories) VALUES (?,?)";

	if (!m_stmtinsUrl)
	{
		std::cerr << "mysql_stmt_init error: " << mysql_error(m_conn) << std::endl;
		throw std::exception();
	}

	if (mysql_stmt_prepare(m_stmtinsUrl, strInsUrl.c_str(),
			strInsUrl.length()))
	{
		std::cerr << "mysql_stmt_prepare error: " << mysql_error(m_conn) << std::endl;
	}

	mysql_stmt_bind_param(m_stmtinsUrl, bind);

}

void MySqlController::insertCategory(const std::string& name,
		const std::string& description)
{
	std::strncpy(m_string1, name.c_str(), MAX_STRING_LEN - 1);
	m_stringlen1 = name.length();
	std::strncpy(m_string2, description.c_str(), MAX_STRING_LEN - 1);
	m_stringlen2 = description.length();

	if (mysql_stmt_execute(m_stmtinsCategory))
	{
		std::cerr << "mysql_stmt_execute error: " << mysql_error(m_conn) << std::endl;
	}
}

void MySqlController::insertDomain(const std::string& name,
		const std::string& categories)
{
	std::strncpy(m_string1, name.c_str(), MAX_STRING_LEN - 1);
	m_stringlen1 = name.length();
	std::strncpy(m_string2, categories.c_str(), MAX_STRING_LEN - 1);
	m_stringlen2 = categories.length();

	if (mysql_stmt_execute(m_stmtinsDomain))
	{
		std::cerr << "mysql_stmt_execute error: " << mysql_error(m_conn) << std::endl;
	}
}

void MySqlController::insertUrl(const std::string& name,
		const std::string& categories)
{
	std::strncpy(m_string1, name.c_str(), MAX_STRING_LEN - 1);
	m_stringlen1 = name.length();
	std::strncpy(m_string2, categories.c_str(), MAX_STRING_LEN - 1);
	m_stringlen2 = categories.length();

	if (mysql_stmt_execute(m_stmtinsUrl))
	{
		std::cerr << "mysql_stmt_execute error: " << mysql_error(m_conn) << std::endl;
	}
}

} /* namespace mysql */
} /* namespace blacklist */
