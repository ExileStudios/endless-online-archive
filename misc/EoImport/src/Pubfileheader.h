#ifndef PubfileheaderH
#define PubfileheaderH

struct PubfileHeader
{
    String file_name;
    char unused_bytes[16];

    ~PubfileHeader();
};

#endif
