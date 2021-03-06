// A Bison parser, made by GNU Bison 3.5.1.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015, 2018-2020 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

// Undocumented macros, especially those whose name start with YY_,
// are private implementation details.  Do not rely on them.



// First part of user prologue.
#line 31 "bison.y"

#include "../src/ThunderScriptCompiler.h"
#include "FlexLexer.h"
#include <stdio.h>

#undef yylex
#define yylex scanner.yylex

#line 49 "bison.tab.cc"


#include "bison.tab.hh"




#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

// Whether we are compiled with exception support.
#ifndef YY_EXCEPTIONS
# if defined __GNUC__ && !defined __EXCEPTIONS
#  define YY_EXCEPTIONS 0
# else
#  define YY_EXCEPTIONS 1
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (false)
# endif


// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << '\n';                       \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yystack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YYUSE (Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void> (0)
# define YY_STACK_PRINT()                static_cast<void> (0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

#line 7 "bison.y"
namespace ts {
#line 146 "bison.tab.cc"


  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  tsParser::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr;
        char const *yyp = yystr;

        for (;;)
          switch (*++yyp)
            {
            case '\'':
            case ',':
              goto do_not_strip_quotes;

            case '\\':
              if (*++yyp != '\\')
                goto do_not_strip_quotes;
              else
                goto append;

            append:
            default:
              yyr += *yyp;
              break;

            case '"':
              return yyr;
            }
      do_not_strip_quotes: ;
      }

    return yystr;
  }


  /// Build a parser object.
  tsParser::tsParser (tsScanner  &scanner_yyarg, tsCompiler  &compiler_yyarg)
#if YYDEBUG
    : yydebug_ (false),
      yycdebug_ (&std::cerr),
#else
    :
#endif
      scanner (scanner_yyarg),
      compiler (compiler_yyarg)
  {}

  tsParser::~tsParser ()
  {}

  tsParser::syntax_error::~syntax_error () YY_NOEXCEPT YY_NOTHROW
  {}

  /*---------------.
  | Symbol types.  |
  `---------------*/

  // basic_symbol.
#if 201103L <= YY_CPLUSPLUS
  template <typename Base>
  tsParser::basic_symbol<Base>::basic_symbol (basic_symbol&& that)
    : Base (std::move (that))
    , value ()
    , location (std::move (that.location))
  {
    switch (this->type_get ())
    {
      case 47: // expStart
      case 48: // expression
      case 49: // value
        value.move< size_t > (std::move (that.value));
        break;

      case 6: // tstCONST_INT
      case 7: // tstCONST_FLOAT
      case 8: // tstCONST_STRING
      case 19: // tstIDENTIFIER
        value.move< std::string > (std::move (that.value));
        break;

      default:
        break;
    }

  }
#endif

  template <typename Base>
  tsParser::basic_symbol<Base>::basic_symbol (const basic_symbol& that)
    : Base (that)
    , value ()
    , location (that.location)
  {
    switch (this->type_get ())
    {
      case 47: // expStart
      case 48: // expression
      case 49: // value
        value.copy< size_t > (YY_MOVE (that.value));
        break;

      case 6: // tstCONST_INT
      case 7: // tstCONST_FLOAT
      case 8: // tstCONST_STRING
      case 19: // tstIDENTIFIER
        value.copy< std::string > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

  }



  template <typename Base>
  bool
  tsParser::basic_symbol<Base>::empty () const YY_NOEXCEPT
  {
    return Base::type_get () == empty_symbol;
  }

  template <typename Base>
  void
  tsParser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move (s);
    switch (this->type_get ())
    {
      case 47: // expStart
      case 48: // expression
      case 49: // value
        value.move< size_t > (YY_MOVE (s.value));
        break;

      case 6: // tstCONST_INT
      case 7: // tstCONST_FLOAT
      case 8: // tstCONST_STRING
      case 19: // tstIDENTIFIER
        value.move< std::string > (YY_MOVE (s.value));
        break;

      default:
        break;
    }

    location = YY_MOVE (s.location);
  }

  // by_type.
  tsParser::by_type::by_type ()
    : type (empty_symbol)
  {}

#if 201103L <= YY_CPLUSPLUS
  tsParser::by_type::by_type (by_type&& that)
    : type (that.type)
  {
    that.clear ();
  }
