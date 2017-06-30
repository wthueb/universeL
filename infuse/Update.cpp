#include "Update.h"

#include "Colors.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include <curl/curl.h>

#ifdef _DEBUG
#pragma comment(lib, "libcurl_a_debug.lib")
#else
#pragma comment(lib, "libcurl_a.lib")
#endif

#pragma comment(lib, "ws2_32.lib")

size_t WriteDataFile(void* contents, size_t size, size_t nmemb, FILE* stream) {
	size_t written = fwrite(contents, size, nmemb, stream);

	return written;
}

size_t WriteDataString(void* contents, size_t size, size_t nmemb, std::string* s)
{
	size_t newLength = size * nmemb;
	size_t oldLength = s->size();

	try
	{
		s->resize(oldLength + newLength);
	}
	catch (std::bad_alloc)
	{
		return 0;
	}

	std::copy(static_cast<char*>(contents), static_cast<char*>(contents) + newLength, s->begin() + oldLength);
	return size * nmemb;
}

bool GetLatestVersion(CURL* &curl, std::string &latestversion)
{
	cyan;
	std::cout << "getting latest version number...\n\n";

	std::string url = "http://wi1.us.to/latest-version.txt";
	
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteDataString);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &latestversion);

	CURLcode result = curl_easy_perform(curl);

	if (result != CURLE_OK)
	{
		curl_easy_cleanup(curl);
		return false;
	}

	latestversion.erase(std::remove(latestversion.begin(), latestversion.end(), '\n'), latestversion.end());

	return true;
}

bool GetCurrentVersion(std::string &currentversion)
{
	HANDLE hFind;
	WIN32_FIND_DATA data;

	char path[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, path);

	sprintf_s(path, "%s\\universeL*.dll", path);

	hFind = FindFirstFileA(path, &data);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		currentversion = data.cFileName;

		do
		{
			if (currentversion.compare(data.cFileName) < 0)
				currentversion = data.cFileName;
		} while (FindNextFileA(hFind, &data));

		FindClose(hFind);
	}
	else
		return false;

	return true;
}

bool DownloadVersion(CURL* &curl, std::string version)
{
	std::string url = "http://wi1.us.to/universeL-downloads/";

	url.append(version);

	cyan;
	std::cout << "downloading latest version from " << url << "...\n\n";

	char outfilepath[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, outfilepath);
	sprintf_s(outfilepath, "%s\\%s", outfilepath, version.c_str());

	FILE* outfile;
	fopen_s(&outfile, outfilepath, "wb");

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteDataFile);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, outfile);

	CURLcode result = curl_easy_perform(curl);

	if (result != CURLE_OK)
	{
		curl_easy_cleanup(curl);
		fclose(outfile);
		return false;
	}

	fclose(outfile);

	green;
	std::cout << "latest version successfully downloaded!\n\n";

	return true;
}

void DeleteOldVersions(std::string latestversion)
{
	HANDLE hFind;
	WIN32_FIND_DATA data;

	char workingpath[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, workingpath);

	char findfiles[MAX_PATH];

	sprintf_s(findfiles, "%s\\universeL*.dll", workingpath);

	std::vector<std::string> olddlls; // contains file names (not full paths)

	hFind = FindFirstFileA(findfiles, &data);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (latestversion.compare(data.cFileName)) // if data.cFileName is not the latest version
				olddlls.push_back(std::string(data.cFileName));
		} while (FindNextFileA(hFind, &data));

		FindClose(hFind);
	}

	if (olddlls.size() <= 0)
		return;

	cyan;
	std::cout << "deleting old versions...\n\n";

	for (size_t i = 0; i < olddlls.size(); ++i)
	{
		cyan;
		 std::cout << "- deleting " << olddlls.at(i) << "...\n";

		 char filepath[MAX_PATH];
		 sprintf_s(filepath, "%s\\%s", workingpath, olddlls.at(i).c_str());

		 if (!remove(filepath))
		 {
			 green;
			 std::cout << "- " << olddlls.at(i) << " successfully deleted\n\n";
		 }
		 else
		 {
			 red;
			 std::cout << "- " << olddlls.at(i) << " could not be deleted...\n\n";
		 }
	}
}

bool Update()
{
	CURL* curl = curl_easy_init();
	if (!curl)
		return false;

	std::string latestversion;
	
	if (!GetLatestVersion(curl, latestversion))
		return false;

	cyan;
	std::cout << "latest version: " << latestversion << "\n\n";
	
	std::string currentversion;

	if (GetCurrentVersion(currentversion))
	{
		cyan;
		std::cout << "current version: " << currentversion << "\n\n";

		if (!currentversion.compare("universeL.dll") || currentversion.compare(latestversion)) // if versions are different
		{
			red;
			std::cout << "current version is out of date!\n\n";
			if (!DownloadVersion(curl, latestversion))
				return false;
		}
		else
		{
			green;
			std::cout << "your version is up to date!\n\n";
		}
	}
	else
	{
		red;
		std::cout << "current version could not be found...\n\n";
		if (!DownloadVersion(curl, latestversion))
			return false;
	}

	DeleteOldVersions(latestversion);

	curl_easy_cleanup(curl);

	return true;
}
