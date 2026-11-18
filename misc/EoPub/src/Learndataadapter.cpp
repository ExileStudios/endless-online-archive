#include <vcl.h>
#pragma hdrstop

#include "Learndataadapter.h"
#include "UnitMain.h"
#include "Learnvalue.h"
#include "Learnvalues.h"
#include <vector.h>

#pragma package(smart_init)

LearnDataAdapter::LearnDataAdapter()
{
}

LearnDataAdapter::~LearnDataAdapter()
{
}

// How many of the skills all the trainers teach are the skill.
int LearnDataAdapter::SkillLearnCount(int skill_id)
{
    LearnValue *trainer;
    int count = 0;
    for (int i = 0; i < (unsigned int)FormMain->trainers_tab->trainer_values->Count(); i++)
    {
        trainer = FormMain->trainers_tab->trainer_values->Insert(i);
        for (int j = 0; j < trainer->skills.size(); j++)
        {
            if (trainer->skills[j].id == skill_id)
                count++;
        }
    }
    return count;
}

// Deletes the trainer at the index and moves the IDs of the trainers after it down by one.
void LearnDataAdapter::DeleteAt(int index)
{
    if (index < 0 || (unsigned int)FormMain->trainers_tab->trainer_values->Count() <= index)
        return;
    FormMain->trainers_tab->trainer_values->DeleteTrainer(index);
    while ((unsigned int)FormMain->trainers_tab->trainer_values->Count() > index)
    {
        FormMain->trainers_tab->trainer_values->Insert(index)->id--;
        index++;
    }
}

// Inserts a skill master at the index and moves the IDs of the masters after it up by one.
void LearnDataAdapter::InsertAt(int index)
{
    if (index < 0 || (unsigned int)FormMain->trainers_tab->trainer_values->Count() <= index)
        return;
    FormMain->trainers_tab->trainer_values->InsertTrainer(index);
    while ((unsigned int)FormMain->trainers_tab->trainer_values->Count() > index)
    {
        FormMain->trainers_tab->trainer_values->Insert(index)->id++;
        index++;
    }
}

// Copies the skills of the trainer with the first ID onto the trainer with the second.
void LearnDataAdapter::CopyLists(String source_id, String target_id)
{
    if (source_id.Length() < 1)
        return;
    if (target_id.Length() < 1)
        return;
    LearnValue *source = FormMain->trainers_tab->trainer_values->Insert(StrToInt(source_id) - 1);
    LearnValue *target = FormMain->trainers_tab->trainer_values->Insert(StrToInt(target_id) - 1);
    target->skills.clear();
    for (int i = 0; i < source->skills.size(); i++)
    {
        LearnItemVal skill = source->skills[i];
        LearnItemVal skill_copy(i);
        skill_copy = skill;
        target->skills.insert(target->skills.end(), skill_copy);
    }
}
