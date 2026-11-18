# GameClient: quirks

These are behaviours of the original client as it shipped. The source keeps every one of them byte for byte, because
changing any of them would change the exe. They are collected here, each with what the line was written to do and what
it does instead, so the comments in `src/` can stay about what the code is for.

## The class file download counts its parts from the spell file's record count

`Packetproces.cpp`, `PacketProcessor::DispatchPacket`

```cpp
                int skill_checksum_high = DecodeNumber(packet.SubString(30, 2));
                int skill_checksum_low = DecodeNumber(packet.SubString(32, 2));
                int skill_record_count = DecodeNumber(packet.SubString(34, 2));
    ...
                    MainForm->skill_values->expected_part_count = skill_record_count / PUB_RECORDS_PER_FILE + 1;
    ...
                int class_checksum_high = DecodeNumber(packet.SubString(36, 2));
                int class_checksum_low = DecodeNumber(packet.SubString(38, 2));
                int class_record_count = DecodeNumber(packet.SubString(40, 2));
                MainForm->class_values->ReadHeader();
                MainForm->class_values->download_requested = false;
                if (MainForm->class_values->checksum_high != class_checksum_high ||
                    MainForm->class_values->checksum_low != class_checksum_low ||
                    MainForm->class_values->header_record_count != class_record_count)
                {
                    MainForm->class_values->received_part_count = 1;
                    MainForm->class_values->expected_part_count = skill_record_count / CLASS_RECORDS_PER_FILE + 1;
    ...
        if (action == AC_SYN5)
        {
            MainForm->class_values->received_part_count++;
            MainForm->class_values->SaveFilePart(DecodeNumber((char)packet[1]), packet.SubString(2, packet.Length()));
            if (MainForm->class_values->received_part_count > MainForm->class_values->expected_part_count ||
                !MainForm->class_values->is_downloading)
```

- **What the function is for:** `PacketProcessor::DispatchPacket` handles every packet from the server. Welcome reply 1
  checks the map and the four data files on disk against the server's and starts a download of each file that differs.
- **What the line was written to do:** Count the class file's parts from `class_record_count`, read from the same reply
  a few lines above and compared with the class file's `record_count` in the line before. The evidence: the item, npc
  and spell blocks each set `expected_part_count` from their own count (`item_record_count / 900`,
  `npc_record_count / 900`, `skill_record_count / 900`), and the class block is the only one that reads another block's
  count. And `ClassValues::ReadHeader` finds the last class part file on disk from the class file's own count,
  `total_records / 250 + 1`, as the item, npc and spell headers do with `total_records / 900 + 1`.
- **What it does:** Step by step:
  - The reply holds, for each file, two checksum words and a record count: the spells at bytes 30, 32 and 34
    (`skill_record_count`), the classes at bytes 36, 38 and 40 (`class_record_count`).
  - When the class file differs, `received_part_count` of `class_values` is set to 1 and `expected_part_count` to
    `skill_record_count / 250 + 1`.
  - `RequestGameData` asks for class part `received_part_count`. Each class part (action 244) adds 1 to it, and the
    download ends when it is more than `expected_part_count`.
  - For example, with 300 spells and 20 classes, `expected_part_count` is 300 / 250 + 1 = 2, where the class count gives
    20 / 250 + 1 = 1. After part 1 the count is 2, not more than 2, so the client asks for class part 2 and the download
    ends only with the next class reply. With fewer than 250 spells and fewer than 250 classes both give 1.
- **Evidence, from the code and not from the names:**
  - spell count: `Packetproces.cpp` `int skill_record_count = DecodeNumber(packet.SubString(34, 2));` the third
    value of the spell group (bytes 30, 32, 34), compared with the spell file's record_count
  - class count: `Packetproces.cpp` `int class_record_count = DecodeNumber(packet.SubString(40, 2));` the third
    value of the class group (bytes 36, 38, 40), compared with the class file's header_record_count and not used after
- **When it happens:** Each time a character enters the game while the class file on disk differs from the server's (a
  checksum word or the record count). The welcome reply (`AC_RESULT` of `OP_PLAY`) comes through `Client::ReceiveData`,
  the class parts (`AC_SYN5`) through `Client::HandleRawPacket`, and `TMainForm::RenderMainLoop_Menu` sends the part
  requests with `RequestGameData` each time `IsRequestDelayDone` allows one.
  - `PacketProcessor::DispatchPacket` is called from `Client::HandleRawPacket`
    (`processor->DispatchPacket(AC_SET, OP_MAP, frame.SubString(4, frame.Length() - 3));`); `Client::HandleRawPacket`
    (`processor->DispatchPacket(AC_SYN1, OP_PLAY, frame.SubString(4, frame.Length() - 3));`); `Client::HandleRawPacket`
    (`processor->DispatchPacket(AC_SYN2, OP_PLAY, frame.SubString(4, frame.Length() - 3));`); `Client::HandleRawPacket`
    (`processor->DispatchPacket(AC_SYN3, OP_PLAY, frame.SubString(4, frame.Length() - 3));`); `Client::HandleRawPacket`
    (`processor->DispatchPacket(AC_SYN4, OP_PLAY, frame.SubString(4, frame.Length() - 3));`); `Client::HandleRawPacket`
    (`processor->DispatchPacket(AC_SYN5, OP_PLAY, frame.SubString(4, frame.Length() - 3));`); `Client::HandleRawPacket`
    (`processor->DispatchPacket(AC_LIST, OP_PLAYERS, frame.SubString(4, frame.Length() - 3));`);
    `Client::HandleRawPacket` (`processor->DispatchPacket(AC_NEW, OP_MAP, frame.SubString(4, frame.Length() - 3));`);
    `Client::HandleRawPacket`
    (`processor->DispatchPacket(AC_RESULT, OP_PLAYERS, frame.SubString(4, frame.Length() - 3));`); `Client::ReceiveData`
    (`processor->DispatchPacket(action, family, frame.SubString(3, frame.Length() - 2));`).
  - `PacketOperator::RequestGameData` is called from `TMainForm::RenderMainLoop_Menu`
    (`net_client->sender->RequestGameData();`).
  - Search of the tree for `expected_part_count = skill_record_count`: 2 line(s), in Packetproces.cpp.
  - Search of the tree for `/ CLASS_RECORDS_PER_FILE`: 1 line(s), in Packetproces.cpp.
  - Reachable with the shipped code: unknown. It is decided by the server's files: the part count changes only when the
    spell file or the class file has 250 records or more, and the client cannot see those counts before the reply.
- **What you see:** With 250 spells or more and fewer classes, the client asks the server for more class parts than the
  class record count gives, and the class file stays marked as needed until a reply to the last one arrives. What the
  server sends for that part is outside the client; each reply is saved with `SaveFilePart` under the part number it
  carries.

## Guild reply 14 pairs the heading of the guild rank message (73) with the text of message 82

`Packetproces.cpp`, `PacketProcessor::DispatchPacket`

```cpp
            if (guild_reply == GuildReply_RecruiterNotHere)
                MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None, MainForm->strings->GetDialogText(DialogText_RecruiterNotFoundNeeds, false),
                    MainForm->strings->GetDialogText(DialogText_RecruiterNotFoundNeeds, true));
    ...
            if (guild_reply == GuildReply_Unknown14)
                MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None, MainForm->strings->GetDialogText(DialogText_RefusedRanking, false),
                    MainForm->strings->GetDialogText(DialogText_RecruiterNotFoundNeeds, true));
```

- **What the function is for:** `PacketProcessor::DispatchPacket` handles every packet from the server. Family 39,
  action 3 is the guild reply: one message box per reply number.
- **What the line was written to do:** Show one message whole, its title and its text. The evidence: the other replies
  of the group pass the same message number twice, as reply 10 does with (82, 0) and (82, 1), and the guild window's
  rank check shows message 73 with (73, 0) and (73, 1).
- **What it does:** Step by step:
  - `guild_reply` 14 calls `ShowMessageBox` with `GetDialogText(DialogText_RefusedRanking, false)` as the title and
    `GetDialogText(DialogText_RecruiterNotFoundNeeds, true)`
    as the text.
  - The text of message 73, (73, true), is shown by one line of the tree, in `GuildController::MouseDown`. The text of
    message 82 is also shown by reply 10, under its own title (82, false).
- **Evidence, from the code and not from the names:**
  - message 73: `Ctrl_guild.cpp`
    `MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None, MainForm->strings->GetDialogText(DialogText_RefusedRanking, false),`
    the rank check (guild_rank above 1) shows message 73 with its own text, (73, 1), on the next line
  - message 82: `Packetproces.cpp`
    `MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None, MainForm->strings->GetDialogText(DialogText_RecruiterNotFoundNeeds, false),`
    reply 10 shows message 82 with its own text, (82, 1), on the next line
- **When it happens:** When the server sends guild reply 14 (family 39, action 3), which comes through
  `Client::ReceiveData`. Which guild request draws reply 14 is up to the server.
  - `PacketProcessor::DispatchPacket` is called from `Client::HandleRawPacket`
    (`processor->DispatchPacket(AC_SET, OP_MAP, frame.SubString(4, frame.Length() - 3));`);
    `Client::HandleRawPacket`
    (`processor->DispatchPacket(AC_SYN1, OP_PLAY, frame.SubString(4, frame.Length() - 3));`);
    `Client::HandleRawPacket`
    (`processor->DispatchPacket(AC_SYN2, OP_PLAY, frame.SubString(4, frame.Length() - 3));`);
    `Client::HandleRawPacket`
    (`processor->DispatchPacket(AC_SYN3, OP_PLAY, frame.SubString(4, frame.Length() - 3));`);
    `Client::HandleRawPacket`
    (`processor->DispatchPacket(AC_SYN4, OP_PLAY, frame.SubString(4, frame.Length() - 3));`);
    `Client::HandleRawPacket`
    (`processor->DispatchPacket(AC_SYN5, OP_PLAY, frame.SubString(4, frame.Length() - 3));`);
    `Client::HandleRawPacket`
    (`processor->DispatchPacket(AC_LIST, OP_PLAYERS, frame.SubString(4, frame.Length() - 3));`);
    `Client::HandleRawPacket`
    (`processor->DispatchPacket(AC_NEW, OP_MAP, frame.SubString(4, frame.Length() - 3));`);
    `Client::HandleRawPacket`
    (`processor->DispatchPacket(AC_RESULT, OP_PLAYERS, frame.SubString(4, frame.Length() - 3));`);
    `Client::ReceiveData` (`processor->DispatchPacket(action, family, frame.SubString(3, frame.Length() - 2));`).
  - Search of the tree for `GetDialogText\(73, true\)`: 0 line(s).
  - Search of the tree for `GetDialogText\(73, false\)`: 0 line(s).
  - Search of the tree for `GetDialogText\(82, true\)`: 0 line(s).
  - Reachable with the shipped code: unknown. The client shows the box for any guild reply 14; whether the server sends
    that reply is outside the client.
- **What you see:** The player sees a box headed with the title of message 73, the box the guild window shows a member
  of rank above 1 who opens a leader's page, and under it the text of message 82, the text reply 10 shows.

## A right click on a group chat line picks the line by the global tab's scroll position

`Ctrl_chat.cpp`, `ChatController::OnMouseClick`

```cpp
    if (active_tab == 4 && button == 1 && mouse_x >= 127 && mouse_x <= 560 && mouse_y >= 333 && mouse_y <= 430)
    {
        global_row = scroll_offsets[3] + (mouse_y - 333) / 14;
    ...
    if (active_tab == 5 && button == 1 && mouse_x >= 127 && mouse_x <= 560 && mouse_y >= 333 && mouse_y <= 430)
    {
        group_row = scroll_offsets[3] + (mouse_y - 333) / 14;
        if (group_senders->Count > group_row)
        {
            if (group_senders->Strings[group_row] != "")
            {
                MainForm->edits->SetText(EditId_Chat, "!" + group_senders->Strings[group_row] + " ");
```

- **What the function is for:** `ChatController::OnMouseClick` handles a click on the chat panel: the tab buttons, the
  scroll bar, and a right click on a chat line, which puts "!" and that line's sender in the chat box.
- **What the line was written to do:** Find the clicked line of the group tab from the group tab's own scroll position,
  `scroll_offsets[4]`. The evidence: the local tab (tab 3) uses `scroll_offsets[2]` and the global tab (tab 4)
  `scroll_offsets[3]` in the two blocks above; `AddGroupMessage` keeps the group tab's position in `scroll_offsets[4]`,
  and `GetScrollOffset`, which the panel draws the open tab with, returns `scroll_offsets[active_tab - 1]`, so 4 for the
  group tab (tab 5).
- **What it does:** Step by step:
  - With the group tab open (`active_tab` 5), a right click on the text area takes the row under the mouse, 14 pixels a
    row, and adds `scroll_offsets[3]`, the global tab's first shown line.
  - That row of `group_senders` goes into the chat box as "!" and the name.
- **Evidence, from the code and not from the names:**
  - group tab: `Ctrl_chat.cpp` `scroll_offsets[4] = group_lines->Count - 7;` the group tab's first shown line is
    kept in scroll_offsets[4]
  - global tab: `Ctrl_chat.cpp` `int global_row = (mouse_y - 333) / 14 + scroll_offsets[3];` a click on the global
    tab (active_tab 4) reads scroll_offsets[3]
- **When it happens:** A right click on the text area of the chat panel while the group tab is open.
  `Mouser::HandleMouseDown` calls `OnMouseClick` for every click on the panel.
  - `ChatController::OnMouseClick` is called from `Mouser::HandleMouseDown`
    (`MainForm->animation->interface_controller->chat->OnMouseClick(click_x, click_y, button);`).
  - Search of the tree for `scroll_offsets\[3\]`: 7 line(s), in Ctrl_chat.cpp.
  - Reachable with the shipped code: reachable. Any right click on a group line once the group tab and the global tab
    are scrolled to different lines.
- **What you see:** With 30 group lines (the tab shows the last 7, from line 23) and the global tab at line 0, a right
  click on the top row puts the sender of group line 0 in the chat box instead of the sender of line 23. When the global
  tab is further down than the group tab has lines, nothing is put in the box.

## A chat message that wraps to six lines writes the sixth line past the end of its line list

`Ctrl_chat.cpp`, `ChatController::AddLocalMessage`; `Ctrl_chat.cpp`, `ChatController::AddPrivateMessage`; `Ctrl_chat.cpp`, `ChatController::AddGlobalMessage`; `Ctrl_chat.cpp`,
`ChatController::AddGroupMessage`

```cpp
    String wrapped_lines[MAX_WRAPPED_CHAT_LINES];
    ...
                for (int i = 0; i <= 5; i++)
                {
                    if (text.Length() < next_start)
                        continue;
                    line_count++;
    ...
                                wrapped_lines[i] = text.SubString(line_start, j - line_start + 1);
    ...
            for (int line_index = 0; line_index < line_count; line_index++)
            {
                local_colors->Add(IntToStr(color));
                local_lines->Add(wrapped_lines[line_index]);
    ...
    String wrapped_lines[4];
```

- **What the function is for:** The four functions add a message to a chat tab. Each one wraps the text to the chat
  width, 420 pixels, keeps the wrapped lines in a local list, `wrapped_lines`, and then adds them to the tab.
- **What the line was written to do:** Hold every wrapped line of the message. The evidence: the wrap loop makes up to
  six lines (its counter runs from 0 to 5), counts each one in `line_count`, and the add loop reads one entry of
  `wrapped_lines` for every counted line.
