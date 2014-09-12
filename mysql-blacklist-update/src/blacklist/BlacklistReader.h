/*
 * BlacklistReader.h
 *
 *  Created on: Sep 11, 2014
 *      Author: steve
 */

#ifndef BLACKLISTREADER_H_
#define BLACKLISTREADER_H_

#include <map>

namespace blacklist
{

class BlacklistReader
{
public:
	BlacklistReader(const std::string& basedir);
	virtual ~BlacklistReader();

	const std::map<std::string, std::string>& getCategories();
	const std::map<std::string, std::string>& getDomains();
	const std::map<std::string, std::string>& getUrls();

private:
	std::map<std::string, std::string> m_categories;
	std::map<std::string, std::string> m_domains;
	std::map<std::string, std::string> m_urls;

	std::string m_basedir;

	void readCategories();
	void readDomains();
	void readUrls();
};

inline const std::map<std::string, std::string>& BlacklistReader::getCategories()
{
	return m_categories;
}

inline const std::map<std::string, std::string>& BlacklistReader::getDomains()
{
	return m_domains;
}

inline const std::map<std::string, std::string>& BlacklistReader::getUrls()
{
	return m_urls;
}

} /* namespace blacklist */

#endif /* BLACKLISTREADER_H_ */