#endif

  tsParser::by_type::by_type (const by_type& that)
    : type (that.type)
  {}

  tsParser::by_type::by_type (token_type t)
    : type (yytranslate_ (t))
  {}

  void
  tsParser::by_type::clear ()
  {
    type = empty_symbol;
  }

  void
  tsParser::by_type::move (by_type& that)
  {
    type = that.type;
    that.clear ();
  }

  int
  tsParser::by_type::type_get () const YY_NOEXCEPT
  {
    return type;
  }


  // by_state.
  tsParser::by_state::by_state () YY_NOEXCEPT
    : state (empty_state)
  {}

  tsParser::by_state::by_state (const by_state& that) YY_NOEXCEPT
    : state (that.state)
  {}

  void
  tsParser::by_state::clear () YY_NOEXCEPT
  {
    state = empty_state;
  }

  void
  tsParser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  tsParser::by_state::by_state (state_type s) YY_NOEXCEPT
    : state (s)
  {}

  tsParser::symbol_number_type
  tsParser::by_state::type_get () const YY_NOEXCEPT
  {
    if (state == empty_state)
      return empty_symbol;
    else
      return yystos_[+state];
  }

  tsParser::stack_symbol_type::stack_symbol_type ()
  {}

  tsParser::stack_symbol_type::stack_symbol_type (YY_RVREF (stack_symbol_type) that)
    : super_type (YY_MOVE (that.state), YY_MOVE (that.location))
  {
    switch (that.type_get ())
    {
      case 47: // expStart
      case 48: // expression
      case 49: // value
        value.YY_MOVE_OR_COPY< size_t > (YY_MOVE (that.value));
        break;

      case 6: // tstCONST_INT
      case 7: // tstCONST_FLOAT
      case 8: // tstCONST_STRING
      case 19: // tstIDENTIFIER
        value.YY_MOVE_OR_COPY< std::string > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

#if 201103L <= YY_CPLUSPLUS
    // that is emptied.
    that.state = empty_state;
#endif
  }

  tsParser::stack_symbol_type::stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) that)
    : super_type (s, YY_MOVE (that.location))
  {
    switch (that.type_get ())
    {
      case 47: // expStart
      case 48: // expression
      case 49: // value
        value.move< size_t > (YY_MOVE (that.value));
        break;

      case 6: // tstCONST_INT
      case 7: // tstCONST_FLOAT
      case 8: // tstCONST_STRING
      case 19: // tstIDENTIFIER
        value.move< std::string > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

    // that is emptied.
    that.type = empty_symbol;
  }

#if YY_CPLUSPLUS < 201103L
  tsParser::stack_symbol_type&
  tsParser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    switch (that.type_get ())
    {
      case 47: // expStart
      case 48: // expression
      case 49: // value
        value.copy< size_t > (that.value);
        break;

      case 6: // tstCONST_INT
      case 7: // tstCONST_FLOAT
      case 8: // tstCONST_STRING
      case 19: // tstIDENTIFIER
        value.copy< std::string > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    return *this;
  }

  tsParser::stack_symbol_type&
  tsParser::stack_symbol_type::operator= (stack_symbol_type& that)
  {
    state = that.state;
    switch (that.type_get ())
    {
      case 47: // expStart
      case 48: // expression
      case 49: // value
        value.move< size_t > (that.value);
        break;

      case 6: // tstCONST_INT
      case 7: // tstCONST_FLOAT
      case 8: // tstCONST_STRING
      case 19: // tstIDENTIFIER
        value.move< std::string > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    // that is emptied.
    that.state = empty_state;
    return *this;
  }
#endif

  template <typename Base>
  void
  tsParser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);
  }

#if YYDEBUG
  template <typename Base>
  void
  tsParser::yy_print_ (std::ostream& yyo,
                                     const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YYUSE (yyoutput);
    symbol_number_type yytype = yysym.type_get ();
#if defined __GNUC__ && ! defined __clang__ && ! defined __ICC && __GNUC__ * 100 + __GNUC_MINOR__ <= 408
    // Avoid a (spurious) G++ 4.8 warning about "array subscript is
    // below array bounds".
    if (yysym.empty ())
      std::abort ();
#endif
    yyo << (yytype < yyntokens_ ? "token" : "nterm")
        << ' ' << yytname_[yytype] << " ("
        << yysym.location << ": ";
    YYUSE (yytype);
    yyo << ')';
  }