- **What it does:** Step by step:
  - `wrapped_lines` holds five strings in each of the four functions.
  - Each pass of the wrap loop stores its line in the entry of the loop's counter, for a counter up to 5. A C++ array
    does not check its index, so the sixth line (counter 5) is stored in the memory after the list.
  - The add loop then steps through the list four bytes at a time for `line_count` entries and reads that same place
    back for the tab.
- **When it happens:** Every message these functions add whose text needs six wrapped lines: about 2,100 pixels of text
  or more in MS Sans Serif 8. They are called for local, private, global and group chat and for most server notices.
  - Search of the tree for `AddLocalMessage\(`: 33 line(s), in Ctrl_chat.cpp, Ctrl_chat.h, Packetproces.cpp,
    Sys_keyboard.cpp.
  - Search of the tree for `AddPrivateMessage\(`: 5 line(s), in Ctrl_chat.cpp, Ctrl_chat.h, Packetproces.cpp,
    Sys_keyboard.cpp.
  - Search of the tree for `AddGlobalMessage\(`: 14 line(s), in Ctrl_chat.cpp, Ctrl_chat.h, Packetproces.cpp,
    Sys_keyboard.cpp.
  - Search of the tree for `AddGroupMessage\(`: 12 line(s), in Ctrl_chat.cpp, Ctrl_chat.h, Packetproces.cpp,
    Sys_keyboard.cpp.
  - Reachable with the shipped code: unknown. It is decided by the length of the text. Typed chat is limited to 128
    characters (edit 11, `SetMaxLength`); text that comes from the server, such as an npc's speech (family 26, action
    34), has no limit in the client.
- **What you see:** The fourth to sixth lines are written into memory the code does not define, next to the function's
  other local values, and the tab shows whatever is read back from there.

## The chat log counts as open even when its file could not be opened, and chat lines are then written to no file

`Filelogger.cpp`, `FFileLog::Open`

```cpp
void FFileLog::Open(String path)
{
    try
    {
        file = fopen(file_path.c_str(), "a");
        is_open = true;
    }
    catch (...)
    {
        is_open = false;
    }
```

- **What the function is for:** `FFileLog::Open` opens the chat log file so that `WriteLine` can add each chat line to
  it.
- **What the line was written to do:** Mark the log open only when the file opened. The evidence: the open is wrapped in
  `try` with `catch (...)` setting `is_open` to 0, and `WriteLine`, `Close` and the destructor all test `is_open` before
  they use `file`.
- **What it does:** Step by step:
  - `access` tests that the file exists; only then does `Open` call `fopen` and set `is_open`.
  - `fopen` (a C library call) reports a file it cannot open by returning NULL; it does not throw, so the `catch` block
    does not run.
  - `is_open` is set to 1 whatever `fopen` returned.
  - Each chat line then reaches `fprintf(file, ...)` with `file` NULL, and `Close` or the destructor calls `fclose` on
    it.
- **When it happens:** When the chat log is switched on and the file cannot be opened: at start with LogChat=ON
  (`TMainForm::FormCreate` creates the log with `GetLogChat`), or when the log button of the settings window switches it
  on. The path comes from LogFile, read in upper case and without spaces, so a path with a space names another folder.
  - `FFileLog::Open` is called from `FFileLog::FFileLog` (`Open(file_path);`); `SettingsController::MouseDown`
    (`MainForm->chat_log->Open(MainForm->config->GetChatLogPath());`).
  - Search of the tree for `new FFileLog\(`: 1 line(s), in TMainForm.cpp.
  - Reachable with the shipped code: reachable. Any LogFile path whose folder does not exist, or a file the user may not
    write, with the log switched on.
- **What you see:** With "LogFile=C:\My Logs\chat.txt" the client opens "C:\MYLOGS\CHAT.TXT"; when that folder does not
  exist, the log counts as open and every chat line is handed to `fprintf` with no file, which the code does not define.

## The last item of the item file has no graphic, type, weight or other values

`Itemvalues.cpp`, `ItemValues::GetWeight`; `Itemvalues.cpp`, `ItemValues::GetGender`;
`Itemvalues.cpp`, `ItemValues::GetGraphic`; `Itemvalues.cpp`, `ItemValues::GetSize`;
`Itemvalues.cpp`, `ItemValues::GetSpecial`; `Itemvalues.cpp`, `ItemValues::GetType`;
`Itemvalues.cpp`, `ItemValues::GetSubType`; `Itemvalues.cpp`, `ItemValues::GetClassRequirement`; `Itemvalues.cpp`, `ItemValues::GetSpecValue`

```cpp
int ItemValues::GetGraphic(int item_id)
{
    int graphic = -1;
    if (item_id > 0 && (unsigned int)Count() > item_id)
        graphic = records[item_id - 1].graphic;
    return graphic;
    }
```

- **What the function is for:** The nine getters return one value of an item from the item list, or a default when the
  id is not in the list.
- **What the line was written to do:** Accept every id from 1 to the item count. The evidence: `Load` adds the items
  with ids `Count() + 1`, so item n is `records[n - 1]` and the last id equals `Count()`; `GetName` and `GetKeyName`
  search the whole list and do find the last item.
- **What it does:** Step by step:
  - Each getter takes the record only when `Count() > item_id`, so the id equal to `Count()` fails the test.
  - The getter returns its default for that id: 0 from `GetWeight`, -1 from the eight others.
- **Evidence, from the code and not from the names:**
  - ids: `Itemvalues.cpp` `AddItem(Count() + 1, file_text.SubString(2, name_end - 1),` item n is records[n - 1]
  - key: `Itemvalues.cpp` `if (it->type == 9 && key_id == it->spec_value)` the key lookup searches every record
- **When it happens:** Every caller that asks for a value of the last item: the inventory, paperdoll, shop, chest,
  locker and map code call these getters. `UseItem` reads the type first.
  - Search of the tree for `Count\(\) > \(unsigned int\)item_id`: 0 line(s).
  - Search of the tree for `item_values->GetGraphic\(`: 29 line(s), in Ctrl_inventory.cpp, Ctrl_paperdoll.cpp,
    Gfx_game.cpp, Gfx_interface.cpp, TMainForm.cpp.
  - Reachable with the shipped code: reachable. The item with the highest id in the server's item file is the last
    record of the list.
- **What you see:** With 500 items, item 500 is drawn with graphic -1, weighs 0, and `UseItem` gets type -1, which
  matches none of its item types, so using it does nothing; its name still shows, from `GetName`.

## A word wider than the text window makes the client add empty lines without end

`Ctrl_menu.cpp`, `MenuController::AddWrappedText`

```cpp
            do
            {
                String fitting_part;
                if ((short)MainForm->fonts->TextWidth(text) <= max_width)
                {
                    text_lines->Add(text);
                    text = "";
                }
                else
                {
                    char line_cut = false;
                    for (int i = 1; i <= text.Length(); i++)
                    {
                        if (text[i] == ' ' || i == text.Length())
                        {
                            String prefix = text.SubString(1, i);
                            if ((short)MainForm->fonts->TextWidth(prefix) >= max_width)
                            {
                                text_lines->Add(fitting_part);
                                text.Delete(1, fitting_part.Length());
                                line_cut = true;
                                break;
                            }
                            fitting_part = prefix;
                        }
                    }
```

- **What the function is for:** `MenuController::AddWrappedText` cuts a text into lines of at most 280 pixels at its
  spaces and adds them to the text window.
- **What the line was written to do:** Take a line off the front of the text on every pass, until the text is empty. The
  evidence: the loop runs `while (text.Length() > 0)`, and every other branch either adds the whole rest and empties
  `text` or deletes the part it just added.
- **What it does:** Step by step:
  - `fitting_part` starts empty on each pass and grows to the longest run of words, ending at a space, that fits.
  - When the first word alone is 280 pixels or wider, no run fits, so `fitting_part` is still empty when the cut is
    made.
  - The empty line is added, `text.Delete(1, 0)` takes nothing off, and the next pass finds the same word.
- **Evidence, from the code and not from the names:**
  - loop: `Ctrl_menu.cpp` `} while (text.Length() > 0);` the loop ends only when text is empty
  - width: `Ctrl_menu.cpp` `max_width = 280;` the widest line, in pixels
- **When it happens:** When one of the texts given to it holds a word, or a word with the space before it, of 280 pixels
  or more. The two callers fill the text window of the new account screen with strings 358, 359 and 360 of the string
  file.
  - `MenuController::AddWrappedText` is called from `Client::ShowPendingScreen`
    (`MainForm->animation->menu->AddWrappedText(MainForm->strings->GetText(GameText_ItIsVeryImportant), true);`);
    `Client::ShowPendingScreen`
    (`MainForm->animation->menu->AddWrappedText(MainForm->strings->GetText(GameText_LotOfPlayersWho), true);`);
    `Client::ShowPendingScreen`
    (`MainForm->animation->menu->AddWrappedText(MainForm->strings->GetText(GameText_YourInformationWillOnly), false);`);
    `Mouser::HandleMouseDown`
    (`MainForm->animation->menu->AddWrappedText(MainForm->strings->GetText(GameText_ItIsVeryImportant), true);`);
    `Mouser::HandleMouseDown`
    (`MainForm->animation->menu->AddWrappedText(MainForm->strings->GetText(GameText_LotOfPlayersWho), true);`);
    `Mouser::HandleMouseDown`
    (`MainForm->animation->menu->AddWrappedText(MainForm->strings->GetText(GameText_YourInformationWillOnly), false);`).
  - Reachable with the shipped code: unknown. It turns on the words of strings 358 to 360 in the string file, which the
    exe does not hold; one word of 280 pixels or more in the window's font (face 1, size 8) would reach it.
- **What you see:** The new account screen never opens: the client stays in the loop, adding an empty line to the text
  window on every pass, until it runs out of memory.

## A click on the bottom edge of the friend or ignore list acts on the name below the last one shown

`Ctrl_social.cpp`, `SocialController::OnMouseClick`

```cpp
        if (mouse_y > window_y + 43 && mouse_y < window_y + 243 && window_x + 17 < mouse_x && window_x + 252 > mouse_x)
        {
            is_dragging_window = false;
            int row = scroll_offset + (mouse_y - window_y - 47) / NPC_TEXT_LINE_HEIGHT;
            if (!list_kind)
            {
                if (friend_names->Count <= row)
                    row = -1;
                if (row >= 0 && button == 1)
                {
                    ToggleFriend(friend_names->Strings[row]);
```

- **What the function is for:** `SocialController::OnMouseClick` handles a click on the friend and ignore window. A left
  click on a friend puts "!name " in the chat box; a right click takes the name off its list.
- **What the line was written to do:** Act only on the 12 names the window shows. The evidence: `OnMouseMove` tests the
  same box, works out the same row and drops any row past 11 (`if (row > 11)`), and `GameGraphics::DrawSocialText` draws
  12 names from the scroll offset (`friend_top_line + 12 > i`).
- **What it does:** Step by step:
  - The list box runs from 44 to 242 pixels below the top of the window, and the rows are 16 pixels from 47.
  - A click 239 to 242 pixels down gives row 12, which no test drops.
  - With more names past the scroll offset than 12, row 12 is a real name that is not drawn, and the click acts on it.
- **Evidence, from the code and not from the names:**
  - hover: `Ctrl_social.cpp` `if (row > 11)` the hover of the same box stops at row 11
  - draw: `Gfx_game.cpp` `for (int i = friend_top_line; i < friend_line_count && friend_top_line + 12 > i; i++)` 12
    names are drawn
- **When it happens:** A click on the bottom 4 pixel lines of the list box while the open list has a name below the
  shown ones. `Mouser::HandleMouseDown` calls `OnMouseClick` for clicks while the window is open.
  - `SocialController::OnMouseClick` is called from `Mouser::HandleMouseDown`
    (`MainForm->animation->social->OnMouseClick(click_x, click_y, button);`).
  - Search of the tree for `row > 11`: 6 line(s), in Ctrl_guild.cpp, Ctrl_inn.cpp, Ctrl_law.cpp, Ctrl_learn.cpp,
    Ctrl_msgboard.cpp, Ctrl_social.cpp.
  - Reachable with the shipped code: reachable. Any list of 13 names or more, not scrolled to its end, and a click on
    the bottom edge of the box.
- **What you see:** With 20 friends at the top of the list, a right click on the bottom edge takes the 13th friend off
  the list, although only the first 12 are shown and none is lit under the mouse. On the ignore list it unignores the
  13th name.

## With more than 100 friends, a friend who is online can be shown as offline

`Ctrl_social.cpp`, `SocialController::AddOnlineName`

```cpp
void SocialController::AddOnlineName(String name)
{
    char is_listed = false;
    if (friend_names->Count > 100)
    {
        int index = friend_names->Count - 1;
        while (index >= 0)
        {
            if (friend_names->Strings[index] == name)
            {
                is_listed = true;
                break;
            }
            index--;
        }
    }
    if (!is_listed)
        online_names->Add(name);
}
```

- **What the function is for:** `SocialController::AddOnlineName` adds one name of the server's list of players online
  to `online_names`.
- **What the line was written to do:** Keep every online name, so that each friend who is online is drawn in the online
  colour. The evidence: `online_names` is read only by `IsOnline`, and its one caller, `GameGraphics::DrawSocialText`,
  asks it about each shown friend to pick the colour; a friend left out of the list is drawn as offline.
- **What it does:** Step by step:
  - With 100 friends or fewer, every name is added.
  - With more than 100, the friend list is searched for the name, letter for letter with case.
  - A name found there is left out of `online_names`; a name that matches no friend exactly is added.
- **Evidence, from the code and not from the names:**
  - draw: `Gfx_game.cpp` `for (int i = friend_top_line; i < friend_line_count && friend_top_line + 12 > i; i++)`
    each of the 12 shown friends is asked of IsOnline, which picks color_dialog_link_over or color_dialog_link
- **When it happens:** Each time the server sends the list of players online, `PacketProcessor::ParseOnlineNames` calls
  `AddOnlineName` for each name. The friend list has no limit in `AddFriend`, `ToggleFriend` or `LoadFriendList`.
  - `SocialController::AddOnlineName` is called from `PacketProcessor::ParseOnlineNames`
    (`MainForm->animation->social->AddOnlineName(online_name);`).
  - `SocialController::IsOnline` is called from `GameGraphics::DrawSocialText`
    (`if (MainForm->animation->social->IsOnline(MainForm->animation->social->friend_names->Strings[i]))`).
  - Search of the tree for `IsOnline\(`: 3 line(s), in Ctrl_social.cpp, Ctrl_social.h, Gfx_game.cpp.
  - Reachable with the shipped code: reachable. A friend list of 101 names or more, and a friend online whose name the
    server spells as the list does.
- **What you see:** With 101 friends, a friend saved as the server spells the name is drawn in the offline colour while
  online. A friend saved with other case, such as "Bob" for "bob", still shows as online, because `IsOnline` ignores
  case.

## Every tune that starts loads a new segment and keeps the last one loaded until the client ends

`Midiplayer.cpp`, `Dx_MidiPlayer::LoadSegment`

```cpp
            memset(&object_desc, 0, sizeof(object_desc));
            object_desc.dwSize = 848;
            MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, file_name, -1, source, MAX_PATH);
            object_desc.guidClass = CLSID_DirectMusicSegment;
            wcscpy(object_desc.wszFileName, source);
            object_desc.dwValidData = 18;
            if (music_loader->GetObject(&object_desc, IID_IDirectMusicSegment,
```

- **What the function is for:** `Dx_MidiPlayer::LoadSegment` loads the MIDI file of a tune as a DirectMusic segment and
  downloads its instruments to the performance, so that `PlaySegment` can play it.
