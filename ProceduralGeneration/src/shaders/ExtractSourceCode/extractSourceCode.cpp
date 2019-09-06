#include "extractSourceCode.h"

#include <iostream>
#include <string>
#include <algorithm>

#include <filesystem>
#include <fstream>
#include <sstream>

#include "engine/Debug.h"
void extractSourceCode() {

	std::string fileNames = "\nthis->fileNames = { ";
	std::string sourceCodes = "this->sourceCode = {";

	std::string path = "src/shaders";
	for (const auto& entry : std::filesystem::directory_iterator(path)) {
		if (entry.is_directory()) continue;

		// retrieve the shader source code from filePath
		std::string shaderCode;
		std::ifstream shaderFile;

		// ensure ifstream objects can throw exceptions:
		shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try {
			// open file
			shaderFile.open(entry.path());
			std::stringstream shaderStream;

			// read file's buffer contents into stream
			shaderStream << shaderFile.rdbuf();

			// close file handler
			shaderFile.close();

			// convert stream into string
			shaderCode = shaderStream.str();
		}
		catch (std::ifstream::failure e) {
			std::cout << "ERROR::EXTRACTING::SOURCE::CODE" << std::endl;
		}

		std::string entryPath = entry.path().string();
		int index = (int)entryPath.size() - 1;
		for (; index >= 0; index--) {
			if (entryPath[index] == '\\' || entryPath[index] == '/') {
				index++;
				break;
			}
		}

		// get the name of the file
		std::string name = entryPath.substr(index);

		sourceCodes += "\"" + shaderCode + "\", ";
		fileNames += "\"" + name + "\", ";
	}

	// trim
	sourceCodes = sourceCodes.substr(0, sourceCodes.size() - 2);
	fileNames = fileNames.substr(0, fileNames.size() - 2);

	// more formatting
	sourceCodes += " };";
	fileNames += " };";

	// write to this file
	std::ofstream out("src/shaders/ExtractSourceCode/sourceCode.txt");

	// we have to convert new line characters to "\\n" so that the code can be compiled
	for (const auto& character : sourceCodes) {
		if (character == '\n') {
			out << "\\n";
		}
		else {
			out << character;
		}
	}
	out << fileNames;
	out.close();
}