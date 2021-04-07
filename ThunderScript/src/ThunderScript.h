#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <cstddef>
#include <map>
#include <stack>
#include <any>
#include "massert.h"

namespace ts
{
	const std::string tsVersion = "0.0.0";

	//define our bytecode commands;

	const std::byte tsPUSH      = (std::byte)0; // open a scope
	const std::byte tsPOP       = (std::byte)1; // exit a scope
	const std::byte tsRETURNF   = (std::byte)2; // return a float
	const std::byte tsALLOCATEF = (std::byte)3; // allocate a float var
	const std::byte tsREADF     = (std::byte)4; // load float from bytecode into operator var
	const std::byte tsLOADF     = (std::byte)5; // load float into operator var
	const std::byte tsSTOREF    = (std::byte)6; // store opeator var in float
	const std::byte tsFLIPF     = (std::byte)7; // store opeator var in float
	const std::byte tsADDF      = (std::byte)8; // add operator vars and store in one of them

	// Define our varible type, the language is statically typed but it's more simple on this side if we store every type of value in one class
	class tsVariable
	{
	public:
		enum class Type
		{
			tsFloat,
			tsInt,
			tsArray
		};
	protected:
		std::any _value;
		Type _type;
	public:
		Type GetType()
		{
			return _type;
		}
		tsVariable()
		{
		};
		tsVariable(const tsVariable& v)
		{
			_type = v._type;
			_value = v._value;
		};
		tsVariable(float value)
		{
			_type = Type::tsFloat;
			_value = value;
		}
		void setValue(float value)
		{
			assert(_type == Type::tsFloat);
			_value = value;
		}
		float getValue()
		{
			return std::any_cast<float>(_value);
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
		void READF(unsigned int oVarIndex, float value)
		{
			bytes.push_back(tsREADF);
			addFloat(value);
			bytes.push_back(std::byte(oVarIndex));
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
		void FLIPF(unsigned int oVarIndex)
		{
			bytes.push_back(tsFLIPF);
			bytes.push_back(std::byte(oVarIndex));
		}
		void ALLOCATE(tsVariable::Type type)
		{
			bytes.push_back(tsALLOCATEF);
		}
		void ADDF(unsigned int oVarIndex)
		{
			bytes.push_back(tsADDF);
			bytes.push_back(std::byte(oVarIndex));
		}
		void STOREF(unsigned int oVarIndex, unsigned int targetVar)
		{
			bytes.push_back(tsSTOREF);
			bytes.push_back((std::byte)oVarIndex);
			addUint(targetVar);
		}
		void LOADF(unsigned int targetVar, unsigned int oVarIndex)
		{
			bytes.push_back(tsLOADF);
			addUint(targetVar);
			bytes.push_back((std::byte)oVarIndex);

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

	class tsStructDef
	{
		std::string identifier;
		std::vector<tsVariable::Type> variableTypes;
	};

	class tsStruct
	{
	public:
		tsStructDef* tsStructDef;
		std::vector<tsVariable> varibles;
	};

	class tsStatement
	{
	public:
		
	};

	class tsFunction
	{
	public: 
		std::string identifier;
		std::vector<tsStruct> paramaters;
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
		tsContext* _context;

		
		std::stack<tsScope> scopes;
		std::vector<float> fStack;

		std::any opVars[2] = { std::any(0), std::any(0) };

	public:
		tsRuntime(tsContext* context)
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
						std::cout << "Return: " << std::any_cast<float>(opVars[1]) << std::endl;
						i = bytecode.bytes.size();
						break;
					}
					case tsALLOCATEF:
					{
						std::cout << "Expanding float array" << std::endl;
						fStack.emplace_back(0);
						break;
					}
					case tsPUSH:
					{
						scopes.push(tsScope(fStack.size()));
						break;
					}
					case tsPOP:
					{
						tsScope scope = scopes.top();
						fStack.resize(scope.numFloats);
						scopes.pop();
						break;
					}
					case tsREADF:
					{
						std::cout << "Reading float" << std::endl;
						float value = bytecode.readFloat(++i);
						i += 4;
						opVars[(char)bytecode.bytes[i]] = value;
					}
						break;
					case tsLOADF:
					{
						std::cout << "Loading float" << std::endl;
						float varIndex = bytecode.readUint(++i);
						i += 4;
						opVars[(char)bytecode.bytes[i]] = fStack[varIndex];
					}
						break;
					case tsSTOREF:
					{
						std::cout << "Storing float" << std::endl;
						unsigned char opVar = (unsigned char)bytecode.bytes[++i];
						fStack[bytecode.readUint(++i)] = std::any_cast<float>(opVars[opVar]);
						i += 3;
						
					}
						break;
					case tsFLIPF:
					{
						std::cout << "Flipping Float" << std::endl;
						unsigned char opVar = (unsigned char)bytecode.bytes[++i];
						opVars[opVar] = -std::any_cast<float>(opVars[opVar]);
					}
						break;
					case tsADDF:
					{
						std::cout << "Adding: " << std::any_cast<float>(opVars[0]) << " + " << std::any_cast<float>(opVars[1]) << std::endl;
						opVars[(unsigned char)bytecode.bytes[++i]] = std::any_cast<float>(opVars[0]) + std::any_cast<float>(opVars[1]);
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