#ifndef ClassvalueH
#define ClassvalueH

// One class of the class file, with the class it comes from.
struct ClassValue
{
    int class_id;
    String name;
    char stat_group;
    unsigned char parent_class_id;
    short strength;
    short intelligence;
    short wisdom;
    short agility;
    short constitution;
    short charisma;

    ClassValue(int record_id);
    ~ClassValue();
};

#endif
