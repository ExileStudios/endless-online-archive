#ifndef Sys_msgH
#define Sys_msgH

// A message-board post: id, author, subject and body.
struct Msg
{
    short post_id;
    String author;
    String subject;
    String body;

    Msg(int post_number, String post_author, String post_subject);
    ~Msg();
};

#endif
