# EoImport: quirks

These are behaviours of the original EoImport as it shipped. The source keeps every one of them byte for byte,
because changing any of them would change the exe. They are collected here, each with what the line was written to
do and what it does instead, so the comments in `src/` can stay about what the code is for.

## The chats pub file is written with the trainers' file id

`UnitMain.cpp`, `TMainForm::ExportChats`

```cpp
String TMainForm::ExportChats()
{
    String created = "";
    String header = "";
    header = header.Insert("ETF", header.Length() + 1);
```

- **What the function is for:** `ExportChats` writes the chat timings and lines of each NPC that talks to
  `serv_chats.epf`, behind a header of a three-letter file id, a random number, the record count and the pub version.
- **What the line was written to do:** Start the chats file with its own id. Each of the other four writers starts
  its file with an id of its own: `ExportDrops` with "EDF", `ExportShops` with "ESF", `ExportInns` with "EID", and
  `ExportTrainers` with "ETF".
- **What it does:** `ExportChats` writes "ETF" too, the id `ExportTrainers` writes, so `serv_chats.epf` and
  `serv_trainers.epf` start alike.
- **When it happens:** Each export with the chats row ticked and `npcs.ini` found: `TMainForm::buttonOkClick` calls
  `ExportChats` (`created_files += ExportChats();`).
- **What you see:** A program that tells the pub files apart by their id reads `serv_chats.epf` as a trainers file.

## The two pages update only while the window's title has "EO" at its second and third letters

`UnitMain.cpp`, `TMainForm::UpdateReadPage` and `TMainForm::UpdateWritePath`

```cpp
void TMainForm::UpdateReadPage()
{
    labelPath->Caption = read_path_caption;
    if (MainForm->Caption[2] != 'E')
        return;
    if (MainForm->Caption[3] != 'O')
        return;
```

- **What the functions are for:** `UpdateReadPage` shows on the first page whether import files were found, with the
  button, the tabs and the cursor to match; `UpdateWritePath` shows the output folder on the second page.
- **What the lines were written to do:** Nothing the pages need: both functions stop when the main window's title does
  not have an "E" at its second character and an "O" at its third.
- **What they do:** The form's title is " EO Pub Import 0.3", with a leading space, so the test passes and the pages
  update. With any other title, both functions return after the path label: the tabs, the OK button, the hints and the
  output folder label keep what they showed before.
- **When it happens:** Each scan of an import folder (`TMainForm::ScanImportFolder` calls `UpdateReadPage();`), and
  each pick of an output folder (`UpdateWritePath();` from the folder search and the Browse button).
- **What you see:** Nothing with the shipped title.

## A skill master's learn line reads its id from the position of ".name", which that line does not have

`Learnvalues.cpp`, `LearnValues::ImportIni`

```cpp
        int name_pos = key.Pos(".name");
    ...
        int learn_pos = key.Pos(".learn");
        if (learn_pos > 0)
        {
            String learn_id_part = key.SubString(1, name_pos - 1);
            int learn_id[1];
            learn_id[0] = ParseNumber(learn_id_part);
```

- **What the function is for:** `ImportIni` reads `skills.ini`: each `.name` key starts a skill master, and each
  `.learn` line adds the skills it teaches, fourteen numbers each.
- **What the line was written to do:** Take the id in front of ".learn" in the key, as the `.name` branch takes the id
  in front of ".name".
- **What it does:** A `.learn` key has no ".name", so `name_pos` is 0 and `key.SubString(1, -1)` is empty;
  `ParseNumber` of it is 0. The id is never read afterwards: each skill goes to the last master the file named.
- **When it happens:** Every `.learn` line.
- **What you see:** Nothing: a learn line written under another master's name still adds to the master above it.

## An inn's behaviour id from its name key is overwritten with its place in the list

`Innvalues.cpp`, `InnValues::ImportIni`

```cpp
            int inn_id = Count() + 1;
            InnValue inn(inn_id);
            inn.behavior_id = behavior_id;
            inn.name = value_text;
            inn.behavior_id = inn_id;
```

- **What the function is for:** `ImportIni` reads `inns.ini`: each `.name` key starts an inn, whose id the key begins
  with, and the keys after it set its costs, its places and its questions.
- **What the line was written to do:** Keep the id the `.name` key gives the inn as its behaviour id.
- **What it does:** The next line but one sets `behavior_id` again, to `inn_id`, the inn's place in the list counted
  from 1. Only an `.innkeeper` key after it sets the id the file gives.
