#include "scanner.h"

#include "util.h"

#include <string>
#include <sstream>
#include <cstring>
#include <cstdarg>
#include <cstdio>
#include <cassert>
#include <iostream>

static int strbin (char *str)
{
	char *start = str;
	char *p = str, c;
	int i;

	while ( (c = *p++) ) {
		if (c != '\\') {
			*str++ = c;
		} else {
			switch (*p) {
				case 'a':
					*str++ = '\a';
					break;
				case 'b':
					*str++ = '\b';
					break;
				case 'c':
					*str++ = '\034';	// TEXTCOLOR_ESCAPE
					break;
				case 'f':
					*str++ = '\f';
					break;
				case 'n':
					*str++ = '\n';
					break;
				case 't':
					*str++ = '\t';
					break;
				case 'r':
					*str++ = '\r';
					break;
				case 'v':
					*str++ = '\v';
					break;
				case '?':
					*str++ = '\?';
					break;
				case '\n':
					break;
				case 'x':
				case 'X':
					c = 0;
					for (i = 0; i < 2; i++)
					{
						p++;
						if (*p >= '0' && *p <= '9')
							c = (c << 4) + *p-'0';
						else if (*p >= 'a' && *p <= 'f')
							c = (c << 4) + 10 + *p-'a';
						else if (*p >= 'A' && *p <= 'F')
							c = (c << 4) + 10 + *p-'A';
						else
						{
							p--;
							break;
						}
					}
					*str++ = c;
					break;
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
					c = *p - '0';
					for (i = 0; i < 2; i++)
					{
						p++;
						if (*p >= '0' && *p <= '7')
							c = (c << 3) + *p - '0';
						else
						{
							p--;
							break;
						}
					}
					*str++ = c;
					break;
				default:
					*str++ = *p;
					break;
			}
			p++;
		}
	}
	*str = 0;
	return int(str - start);
}

Scanner::Scanner() {
    ScriptOpen = false;
}

Scanner::Scanner(const Scanner &other) {
	ScriptOpen = false;
	*this = other;
}

bool Scanner::OpenFile(const char *name) {
	uint8_t *filebuf;
	int filesize;

	Close ();
	filesize = Util::ReadFile (name, &filebuf);
	if(filesize < 0) {
		return false;
	}
	ScriptBuffer = std::string((const char *)filebuf, filesize);
	delete[] filebuf;
	ScriptName = name;	// This is used for error messages so the full file name is preferable
	PrepareScript ();
	return true;
}

void Scanner::Close() {
	ScriptOpen = false;
	ScriptBuffer = "";
	BigStringBuffer = "";
	StringBuffer[0] = '\0';
	String = StringBuffer;
}

void Scanner::SetEscape(bool esc) {
	Escape = esc;
}

const Scanner::SavedPos Scanner::SavePos() {
	SavedPos pos;

	CheckOpen();
	if(End) {
		pos.SavedScriptPtr = NULL;
	} else {
		pos.SavedScriptPtr = ScriptPtr;
	}
	pos.SavedScriptLine = Line;
	return pos;
}

void Scanner::RestorePos(const Scanner::SavedPos &pos) {
	if (pos.SavedScriptPtr) {
		ScriptPtr = pos.SavedScriptPtr;
		Line = pos.SavedScriptLine;
		End = false;
	} else {
		End = true;
	}
	AlreadyGot = false;
	LastGotToken = false;
	Crossed = false;
}

std::string Scanner::TokenName (int token, const char *string)
{
	static const char *const names[] =
	{
#define xx(sym,str)		str,
#include "tokens.h"
#undef xx
	};

	std::string work;

	if (token > ' ' && token < 256)
	{
		work = '\'';
		work += token;
		work += '\'';
	}
	else if (token >= TK_Identifier && token < TK_LastToken)
	{
		work = names[token - TK_Identifier];
		if (string != NULL && token >= TK_Identifier && token <= TK_FloatConst)
		{
			work += ' ';
			char quote = (token == TK_StringConst) ? '"' : '\'';
			work += quote;
			work += string;
			work += quote;
		}
	}
	else
	{
        std::ostringstream ss;
        ss << "Unknown(" << token << ")";
		std::string work = ss.str();
		return work;
	}
	return work;
}

bool Scanner::GetString() {
	return ScanString(false);
}

void Scanner::MustGetString() {
	if (GetString() == false) {
		ScriptError("Missing string (unexpected end of file).");
	}
}

void Scanner::MustGetStringName(const char *name) {
	MustGetString();
	if(Compare(name) == false) {
		ScriptError ("Expected '%s', got '%s' at line %d.", name, String, GetMessageLine());
	}
}

bool Scanner::CheckString(const char *name) {
	if(GetString()) {
		if(Compare(name)) {
			return true;
		}
		UnGet();
	}
	return false;
}

bool Scanner::GetToken ()
{
	if (ScanString (true))
	{
		if (TokenType == TK_FloatConst)
		{
			char *stopper;
			Float = strtod(String, &stopper);
		}
		else if (TokenType == TK_StringConst)
		{
			StringLen = strbin(String);
		}
		return true;
	}
	return false;
}

