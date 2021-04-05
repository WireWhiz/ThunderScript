#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <cstddef>
#include <map>
#include <stack>
#include "massert.h"

namespace ts
{
	const std::string tsVersion = "0.0.0";

	//define our bytecode commands;

	const std::byte tsRETURN   = (std::byte)0;
	const std::byte tsALLOCATEF= (std::byte)1;
	const std::byte tsPUSH     = (std::byte)2;
	const std::byte tsPOP      = (std::byte)3;
	const std::byte tsCOPY     = (std::byte)4;
	const std::byte tsLOADF    = (std::byte)5;
	const std::byte tsADD      = (std::byte)6;


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
		void createInlineFloat(float value)
		{
			bytes.push_back(tsLOADF);
			addFloat(value);
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
		void ALLOCATEF()
		{
			bytes.push_back(tsALLOCATEF);
		}
		void ADD(unsigned int aIndex, unsigned int bIndex)
		{
			bytes.push_back(tsADD);
			addUint(aIndex);
			addUint(bIndex);
		}
		void COPY(unsigned int sourceVar, unsigned int targetVar)
		{
			bytes.push_back(tsCOPY);
			addUint(sourceVar);
			addUint(targetVar);
		}
		void PUSH()
		{
			bytes.push_back(tsPUSH);
		}
		void POP()
		{
			bytes.push_back(tsPOP);
		}
		void RETURN(unsigned int varIndex)
		{
			bytes.push_back(tsRETURN);
			addUint(varIndex);
		}
	};

	

	class tsVariable 
	{
	public:
		enum class Type
		{
			tsFloat
		};
	protected:
		Type _type;
	public:
		Type GetType()
		{
			return _type;
		}
		virtual void setValue(float value) = 0;
		virtual float getValue() = 0;
	};

	class tsFloat : public tsVariable
	{
	private:
		float _value = 0;
	public:
		void setValue(float value)
		{
			_value = value;
		}
		float getValue()
		{
			return _value;
		}
		tsFloat()
		{
			_value = 0;
			_type = tsVariable::Type::tsFloat;
		}
		tsFloat(float value)
		{
			_value = value;
			_type = tsVariable::Type::tsFloat;
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
		std::vector<tsFloat> varibles;
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

	

	class tsContext
	{
	public:
		std::vector<tsScript> scripts;
	};

	class tsRuntime
	{
	private:
		tsContext* _context;

		size_t scope = 0;
		std::stack<size_t> scopes;
		std::vector<tsFloat> stack;

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
					case tsRETURN:
					{
						unsigned int varIndex = bytecode.readUint(++i);
						std::cout << "Return: " << stack[varIndex].getValue() << std::endl;
						i = bytecode.bytes.size();
						break;
					}
					case tsALLOCATEF:
					{
						std::cout << "Allocating float" << std::endl;
						tsFloat v(0);
						stack.push_back(v);
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
						std::cout << "Loading var" << std::endl;
						float value = bytecode.readFloat(++i);
						i += 3;
						tsFloat v(value);
						stack.push_back(v);
					}
					break;
					case tsCOPY:
					{
						std::cout << "Copying var" << std::endl;
						unsigned int a = bytecode.readUint(++i);
						i += 4;
						unsigned int b = bytecode.readUint(i);
						i += 3;
						stack[b].setValue(stack[a].getValue());

						break;
					}
					case tsADD:
					{
						std::cout << "Addign var" << std::endl;
						unsigned int a = bytecode.readUint(++i);
						i += 4;
						unsigned int b = bytecode.readUint(i);
						i += 3;
						std::cout << "Adding " << (float)stack[a].getValue() << " to " << (float)stack[b].getValue() << std::endl;
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