- **When it happens:** Every `.name` key of `inns.ini`.
- **What you see:** An inn with no `.innkeeper` line is exported with its place in the file as its behaviour id, not
  the id its name key carries.

## ".inncost" and ".innsleepcost" set the same value

`Innvalues.cpp`, `InnValues::ImportIni`

```cpp
        key_pos = key.Pos(".innsleepcost");
        if (key_pos > 0)
        {
            records[inn_count - 1].sleep_cost = ReadNextNumber();
            continue;
        }
    ...
        key_pos = key.Pos(".inncost");
        if (key_pos > 0)
        {
            records[inn_count - 1].sleep_cost = ReadNextNumber();
            continue;
        }
```

- **What the lines were written to do:** Read two costs: the cost of sleeping at the inn, and the inn's own cost.
- **What they do:** Both keys write `sleep_cost`; the inn record has no other cost.
- **When it happens:** Each of the two keys in `inns.ini`.
- **What you see:** Whichever of the two comes later in the file is the inn's sleep cost.

## A drop rate with one digit before its decimal point gains a 0 after that digit

`Npcvalues.cpp`, `NpcValues::ParseRate`

```cpp
    if (text.Length() > 1 && text[2] == '.')
        text.Insert("0", 2);
```

- **What the function is for:** `ParseRate` reads a drop rate with up to two decimals in hundredths, "12.5" as 1250.
- **What the line was written to do:** Give a one-digit whole part a second digit, so that every rate has the same
  shape; the evidence is the test, which looks for the dot right after the first digit.
- **What it does:** `Insert("0", 2)` puts the 0 in front of the second character, the dot, so the 0 comes after the
  digit: "5.5" becomes "50.5".
- **When it happens:** Each drop line of `drops.ini` whose rate has one digit before its dot.
- **What you see:** A rate of 5.5 is exported as 50.5 (5050), ten times the rate and more; "12.5" and "5" read right.

## A number encoder for text that nothing calls

`UnitMain.cpp`, `TMainForm::EncodeNumber(String text, int width)`, right before `TMainForm::EncodeNumber(unsigned
int value, int width)`

```cpp
String TMainForm::EncodeNumber(String text, int width)
{
    if (text == "")
        text = "0";
    return EncodeNumber(StrToInt(text), width);
}
```

- **What the function is for:** It encodes a number given as its text, an empty text as 0. The pub editor has the same
  function, right before the same number encoder, and its export form calls it with the grids' cell texts.
- **What it does here:** Nothing calls it: every writer of EoImport passes numbers. The linker leaves its code out of
  the exe, but the compiler has already laid its two strings, `""` and `"0"`, among the unit's others, so they stay in
  the exe's data, between `ExportInns`'s strings and `EncodeNumber`'s.
- **What you see:** Nothing.

## Readers and writers nothing calls

`Configuration.cpp`, `Config::ReadInteger` and `Config::ReadColor`, right after `Config::ReadSwitch`;
`Config::SaveSettings` and `Config::SwitchText`, right after `Config::LoadSettings`

- **What the functions are for:** `ReadColor` reads a color setting, `DEFAULT` as 0, through `ReadInteger`, which reads
  a number of digits only. `SaveSettings` writes the settings back to the program's `.ini` file under the comments that
  explain them, each switch as `on` or `off` through `SwitchText`. The game client's configuration has the same two
  readers in the same place, and the pub editor writes the same file with the same lines.
- **What they do here:** Nothing calls them: EoImport reads its settings and never writes them. The linker leaves
  their code out of the exe, but their strings stay in its data, in the order the functions use them: `"DEFAULT"`
  `"0"` between `ReadSwitch`'s strings and `LoadSettings`', and the configuration file's lines, `"./"` `".ini"`, then
  `"off"` `"on"` after `LoadSettings`'. `ReadInteger` uses no string, so where it stood can't be seen; it is written
  next to the `ReadColor` that calls it.
- **What you see:** Nothing.

## Numbers the source keeps as numbers

Two kinds of value are written as the numbers the exe holds, because no declaration ties them to a name:

- `ModalResult` is set and compared as 1 and 3 (`UnitFileExist.cpp`, `UnitMain.cpp`). The property's type is
  `TModalResult`, a plain `int`; the named results `mrOk` (1) and `mrAbort` (3) are declared as `Shortint` constants,
  not of that type.
- `ShellExecuteA` is passed 1 for its show command (`UnitMain.cpp`, `UnitFileNotFound.cpp`). The parameter is a plain
  `INT`; `SW_SHOWNORMAL` (1) is a macro.
