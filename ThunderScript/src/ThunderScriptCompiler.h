#pragma once
#include <vector>
#include <array>
#include <string>
#include <string_view>
#include <iostream>
#include <fstream>
#include <cstddef>
#include <map>
#include <stack>
#include "ThunderScript.h"

namespace ts
{
	// Thunder Script Reserved Words
	constexpr std::array<const std::string_view, 12> tsReservedWords = {
		// Best practices, put the longer ones first just in case
		"end", //To be removed, will be replaced with out structs
		"def",
		"int",
		"float",
		"bool",
		"if",
		"in",
		"ref",
		"true",
		"false",
		"while",
		"for"
	};
	constexpr unsigned int GetIndexOfReservedWord(const char token[])
	{
		for (unsigned int i = 0; i < tsReservedWords.size(); i++)
		{
			if (token == tsReservedWords[i])
				return i;
		}
		return 0;
	}
	// Thunder Script Operators
	constexpr std::array<const std::string_view, 25> tsOperators = {
		// Longer Operators must be defined first or they will be mistaken for shorter ones
		"++", "--", "+=", "-=",
		">=", "<=", "||", "&&", 
		"==",
		"(", ")", "{", "}", "[", "]",
		";", "+", "-", "=", "*", "/",
		"#", ">", "<",  "!"
	};
	constexpr unsigned int GetIndexOfOperator(const char token[])
	{
		int output = 0;
		for (unsigned int i = 0; i < tsOperators.size(); i++)
		{
			if (token == tsOperators[i])
			{
				output = i;
				break;
			}
		}
		return output;
	}

	class tsCompileError
	{
	public:
		std::string message;
		size_t line;
		tsCompileError(const std::string& _message, size_t _line)
		{
			message = _message;
			line = _line;
		}
		void display()
		{
			std::cout << "Compiler failed with message:\n" << message
				<< "\nOn line:\n" << line << std::endl;
		}
	};

	class tsToken
	{
	public:
		enum class Type
		{
			tsOperator,
			tsReservedWord,
			tsIdentifier,
		};
		Type type;
		std::string token;
		int tokenIndex;
		size_t line;
		tsToken(Type _type, std::string& _token, int _tokenIndex, size_t _line)
		{
			type = _type;
			token = _token;
			tokenIndex = _tokenIndex;
			line = _line;

		}
	};

	class tsVar
	{
	public:
		std::string identifier;
		unsigned int index;
		ValueType type;
		bool inUse;
		bool constant;
		bool initalized;
		int size;
	};

	class tsVarPool
	{
	private:
		std::vector<tsVar> vars;
		unsigned int bytes = 0;
		unsigned int tempIndex = 0;
		std::stack<std::vector<tsVar*>> scopes;
	public:
		void reset()
		{
			vars.clear();
			bytes = 0;
			tempIndex = 0;
			scopes.empty();
		}
		unsigned int sizeOf()
		{
			return bytes;
		}
		void enterScope()
		{
			scopes.push(std::vector<tsVar*>());
		}
		void exitScope()
		{
			for (unsigned int i = 0; i < scopes.top().size(); i++)
			{
				scopes.top()[i]->inUse = false;
			}
			scopes.pop();
		}
		tsVar requestTempVar(ValueType type)
		{
			tsVar r = requestVar(type, "temp " + std::to_string(tempIndex++), false, true);
			return r;
		}

		tsVar requestVar(ValueType type, const std::string& identifier, bool isConstant = false, bool isInitalized = false)
		{
			for (unsigned int i = 0; i < vars.size(); i++)
			{
				if (vars[i].type == type && !vars[i].inUse)
				{
					vars[i].inUse = true;
					vars[i].identifier = identifier;
					vars[i].constant = isConstant;
					vars[i].initalized = isInitalized;
					scopes.top().push_back(&vars[i]);
					return vars[i];
				}
			}
			tsVar var;
			switch (type)
			{
				case ValueType::tsInt:
					var.size = sizeof(int);
					break;
				case ValueType::tsFloat:
					var.size = sizeof(float);
					break;
				case ValueType::tsBool:
					var.size = sizeof(bool);
					break;
				default:
					massert(false, "Tried to create var of unimplemented type");
			}
			var.index = bytes;
			bytes += var.size;
			var.type = type;
			var.inUse = true;
			var.constant = isConstant;
			var.identifier = identifier;
			var.initalized = isInitalized;
			size_t index = vars.size();
			vars.push_back(var);
			scopes.top().push_back(&vars[index]);
			std::cout << "created var at index: " << var.index << std::endl;
			return var;
		}

		template<ValueType T>
		tsVar cast(tsBytecode& bytecode, tsVar& var)
		{
			if (var.type == T)
				return var;

			std::cout << "casing" << std::endl;
			tsVar newVar = requestTempVar(T);
			switch (var.type)
			{
				case ValueType::tsFloat:
					bytecode.CAST<ValueType::tsFloat, T>(var.index, newVar.index);
					break;
				case ValueType::tsInt:
					bytecode.CAST<ValueType::tsInt, T>(var.index, newVar.index);
					break;
				default:
					massert(false, "Unimplemented cast type " + std::to_string((int)var.type));
					break;
			}
			return newVar;
		}

		void initialize(tsVar var)
		{
			for (unsigned int i = 0; i < vars.size(); i++)
			{
				if (vars[i].inUse && var.identifier == vars[i].identifier)
				{
					vars[i].initalized = true;
					return;
				}
			}
		}
		bool getIndexOfIdentifier(std::string identifier, tsVar& var)
		{
			for (unsigned int i = 0; i < vars.size(); i++)
			{
				if (vars[i].inUse && identifier == vars[i].identifier)
				{
					var = vars[i];
					return true;
				}
			}
			return false;
		}
		
	};

	

	class tsIdentifier
	{
	public:
		enum class IdentType
		{
			tsInt,
			tsFloat,
			tsFunction
		};
		IdentType type;
		std::string token;
		tsIdentifier()
		{
			assert(false);
		}
		tsIdentifier(const tsIdentifier& ident)
		{
			token = ident.token;
			type = ident.type;
		}
		tsIdentifier(const std::string& _token, const IdentType _type)
		{
			token = _token;
			type = _type;
		}
	};

	class tsCompiler
	{
		std::shared_ptr<tsContext> _context;
		tsVarPool vars;
		size_t line;

	public:
		bool compile(const std::string& path, std::shared_ptr<tsContext>& context)
		{
			// Read the script file. Return if we can't find it.
			std::ifstream f;
			f.open(path);
			if (!f.is_open())
				return false;

			//Read the entire script into a string
			char c;
			std::string scriptText = "";
			while (f.get(c))
				scriptText += c;

			// Release the file
			f.close();

			vars.reset();
			line = 0;

			tsScript script;
			removeComments(scriptText);
			std::vector<tsToken> tokens;
			parseTokens(scriptText, tokens);
			
			vars.enterScope();
			processPreprocessors(tokens, script);
			std::cout << "Tokens: " << std::endl << std::endl;
			for (size_t i = 0; i < tokens.size(); i++)
			{
				std::cout << "Type: " << (unsigned int)tokens[i].type << " Token: " << tokens[i].token << std::endl;
			}
			
			_context = context;
			GenerateBytecode(tokens, script);
			script.numBytes = vars.sizeOf();
			context->scripts.push_back(script);

			return true;
		}

