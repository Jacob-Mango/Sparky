#include <iostream>
#include <fstream>
#include <sstream>

#include <sys/stat.h>

#include "Mesh.h"
#include "Utils.h"
#include "Writer.h"

using namespace std;
using namespace sp;
using namespace tools;

int main(int argc, char* argv[]) {
	String name, outputPath;

	if (argc < 2)
	{
		std::cout << "Invalid arguments" << std::endl;
		std::cout << std::endl;
		std::cout << "Usage: SPMBuild input_file [output_filename]" << std::endl;
		return -1;
	}
	String path = argv[1];
	auto split = SplitString(path, "/\\");
	name = split.back();

	if (argc > 2) {
		outputPath = argv[2];
	}
	else {
		outputPath = name.substr(0, name.size() - 4) + ".spm";
		std::cout << "Output path not specified. Outputting to " << outputPath << std::endl;
	}

	Mesh* mesh = new Mesh();

	mesh->LoadMesh(path);

	Writer* writer = new Writer(name, POSITION | NORMAL | UV | BINORMAL | TANGENT | JOINTWEIGHT | JOINTINDEX, mesh->m_Entries, mesh->m_BoneInfo);
	writer->Write(outputPath);
	
	return 0;
}