- **What the line was written to do:** Hold one segment at a time and free it when it is done with. The evidence: `Play`
  stops the segment that plays before it loads the next one, and `ReleaseDirectMusic` frees the segment it holds with an
  unload (`GUID_Unload`) and `Release` before it sets `music_segment` to 0.
- **What it does:** Step by step:
  - `GetObject` writes the new segment into `music_segment` over the one already there.
  - The old segment gets no unload and no `Release`, so its instruments stay downloaded and its reference stays held.
  - The only unload and `Release` of `music_segment` are in `ReleaseDirectMusic`, which only the destructor calls, and
    no code deletes the player, so not even the last segment is freed.
- **Evidence, from the code and not from the names:**
  - free: `Midiplayer.cpp` `music_segment->SetParam(GUID_Unload, -1, 0, 0, music_performance);` the one place the
    segment is unloaded, with its Release on the next line
  - stop: `Midiplayer.cpp` `if (!music_performance->Stop((IDirectMusicSegment *)music_segment, 0, 0, 0))` Play stops
    the playing segment here before it loads the next
- **When it happens:** Each tune change: `SfxController::PlayMusic` calls `Play` for a map tune or a jukebox song, from
  `TMainForm::Timer1Timer` when a tune is queued and from `TMainForm::FormCreate` for the title tune when the intro is
  skipped.
  - `Dx_MidiPlayer::LoadSegment` is called from `Dx_MidiPlayer::Play` (`LoadSegment(file_path.c_str());`).
  - Search of the tree for `music_segment->Release\(\)`: 1 line(s), in Midiplayer.cpp.
  - Search of the tree for `LoadSegment\(`: 3 line(s), in Midiplayer.cpp, Midiplayer.h.
  - Search of the tree for `delete.*midi_player`: 0 line(s).
  - Reachable with the shipped code: reachable. Every tune after the first, with music on.
- **What you see:** The segment and the downloaded instruments of every tune played stay in memory until the client
  ends, so the client's memory grows with each new map tune or jukebox song in a session.

## Turning music on from the message box marks it working without checking that DirectMusic started

`Sys_mouse.cpp`, `Mouser::HandleMouseDown`

```cpp
void SfxController::CreateMidiPlayer()
{
    midi_player = new Dx_MidiPlayer();
}
```

- **What the function is for:** `Mouser::HandleMouseDown` handles the yes button of the message box that the music
  switch of the settings panel shows while music is marked unavailable (box kind 6, `ConfirmAction_EnableMusic`).
- **What the line was written to do:** Keep music off when the player could not start DirectMusic. The evidence: the
  `SfxController` constructor starts the player the same way and then tests `is_initialized`, turning music off and
  unavailable when it is 0.
- **What it does:** Step by step:
  - `InitMidiPlayer` calls `Initialize`, which sets `is_initialized` from `InitDirectMusic`.
  - The yes button does not test `is_initialized`; it sets the music volume to 100 and marks music available.
  - When the first `CoCreateInstance` of `InitDirectMusic` fails, the performance pointer `music_performance` is NULL.
  - `PlayMusic` tests `is_initialized` and plays nothing, but the next click on the music switch finds music available
    and calls `SfxController::SetMusicVolume`, which calls `Dx_MidiPlayer::SetVolume`, and that calls `SetGlobalParam`
    through `music_performance`.
- **Evidence, from the code and not from the names:**
  - ctor: `Sfxprotection.cpp` `MainForm->config->SetMusicAvailable(0);` the constructor marks music unavailable when
    the player did not start
  - box: `Sys_mouse.cpp` `MainForm->config->SetMusicAvailable(100);` the yes button marks music available whatever
    Initialize gave
  - switch: `Ctrl_settings.cpp` `MainForm->sfx->SetMusicVolume(0);` the music switch at volume 100 sets the player's
    volume to 0
- **When it happens:** Music is marked unavailable at start when setup.ini sets Music to 0 or when DirectMusic failed in
  the constructor. A click on the music switch then shows the box, and its yes button calls `InitMidiPlayer`.
  - Search of the tree for `is_initialized\)`: 9 line(s), in Midiplayer.cpp, Packetcontrols.cpp, Sfxprotection.cpp,
    Sys_mouse.cpp.
  - Reachable with the shipped code: reachable. A computer on which DirectMusic cannot be created, a yes to the box that
    offers music again, and one more click on the music switch.
- **What you see:** That click calls through a NULL interface pointer, which raises an access violation. On a computer
  where DirectMusic works, the box only starts the player and music plays.

## An npc's second animation (sheet frame 8) is never shown: no line sets the flag that starts it

`Gfx_npcs.cpp`, `NpcGraphics::DrawNpc`; `Sys_npcs.cpp`, `Npcs::Animate`

```cpp
    else if (npc.alt_attacking && npc.alt_attack_frame <= 2)
    {
            if (npc.direction == FacingDirection_Down || npc.direction == FacingDirection_Right)
        {
            if (mirrored)
                BlitNpcFrame(*(TRect *)(char *)this, graphic->resource->front_mirrored,
                    graphic->resource->GetFrameRect(8), 255);
    ...
        if ((*it)->alt_attack_start)
        {
            (*it)->alt_attack_start = 0;
            (*it)->alt_attack_direction = (*it)->direction;
            (*it)->alt_attack_frame = -1;
            (*it)->alt_attacking = true;
        }
        if ((*it)->alt_attacking)
        {
            if ((*it)->alt_attack_frame <= 2)
            {
                (*it)->alt_attack_frame++;
                if ((*it)->alt_attack_frame == 1)
                    MainForm->sfx->PlaySfx(SfxSound_NpcAltAttack);
```

- **What the function is for:** `NpcGraphics::DrawNpc` draws one npc with the frame of its current action;
  `Npcs::Animate` steps every npc's action counters once per tick.
- **What the line was written to do:** The second animation is written the same way as the attack. `Npcs::StartAttack`
  sets `attack_start`, `Animate` turns it into `attacking`, counts its frame and plays a sound on its first step, and
  `DrawNpc` draws the attack frames while `attacking` is set. For the second animation `Animate` turns
  `alt_attack_start` into `alt_attacking`, sets `alt_attack_frame` to -1, counts it and plays sound 11 on its first
  step, and `DrawNpc` draws frame 8 while `alt_attacking` is set. It has no starter like `StartAttack`.
- **What it does:** Step by step:
  - The `Npc` constructor sets `alt_attack_start` to 0.
  - The only other line that writes it is `Animate` clearing it.
  - No line of the tree sets it to 1 (search below), so `Animate` never sets `alt_attacking`, which stays 0 from the
    constructor on.
  - So the frame-8 branch `else if (npc.alt_attacking && npc.alt_attack_frame <= 2)` in `DrawNpc` is never entered, and
    `MainForm->sfx->PlaySfx(SfxSound_NpcAltAttack);` in `Animate` never runs.
- **When it happens:** `TMainForm::RenderGame_ViewPort_Frame` calls `DrawNpc` for every npc in view on each frame, and
  `TMainForm::HandleMMTimerEvent` calls `Animate` on each tick; the frame-8 branch of either needs `alt_attack_start`
  set first.
  - Search of the tree for `alt_attack_start = (1|true)\b`: 0 line(s).
  - Search of the tree for `alt_attack_start =[^=]`: 3 line(s), in Sys_npc.cpp, Sys_npcs.cpp.
  - Search of the tree for `PlaySfx\(11\)`: 0 line(s).
  - Reachable with the shipped code: never reached. The search for a store of 1 or true into `alt_attack_start` finds 0
    lines. The search for any store into it finds 3: the constructor's 0, the 0 of `AddNpc`, and `Animate`'s 0.
- **What you see:** No npc is ever drawn with frame 8 of its sheet, and sound 11 is never played:
  `PlaySfx(SfxSound_NpcAltAttack)` is on one line of the tree, the one in `Animate`.

## An npc's sheets hold 8 frames, but its frame-8 images are loaded at the right edge, so they are never kept

`Gfx_npc.cpp`, `NpcResources::CreateSheets`; `Gfx_npc.cpp`, `NpcResources::LoadFrontFrame`; `Sys_npcs.cpp`, `Npcs::LoadPendingGraphics`

```cpp
        frame_count = 8;
        size = *(TPoint *)MainForm->gfx_transfers;
        frame_width = size.x;
        frame_height = size.y;
        width = size.x * frame_count;
    ...
            MainForm->gfx_transfers->DrawAt(sheet, frame * frame_width, 0);
    ...
    frame_mirrored[frame] = true;
    ...
                    if ((unsigned char)(char)it->load_stage == 3)
                    {
    ...
                        if (!it->resource->LoadFrontFrame(8, open_library, resource_id + 16))
    ...
                        if (!it->resource->LoadBackFrame(8, open_library, resource_id + 17))
    ...
                        it->resource->MirrorFrame(8);
```

- **What the function is for:** `NpcResources::CreateSheets` makes an npc's front and back sheets, one row of frames
  side by side; `LoadFrontFrame` and `LoadBackFrame` draw one image of the graphics library into a frame slot of a
  sheet; `Npcs::LoadPendingGraphics` loads each npc's images in steps, one step per call.
- **What the line was written to do:** `LoadPendingGraphics` fills nine slots after the step that makes the sheets:
  frames 0 and 1, then 2 to 5, then 6, 7 and 8, from resources `resource_base + 16` and `+ 17` for frame 8.
  `NpcGraphics::DrawNpc` asks `GetFrameRect` for frame 8 in its second-animation block. So the sheets were meant to be
  wide enough for frame 8.
- **What it does:** Step by step:
  - `CreateSheets` sets `frame_width` to the width of the first image and makes the four sheets
    `frame_width * frame_count` wide with `frame_count` 8, so the slots are frames 0 to 7; `sheet_width` gets the same
    width.
  - `LoadFrame` draws at x `frame * frame_width`. For frame 8 that is `8 * frame_width`, the sheet's right edge, so the
    whole image falls outside the sheet.
  - The image is still loaded from the library each time: `LoadResourceBitmap` runs before the draw.
  - `MirrorFrame(8)` blits the empty strip past the edge and sets `frame_mirrored[8]`, one past the eight flags
    `frame_mirrored` holds; that byte is `sheets_created`, which is already true.
  - The only reads of an npc sheet's frame 8 are the four `GetFrameRect(8)` blits in `DrawNpc`: left `8 * frame_width`,
    right `9 * frame_width`, wholly past the sheet. The search below finds two more, of the player's own sheets
    (`GfxPlayerGetFrameRect(8)`).
- **When it happens:** `TMainForm::Timer1Timer` calls `LoadPendingGraphic` of the npc graphics on each tick. Every npc
  graphic starts unloaded, and each call takes the first unloaded graphic one step further (`load_stage`); the step with
  `load_stage == 3` runs the two frame-8 loads, while the library is open.
  - Search of the tree for `Load(Front|Back)Frame\(8,`: 2 line(s), in Gfx_mapnpcs.cpp.
  - Search of the tree for `GetFrameRect\(8\)`: 6 line(s), in Gfx_global_npcs.cpp, Gfx_global_sprites.cpp.
  - Reachable with the shipped code: reachable. The step runs once for every npc graphic that loads, with `resource_id`
    `40 * it->graphic_id + 61`.
- **What you see:** Each npc graphics load reads two images from the library and throws them away, and no npc sheet ever
  holds a frame-8 image. Nothing shows the loss, because the only draw of frame 8 is never reached (quirk 201).

## The start-up file check only tests that each file exists: every expected file size is empty

`Fileprotection.cpp`, `FileController::VerifyFiles`; `Fileprotection.cpp`,
`FileController::BuildFileList`

```cpp
    file_sizes->Add("");
...
            if (file_sizes->Strings[i] != "")
            {
                try
                {
                    FileHandle = open(lpFileName.c_str(), 0);
                    int file_length = filelength(FileHandle);
                    if (file_length != StrToInt(file_sizes->Strings[i]))
```

- **What the function is for:** `FileController` lists the program, its data, graphics, sound and music files with an
  expected size each; `VerifyFiles` runs at start-up and refuses to go on when a file is missing or has the wrong size.
- **What the line was written to do:** `VerifyFiles` opens each file whose expected size is not empty, compares
  `filelength` with it, and sets the result to 0 when they differ; its error text reads "Corrupt file". The size list
  was meant to hold sizes.
- **What it does:** 
  - `BuildFileList` adds an empty string to `file_sizes` for every file it lists, and these are the only stores into
    that list (searches below); nothing loads, assigns or changes its strings later.
  - So `if (file_sizes->Strings[i] != "")` is false for every file, and the open, `filelength` and `StrToInt` lines are
    not reached.
  - A file that exists passes whatever its size or content; only a missing file fails the check.
  - The program's own code agrees: the only calls on the size list are its adds in `BuildFileList`, the reads in
    `VerifyFiles` and the clear in the destructor, and the controller is used only from `FormCreate`.
- **When it happens:** Once, in `TMainForm::FormCreate`, before the game starts.
  - Search of the tree for `\bfile_sizes->Add\(""\)`: 7 line(s), in Fileprotection.cpp.
  - Search of the tree for `\bfile_sizes\b(?!->Add\(""\)|->Strings\[i\]|->Clear\(\)| = new TStringList\(\);|;)`: 0
    line(s).
  - Reachable with the shipped code: never reached. The search for any use of `file_sizes` that is not an empty `Add`, a
    read of `Strings[i]`, `Clear`, its creation or its delete finds 0 lines.
- **What you see:** A truncated or altered data, graphics, sound or music file passes the start-up check; the "Corrupt
  file" error is shown only for a file that is missing.

## The spot check tests the tile below the player twice and never the tile above, so a spot tile above starts nothing

`Ctrl_game.cpp`, `GameController::StartSpotCountdown`

```cpp
void GameController::StartSpotCountdown(int x, int y)
{
    if (MainForm->map->GetTileSpec(x, y) == TileSpec_CountdownSpot ||
        MainForm->map->GetTileSpec(x - 1, y) == TileSpec_CountdownSpot ||
        MainForm->map->GetTileSpec(x, y + 1) == TileSpec_CountdownSpot ||
        MainForm->map->GetTileSpec(x + 1, y) == TileSpec_CountdownSpot ||
        MainForm->map->GetTileSpec(x, y + 1) == TileSpec_CountdownSpot)
    {
        spot_countdown = 130;
        spot_x = x;
        spot_y = y;
    }
}
```

- **What the function is for:** `GameController::StartSpotCountdown` starts the spot countdown (`spot_countdown`,
  `spot_x`, `spot_y`) when the given tile or a tile next to it has the spot spec; `Tick` then warns the player and
  closes the connection if they are still on that tile when the count reaches its end.
- **What the line was written to do:** Test the given tile and its four neighbours. The evidence: the condition has five
  terms, the first is the tile itself and the next three are the tiles to the left (`x - 1, y`), below (`x, y + 1`) and
  to the right (`x + 1, y`), each written once; the fifth is the only place the tile above (`x, y - 1`) could be tested.
- **What it does:** Step by step:
  - The condition tests `GetTileSpec(x, y)`, `GetTileSpec(x - 1, y)`, `GetTileSpec(x, y + 1)`, `GetTileSpec(x + 1, y)`
    and then `GetTileSpec(x, y + 1)` again.
  - No term tests `GetTileSpec(x, y - 1)`. With a spot tile only above the player (for example the player at (10, 11)
    and the spot tile at (10, 10)), all five terms are false and `spot_countdown` stays 0.
