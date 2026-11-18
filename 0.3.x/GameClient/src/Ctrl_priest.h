#ifndef Ctrl_priestH
#define Ctrl_priestH

// The priest's state: the request id from the last priest reply, sent back with the wedding answers.
struct PriestController
{
    int request_id;

    PriestController();
    ~PriestController();
    void SetRequestId(int id);
    int GetRequestId();
};

#endif