	private:

	#pragma region PreProcessing
		void processPreprocessors(std::vector<tsToken>& tokens, tsScript& script)
		{
			std::cout << "generating preprocessors" << std::endl;

			for (size_t i = 0; i < tokens.size(); i++)
			{
				std::cout << i << ": " << tokens[i].token << std::endl;
				if (tokens[i].token == "#")
				{
					std::cout << "found preprocessor" << std::endl;
					switch (tokens[i + 1].tokenIndex)
					{
						case GetIndexOfReservedWord("in"):
						{
							GenerateGlobal(i, tokens, script, tsGlobal::GlobalType::tsIn);
							tokens.erase(tokens.begin() + i, tokens.begin() + i + 4);
							i -= 1;
						}
						break;
						case GetIndexOfReservedWord("ref"):
						{
							GenerateGlobal(i, tokens, script, tsGlobal::GlobalType::tsRef);
							tokens.erase(tokens.begin() + i, tokens.begin() + i + 4);
							i -= 1;
						}
						break;
						default:
							throw tsCompileError("Unknown preprocessor: " + tokens[i + 1].token, tokens[i].line);
					}

				}
			}
		}

		void GenerateGlobal(const size_t& i, std::vector<tsToken>& tokens, tsScript& script, tsGlobal::GlobalType writeMode)
		{
			tsGlobal g;
			switch (tokens[i + 2].tokenIndex)
			{
				case GetIndexOfReservedWord("float"):
					g.type = ValueType::tsFloat;
					break;
				case GetIndexOfReservedWord("int"):
					g.type = ValueType::tsInt;
					break;
				case GetIndexOfReservedWord("bool"):
					g.type = ValueType::tsBool;
					break;
				default:
					throw tsCompileError("Unknown global type: " + tokens[i+2].token, tokens[i + 2].line);
			}
			g.identifier = tokens[i + 3].token;
			std::cout << "Found global: " << g.identifier << std::endl;
			tsVar var = vars.requestVar(g.type, g.identifier, tsGlobal::GlobalType::tsRef != writeMode, true);
			g.index = var.index;
			script.globals.push_back(g);

		}

		void removeComments(std::string& scriptText)
		{
			size_t len = scriptText.length();

			for (size_t i = 0; i < len - 1; i++)
			{

				std::string key({ scriptText[i],scriptText[i + 1] });
				if (key == "//")
				{
					std::cout << "found // comment" << std::endl;
					size_t length = scriptText.find("\n", i);
					while (i < length)
					{
						if (scriptText[i] != '\n')
						{
							scriptText.erase(scriptText.begin() + i);
							length--;
						}
						else
						{
							i++;
						}
					}
					scriptText.insert(i, " ");
					len = scriptText.length();
				}
				else if (key == "/*")
				{
					std::cout << "Found /**/ comment" << std::endl;
					size_t length = scriptText.find("*/", i) + 2;
					while (i < length)
					{
						if (scriptText[i] != '\n')
						{
							scriptText.erase(scriptText.begin() + i);
							length--;
						}
						else
						{
							i++;
						}
					}
					scriptText.insert(i, " ");
					len = scriptText.length();
				}
			}
		}
	#pragma endregion

	#pragma region Parser
		static void parseTokens(const std::string& scriptText, std::vector<tsToken>& tokens)
		{
			std::cout << "Parsing tokens" << std::endl;
			tokens.clear();
			size_t line = 1; // Text editors start at index 1 :(
			for (size_t i = 0; i < scriptText.length(); i++)
			{
				std::cout << "parsing index: " << i << std::endl;
				char currentChar = scriptText[i];// First we check if the current token is equal to any operators
				std::string token = "";
				int tokenIndex = 0;
				//Did we find the start of a token?
				if (currentChar != ' ' && currentChar != '\n' && currentChar != '\t')
				{
					// Is it an operator?
					if (isOperator(i, scriptText, tokenIndex))
					{
						std::cout << "found operator " << tsOperators[tokenIndex] << std::endl;
						token = tsOperators[tokenIndex];

						// If it is add it to the vector and increment i
						tokens.push_back(tsToken(tsToken::Type::tsOperator, token, tokenIndex, line));
						i += token.length() - 1;
					}
					// Then if it wasn't an operator check if it was a reserved word
					else if (isReservedWord(i, scriptText, tokenIndex))
					{
						std::cout << "found reserved word" << std::endl;

						token = tsReservedWords[tokenIndex];
						// If it is add it to the vector and increment i
						tokens.push_back(tsToken(tsToken::Type::tsReservedWord, token, tokenIndex, line));
						i += token.length() - 1;
					}
					// If it was not ether of those then it's an identifier or literal of some type
					else
					{
						std::cout << "found identifier" << std::endl;

						//Loop until we find a character that we know is not part of an identifier or literal
						int endTokenIndex = 0;
						while (i < scriptText.length()
							   && !(scriptText[i] == ' '
									|| scriptText[i] == '\n'
									|| isOperator(i, scriptText, endTokenIndex)))
						{
							token += scriptText[i];
							i++;
						}
						// We incremented one past our desired end point, we need to step back so the next token is parsed on the next loop.
						i--;
						// When the loop ends we know we have the whole token so we can add it to the array
						massert(token != "" && token != " ", "Somehow an empty identifier was found. This is wrong.");
						tokens.push_back(tsToken(tsToken::Type::tsIdentifier, token, -1, line));
						// If we found an end token we migh as well save ourselves some time and add it as well
						if (endTokenIndex >= 1)
						{
							std::string endToken = (std::string)tsOperators[endTokenIndex];
							i += endToken.length();
							tokens.push_back(tsToken(tsToken::Type::tsOperator, endToken, endTokenIndex, line));
						}
					}
				}
				if (currentChar == '\n')
					line++;
			}
		}

		static bool isOperator(size_t start, const std::string& script, int& token)
		{
			
			massert(start < script.length(), "Start must be less then script.length()");
			for (int o = 0; o < tsOperators.size(); o++)
			{
				size_t length = tsOperators[o].length();
				if (length + start - 1 < script.length()
					&& script.substr(start, length) == tsOperators[o])
				{
					token = o;
					return true;
				}
			}
			return false;
		}
		static bool isReservedWord(size_t start, const std::string& script, int& token)
		{
			
			for (int w = 0; w < tsReservedWords.size(); w++)
			{
				size_t length = tsReservedWords[w].length();
				if (length + start < script.length()
					&& script.substr(start, length) == tsReservedWords[w])
				{
					token = w;
					return true;
				}
			}
			return false;
		}
	#pragma endregion

