#include "CashSerializer.h"

#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <map>
#include <thread>
#include <sstream>

#include <windows.h>
#include <sys/types.h>
#include <sys/stat.h>

#define Error(string) throw std::exception(string)

namespace fs = std::experimental::filesystem;

const bool DEBUG = false;
HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

void GreenMassege(const std::string& txt)
{
	SetConsoleTextAttribute(console, 10);
	std::cout << txt;
	SetConsoleTextAttribute(console, 15);
}
void RedMassege(const std::string& txt)
{
	SetConsoleTextAttribute(console, 12);
	std::cout << txt;
	SetConsoleTextAttribute(console, 15);
}

void DebugCache(const Types::Cache& vocab)
{
	if (DEBUG)
	{
		for (Types::Cache::const_iterator it = vocab.begin();
			it != vocab.end(); ++it)
		{
			std::cout << it->first << "\n";

			for (std::multimap <size_t, std::string>::const_iterator jt = it->second.begin();
				jt != it->second.end(); ++jt)
			{
				std::cout << jt->first << " " << jt->second << "\n";
			}
		}
	}
}

void FormatStr(std::string& str)
{
	char chars[] = "()!@#$%^&-*+=_\'\"|\\?/><,.:;{}[]";

	for (unsigned int i = 0; i < strlen(chars); ++i)
	{
		str.erase(std::remove(str.begin(), str.end(), chars[i]), str.end());
	}

	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

void Handler(size_t& counter, const size_t& size)
{
	while (true)
	{
		if (counter == 0)
		{
			return;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		size_t percents = 0;
		const size_t newp = counter * 100 / size;
		if (newp != percents)
		{
			percents = newp;
			std::cout << "\r" << percents << "%";
		}
	}
}

Types::FileVoc GetFileCash(const fs::path& path)
{
	std::cout << "Indexing file " + path.filename().generic_string() + "\n";
	std::ifstream file(path.generic_string());
	if (!file.is_open())
		Error("Cannot open file.");

	size_t fsize = fs::file_size(path);
	size_t counter = 1;

	Types::FileVoc voc;
	std::thread bg(Handler, std::ref(counter), std::ref(fsize));
	
	while (!file.eof())
	{
		std::string word;
		file >> word;
		counter += word.size() + 1;

		FormatStr(word);
		if (word == "")
			continue;
		voc[word] = voc.find(word) == voc.end() ? 1 : ++voc[word];
	}
	counter = 0;
	bg.join();

	file.close();
	GreenMassege("\rOK  \n");
	return voc;
}

void AddFileToVocab(
	Types::Cache& vocab,
	const fs::path& filepath,
	const Types::FileVoc& file_vocab)
{
	const std::string fstr = filepath.filename().generic_string();
	for (Types::FileVoc::const_iterator it = file_vocab.begin();
		it != file_vocab.end(); ++it)
	{
		if (vocab.find(it->first) == vocab.end())
		{
			Types::WordMap newMap;
			newMap.insert(std::make_pair(it->second, fstr));
			vocab[it->first] = newMap;
		}
		else
		{
			vocab[it->first].insert(std::make_pair(it->second, fstr));
		}
	}
}

Types::Cache Index(const fs::path& path)
{
	std::vector<fs::path> files;
	for (auto & p : fs::directory_iterator(path))
	{
		if (p.path().has_extension())
		{
			files.push_back(p.path());
		}
	}
	Types::Cache vocab;
	for (size_t i = 0; i < files.size(); ++i)
	{
		const auto file_vocab = GetFileCash(files[i]);
		AddFileToVocab(vocab, files[i], file_vocab);
	}
	std::cout << "Saving...\n";
	Serializer::Serialize(vocab, path);
	Serializer::WriteFilesData(path);

	DebugCache(vocab);
	return vocab;
}

bool CheckFiles(const fs::path& path)
{
	return Serializer::ReadFilesData(path) !=
		Serializer::GetFilesData(path);
}

void Load(const fs::path& path, Types::Cache& cache)
{
	std::cout << "Loading...\n";
	if (CheckFiles(path))
	{
		std::cout << "Some files was chenged.\nDo you want to reindex?\n[y/n]\n";
		while (true)
		{
			std::string ansver;
			std::cin >> ansver;
			if (ansver == "y")
			{
				cache = Index(path);
				break;
			}
			else if (ansver == "n")
				break;
			else
			{
				std::cout << "[y/n]?\n";
			}
		}
	}
	else
	{
		cache = Serializer::Deserialize(path);
		DebugCache(cache);
	}
	GreenMassege("Done\n");
}

Types::WordMap::const_iterator GetCount4File(
	const Types::WordMap& wordmap, const std::string& word)
{
	Types::WordMap::const_iterator it = wordmap.cbegin();

	for (; it != wordmap.end(); ++it)
		if (it->second == word)
			break;

	return it;
}

Types::Tout GetOut(
	std::vector<std::string>& words,
	Types::Cache& cache)
{
	std::vector<Types::WordMap> found;
	Types::Tout result;
	for (size_t i = 0; i < words.size(); ++i)
	{
		FormatStr(words[i]);
		Types::Cache::iterator it = cache.find(words[i]);
		if (it != cache.end())
		{
			found.push_back(it->second);
		}
	}
	
	std::map<std::string, std::pair<size_t, size_t>> buf;
	for (size_t i = 0; i < found.size(); ++i)
	{
		for(Types::WordMap::const_iterator it = found[i].cbegin();
			it != found[i].end(); ++it)
		{
			buf[it->second] = buf.find(it->second) == buf.end() ? 
				std::make_pair(size_t(1), it->first) :
				std::make_pair(buf[it->second].first + 1, buf[it->second].second + it->first);
		}
	}

	for (std::map<std::string, std::pair<size_t, size_t>>::const_iterator it = buf.begin();
		it != buf.end(); ++it)
	{
		result.insert(std::make_pair(it->second, it->first));
	}

	return result;
}

int main(size_t argc, char* args[])
{
	setlocale(LC_ALL, "rus");
	if (argc > 2)
	{
		return 1;
	}
	std::string arg;
	
	if (argc == 2)
	{
		arg = args[1];
	}

	Types::Cache cache;
	if (arg == "--help")
	{
		std::cout << "First parameter is path to directory.\n";
		std::cout << "In program commands:\n";
		std::cout << "For searching just write text.\n";
		std::cout << "	!index - recalculate cache.\n";
		std::cout << "	!cd - change directory.\n";
		std::cout << "	!help - support.\n";
		std::cout << "	!exit - close application.\n";
	}
	else
	{
		std::string path;
		if (arg.empty())
		{
			std::cout << "Use !help for more information.\n";
		}
		else
		{
			path = arg;
			Index(path);
			return 0;
		}
		while (true)
		{
			try
			{
				std::string buf;
				std::string command;
				std::vector<std::string> args;
				std::getline(std::cin, buf);
				std::stringstream ss(buf);
				if (buf[0] == '!')
				{
					ss >> command;
				}
				while (!ss.eof())
				{
					std::string word;
					ss >> word;
					args.push_back(word);
				}
				
				if (command == "!help")
				{
					std::cout << "In program commands:\n";
					std::cout << "For searching just write text.\n";
					std::cout << "	!index - recalculate cache.\n";
					std::cout << "	!load - to reload cache.\n";
					std::cout << "	!cd - change directory.\n";
					std::cout << "	!exit - close application.\n";
				}
				else if (command == "!index")
				{
					if (path.empty())
						Error("Please check the directory!");

					cache = Index(path);
				}
				else if (command == "!cd")
				{
					if (args.size() > 1)
						Error("Wrong path.");

					path = args[0];
				}
				else if (command == "!load")
				{
					if (path.empty())
						Error("Please check the directory!");

					Load(path, cache);
				}
				else if (command == "!exit")
				{
					return 0;
				}
				else
				{
					if (!command.empty())
						Error("Unknown comand.");

					Types::Tout out = GetOut(args, cache);

					if (out.empty())
						Error("Not found!");

					GreenMassege("Found in fles:\n");
					
					for (Types::Tout::const_iterator it = out.begin();
						it != out.end(); ++it)
					{
						std::cout << it->second << " " << it->first.second << " times with "
							<< it->first.first << " words\n";
					}
				}
			}
			catch (std::exception ex)
			{
				RedMassege(ex.what());
				std::cout << "\n";
			}
			catch (...)
			{
				RedMassege("Unknown exeption.\n");
			}
		}
	}
	return 0;
}
