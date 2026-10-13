# EOSoda: quirks

These are behaviours of the original EOSoda as it shipped. The source keeps every one of them byte for byte, because
changing any of them would change the exe. They are collected here, each with what the line was written to do and what
it does instead, so the comments in `src/` can stay about what the code is for.

## Letters the first pass inserts are read again by the same pass, so a doubled letter can run on

`Unit1.cpp`, `TForm1::Sodanize` (`0x004015d8`)

```cpp
        for (int i = 1; i <= text.Length(); i++)
        {
            char vowel_end = 0;
    ...
            if (!vowel_end && text[i] != ' ')
            {
                int double_roll = random(10);
                if (double_roll < 4)
                    text.Insert(text[i], i + 1);
            }
        }
    ...
            if (hic_roll < 3)
            {
                text.Insert("*hic* ", k + 1);
                k += 6;
            }
```

- **What the function is for:** `Sodanize` turns the text typed into the edit box into drunk-sounding chat text in three
  passes over the string; `buttonOkClick` shows the result and can copy it to the clipboard.
- **What the line was written to do:** Roll once for each letter of the typed text. The evidence: the third pass, which
  inserts "*hic* ", moves its index past what it inserted with `k += 6;`, and the first pass has no such step after any
  of its inserts.
- **What it does:** Step by step:
  - The first pass walks `i` from 1 while `i <= text.Length()`, and `text.Length()` grows with every insert.
  - When `double_roll` (`random(10)`) is below 4, `text.Insert(text[i], i + 1);` puts a copy of the letter right after
    it.
  - The next turn of the loop reaches that copy at `i + 1` and rolls for it again, so each copy has the same 4 in 10
    chance of getting one more copy after it.
  - The inserted "j", "w" and "u" are read the same way: the "u" put after an `i` is then tested as a vowel by the
    `'u' || 'o'` test, and can get a "w" after it.
- **When it happens:** Each conversion: a click on the OK panel (`buttonOk`, whose OnClick event the form sets to
  `buttonOkClick`) or Enter in the edit box (`Edit1KeyUp` calls `buttonOkClick` when `Key == 13`).
  - `TForm1::Sodanize` is called from `TForm1::buttonOkClick` (`String sodanized = Sodanize(this, Edit1->Text);`).
  - `TForm1::buttonOkClick` is called from `TForm1::Edit1KeyUp` (`buttonOkClick(buttonOk);`).
  - Reachable with the shipped code: reachable. Any typed letter that is not a vowel the pass slurs or a space reaches
    the doubling roll.
- **What you see:** A doubled letter gets 1 extra copy with odds 4 in 10, 2 extra copies with odds 16 in 100, 3 with 64
  in 1000, and so on, so "hello" can come out as "hellllo".

## A capital I gets no "u" after it in the first pass, and it can be doubled where a small i cannot

`Unit1.cpp`, `TForm1::Sodanize` (`0x004015d8`)

```cpp
            if (text[i] == 'e' || text[i] == 'a' || text[i] == 'E' || text[i] == 'A')
    ...
            if (text[i] == 'u' || text[i] == 'o' || text[i] == 'U' || text[i] == 'O')
    ...
            if (text[i] == 'i')
            {
                vowel_end = 1;
                int u_roll = random(10);
                if (u_roll < 4)
                    text.Insert("u", i + 1);
            }
            if (!vowel_end && text[i] != ' ')
```

- **What the function is for:** `Sodanize` turns the text typed into the edit box into drunk-sounding chat text in three
  passes over the string; the first pass slurs vowels and doubles letters.
- **What the line was written to do:** Treat `i` and `I` alike, as the first pass does for its other vowels. The
  evidence: the tests above it read `'e' || 'a' || 'E' || 'A'` and `'u' || 'o' || 'U' || 'O'`, and the second pass tests
  both `'I'` and `'i'`.
- **What it does:** Step by step:
  - `if (text[i] == 'i')` tests the small letter only.
  - For `i` it sets `vowel_end = 1;`, so the doubling roll at the end of the loop body is skipped, and when `u_roll`
    (`random(10)`) is below 4 it inserts "u" after the letter.
  - For `I` that block does not run: `vowel_end` stays 0, so `text[i] != ' '` lets the doubling roll copy it with odds 4
    in 10.
- **When it happens:** Each conversion of a text with a capital I: a click on the OK panel (`buttonOk`) or Enter in the
  edit box.
  - `TForm1::Sodanize` is called from `TForm1::buttonOkClick` (`String sodanized = Sodanize(this, Edit1->Text);`).
  - `TForm1::buttonOkClick` is called from `TForm1::Edit1KeyUp` (`buttonOkClick(buttonOk);`).
  - Reachable with the shipped code: reachable. Any text with a capital I reaches it.
- **What you see:** "it" can come out as "iut", while "It" can come out as "IIt" and gets no "u".

## The clipboard label puts ".." after the text even when the whole text fits in it

`Unit1.cpp`, `TForm1::buttonOkClick` (`0x00401e34`)

```cpp
    if (CheckBox1->Checked)
    {
        if (sodanized.Length() > 0)
        {
            CopyToClipboard(sodanized);
            Timer1->Enabled = true;
            Label3->Caption = "'" + sodanized.SubString(1, 22) + "..' copied to clipboard";
```

- **What the function is for:** `buttonOkClick` converts the edit box's text with `Sodanize`, shows it in
  `MemoSodanized` and, when `CheckBox1` is ticked, copies it to the clipboard and shows a line in `Label3` that
  `Timer1Timer` hides again.
