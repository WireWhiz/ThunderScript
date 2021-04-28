#include <iostream>
#include <fstream>
#include "ThunderScript.h"
#include "ThunderScriptCompiler.h"
#include <string>
#include <chrono>
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
		try
		{
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
					auto start = std::chrono::high_resolution_clock::now();
					runtime.Run();
					auto stop = std::chrono::high_resolution_clock::now();
					std::cout << "\n\nGlobal r has a value of: " << runtime.GetGlobal<float>("r") << std::endl;
					std::cout << "Global testBool has a value of: " << runtime.GetGlobal<bool>("testBool") << std::endl;
					std::cout << "Program took: " << std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count()
						<< " microseconds" << std::endl;
				}
			}
			else
			{
				std::cout << "Could not find file." << std::endl;
			}
		}
		catch (ts::tsCompileError error)
		{
			error.display();
		}
		

	}
	return 0;

	
}