#pragma once

#include <string>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <filesystem>
#include <vector>
#include <map>

namespace fs = std::experimental::filesystem;
namespace Types
{
	struct comp
	{
		bool operator() (const size_t a, const size_t b)
		{
			return a > b;
		}
	};

	typedef std::multimap<size_t, std::string, comp>	WordMap;
	typedef std::map<std::string, WordMap >				Cache;
	typedef std::map<std::string, size_t>				FileVoc;
	typedef std::map<fs::path, time_t>	FilesData;
}
class Serializer
{
	//File struct:
	// Word
	// Count of files with this word
	// Count of this word in current file
	// Current file name
	// Count int next file
	// ...
	// Next word
	// ...


private:
	static time_t GetModDate(fs::path path)
	{
		struct stat result;
		stat(path.generic_string().c_str(), &result);

		return result.st_mtime;
	}
public:

	static void WriteFilesData(const fs::path& path)
	{
		const std::string file_path = path.generic_string() + "/files_data";
		std::vector<fs::path> files;
		for (auto & p : fs::directory_iterator(path))
		{
			if (p.path().has_extension())
			{
				files.push_back(p.path());
			}
		}
		std::ofstream fstream(file_path);

		for (size_t i = 0; i < files.size(); ++i)
		{
			fstream << files[i].filename().generic_string() + " " << GetModDate(files[i]) << "\n";
		}
		fstream.close();
	}
	static Types::FilesData GetFilesData(const fs::path& path)
	{
		const std::string file_path = path.generic_string() + "/files_data";
		std::vector<fs::path> files;
		Types::FilesData data;
		for (auto & p : fs::directory_iterator(path))
		{
			if (p.path().has_extension())
			{
				files.push_back(p.path());
			}
		}
		for (size_t i = 0; i < files.size(); ++i)
		{
			data.insert(std::make_pair(files[i].filename().generic_string(), GetModDate(files[i])));
		}
		return data;
	}
	static Types::FilesData ReadFilesData(const fs::path& path)
	{
		const std::string file_path = path.generic_string() + "/files_data";
		Types::FilesData data;
		std::ifstream fstream(file_path);
		if (!fstream.is_open())
			throw std::exception((file_path + "\nFile not opened.").c_str());

		while (!fstream.eof())
		{
			std::string filename;
			fstream >> filename;
			if (filename == "")
				continue;
			time_t time;
			fstream >> time;
			data.insert(std::make_pair(filename, time));
		}
		return data;
	}

	static void Serialize(const Types::Cache& cache, const fs::path& path)
	{
	    const std::string file_path =	path.generic_string() + "/cache";
		std::ofstream fstream(file_path);

		for (Types::Cache::const_iterator it = cache.begin();
			it != cache.end(); ++it)
		{
			fstream << it->first << " " << it->second.size() << "\n";
			for (Types::WordMap::const_iterator jt = it->second.begin();
				jt != it->second.end(); ++jt)
			{
				fstream << jt->first << " " << jt->second << "\n";
			}
		}

		std::cout << "Saved to file: " + file_path + "\n";
		fstream.close();
	}

	static Types::Cache Deserialize(fs::path path)
	{
		const std::string file_path = path.generic_string() + "/cache";
		Types::Cache result;
		std::ifstream fstream(file_path);
		if (!fstream.is_open())
			throw std::exception((file_path + "\nFile not opened.").c_str());

		while (!fstream.eof())
		{
			std::string word;
			fstream >> word;
			if (word == "")
				continue;
			size_t k;
			fstream >> k;
			Types::WordMap map;

			for (size_t i = 0; i < k; ++i)
			{
				size_t count;
				fstream >> count;
				std::string filename;
				fstream >> filename;

				map.insert(std::make_pair(count, filename));
			}

			result[word] = map;
		}

		return result;
	}

private:
	Serializer() {}
	~Serializer() {}
};
