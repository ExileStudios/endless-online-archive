# EoPub: quirks

These are traits of the original EoPub, the pub editor, as it shipped. The source keeps every one of them byte for
byte, because changing any of them would change the exe. Each is listed here with what the line was written to do and
what it does instead, so the comments in `src/` can stay about what the code is for.

## The new folder form still links the folder dialog it no longer holds

`UnitNewFolder.cpp`, the unit's link records

```cpp
#pragma package(smart_init)
#pragma link "ElFolderDlg"
#pragma link "ElPopBtn"
#pragma resource "*.dfm"
```

- **What the lines are for:** The IDE writes a `#pragma link` line for each library unit whose component is dropped on
  a form, so the program links that unit.
- **What the line was written to do:** Link the folder dialog of a `TElFolderDialog` dropped on the new folder form.
- **What it does:** The dialog was later deleted from the form, and the IDE does not remove a link line when it removes
  the component. No form holds a folder dialog and no code uses one, yet the dialog's unit, and the shell helpers it
  uses, still ship in the program. They start up just ahead of `UnitNewFolder`.
- **When it happens:** Each start of the program.
- **What you see:** Nothing. The program is larger by the dialog's code.

## The skill master records link the class and packet units they never use

`Learnvalues.cpp`, the unit's link records

```cpp
#pragma package(smart_init)
#pragma link "Packetbuilder"
#pragma link "Classvalues"
```

- **What the lines are for:** A `#pragma link` line makes the linker take the named unit, and puts that unit's start-up
  code ahead of this one's.
- **What the lines were written to do:** Nothing the skill master records need. `LearnValues` uses neither unit, and
  `Packetbuilder` is an empty unit that nothing in the program refers to.
- **What they do:** They decide the start-up order. The program starts `ClassValue`, `ClassValues` and `Packetbuilder`
  right before `LearnValues`, and only these two lines explain that order. No include, project unit list or shared
  header gives it.
- **When it happens:** Each start of the program.
- **What you see:** Nothing.

## An NPC record object that no code makes

`Npcvalue.cpp`, `Npcvalue_UnusedLocals1`

```cpp
void Npcvalue_UnusedLocals1()
{
    NpcValue object;
}
```

- **What the function is for:** It stands for a use of `NpcValue`'s default constructor that the source once had.
- **What the code was written to do:** Make an NPC record with the default constructor, `NpcValue()`.
- **What it does:** Nothing in the program calls it, so the linker drops its code: no code of it is in the image. The
  compiler still wrote the type descriptors of `NpcValue` and of a pointer to it where that use stood, and the linker
  keeps them in their place. The descriptor's place is the trace, and the function keeps it there.
- **When it happens:** Never; the function is not called.
- **What you see:** Nothing.

## The tree view sets an item's overlay image before it draws it

The program's ElPack tree view, as linked

- **What the code is for:** The data grids are ElPack tree views. When a tree item shows a state image with an overlay
  image, the tree view draws the overlay on top of it.
- **What it was written to do:** Draw the overlay from the image list's overlay index.
- **What it does:** The ElPack the program was built with first sets that overlay index in the image list, then draws
  it. That is one line more than the ElPack 4.01 Update 1 source has, and it is why the program imports
  `ImageList_SetOverlayImage` from `comctl32.dll`.
- **When it happens:** Each draw of a tree item that has a state image with an overlay.
- **What you see:** The overlay as the image list holds it. Building the program needs an ElPack with this change.