	#pragma region expression operations
		// 0: variables, 1: add and subtract, 2: multiply and divide, 3: negate, 4: scope
		class tsOperation
		{
		public:
			enum class DepSide
			{
				none,
				left,
				right,
				both
			};
			size_t line;
			virtual unsigned int getPriority() = 0;
			virtual DepSide getDepSide()
			{
				return DepSide::none;
			};
			virtual tsVar getValue(tsBytecode& bytecode, tsVarPool& vars) = 0;
		};

		// An operation dependant upon one other operation
		class tsDOperator : public tsOperation
		{
		protected:
			std::unique_ptr<tsOperation> _dep;
		public:
			void setDep(std::unique_ptr<tsOperation>& dep)
			{
				_dep = std::move(dep);
			}
		};

		// An operator with a dependancy on the left
		class tsLOperator : public tsDOperator
		{
		public:
			DepSide getDepSide()
			{
				return DepSide::left;
			}
		};
		// An operator with a dependancy on the right
		class tsROperator : public tsDOperator
		{
		public:
			DepSide getDepSide()
			{
				return DepSide::right;
			}
		};

		// An operation that has two dependancies
		class tsDDOperator : public tsOperation
		{
		protected:
			std::unique_ptr<tsOperation> _dep1;
			std::unique_ptr<tsOperation> _dep2;
		public:
			DepSide getDepSide()
			{
				return DepSide::both;
			}
			ValueType GetStdEqType(const tsVar& a, const tsVar& b)
			{
				if (a.type == b.type)
					return a.type;
				if (a.type == ValueType::tsFloat || b.type == ValueType::tsFloat)
					return ValueType::tsFloat;
				else
					return ValueType::tsUnknown;
			}
			void setDeps(std::unique_ptr<tsOperation>& dep1, std::unique_ptr<tsOperation>& dep2)
			{
				massert((dep1.get() != nullptr) && (dep2.get() != nullptr), "Input pointer(s) were null")
				_dep1 = std::move(dep1);
				_dep2 = std::move(dep2);
			}
		};
		class tsScopeOperation : public tsOperation
		{
		public:
			std::unique_ptr<tsOperation> operation;
			unsigned int getPriority()
			{
				return 5;
			}
			tsVar getValue(tsBytecode& bytecode, tsVarPool& vars)
			{
				return operation->getValue(bytecode, vars);
			}
		};
		class tsVarOperation : public tsOperation
		{
		public:
			tsVar _var;
			tsVarOperation(tsVar& var)
			{
				_var = var;
			}
			unsigned int getPriority()
			{
				return 1;
			}
			tsVar getValue(tsBytecode& bytecode, tsVarPool& vars)
			{
				return _var;
			}
		};
		class tsAssignOperation : public tsDDOperator
		{
		public:
			unsigned int getPriority()
			{
				return 0;
			}
			tsVar getValue(tsBytecode& bytecode, tsVarPool& vars)
			{
				std::cout << "Writing assign code\n";
				vars.enterScope();
				tsVar a = _dep1->getValue(bytecode, vars);
				tsVar b = _dep2->getValue(bytecode, vars);
				vars.exitScope();
				
				if (a.constant)
					throw tsCompileError("Can not assign a value to constant var: " + a.identifier, line);

				if (!b.initalized)
					throw tsCompileError("Use of uninitalized variable: " + b.identifier, line);

				vars.initialize(a);

				switch (a.type)
				{
					case ValueType::tsFloat:
						b = vars.cast<ValueType::tsFloat>(bytecode, b);
						bytecode.pushCmd(tsMOVE, sizeof(float), b.index, a.index);
						break;
					case ValueType::tsInt:
						b = vars.cast<ValueType::tsInt>(bytecode, b);
						bytecode.pushCmd(tsMOVE, sizeof(int), b.index, a.index);
						break;
					case ValueType::tsBool:
						if(b.type != ValueType::tsBool)
							throw tsCompileError("Can not auto cast type " + static_cast<std::string>(getValueTypeName(b.type)) + " to boolian", line);
						bytecode.pushCmd(tsMOVE, sizeof(bool), b.index, a.index);
						break;
					default:
						throw tsCompileError("Can not assign type " + static_cast<std::string>(getValueTypeName(b.type)) + " to " + static_cast<std::string>(getValueTypeName(a.type)), line);
				}
				return a;
			}
		};
		class tsAddOperation : public tsDDOperator
		{
		public:
			unsigned int getPriority()
			{
				return 2;
			}
			tsVar getValue(tsBytecode& bytecode, tsVarPool& vars)
			{
				std::cout << "Getting value of add operation" << std::endl;
				vars.enterScope();
				tsVar a = _dep1->getValue(bytecode, vars);
				tsVar b = _dep2->getValue(bytecode, vars);
				vars.exitScope();
				ValueType type = a.type;


				if (!a.initalized)
					throw tsCompileError("Use of uninitalized variable: " + a.identifier, line);
				if (!b.initalized)
					throw tsCompileError("Use of uninitalized varialbe: " + b.identifier, line);

				tsVar result = vars.requestTempVar(type);
				switch (type)
				{
					case ValueType::tsFloat:
						a = vars.cast<ValueType::tsFloat>(bytecode, a);
						b = vars.cast<ValueType::tsFloat>(bytecode, b);
						bytecode.pushCmd(tsADDF, a.index, b.index, result.index);
						break;
					case ValueType::tsInt:
						a = vars.cast<ValueType::tsInt>(bytecode, a);
						b = vars.cast<ValueType::tsInt>(bytecode, b);
						bytecode.pushCmd(tsADDI, a.index, b.index, result.index);
						break;
				}
				return result;
			}
		};
		class tsNegateOperation : public tsROperator
		{
			unsigned int getPriority()
			{
				return 4;
			}
			tsVar getValue(tsBytecode& bytecode, tsVarPool& vars)
			{
				std::cout << "Writing negate code\n";
				vars.enterScope();
				tsVar a = _dep->getValue(bytecode, vars);
				vars.exitScope();
				ValueType type = a.type;

				if (!a.initalized)
					throw tsCompileError("Use of uninitalized variable: " + a.identifier, line);

				tsVar result = vars.requestTempVar(type);
				switch (type)
				{
					case ValueType::tsFloat:
						bytecode.pushCmd(tsFLIPF, a.index, result.index);
						break;
					case ValueType::tsInt:
						bytecode.pushCmd(tsFLIPI, a.index, result.index);
						break;
					default:
						throw tsCompileError("Invalid type for negate operation", line);
				}
				return result;
			}
		};
		class tsSubtractOperation : public tsDDOperator
		{
		public:
			unsigned int getPriority()
			{
				return 2;
			}
			tsVar getValue(tsBytecode& bytecode, tsVarPool& vars)
			{
				std::cout << "Getting value of subtract operation" << std::endl;
				massert((_dep1.get() != nullptr) && (_dep2.get() != nullptr), "Dependancy pointer(s) were null " + std::to_string(_dep1.get() != nullptr) + " " + std::to_string(_dep2.get() != nullptr));
				vars.enterScope();
				tsVar a = _dep1->getValue(bytecode, vars);
				tsVar b = _dep2->getValue(bytecode, vars);
				vars.exitScope();
				ValueType type = GetStdEqType(a, b);

				if (!a.initalized)
					throw tsCompileError("Use of uninitalized variable: " + a.identifier, line);
				if (!b.initalized)
					throw tsCompileError("Use of uninitalized varialbe: " + b.identifier, line);

				tsVar result = vars.requestTempVar(type);
				switch (type)
				{
					case ValueType::tsFloat:
						a = vars.cast<ValueType::tsFloat>(bytecode, a);
						b = vars.cast<ValueType::tsFloat>(bytecode, b);
						bytecode.pushCmd(tsFLIPF, b.index, b.index);
						bytecode.pushCmd(tsADDF, a.index, b.index, result.index);
						break;
					case ValueType::tsInt:
						a = vars.cast<ValueType::tsInt>(bytecode, a);
						b = vars.cast<ValueType::tsInt>(bytecode, b);
						bytecode.pushCmd(tsFLIPI, b.index, b.index);
						bytecode.pushCmd(tsADDI, a.index, b.index, result.index);
						break;
					default:
						throw tsCompileError("Invalid type for add operation.", line);
				}
				return result;
			}
		};
		class tsMultiplyOperation : public tsDDOperator
		{
			unsigned int getPriority()
			{
				return 3;
			}
			tsVar getValue(tsBytecode& bytecode, tsVarPool& vars)
			{
				std::cout << "Getting value of multiply operation" << std::endl;
				massert((_dep1.get() != nullptr) && (_dep2.get() != nullptr), "Dependancy pointer(s) were null " + std::to_string(_dep1.get() != nullptr) + " " + std::to_string(_dep2.get() != nullptr));
				vars.enterScope();
				tsVar a = _dep1->getValue(bytecode, vars);
				tsVar b = _dep2->getValue(bytecode, vars);
				vars.exitScope();
				ValueType type = GetStdEqType(a, b);

				if (!a.initalized)
					throw tsCompileError("Use of uninitalized variable: " + a.identifier, line);
				if (!b.initalized)
					throw tsCompileError("Use of uninitalized varialbe: " + b.identifier, line);

				tsVar result = vars.requestTempVar(type);
				switch (type)
				{
					case ValueType::tsFloat:
						a = vars.cast<ValueType::tsFloat>(bytecode, a);
						b = vars.cast<ValueType::tsFloat>(bytecode, b);
						bytecode.pushCmd(tsMULF, a.index, b.index, result.index);
						break;
					case ValueType::tsInt:
						a = vars.cast<ValueType::tsInt>(bytecode, a);
						b = vars.cast<ValueType::tsInt>(bytecode, b);
						bytecode.pushCmd(tsMULI, a.index, b.index, result.index);
						break;
					default:
						throw tsCompileError("Invalid type for multiply operation.", line);
				}
				return result;
			}
		};
		class tsDivideOperation : public tsDDOperator
		{
			unsigned int getPriority()
			{
				return 3;
			}