#endif

  void
  tsParser::yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym)
  {
    if (m)
      YY_SYMBOL_PRINT (m, sym);
    yystack_.push (YY_MOVE (sym));
  }

  void
  tsParser::yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym)
  {
#if 201103L <= YY_CPLUSPLUS
    yypush_ (m, stack_symbol_type (s, std::move (sym)));
#else
    stack_symbol_type ss (s, sym);
    yypush_ (m, ss);
#endif
  }

  void
  tsParser::yypop_ (int n)
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  tsParser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  tsParser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  tsParser::debug_level_type
  tsParser::debug_level () const
  {
    return yydebug_;
  }

  void
  tsParser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  tsParser::state_type
  tsParser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - yyntokens_] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - yyntokens_];
  }

  bool
  tsParser::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  bool
  tsParser::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  tsParser::operator() ()
  {
    return parse ();
  }

  int
  tsParser::parse ()
  {
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The locations where the error started and ended.
    stack_symbol_type yyerror_range[3];

    /// The return value of parse ().
    int yyresult;

#if YY_EXCEPTIONS
    try
#endif // YY_EXCEPTIONS
      {
    YYCDEBUG << "Starting parse\n";


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, YY_MOVE (yyla));

  /*-----------------------------------------------.
  | yynewstate -- push a new symbol on the stack.  |
  `-----------------------------------------------*/
  yynewstate:
    YYCDEBUG << "Entering state " << int (yystack_[0].state) << '\n';

    // Accept?
    if (yystack_[0].state == yyfinal_)
      YYACCEPT;

    goto yybackup;


  /*-----------.
  | yybackup.  |
  `-----------*/
  yybackup:
    // Try to take a decision without lookahead.
    yyn = yypact_[+yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token: ";
#if YY_EXCEPTIONS
        try
#endif // YY_EXCEPTIONS
          {
            yyla.type = yytranslate_ (yylex (&yyla.value, &yyla.location));
          }
#if YY_EXCEPTIONS
        catch (const syntax_error& yyexc)
          {
            YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
            error (yyexc);
            goto yyerrlab1;
          }
#endif // YY_EXCEPTIONS
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.type_get ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.type_get ())
      {
        goto yydefault;
      }

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", state_type (yyn), YY_MOVE (yyla));
    goto yynewstate;


  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[+yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;


  /*-----------------------------.
  | yyreduce -- do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_ (yystack_[yylen].state, yyr1_[yyn]);
      /* Variants are always initialized to an empty instance of the
         correct type. The default '$$ = $1' action is NOT applied
         when using variants.  */
      switch (yyr1_[yyn])
    {
      case 47: // expStart
      case 48: // expression
      case 49: // value
        yylhs.value.emplace< size_t > ();
        break;

      case 6: // tstCONST_INT
      case 7: // tstCONST_FLOAT
      case 8: // tstCONST_STRING
      case 19: // tstIDENTIFIER
        yylhs.value.emplace< std::string > ();
        break;

      default:
        break;
    }


      // Default location.
      {
        stack_type::slice range (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, range, yylen);
        yyerror_range[1].location = yylhs.location;
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
#if YY_EXCEPTIONS
      try
#endif // YY_EXCEPTIONS
        {
          switch (yyn)
            {
  case 4:
#line 96 "bison.y"
                                            {compiler.generateVar(yystack_[0].value.as < std::string > (), tsVarType::tsBool);}
#line 770 "bison.tab.cc"
    break;

  case 5:
#line 97 "bison.y"
                                                                       {tsVar var = compiler.generateVar(yystack_[1].value.as < std::string > (), tsVarType::tsBool);;}
#line 776 "bison.tab.cc"
    break;

  case 6:
#line 98 "bison.y"
                                                           {compiler.generateVar(yystack_[0].value.as < std::string > (), tsVarType::tsInt);}
#line 782 "bison.tab.cc"
    break;

  case 7:
#line 99 "bison.y"
                                                                          {tsVar var = compiler.generateVar(yystack_[2].value.as < std::string > (), tsVarType::tsInt); compiler.assignVar(var.varIndex, yystack_[0].value.as < size_t > (), scanner.lineno());}
#line 788 "bison.tab.cc"
    break;

  case 8:
#line 100 "bison.y"
                                                             {compiler.generateVar(yystack_[0].value.as < std::string > (), tsVarType::tsFloat);}
#line 794 "bison.tab.cc"
    break;

  case 9:
#line 101 "bison.y"
                                                               {printf("Found assign expression "); compiler.assignVar(compiler.getVarIndex(yystack_[2].value.as < std::string > ()), yystack_[0].value.as < size_t > (), scanner.lineno());}
#line 800 "bison.tab.cc"
    break;

  case 13:
#line 108 "bison.y"
                                                                    {printf("Found scope");}
#line 806 "bison.tab.cc"
    break;

  case 23:
#line 129 "bison.y"
                                                         {compiler.generateGlobal(yystack_[0].value.as < std::string > (), tsVarType::tsInt, tsGlobal::GlobalType::tsRef, scanner.lineno());}
#line 812 "bison.tab.cc"
    break;

  case 24:
#line 130 "bison.y"
                                                                          {compiler.generateGlobal(yystack_[0].value.as < std::string > (), tsVarType::tsBool, tsGlobal::GlobalType::tsRef, scanner.lineno());}
#line 818 "bison.tab.cc"
    break;

  case 25:
#line 131 "bison.y"
                                                                           {compiler.generateGlobal(yystack_[0].value.as < std::string > (), tsVarType::tsFloat, tsGlobal::GlobalType::tsRef, scanner.lineno());}
#line 824 "bison.tab.cc"
    break;

  case 26:
#line 132 "bison.y"
                                                                        {compiler.generateGlobal(yystack_[0].value.as < std::string > (), tsVarType::tsInt, tsGlobal::GlobalType::tsIn, scanner.lineno());}
#line 830 "bison.tab.cc"
    break;

  case 27:
#line 133 "bison.y"
                                                                         {compiler.generateGlobal(yystack_[0].value.as < std::string > (), tsVarType::tsBool, tsGlobal::GlobalType::tsIn, scanner.lineno());}
#line 836 "bison.tab.cc"
    break;

  case 28:
#line 134 "bison.y"
                                                                          {compiler.generateGlobal(yystack_[0].value.as < std::string > (), tsVarType::tsFloat, tsGlobal::GlobalType::tsIn, scanner.lineno());}
#line 842 "bison.tab.cc"
    break;

  case 29:
#line 137 "bison.y"
                                           {yylhs.value.as < size_t > () = yystack_[1].value.as < size_t > ();}
#line 848 "bison.tab.cc"
    break;

  case 30:
#line 139 "bison.y"
                                                            {yylhs.value.as < size_t > () = compiler.add(yystack_[3].value.as < size_t > (), yystack_[1].value.as < size_t > (), scanner.lineno());}
#line 854 "bison.tab.cc"
    break;

  case 31:
#line 140 "bison.y"
                                                                    {yylhs.value.as < size_t > () = compiler.sub(yystack_[3].value.as < size_t > (), yystack_[1].value.as < size_t > (), scanner.lineno());}
#line 860 "bison.tab.cc"
    break;

  case 32:
#line 141 "bison.y"
                                        { yylhs.value.as < size_t > () = yystack_[0].value.as < size_t > ();}
#line 866 "bison.tab.cc"
    break;

  case 33:
#line 144 "bison.y"
                                {printf("Found identifier\n");yylhs.value.as < size_t > () = compiler.getVarIndex(yystack_[0].value.as < std::string > ());}
#line 872 "bison.tab.cc"
    break;

  case 34:
#line 145 "bison.y"
                                               {printf("completed const int\n");yylhs.value.as < size_t > () = compiler.getConst(yystack_[0].value.as < std::string > (), tsVarType::tsInt, scanner.lineno());}
#line 878 "bison.tab.cc"
    break;

  case 35:
#line 148 "bison.y"
                                 {compiler.enterScope();}
#line 884 "bison.tab.cc"
    break;

  case 36:
#line 149 "bison.y"
                                 {compiler.exitScope();}
#line 890 "bison.tab.cc"
    break;


#line 894 "bison.tab.cc"

            default:
              break;
            }
        }
#if YY_EXCEPTIONS
      catch (const syntax_error& yyexc)
        {
          YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
          error (yyexc);
          YYERROR;
        }
#endif // YY_EXCEPTIONS
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;
      YY_STACK_PRINT ();

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, YY_MOVE (yylhs));
    }
    goto yynewstate;


  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        error (yyla.location, yysyntax_error_ (yystack_[0].state, yyla));
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.type_get () == yyeof_)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:
    /* Pacify compilers when the user code never invokes YYERROR and
       the label yyerrorlab therefore never appears in user code.  */
    if (false)
      YYERROR;

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    goto yyerrlab1;


  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    {
      stack_symbol_type error_token;
      for (;;)
        {
          yyn = yypact_[+yystack_[0].state];
          if (!yy_pact_value_is_default_ (yyn))
            {
              yyn += yy_error_token_;
              if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yy_error_token_)
                {
                  yyn = yytable_[yyn];
                  if (0 < yyn)
                    break;
                }
            }

          // Pop the current state because it cannot handle the error token.
          if (yystack_.size () == 1)
            YYABORT;

          yyerror_range[1].location = yystack_[0].location;
          yy_destroy_ ("Error: popping", yystack_[0]);
          yypop_ ();
          YY_STACK_PRINT ();
        }

      yyerror_range[2].location = yyla.location;
      YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

      // Shift the error token.
      error_token.state = state_type (yyn);
      yypush_ ("Shifting", YY_MOVE (error_token));
    }
    goto yynewstate;


  /*-------------------------------------.
  | yyacceptlab -- YYACCEPT comes here.  |
  `-------------------------------------*/
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;


  /*-----------------------------------.
  | yyabortlab -- YYABORT comes here.  |
  `-----------------------------------*/
  yyabortlab:
    yyresult = 1;
    goto yyreturn;


  /*-----------------------------------------------------.
  | yyreturn -- parsing is finished, return the result.  |
  `-----------------------------------------------------*/
  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
