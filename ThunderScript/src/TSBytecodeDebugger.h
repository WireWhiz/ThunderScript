#pragma once
#include "ThunderScript.h"

namespace ts
{
	void DisplayBytecode(tsBytecode& bytecode)
	{
		for (size_t i = 0; i < bytecode.bytes.size(); i++)
		{
			std::cout << i << ":  ";
			switch (bytecode.bytes[i])
			{
				case tsRETURN:
				{
					std::cout << "Return" << std::endl;
					break;
				}
				case tsALLOCATEI:
				{
					std::cout << "Allocate int" << std::endl;
					break;
				}
				case tsALLOCATEF:
				{
					std::cout << "Allocate float" << std::endl;
					break;
				}
				case tsPUSH:
				{
					std::cout << "Enter scope" << std::endl;
					break;
				}
				case tsPOP:
				{
					std::cout << "Exit scope" << std::endl;
					break;
				}
				case tsLOADF:
				{
					std::cout << "Load float " << bytecode.readFloat(++i);
					i += 4;
					std::cout << " " << bytecode.readUint(i) << std::endl;
					i += 3;
				}
				break;
				case tsMOVEF:
				{
					std::cout << "Move float " << bytecode.readUint(++i);
					i += 4;
					std::cout << " " << bytecode.readUint(i) << std::endl;
					i += 3;
				}
				break;
				case tsFLIPF:
				{
					std::cout << "Fipp float " << bytecode.readUint(++i) << std::endl;
					i += 3;
				}
				break;
				case tsADDF:
				{
					unsigned int a = bytecode.readUint(++i);
					i += 4;
					unsigned int b = bytecode.readUint(i);
					i += 4;
					unsigned int r = bytecode.readUint(i);
					i += 3;
					std::cout << "Add floats: " << a << " " << b << " " << r << std::endl;
					break;
				}
				case tsMULF:
				{
					unsigned int a = bytecode.readUint(++i);
					i += 4;
					unsigned int b = bytecode.readUint(i);
					i += 4;
					unsigned int r = bytecode.readUint(i);
					i += 3;
					std::cout << "Multiply floats: " << a << " " << b << " " << r << std::endl;
				}
				break;
				case tsDIVF:
				{
					unsigned int a = bytecode.readUint(++i);
					i += 4;
					unsigned int b = bytecode.readUint(i);
					i += 4;
					unsigned int r = bytecode.readUint(i);
					i += 3;
					std::cout << "Divide floats: " << a << " " << b << " " << r << std::endl;

				}
				break;
				case tsLOADI:
				{
					std::cout << "Load int "<< bytecode.readFloat(++i);
					i += 4;
					std::cout << " " << bytecode.readUint(i) << std::endl;
					i += 3;
				}
				break;
				case tsMOVEI:
				{
					std::cout << "Move int " << bytecode.readUint(++i);
					i += 4;
					std::cout << " " << bytecode.readUint(i) << std::endl;
					i += 3;
				}
				break;
				case tsFLIPI:
				{
					std::cout << "Fipp int " << bytecode.readUint(++i) << std::endl;
					i += 3;
				}
				break;
				case tsADDI:
				{
					unsigned int a = bytecode.readUint(++i);
					i += 4;
					unsigned int b = bytecode.readUint(i);
					i += 4;
					unsigned int r = bytecode.readUint(i);
					i += 3;
					std::cout << "Add ints: " << a << " " << b << " " << r << std::endl;
					break;
				}
				case tsMULI:
				{
					unsigned int a = bytecode.readUint(++i);
					i += 4;
					unsigned int b = bytecode.readUint(i);
					i += 4;
					unsigned int r = bytecode.readUint(i);
					i += 3;
					std::cout << "Multiply int: " << a << " " << b << " " << r << std::endl;
				}
				break;
				case tsDIVI:
				{
					unsigned int a = bytecode.readUint(++i);
					i += 4;
					unsigned int b = bytecode.readUint(i);
					i += 4;
					unsigned int r = bytecode.readUint(i);
					i += 3;
					std::cout << "Divide int: " << a << " " << b << " " << r << std::endl;

				}
				break;
				default:
				{
					std::cout << "Unkown byte: " << (int)bytecode.bytes[i];
					break;
				}
			}
		}
	}
}
