#ifndef ClassvalueH
#define ClassvalueH

// One class of the class file, with the class it comes from.
struct ClassValue
{
    int class_id;
    int parent_class_id;
    String name;
    short class_record_byte_at_3;
    short class_record_word_at_4;
    short class_record_word_at_6;
    short class_record_word_at_8;
    short class_record_word_at_10;
    short class_record_word_at_12;
    short class_record_word_at_14;

    ClassValue(int record_id);
    ~ClassValue();
};

#endif