			tsVar getValue(tsBytecode& bytecode, tsVarPool& vars)
			{
				std::cout << "Getting value of divide operation" << std::endl;
				massert((_dep1.get() != nullptr) && (_dep2.get() != nullptr), "Dependancy pointer(s) were null " + std::to_string(_dep1.get() != nullptr) + " " + std::to_string(_dep2.get() != nullptr));
				vars.enterScope();
				tsVar a = _dep1->getValue(bytecode, vars);
				tsVar b = _dep2->getValue(bytecode, vars);
				vars.exitScope();
				ValueType type = GetStdEqType(a, b);

				if (!a.initalized)
					throw tsCompileError("Use of uninitalized variable: " + a.identifier, line);
				if (!b.initalized)
					throw tsCompileError("Use of uninitalized varialbe: " + b.identifier, line);

				tsVar result = vars.requestTempVar(type);
				switch (type)
				{
					case ValueType::tsFloat:
						a = vars.cast<ValueType::tsFloat>(bytecode, a);
						b = vars.cast<ValueType::tsFloat>(bytecode, b);
						bytecode.pushCmd(tsADDF,a.index, b.index, result.index);
						break;
					case ValueType::tsInt:
						a = vars.cast<ValueType::tsInt>(bytecode, a);
						b = vars.cast<ValueType::tsInt>(bytecode, b);
						bytecode.pushCmd(tsADDI, a.index, b.index, result.index);
						break;
					default:
						throw tsCompileError("Invalid type for divide operation.", line);
				}
				return result;
			}
		};
		class tsNotOperation : public tsROperator
		{
		public:
			unsigned int getPriority()
			{
				return 4;
			}
			tsVar getValue(tsBytecode& bytecode, tsVarPool& vars)
			{
				std::cout << "Writing negate code\n";
				vars.enterScope();
				tsVar a = _dep->getValue(bytecode, vars);
				vars.exitScope();
				ValueType type = a.type;

				if (!a.initalized)
					throw tsCompileError("Use of uninitalized variable: " + a.identifier, line);
				if (a.type != ValueType::tsBool)
					throw tsCompileError("Invalid type for not operation: " + (std::string)getValueTypeName(a.type), line);

				tsVar result = vars.requestTempVar(type);
				bytecode.pushCmd(tsNOT, a.index, result.index);
				return result;
			}
		};
		class tsAndOperation : public tsDDOperator
		{
			unsigned int getPriority()
			{
				return 3;
			}

			tsVar getValue(tsBytecode& bytecode, tsVarPool& vars)
			{
				std::cout << "Getting value of divide operation" << std::endl;
				massert((_dep1.get() != nullptr) && (_dep2.get() != nullptr), "Dependancy pointer(s) were null " + std::to_string(_dep1.get() != nullptr) + " " + std::to_string(_dep2.get() != nullptr));
				vars.enterScope();
				tsVar a = _dep1->getValue(bytecode, vars);
				tsVar b = _dep2->getValue(bytecode, vars);
				vars.exitScope();
				ValueType type = GetStdEqType(a, b);

				if (!a.initalized)
					throw tsCompileError("Use of uninitalized variable: " + a.identifier, line);
				if (!b.initalized)
					throw tsCompileError("Use of uninitalized varialbe: " + b.identifier, line);
				if (a.type != ValueType::tsBool)
					throw tsCompileError("Invalid type on left side of AND operation: ", line);
				if (a.type != ValueType::tsBool)
					throw tsCompileError("Invalid type on right side of AND operation: ", line);

				tsVar result = vars.requestTempVar(type);
				bytecode.pushCmd(tsAND, a.index, b.index, result.index);
				return result;
			}
		};
		class tsOrOperation : public tsDDOperator
		{
			unsigned int getPriority()
			{
				return 3;
			}