- **When it happens:** `PacketProcessor::DispatchPacket` calls it for family 45 action 10, the arena notice that also
  prints the notice text, with the own character's tile (`own_spot_x`, `own_spot_y`). It is the only call.
  - `GameController::StartSpotCountdown` is called from `PacketProcessor::DispatchPacket`
    (`MainForm->animation->game->StartSpotCountdown(own_spot_x, own_spot_y);`).
  - Search of the tree for `GetTileSpec\(x, y \+ 1\) == TileSpec_CountdownSpot`: 2 line(s), in Ctrl_game.cpp.
  - Search of the tree for `GetTileSpec\(x, y - 1\)`: 0 line(s).
  - Reachable with the shipped code: reachable. The arena notice is a server packet and nothing guards the call; the
    condition depends only on the map tiles around the player.
- **What you see:** A player whose only neighbouring spot tile is the one above them gets no countdown: `Walk` does not
  refuse the step back onto their tile, and `Tick` neither warns them nor closes the connection. A player with the spot
  tile below them is tested twice, with the same result as once.

## On a scrolled spell or skill grid, every mouse move over an icon shows its name in the status bar again

`Ctrl_spells.cpp`, `SpellController::UpdateHover`; `Ctrl_skills.cpp`, `SkillController::UpdateHover`

```cpp
                if (col == it->grid_x && grid_row == it->grid_y && it->is_visible)
                {
                    found = true;
                    if (hover_col != it->grid_x || hover_row != it->grid_y)
                    {
    ...
                        hover_col = it->grid_x;
                        hover_row = it->grid_y - top_row;
    ...
                        if (hover_message_delay < 1)
                            MainForm->animation->interface_controller->ShowStatusMessage(message_type, hover_text);
```

- **What the function is for:** `SpellController::UpdateHover` and `SkillController::UpdateHover` track the icon under
  the mouse. When the mouse reaches a new icon they show its name in the status bar, mark the panel for redraw and
  measure the name.
- **What the line was written to do:** Run that branch only when the mouse reaches a different icon. The evidence: the
  test compares the stored hover cell with the icon's cell, and the branch stores the cell it compares against.
- **What it does:** Step by step:
  - The branch stores `hover_row = it->grid_y - top_row`, the shown row.
  - The test compares `hover_row` with `it->grid_y`, the grid row, which is larger by `top_row`.
  - With `top_row` above 0 the two never match, so the branch runs on every mouse move over an icon.
- **When it happens:** The grid is scrolled down, by the scroll arrows, the thumb or the wheel, and the mouse moves
  within an icon's cell. With the grid at its top, `top_row` is 0 and the test works.
  - `SpellController::UpdateHover` is called from `SpellController::MouseMove` (`UpdateHover(x, y);`);
    `SpellController::MouseDown` (`UpdateHover(x, y);`).
  - `SkillController::UpdateHover` is called from `SkillController::MouseMove` (`UpdateHover(x, y);`);
    `SkillController::MouseDown` (`UpdateHover(x, y);`).
  - Search of the tree for `hover_row != it->grid_y`: 1 line(s), in Ctrl_spells.cpp.
  - Search of the tree for `[^_]hover_row = it->grid_y - top_row`: 2 line(s), in Ctrl_skills.cpp, Ctrl_spells.cpp.
  - Reachable with the shipped code: reachable. `ScrollDown` raises `top_row` while it is below its limit, and
    `UpdateHover` runs on every mouse move.
- **What you see:** On a scrolled grid each mouse move over an icon calls `ShowStatusMessage` again, which replaces the
  status text and restarts its timer, so the hovered name pushes out any other status message; the panel is redrawn and
  the name measured again on every move.

## The skill panel is never filled: no code adds a skill to its grid, so it always shows empty

`Ctrl_skills.cpp`, `SkillController::SaveLayout`

```cpp
            slot_skill_id = 0;
            for (vector<SkillItem>::iterator it = skills.begin(); it != skills.end(); it++)
            {
    ...
            int slot = j + 8 * i;
            MainForm->registry->WriteSlotValue(3, slot, slot_skill_id);
```

- **What the function is for:** `SkillController` runs a second icon grid beside the spell grid: hover names, dragging
  and swapping icons, the training buttons, and `SaveLayout`, which writes the grid to registry store 3.
- **What the line was written to do:** Hold the known skills the way `SpellController` holds the known spells. The
  evidence: `SpellController` fills its `spells` grid with `AddSpell` and `RestoreLayout`, which reads store 2 back;
  `SkillController` has the same drag, swap and save code over `skills`, and the learn window and the forget box test
  `HasSkill` next to `HasSpell`.
- **What it does:** Step by step:
  - `SkillController` has no add or restore method; its only changes to `skills` are `erase` in `RemoveSkill` and
    `clear` in `Reset`.
  - No other code adds to `skills`, and nothing reads registry store 3.
  - So `skills` stays empty: `HasSkill` is always false, and the drag and training paths, which start from a click on an
    icon, never run.
- **Evidence, from the code and not from the names:**
  - spells: `Ctrl_spells.cpp` `MainForm->registry->OpenSlotKey(LayoutKey_Spells);` The spell grid is filled here, and
    the new spell written to store 2.
  - spells: `Ctrl_spells.cpp` `MainForm->registry->OpenSlotKey(LayoutKey_Spells);` The saved spell layout is read back
    from store 2.
  - skills: `Ctrl_skills.cpp` `MainForm->registry->OpenSlotKey(LayoutKey_Skills);` The skill layout is written, and
    nothing reads it back or adds to `skills`.
- **When it happens:** Always. Scan of every write: the tree has no insert into `skills`, and every other use of the
  panel's pointer is a call to its own methods or a read of its members.
  - Search of the tree for `[^_a-z]skills\.(insert|push_back)`: 0 line(s).
  - Search of the tree for `interface_controller->skills->TestAndClear`: 3 line(s), in TMainForm.cpp.
  - Search of the tree for `ReadSlotValue\(3`: 0 line(s).
  - Search of the tree for `WriteSlotValue\(3`: 0 line(s).
  - Reachable with the shipped code: reachable. The panel opens and draws like the spell panel; only its list is never
    filled.
- **What you see:** The skill panel always shows an empty grid. The learn window and the forget box still work, because
  each also asks `HasSpell` or counts the spell grid.

## On a scrolled skill list, the status bar names the skill from the unscrolled list, not the hovered one

`Ctrl_learn.cpp`, `LearnController::MouseMove`

```cpp
        if (page == LearnPage_SkillList)
        {
            row = (y - window_y - 50) / NPC_LIST_ROW_HEIGHT;
            if ((int)offered_skills.size() <= row || row > 4)
                row = -1;
        }
    ...
            if (row != -1 && page == LearnPage_SkillList)
            {
                SkillValue *skill = MainForm->skill_values->GetSkill(offered_skills[row].skill_id - 1);
    ...
        if (window_x + 76 > x && hovered_row >= 0 && page == LearnPage_SkillList)
        {
            offered_index = hovered_row + GetTopLine();
            skill_id = MainForm->animation->learn->offered_skills[offered_index].skill_id;
```

- **What the function is for:** `LearnController::MouseMove` tracks the hovered row of the skill master's list page.
  When the row changes it names the row's skill in the status bar with its requirements, and over the icon it opens the
  skill's tooltip.
- **What the line was written to do:** Name the skill under the mouse. The evidence: `LearnController::MouseDown` picks
  the clicked skill as `top_line + (y - window_y - 50) / NPC_LIST_ROW_HEIGHT`, the list draw starts from `GetTopLine()`,
  and the tooltip a few lines below in the same function reads `offered_skills[offered_index]` with
  `offered_index = hovered_row + GetTopLine()`. `ShopController::MouseMove` does the same for the shop lists, as
  `int index = GetTopRow() + row;`.
- **What it does:** Step by step:
  - `row` is the shown row, counted from the top of the window's list area.
  - The status text reads `offered_skills[row]`, with no `top_line` added.
  - With the list scrolled, that is the skill that many rows above the hovered one; the tooltip, which adds the top
    line, shows the hovered skill.
- **Evidence, from the code and not from the names:**
  - click: `Ctrl_learn.cpp` `int row = top_line + (y - window_y - 50) / NPC_LIST_ROW_HEIGHT;` The click adds the
    scroll position.
- **When it happens:** The list of skills on offer has more than five entries and is scrolled down, and the mouse moves
  onto a row.
  - `LearnController::MouseMove` is called from `Mouser::HandleMove` (`MainForm->animation->learn->MouseMove(x, y);`).
  - Search of the tree for `offered_skills\[row\]`: 65 line(s), in Ctrl_learn.cpp.
  - Search of the tree for `hovered_row \+ GetTopLine\(\)`: 1 line(s), in Ctrl_learn.cpp.
  - Reachable with the shipped code: reachable. The list scrolls once it holds more than five entries, and the status
    text is set on every row change.
- **What you see:** The highlight, the tooltip and the click all use the hovered skill, but the status bar names a
  different one, with that skill's level, stat, gold and class requirements.

## In four npc windows a click on a row also grabs the window, so dragging from a row moves it

`Ctrl_guild.cpp`, `GuildController::MouseDown`; `Ctrl_inn.cpp`, `InnController::MouseDown`; `Ctrl_law.cpp`, `LawController::MouseDown`; `Ctrl_locker.cpp`,
`LockerController::MouseDown`; `Ctrl_barber.cpp`, `BarberController::MouseDown`

```cpp
    char row_handled = false;
    if (button == 0 || button == 1)
    {
        if (window_y + 43 < y && window_y + 243 > y && window_x + 17 < x && window_x + 252 > x)
        {
            is_dragging = false;
    ...
    if (!row_handled)
    {
    ...
    if (!handled)
    ...
        if (window_x <= x && window_x + 284 >= x && y >= window_y && window_y + 290 >= y && !thumb_grabbed)
        {
            is_dragging = true;
            drag_offset_x = x - window_x;
            drag_offset_y = y - window_y;
        }
```

- **What the function is for:** Each of these `MouseDown` functions first handles a click on a row of the window, then,
  under a "not handled" test, the scroll bar and the start of a window move.
- **What the line was written to do:** Skip the scroll bar and window-move block after a row took the click. The
  evidence: the block is guarded by a flag that starts at 0, and the windows built the same way set their flag when a
  row is clicked, as `ChestController::MouseDown` does with `handled = true` and `ShopController::MouseDown` in each row
  branch.
- **What it does:** Step by step:
  - `row_handled` in `GuildController::MouseDown` and `handled` in the inn, law, locker and barber versions start at 0 and
    are never set.
  - So after a row click the last test of the block still runs, and inside the window it sets `is_dragging`.
  - The row branches of all four windows clear `is_dragging` first; the block sets it again.
- **Evidence, from the code and not from the names:**
  - set: `Ctrl_chest.cpp` `handled = true;` The chest window sets its flag when a row takes the click.
- **When it happens:** A row of one of these windows is clicked and the mouse moves before the button is let go. The
  flags are locals of each function.
  - `GuildController::MouseDown` is called from `Mouser::HandleMouseDown`
    (`MainForm->animation->guild->MouseDown(click_x, click_y, button);`).
  - `InnController::MouseDown` is called from `Mouser::HandleMouseDown`
    (`MainForm->animation->inn->MouseDown(click_x, click_y, button);`).
  - `LawController::MouseDown` is called from `Mouser::HandleMouseDown`
    (`MainForm->animation->law->MouseDown(click_x, click_y, button);`).
  - `LockerController::MouseDown` is called from `Mouser::HandleMouseDown`
    (`MainForm->animation->locker->MouseDown(click_x, click_y, button);`).
  - Search of `InnController::MouseDown`, `LawController::MouseDown` and `LockerController::MouseDown` for
    `if \(!handled\)`: 3 line(s), Ctrl_inn.cpp, Ctrl_law.cpp, Ctrl_locker.cpp.
  - Search of `GuildController::MouseDown` for `if \(!handled\)`: 1 line(s), Ctrl_guild.cpp.
  - Search of `GuildController::MouseDown` for `\bhandled = (1|true)`: 0 line(s).
  - Search of `InnController::MouseDown` for `\bhandled = (1|true)`: 0 line(s).
  - Search of `LawController::MouseDown` for `\bhandled = (1|true)`: 0 line(s).
  - Search of `LockerController::MouseDown` for `\bhandled = (1|true)`: 0 line(s).
  - So `handled` is never set inside those four bodies, unlike in `ChestController::MouseDown`, which sets it when a row
    takes the click.
  - Search of `ChestController::MouseDown` for `\bhandled = (1|true)`: 1 line(s), Ctrl_chest.cpp.
  - Reachable with the shipped code: reachable. Every row click in these windows runs the block, and `MouseMove` moves
    the window while `is_dragging` is set.
- **What you see:** Pressing on a row and moving the mouse before letting go drags the whole window, and while it is
  dragged the row hover is not tracked; `MouseUp` ends the drag.

## Opening the locker, memo or law window can leave the learn, guild or inn window open under or over it

`Ctrl_locker.cpp`, `LockerController::Open`; `Ctrl_memo.cpp`, `MemoController::Open`;
`Ctrl_law.cpp`, `LawController::Open`

```cpp
    MainForm->animation->shop->is_visible = false;
    MainForm->animation->guild->is_visible = false;
    MainForm->animation->memo->is_visible = false;
    MainForm->animation->locker->is_visible = false;
    MainForm->animation->law->is_visible = false;
    MainForm->animation->inn->is_visible = false;
```

- **What the function is for:** The seven npc windows that share the shop graphics (shop, locker, law, learn, guild,
  memo, inn) each hide the others when they open.
- **What the line was written to do:** Hide the other six. The evidence: `ShopController::Open`,
  `LearnController::Open`, `GuildController::Open` and `InnController::Open` each clear `is_visible` on exactly the six
  other windows.
- **What it does:** Step by step:
  - `LockerController::Open` clears `locker` (itself) where the others clear `learn`, so the learn window stays.
  - `MemoController::Open` clears `memo` (itself) where the others clear `guild`, so a guild window stays.
  - `LawController::Open` clears five windows and leaves out `inn`, so an inn window stays.
- **Evidence, from the code and not from the names:**
  - full: `Ctrl_inn.cpp` `MainForm->animation->law->is_visible = false;` The inn window hides all six others.
  - short: `Ctrl_law.cpp` `MainForm->animation->memo->is_visible = false;` The law window's list ends here, without the
    inn.
- **When it happens:** The server's reply that opens the locker, a memo or the law window arrives while the learn, guild
  or inn window is open.
  - `LockerController::Open` is called from `PacketProcessor::DispatchPacket`
    (`MainForm->animation->locker->Open(locker_title, 0, locker_x, locker_y);`).
  - `LawController::Open` is called from `PacketProcessor::DispatchPacket`
    (`MainForm->animation->law->Open(law_request_id);`).
  - Reachable with the shipped code: unknown. While any of these windows is open, clicks go to the window code in
    `Mouser::HandleMouseDown` (`box_handled`), so the request for the second window has to leave before the first one
    opens, or the server has to send the reply on its own.
- **What you see:** Both windows stay visible. `TMainForm::RenderGame_ViewPort_Frame` draws memo, guild, inn, law,
  locker, shop and learn in that order, so the learn window covers the locker, the guild window covers the new memo, and
  the law window covers the inn window.

## A board post with a subject over 32 characters is sent with that subject's start as its body

`Ctrl_msgboard.cpp`, `MsgBoardController::SubmitPost`

```cpp
    if (subject.Length() > 32)
        body = subject.SubString(1, 32);
    if (body.Length() > 1200)
        body = body.SubString(1, 1200);
```

- **What the function is for:** `MsgBoardController::SubmitPost` cuts a new post to the board's limits and sends it: the
  subject to 32 characters and the body to 1200.
