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

	#define tsEND       std::byte(0) // return values
	#define tsItoF      std::byte(1) // cast an int to a float 
	#define tsFtoI      std::byte(2) // cast a float to an int
	#define tsLOAD      std::byte(3) // load bytes from code into memory
	#define tsMOVE      std::byte(4) // copy a float from one index to another
	#define tsFLIPF     std::byte(5) // store opeator var in float
	#define tsADDF      std::byte(6) // add operator vars and store in one of them
	#define tsMULF      std::byte(7) // multiply a float
	#define tsDIVF      std::byte(8) // divide a float
	#define tsMOVEI     std::byte(9) // load float from bytecode into var
	#define tsFLIPI     std::byte(10) // store opeator var in float
	#define tsADDI      std::byte(11) // add operator vars and store in one of them
	#define tsMULI      std::byte(12) // multiply a float
	#define tsDIVI      std::byte(13) // divide a float
	

	enum class ValueType
	{
		tsUnknown,
		tsInt,
		tsFloat
	};

	class tsBytes
	{
	private:
		std::vector<std::byte> bytes;
	public:
		template <class T>
		void pushBack(T value)//, unsigned int index)
		{
			size_t index = bytes.size();
			bytes.resize(bytes.size() + sizeof(T), std::byte(0));
			std::memcpy(&bytes[index], &value, sizeof(T));
		}
		template <class T>
		void set(size_t index, T value)
		{
			massert(index >= 0 && index < bytes.size() - sizeof(T) + 1, "Index " + std::to_string(index) + " out of byte range");
			std::memcpy(&bytes[index], &value, sizeof(T));
		}
		template <class T>
		T read(size_t index) const
		{
			massert(index >= 0 && index < bytes.size() - sizeof(T) + 1, "Index " + std::to_string(index) + " out of byte range");
			T rv;
			std::memcpy(&rv, &bytes[index], sizeof(T));
			return rv;
		}
		size_t size() const
		{
			return bytes.size();
		}
		void clear()
		{
			bytes.clear();
		}
		void setSize(size_t size)
		{
			bytes.reserve(size);
			bytes.resize(size, std::byte(0));
		}
		void copy(unsigned int a, unsigned int b, unsigned int size)
		{
			std::memcpy(&bytes[a], &bytes[b], size);
		}
	};

	class tsBytecode
	{
		
		std::stack<unsigned int> scope;
		unsigned int numVars = 0;
	public:
		tsBytes bytes;
		
		template<class T>
		void LOAD(unsigned int index, T value)
		{
			bytes.pushBack(tsLOAD);
			bytes.pushBack((unsigned int)sizeof(T));
			bytes.pushBack(index);
			bytes.pushBack(value);
		}
		template<class T>
		void MOVE(unsigned int var, unsigned int targetIndex)
		{
			bytes.pushBack(tsMOVE);
			bytes.pushBack((unsigned int)sizeof(T));
			bytes.pushBack(var);
			bytes.pushBack(targetIndex);
		}
		template<ValueType T1, ValueType T2>
		void CAST(unsigned int a, unsigned int b)
		{
			if (T1 == ValueType::tsInt && T2 == ValueType::tsFloat)
			{
				bytes.pushBack(tsItoF);
			}
			else if (T1 == ValueType::tsFloat && T2 == ValueType::tsInt)
			{

				bytes.pushBack(tsFtoI);
			}
			bytes.pushBack(a);
			bytes.pushBack(b);
		}
		void FLIPI(unsigned int var)
		{
			bytes.pushBack(tsFLIPI);
			bytes.pushBack(var);
		}
		void FLIPF(unsigned int var)
		{
			bytes.pushBack(tsFLIPF);
			bytes.pushBack(var);
		}
		void ADDI(unsigned int aIndex, unsigned int bIndex, unsigned int returnIndex)
		{
			bytes.pushBack(tsADDI);
			bytes.pushBack(aIndex);
			bytes.pushBack(bIndex);
			bytes.pushBack(returnIndex);
		}
		void ADDF(unsigned int aIndex, unsigned int bIndex, unsigned int returnIndex)
		{
			bytes.pushBack(tsADDF);
			bytes.pushBack(aIndex);
			bytes.pushBack(bIndex);
			bytes.pushBack(returnIndex);
		}
		void MULI(unsigned int aIndex, unsigned int bIndex, unsigned int returnIndex)
		{
			bytes.pushBack(tsMULI);
			bytes.pushBack(aIndex);
			bytes.pushBack(bIndex);
			bytes.pushBack(returnIndex);
		}
		void MULF(unsigned int aIndex, unsigned int bIndex, unsigned int returnIndex)
		{
			bytes.pushBack(tsMULF);
			bytes.pushBack(aIndex);
			bytes.pushBack(bIndex);
			bytes.pushBack(returnIndex);
		}
		void DIVI(unsigned int aIndex, unsigned int bIndex, unsigned int returnIndex)
		{
			bytes.pushBack(tsDIVI);
			bytes.pushBack(aIndex);
			bytes.pushBack(bIndex);
			bytes.pushBack(returnIndex);
		}
		void DIVF(unsigned int aIndex, unsigned int bIndex, unsigned int returnIndex)
		{
			bytes.pushBack(tsDIVF);
			bytes.pushBack(aIndex);
			bytes.pushBack(bIndex);
			bytes.pushBack(returnIndex);
		}
		void END()
		{
			bytes.pushBack(tsEND);
		}
	};


	class tsGlobal
	{
	public:
		enum class GlobalType
		{
			tsIn,
			tsRef
		};

		ValueType type;
		GlobalType writeMode;
		std::string identifier;
		unsigned int index;
	};

	class tsConst
	{
	public:
		ValueType type;
		std::string identifier;
		unsigned int index;
	};

	class tsScript
	{
	public:
		unsigned int numBytes;

		std::vector<tsGlobal> globals;
		tsBytecode bytecode;
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
		bool scriptLoaded;

		size_t cursor;
		tsBytes stack;

	public:
		tsRuntime(std::shared_ptr<tsContext>& context)
		{
			_context = context;
		}

		void LoadScript(unsigned int script)
		{
			loadedScript = script;
			cursor = 0;
			stack.clear();
			stack.setSize(_context->scripts[loadedScript].numBytes);
			ExecuteByteCode(_context->scripts[loadedScript].bytecode);
			scriptLoaded = true;
		}


		template<class T>
		void SetGlobal(std::string identifier, T value)
		{
			if (scriptLoaded)
			{
				tsScript& script = _context->scripts[loadedScript];
				for (unsigned int i = 0; i < script.globals.size(); i++)
				{
					if (script.globals[i].identifier == identifier)
					{
						SetGlobal(i, value);
						return;
					}
				}
				massert(false, "Could not find identifier: " + identifier);
			}
			else
				std::cout << "Could not set global, no loaded function.\n";
		}

		template<class T>
		void SetGlobal(unsigned int index, T value)
		{
			tsScript& script = _context->scripts[loadedScript];
			stack.set(script.globals[index].index, value);
			std::cout << "Global " << index << " at index " << script.globals[index].index << " set to " << stack.read<T>(script.globals[index].index) << std::endl;
		}

		template<class T>
		T GetGlobal(std::string identifier)
		{
			if (scriptLoaded)
			{
				tsScript& script = _context->scripts[loadedScript];
				for (unsigned int i = 0; i < script.globals.size(); i++)
				{
					if (script.globals[i].identifier == identifier)
					{
						return GetGlobal<T>(i);
					}
				}
				massert(false, "Could not find identifier: " + identifier);
			}
			else
				std::cout << "Could not set global, no loaded function.\n";
			return 0;
		}

		template<class T>
		T GetGlobal(unsigned int index)
		{
			tsScript& script = _context->scripts[loadedScript];
			return stack.read<T>(script.globals[index].index);
		}

		void Run()
		{
			std::cout << "Running script: " << loadedScript << " at cursor index: " << cursor << std::endl;
			size_t start = cursor;
			ExecuteByteCode(_context->scripts[loadedScript].bytecode);
			// Return the cursor to the start of the function after we complete it
			cursor = start;
		}

		void ExecuteByteCode(const tsBytecode& bytecode)
		{
			bool executing = true;
			while (executing && cursor < bytecode.bytes.size())
			{
				std::cout << "Executing code " << (int)bytecode.bytes.read<std::byte>(cursor) << " at index " << cursor << std::endl;
				switch (bytecode.bytes.read<std::byte>(cursor))
				{
					case tsEND:
					{
						executing = false;
						break;
					}
					case tsLOAD:
					{
						unsigned int size = bytecode.bytes.read<unsigned int>(++cursor);
						cursor += 4;
						unsigned int index = bytecode.bytes.read<unsigned int>(cursor);
						cursor += 3;
						for (int i = 0; i < size; i++)
						{
							stack.set(index + i, bytecode.bytes.read<std::byte>(++cursor));
						}
					}
						break;
					case tsMOVE:
					{
						unsigned int size = bytecode.bytes.read<unsigned int>(++cursor);
						cursor += 4;
						unsigned int index1 = bytecode.bytes.read<unsigned int>(cursor);
						cursor += 4;
						unsigned int index2 = bytecode.bytes.read<unsigned int>(cursor);
						cursor += 3;
						stack.copy(index2, index1, 4);
						std::cout << "Value of moved float(?): " << stack.read<float>(index1) << std::endl;
					}
						break;
					case tsFtoI:
					{
						unsigned int index1 = bytecode.bytes.read<unsigned int>(++cursor);
						cursor += 4;
						unsigned int index2 = bytecode.bytes.read<unsigned int>(cursor);
						cursor += 3;
						stack.set(index2, (int)(stack.read<float>(index1)));
					}
						break;
					case tsItoF:
					{
						unsigned int index1 = bytecode.bytes.read<unsigned int>(++cursor);
						cursor += 4;
						unsigned int index2 = bytecode.bytes.read<unsigned int>(cursor);
						cursor += 3;
						stack.set(index2, (float)(stack.read<int>(index1)));
					}
						break;
					case tsFLIPF:
					{
						unsigned int index1 = bytecode.bytes.read<unsigned int>(++cursor);
						cursor += 4;
						unsigned int index2 = bytecode.bytes.read<unsigned int>(++cursor);
						cursor += 3;
						stack.set(index2, -stack.read<float>(index1));
					}
						break;
					case tsADDF:
					{
						unsigned int a = bytecode.bytes.read<unsigned int>(++cursor);
						cursor += 4;
						unsigned int b = bytecode.bytes.read<unsigned int>(cursor);
						cursor += 4;
						unsigned int r = bytecode.bytes.read<unsigned int>(cursor);
						cursor += 3;
						std::cout << "Adding: " << stack.read<float>(a) << " to " << stack.read<float>(b) << std::endl;
						stack.set(r, stack.read<float>(a) + stack.read<float>(b));
						std::cout << "result of adding is: " << stack.read<float>(r) << std::endl;
						break;
					}
					case tsMULF:
					{
						unsigned int a = bytecode.bytes.read<unsigned int>(++cursor);
						cursor += 4;
						unsigned int b = bytecode.bytes.read<unsigned int>(cursor);
						cursor += 4;
						unsigned int r = bytecode.bytes.read<unsigned int>(cursor);
						cursor += 3;
						stack.set(r, stack.read<float>(a) * stack.read<float>(b));
					}
						break;
					case tsDIVF:
					{
						unsigned int a = bytecode.bytes.read<unsigned int>(++cursor);
						cursor += 4;
						unsigned int b = bytecode.bytes.read<unsigned int>(cursor);
						cursor += 4;
						unsigned int r = bytecode.bytes.read<unsigned int>(cursor);
						cursor += 3;
						stack.set(r, stack.read<float>(a) / stack.read<float>(b));
					}
					break;
					case tsFLIPI:
					{
						unsigned int index1 = bytecode.bytes.read<unsigned int>(++cursor);
						cursor += 4;
						unsigned int index2 = bytecode.bytes.read<unsigned int>(++cursor);
						cursor += 3;
						stack.set(index2, -stack.read<int>(index1));
					}
					break;
					case tsADDI:
					{
						unsigned int a = bytecode.bytes.read<unsigned int>(++cursor);
						cursor += 4;
						unsigned int b = bytecode.bytes.read<unsigned int>(cursor);
						cursor += 4;
						unsigned int r = bytecode.bytes.read<unsigned int>(cursor);
						cursor += 3;
						stack.set(r, stack.read<int>(a) + stack.read<int>(b));
						break;
					}
					case tsMULI:
					{
						unsigned int a = bytecode.bytes.read<unsigned int>(++cursor);
						cursor += 4;
						unsigned int b = bytecode.bytes.read<unsigned int>(cursor);
						cursor += 4;
						unsigned int r = bytecode.bytes.read<unsigned int>(cursor);
						cursor += 3;
						stack.set(r, stack.read<int>(a) * stack.read<int>(b));
					}
					break;
					case tsDIVI:
					{
						unsigned int a = bytecode.bytes.read<unsigned int>(++cursor);
						cursor += 4;
						unsigned int b = bytecode.bytes.read<unsigned int>(cursor);
						cursor += 4;
						unsigned int r = bytecode.bytes.read<unsigned int>(cursor);
						cursor += 3;
						stack.set(r, stack.read<int>(a) / stack.read<int>(b));
					}
					break;
					default:
					{
						massert(false, "Unknown byte code! " + std::to_string((unsigned int)bytecode.bytes.read<std::byte>(cursor)));
						//__assume(false);
						//break;
					}
				}
				++cursor;
			}
			
		}
	};
}