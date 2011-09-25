#ifndef __FILEIO_H__
#define __FILEIO_H__

#include <fstream>
#include <string>
#include <vector>

/*
 * FileIO
 * Utility methods for handling file input/output.
 */
class FileIO {
	public:
		// Loads a text file into a string.
		// Arguments
		//		filename: Filename of the file to load.
		static std::string loadTextFile(std::string const& filename)
		{
			std::string text;

			if (!filename.empty()) {
				std::ifstream file(filename.c_str());
				text = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
			}

			return text;
		}

		// Loads a text file into a vector of strings.
		// Arguments
		//		filename: Filename of the file to load.
		static std::vector<std::string> loadLinesFromFile(std::string const& filename)
		{
			std::vector<std::string> text;

			if (!filename.empty()) {
				std::ifstream file(filename.c_str());
				std::string item;
				while (std::getline(file, item)) {
					text.push_back(item);
				}
			}

			return text;
		}
};

#endif
