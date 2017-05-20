#pragma once

#include <cstddef>
#include <string>

enum {
	TK_SequenceStart = 256,
#define xx(sym,str) sym,
#include "tokens.h"
#undef xx
	TK_LastToken
};


class Scanner {
public:
    struct SavedPos
	{
		const char *SavedScriptPtr;
		int SavedScriptLine;
	};
    Scanner();
    Scanner(const Scanner &other);
    bool OpenFile(const char *filename);
    void Close();
    void SetEscape(bool esc);

    const SavedPos SavePos();
    void RestorePos(const SavedPos &pos);

    static std::string TokenName(int token, const char *string=NULL);

    bool GetString();
	void MustGetString();
	void MustGetStringName(const char *name);
	bool CheckString(const char *name);

    bool GetToken();
	void MustGetAnyToken();
	void TokenMustBe(int token);
	void MustGetToken(int token);
	bool CheckToken(int token);

    bool GetNumber();
	void MustGetNumber();
	bool CheckNumber();

	bool GetFloat();
	void MustGetFloat();
	bool CheckFloat();

	void UnGet();

    bool Compare(const char *text);
    int MatchString(const char * const *strings, size_t stride = sizeof(char*));
	int MustMatchString(const char * const *strings, size_t stride = sizeof(char*));
	int GetMessageLine();

	void UnknownTokenError();

	void ScriptError(const char *message, ...);
	//void ScriptMessage(const char *message, ...);

    char* String;
    int StringLen;
    int TokenType;
    int Number;
	double Float;
    int Line;
	bool End;
    bool Crossed;
    std::string ScriptName;
protected:
	void PrepareScript();
	void CheckOpen();
	bool ScanString(bool tokens);

    static const int MAX_STRING_SIZE = 128;

    bool ScriptOpen;
    std::string ScriptBuffer;
    char StringBuffer[MAX_STRING_SIZE];
    std::string BigStringBuffer;
    bool AlreadyGot;
    int AlreadyGotLine;
    bool LastGotToken;
    const char *LastGotPtr;
	int LastGotLine;
    const char *ScriptPtr;
	const char *ScriptEndPtr;
    bool Escape;
};