#if YY_EXCEPTIONS
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack\n";
        // Do not try to display the values of the reclaimed symbols,
        // as their printers might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
#endif // YY_EXCEPTIONS
  }

  void
  tsParser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what ());
  }

  // Generate an error message.
  std::string
  tsParser::yysyntax_error_ (state_type yystate, const symbol_type& yyla) const
  {
    // Number of reported tokens (one for the "unexpected", one per
    // "expected").
    std::ptrdiff_t yycount = 0;
    // Its maximum.
    enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
    // Arguments of yyformat.
    char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];

    /* There are many possibilities here to consider:
       - If this state is a consistent state with a default action, then
         the only way this function was invoked is if the default action
         is an error action.  In that case, don't check for expected
         tokens because there are none.
       - The only way there can be no lookahead present (in yyla) is
         if this state is a consistent state with a default action.
         Thus, detecting the absence of a lookahead is sufficient to
         determine that there is no unexpected or expected token to
         report.  In that case, just report a simple "syntax error".
       - Don't assume there isn't a lookahead just because this state is
         a consistent state with a default action.  There might have
         been a previous inconsistent state, consistent state with a
         non-default action, or user semantic action that manipulated
         yyla.  (However, yyla is currently not documented for users.)
       - Of course, the expected token list depends on states to have
         correct lookahead information, and it depends on the parser not
         to perform extra reductions after fetching a lookahead from the
         scanner and before detecting a syntax error.  Thus, state merging
         (from LALR or IELR) and default reductions corrupt the expected
         token list.  However, the list is correct for canonical LR with
         one exception: it will still contain any token that will not be
         accepted due to an error action in a later state.
    */
    if (!yyla.empty ())
      {
        symbol_number_type yytoken = yyla.type_get ();
        yyarg[yycount++] = yytname_[yytoken];

        int yyn = yypact_[+yystate];
        if (!yy_pact_value_is_default_ (yyn))
          {
            /* Start YYX at -YYN if negative to avoid negative indexes in
               YYCHECK.  In other words, skip the first -YYN actions for
               this state because they are default actions.  */
            int yyxbegin = yyn < 0 ? -yyn : 0;
            // Stay within bounds of both yycheck and yytname.
            int yychecklim = yylast_ - yyn + 1;
            int yyxend = yychecklim < yyntokens_ ? yychecklim : yyntokens_;
            for (int yyx = yyxbegin; yyx < yyxend; ++yyx)
              if (yycheck_[yyx + yyn] == yyx && yyx != yy_error_token_
                  && !yy_table_value_is_error_ (yytable_[yyx + yyn]))
                {
                  if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                    {
                      yycount = 1;
                      break;
                    }
                  else
                    yyarg[yycount++] = yytname_[yyx];
                }
          }
      }

    char const* yyformat = YY_NULLPTR;
    switch (yycount)
      {
#define YYCASE_(N, S)                         \
        case N:                               \
          yyformat = S;                       \
        break
      default: // Avoid compiler warnings.
        YYCASE_ (0, YY_("syntax error"));
        YYCASE_ (1, YY_("syntax error, unexpected %s"));
        YYCASE_ (2, YY_("syntax error, unexpected %s, expecting %s"));
        YYCASE_ (3, YY_("syntax error, unexpected %s, expecting %s or %s"));
        YYCASE_ (4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
        YYCASE_ (5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
      }

    std::string yyres;
    // Argument number.
    std::ptrdiff_t yyi = 0;
    for (char const* yyp = yyformat; *yyp; ++yyp)
      if (yyp[0] == '%' && yyp[1] == 's' && yyi < yycount)
        {
          yyres += yytnamerr_ (yyarg[yyi++]);
          ++yyp;
        }
      else
        yyres += *yyp;
    return yyres;
  }


  const signed char tsParser::yypact_ninf_ = -60;

  const signed char tsParser::yytable_ninf_ = -36;

  const signed char
  tsParser::yypact_[] =
  {
      17,   -30,   -60,   -10,    -8,    -4,   -16,    24,    25,    33,
     -60,    20,    38,    51,    26,    -3,   -60,   -60,   -60,   -60,
     -60,    -2,    27,     1,    40,   -60,    -5,    35,   -60,    -5,
      -5,    -5,    -5,    46,    47,    48,    49,    50,    52,   -60,
     -60,   -60,    -5,    -5,   -60,   -60,    17,   -60,   -60,    -5,
     -60,    54,    55,    56,   -60,   -60,   -60,   -60,   -60,   -60,
     -60,   -60,    59,   -60,    17,    17,    17,   -60,   -60,   -60,
     -60,   -60,   -60,   -60
  };

  const signed char
  tsParser::yydefact_[] =
  {
      35,     0,    34,     0,     0,     0,    33,     0,     0,     0,
      16,     0,     0,     0,     0,     3,    11,    17,    18,    19,
      10,     0,     0,    32,     0,    15,     4,     6,     8,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     1,
      12,     2,     0,     0,    14,    29,    35,    33,     5,     0,
       9,     0,     0,     0,    24,    23,    25,    27,    26,    28,
      36,    36,     0,     7,    35,    35,    35,    30,    31,    36,
      20,    21,    22,    13
  };

  const signed char
  tsParser::yypgoto_[] =
  {
     -60,   -11,   -60,    -6,   -60,   -60,   -60,   -60,   -60,   -60,
      14,   -60,    53,   -59
  };

  const signed char
  tsParser::yydefgoto_[] =
  {
      -1,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    67
  };

  const signed char
  tsParser::yytable_[] =
  {
       1,     2,    68,     2,    41,    25,     3,     4,     5,    26,
      73,    27,   -35,    29,    47,    28,     6,     7,     8,     9,
       1,    42,    43,     2,   -35,   -35,     3,     4,     5,    33,
      34,    35,    10,    11,    12,    62,     6,     7,     8,     9,
      48,    30,    31,    50,    51,    52,    53,    36,    37,    38,
      32,    39,    10,    11,    12,    46,    60,    61,    70,    71,
      72,    40,    44,    63,    49,    54,    55,    56,    57,    58,
       0,    59,    64,    65,    66,    69,    45
  };

  const signed char
  tsParser::yycheck_[] =
  {
       3,     6,    61,     6,    15,    35,     9,    10,    11,    19,
      69,    19,    15,    29,    19,    19,    19,    20,    21,    22,
       3,    23,    24,     6,    23,    24,     9,    10,    11,     9,
      10,    11,    35,    36,    37,    46,    19,    20,    21,    22,
      26,    17,    17,    29,    30,    31,    32,     9,    10,    11,
      17,     0,    35,    36,    37,    15,    42,    43,    64,    65,
      66,    35,    35,    49,    29,    19,    19,    19,    19,    19,
      -1,    19,    18,    18,    18,    16,    23
  };

  const signed char
  tsParser::yystos_[] =
  {
       0,     3,     6,     9,    10,    11,    19,    20,    21,    22,
      35,    36,    37,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    35,    19,    19,    19,    29,
      17,    17,    17,     9,    10,    11,     9,    10,    11,     0,
      35,    39,    23,    24,    35,    50,    15,    19,    48,    29,
      48,    48,    48,    48,    19,    19,    19,    19,    19,    19,
      48,    48,    39,    48,    18,    18,    18,    51,    51,    16,
      41,    41,    41,    51
  };

  const signed char
  tsParser::yyr1_[] =
  {
       0,    38,    39,    39,    40,    40,    40,    40,    40,    40,
      41,    41,    41,    41,    41,    41,    41,    42,    42,    42,
      43,    44,    45,    46,    46,    46,    46,    46,    46,    47,
      48,    48,    48,    49,    49,    50,    51
  };

  const signed char
  tsParser::yyr2_[] =
  {
       0,     2,     2,     1,     2,     3,     2,     4,     2,     3,
       1,     1,     2,     5,     2,     2,     1,     1,     1,     1,
       5,     5,     5,     3,     3,     3,     3,     3,     3,     2,
       4,     4,     1,     1,     1,     0,     0
  };



  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a yyntokens_, nonterminals.
  const char*
  const tsParser::yytname_[] =
  {
  "$end", "error", "$undefined", "tstEND", "tstTRUE", "tstFALSE",
  "tstCONST_INT", "tstCONST_FLOAT", "tstCONST_STRING", "tstDEF_BOOL",
  "tstDEF_INT", "tstDEF_FLOAT", "tstDEF_STRING", "\"[\"", "\"]\"", "\"{\"",
  "\"}\"", "\"(\"", "\")\"", "tstIDENTIFIER", "tstIF", "tstWHILE",
  "tstFOR", "\"+\"", "\"-\"", "tstMUL", "tstDIV", "tstLESS", "tstMORE",
  "\"=\"", "tstLESS_EQUAL", "tstMORE_EQUAL", "tstAND", "tstOR", "tstNOT",
  "\";\"", "tstGLOBAL_REF", "tstGLOBAL_IN", "$accept", "line", "variable",
  "statement", "flow", "if", "while", "for", "preprocessor", "expStart",
  "expression", "value", "enterScope", "exitScope", YY_NULLPTR
  };

#if YYDEBUG
  const unsigned char
  tsParser::yyrline_[] =
  {
       0,    92,    92,    93,    96,    97,    98,    99,   100,   101,
     105,   106,   107,   108,   109,   110,   111,   115,   116,   117,
     120,   123,   126,   129,   130,   131,   132,   133,   134,   137,
     139,   140,   141,   144,   145,   148,   149
  };

  // Print the state stack on the debug stream.
  void
  tsParser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << int (i->state);
    *yycdebug_ << '\n';
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  tsParser::yy_reduce_print_ (int yyrule)
  {
    int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):\n";
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG

  tsParser::token_number_type
  tsParser::yytranslate_ (int t)
  {
    // YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to
    // TOKEN-NUM as returned by yylex.
    static
    const token_number_type
    translate_table[] =
    {
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37
    };
    const int user_token_number_max_ = 292;

    if (t <= 0)
      return yyeof_;
    else if (t <= user_token_number_max_)
      return translate_table[t];
    else
      return yy_undef_token_;
  }

#line 7 "bison.y"
} // ts
#line 1375 "bison.tab.cc"

#line 151 "bison.y"


void ts::tsParser::error(const location_type &l, const std::string &err_message){
	std::cout << "Error: " << err_message << " at line " << scanner.lineno() << std::endl;
}