- **What the line was written to do:** Cut the subject to its first 32 characters. The evidence: the next test cuts the
  body with `body = body.SubString(1, 1200)`, and `CreatePostEdits` gives the subject edit the 32-character limit.
- **What it does:** Step by step:
  - When `subject` is longer than 32 characters, the cut piece is stored in `body`, not in `subject`.
  - The body the player wrote is lost, and the subject goes out uncut.
  - `SendBoardPost(board_id, subject, body)` sends the long subject with its own first 32 characters as the body.
- **Evidence, from the code and not from the names:**
  - subject: `Ctrl_msgboard.cpp` `MainForm->edits->SetMaxLength(POST_SUBJECT_EDIT, 32);` The subject edit holds at most
    32 characters.
  - body: `Ctrl_msgboard.cpp` `body = body.SubString(1, 1200);` The body is cut into itself.
- **When it happens:** A post whose subject is longer than 32 characters is sent. The store is
  `body = subject.SubString(1, 32)` in `SubmitPost`: the cut subject goes into `body`, the body argument.
  - `MsgBoardController::SubmitPost` is called from `Sys_mouse_HandleClick`
    (`MainForm->animation->message_board->SubmitPost(post_subject, post_message);`).
  - Reachable with the shipped code: unknown. The subject comes from edit 20, which `CreatePostEdits` limits with
    `SetMaxLength(POST_SUBJECT_EDIT, 32)`; a longer subject reaches this test only if the edit lets one through.
- **What you see:** The post goes to the server with the long subject and, as its body, the subject's first 32
  characters.

## The board opens a post only for clicks on a row's left part, and also for clicks above and below the list

`Ctrl_msgboard.cpp`, `MsgBoardController::MouseDown`

```cpp
        if (window_x <= x && window_x + 282 >= x && y >= window_y && y <= window_y + 290)
        {
            int row = top_line + (y - window_y - 47) / NPC_TEXT_LINE_HEIGHT;
            if (!view_mode)
            {
                if (posts.size() <= (unsigned int)row)
                    row = -1;
```

- **What the function is for:** On the post list `MsgBoardController::MouseDown` opens the post in the clicked row;
  other clicks go to the scroll bar or start moving the window.
- **What the line was written to do:** Open a post for a click on one of the twelve shown rows. The evidence:
  `MouseMove` tracks the hovered row only inside
  `y > window_y + 43 && y < window_y + 243 && x > window_x + 17 && x < window_x + 467`, limits it to rows 0 to 11, and
  the hover bar is drawn across that width.
- **What it does:** Step by step:
  - The click test uses the window's left part and full height: `x` up to `window_x + 282`, `y` from `window_y` to
    `window_y + 290`.
  - The row is `top_line + (y - window_y - 47) / 16`, with no limit to the twelve shown rows.
  - Above the list the division rounds toward zero, giving `top_line`, `top_line - 1` or `top_line - 2`; below it gives
    rows 12 to 15 past `top_line`.
- **Evidence, from the code and not from the names:**
  - hover: `Ctrl_msgboard.cpp` `if (row > 11)` The hovered row is one of the twelve shown rows.
  - click: `Ctrl_msgboard.cpp` `int row = top_line + (y - window_y - 47) / NPC_TEXT_LINE_HEIGHT;` The clicked row
    has no such limit.
- **When it happens:** The post list is shown and a click lands on the right part of a row, above the list, or below it.
  - `MsgBoardController::MouseDown` is called from `Mouser::HandleMouseDown`
    (`MainForm->animation->message_board->MouseDown(click_x, click_y, button);`).
  - Reachable with the shipped code: reachable. Every click on the post list goes through this test.
- **What you see:** A click on a highlighted row right of `window_x + 282` opens nothing and starts moving the window. A
  click near the window's top opens the first shown post, or on a scrolled list one of the two posts above it, and a
  click below the list opens a post that is not shown, when the board has one there.

## A click below the last line of a short help page reads past the end of the page's line list

`Ctrl_help.cpp`, `HelpController::MouseDown`

```cpp
            int line = (y - 350) / 14;
            if (line < 7)
            {
                line += top_line;
                if (link_targets->Strings[line] != "")
```

- **What the function is for:** `HelpController::MouseDown` opens the linked page when the clicked line has a link.
- **What the line was written to do:** Look up only lines the page has. The evidence: the group panel's click tests its
  row against the list size before it reads the entry, and the help panel's own scroll code works from `lines->Count`.
- **What it does:** Step by step:
  - The click gives one of the seven shown rows, and `line` adds `top_line`.
  - The code reads `link_targets->Strings[line]` with no test against `link_targets->Count`.
  - On a page with fewer than seven lines, a click under the text asks for a line that does not exist, and the string
    list raises its index error.
- **Evidence, from the code and not from the names:**
  - guarded: `Ctrl_group.cpp` `if (row >= 0 && (int)members.size() > row && members[row].removable)` The group panel
    tests the size first.
  - unguarded: `Ctrl_help.cpp` `if (line < 7)` The help panel tests only the shown row.
- **When it happens:** A help page shorter than the panel is shown and the empty part below its text is clicked.
  - `HelpController::MouseDown` is called from `Mouser::HandleMouseDown`
    (`MainForm->animation->interface_controller->help->MouseDown(click_x, click_y);`).
  - `Mouser::HandleMouseDown` is called from `TMainForm::FormMouseDown`
    (`mouse->HandleMouseDown(X, Y, MouseButton_Left);`); `TMainForm::FormMouseDown`
    (`mouse->HandleMouseDown(X, Y, MouseButton_Right);`).
  - Reachable with the shipped code: unknown. It needs a help page with fewer lines than the panel shows; the pages come
    from the help files, which are data outside the exe.
- **What you see:** The index error leaves `HelpController::MouseDown`, `Mouser::HandleMouseDown` and
  `TMainForm::FormMouseDown`, none of which catches it there, and reaches `ApplicationEvents1Exception`, which only
  copies the message. The click does nothing, no message is shown, and the rest of that click's handling in
  `Mouser::HandleMouseDown` is skipped.

## A help line that wraps into more than ten pieces is written past the end of its ten-piece array

`Ctrl_help.cpp`, `HelpController::AddText`

```cpp
        String wrapped_pieces[MAX_WRAP_PIECES];
    ...
                    for (int i = 0; i <= 12; i++)
                    {
    ...
                                    wrapped_pieces[i] = text.SubString(piece_start, j - piece_start + 1);
```

- **What the function is for:** `HelpController::AddText` wraps a long help line into pieces that fit the panel and adds
  each piece as a line.
- **What the line was written to do:** Keep every piece inside the piece array. The evidence: the array is declared as
  `String wrapped_pieces[10]`, and every piece is stored in `wrapped_pieces[i]` and read back from it.
- **What it does:** Step by step:
  - The wrap loop runs `i` from 0 to 12, one piece for each turn while text is left.
  - A line that needs eleven to thirteen pieces stores them in `wrapped_pieces[10]` to `wrapped_pieces[12]`, past the
    array.
  - The add loop then reads those pieces back from the same places.
- **Evidence, from the code and not from the names:**
  - array: `Ctrl_help.cpp` `String wrapped_pieces[10];` Ten pieces fit.
  - loop: `Ctrl_help.cpp` `for (int i = 0; i <= 12; i++)` Up to thirteen are made.
- **When it happens:** A help file holds a line so long that it wraps into more than ten pieces of the panel's width. In
  the exe the array is the last local of the frame (at ebp-0x2C, ten slots), so the eleventh piece lands on the next
  slot and the twelfth and thirteenth on the saved frame pointer and the return address.
  - `HelpController::AddText` is called from `HelpController::LoadPage`
    (`AddText(line, link_target, line_flag, hide_icon);`).
  - Reachable with the shipped code: unknown. It depends on the line lengths in the help files, which are data outside
    the exe.
- **What you see:** The extra pieces overwrite what lies after the array on the stack, including the function's saved
  frame pointer and return address for the twelfth and thirteenth.

## Holding the book's scroll arrow scrolls only one line, because nothing repeats it

`Ctrl_book.cpp`, `BookController::MouseDown`

```cpp
    if (x >= window_x + 190 && x <= window_x + 205 && y >= window_y + 35 && y <= window_y + 50)
    {
        scroll_up_held = true;
        scroll_repeat_delay = 4;
        ScrollUp();
    }
```

- **What the function is for:** A click on the book's scroll arrows scrolls one line and, as in the other windows, marks
  the arrow held with a short delay before it repeats.
- **What the line was written to do:** Repeat the scroll while the arrow is held. The evidence:
  `BookController::MouseDown` sets `scroll_up_held` or `scroll_down_held` and `scroll_repeat_delay = 4` the same way the
  other windows do, and their `Tick`, such as `ShopController::Tick`, counts the delay down and then calls `ScrollUp` or
  `ScrollDown`.
- **What it does:** Step by step:
  - `BookController` has no `Tick`, and no code reads `scroll_up_held`, `scroll_down_held` or `scroll_repeat_delay`.
  - So the click scrolls one line, and holding the button does nothing more.
- **Evidence, from the code and not from the names:**
  - repeat: `Ctrl_inn.cpp` `if (scroll_repeat_delay > 0)` The inn window counts the delay down and repeats a held
    arrow.
  - book: `Ctrl_book.cpp` `scroll_repeat_delay = 4;` The book marks the arrow held, and nothing reads it.
- **When it happens:** The book window is open and a scroll arrow is held down. Scan of every read: the tree has none;
  the three fields are written only by `MouseDown` and cleared by the constructor and `MouseUp`.
  - `BookController::MouseDown` is called from `Mouser::HandleMouseDown`
    (`MainForm->animation->book->MouseDown(click_x, click_y, button);`).
  - Search of the tree for `book->scroll_(up|down)_held|book->scroll_repeat_delay`: 0 line(s).
  - Reachable with the shipped code: reachable. Any book with more entries than fit has working arrows, and holding one
    is ordinary use.
- **What you see:** Each press scrolls one line; a long book is scrolled by clicking repeatedly or by dragging the
  thumb.

## A news item that wraps into more than ten lines is written past the end of its ten-piece array

`Ctrl_news.cpp`, `NewslistController::AddNewsItem`

```cpp
        String wrapped_pieces[MAX_WRAP_PIECES];
    ...
                    for (int i = 0; i <= 12; i++)
                    {
    ...
                                    wrapped_pieces[i] = text.SubString(piece_start, j - piece_start + 1);
```

- **What the function is for:** `NewslistController::AddNewsItem` wraps one news item into pieces that fit the news
  panel and adds each piece as a line.
- **What the line was written to do:** Keep every piece inside the piece array. The evidence: the array is declared as
  `String wrapped_pieces[10]`, and every piece is stored in `wrapped_pieces[i]` and read back from it.
- **What it does:** Step by step:
  - The wrap loop runs `i` from 0 to 12, one piece for each turn while text is left.
  - An item that needs eleven to thirteen pieces stores them in `wrapped_pieces[10]` to `wrapped_pieces[12]`, past the
    array.
  - The add loop then reads those pieces back from the same places.
- **Evidence, from the code and not from the names:**
  - array: `Ctrl_news.cpp` `String wrapped_pieces[10];` Ten pieces fit.
  - loop: `Ctrl_news.cpp` `for (int i = 0; i <= 12; i++)` Up to thirteen are made.
- **When it happens:** The server's welcome reply carries a news item so long that it wraps into more than ten pieces of
  the panel's width. In the exe the array is the last local of the frame (at ebp-0x2C, ten slots), so the eleventh piece
  lands on the next slot and the twelfth and thirteenth on the saved frame pointer and the return address.
  - `NewslistController::AddNewsItem` is called from `PacketProcessor::DispatchPacket`
    (`MainForm->animation->interface_controller->news->AddNewsItem(MainForm->net_client->NextToken());`).
  - Reachable with the shipped code: unknown. It depends on the length of the news items the server sends, which is
    outside the exe.
- **What you see:** The extra pieces overwrite what lies after the array on the stack, including the function's saved
  frame pointer and return address for the twelfth and thirteenth.

## A word too wide for the help window makes it add empty lines without end, so the client stops responding

`Ctrl_report.cpp`, `ReportController::AddText`

```cpp
            do
            {
                String fitted_text;
    ...
                            String prefix = text.SubString(1, i);
                            if ((short)MainForm->fonts->TextWidth(prefix) >= max_width)
                            {
                                lines->Add(fitted_text);
                                line_values->Add(right_text);
                                text.Delete(1, fitted_text.Length());
    ...
                            fitted_text = prefix;
    ...
            while (text.Length() > 0);
```

- **What the function is for:** `ReportController::AddText` wraps a text into lines that fit the window: it grows the
  fitted part one word at a time and cuts it off as a line when the next word would make it too wide.
- **What the line was written to do:** Move on through the text on every turn. The evidence: the loop ends only on
  `while (text.Length() > 0)`, and each turn that cuts a line removes that line from the front with
  `text.Delete(1, fitted_text.Length())`.
- **What it does:** Step by step:
  - `fitted_text` starts empty on every turn of the outer loop.
  - When the first word of what is left, with its space, is already as wide as `max_width`, the width check fires before
    `fitted_text` holds anything.
  - The empty `fitted_text` is added as a line, and `text.Delete(1, fitted_text.Length())` removes nothing.
  - The next turn finds the same text, so the loop keeps adding empty lines and never ends.
- **Evidence, from the code and not from the names:**
  - width: `Ctrl_report.cpp` `int max_width = 200;` Every line is cut to this width.
  - cut: `Ctrl_report.cpp` `text.Delete(1, fitted_text.Length());` The cut removes only the part that fitted before
    the next word, which is empty when the first word is already too wide.
- **When it happens:** A text given to `ReportController::AddText` has a word, or a run without spaces, at least as wide
  as the window's line width at the start of a line. Every call passes a text that `StringContainer::GetText` reads from
  a data file with `LoadFromFile`.
  - `ReportController::AddText` is called from `Mouser::OpenHelpPanel`
    (`MainForm->animation->report->AddText(MainForm->strings->GetText(GameText_ListWithHelpfullTopics), "", true);`);
    `Mouser::OpenHelpPanel`
    (`MainForm->animation->report->AddText(MainForm->strings->GetText(GameText_YouCanAlwaysAsk), "", true);`);
    `Mouser::OpenHelpPanel`
    (`MainForm->animation->report->AddText(MainForm->strings->GetText(GameText_IForgotMyPassword), "", false);`);
    `Mouser::OpenHelpPanel`
    (`MainForm->animation->report->AddText(MainForm->strings->GetText(GameText_IWantToReport), "", false);`);
    `Mouser::OpenHelpPanel`
    (`MainForm->animation->report->AddText(MainForm->strings->GetText(GameText_IWantToSpeak), "", false);`).
  - Search of the tree for `report->AddText\(`: 5 line(s), in Sys_mouse.cpp.
  - Reachable with the shipped code: unknown. It depends on the texts in the client's data file, which is outside the
    exe.
- **What you see:** Opening the help window then hangs the client, with both line lists growing by an empty line on
  every turn.

## With the soft keypad, the login OK and Cancel buttons take clicks 22 pixels left of where they are drawn

`Sys_mouse.cpp`, `Mouser::HandleMouseDown`

```cpp
                if (MainForm->animation->GetSubState() == MenuScreen_PasswordGrid && x >= 360 && x <= 450 && y >= 388 &&
                    y <= 417 &&
                    MainForm->animation->menu->login_enabled)
    ...
                if (x >= 452 && x <= 542 && y >= 394 && y <= 423)
                {
                    MainForm->edits->Clear();
                    MainForm->gfx->LoadMainMenuScreen(true);
                    MainForm->animation->SetSubState(MenuScreen_Main);
                }
```

