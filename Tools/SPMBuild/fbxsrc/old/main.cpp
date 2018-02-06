#include <iostream>
#include "FBXExporter.h"

static std::vector<String> SplitString(const String& string, const String& delimiters)
{
	size_t start = 0;
	size_t end = string.find_first_of(delimiters);

	std::vector<String> result;

	while (end <= String::npos)
	{
		result.emplace_back(string.substr(start, end - start));

		if (end == String::npos)
			break;

		start = end + 1;
		end = string.find_first_of(delimiters, start);
	}

	return result;
}

static std::vector<String> SplitString(const String& string, const char delimiter)
{
	return SplitString(string, String(1, delimiter));
}

static std::vector<String> SplitStringIntoTokens(const String& string)
{
	return SplitString(string, " \t\n");
}

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		std::cout << "Usage: SPMBuild fbx_filename [output_filename]" << std::endl;
		getchar();
		return -1;
	}

	String path = argv[1];
	auto split = SplitString(path, "/\\");
	String name = split.back();

	String outputPath = "";

	if (argc > 2)
		outputPath = argv[2];
	else
		outputPath = name.substr(0, name.size() - 4) + ".spm";

	FBXExporter* myExporter = new FBXExporter();
	myExporter->Initialize();
	if (!myExporter->LoadScene(path.c_str(), outputPath.c_str()))
	{
		std::cout << "Could not read file!" << std::endl;
		getchar();
		return -1;
	}
	myExporter->ExportFBX();

	getchar();
	return 0;
}