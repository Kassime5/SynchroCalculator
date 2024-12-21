#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

// Function to sanitize variable names for C++ (removing invalid characters)
std::string sanitizeName(const std::string& name) {
    std::string sanitized = name;
    for (char& c : sanitized) {
        if (!isalnum(c)) c = '_';
    }
    return sanitized;
}

// Function to generate binary data as C++ array and its size
void generateBinaryData(const std::string& inputFile, std::ofstream& cppFile, std::ofstream& headerFile, std::vector<std::string>& assetNames) {
    std::ifstream inFile(inputFile, std::ios::binary);
    if (!inFile) {
        std::cerr << "Failed to open file: " << inputFile << std::endl;
        return;
    }

    // Sanitize variable name from input file name
    std::string variableName = sanitizeName(fs::path(inputFile).stem().string());
    std::string arrayName = variableName + "_data";
    std::string sizeName = variableName + "_size";

    assetNames.push_back(variableName);

    // Write data array and size to .cpp
    cppFile << "// Binary data for " << inputFile << "\n";
    cppFile << "unsigned char " << arrayName << "[] = {\n";

    std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(inFile), {});
    for (size_t i = 0; i < buffer.size(); ++i) {
        cppFile << "0x" << std::hex << static_cast<int>(buffer[i]);
        if (i < buffer.size() - 1) cppFile << ",";
        if ((i + 1) % 12 == 0) cppFile << "\n";
    }
    cppFile << "\n};\n";

    cppFile << "unsigned int " << sizeName << " = " << std::dec << buffer.size() << ";\n\n";

    // Write extern declarations to .h
    headerFile << "extern unsigned char " << arrayName << "[];\n";
    headerFile << "extern unsigned int " << sizeName << ";\n\n";
}

int main(int argc, char* argv[]) {
    if (argc < 1) {
        std::cerr << "Usage: " << argv[0] << " <file1> <file2> ...\n";
        return 1;
    }

    // Output files
	std::string outputName = "AssetsToBinary";
    std::ofstream cppFile(outputName + ".cpp");
    std::ofstream headerFile(outputName + ".h");

    if (!cppFile || !headerFile) {
        std::cerr << "Failed to create output files.\n";
        return 1;
    }

    // Write header guard
    std::string guardName = sanitizeName(outputName) + "_H";
    headerFile << "#ifndef " << guardName << "\n";
    headerFile << "#define " << guardName << "\n\n";

    headerFile << "#include <vector>\n";
    headerFile << "#include <utility>\n";
    headerFile << "\n";

    cppFile << "#include \"" << outputName << ".h\"\n\n";

    // List of asset names
    std::vector<std::string> assetNames;

    // Process each input file
    for (int i = 1; i < argc; ++i) {
        generateBinaryData(argv[i], cppFile, headerFile, assetNames);
    }

    // Write the global list of assets in the .cpp file
    cppFile << "// List of assets (binary data and their sizes)\n";
    cppFile << "std::vector<std::pair<unsigned char*, unsigned int>> assets = {\n";
    for (const auto& name : assetNames) {
        cppFile << "    {" << name << "_data, " << name << "_size},\n";
    }
    cppFile << "};\n";

    // Write extern declaration of the list in the .h file
    headerFile << "extern std::vector<std::pair<unsigned char*, unsigned int>> assets;\n\n";

    // End header guard
    headerFile << "#endif //" << guardName << "\n";

    std::cout << "Generated " << outputName << ".h and " << outputName << ".cpp\n";
    return 0;
}