			tsVar getValue(tsBytecode& bytecode, tsVarPool& vars)
			{
				std::cout << "Getting value of divide operation" << std::endl;
				massert((_dep1.get() != nullptr) && (_dep2.get() != nullptr), "Dependancy pointer(s) were null " + std::to_string(_dep1.get() != nullptr) + " " + std::to_string(_dep2.get() != nullptr));
				vars.enterScope();
				tsVar a = _dep1->getValue(bytecode, vars);
				tsVar b = _dep2->getValue(bytecode, vars);
				vars.exitScope();
				ValueType type = GetStdEqType(a, b);

				if (!a.initalized)
					throw tsCompileError("Use of uninitalized variable: " + a.identifier, line);
				if (!b.initalized)
					throw tsCompileError("Use of uninitalized varialbe: " + b.identifier, line);
				if (a.type != ValueType::tsBool)
					throw tsCompileError("Invalid type on left side of OR operation: ", line);
				if (b.type != ValueType::tsBool)
					throw tsCompileError("Invalid type on right side of OR operation: ", line);

				tsVar result = vars.requestTempVar(type);
				bytecode.pushCmd(tsOR, a.index, b.index, result.index);
				return result;
			}
		};
		#pragma region Comparason Operations
		class tsEqualOperation : public tsDDOperator
		{
		public:
			unsigned int getPriority()
			{
				return 3;
			}

			tsVar getValue(tsBytecode& bytecode, tsVarPool& vars)
			{
				std::cout << "Getting value of divide operation" << std::endl;
				massert((_dep1.get() != nullptr) && (_dep2.get() != nullptr), "Dependancy pointer(s) were null " + std::to_string(_dep1.get() != nullptr) + " " + std::to_string(_dep2.get() != nullptr));
				vars.enterScope();
				tsVar a = _dep1->getValue(bytecode, vars);
				tsVar b = _dep2->getValue(bytecode, vars);
				vars.exitScope();

				if (!a.initalized)
					throw tsCompileError("Use of uninitalized variable: " + a.identifier, line);
				if (!b.initalized)
					throw tsCompileError("Use of uninitalized varialbe: " + b.identifier, line);

				ValueType type;

				if (a.type == ValueType::tsFloat || b.type == ValueType::tsFloat)
					type = ValueType::tsFloat;
				else if (a.type == ValueType::tsInt && b.type == ValueType::tsInt)
					type = ValueType::tsInt;
				else if (a.type == ValueType::tsBool && b.type == ValueType::tsBool)
					type = ValueType::tsBool;
				else
					throw tsCompileError("Invalid comparason operation between" + (std::string)getValueTypeName(a.type) + " and " + (std::string)getValueTypeName(b.type), line);


				tsVar result = vars.requestTempVar(type);
				switch (type)
				{
					case ValueType::tsFloat:
						a = vars.cast<ValueType::tsFloat>(bytecode, a);
						b = vars.cast<ValueType::tsFloat>(bytecode, b);
						bytecode.pushCmd(tsEqualF, a.index, b.index, result.index);
						break;
					case ValueType::tsInt:
						bytecode.pushCmd(tsEqualI, a.index, b.index, result.index);
						break;
					case ValueType::tsBool:
						bytecode.pushCmd(tsEqualB, a.index, b.index, result.index);
				}
				return result;
			}
		};
		class tsLessOperation : public tsDDOperator
		{
		public:
			unsigned int getPriority()
			{
				return 3;
			}

			tsVar getValue(tsBytecode& bytecode, tsVarPool& vars)
			{
				std::cout << "Getting value of divide operation" << std::endl;
				massert((_dep1.get() != nullptr) && (_dep2.get() != nullptr), "Dependancy pointer(s) were null " + std::to_string(_dep1.get() != nullptr) + " " + std::to_string(_dep2.get() != nullptr));
				vars.enterScope();
				tsVar a = _dep1->getValue(bytecode, vars);
				tsVar b = _dep2->getValue(bytecode, vars);
				vars.exitScope();

				if (!a.initalized)
					throw tsCompileError("Use of uninitalized variable: " + a.identifier, line);
				if (!b.initalized)
					throw tsCompileError("Use of uninitalized varialbe: " + b.identifier, line);

				ValueType type;

				if (a.type == ValueType::tsFloat || b.type == ValueType::tsFloat)
					type = ValueType::tsFloat;
				else if (a.type == ValueType::tsInt && b.type == ValueType::tsInt)
					type = ValueType::tsInt;
				else
					throw tsCompileError("Invalid comparason operation between" + (std::string)getValueTypeName(a.type) + " and " + (std::string)getValueTypeName(b.type), line);


				tsVar result = vars.requestTempVar(ValueType::tsBool);
				switch (type)
				{
					case ValueType::tsFloat:
						a = vars.cast<ValueType::tsFloat>(bytecode, a);
						b = vars.cast<ValueType::tsFloat>(bytecode, b);
						bytecode.pushCmd(tsLessF, a.index, b.index, result.index);
						break;
					case ValueType::tsInt:
						bytecode.pushCmd(tsLessI, a.index, b.index, result.index);
						break;
				}
				return result;
			}
		};
		class tsMoreOperation : public tsDDOperator
		{
		public:
			unsigned int getPriority()
			{
				return 3;
			}

			tsVar getValue(tsBytecode& bytecode, tsVarPool& vars)
			{
				std::cout << "Getting value of divide operation" << std::endl;
				massert((_dep1.get() != nullptr) && (_dep2.get() != nullptr), "Dependancy pointer(s) were null " + std::to_string(_dep1.get() != nullptr) + " " + std::to_string(_dep2.get() != nullptr));
				vars.enterScope();
				tsVar a = _dep1->getValue(bytecode, vars);
				tsVar b = _dep2->getValue(bytecode, vars);
				vars.exitScope();

				if (!a.initalized)
					throw tsCompileError("Use of uninitalized variable: " + a.identifier, line);
				if (!b.initalized)
					throw tsCompileError("Use of uninitalized varialbe: " + b.identifier, line);

				ValueType type;

				if (a.type == ValueType::tsFloat || b.type == ValueType::tsFloat)
					type = ValueType::tsFloat;
				else if (a.type == ValueType::tsInt && b.type == ValueType::tsInt)
					type = ValueType::tsInt;
				else
					throw tsCompileError("Invalid comparason operation between" + (std::string)getValueTypeName(a.type) + " and " + (std::string)getValueTypeName(b.type), line);


				tsVar result = vars.requestTempVar(ValueType::tsBool);
				switch (type)
				{
					case ValueType::tsFloat:
						a = vars.cast<ValueType::tsFloat>(bytecode, a);
						b = vars.cast<ValueType::tsFloat>(bytecode, b);
						bytecode.pushCmd(tsLessF, b.index, a.index, result.index);
						break;
					case ValueType::tsInt:
						bytecode.pushCmd(tsLessI, b.index, a.index, result.index);
						break;
				}
				return result;
			}
		};
		class tsLessEqualOperation : public tsDDOperator
		{
		public:
			unsigned int getPriority()
			{
				return 3;
			}