- **What the function is for:** The login screen has two layouts. Without the soft keypad (sub-state 4) a name box and a
  password box sit above OK and Cancel; with it (sub-states 7 and 8) the name comes first and the password is picked on
  a grid.
- **What the line was written to do:** Take each click where its button is drawn. The evidence:
  `MenuGraphics::DrawConfirmButtons` draws the buttons of each layout in their own place, and `Mouser::HandleMove`
  lights each layout's buttons exactly where they are drawn; in the layout without the keypad, the click areas match the
  drawn buttons.
- **What it does:** Step by step:
  - Without the keypad the buttons are drawn at x 360 and 452; with it, at x 382 and 474, each 90 pixels wide.
  - `Mouser::HandleMove` uses the same two sets of areas to light them, so the light follows the drawn button.
  - `Mouser::HandleMouseDown` tests OK on the password grid (sub-state 8) at x 360 to 450, and Cancel at x 452 to 542 in
    sub-states 4, 7 and 8 alike: the areas of the layout without the keypad.
  - Sub-states 7 and 8 come only with the keypad: two of the three `SetSubState(MenuScreen_GridAccountName)` calls open
    the login screen in the
    branch taken when `GetSkipSoftpad` gives 0, the third goes back from the grid, and the grid opens only from 7.
- **When it happens:** Each click on the login screen with the keypad layout (the skip-keypad setting not set), which
  `TMainForm::FormMouseDown` hands to `Mouser::HandleMouseDown`. The searches below find the Cancel area twice (its
  light without the keypad and the click test), the OK area at x 360 three times (its light and the two click tests),
  and the two draws of each keypad-layout button.
  - `MenuGraphics::DrawConfirmButtons` is called from `TMainForm::RenderMainLoop_Menu`
    (`gfx->menu->DrawConfirmButtons();`).
  - `Mouser::HandleMove` is called from `TMainForm::HandleMMTimerEvent` (`mouse->HandleMove();`).
  - Search of the tree for `\*mouse >= 452 && \*mouse <= 542`: 0 line(s).
  - Search of the tree for `\*mouse >= 360 && \*mouse <= 450`: 0 line(s).
  - Search of the tree for `BltFast\(382, 391`: 2 line(s), in Gfx_menu.cpp.
  - Search of the tree for `BltFast\(474, 391`: 2 line(s), in Gfx_menu.cpp.
  - Search of the tree for `SetSubState\(MenuScreen_GridAccountName\)`: 3 line(s), in Packetcontrols.cpp,
    Packetproces.cpp, Sys_mouse.cpp.
  - Reachable with the shipped code: reachable. The keypad layout is the one used unless the skip-keypad setting is on.
- **What you see:** On the password grid a click on the right fifth of the lit OK button (x 452 to 471) cancels back to
  the main menu, the strip just left of OK (x 360 to 381) logs in, and the right end of Cancel does nothing.

## If another box has the focus as the chat lock runs out, the chat box stays hidden and closed to typing

`Sys_edits.cpp`, `Edits::TimerTick`; `Sys_edits.cpp`, `Edits::LockChatInput`

```cpp
    if (chat_locked)
    {
        lock_countdown--;
        if (lock_countdown < 0)
            chat_locked = false;
    }
    tick_redraw = true;
    for (vector<Edit>::iterator it = edits.begin(); it != edits.end(); it++)
    {
        if (it->focused)
        {
            if (chat_locked && !lock_countdown && it->id == EditId_Chat)
            {
                it->visible = true;
                it->editable = true;
            }
    ...
            it->editable = false;
            it->visible = false;
```

- **What the function is for:** When the server locks a player's chat, `Edits::LockChatInput` hides the chat box (edit
  11), makes it not editable and starts `lock_countdown`; `Edits::TimerTick` counts it down and ends the lock.
- **What the line was written to do:** Give the chat box back when the countdown ends. The evidence: the lock and the
  countdown start together, `chat_locked` is cleared when the countdown passes 0, and the one store that makes the chat
  box visible and editable again sits on the tick the countdown reaches 0.
- **What it does:** Step by step:
  - Each slow tick, `lock_countdown` goes down by one, and `chat_locked` is cleared once it is below 0.
  - Then the loop finds the first box that has the focus and stops there.
  - Only when that box is the chat box, and the countdown is exactly 0, are its visible and editable flags set back.
  - If an input box or an amount box has the focus on that one tick, nothing is given back; on the next tick
    `chat_locked` is cleared, so the test can not hold again.
- **When it happens:** The lock starts from `PacketProcessor::DispatchPacket` (family 18, action 16, when no lock is
  running), the only call of `LockChatInput`; `TMainForm::HandleMMTimerEvent` runs `Edits::TimerTick` every fiftieth
  tick. Opening an input box or an amount box adds its edit box, which takes the focus, and the chat box gets it back
  when the box closes. The write scan: in the tree, the chat box's editable and visible flags are stored only in
  `LockChatInput`, in this restore, through `SetEditable` and `SetVisible` (never called with 11), and by the `Edit`
  constructor when `AddEdit(11, ...)` makes the box as the game is entered. Over every function of the exe, no call of
  `SetEditable` or `SetVisible` passes 11, and `LockChatInput` has the one caller.
  - `Edits::LockChatInput` is called from `PacketProcessor::DispatchPacket` (`MainForm->edits->LockChatInput();`).
  - `Edits::TimerTick` is called from `TMainForm::HandleMMTimerEvent` (`edits->TimerTick();`).
  - Search of the tree for `(?<=->)editable =`: 3 line(s), in Sys_edits.cpp.
  - Search of the tree for `(?<=->)visible =`: 3 line(s), in Sys_edits.cpp.
  - Search of the tree for `SetEditable\(11`: 0 line(s).
  - Search of the tree for `SetVisible\(11`: 0 line(s).
  - Search of the tree for `AddEdit\(11`: 0 line(s).
  - Reachable with the shipped code: reachable. A locked player who has a drop, trade or other amount box, or an input
    box, open at the moment the lock runs out.
- **What you see:** The lock icon goes away, but the chat box stays hidden and takes no typing. It comes back only when
  the game is entered again and the chat box is made anew.

## Small ones with no effect

- **The ping shown in chat is a large negative number when midnight passes before the reply.** `Packetproces.cpp`,
  `PacketProcessor::DispatchPacket`.
  - Code:
    `TDateTime now = Now(); TTimeStamp sent_stamp; sent_stamp = DateTimeToTimeStamp(MainForm->animation->interface_controller->status->ping_sent_time); TTimeStamp now_stamp; now_stamp = DateTimeToTimeStamp(now); int round_trip_ms = now_stamp.Time - sent_stamp.Time;`
  - What it does: `DateTimeToTimeStamp` splits a time into the day number and `Time`, the milliseconds since midnight.
    `round_trip_ms` subtracts only the `Time` of `ping_sent_time` from the `Time` of now, so a reply after midnight to a
    request sent before it gives a negative difference.
  - When it happens: The ping command typed in the chat box sets `ping_sent_time` and sends `SendPingRequest`; the reply
    is family 35, action 241. Reachable with the shipped code: reachable. Any ping sent just before midnight whose reply
    arrives after it.
  - What you see: Sent at 23:59:59.900 and answered at 00:00:00.100, the chat line shows -86399800 ms instead of 200 ms.
- **A map item the server shows is sorted into place twice in a row.** `Packetproces.cpp`,
  `PacketProcessor::DispatchPacket`.
  - Code:
    `MainForm->map_items->AddProtectedItem(shown_item_uid, shown_item_id, shown_item_x, shown_item_y, shown_item_amount, PROTECTED_OWNER_NONE, 10); MainForm->map_items->SortByPosition();`
  - What it does: The handler adds the item with `AddProtectedItem`, then calls `SortByPosition` twice in a row. The
    second call sorts a list that is already in order.
  - When it happens: Each item reply of family 14, action 7. Reachable with the shipped code: reachable. It runs every
    time that reply arrives.
  - What you see: Nothing a player can see; the map's item list is sorted one more time.
- **Walking through a player who blocks the way passes that player's id to the request, which leaves it out.**
  `Packetoperations.cpp`, `PacketOperator::SendWalkGhost`.
  - Code:
    `void PacketOperator::SendWalkGhost(int direction, int x, int y, int blocking_player_id) { MainForm->net_client->CommitActionTime(); String packet = packet + MainForm->net_client->EncodeNumber(direction, 1); packet = packet + MainForm->net_client->EncodeNumber(MainForm->net_client->last_action_tick, 3); packet = packet + MainForm->net_client->EncodeNumber(x, 1); packet = packet + MainForm->net_client->EncodeNumber(y, 1); MainForm->net_client->SendPacket(AC_HIT, OP_MOVE, packet);`
  - What it does: `GameController::Tick` calls `SendWalkGhost` with `last_blocking_player_id` as the fourth argument,
    `blocking_player_id`. The function packs only the direction, `last_action_tick` and the tile, so the id goes
    nowhere.
  - When it happens: When a player stands in the way for more than 42 ticks and the player is not casting, attacking or
    sitting. Reachable with the shipped code: reachable. `GameController::Tick` makes the call each time that wait runs
    out.
  - What you see: Nothing a player can see; the request is the same as a step without the id.
- **A path step down or up tests the map edge of the other direction.** `Pathfinder.cpp`, `PathFinder::FollowPath`.
  - Code: ``
  - What it does: Step by step: - A step down (`next_y > player_y`) tests `GetCenterY() >= 0`, the top edge. - A step up
    (`next_y < player_y`) tests `GetCenterY() <= GetHeight()`, the bottom edge. - `GameController::Walk`, which walks
    with the arrow keys, tests the top edge for a step up (`GetCenterY() > 0`) and the bottom edge for a step down
    (`GetCenterY() < GetHeight()`).
  - When it happens: Every step of a path, which `TMainForm::HandleMMTimerEvent` takes through `FollowPath`. Called from
    `TMainForm::HandleMMTimerEvent`. The search for `GetCenterY\(\) >= 0` finds 1 line(s). Reachable with the shipped
    code: reachable. Every path step down or up.
  - What you see: Nothing a player can see: both tests pass while the view is on the map, and `FindPath` keeps every
    step of a path inside the map, so the step is still refused only by a blocked or taken tile.
- **The amount box has a title strip and a prompt for kind 3, but no caller opens the box with kind 3.** `Gfx_game.cpp`,
  `GameGraphics::DrawItemAmountText`; `TMainForm.cpp`, `TMainForm::RenderGame_ViewPort_Frame`.
  - Code:
    `int title_id = 57; if (MainForm->animation->game->amount_box_kind == AmountBoxKind_JunkItem || MainForm->animation->game->amount_box_kind == 3) title_id = title_id + MainForm->animation->game->amount_box_kind - 1; if (animation->game->amount_box_kind == AmountBoxKind_Amount) OffscreenSurface->BltFast(animation->game->amount_box_x + 11, animation->game->amount_box_y + 10, gfx->interface_graphics->control_sheet, &gfx->game->amount_title_rect, DDBLTFAST_WAIT);`
  - What it does: Step by step: - `OpenAmountBox` and `OpenAmountBoxAtTile` store their first argument in
    `amount_box_kind`; the search below finds no other store. - The seventeen call sites pass 1, 2, 4, 5, 6, 7, 8, 9, 10
    or 11 (calls below). - So `amount_box_kind == AmountBoxKind_Amount` never holds: the strip `junk_title_rect` and the
    third prompt text are not drawn, and the mouse handler `Mouser::HandleMouseDown` has no kind-3 branch of its own.
  - When it happens: The amount box opens from the inventory, bank, guild, shop and trade clicks listed below, and from
    dropping an item on a map tile (`OpenAmountBoxAtTile`). Called from `BankController::MouseDown`,
    `BankController::MouseDown`, `GuildController::MouseDown`, `InventoryController::MouseDown`,
    `InventoryController::MouseDown`, `ShopController::MouseDown`, `ShopController::MouseDown`,
    `Mouser::HandleMouseDown`, `Mouser::HandleMouseDown`, `Mouser::HandleMouseDown`, `Mouser::HandleMouseDown`,
    `Mouser::HandleMouseUp`, `Mouser::HandleMouseUp`, `Mouser::HandleMouseUp`, `Mouser::HandleMouseUp`,
    `Mouser::HandleMouseDown`, `Mouser::HandleMouseUp`. The search for `\bamount_box_kind =[^=]` finds 2 line(s). The
    search for `amount_box_kind == 3\b` finds 0 line(s). Reachable with the shipped code: never reached. Every call of
    `OpenAmountBox` and `OpenAmountBoxAtTile` passes a literal kind other than 3 (calls below), and `amount_box_kind` is
    stored on two lines only, both from that argument.
  - What you see: None visible. The kind-3 title strip of the control sheet and its prompt text never appear on screen.
- **The player's graphics load stage copied for BlitFrame is overwritten with 99, so the frame-buffer draw always
  runs.** `Gfx_global_sprites.cpp`, `SpriteGlobalGraphics::DrawPlayer`; `Gfx_global_sprites.cpp`,
  `SpriteGlobalGraphics::BlitFrame`.
  - Code:
    `if (player->graphics_pending) draw_progress = player->load_progress; else draw_progress = 99; draw_progress = 99; if (use_frame_buffer) { if (draw_progress > 1)`
  - What it does: Step by step: - `draw_progress = player->load_progress;` when the player's graphics are pending, else
    `draw_progress = 99;`. - The next line sets `draw_progress = 99;` in every case, so the first value is dropped. -
    `BlitFrame` then always finds `draw_progress > 1` true. - The value it would have kept is `load_progress`, which
    both `Player` constructors and `Players::ReloadGraphics` set to 0 and no line raises.
  - When it happens: Every player draw from `TMainForm::RenderGame_ViewPort_Frame` runs these lines. The search for
    `\bdraw_progress =` finds 3 line(s). The search for `\bload_progress =` finds 3 line(s). Reachable with the shipped
    code: reachable. `DrawPlayer` runs for every player in view, and the overwrite has no condition.
  - What you see: None visible: `draw_progress` is 99 on every draw. Had the first value been kept, a player whose
    graphics are still pending (`load_progress` 0) would skip the frame-buffer blit in `BlitFrame`.
- **The bitmap loaders' surface setup calls Restore on the surface only when the surface pointer is null.**
  `Gfx_tile.cpp`, `TileResource::LoadSurface`.
  - Code:
    `if (!surface) surface->Restore(); if (surface->Created) surface->Destroy(); surface->GetSurfaceDesc(surface_desc); surface->Create(surface_desc, MainForm->Dx_Draw1);`
  - What it does: Step by step: - `if (!surface)` calls `surface->Restore();` only when `surface` is null, so the call
    would go through a null pointer. - Every surface the loaders test is a member made by `new TDx_DrawSurface(NULL)` in
    a constructor (searches below), so the test is always false and `Restore` is never called.
  - When it happens: Every bitmap load of the map lists, the effects and the window graphics runs the test. The search
    for `if \(!(surface|background_surface|item_slot_surface|cell_sheet_surface|layer[012]_surface)\)` finds 1 line(s).
    The search for `(\b|->)(surface|background_surface|item_slot_surface|cell_sheet_surface|layer[012]_surface) =[^=]`
    finds 37 line(s). The search for
    `(\b|->)(surface|background_surface|item_slot_surface|cell_sheet_surface|layer[012]_surface) = (?!new TDx_DrawSurface\(NULL\);)`
    finds 5 line(s). Reachable with the shipped code: never reached. The search for any store to the surface members the
    loaders test (`surface`, `background_surface`, `item_slot_surface`, `cell_sheet_surface` and the three effect
    layers) finds 31 lines, and the search for one that is not `new TDx_DrawSurface(NULL)` finds 0.
  - What you see: None visible: no surface is restored here, lost or not.
