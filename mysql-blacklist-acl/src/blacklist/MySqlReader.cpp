/*
 * MySqlReader.cpp
 *
 *  Created on: Sep 12, 2014
 *      Author: steve
 */

#include "MySqlReader.h"

#include <iostream>
#include <cstring>

namespace blacklist
{

MySqlReader::MySqlReader(const std::string& dbhost, const std::string& dbuser,
		const std::string& dbpass, const std::string& dbname)
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
		std::cerr << "mysql_real_connect error: " << mysql_error(m_conn)
				<< std::endl;
		throw std::exception();
	}

	m_stringinlen = 0;
	m_stringoutlen = 0;
	m_stringoutnul = false;

	m_stmtqueryDomain = NULL;
	m_stmtqueryUrl = NULL;
	m_stmtqueryWlDomain = NULL;
	m_stmtqueryWlUrl = NULL;

}

MySqlReader::~MySqlReader()
{
	if (m_conn)
	{
		mysql_close(m_conn);
	}

	if (m_stmtqueryDomain)
	{
		mysql_stmt_close(m_stmtqueryDomain);
		mysql_stmt_close(m_stmtqueryUrl);
		mysql_stmt_close(m_stmtqueryWlDomain);
		mysql_stmt_close(m_stmtqueryWlUrl);

	}
}

void MySqlReader::readBlockedCategories(
		std::unordered_set<std::string>& categories)
{
	if (mysql_query(m_conn, "SELECT name FROM block_category") == 0)
	{
		MYSQL_RES * res = mysql_store_result(m_conn);
		if (res)
		{
			MYSQL_ROW row;
			while ((row = mysql_fetch_row(res)))
			{
				categories.insert(std::string(row[0]));
			}
		}
	}

}

void MySqlReader::readCategories(
		std::unordered_map<std::string, std::string>& categories)
{
	if (mysql_query(m_conn, "SELECT name, description FROM category") == 0)
	{
		MYSQL_RES * res = mysql_store_result(m_conn);
		if (res)
		{
			MYSQL_ROW row;
			while ((row = mysql_fetch_row(res)))
			{
				categories.insert(
						std::pair<std::string, std::string>(std::string(row[0]),
								std::string(row[1])));
			}
		}
	}
}

