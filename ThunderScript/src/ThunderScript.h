#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <cstddef>
#include <map>
#include <stack>
#include <any>
#include <memory>
#include "massert.h"

namespace ts
{
	const std::string tsVersion = "0.0.0";

	//define our bytecode commands;

	const std::byte tsPUSH      = (std::byte)0; // open a scope
	const std::byte tsPOP       = (std::byte)1; // exit a scope
	const std::byte tsRETURNF   = (std::byte)2; // return a float
	const std::byte tsALLOCATEF = (std::byte)3; // allocate a float var
	const std::byte tsLOADF     = (std::byte)4; // load float from bytecode into var
	const std::byte tsMOVEF     = (std::byte)5; // copy a float from one index to another
	const std::byte tsFLIPF     = (std::byte)6; // store opeator var in float
	const std::byte tsADDF      = (std::byte)7; // add operator vars and store in one of them

	// Define our varible type, the language is statically typed but it's more simple on this side if we store every type of value in one class
	class tsVariable
	{
	public:
		virtual void SetValue(float value)
		{
			massert(false, "Called base tsVar function");
		};
		virtual float GetValue()
		{
			massert(false, "Called base tsVar function");
			return 0;
		};
	};

	class tsFloat : public tsVariable
	{
	public:
		float _value;
		void SetValue(float value)
		{
			_value = value;
		}
		float GetValue()
		{
			return _value;
		}
	};

	class tsBytecode
	{
	private:
		union ByteFloat
		{
			float f;
			std::byte b[4];
		};
		union ByteUint
		{
			unsigned int i;
			std::byte b[4];
		};
	public:
		std::vector<std::byte> bytes;
	
		//The only place conversions will take place is here, that way if I have to port the code it's all here;
		unsigned int readUint(size_t index) const
		{
			ByteUint output;
			output.b[0] = bytes[index];
			output.b[1] = bytes[index + 1];
			output.b[2] = bytes[index + 2];
			output.b[3] = bytes[index + 3];
			return output.i;
		}
		float readFloat(size_t index) const 
		{
			ByteFloat output;
			output.b[0] = bytes[index];
			output.b[1] = bytes[index + 1];
			output.b[2] = bytes[index + 2];
			output.b[3] = bytes[index + 3];
			return output.f;
		}
		void addUint(unsigned int uint)
		{
			ByteUint o;
			o.i = uint;
			bytes.push_back(o.b[0]);
			bytes.push_back(o.b[1]);
			bytes.push_back(o.b[2]);
			bytes.push_back(o.b[3]);
		}
		
		void addFloat(float value)
		{
			ByteFloat o;
			o.f = value;
			bytes.push_back(o.b[0]);
			bytes.push_back(o.b[1]);
			bytes.push_back(o.b[2]);
			bytes.push_back(o.b[3]);
		}
		void LOADF(unsigned int varIndex, float value)
		{
			bytes.push_back(tsLOADF);
			addFloat(value);
			addUint(varIndex);
		}
		void MOVEF(unsigned int varIndex, unsigned int targetIndex)
		{
			bytes.push_back(tsMOVEF);
			addUint(varIndex);
			addUint(targetIndex);
		}
		void FLIPF(unsigned int varIndex)
		{
			bytes.push_back(tsFLIPF);
			addUint(varIndex);
		}
		void ALLOCATEF()
		{
			bytes.push_back(tsALLOCATEF);
		}
		void ADDF(unsigned int aIndex, unsigned int bIndex, unsigned int returnIndex)
		{
			bytes.push_back(tsADDF);
			addUint(aIndex);
			addUint(bIndex);
			addUint(returnIndex);
		}
		void PUSH()
		{
			bytes.push_back(tsPUSH);
		}
		void POP()
		{
			bytes.push_back(tsPOP);
		}
		void RETURNF()
		{
			bytes.push_back(tsRETURNF);
		}
	};

	class tsFunction
	{
	public: 
		std::string identifier;
		tsBytecode bytecode;
		tsFunction(std::string _identifier)
		{
			identifier = _identifier;
		}
	};

	class tsScript
	{
	public:
		unsigned int numStructs;
		unsigned int numFunctions;
		unsigned int maxVars;

		unsigned int mainFunction;
		std::vector<tsFunction> functions;
	};

	struct tsScope
	{
		unsigned int numFloats;
		tsScope(unsigned int _numFloats)
		{
			numFloats = _numFloats;
		}
	};

	class tsContext
	{
	public:
		std::vector<tsScript> scripts;
	};

	class tsRuntime
	{
	private:
		std::shared_ptr<tsContext> _context;

		
		std::stack<unsigned int > scopes;
		std::vector<std::unique_ptr<tsVariable>> stack;

	public:
		tsRuntime(std::shared_ptr<tsContext>& context)
		{
			_context = context;
		}

		void RunScript(const tsScript& script)
		{
			RunFunction(script.functions[script.mainFunction]);
		}

		void RunFunction(const tsFunction& function)
		{
			ExecuteByteCode(function.bytecode);
		}

		void ExecuteByteCode(const tsBytecode& bytecode)
		{
			for (size_t i = 0; i < bytecode.bytes.size(); i++)
			{
				switch (bytecode.bytes[i])
				{
					case tsRETURNF:
					{
						std::cout << "Return: " << stack[0]->GetValue() << std::endl;
						i = bytecode.bytes.size();
						break;
					}
					case tsALLOCATEF:
					{
						std::cout << "Expanding float array" << std::endl;
						stack.push_back(std::make_unique<tsFloat>());
						break;
					}
					case tsPUSH:
					{
						scopes.push(stack.size());
						break;
					}
					case tsPOP:
					{
						stack.resize(scopes.top());
						scopes.pop();
						break;
					}
					case tsLOADF:
					{
						std::cout << "Loading float" << std::endl;
						float value = bytecode.readFloat(++i);
						i += 4;
						stack[bytecode.readUint(i)]->SetValue(value);
						i += 3;
					}
						break;
					case tsMOVEF:
					{
						std::cout << "Moving var" << std::endl;
						unsigned int a = bytecode.readUint(++i);
						i += 4;
						unsigned int b = bytecode.readUint(i);
						i += 3;
						stack[b]->SetValue(stack[a]->GetValue());
					}
						break;
					case tsFLIPF:
					{
						std::cout << "Flipping Float" << std::endl;
						unsigned int varIndex = bytecode.readUint(++i);
						i += 3;
						stack[varIndex]->SetValue( -stack[varIndex]->GetValue());
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
						std::cout << "Adding: " << stack[a]->GetValue() << " + " << stack[b]->GetValue() << std::endl;
						stack[r]->SetValue( stack[a]->GetValue() + stack[b]->GetValue());
						break;
					}
					default:
					{

						massert(false, "Unknown byte code! " + std::to_string((int)bytecode.bytes[i]));
						break;
					}
				}
			}
			
		}
	};
}