- **What the line was written to do:** Show the start of a long result with ".." to say that it goes on. The evidence:
  `Sodanize` closes its own cut text with the same "..", and only under `if (text.Length() >= 128)`.
- **What it does:** `sodanized.SubString(1, 22)` gives the first 22 characters, or the whole text when it is shorter,
  and "..'" is added after it whatever its length.
- **When it happens:** Each conversion with `CheckBox1` ticked and a result of 1 character or more: a click on the OK
  panel or Enter in the edit box.
  - `TForm1::buttonOkClick` is called from `TForm1::Edit1KeyUp` (`buttonOkClick(buttonOk);`).
  - Reachable with the shipped code: reachable. Any text of 22 characters or fewer, with the box ticked, shows it.
- **What you see:** Typing "hi" can give the label 'hhi..' copied to clipboard, though the clipboard holds the whole
  text, "hhi".

## A result of exactly 128 characters is still cut, and its last two characters become ".."

`Unit1.cpp`, `TForm1::Sodanize` (`0x004015d8`)

```cpp
    if (text.Length() >= 128)
    {
        text = text.SubString(1, 126);
        text.Insert("..", text.Length() + 1);
    }
```

- **What the function is for:** `Sodanize` turns the text typed into the edit box into drunk-sounding chat text; its
  last step keeps the result short enough for one chat message.
- **What the line was written to do:** Keep the result to 128 characters. The evidence: what the cut leaves, 126
  characters and "..", is 128 characters.
- **What it does:** Step by step:
  - A result of 128 characters passes `text.Length() >= 128`.
  - `text = text.SubString(1, 126);` drops its last two characters.
  - `text.Insert("..", text.Length() + 1);` adds two dots at the end: 128 characters again.
- **When it happens:** Each conversion whose result is 128 characters or longer: a click on the OK panel or Enter in the
  edit box.
  - `TForm1::Sodanize` is called from `TForm1::buttonOkClick` (`String sodanized = Sodanize(this, Edit1->Text);`).
  - Reachable with the shipped code: reachable. A typed text that the passes grow to exactly 128 characters reaches it.
- **What you see:** A result of exactly 128 characters ends in ".." in place of its last two characters; one of 127
  characters comes out whole.

## The clipboard copy frees the memory block it has just handed to the clipboard

`Unit1.cpp`, `TForm1::CopyToClipboard` (`0x00402050`)

```cpp
    memory = GlobalAlloc(2, text.Length() + 1);
    ...
        memcpy(GlobalLock(memory), text.c_str(), text.Length() + 1);
        GlobalUnlock(memory);
        SetClipboardData(1, memory);
        CloseClipboard();
        GlobalFree(memory);
```

- **What the function is for:** `CopyToClipboard` puts the converted text on the Windows clipboard as plain text;
  `buttonOkClick` calls it when `CheckBox1` is ticked.
- **What the line was written to do:** Give back the block the function took with `GlobalAlloc`. The evidence: the
  function closes each thing it opens, `GlobalUnlock(memory);` after `GlobalLock(memory)` and `CloseClipboard();` after
  `OpenClipboard`, and `GlobalFree(memory);` stands as the pair of `GlobalAlloc(2, text.Length() + 1)`.
- **What it does:** Step by step:
  - `GlobalAlloc(2, text.Length() + 1)` takes a movable block (flag 2) one byte longer than the text, and `memcpy`
    copies the text and its closing zero into it.
  - `SetClipboardData(1, memory);` gives the block to the clipboard as plain text (format 1).
  - After `CloseClipboard();` the function calls `GlobalFree(memory);` on the same block. The Windows documentation of
    `SetClipboardData` says the system owns the block from then on and the program must not free it.
- **When it happens:** Each conversion with `CheckBox1` ticked and a result of 1 character or more: a click on the OK
  panel or Enter in the edit box.
  - `TForm1::CopyToClipboard` is called from `TForm1::buttonOkClick` (`CopyToClipboard(sodanized);`).
  - `TForm1::buttonOkClick` is called from `TForm1::Edit1KeyUp` (`buttonOkClick(buttonOk);`).
  - Reachable with the shipped code: reachable. Any conversion with the box ticked reaches it.
- **What you see:** What a later paste gives depends on how Windows keeps clipboard data: the program has freed the
  block it handed over.

## Small ones with no effect

- **`Sodanize` rolls one random number it does not use.** `Unit1.cpp`, `TForm1::Sodanize` (`0x004015d8`).
  - Code: `int unused_roll = random(10);`
  - What it does: `int unused_roll = random(10);` runs before the first pass, and no later line reads `unused_roll`.
  - When it happens: Each conversion of a text of 1 character or more. The search for `\bunused_roll\b` finds 1 line(s).
    Reachable with the shipped code: reachable. Any text of 1 character or more reaches it.
  - What you see: None that shows. The roll takes one number from the random sequence before the first pass.
- **Entering the result box sets the cursor after the first character, then selects all of the text.** `Unit1.cpp`,
  `TForm1::MemoSodanizedEnter` (`0x004021bc`).
  - Code: `MemoSodanized->SelStart = 1; MemoSodanized->SelectAll();`
  - What it does: `MemoSodanized->SelStart = 1;` puts the cursor after the first character, and
    `MemoSodanized->SelectAll();` on the next line selects the whole text, so the first line has no effect.
  - When it happens: Each time `MemoSodanized` gets the focus (its OnEnter event, set in the form). Reachable with the
    shipped code: reachable. Any click or tab into the result box reaches it.
  - What you see: None. The whole result is selected, ready to copy.
