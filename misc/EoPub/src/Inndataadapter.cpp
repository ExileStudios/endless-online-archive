#include <vcl.h>
#pragma hdrstop

#include "Inndataadapter.h"
#include "UnitMain.h"
#include "Innvalues.h"
#include <vector.h>

#pragma package(smart_init)

InnDataAdapter::InnDataAdapter()
{
}

InnDataAdapter::~InnDataAdapter()
{
}

// Deletes the inn at the index and moves the IDs of the inns after it down by one.
void InnDataAdapter::DeleteAt(int index)
{
    if (index < 0 || (unsigned int)FormMain->inns_tab->inn_values->Count() <= index)
        return;
    FormMain->inns_tab->inn_values->DeleteInn(index);
    for (int inn_index = index; inn_index < (unsigned int)FormMain->inns_tab->inn_values->Count(); inn_index++)
        FormMain->inns_tab->inn_values->GetInn(inn_index)->id--;
}

// Inserts an inn at the index and moves the IDs of the inns after it up by one.
void InnDataAdapter::InsertAt(int index)
{
    if (index < 0 || (unsigned int)FormMain->inns_tab->inn_values->Count() <= index)
        return;
    FormMain->inns_tab->inn_values->InsertInn(index);
    for (int inn_index = index; inn_index < (unsigned int)FormMain->inns_tab->inn_values->Count(); inn_index++)
        FormMain->inns_tab->inn_values->GetInn(inn_index)->id++;
}

// Copies the questions of the inn with the first ID onto the inn with the second.
void InnDataAdapter::CopyLists(String source_id, String target_id)
{
    if (source_id.Length() < 1)
        return;
    if (target_id.Length() < 1)
        return;
    InnValue *source = FormMain->inns_tab->inn_values->GetInn(StrToInt(source_id) - 1);
    InnValue *target = FormMain->inns_tab->inn_values->GetInn(StrToInt(target_id) - 1);
    target->questions.clear();
    for (int i = 0; i < source->questions.size(); i++)
    {
        InnQuizItem question = source->questions[i];
        InnQuizItem question_copy;
        question_copy.question = question.question;
        question_copy.answer = question.answer;
        target->questions.insert(target->questions.end(), question_copy);
    }
}