- **A wall sheet exactly 120 pixels wide, or a tile sheet exactly 250, is flagged animated but drawn whole.**
  `Gfx_wall.cpp`, `WallResource::LoadSurface`; `Gfx_wall.cpp`, `Gfx_wall_GetFrameRect`;
  `Gfx_tile.cpp`, `TileResource::LoadSurface`; `Gfx_tile.cpp`, `Gfx_tile_GetFrameRect`.
  - Code:
    `if (MainForm->resource_bitmap->Width >= 120) if (self->sheet_width > 120) if (MainForm->resource_bitmap->Width >= 250) if (self->surface_rect.right > 250)`
  - What it does: - `LoadSurface` sets the flag at `Width >= 120` for walls and `>= 250` for tiles. - `GetFrameRect`
    cuts the sheet into quarters only at `> 120` and `> 250`. - At exactly that width the flag is set, so the map keeps
    redrawing for animation, but every frame is the whole sheet.
  - When it happens: Every wall and ground tile load and draw. Reachable with the shipped code: unknown. It depends on
    whether the wall or tile library holds a bitmap of exactly that width.
  - What you see: Such a sheet would be drawn whole, its four frames side by side, on every animation step.
- **An open-door wall graphic exactly one pixel tall counts as a failed load.** `Gfx_mapwalls.cpp`,
  `WallGraphics::LoadGraphicNow`.
  - Code:
    `TPoint size = it->resource->LoadSurface(module, it->graphic_id + 100); it->width = size.x; it->height = size.y; if (size.x != -1 && size.y != 1) { it->is_loaded = true; it->unfetched_frames = 0; }`
  - What it does: Step by step: - `LoadGraphicNow` loads one wall graphic at once, for the open door
    `MapController::OpenDoor` shows, and marks it loaded when `size.x != -1 && size.y != 1`. - A failed `LoadSurface`
    returns -1 for both width and height; the width is tested against -1, the height against 1. - `LoadPendingGraphic`,
    the queue's loader of the same list, tests only `size.x == -1`. - So a bitmap of height 1 is left unloaded here,
    though the queue's loader would keep it.
  - When it happens: Each door that opens, the first time its open graphic is drawn. Reachable with the shipped code:
    unknown. It depends on whether the wall library holds an open-door bitmap exactly one pixel tall.
  - What you see: Such a graphic would stay unloaded after the door opens, and the door would show no open wall until
    the queue loads it.
- **The game interface loads only if the first graphics file opened, whether or not the interface file did.**
  `Gfxprotection.cpp`, `GfxController::LoadGameGraphics`.
  - Code:
    `HMODULE interface_library = LoadLibraryA(GetGfxFileName(INTERFACE_GFX_FILE).c_str()); if (LibraryA) { int h2 = (int)interface_library;`
  - What it does: - The second half opens `interface_library` but tests `LibraryA`, the first file's handle, which it
    has already passed to `FreeLibrary`; the program's own code tests the first handle too. - If the interface file
    fails to open while the first one opened, every interface loader gets a null library handle, so its bitmap is looked
    for in the program's own file instead. - If the first file failed and the interface file opened, the interface
    bitmaps are not loaded and `interface_library` is left open.
  - When it happens: Once per login, from the welcome reply in `PacketProcessor::DispatchPacket`, after
    `CreateGameGraphics`. Reachable with the shipped code: unknown. It needs exactly one of the two graphics files to
    fail to open.
  - What you see: With a missing or unreadable file, the interface is drawn from whatever the loads left in its
    surfaces.
- **Screenshots check for the program's screen folder but are saved into a screen folder under the working folder.**
  `Gfx_interface.cpp`, `InterfaceGraphics::SaveScreenshot`; `Configuration.cpp`, `Config::Config`.
  - Code:
    `String screen_folder = MainForm->config->GetAppPath() + "/screen/"; String relative_folder = ".\\screen\\"; lp_file_name = relative_folder + "ENDL" + number_text + ".BMP"; if (access(lp_file_name.c_str(), 0)) String screen_path = app_path + "screen";`
  - What it does: Step by step: - The config sets `screen_folder_exists` from a folder beside the program
    (`app_path + "screen"`). - `SaveScreenshot` builds `screen_folder` from the program's path and does not use it; the
    free name is searched and the file saved under `relative_folder`, a path relative to the working folder.
  - When it happens: Each screenshot key press in game, from `KeyBoard::HandleGameKeys` and from the key test in
    `TMainForm::Timer1Timer`. Reachable with the shipped code: reachable. Starting the program from a shortcut or
    command line whose working folder is not the program's folder makes the two folders differ.
  - What you see: In that case the screenshot goes to a screen folder under the working folder; with none there, no
    screenshot is kept, while the screen folder beside the program stays empty.
- **A jumping player's health bar and damage number rise twice as far as the body.** `Gfx_global_sprites.cpp`,
  `SpriteGlobalGraphics::DrawPlayer`.
  - Code:
    `if (player->jump_frame == 0 || player->jump_frame == 2) { player_rect.top = player_rect.top - 14; player_rect.bottom = player_rect.bottom - 14; } if (player->jump_frame == 0 || player->jump_frame == 2) { jump_bar_y -= 14; jump_bar_y -= 14; } if (player->jump_frame == 1) { jump_bar_y -= 22; jump_bar_y -= 22; }`
  - What it does: Step by step: - The jump branch of `DrawPlayer` (`jumping`, step `jump_frame`) lifts the body frame by
    14 pixels on steps 0 and 2 and by 22 on step 1, one subtraction each from `player_rect.top` and
    `player_rect.bottom`. - The health bar and damage number positions repeat that lift as two subtractions from the
    same variable (`jump_bar_y -= 14;` twice, `jump_damage_y -= 22;` twice, and so on), so they rise 28 and 44 pixels. -
    Apart from the lift, the jump places the bar and number exactly as the walk branch does.
  - When it happens: A player jumps (`Players::StartJump`, called when a step lands on or leaves a jump tile) while that
    player's overhead health bar (`health_bar_shown`) or damage number (`damage_shown`) is showing. The search for
    `(jump_bar_y|jump_damage_y) -=` finds 8 line(s). Reachable with the shipped code: reachable. `Players::StartJump` is
    called from the arrow-key walk in `GameController::Walk`, from `PathFinder::FollowPath` and from the walk packet in
    `PacketProcessor::DispatchPacket`; the health bar and damage number show after any hit.
  - What you see: During the jump the bar and the number float 14 or 22 pixels higher above the body than usual, then
    drop back when the jump ends.
- **The help report window brightens its link lines by the guild window's hovered row, not its own.** `Gfx_game.cpp`,
  `GameGraphics::DrawReportText`.
  - Code:
    `if (line[1] == '*') { line.Delete(1, 1); if (MainForm->animation->guild->hovered_row == row) MainForm->fonts->color = 0xDCDCDC; else MainForm->fonts->color = 0xB4B4B4; MainForm->fonts->is_underlined = true;`
  - What it does: Step by step: - `DrawReportText` draws a line that starts with an asterisk as a link, underlined, and
    picks `0xDCDCDC` when `MainForm->animation->guild->hovered_row == row`, else `0xB4B4B4`. - The report window has a
    hovered line of its own, `report->hovered_line`, which `DrawReportWindow` reads for the row highlight bar; only the
    `ReportController` constructor stores it, as -1. - The same test with `guild->hovered_row` is the guild window's own
    link test in `DrawGuildText`, the function before it in the file.
  - When it happens: The help report window is open (`ReportController::Open`, from `Mouser::OpenHelpPanel`) and one of
    its lines is a link. The search for `guild->hovered_row == row` finds 2 line(s). The search for
    `report->hovered_line` finds 2 line(s). Reachable with the shipped code: unknown. The draw runs on every frame while
    the report window is open. Its lines are language texts added by `ReportController::AddText`; whether any starts
    with an asterisk depends on the text file the client reads, which is data outside the program.
    `ReportController::MouseDown` acts on up to three such link lines.
  - What you see: A hovered link in the help report window keeps the dim link colour. The link drawn bright, if any, is
    the one whose row equals the guild window's last hovered row (0 after `GuildController::ClearText`, -1 once the
    mouse leaves the guild list).
- **The experience window's per-hour rates divide whole minutes and whole hours, so they move in steps.**
  `Gfx_game.cpp`, `GameGraphics::DrawStatText`.
  - Code:
    `if (session_rate < 60) { if (session_rate > 0) { session_rate_scaled = (double)(60 / session_rate) * MainForm->animation->interface_controller->status->total_exp_gained; session_rate = session_rate_scaled; } } else { session_rate_hourly = (double)MainForm->animation->interface_controller->status->total_exp_gained / (session_rate / 60); session_rate = session_rate_hourly; }`
  - What it does: Step by step: - Under an hour the rate is the gain times 60 / minutes, a whole-number division taken
    before the conversion to double: 7 minutes gives 8, 25 minutes gives 2, and every length from 31 to 59 minutes gives
    1. - From an hour on the rate is the gain divided by minutes / 60, in whole hours: 60 to 119 minutes divide by 1,
    120 to 179 by 2. - Both rates work this way: the session rate over the minutes since login, and the all-play rate
    over `usage_minutes` plus the session. - Under an hour of all play the all-play rate scales the session gain
    `total_exp_gained`; from an hour on it divides the character's whole `experience`.
  - When it happens: Each draw of the experience window. The search for `60 / (session_rate|total_rate)\)` finds 2
    line(s). The search for `(session_rate|total_rate) / 60\)` finds 2 line(s). Reachable with the shipped code:
    reachable. `DrawStatText` runs on every frame while the experience window is open, and the session length grows
    through every band above as the player stays logged in.
  - What you see: The rates are off by up to the size of the dropped fraction and jump at band edges. After 45 minutes
    the session rate shows the session gain unscaled; after 90 minutes it shows the whole session gain as one hour's
    worth, and at 120 minutes it halves at once.
- **Players stay marked as loading graphics for good: the done check waits on a cell flag that is only set to 0.**
  `Sys_players.cpp`, `Players::LoadPendingGraphics`; `Sys_players.cpp`, `Players::ReloadGraphics`.
  - Code:
    `if (i->reload_requested && Application->Active) { i->FreeResources(); i->CreateResources(); i->reload_requested = false; i->graphics_pending = true; i->extra_sheets_loaded = false; if (i->graphics_pending && !i->reload_requested && (Application->Active || !MainForm->config->GetFullscreen())) i->redraw = 1; i->cast_sheets_loaded = true; loaded_count++; if (i->sheets_cleared && i->walk_sheets_loaded && i->walk_tail_sheets_loaded && i->stand_sheet_loaded && i->sit_sheets_loaded && i->attack_sheets_loaded && i->extra_sheets_loaded && i->cast_sheets_loaded && i->emote_sheets_loaded) { i->graphics_pending = false; i->redraw = true; } it->graphics_pending = true; it->extra_sheets_loaded = false;`
  - What it does: Step by step: - When a player is marked pending, the flags from `sheets_cleared` to
    `emote_sheets_loaded` are set to 0: in `LoadPendingGraphics` when a reload is asked for (`reload_requested`), and in
    `ReloadGraphics`. - Each group block sets its own flag to 1 when it draws. The player's byte `extra_sheets_loaded`
    (the member the header declares as `extra_sheets_loaded`) is only ever given 0, or copied from another player
    record. - The test at the end needs `extra_sheets_loaded` along with the other eight flags, so it is false for every
    player, and `graphics_pending = false` with the redraw after it is not reached. - So `graphics_pending` stays 1. On
    each call the function works out the six part numbers again for every player in the list (until four groups are
    drawn), and every group block finds its flag set and draws nothing.
  - When it happens: On every call of `LoadPendingGraphics`, which `TMainForm::Timer1Timer` makes on each tick of its
    timer, for every player that `AddPlayer` adds (it passes 1 to the `Player` constructor, which marks the player
    pending), and for every player `ReloadGraphics` marks after a change of looks. The write scan behind "only ever
    given 0": in the tree, the searches below find the two constructor lines and the two clears in this file, and no
    other line names the byte. Over every function of the exe, each store that reaches byte 22 of a record (byte, word
    or dword stores, ORs, copies and fills) was listed: inside the player code there are 21. Four give 0 (the two
    constructors, these two clears), and the remaining 17 copy byte 22 from another player record (the record copies of
    the list, of `SortByPosition` swaps, and of the local player that `AddPlayer`, `AddPreviewPlayer` and
    `AddCharacterSlots` insert). The code outside reaches players only through the `Players` functions, or by copies
    such as `GetPlayerById`. Called from `TMainForm::Timer1Timer`. The search for
    `(extra_sheets_loaded|extra_sheets_loaded) = 1` finds 0 line(s). The search for
    `(i|it)->extra_sheets_loaded = false;` finds 2 line(s). The search for `(?<![\w>.])extra_sheets_loaded = false;`
    finds 2 line(s). Reachable with the shipped code: never reached. The clearing line needs `extra_sheets_loaded` to be
    non-zero. The write scan in the when field finds every store to that byte giving 0 or a copy of the same byte of
    another player, so no player ever holds a non-zero value there.
  - What you see: Nothing a player sees changes: the groups are drawn as their actions need them either way, and the
    other blocks ask for their own redraws. The cost is the work of the loop on every timer tick for every player on the
    list, for as long as they stay on it.
- **Weapon 67 is listed in two swing-effect groups; the later entry wins, so it swings with the group 3 effect.**
  `Sys_weaponmap.cpp`, `WeaponMapper::WeaponMapper`.
  - Code:
    `weapon_effect_types[66] = 1; weapon_effect_types[67] = 2; weapon_effect_types[3] = 2; weapon_effect_types[1] = 3; weapon_effect_types[9] = 3; weapon_effect_types[65] = 3; weapon_effect_types[67] = 3;`
  - What it does: Step by step: - The table starts at 0 for every weapon, then each group's weapons are set: group 1,
    group 2, group 3 and on. - Weapon 67 is set to 2 as the first entry of the group 2 lines, and again to 3 among the
    group 3 lines. - The second store replaces the first, so the table holds 3 for weapon 67 and its group 2 entry has
    no effect.
  - When it happens: Every time a player swings weapon 67: `TMainForm::RenderGame_ViewPort_Frame` reads
    `weapon_effect_types[player.weapon]` for a weapon that `IsRangedWeapon` does not list, and draws that group's swing
    effect. The table is filled once, when the form builds its `WeaponMapper`; the search below finds the two stores and
    no other line that names entry 67. The search for `weapon_effect_types\[67\]` finds 2 line(s). The search for
    `weapon_effect_types\[` finds 41 line(s). Reachable with the shipped code: reachable. Any player attacking with
    weapon 67.
  - What you see: A player swinging weapon 67 shows the swing effect of group 3, the group of weapons 1, 9 and 65; the
    group 2 line for it does nothing.
