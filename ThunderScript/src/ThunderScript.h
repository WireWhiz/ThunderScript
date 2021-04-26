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

	//bytecode commands:

	#define tsEND       std::byte(0) // return values
	#define tsJUMP      std::byte(1) // goto a command index
	#define tsJUMPF		std::byte(2) // if a bool is false, goto an index
	#define tsItoF      std::byte(3) // cast an int to a float  
	#define tsFtoI      std::byte(4) // cast a float to an int
	#define tsLOAD      std::byte(5) // load bytes from code into memory
	#define tsMOVE      std::byte(6) // copy a set of bytes from one index to another
	#define tsFLIPI     std::byte(7) // flip an int
	#define tsADDI      std::byte(8) // add an int
	#define tsMULI      std::byte(9) // multiply an int
	#define tsDIVI      std::byte(10) // divide an int
	#define tsFLIPF     std::byte(11) // flip a float
	#define tsADDF      std::byte(12) // add a float
	#define tsMULF      std::byte(13) // multiply a float
	#define tsDIVF      std::byte(14) // divide a float
	#define tsNOT       std::byte(15) // invert a bool
	#define tsAND       std::byte(16) // and operation on two bools
	#define tsOR        std::byte(17) // or operation on two bools
	#define tsLessI     std::byte(18) // float less then operation
	#define tsLessF     std::byte(19) // int less then operation
	#define tsLessEqualI std::byte(20) // float less then or equal to operation
	#define tsLessEqualF std::byte(21) // int less then or equal to operation
	#define tsEqualI    std::byte(22) // Compare ints
	#define tsEqualF    std::byte(23) // Compare ints
	#define tsEqualB    std::byte(24) // XAND 

	

	enum class ValueType
	{
		tsUnknown,
		tsInt,
		tsFloat,
		tsBool
	};

	constexpr std::string_view getValueTypeName(ValueType type)
	{
		switch (type)
		{
			case ValueType::tsFloat:
				return "float";
			case ValueType::tsInt:
				return "int";
			case ValueType::tsBool:
				return "bool";
			default:
				return "Unknown type";
		}
	}

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
			massert(index >= 0 && index < bytes.size() + 1 - sizeof(T), "Index " + std::to_string(index) + " out of byte range");
			std::memcpy(&bytes[index], &value, sizeof(T));
		}
		template <class T>
		T read(size_t index) const
		{
			massert(index >= 0 && index < bytes.size() + 1 - sizeof(T), "Index " + std::to_string(index) + " out of byte range");
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
			massert(a >= 0 && a <= bytes.size() - size + 1, "Index " + std::to_string(a) + " out of byte range");
			massert(b >= 0 && b <= bytes.size() - size + 1, "Index " + std::to_string(b) + " out of byte range");
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
		size_t JUMPF(unsigned int condition)
		{
			bytes.pushBack(tsJUMPF);
			bytes.pushBack(condition);
			size_t gotoIndex = bytes.size();
			bytes.pushBack((size_t)(0));
			return gotoIndex;
		}
		void GOTO(size_t index)
		{
			bytes.pushBack(tsJUMP);
			bytes.pushBack(index);
		}
		void pushCmd(std::byte c)
		{
			bytes.pushBack(c);
		}
		void pushCmd(std::byte c, unsigned int i, unsigned int r)
		{
			bytes.pushBack(c);
			bytes.pushBack(i);
			bytes.pushBack(r);
		}
		void pushCmd(std::byte c, unsigned int a, unsigned int b, unsigned int r)
		{
			bytes.pushBack(c);
			bytes.pushBack(a);
			bytes.pushBack(b);
			bytes.pushBack(r);
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
				//std::cout << "Executing code " << (int)bytecode.bytes.read<std::byte>(cursor) << " at index " << cursor << std::endl;
				switch (bytecode.bytes.read<std::byte>(cursor))
				{
					case tsEND:
						executing = false;
						break;
					case tsJUMP:
					{
						size_t index = bytecode.bytes.read<size_t>(++cursor);
						cursor = index - 1;
					}
						break;
					case tsJUMPF:
					{
						unsigned int condition = bytecode.bytes.read<unsigned int>(++cursor);
						cursor += 4;
						size_t index = bytecode.bytes.read<size_t>(cursor);
						//std::cout << sizeof(size_t);
						cursor += 7;
						if (!stack.read<bool>(condition))
						{
							cursor = index - 1;
						}
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
						stack.copy(index2, index1, size);
						if (size == 4 && false)
						{
							std::cout << "Value of moved float: " << stack.read<float>(index1) << std::endl;
							std::cout << "Value of moved int: " << stack.read<int>(index1) << std::endl;
							std::cout << "Value of moved bool: " << stack.read<bool>(index1) << std::endl;
						}
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
						unsigned int index2 = bytecode.bytes.read<unsigned int>(cursor);
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
						//std::cout << "Adding: " << stack.read<float>(a) << " to " << stack.read<float>(b) << std::endl;
						stack.set(r, stack.read<float>(a) + stack.read<float>(b));
						//std::cout << "result of adding is: " << stack.read<float>(r) << std::endl;
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
						unsigned int index2 = bytecode.bytes.read<unsigned int>(cursor);
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
					case tsNOT:
					{
						unsigned int a = bytecode.bytes.read<unsigned int>(++cursor);
						cursor += 4;
						unsigned int r = bytecode.bytes.read<unsigned int>(cursor);
						cursor += 3;
						stack.set(r, !stack.read<bool>(a));
					}
					break;
					case tsAND:
					{
						unsigned int a = bytecode.bytes.read<unsigned int>(++cursor);
						cursor += 4;
						unsigned int b = bytecode.bytes.read<unsigned int>(cursor);
						cursor += 4;
						unsigned int r = bytecode.bytes.read<unsigned int>(cursor);
						cursor += 3;
						stack.set(r, stack.read<bool>(a) && stack.read<bool>(b));
					}
					break;
					case tsOR:
					{
						unsigned int a = bytecode.bytes.read<unsigned int>(++cursor);
						cursor += 4;
						unsigned int b = bytecode.bytes.read<unsigned int>(cursor);
						cursor += 4;
						unsigned int r = bytecode.bytes.read<unsigned int>(cursor);
						cursor += 3;
						stack.set(r, stack.read<bool>(a) || stack.read<bool>(b));
					}
					break;
					case tsEqualI:
					{
						unsigned int a = bytecode.bytes.read<unsigned int>(++cursor);
						cursor += 4;
						unsigned int b = bytecode.bytes.read<unsigned int>(cursor);
						cursor += 4;
						unsigned int r = bytecode.bytes.read<unsigned int>(cursor);
						cursor += 3;
						stack.set(r, stack.read<int>(a) == stack.read<int>(b));
					}
						break;
					case tsEqualF:
					{
						unsigned int a = bytecode.bytes.read<unsigned int>(++cursor);
						cursor += 4;
						unsigned int b = bytecode.bytes.read<unsigned int>(cursor);
						cursor += 4;
						unsigned int r = bytecode.bytes.read<unsigned int>(cursor);
						cursor += 3;
						stack.set(r, stack.read<float>(a) == stack.read<float>(b));
					}
					break;
					case tsEqualB:
					{
						unsigned int a = bytecode.bytes.read<unsigned int>(++cursor);
						cursor += 4;
						unsigned int b = bytecode.bytes.read<unsigned int>(cursor);
						cursor += 4;
						unsigned int r = bytecode.bytes.read<unsigned int>(cursor);
						cursor += 3;
						stack.set(r, stack.read<bool>(a) == stack.read<bool>(b));
					}
					break;
					case tsLessI:
					{
						unsigned int a = bytecode.bytes.read<unsigned int>(++cursor);
						cursor += 4;
						unsigned int b = bytecode.bytes.read<unsigned int>(cursor);
						cursor += 4;
						unsigned int r = bytecode.bytes.read<unsigned int>(cursor);
						cursor += 3;
						stack.set(r, stack.read<int>(a) < stack.read<int>(b));
					}
					break;
					case tsLessF:
					{
						unsigned int a = bytecode.bytes.read<unsigned int>(++cursor);
						cursor += 4;
						unsigned int b = bytecode.bytes.read<unsigned int>(cursor);
						cursor += 4;
						unsigned int r = bytecode.bytes.read<unsigned int>(cursor);
						cursor += 3;
						stack.set(r, stack.read<float>(a) < stack.read<float>(b));
					}
					break;
					case tsLessEqualI:
					{
						unsigned int a = bytecode.bytes.read<unsigned int>(++cursor);
						cursor += 4;
						unsigned int b = bytecode.bytes.read<unsigned int>(cursor);
						cursor += 4;
						unsigned int r = bytecode.bytes.read<unsigned int>(cursor);
						cursor += 3;
						stack.set(r, stack.read<int>(a) <= stack.read<int>(b));
					}
					break;
					case tsLessEqualF:
					{
						unsigned int a = bytecode.bytes.read<unsigned int>(++cursor);
						cursor += 4;
						unsigned int b = bytecode.bytes.read<unsigned int>(cursor);
						cursor += 4;
						unsigned int r = bytecode.bytes.read<unsigned int>(cursor);
						cursor += 3;
						stack.set(r, stack.read<float>(a) <= stack.read<float>(b));
					}
					break;
					default:
					{
						//massert(false, "Unknown byte code! " + std::to_string((unsigned int)bytecode.bytes.read<std::byte>(cursor)));
						__assume(false);
						//break;
					}
				}
				++cursor;
			}
			
		}
	};
}