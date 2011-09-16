#ifndef __FILEIO_H__
#define __FILEIO_H__

/*
 * loadTextFile
 * Loads a text file into a string.
 *
 * Arguments
 *     filename: Filename of the file to load.
 */

class FileIO {
	public:
		static std::string loadTextFile(std::string const& filename)
		{
			std::string text;

			if (!filename.empty()) {
				std::ifstream file(filename.c_str());
				text = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
			}

			return text;
		}
};

#endif