- **Freeing the sound buffers frees buffer 4 for buffers 5 to 7, and on close buffer 1 for buffers 2 to 7.**
  `TMainForm.cpp`, `TMainForm::Dx_Sound1Destroy`; `TMainForm.cpp`, `TMainForm::FormDestroy`.
  - Code:
    `if (Dx_SoundBuffer7->Created) Dx_SoundBuffer4->Destroy(); if (Dx_SoundBuffer6->Created) Dx_SoundBuffer4->Destroy(); if (Dx_SoundBuffer5->Created) Dx_SoundBuffer4->Destroy(); if (Dx_SoundBuffer4->Created) Dx_SoundBuffer4->Destroy(); if (Dx_SoundBuffer1->Created) Dx_SoundBuffer1->Destroy(); if (Dx_SoundBuffer2->Created) Dx_SoundBuffer1->Destroy(); if (Dx_SoundBuffer3->Created) Dx_SoundBuffer1->Destroy();`
  - What it does: Step by step: - In `Dx_Sound1Destroy`, `Dx_SoundBuffer7->Created` is tested, and when it holds,
    `Dx_SoundBuffer4->Destroy()` runs; the same goes for buffers 6 and 5. Then buffers 4, 3, 2 and 1 are each tested and
    freed. - In `FormDestroy`, each of the seven tests, from buffer 1 to buffer 7, frees buffer 1. - So buffers 5, 6 and
    7 are left as they are when the device goes, and buffers 2 to 7 when the form closes; buffer 4's `Destroy` runs up
    to four times and buffer 1's up to seven, the later times on a buffer that is already freed. What a second `Destroy`
    does is up to the TDx library.
  - When it happens: `FormDestroy` runs once as the program closes. `Dx_Sound1Destroy` has no caller in the tree; the
    form file, which the tree does not hold, binds it to the sound device, which is freed in `FormDestroy`, on the ways
    out in `FormCreate` and `FormKeyDown`, and in `Mouser::HandleMouseDown` when sound is switched back on, which frees
    and makes the device again. The searches below count the frees of the device and of buffers 5 to 7. The search for
    `Dx_Sound1->Destroy\(\)` finds 8 line(s). The search for `Dx_SoundBuffer[567]->Destroy\(\)` finds 3 line(s).
    Reachable with the shipped code: unknown. `FormDestroy` runs on every close; the `Dx_Sound1Destroy` half depends on
    the form file binding it to the sound device's destroy event, and the tree holds no call to it.
  - What you see: Buffers that are still made when the device or the form goes stay marked as made: while the game runs,
    until `UpdateSounds` sees them stop and frees them; on close, until the program ends. Nothing a player hears changes
    on the way out.
- **The surface restores before each frame is shown run only when the surface pointer is null, so they restore
  nothing.** `TMainForm.cpp`, `TMainForm::RenderOffscreenToPrimary`.
  - Code:
    `if (!BackBuffer) BackBuffer->Restore(); BackBuffer->BltFast(0, 0, OffscreenSurface, &screen_rect, DDBLTFAST_WAIT); if (!PrimarySurface) PrimarySurface->Restore(); PrimarySurface->Flip(BackBuffer, DDFLIP_WAIT); if (PrimarySurface->ErrorValue == DDERR_SURFACELOST || OffscreenSurface->ErrorValue == DDERR_SURFACELOST || BackBuffer->ErrorValue == DDERR_SURFACELOST) Dx_Draw1->RestoreAllSurfaces(); if (!PrimarySurface) PrimarySurface->Restore(); if (!OffscreenSurface) OffscreenSurface->Restore();`
  - What it does: Step by step: - Before each blit or flip, `if (!PrimarySurface)`, `if (!BackBuffer)` and
    `if (!OffscreenSurface)` test the pointer to the surface object, not whether the surface is lost. - The pointers are
    form components and are not null, so the restore calls under them are skipped; a null pointer would call `Restore`
    through that null pointer. - The very next line uses the same pointer to blit or flip.
  - When it happens: On every frame shown. The form sets these pointers itself; the searches below find no line of the
    tree that assigns them, and `FormCreate` installs `OnIdle` only after testing `PrimarySurface->Created` and
    `OffscreenSurface->Created` through them. The search for `(PrimarySurface|BackBuffer|OffscreenSurface) =` finds 0
    line(s). The search for `if \(!(PrimarySurface|BackBuffer|OffscreenSurface)\)$` finds 5 line(s). Reachable with the
    shipped code: never reached. The restores need a null surface pointer, and no line sets these pointers; a lost
    surface is restored only by the `RestoreAllSurfaces` test after the frame is shown.
  - What you see: A lost surface makes one frame's blit or flip fail, and the test after it restores the surfaces for
    the next frame; the lines before the draw add nothing.
- **On the password grid, 2, 4, 6 and 8 do nothing, while letters and other digits open the grid's message box.**
  `Sys_keyboard.cpp`, `KeyBoard::HandleMenuKeys`.
  - Code:
    `if ((MainForm->DIKeyboardState1->Keys[DIK_0] & 0x80) != 0 || (MainForm->DIKeyboardState1->Keys[DIK_1] & 0x80) != 0 || (MainForm->DIKeyboardState1->Keys[DIK_3] & 0x80) != 0 || (MainForm->DIKeyboardState1->Keys[DIK_5] & 0x80) != 0 ||`
  - What it does: Step by step: - The test reads the key state of the digit keys 0, 1, 3, 5, 7 and 9 (key codes 11, 2,
    4, 6, 8 and 10), then of the 26 letter keys. - When any of them is down, the box opens with
    `GetText(GameText_Information)` and `GetText(GameText_UseYourArrowkeysAnd)`, with its picture, and the caret is
    turned off. - The keys for 2, 4, 6 and 8 (key codes 3, 5, 7 and 9) are not in the test, and the grid takes no typed
    text, so pressing one of them does nothing at all.
  - When it happens: Each keyboard read while the password grid is up and no message box is open:
    `TMainForm::HandleMMTimerEvent` calls `KeyBoard::PollKeyboard`, which calls `HandleMenuKeys`, and the grid is
    entered by Enter or a click on the account name screen. The searches below find the one line that starts the test
    and the two places that open this box. Called from `KeyBoard::PollKeyboard`, `TMainForm::HandleMMTimerEvent`. The
    search for `Keys\[DIK_0\] & 0x80\) != 0 \|\|` finds 1 line(s). The search for `GetText\(384\)` finds 0 line(s).
    Reachable with the shipped code: reachable. Any player who types on the password grid.
  - What you see: A player who tries to type a password sees the box for most keys, but 2, 4, 6 and 8 give no answer, so
    the box does not come up for them.
- **On the account name screen, once a name is typed, a click anywhere moves on to the password grid.** `Sys_mouse.cpp`,
  `Mouser::HandleMouseDown`.
  - Code:
    `if (MainForm->animation->GetSubState() == MenuScreen_GridAccountName) { if (MainForm->edits->GetText(EditId_AccountName) != "") { MainForm->registry->SetAccountName(MainForm->edits->GetText(EditId_AccountName));`
  - What it does: Step by step: - For sub-states 4 and 8 the press is first tested against the OK button's area; for
    sub-state 7 only the name box is tested, for not being empty. - So any press on the screen opens the grid once a
    name is in the box, wherever it lands. - `TMainForm::FormMouseDown` focuses the edit box under the press before it
    calls `Mouser::HandleMouseDown`, so a press on the name box itself also moves on, with the name as it stands. - A
    press on Cancel first opens the grid and then, in the same call, goes back to the main menu.
  - When it happens: Each press on the account name screen with a name typed; `TMainForm::FormMouseDown` hands every
    press to `Mouser::HandleMouseDown`. The searches below find this test, and the Enter key's test in
    `KeyBoard::HandleEditKeys`, the only other way to the grid from the name screen. The search for
    `if \(MainForm->animation->GetSubState\(\) == 7\)` finds 0 line(s). The search for
    `MainForm->animation->GetSubState\(\) == 7\)` finds 0 line(s). Reachable with the shipped code: reachable. Any
    player who types a name and then clicks, for example on the name box to correct it.
  - What you see: A click meant to place the caret in the name box, or on empty space, opens the password grid with the
    name as typed so far.
- **On the password grid only the right third of the delete key lights up and takes clicks.** `Sys_mouse.cpp`,
  `Mouser::HandleMouseDown`; `Sys_mouse.cpp`, `Mouser::HandleMove`.
  - Code:
    `if (x >= 558 && x <= 562 && y >= 284 && y <= 298) { MainForm->sfx->PlaySfx(SfxSound_Send); MainForm->animation->menu->DeleteLastLetter(); } if (x >= 558 && x <= 562 && y >= 284 && y <= 298) MainForm->animation->menu->delete_key_lit = true;`
  - What it does: Step by step: - `MenuGraphics` draws the lit delete key at x 548, y 283 from `small_key_rect`, which
    is 15 pixels square, so the key covers x 548 to 562. - `Mouser::HandleMove` lights it, and `Mouser::HandleMouseDown`
    acts on it, only for x 558 to 562. - The other keys light and take clicks over the whole of their drawn area, for
    example the select key at x 514 to 562.
  - When it happens: Each move and click over the delete key on the password grid. The searches below find the two tests
    of the narrow area and the one draw of the key. The search for `x >= 558 && x <= 562` finds 2 line(s). The search
    for `BltFast\(548, 283` finds 1 line(s). Reachable with the shipped code: reachable. Any player who uses the mouse
    to remove a letter on the password grid.
  - What you see: Clicking the left two thirds of the delete key does nothing, and the key does not light there.
- **Leaving a guild sends the leave request with an empty guild tag, as the tag is cleared one line earlier.**
  `Sys_mouse.cpp`, `Mouser::HandleMouseDown`.
  - Code:
    `MainForm->animation->interface_controller->status->guild_tag = ""; MainForm->animation->interface_controller->status->guild_rank = GUILD_RANK_LOWEST; MainForm->animation->interface_controller->status->guild_name = ""; MainForm->animation->interface_controller->status->guild_rank_name = ""; MainForm->animation->interface_controller->status->MarkDirty(); MainForm->net_client->sender->LeaveGuild(MainForm->animation->guild->npc_session_id, MainForm->animation->interface_controller->status->guild_tag);`
  - What it does: Step by step: - `guild_tag`, the rank, `guild_name` and the rank name are cleared first. -
    `LeaveGuild` is then called with `status->guild_tag`, which is now empty. - `LeaveGuild` appends the tag after the
    four-character session value, so the request ends after the session.
  - When it happens: Each time a player confirms leaving a guild in the guild window, the only call of `LeaveGuild` in
    the tree (searches below). Called from `Mouser::HandleMouseDown`. The search for `LeaveGuild\(` finds 3 line(s). The
    search for `guild_tag = "";` finds 2 line(s). Reachable with the shipped code: reachable. Any guild member who
    leaves through the guild window.
  - What you see: The leave request reaches the server without the guild tag. What the server makes of that the client
    does not show; the client has already cleared its own guild details.
- **The world load's give-up step waits for a count of 150 that never comes: the count goes back to 0 at 12.**
  `Animationprotector.cpp`, `AnimationController::UpdateIntroAndDelays`.
  - Code:
    `if (reply_timeout >= 1 && menu->is_loading_world) { reply_timeout++; if (reply_timeout == 12) { menu->SetEnteringGame(true); menu->StartRequestDelay(); reply_timeout = 0; } if (reply_timeout == 150) { MainForm->gfx->FreeGameGraphics();`
  - What it does: Step by step: - `StartReplyTimeout` sets `reply_timeout` to 1 when the welcome reply starts the world
    load. - Each pass while `is_loading_world` holds adds 1 to it. - At 12 it starts entering the game
    (`SetEnteringGame(true)`, `StartRequestDelay`) and sets the count back to 0. - The test for 150 comes after that, in
    the same pass, and the count has gone no higher than 12 before it.
  - When it happens: `PacketProcessor::DispatchPacket` calls `StartReplyTimeout` on the welcome reply that starts the
    world load; `TMainForm::HandleMMTimerEvent` calls `UpdateIntroAndDelays` every 40th tick while the client animates.
    Called from `PacketProcessor::DispatchPacket`, `TMainForm::HandleMMTimerEvent`. The search for
    `\baccount_progress =[^=]` finds 5 line(s). The search for `\baccount_progress\+\+` finds 0 line(s). The search for
    `reply_timeout == 150` finds 1 line(s). Reachable with the shipped code: never reached. The search for any store to
    `reply_timeout` finds 5 lines: 1 in `StartReplyTimeout`, and 0 in the constructor, in `StopReplyTimeout` and in
    the two branches; the one other change is the step of 1, so the count never passes 12.
  - What you see: None visible from this line: the message box of dialog message 1 is never shown from here, and a world
    load is never stopped by this count.
- **With block transfer drawing, each transparent draw leaves a bitmap and a memory device context behind.**
  `Gfxtransfers.cpp`, `GfxTransfers::DrawAt`.
  - Code:
    `Graphics::TBitmap *buffer = new Graphics::TBitmap(); buffer->Width = bitmap->Width; buffer->Height = bitmap->Height; BitBlt(dc, x, y, buffer->Width, buffer->Height, buffer_dc, 0, 0, SRCCOPY); } }`
  - What it does: Step by step: - Each transparent draw makes a new `TBitmap` of the bitmap's size and a new memory
    device context, and selects the one into the other. - After the last `BitBlt` the function returns 1; the work
    bitmap is never deleted, and the device context is never passed to `DeleteDC`.
  - When it happens: Only while block transfer drawing is on: the `BLOCKTRANSFER` setting turns it on when the settings
    load, and `TMainForm::CountDrawError` turns it on after a third failed draw. The draws come from
    `NpcResource::LoadFrame`, `PlayerResources::DrawFrontLayer` and the other player layer draws, and
    `SpriteGlobalGraphics::DrawLayerToEmoteBuffer`, each after `SetTransparentColor`. Called from
    `SpriteGlobalGraphics::DrawLayerToEmoteBuffer`, `SpriteGlobalGraphics::DrawLayerToEmoteBufferPixelKeyed`,
    `NpcResource::LoadFrame`, `PlayerResources::DrawFrontLayer`, `PlayerResources::DrawBackLayer`,
    `PlayerResources::DrawFrontLayerPixelKeyed`, `PlayerResources::DrawBackLayerPixelKeyed`, `GfxTransfers::Draw`. The
    search for `\bbuffer = new Graphics::TBitmap\(\)` finds 1 line(s). The search for `delete buffer\b` finds 0 line(s).
    The search for `CreateCompatibleDC\(0\)` finds 2 line(s). The search for `DeleteDC\(` finds 1 line(s). Reachable
    with the shipped code: reachable. A player with `BLOCKTRANSFER` on, or whose machine fails three draws, as soon as
    npc, player or emote graphics load.
  - What you see: Nothing changes on screen. Each npc frame, player layer and emote drawn this way leaves one bitmap and
    one device context in the process, and they add up for as long as the client runs.
- **The npc frame loaders are declared to return a result but have no return statement.** `Gfx_npc.cpp`,
  `NpcResource::LoadFrontFrame`; `Gfx_npc.cpp`, `NpcResource::LoadBackFrame`.
  - Code:
    `char NpcResource::LoadFrontFrame(int frame, int library, int resource_id) { LoadFrame(front_sheet, frame, library, resource_id); } char NpcResource::LoadBackFrame(int frame, int library, int resource_id) { LoadFrame(back_sheet, frame, library, resource_id); }`
  - What it does: Step by step: - Each calls `LoadFrame` with its own sheet and ends without a `return`. - The compiled
    function ends right after the call, so the register a `char` result comes back in still holds `LoadFrame`'s answer
    when the caller reads it.
  - When it happens: `NpcGraphics::LoadPendingGraphic` loads an npc graphic stage by stage and
    `NpcGraphics::LoadGraphicNow` loads one at once; each frame load goes through these two. The search for
    `^char NpcResource::Load(Front|Back)Frame\(` finds 2 line(s). The search for `return LoadFrame\(` finds 0 line(s).
    Reachable with the shipped code: reachable. Every npc graphic loaded on a map runs them.
  - What you see: None visible: the callers get `LoadFrame`'s result after all, so a failed load is counted as it would
    be with the return written.
