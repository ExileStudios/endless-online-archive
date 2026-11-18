#ifndef PubfileheaderH
#define PubfileheaderH

struct PubfileHeader
{
    String file_name;
    short file_length;
    short checksum_high;
    short checksum_low;
    char version;
    unsigned short record_count;
    short records_read;
    unsigned char load_state;

    PubfileHeader(String name);
    ~PubfileHeader();
};


// Shared body: a class defines its GetMatchScore with DEFINE_GET_MATCH_SCORE(TheClass).
#define DEFINE_GET_MATCH_SCORE(Class) \
int Class::GetMatchScore(String name, String text) \
{ \
    int score; \
    if (name.Length() < 4) \
        return 0; \
    String text_left = text; \
    name = LowerCase(name); \
    String name_left = name; \
    int matched = 0; \
    for (int pos = 1; pos <= name_left.Length(); pos++) \
    { \
        for (int j = 1; j <= text_left.Length(); j++) \
        { \
            if (name_left[pos] == text_left[j]) \
            { \
                name_left[pos] = -54; \
                text_left[j] = -55; \
                matched++; \
                break; \
            } \
        } \
    } \
    int letter_score = (double)matched / text_left.Length() * 100.0; \
    if (letter_score < 50) \
        return letter_score; \
    int next = 1; \
    int run = 0; \
    int longest_run = 0; \
    for (int k = 1; k <= name.Length(); k++) \
    { \
        if (text.Length() < next) \
            break; \
        if (text[next] == name[k]) \
        { \
            next++; \
            run++; \
            if (run > longest_run) \
                longest_run = run; \
        } \
        else \
            run = 0; \
    } \
    int order_score = (double)longest_run / name.Length() * 100.0; \
    score = (letter_score + order_score) / 2; \
    if (abs(text.Length() - name.Length()) > 2) \
        score -= 5; \
    if (score < 0) \
        score = 0; \
    return score; \
}

// Shared body: a class defines its StripSpaces with DEFINE_STRIP_SPACES(TheClass).
#define DEFINE_STRIP_SPACES(Class) \
String Class::StripSpaces(String name) \
{ \
    String result = ""; \
    for (int i = 1; i <= name.Length(); i++) \
    { \
        if (name[i] != ' ') \
            result += name[i]; \
    } \
    return LowerCase(result); \
}

// Shared body: a class defines its ParseNumber with DEFINE_PARSE_NUMBER(TheClass).
#define DEFINE_PARSE_NUMBER(Class) \
int Class::ParseNumber(String text) \
{ \
    if (text.Length() > 5) \
        text = text.SubString(1, 5); \
    String digits = ""; \
    for (int i = 1; i <= text.Length(); i++) \
    { \
        if (text[i] < '0') \
            return -1; \
        if (text[i] > '9') \
            return -1; \
        digits += text[i]; \
    } \
    if (digits.Length() < 1) \
        return -1; \
    return StrToInt(digits) - 1; \
}

// Deletes every item record.
// Shared body: a class defines its ClearHeaders with DEFINE_CLEAR_HEADERS(TheClass); each class has its own headers.
#define DEFINE_CLEAR_HEADERS(Class) \
void Class::ClearHeaders() \
{ \
    for (int i = 0; i < headers.size(); i++) \
        delete headers[i]; \
    headers.clear(); \
}

// Reads one byte at the read position, stored plus one; a value past 252 reads as 0.
// Shared body: a class defines its ReadByte with DEFINE_READ_BYTE(TheClass); each class has its own data,
// read_position.
#define DEFINE_READ_BYTE(Class) \
char Class::ReadByte() \
{ \
    unsigned char value; \
    try \
    { \
        value = data[read_position]; \
        value--; \
    } \
    catch (...) \
    { \
        value = 0; \
    } \
    read_position++; \
    if (value > 252) \
        value = 0; \
    return value; \
}

#endif