			tsVar getValue(tsBytecode& bytecode, tsVarPool& vars)
			{
				std::cout << "Getting value of divide operation" << std::endl;
				massert((_dep1.get() != nullptr) && (_dep2.get() != nullptr), "Dependancy pointer(s) were null " + std::to_string(_dep1.get() != nullptr) + " " + std::to_string(_dep2.get() != nullptr));
				vars.enterScope();
				tsVar a = _dep1->getValue(bytecode, vars);
				tsVar b = _dep2->getValue(bytecode, vars);
				vars.exitScope();

				if (!a.initalized)
					throw tsCompileError("Use of uninitalized variable: " + a.identifier, line);
				if (!b.initalized)
					throw tsCompileError("Use of uninitalized varialbe: " + b.identifier, line);

				ValueType type;

				if (a.type == ValueType::tsFloat || b.type == ValueType::tsFloat)
					type = ValueType::tsFloat;
				else if (a.type == ValueType::tsInt && b.type == ValueType::tsInt)
					type = ValueType::tsInt;
				else
					throw tsCompileError("Invalid comparason operation between" + (std::string)getValueTypeName(a.type) + " and " + (std::string)getValueTypeName(b.type), line);


				tsVar result = vars.requestTempVar(ValueType::tsBool);
				switch (type)
				{
					case ValueType::tsFloat:
						a = vars.cast<ValueType::tsFloat>(bytecode, a);
						b = vars.cast<ValueType::tsFloat>(bytecode, b);
						bytecode.pushCmd(tsLessEqualF, a.index, b.index, result.index);
						break;
					case ValueType::tsInt:
						bytecode.pushCmd(tsLessEqualI, a.index, b.index, result.index);
						break;
				}
				return result;
			}
		};
		class tsMoreEqualOperation : public tsDDOperator
		{
		public:
			unsigned int getPriority()
			{
				return 3;
			}

			tsVar getValue(tsBytecode& bytecode, tsVarPool& vars)
			{
				std::cout << "Getting value of divide operation" << std::endl;
				massert((_dep1.get() != nullptr) && (_dep2.get() != nullptr), "Dependancy pointer(s) were null " + std::to_string(_dep1.get() != nullptr) + " " + std::to_string(_dep2.get() != nullptr));
				vars.enterScope();
				tsVar a = _dep1->getValue(bytecode, vars);
				tsVar b = _dep2->getValue(bytecode, vars);
				vars.exitScope();

				if (!a.initalized)
					throw tsCompileError("Use of uninitalized variable: " + a.identifier, line);
				if (!b.initalized)
					throw tsCompileError("Use of uninitalized varialbe: " + b.identifier, line);

				ValueType type;

				if (a.type == ValueType::tsFloat || b.type == ValueType::tsFloat)
					type = ValueType::tsFloat;
				else if (a.type == ValueType::tsInt && b.type == ValueType::tsInt)
					type = ValueType::tsInt;
				else
					throw tsCompileError("Invalid comparason operation between" + (std::string)getValueTypeName(a.type) + " and " + (std::string)getValueTypeName(b.type), line);


				tsVar result = vars.requestTempVar(ValueType::tsBool);
				switch (type)
				{
					case ValueType::tsFloat:
						a = vars.cast<ValueType::tsFloat>(bytecode, a);
						b = vars.cast<ValueType::tsFloat>(bytecode, b);
						bytecode.pushCmd(tsLessEqualF, b.index, a.index, result.index);
						break;
					case ValueType::tsInt:
						bytecode.pushCmd(tsLessEqualI, b.index, a.index, result.index);
						break;
				}
				return result;
			}
		};
		#pragma endregion

	#pragma endregion

	#pragma region Bytecode Generation

		void GenerateBytecode(std::vector<tsToken> tokens, tsScript& script)
		{
			GenerateConstVars(tokens, script.bytecode);
			for (size_t i = 0; i < tokens.size(); i++)
			{
				//massert(tokens[i].type == tsToken::Type::tsReservedWord, "token was not of type reserved word, this means you haven't coded in error checking yet and you forgot how your own language works.");
				GenerateStatement(i, tokens, script.bytecode);
				/*
				switch (tokens[i].tokenIndex)
				{
					//case GetIndexOfReservedWord("def"):
						
						//GenerateFunction(i, tokens, script);
						//break;
					default:
						
						break;
				}*/
			}
		}

