#ifndef MAPS_H_
#define MAPS_H_

#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

// #include <cassert>
#include <cstdio>
#include <cstring>

#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/stat.h>
#include <sys/wait.h>

struct maps_lines
{
	std::string range;
	uintptr_t start;
	uintptr_t end;
	std::string permissions;
	uintptr_t offset;
	std::string device;
	uintptr_t inode;
	std::string path;

	maps_lines(std::string line)
	{
		std::stringstream lines(line);
		lines >> range >> permissions >> offset >> device >> inode >> path;
		
		// split range
		std::size_t delim = range.find('-');
		end = std::strtoul(range.substr(delim + 1).c_str(), NULL, 16);
		start = std::strtoul(range.substr(0, delim).c_str(), NULL, 16);
	}

	std::string str()
	{
		return std::to_string(start) + "-" + std::to_string(end) + " " + 
		                   permissions + " " + path;
	}
};

// https://stackoverflow.com/a/868894
class InputParser{
	public:
		InputParser (int &argc, char **argv){
			for (int i=1; i < argc; ++i)
				this->tokens.push_back(std::string(argv[i]));
		}
		/// @author iain
		const std::string& getCmdOption(const std::string &option) const{
			std::vector<std::string>::const_iterator itr;
			itr =  std::find(this->tokens.begin(), this->tokens.end(), option);
			if (itr != this->tokens.end() && ++itr != this->tokens.end()){
				return *itr;
			}
			static const std::string empty_string("");
			return empty_string;
		}
		/// @author iain
		bool cmdOptionExists(const std::string &option) const{
			return std::find(this->tokens.begin(), this->tokens.end(), option)
				   != this->tokens.end();
		}
	private:
		std::vector <std::string> tokens;
};

#endif
