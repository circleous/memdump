#ifndef MAPS_H_
#define MAPS_H_

#include <regex>
#include <string>
#include <sstream>
#include <vector>

struct maps_lines
{
	bool valid;
	uintptr_t start;
	uintptr_t end;
	std::string permissions;
	uintptr_t offset;
	std::string device;
	uintptr_t inode;
	std::string pathname;

	maps_lines(std::string line)
	{
		std::regex re("^([^\\s-]+)-([^\\s]+)\\s([^\\s]+)\\s([^\\s]+)\\s([^\\s]+)\\s([^\\s]+)\\s*(.*)$");
		std::smatch sm;

		std::regex_match(line, sm, re);

		if (sm.size() != 8) {
			valid = false;
			return;
		}

		//assign each part to a member variable
		start = std::stoull(sm[1],NULL, 16);
		end = std::stoull(sm[2], NULL, 16);
		permissions = sm[3];
		offset = std::stoull(sm[4], NULL, 16);
		device = sm[5];
		inode = std::stoull(sm[6], NULL, 10);
		pathname = sm[7];
		valid = true;
	}

	std::string str()
	{
		std::stringstream ss;
		ss << std::hex << start << "-" << end << " " 
			<< permissions << " "
			<< offset << " "
			<< device << " "
			<< std::dec << inode << " "
			<< pathname;
		return ss.str();
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