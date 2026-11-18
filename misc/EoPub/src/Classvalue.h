#ifndef ClassvalueH
#define ClassvalueH

// One class of the class file: its name, parent, stat group and base stats.
struct ClassValue
{
    int class_id;
    String name;
    unsigned char parent_class_id;
    unsigned char stat_group;
    unsigned short strength;
    unsigned short intelligence;
    unsigned short wisdom;
    unsigned short agility;
    unsigned short constitution;
    unsigned short charisma;

    ClassValue();
    ClassValue(int record_id);
    ~ClassValue();
};

#endif
