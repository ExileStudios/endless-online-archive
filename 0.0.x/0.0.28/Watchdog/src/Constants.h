#ifndef ConstantsH
#define ConstantsH

#define VOLUME_INFO_BUFFER_SIZE 1000

// Loads an ini file, its path taken from the folder the exe is in.
// Shared body: a class defines its SetIniPath with DEFINE_SET_INI_PATH(TheClass); each class has its own ini_lines.
#define DEFINE_SET_INI_PATH(Class) \
void Class::SetIniPath(String path) \
{ \
    ini_lines->Clear(); \
    try \
    { \
        int last_slash_pos; \
        String exe = Application->ExeName; \
        last_slash_pos = 0; \
        if (exe.Length() >= 1) \
        { \
            int pos = exe.Length(); \
            while (pos >= 1) \
            { \
                if (exe[pos] == '\\') \
                { \
                    last_slash_pos = pos; \
                    break; \
                } \
                pos--; \
            } \
        } \
        String dir = ""; \
        if (exe.Length() >= 1 && last_slash_pos >= 1) \
        { \
            for (int i = 1; i < last_slash_pos; i++) \
                dir = dir + exe[i]; \
        } \
        exe = dir; \
        String tail; \
        if (exe[exe.Length()] != '\\') \
            tail = "\\" + path; \
        else \
            tail = path; \
        ini_lines->LoadFromFile(exe + tail); \
    } \
    catch (...) \
    { \
    } \
}

// A switch from the loaded ini: "on" or "1" is true, any other value false.
// Shared body: a class defines its ReadIniBool with DEFINE_READ_INI_BOOL(TheClass); each class has its own ini_lines.
#define DEFINE_READ_INI_BOOL(Class) \
bool Class::ReadIniBool(String key, bool default_value) \
{ \
    bool result = default_value; \
    if (ini_lines->Count >= 1) \
    { \
        try \
        { \
            for (int i = 0; i < ini_lines->Count; i++) \
            { \
                char before_eq; \
                String line = ini_lines->Strings[i]; \
                before_eq = 1; \
                char skip_leading = 1; \
                String ini_name = ""; \
                String value = ""; \
                if (line.Length() >= 1) \
                { \
                    for (int j = 1; j <= line.Length(); j++) \
                    { \
                        if (line[j] == '=') \
                            before_eq = 0; \
                        if (before_eq) \
                        { \
                            if (line[j] != ' ' && line[j] != '=') \
                                ini_name = ini_name + line[j]; \
                        } \
                        else if (skip_leading) \
                        { \
                            if (line[j] != ' ' && line[j] != '=') \
                            { \
                                skip_leading = 0; \
                                value = value + line[j]; \
                            } \
                        } \
                        else \
                            value = value + line[j]; \
                    } \
                    if (LowerCase(ini_name) == LowerCase(key)) \
                    { \
                        if (value.Length() >= 1) \
                        { \
                            if (LowerCase(value) == "on" || LowerCase(value) == "1") \
                                result = 1; \
                            else \
                                result = 0; \
                        } \
                        break; \
                    } \
                } \
            } \
        } \
        catch (...) \
        { \
        } \
    } \
    return result; \
}

// A text from the loaded ini ( the name in any case ), or the default; the spaces before the value are skipped.
// Shared body: a class defines its ReadIniString with DEFINE_READ_INI_STRING(TheClass); each class has its own
// ini_lines.
#define DEFINE_READ_INI_STRING(Class) \
String Class::ReadIniString(String key, String default_value) \
{ \
    String result = default_value; \
    if (ini_lines->Count >= 1) \
    { \
        try \
        { \
            for (int i = 0; i < ini_lines->Count; i++) \
            { \
                char before_eq; \
                String line = ini_lines->Strings[i]; \
                before_eq = 1; \
                char skip_leading = 1; \
                String ini_name = ""; \
                String value = ""; \
                if (line.Length() >= 1) \
                { \
                    for (int j = 1; j <= line.Length(); j++) \
                    { \
                        if (line[j] == '=') \
                            before_eq = 0; \
                        if (before_eq) \
                        { \
                            if (line[j] != ' ' && line[j] != '=') \
                                ini_name = ini_name + line[j]; \
                        } \
                        else if (skip_leading) \
                        { \
                            if (line[j] != ' ' && line[j] != '=') \
                            { \
                                skip_leading = 0; \
                                value = value + line[j]; \
                            } \
                        } \
                        else \
                            value = value + line[j]; \
                    } \
                    if (LowerCase(ini_name) == LowerCase(key)) \
                    { \
                        if (value.Length() >= 1) \
                            result = value; \
                        break; \
                    } \
                } \
            } \
        } \
        catch (...) \
        { \
        } \
    } \
    return result; \
}

// A number from the loaded ini ( the name in any case ), or the default; spaces anywhere in the line are skipped.
// Shared body: a class defines its ReadIniInt with DEFINE_READ_INI_INT(TheClass); each class has its own ini_lines.
#define DEFINE_READ_INI_INT(Class) \
int Class::ReadIniInt(String key, int default_value) \
{ \
    int result = default_value; \
    if (ini_lines->Count >= 1) \
    { \
        try \
        { \
            for (int i = 0; i < ini_lines->Count; i++) \
            { \
                String line = ini_lines->Strings[i]; \
                char before_eq = 1; \
                String ini_name = ""; \
                String value = ""; \
                if (line.Length() >= 1) \
                { \
                    for (int j = 1; j <= line.Length(); j++) \
                    { \
                        if (line[j] == '=') \
                            before_eq = 0; \
                        if (line[j] != ' ' && line[j] != '=') \
                        { \
                            if (before_eq) \
                                ini_name = ini_name + line[j]; \
                            else \
                                value = value + line[j]; \
                        } \
                    } \
                    if (LowerCase(ini_name) == LowerCase(key)) \
                    { \
                        if (value.Length() >= 1) \
                            result = StrToInt(value); \
                        break; \
                    } \
                } \
            } \
        } \
        catch (...) \
        { \
        } \
    } \
    return result; \
}

#endif