void Scanner::MustGetAnyToken ()
{
	if (GetToken () == false)
	{
		ScriptError ("Missing token (unexpected end of file).");
	}
}

void Scanner::TokenMustBe (int token)
{
	if (TokenType != token)
	{
		std::string tok1 = TokenName(token);
		std::string tok2 = TokenName(TokenType, String);
		ScriptError ("Expected %s but got %s instead at line %d.", tok1.c_str(), tok2.c_str(), GetMessageLine());
	}
}

void Scanner::MustGetToken (int token)
{
	MustGetAnyToken ();
	TokenMustBe(token);
}

bool Scanner::CheckToken (int token)
{
	if (GetToken ())
	{
		if (TokenType == token)
		{
			return true;
		}
		UnGet ();
	}
	return false;
}

bool Scanner::GetFloat ()
{
	char *stopper;

	CheckOpen ();
	if (GetString())
	{
		Float = strtod (String, &stopper);
		if (*stopper != 0)
		{
			ScriptError ("SC_GetFloat: Bad numeric constant \"%s\".", String);
		}
		Number = (int)Float;
		return true;
	}
	else
	{
		return false;
	}
}

void Scanner::MustGetFloat ()
{
	if (GetFloat() == false)
	{
		ScriptError ("Missing floating-point number (unexpected end of file).");
	}
}

bool Scanner::CheckFloat ()
{
	char *stopper;

	if (GetString())
	{
		if (String[0] == 0)
		{
			UnGet();
			return false;
		}

		Float = strtod (String, &stopper);
		if (*stopper != 0)
		{
			UnGet();
			return false;
		}
		return true;
	}
	else
	{
		return false;
	}
}

void Scanner::UnGet ()
{
	AlreadyGot = true;
	AlreadyGotLine = LastGotLine;	// in case of an error we want the line of the last token.
}

bool Scanner::Compare (const char *text) {
	return (strcasecmp (text, String) == 0);
}

int Scanner::MatchString (const char * const *strings, size_t stride)
{
	int i;

	assert(stride % sizeof(const char*) == 0);

	stride /= sizeof(const char*);

	for (i = 0; *strings != NULL; i++)
	{
		if (Compare (*strings))
		{
			return i;
		}
		strings += stride;
	}
	return -1;
}

int Scanner::MustMatchString (const char * const *strings, size_t stride)
{
	int i;

	i = MatchString (strings, stride);
	if (i == -1)
	{
		ScriptError ("Unknown keyword '%s'", String);
	}
	return i;
}

int Scanner::GetMessageLine()
{
	return AlreadyGot? AlreadyGotLine : Line;
}

void Scanner::UnknownTokenError() {
	ScriptError("Unknown token %s at line %d", TokenName(TokenType, String).c_str(), GetMessageLine());
}

void Scanner::ScriptError(const char *message, ...) {
    va_list vl;
    va_start(vl, message);
    vfprintf(stderr, message, vl);
    va_end(vl);
    printf("\n");
	exit(2);
}

void Scanner::PrepareScript() {
	// The scanner requires the file to end with a '\n', so add one if
	// it doesn't already.
	if(ScriptBuffer.length() == 0 || ScriptBuffer[ScriptBuffer.length() - 1] != '\n') {
		// If the last character in the buffer is a null character, change
		// it to a newline. Otherwise, append a newline to the end.
		if(ScriptBuffer.length() > 0 && ScriptBuffer[ScriptBuffer.length() - 1] == '\0') {
			ScriptBuffer[ScriptBuffer.length() - 1] = '\n';
		} else {
			ScriptBuffer += '\n';
		}
	}

	ScriptPtr = &ScriptBuffer[0];
	ScriptEndPtr = &ScriptBuffer[ScriptBuffer.length()];
	Line = 1;
	End = false;
	ScriptOpen = true;
	String = StringBuffer;
	AlreadyGot = false;
	LastGotToken = false;
	LastGotPtr = NULL;
	LastGotLine = 1;
	Escape = true;
	StringBuffer[0] = '\0';
	BigStringBuffer = "";
}

void Scanner::CheckOpen() {
	if(ScriptOpen == false) {
		std::cerr << "SC_ call before SC_Open()." << std::endl;
	}
}

bool Scanner::ScanString (bool tokens)
{
	const char *marker, *tok;
	bool return_val;

	CheckOpen();
	if (AlreadyGot)
	{
		AlreadyGot = false;
		if (!tokens || LastGotToken)
		{
			return true;
		}
		ScriptPtr = LastGotPtr;
		Line = LastGotLine;
	}

	Crossed = false;
	if (ScriptPtr >= ScriptEndPtr)
	{
		End = true;
		return false;
	}

	LastGotPtr = ScriptPtr;
	LastGotLine = Line;

	// In case the generated scanner does not use marker, avoid compiler warnings.
	marker;
#include "lexer.h"
	LastGotToken = tokens;
	return return_val;
}
