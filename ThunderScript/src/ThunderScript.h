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
	const std::byte tsRETURN    = (std::byte)2; // return a float
	const std::byte tsALLOCATEF = (std::byte)3; // allocate a float var
	const std::byte tsLOADF     = (std::byte)4; // load float from bytecode into var
	const std::byte tsMOVEF     = (std::byte)5; // copy a float from one index to another
	const std::byte tsFLIPF     = (std::byte)6; // store opeator var in float
	const std::byte tsADDF      = (std::byte)7; // add operator vars and store in one of them
	const std::byte tsMULF      = (std::byte)8; // multiply a float
	const std::byte tsDIVF      = (std::byte)9; // divide a float
	const std::byte tsALLOCATEI = (std::byte)10; // allocate a float var
	const std::byte tsLOADI     = (std::byte)11; // load float from bytecode into var
	const std::byte tsMOVEI     = (std::byte)12; // load float from bytecode into var
	const std::byte tsFLIPI     = (std::byte)13; // store opeator var in float
	const std::byte tsADDI      = (std::byte)14; // add operator vars and store in one of them
	const std::byte tsMULI      = (std::byte)15; // multiply a float
	const std::byte tsDIVI      = (std::byte)16; // divide a float

	class tsBytecode
	{
	private:
		union ByteUint
		{
			unsigned int i;
			std::byte b[4];
		};
		union ByteInt
		{
			int i;
			std::byte b[4];
		};
		union ByteFloat
		{
			float f;
			std::byte b[4];
		};
		
		std::stack<unsigned int> scope;
		unsigned int numVars = 0;
	public:
		std::vector<std::byte> bytes;
	
		//The only place conversions will take place is here, that way if I have to port the code it's all here;
		void addUint(unsigned int uint)
		{
			ByteUint o;
			o.i = uint;
			bytes.push_back(o.b[0]);
			bytes.push_back(o.b[1]);
			bytes.push_back(o.b[2]);
			bytes.push_back(o.b[3]);
		}
		unsigned int readUint(size_t index) const
		{
			ByteUint output;
			output.b[0] = bytes[index];
			output.b[1] = bytes[index + 1];
			output.b[2] = bytes[index + 2];
			output.b[3] = bytes[index + 3];
			return output.i;
		}
		void addInt(unsigned int uint)
		{
			ByteInt o;
			o.i = uint;
			bytes.push_back(o.b[0]);
			bytes.push_back(o.b[1]);
			bytes.push_back(o.b[2]);
			bytes.push_back(o.b[3]);
		}
		unsigned int readInt(size_t index) const
		{
			ByteInt output;
			output.b[0] = bytes[index];
			output.b[1] = bytes[index + 1];
			output.b[2] = bytes[index + 2];
			output.b[3] = bytes[index + 3];
			return output.i;
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
		float readFloat(size_t index) const 
		{
			ByteFloat output;
			output.b[0] = bytes[index];
			output.b[1] = bytes[index + 1];
			output.b[2] = bytes[index + 2];
			output.b[3] = bytes[index + 3];
			return output.f;
		}
		void LOADI(unsigned int varIndex, float value)
		{
			bytes.push_back(tsLOADI);
			addInt(value);
			addUint(varIndex);
		}
		void LOADF(unsigned int varIndex, float value)
		{
			bytes.push_back(tsLOADF);
			addFloat(value);
			addUint(varIndex);
		}
		unsigned int ALLOCATEI()
		{
			bytes.push_back(tsALLOCATEI);
			return ++numVars - 1;
		}
		unsigned int ALLOCATEF()
		{
			bytes.push_back(tsALLOCATEF);
			return ++numVars - 1;
		}
		void MOVEI(unsigned int varIndex, unsigned int targetIndex)
		{
			bytes.push_back(tsMOVEI);
			addUint(varIndex);
			addUint(targetIndex);
		}
		void MOVEF(unsigned int varIndex, unsigned int targetIndex)
		{
			bytes.push_back(tsMOVEF);
			addUint(varIndex);
			addUint(targetIndex);
		}
		void FLIPI(unsigned int varIndex)
		{
			bytes.push_back(tsFLIPI);
			addUint(varIndex);
		}
		void FLIPF(unsigned int varIndex)
		{
			bytes.push_back(tsFLIPF);
			addUint(varIndex);
		}
		void ADDI(unsigned int aIndex, unsigned int bIndex, unsigned int returnIndex)
		{
			bytes.push_back(tsADDI);
			addUint(aIndex);
			addUint(bIndex);
			addUint(returnIndex);
		}
		void ADDF(unsigned int aIndex, unsigned int bIndex, unsigned int returnIndex)
		{
			bytes.push_back(tsADDF);
			addUint(aIndex);
			addUint(bIndex);
			addUint(returnIndex);
		}
		void MULI(unsigned int aIndex, unsigned int bIndex, unsigned int returnIndex)
		{
			bytes.push_back(tsMULI);
			addUint(aIndex);
			addUint(bIndex);
			addUint(returnIndex);
		}
		void MULF(unsigned int aIndex, unsigned int bIndex, unsigned int returnIndex)
		{
			bytes.push_back(tsMULF);
			addUint(aIndex);
			addUint(bIndex);
			addUint(returnIndex);
		}
		void DIVI(unsigned int aIndex, unsigned int bIndex, unsigned int returnIndex)
		{
			bytes.push_back(tsDIVI);
			addUint(aIndex);
			addUint(bIndex);
			addUint(returnIndex);
		}
		void DIVF(unsigned int aIndex, unsigned int bIndex, unsigned int returnIndex)
		{
			bytes.push_back(tsDIVF);
			addUint(aIndex);
			addUint(bIndex);
			addUint(returnIndex);
		}
		void PUSH()
		{
			bytes.push_back(tsPUSH);
			scope.push(numVars);
		}
		void POP()
		{
			bytes.push_back(tsPOP);
			numVars = scope.top();
			scope.pop();
		}
		void RETURN()
		{
			bytes.push_back(tsRETURN);
		}
	};

	enum class ValueType
	{
		tsUnknown,
		tsInt,
		tsFloat
	};

	// Define our varible type, the language is statically typed but it's more simple on this side if we store every type of value in one class
	class tsVariable
	{
	public:
		virtual ValueType getType() = 0;
	};

	class tsSingle : public tsVariable
	{
	public:
		virtual void setFloat(float value) = 0;
		virtual float getFloat() = 0;
		virtual void setInt(int value) = 0;
		virtual int getInt() = 0;
	};

	tsSingle& toSingle(std::unique_ptr<tsVariable>& var)
	{
		return *static_cast<tsSingle*>(var.get());
	}

	class tsInt : public tsSingle
	{
	public:
		int _value;
		ValueType getType()
		{
			return ValueType::tsInt;
		}
		void setInt(int value)
		{
			_value = value;
		}
		int getInt()
		{
			return _value;
		}
		void setFloat(float value)
		{
			_value = value;
		}
		float getFloat()
		{
			return _value;
		}

	};

	class tsFloat : public tsSingle
	{
	public:
		float _value;
		ValueType getType()
		{
			return ValueType::tsFloat;
		}
		void setInt(int value)
		{
			_value = value;
		}
		int getInt()
		{
			return _value;
		}
		void setFloat(float value)
		{
			_value = value;
		}
		float getFloat()
		{
			return _value;
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
	

	class tsContext
	{
	public:
		std::vector<tsScript> scripts;
	};

	class tsRuntime
	{
	private:
		std::shared_ptr<tsContext> _context;

		unsigned int loadedScript;
		unsigned int loadedFunction;

		size_t cursor;
		std::stack<unsigned int> scopes;
		std::vector<std::unique_ptr<tsVariable>> stack;

	public:
		tsRuntime(std::shared_ptr<tsContext>& context)
		{
			_context = context;
		}

		void LoadScript(unsigned int script)
		{
			loadedScript = script;
			LoadFunction(_context->scripts[script].mainFunction);
		}

		void LoadFunction(unsigned int index)
		{
			cursor = 0;
			loadedFunction = index;
			ExecuteByteCode(_context->scripts[loadedScript].functions[loadedFunction].bytecode);
		}

		void Run()
		{
			std::cout << "Running script: " << loadedScript << " function: " << loadedFunction << " at cursor index: " << cursor << std::endl;
			size_t start = cursor;
			ExecuteByteCode(_context->scripts[loadedScript].functions[loadedFunction].bytecode);
			// Return the cursor to the start of the function after we complete it
			cursor = start;
		}

		void ExecuteByteCode(const tsBytecode& bytecode)
		{
			bool executing = true;
			while (executing && cursor < bytecode.bytes.size())
			{
				switch (bytecode.bytes[cursor])
				{
					case tsRETURN:
					{
						std::cout << "Return: " << toSingle(stack[0]).getFloat() << std::endl;
						executing = false;
						break;
					}
					case tsALLOCATEF:
					{
						stack.push_back(std::make_unique<tsFloat>());
						break;
					}
					case tsALLOCATEI:
					{
						stack.push_back(std::make_unique<tsInt>());
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
						float value = bytecode.readFloat(++cursor);
						cursor += 4;
						toSingle(stack[bytecode.readUint(cursor)]).setFloat(value);
						cursor += 3;
					}
						break;
					case tsMOVEF:
					{
						unsigned int a = bytecode.readUint(++cursor);
						cursor += 4;
						unsigned int b = bytecode.readUint(cursor);
						cursor += 3;
						toSingle(stack[b]).setFloat(toSingle(stack[a]).getFloat());
					}
						break;
					case tsFLIPF:
					{
						unsigned int varIndex = bytecode.readUint(++cursor);
						cursor += 3;
						toSingle(stack[varIndex]).setFloat( -toSingle(stack[varIndex]).getFloat());
					}
						break;
					case tsADDF:
					{
						unsigned int a = bytecode.readUint(++cursor);
						cursor += 4;
						unsigned int b = bytecode.readUint(cursor);
						cursor += 4;
						unsigned int r = bytecode.readUint(cursor);
						cursor += 3;
						toSingle(stack[r]).setFloat( toSingle(stack[a]).getFloat() + toSingle(stack[b]).getFloat());
						break;
					}
					case tsMULF:
					{
						unsigned int a = bytecode.readUint(++cursor);
						cursor += 4;
						unsigned int b = bytecode.readUint(cursor);
						cursor += 4;
						unsigned int r = bytecode.readUint(cursor);
						cursor += 3;
						toSingle(stack[r]).setFloat(toSingle(stack[a]).getFloat() * toSingle(stack[b]).getFloat());
					}
						break;
					case tsDIVF:
					{
						unsigned int a = bytecode.readUint(++cursor);
						cursor += 4;
						unsigned int b = bytecode.readUint(cursor);
						cursor += 4;
						unsigned int r = bytecode.readUint(cursor);
						cursor += 3;
						toSingle(stack[r]).setFloat(toSingle(stack[a]).getFloat() / toSingle(stack[b]).getFloat());
					}
					break;
					case tsLOADI:
					{
						int value = bytecode.readInt(++cursor);
						cursor += 4;
						toSingle(stack[bytecode.readUint(cursor)]).setInt(value);
						cursor += 3;
					}
					break;
					case tsMOVEI:
					{
						unsigned int a = bytecode.readUint(++cursor);
						cursor += 4;
						unsigned int b = bytecode.readUint(cursor);
						cursor += 3;
						toSingle(stack[b]).setInt(toSingle(stack[a]).getInt());
					}
					break;
					case tsFLIPI:
					{
						unsigned int varIndex = bytecode.readUint(++cursor);
						cursor += 3;
						toSingle(stack[varIndex]).setInt(-toSingle(stack[varIndex]).getInt());
					}
					break;
					case tsADDI:
					{
						unsigned int a = bytecode.readUint(++cursor);
						cursor += 4;
						unsigned int b = bytecode.readUint(cursor);
						cursor += 4;
						unsigned int r = bytecode.readUint(cursor);
						cursor += 3;
						toSingle(stack[r]).setInt(toSingle(stack[a]).getInt() + toSingle(stack[b]).getInt());
						break;
					}
					case tsMULI:
					{
						unsigned int a = bytecode.readUint(++cursor);
						cursor += 4;
						unsigned int b = bytecode.readUint(cursor);
						cursor += 4;
						unsigned int r = bytecode.readUint(cursor);
						cursor += 3;
						toSingle(stack[r]).setInt(toSingle(stack[a]).getInt() * toSingle(stack[b]).getInt());
					}
					break;
					case tsDIVI:
					{
						unsigned int a = bytecode.readUint(++cursor);
						cursor += 4;
						unsigned int b = bytecode.readUint(cursor);
						cursor += 4;
						unsigned int r = bytecode.readUint(cursor);
						cursor += 3;
						toSingle(stack[r]).setInt(toSingle(stack[a]).getInt() / toSingle(stack[b]).getInt());
					}
					break;
					default:
					{
						massert(false, "Unknown byte code! " + std::to_string((unsigned int)bytecode.bytes[cursor]));
						__assume(false);
						//break;
					}
				}
				++cursor;
			}
			
		}
	};
}