		void GenerateFunction(size_t& i, std::vector<tsToken> tokens, tsScript& script)
		{
			massert(tokens[i].token == "def", "Generate function called on " + tokens[i].token);
			i++;


			//For now we're going to ignore the paramaters of the function, because I'm impataent and want to get something working.
			//GetFunctionParams(++i, tokens, function);
			GenerateStatement(i, tokens, script.bytecode);

			//script.functions.push_back(function);
		}
		void GenerateConstVars(std::vector<tsToken>& tokens, tsBytecode& bytecode)
		{
			std::cout << "Generating constants" << std::endl;
			for(unsigned int token = 0; token < tokens.size(); token++)
			{
				if (tokens[token].type == tsToken::Type::tsIdentifier)
				{
					try
					{
						if (tokens[token].token.find(".") == std::string::npos)
						{
							int value = std::stoi(tokens[token].token); // try to cast string to int, if it fails it is not a int
							std::cout << "Found const int: " << value << std::endl;
							std::string key = "const int " + std::to_string(value);
							tsVar var;
							if(vars.getIndexOfIdentifier(key, var))
							{
								std::cout << "Found predifined const int" << std::endl;
							}
							else
							{
								
								std::cout << "Making new const int" << std::endl;
								unsigned int var = vars.requestVar(ValueType::tsInt, key, true, true).index;
								bytecode.LOAD(var, value);
							}
							tokens[token].token = key;
						}
						else
						{
							float value = std::stof(tokens[token].token); // try to cast string to float, if it fails it is not a float
							std::cout << "Found const float: " << value << std::endl;
							std::string key = "const float " + std::to_string(value);
							tsVar var;
							if (vars.getIndexOfIdentifier(key, var))
							{
								std::cout << "Found predifined const float" << std::endl;
							}
							else
							{

								std::cout << "Making new const float" << std::endl;
								unsigned int var = vars.requestVar(ValueType::tsFloat, key, true, true).index;
								bytecode.LOAD(var, value);
							}
							tokens[token].token = key;
						}
					}
					catch (...)
					{
					}
					
				}
				else if (tokens[token].type == tsToken::Type::tsReservedWord)
				{
					switch (tokens[token].tokenIndex)
					{
						case GetIndexOfReservedWord("true"):
						{
							std::cout << "Found const true: " << std::endl;
							std::string key = "const true";
							tsVar var;
							if (vars.getIndexOfIdentifier(key, var))
							{
								std::cout << "Found predifined const true" << std::endl;
							}
							else
							{

								std::cout << "Making new const true" << std::endl;
								unsigned int var = vars.requestVar(ValueType::tsBool, key, true, true).index;
								bytecode.LOAD(var, true);
							}
							tokens[token].type = tsToken::Type::tsIdentifier;
							tokens[token].token = key;
						}
							break;
						case GetIndexOfReservedWord("false"):
						{
							std::cout << "Found const false: " << std::endl;
							std::string key = "const false";
							tsVar var;
							if (vars.getIndexOfIdentifier(key, var))
							{
								std::cout << "Found predifined const false" << std::endl;
							}
							else
							{

								std::cout << "Making new const false" << std::endl;
								unsigned int var = vars.requestVar(ValueType::tsBool, key, true, true).index;
								bytecode.LOAD(var, false);
							}
							tokens[token].type = tsToken::Type::tsIdentifier;
							tokens[token].token = key;
						}
							break;
					}
				}
			}
			//We signal that we are done loading with a return;
			bytecode.pushCmd(tsEND);
		}
		/*
		void GetFunctionParams(size_t& i, const std::vector<tsToken>& tokens, tsFunction& function)
		{
			massert(tokens[i].token == "(", "Paramater block must start with a (");
			i++;
			while (i < tokens.size() && tokens[i].token != ")")
			{
				//paramater stuff here;
				i++;
			}
			massert(tokens[i].token == ")", "Paramater block must end with and )");
			i++;
		}
		*/
		void GenerateStatement(size_t& i, const std::vector<tsToken> tokens, tsBytecode& bytecode)
		{
			while (i < tokens.size() && tokens[i].token != "}")
			{
				line = tokens[i].line;
				switch (tokens[i].type)
				{
					case tsToken::Type::tsOperator:
						switch (tokens[i].tokenIndex)
						{
							case GetIndexOfOperator("{"):
							{

								//Store how many varibles are in the parent scope
								vars.enterScope();

								//enter a new scope
								std::cout << "Generating statement" << std::endl;
								GenerateStatement(++i, tokens, bytecode);
								//Delete the varible ids from the last scope
								vars.exitScope();
								break;
							}
							case GetIndexOfOperator(";"):
								break;
							default:
								massert(false, "Unknown statement operator: " + tokens[i].token);
								//GenerateExpression(0, i, tokens, bytecode);
								break;
						}
						break;
						
					case tsToken::Type::tsReservedWord:
						switch (tokens[i].tokenIndex)
						{
							case GetIndexOfReservedWord("float"):
								GenerateFloat(i, tokens, bytecode);
								break;
							case GetIndexOfReservedWord("int"):
								GenerateInt(i, tokens, bytecode);
								break;
							case GetIndexOfReservedWord("bool"):
								GenerateBool(i, tokens, bytecode);
								break;
							case GetIndexOfReservedWord("end"):
								GenerateEnd(i, tokens, bytecode);
								break;
							case GetIndexOfReservedWord("if"):
								GenerateIf(i, tokens, bytecode);
								break;
							case GetIndexOfReservedWord("while"):
								GenerateWhile(i, tokens, bytecode);
								break;
							default:
								throw tsCompileError("Unknown reserved word: " + tokens[i].token, line);
								//GenerateExpression
								break;
						}
						break;
						
					default:
						GenerateExpression(i, tokens, bytecode);
						break;
				}
				++i;
			}
			
			
		}

		unsigned int GenerateExpression(size_t& i, const std::vector<tsToken>& tokens, tsBytecode& bytecode)
		{
			std::unique_ptr<tsOperation> operation;
			size_t end = 0;
			while (tokens[i + end].token != ";") ++end;
			std::vector opTokens(tokens.begin() + i, tokens.begin() + i + end);
			std::cout << "Expression: ";
			for (size_t j = 0; j < opTokens.size(); j++)
			{
				std::cout << opTokens[j].token;
			}
			std::cout << std::endl;
			GenerateOperation(opTokens, operation);

			
			unsigned int returnVal = operation->getValue(bytecode, vars).index;
			
			i += end;
			return returnVal;

		}

		void GenerateOperation(const std::vector<tsToken>& tokens, std::unique_ptr<tsOperation>& operation)
		{
			std::vector<std::unique_ptr<tsOperation>> operations;
			for (size_t i = 0; i < tokens.size(); i++)
			{
				tsToken token = tokens[i];
				line = token.line;
				if (token.type == tsToken::Type::tsIdentifier)
				{
					tsVar var;
					if (vars.getIndexOfIdentifier(token.token, var))
					{
						std::cout << "found var: " << var.identifier << std::endl;
						operations.push_back(std::make_unique<tsVarOperation>(var));
					}
				}
				else if (token.type == tsToken::Type::tsOperator)
				{

					switch (tokens[i].tokenIndex)
					{
						case GetIndexOfOperator("("):
						{
							std::cout << "Found scoped operators: ";

							int pCount = 1;
							std::vector<tsToken> subTokens;
							i++;
							while (pCount > 0)
							{
								if (tokens[i].token == "(")
									pCount++;
								else if (tokens[i].token == ")")
									pCount--;

								if (pCount > 0)
								{
									subTokens.push_back(tokens[i]);
									std::cout << tokens[i].token;
									i++;
								}
								
							}
							std::cout << std::endl;
							std::unique_ptr<tsScopeOperation> subExpression = std::make_unique<tsScopeOperation>();
							GenerateOperation(subTokens, subExpression->operation);
							operations.push_back(std::move(subExpression));
						}
							break;
						case GetIndexOfOperator("+"):
							std::cout << "Found +" << std::endl;
							operations.push_back(std::make_unique<tsAddOperation>());
							break;
						case GetIndexOfOperator("-"):
							std::cout << "Found -" << std::endl;

							if(i == 0 || (tokens[i - 1].type == tsToken::Type::tsOperator && tokens[i - 1].token != ")"))
								operations.push_back(std::make_unique<tsNegateOperation>());
							else
								operations.push_back(std::make_unique<tsSubtractOperation>());
							break;
						case GetIndexOfOperator("*"):
							std::cout << "Found *" << std::endl;

							operations.push_back(std::make_unique<tsMultiplyOperation>());
							break;
						case GetIndexOfOperator("/"):
							std::cout << "Found /" << std::endl;

							operations.push_back(std::make_unique<tsDivideOperation>());
							break;
						case GetIndexOfOperator("&&"):
							std::cout << "Found &&" << std::endl;
							operations.push_back(std::make_unique<tsAddOperation>());
							break;
						case GetIndexOfOperator("||"):
							std::cout << "Found ||" << std::endl;
							operations.push_back(std::make_unique<tsOrOperation>());
							break;
						case GetIndexOfOperator("!"):
							std::cout << "Found !" << std::endl;
							operations.push_back(std::make_unique<tsNotOperation>());
							break;
						case GetIndexOfOperator("=="):
							std::cout << "Found ==" << std::endl;
							operations.push_back(std::make_unique<tsEqualOperation>());
							break;
						case GetIndexOfOperator("<"):
							std::cout << "Found <" << std::endl;
							operations.push_back(std::make_unique<tsLessOperation>());
							break;
						case GetIndexOfOperator(">"):
							std::cout << "Found >" << std::endl;
							operations.push_back(std::make_unique<tsMoreOperation>());
							break;
						case GetIndexOfOperator("<="):
							std::cout << "Found <=" << std::endl;
							operations.push_back(std::make_unique<tsLessEqualOperation>());
							break;
						case GetIndexOfOperator(">="):
							std::cout << "Found >=" << std::endl;
							operations.push_back(std::make_unique<tsMoreEqualOperation>());
							break;
						case GetIndexOfOperator("="):
							std::cout << "Found =" << std::endl;

							operations.push_back(std::make_unique<tsAssignOperation>());
							break;
						default:
							throw tsCompileError("Unknown operator: " + tokens[i].token, line);
					}
				}
				else
					throw tsCompileError("Unknown expression reserved word: " + tokens[i].token, line);
				if (operations.size() > 0)
				{
					std::cout << "operations size: " << operations.size() << std::endl;
					massert(operations.size() > 0, "size less then 1")
					operations[operations.size() - 1]->line = line;
				}
			}
			
			for (int priority = 5; priority >= 0; priority--)
			{
				for (unsigned int i = 0; i < operations.size(); i++)
				{
					if (operations[i] -> getPriority() == priority)
					{
						switch (operations[i] -> getDepSide())
						{
							case tsOperation::DepSide::both:
								std::cout << "Found double sided operator" << std::endl;

								static_cast<tsDDOperator*>(operations[i].get()) -> setDeps(operations[i - 1], operations[i + 1]);
								
								// We need to erase these pointers as they are no longer assigned
								// Need to erase +1 first since otherwise the index for it would be off
								operations.erase(operations.begin() + i + 1);
								operations.erase(operations.begin() + i - 1);
								// Need to step back because we deleted and index before this one
								i--;
								break;
							case tsOperation::DepSide::right:
								std::cout << "Found right sided operator" << std::endl;

								static_cast<tsROperator*>(operations[i].get())->setDep(operations[i + 1]);

								operations.erase(operations.begin() + i + 1);
								break;
							case tsOperation::DepSide::left:
								std::cout << "Found left sided operator" << std::endl;

								static_cast<tsROperator*>(operations[i].get())->setDep(operations[i - 1]);

								operations.erase(operations.begin() + i - 1);
								// Need to step back because we deleted and index before this one
								i--;
								break;
						}
					}
				}
			}
			operation = std::move(operations[0]);
		}

