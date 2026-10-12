#ifndef ConstantsH
#define ConstantsH

#define ELEMENT_COUNT 7
#define EO_BREAK_BYTE 255
#define EO_NUMBER_BASE 253
#define EO_NUMBER_BASE_CUBED 16194277
#define EO_NUMBER_BASE_INVERSE 0.00395256916996047430813L
#define EO_NUMBER_BASE_SQUARED 64009
#define EO_NUMBER_EMPTY 0xFE
#define EO_NUMBER_EMPTY_CHAR (-2)
#define MS_PER_MINUTE 60000
#define MS_PER_SECOND 1000
#define SECONDS_PER_DAY 86400

// Shared body: a class defines its EncodeNumber with DEFINE_ENCODE_NUMBER(TheClass); each class has its own
// encode_scratch.
#define DEFINE_ENCODE_NUMBER(Class) \
String Class::EncodeNumber(unsigned int value, signed int width) \
{ \
    int rem; \
    char digit; \
    try \
    { \
        unsigned int quotient = 1; \
        char flag = 1; \
        for (int i = 0; i < width; i++) \
        { \
            if (flag) \
            { \
                double scaled = value * EO_NUMBER_BASE_INVERSE; \
                quotient = scaled; \
                rem = value % EO_NUMBER_BASE; \
                digit = rem + 1; \
                encode_scratch[i] = digit; \
                value = quotient; \
                if (quotient < 1) \
                    flag = 0; \
                else \
                { \
                    /* the number needs another byte */ \
                    if (i + 1 == width) \
                        width++; \
                } \
            } \
            else \
            { \
                char pad = EO_NUMBER_EMPTY_CHAR; \
                encode_scratch[i] = pad; \
            } \
        } \
    } \
    catch (...) \
    { \
        width = 0; \
    } \
    String encoded_str(encode_scratch, width); \
    return encoded_str; \
}

// Shared body: a class defines its DecodeNumber with DEFINE_DECODE_NUMBER(TheClass).
#define DEFINE_DECODE_NUMBER(Class) \
int Class::DecodeNumber(String value) \
{ \
    int result; \
    String value_copy = value; \
    result = 0; \
    try \
    { \
        for (int byte_index = 1; byte_index <= value_copy.Length(); byte_index++) \
        { \
            char raw_char = value_copy[byte_index]; \
            unsigned char ch = raw_char; \
            if (ch == EO_NUMBER_EMPTY || !ch) \
                break; \
            int digit = ch; \
            digit--; \
            if (byte_index == 1) \
                result += digit; \
            if (byte_index == 2) \
                result += EO_NUMBER_BASE * digit; \
            if (byte_index == 3) \
                result += EO_NUMBER_BASE_SQUARED * digit; \
            if (byte_index == 4) \
                result += EO_NUMBER_BASE_CUBED * digit; \
        } \
    } \
    catch (...) \
    { \
        result = 0; \
    } \
    return result; \
}

// Writes a number the protocol's way: base 253, lowest digit first, each digit plus one, padded with 254.
// Shared body: a class defines its EncodeNumber with DEFINE_LEARN_ENCODE_NUMBER(TheClass).
#define DEFINE_LEARN_ENCODE_NUMBER(Class) \
String Class::EncodeNumber(unsigned int value, int width) \
{ \
    unsigned int remaining = value; \
    String result = ""; \
    try \
    { \
        unsigned int quotient; \
        do \
        { \
            int rem; \
            char digit; \
            double scaled = remaining * EO_NUMBER_BASE_INVERSE; \
            quotient = scaled; \
            rem = remaining % EO_NUMBER_BASE; \
            digit = rem + 1; \
            result.Insert(digit, result.Length() + 1); \
            if (quotient >= 1) \
                remaining = quotient; \
        } while (quotient >= 1); \
    } \
    catch (...) \
    { \
        result = ""; \
    } \
    if (result.Length() < width) \
    { \
        char pad = EO_NUMBER_EMPTY_CHAR; \
        int count = width - result.Length(); \
        for (int i = 0; i < count; i++) \
            result.Insert(pad, result.Length() + 1); \
    } \
    return result; \
}

// Reads a number of up to four bytes; a 254 ends it.
// Shared body: a class defines its DecodeNumber with DEFINE_INN_DECODE_NUMBER(TheClass).
#define DEFINE_INN_DECODE_NUMBER(Class) \
int Class::DecodeNumber(String value) \
{ \
    int result; \
    String value_copy = value; \
    result = 0; \
    try \
    { \
        for (int byte_index = 1; byte_index <= value_copy.Length(); byte_index++) \
        { \
            char raw_char = value_copy[byte_index]; \
            unsigned char ch = raw_char; \
            if (ch == EO_NUMBER_EMPTY) \
                break; \
            int digit = ch; \
            digit--; \
            if (byte_index == 1) \
                result += digit; \
            if (byte_index == 2) \
                result += EO_NUMBER_BASE * digit; \
            if (byte_index == 3) \
                result += EO_NUMBER_BASE_SQUARED * digit; \
            if (byte_index == 4) \
                result += EO_NUMBER_BASE_CUBED * digit; \
        } \
    } \
    catch (...) \
    { \
        result = 0; \
    } \
    return result; \
}

// Reads a number of up to four bytes; a 254 ends it.
// Shared body: a class defines its DecodeNumber with DEFINE_MAP_DECODE_NUMBER(TheClass).
#define DEFINE_MAP_DECODE_NUMBER(Class) \
int Class::DecodeNumber(String value) \
{ \
    int result = 0; \
    try \
    { \
        for (int digit_index = 1; digit_index <= value.Length(); digit_index++) \
        { \
            char raw_char = value[digit_index]; \
            unsigned char ch = raw_char; \
            if (ch == EO_NUMBER_EMPTY || !ch) \
                break; \
            int digit = ch; \
            digit--; \
            if (digit_index == 1) \
                result += digit; \
            if (digit_index == 2) \
                result += EO_NUMBER_BASE * digit; \
            if (digit_index == 3) \
                result += EO_NUMBER_BASE_SQUARED * digit; \
            if (digit_index == 4) \
                result += EO_NUMBER_BASE_CUBED * digit; \
        } \
    } \
    catch (...) \
    { \
        result = 0; \
    } \
    return result; \
}

// The next `;` separated field of the line being read.
// Shared body: a class defines its Extract with DEFINE_EXTRACT(TheClass); each class has its own current_line.
#define DEFINE_EXTRACT(Class) \
String Class::Extract() \
{ \
    int pos = current_line.Pos(";"); \
    if (pos < 1) \
        return current_line; \
    String result = current_line.SubString(1, pos - 1); \
    current_line.Delete(1, pos); \
    return result; \
}

#endif
