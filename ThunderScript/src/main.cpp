#include <iostream>
#include "ThunderScript.h"
#include "ThunderScriptCompiler.h"
#include <string>
#include "massert.h"

int main()
{
	std::cout << "ThunderScript Compiler Version: " << ts::tsVersion <<std::endl;
	ts::tsCompiler compiler;
	while (true)
	{
		std::cout << "Please enter a file path: ";
		std::string filePath;
		std::cin >> filePath;
		if (filePath == "exit")
			break;
		ts::tsContext* tsc = new ts::tsContext;
		
		if (compiler.compile(filePath, tsc))
		{
			std::cout << "Sucessfully read file!" << std::endl;

			ts::tsBytecode bytecode = tsc->scripts[0].functions[0].bytecode;
			for (size_t i = 0; i < bytecode.bytes.size(); i++)
			{
				std::cout << (int)bytecode.bytes[i] << std::endl;
			}

			std::cout << "Do you want to run it? (y/n): ";
			char input;
			std::cin >> input;
			if (input == 'y')
			{
				std::cout << "Running script:\n\n";
				ts::tsRuntime runtime(tsc);

				runtime.RunScript(tsc -> scripts[0]);
			}
		}
		else
		{
			std::cout << "Could not find file." << std::endl;
		}
		delete tsc;

	}
	return 0;

	
}