		bool GetIndexOfFunction(const std::string& id, unsigned int& index)
		{
			return false;
		}

		void GenerateIf(size_t& i, std::vector<tsToken> tokens, tsBytecode& bytecode)
		{
			assert(tokens[i].token == "if");
			if (tokens[++i].token != "(")
				throw tsCompileError("Expected \"(\" after if.", tokens[i].line);

			++i;
			int pCount = 1;
			std::vector<tsToken> subTokens;
			std::cout << "If condition: ";
			while (pCount > 0)
			{
				if (tokens[i].token == "(")
					pCount++;
				else if (tokens[i].token == ")")
					pCount--;

				if (pCount > 0)
				{
					subTokens.push_back(tokens[i]);
					std::cout << tokens[i].token;
					i++;
				}
			}
			std::cout << std::endl;
			std::unique_ptr<tsOperation> conditionOperation;
			GenerateOperation(subTokens, conditionOperation);
			tsVar condition = conditionOperation->getValue(bytecode, vars);

			if (condition.type != ValueType::tsBool)
				throw tsCompileError("condition of if statment must be bool", line);
			if (!condition.initalized)
				throw tsCompileError("condition of if statment must be initalized", line);

			size_t index = bytecode.JUMPF(condition.index);
			GenerateStatement(++i, tokens, bytecode);
			bytecode.bytes.set(index, bytecode.bytes.size());
		}

		void GenerateWhile(size_t& i, std::vector<tsToken> tokens, tsBytecode& bytecode)
		{
			assert(tokens[i].token == "while");
			if (tokens[++i].token != "(")
				throw tsCompileError("Expected \"(\" after if.", tokens[i].line);

			++i;
			int pCount = 1;
			std::vector<tsToken> subTokens;
			std::cout << "While condition: ";
			while (pCount > 0)
			{
				if (tokens[i].token == "(")
					pCount++;
				else if (tokens[i].token == ")")
					pCount--;

				if (pCount > 0)
				{
					subTokens.push_back(tokens[i]);
					std::cout << tokens[i].token;
					i++;
				}
			}
			std::cout << std::endl;
			size_t startIndex = bytecode.bytes.size();

			std::unique_ptr<tsOperation> conditionOperation;
			GenerateOperation(subTokens, conditionOperation);
			tsVar condition = conditionOperation->getValue(bytecode, vars);

			if (condition.type != ValueType::tsBool)
				throw tsCompileError("condition of if statment must be bool", line);
			if (!condition.initalized)
				throw tsCompileError("condition of if statment must be initalized", line);

			size_t endIndex = bytecode.JUMPF(condition.index);
			i += 2;
			GenerateStatement(i, tokens, bytecode);
			
			bytecode.GOTO(startIndex);
			bytecode.bytes.set(endIndex, bytecode.bytes.size());
			std::cout << "While start index: " << startIndex << std::endl;
			std::cout << "While end index: " << bytecode.bytes.read<size_t>( endIndex) << std::endl;
			/*
			operation
			if operation jump to end
			statment
			goto operation
			end
			*/
		}
		
		void GenerateBool(size_t& i, std::vector<tsToken> tokens, tsBytecode& bytecode)
		{
			assert(tokens[i].token == "bool");
			vars.requestVar(ValueType::tsBool, tokens[++i].token, false);
			if (tokens[++i].token != ";")
				GenerateExpression(--i, tokens, bytecode);
			else
				++i;
		}

		void GenerateFloat(size_t& i, std::vector<tsToken> tokens, tsBytecode& bytecode)
		{
			assert(tokens[i].token == "float");
			vars.requestVar(ValueType::tsFloat, tokens[++i].token, false);
			if (tokens[++i].token != ";")
				GenerateExpression(--i, tokens, bytecode);
			else
				++i;
		}

		void GenerateInt(size_t& i, std::vector<tsToken> tokens, tsBytecode& bytecode)
		{
			assert(tokens[i].token == "int");
			vars.requestVar(ValueType::tsInt, tokens[++i].token, false);
			if (tokens[++i].token != ";")
				GenerateExpression(--i, tokens, bytecode);
			else
				++i;
		}

		void GenerateEnd(size_t& i, std::vector<tsToken> tokens, tsBytecode& bytecode)
		{
			assert(tokens[i].token == "end");
			bytecode.pushCmd(tsEND);
		}
		
	#pragma endregion

	};

	

}