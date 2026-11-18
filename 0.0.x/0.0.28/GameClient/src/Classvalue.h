#ifndef ClassvalueH
#define ClassvalueH

// One class of the class file, with the class it comes from.
struct ClassValue
{
    int class_id;
    int parent_class_id;
    String name;
    short stat_group;
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
