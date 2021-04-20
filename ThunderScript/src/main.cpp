#include <iostream>
#include <fstream>
#include "ThunderScript.h"
#include "ThunderScriptCompiler.h"
#include <string>
#include "massert.h"
#include "TSBytecodeDebugger.h";

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
		std::shared_ptr<ts::tsContext> tsc = std::make_shared<ts::tsContext>();
		
		if (compiler.compile(filePath, tsc))
		{
			std::cout << "Sucessfully read file!" << std::endl;

			ts::tsBytecode bytecode = tsc->scripts[0].bytecode;
			
			ts::DisplayBytecode(bytecode);

			char input;
			std::cout << "Do you want to run it? (y/n): ";
			std::cin >> input;
			if (input == 'y')
			{
				std::cout << "Running script:\n\n";
				ts::tsRuntime runtime(tsc);

				runtime.LoadScript(0);
				runtime.SetGlobal<float>("a", 2);
				runtime.SetGlobal<float>("b", 3);
				runtime.Run();
				std::cout << "Global r has a value of: " << runtime.GetGlobal<float>("r") << std::endl;
			}
		}
		else
		{
			std::cout << "Could not find file." << std::endl;
		}

	}
	return 0;

	
}