void MySqlReader::prepareQueries()
{
	// BIND MYSQL
	MYSQL_BIND bindin[1];
	memset(bindin, 0, sizeof(bindin));
	bindin[0].buffer_type = MYSQL_TYPE_STRING;
	bindin[0].buffer = m_stringin;
	bindin[0].buffer_length = MAX_STRING_LEN;
	bindin[0].is_null = 0;
	bindin[0].length = &m_stringinlen;

	MYSQL_BIND bindout[1];
	memset(bindout, 0, sizeof(bindout));
	bindout[0].buffer_type = MYSQL_TYPE_STRING;
	bindout[0].buffer = m_stringout;
	bindout[0].buffer_length = MAX_STRING_LEN;
	bindout[0].is_null = &m_stringoutnul;
	bindout[0].length = &m_stringoutlen;
	bindout[0].error = &m_stringouterr;

	// Domain
	m_stmtqueryDomain = mysql_stmt_init(m_conn);
	const std::string strQueryDomain =
			"SELECT categories FROM domain WHERE name = ?";

	if (!m_stmtqueryDomain)
	{
		std::cerr << "mysql_stmt_init error: " << mysql_error(m_conn)
				<< std::endl;
		throw std::exception();
	}

	if (mysql_stmt_prepare(m_stmtqueryDomain, strQueryDomain.c_str(),
			strQueryDomain.length()))
	{
		std::cerr << "mysql_stmt_prepare error: " << mysql_error(m_conn)
				<< std::endl;
	}

	mysql_stmt_bind_param(m_stmtqueryDomain, bindin);
	mysql_stmt_bind_result(m_stmtqueryDomain, bindout);

	// URL
	m_stmtqueryUrl = mysql_stmt_init(m_conn);
	const std::string strQueryUrl = "SELECT categories FROM url WHERE name = ?";

	if (!m_stmtqueryUrl)
	{
		std::cerr << "mysql_stmt_init error: " << mysql_error(m_conn)
				<< std::endl;
		throw std::exception();
	}

	if (mysql_stmt_prepare(m_stmtqueryUrl, strQueryUrl.c_str(),
			strQueryUrl.length()))
	{
		std::cerr << "mysql_stmt_prepare error: " << mysql_error(m_conn)
				<< std::endl;
	}

	mysql_stmt_bind_param(m_stmtqueryUrl, bindin);
	mysql_stmt_bind_result(m_stmtqueryUrl, bindout);

	// WhitelistDomain
	m_stmtqueryWlDomain = mysql_stmt_init(m_conn);
	const std::string strQueryWlDomain =
			"SELECT id FROM whitelist_domain WHERE name = ?";

	if (!m_stmtqueryWlDomain)
	{
		std::cerr << "mysql_stmt_init error: " << mysql_error(m_conn)
				<< std::endl;
		throw std::exception();
	}

	if (mysql_stmt_prepare(m_stmtqueryWlDomain, strQueryWlDomain.c_str(),
			strQueryWlDomain.length()))
	{
		std::cerr << "mysql_stmt_prepare error: " << mysql_error(m_conn)
				<< std::endl;
	}

	mysql_stmt_bind_param(m_stmtqueryWlDomain, bindin);
	mysql_stmt_bind_result(m_stmtqueryWlDomain, bindout);

	// WhitelistUrl
	m_stmtqueryWlUrl = mysql_stmt_init(m_conn);
	const std::string strQueryWlUrl =
			"SELECT id FROM whitelist_url WHERE name = ?";

	if (!m_stmtqueryWlUrl)
	{
		std::cerr << "mysql_stmt_init error: " << mysql_error(m_conn)
				<< std::endl;
		throw std::exception();
	}

	if (mysql_stmt_prepare(m_stmtqueryWlUrl, strQueryWlUrl.c_str(),
			strQueryWlUrl.length()))
	{
		std::cerr << "mysql_stmt_prepare error: " << mysql_error(m_conn)
				<< std::endl;
	}

	mysql_stmt_bind_param(m_stmtqueryWlUrl, bindin);
	mysql_stmt_bind_result(m_stmtqueryWlUrl, bindout);

}

bool MySqlReader::queryDomain(const std::string& domain,
		std::string& categories)
{
	bool ret = false;

	std::strncpy(m_stringin, domain.c_str(), MAX_STRING_LEN - 1);
	m_stringinlen = domain.length();

	if (mysql_stmt_execute(m_stmtqueryDomain) == 0)
	{
		if (mysql_stmt_fetch(m_stmtqueryDomain) == 0)
		{
			categories = m_stringout;
			ret = true;
			mysql_stmt_free_result(m_stmtqueryDomain);
		}
	}

	std::cerr << "queryDomain domain=" << domain << " categories=" << categories << std::endl;
	return ret;
}

bool MySqlReader::queryUrl(const std::string& url, std::string& categories)
{
	bool ret = false;

	std::strncpy(m_stringin, url.c_str(), MAX_STRING_LEN - 1);
	m_stringinlen = url.length();

	if (mysql_stmt_execute(m_stmtqueryUrl) == 0)
	{
		if (mysql_stmt_fetch(m_stmtqueryUrl) == 0)
		{
			categories = m_stringout;
			ret = true;
		}
	}

	return ret;
}

bool MySqlReader::queryWhitelistDomain(const std::string& domain)
{
	bool ret = false;

	std::strncpy(m_stringin, domain.c_str(), MAX_STRING_LEN - 1);
	m_stringinlen = domain.length();

	if (mysql_stmt_execute(m_stmtqueryWlDomain) == 0)
	{
		if (mysql_stmt_fetch(m_stmtqueryWlDomain) == 0)
		{
			ret = true;
		}
	}

	return ret;
}

bool MySqlReader::queryWhitelistUrl(const std::string& url)
{
	bool ret = false;

	std::strncpy(m_stringin, url.c_str(), MAX_STRING_LEN - 1);
	m_stringinlen = url.length();

	if (mysql_stmt_execute(m_stmtqueryWlUrl) == 0)
	{
		if (mysql_stmt_fetch(m_stmtqueryWlUrl) == 0)
		{
			ret = true;
		}
	}

	return ret;
}

} /* namespace blacklist */
