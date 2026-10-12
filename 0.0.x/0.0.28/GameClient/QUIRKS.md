# GameClient: quirks

These are behaviours of the original client as it shipped. The source keeps every one of them byte for byte, because
changing any of them would change the exe. They are collected here, each with what the line was written to do and what
it does instead, so the comments in `src/` can stay about what the code is for.

## The class file download counts its parts from the spell file's record count

`Packetproces.cpp`, `PacketProcessor::DispatchPacket` (`0x004af778`)

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
                MainForm->class_values->unread_flag_at_1 = 0;
                if (MainForm->class_values->checksum_high != class_checksum_high ||
                    MainForm->class_values->checksum_low != class_checksum_low ||
                    MainForm->class_values->record_count != class_record_count)
                {
                    MainForm->class_values->received_part_count = 1;
                    MainForm->class_values->expected_part_count = skill_record_count / CLASS_RECORDS_PER_FILE + 1;
    ...
        if (action == AC_SYN5)
        {
            MainForm->class_values->received_part_count++;
            MainForm->class_values->SaveFilePart(DecodeNumber(String(packet[1])), packet.SubString(2, packet.Length()));
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
  - spell count: `Packetproces.cpp:167` `int skill_record_count = DecodeNumber(packet.SubString(34, 2));` the third
    value of the spell group (bytes 30, 32, 34), compared with the spell file's record_count
  - class count: `Packetproces.cpp:184` `int class_record_count = DecodeNumber(packet.SubString(40, 2));` the third
    value of the class group (bytes 36, 38, 40), compared with the class file's record_count and not used after that
  - class count: `Classvalues.cpp:72` `int last_file_number = total_records / CLASS_RECORDS_PER_FILE + 1;` the class
    file numbers its parts by its own record count, 250 records to a part
- **When it happens:** Each time a character enters the game while the class file on disk differs from the server's (a
  checksum word or the record count). The welcome reply (family 5, action 3) comes through `Client::ReceiveData`, the
  class parts (action 244) through `Client::HandleRawPacket`, and `TMainForm::OnIdle` sends the part requests with
  `RequestGameData`.
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
  - `PacketOperator::RequestGameData` is called from `TMainForm::OnIdle` (`net_client->sender->RequestGameData();`).
  - Search of the tree for `expected_part_count = skill_record_count`: 2 line(s), Packetproces.cpp:175,
    Packetproces.cpp:192.
  - Search of the tree for `/ CLASS_RECORDS_PER_FILE`: 2 line(s), Classvalues.cpp:72, Packetproces.cpp:192.
  - Reachable with the shipped code: unknown. It is decided by the server's files: the part count changes only when the
    spell file or the class file has 250 records or more, and the client cannot see those counts before the reply.
- **What you see:** With 250 spells or more and fewer classes, the client asks the server for more class parts than the
  class record count gives, and the class file stays marked as needed until a reply to the last one arrives. What the
  server sends for that part is outside the client; each reply is saved with `SaveFilePart` under the part number it
  carries.

## Guild reply 14 pairs the heading of the guild rank message (73) with the text of message 82

`Packetproces.cpp`, `PacketProcessor::DispatchPacket` (`0x004af778`)

```cpp
            if (guild_reply == GuildReply_RecruiterNotHere)
                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None, MainForm->strings->GetDialogText(82, false),
                    MainForm->strings->GetDialogText(82, true));
    ...
            if (guild_reply == GuildReply_Unknown14)
                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None, MainForm->strings->GetDialogText(73, false),
                    MainForm->strings->GetDialogText(82, true));
```

- **What the function is for:** `PacketProcessor::DispatchPacket` handles every packet from the server. Family 39,
  action 3 is the guild reply: one message box per reply number.
- **What the line was written to do:** Show one message whole, its title and its text. The evidence: the other replies
  of the group pass the same message number twice, as reply 10 does with (82, 0) and (82, 1), and the guild window's
  rank check shows message 73 with (73, 0) and (73, 1).
- **What it does:** Step by step:
  - `guild_reply` 14 calls `ShowMessageBox` with `GetDialogText(73, 0)` as the title and `GetDialogText(82, 1)` as the
    text.
  - The text of message 73, (73, 1), is shown by one line of the tree, in `GuildController::MouseDown`. The text of
    message 82 is also shown by reply 10, under its own title (82, 0).
- **Evidence, from the code and not from the names:**
  - message 73: `Ctrl_guild.cpp:319`
    `MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None, MainForm->strings->GetDialogText(73, false),` the
    rank check (guild_rank above 1) shows message 73 with its own text, (73, 1), on the next line
  - message 82: `Packetproces.cpp:2894`
    `MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None, MainForm->strings->GetDialogText(82, false),`
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
  - Search of the tree for `GetDialogText\(73, true\)`: 1 line(s), Ctrl_guild.cpp:320.
  - Search of the tree for `GetDialogText\(73, false\)`: 2 line(s), Ctrl_guild.cpp:319, Packetproces.cpp:2918.
  - Search of the tree for `GetDialogText\(82, true\)`: 2 line(s), Packetproces.cpp:2895, Packetproces.cpp:2919.
  - Reachable with the shipped code: unknown. The client shows the box for any guild reply 14; whether the server sends
    that reply is outside the client.
- **What you see:** The player sees a box headed with the title of message 73, the box the guild window shows a member
  of rank above 1 who opens a leader's page, and under it the text of message 82, the text reply 10 shows.

## A right click on a group chat line picks the line by the global tab's scroll position

`Ctrl_chat.cpp`, `ChatController::OnMouseClick` (`0x0049d40c`)

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
                MainForm->edits->SetText(11, "!" + group_senders->Strings[group_row] + " ");
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
  - group tab: `Ctrl_chat.cpp:695` `scroll_offsets[4] = group_lines->Count - 7;` the group tab's first shown line is
    kept in scroll_offsets[4]
  - global tab: `Ctrl_chat.cpp:938` `global_row = scroll_offsets[3] + (mouse_y - 333) / 14;` a click on the global tab
    (active_tab 4) reads scroll_offsets[3]
- **When it happens:** A right click on the text area of the chat panel while the group tab is open.
  `Sys_mouse_HandleClick` calls `OnMouseClick` for every click on the panel.
  - `ChatController::OnMouseClick` is called from `Sys_mouse_HandleClick`
    (`MainForm->animation->interface_controller->chat->OnMouseClick(x, y, button);`).
  - Search of the tree for `scroll_offsets\[3\]`: 7 line(s), Ctrl_chat.cpp:833, Ctrl_chat.cpp:835, Ctrl_chat.cpp:838,
    Ctrl_chat.cpp:840, Ctrl_chat.cpp:841, Ctrl_chat.cpp:938, Ctrl_chat.cpp:950.
  - Reachable with the shipped code: reachable. Any right click on a group line once the group tab and the global tab
    are scrolled to different lines.
- **What you see:** With 30 group lines (the tab shows the last 7, from line 23) and the global tab at line 0, a right
  click on the top row puts the sender of group line 0 in the chat box instead of the sender of line 23. When the global
  tab is further down than the group tab has lines, nothing is put in the box.

## A chat message that wraps to more than three lines writes the extra lines past the end of its line list

`Ctrl_chat.cpp`, `ChatController::AddLocalMessage` (`0x0049b8ac`); `Ctrl_chat.cpp`, `ChatController::AddPrivateMessage`
(`0x0049b038`); `Ctrl_chat.cpp`, `ChatController::AddGlobalMessage` (`0x0049caa0`); `Ctrl_chat.cpp`,
`ChatController::AddGroupMessage` (`0x0049c1a8`)

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
  six lines (`for (int i = 0; i <= 5; i++)`), counts each one in `line_count`, and the add loop reads
  `wrapped_lines[line_index]` for every counted line.
- **What it does:** Step by step:
  - `wrapped_lines` holds three strings in `AddLocalMessage`, `AddPrivateMessage` and `AddGlobalMessage`, and four in
    `AddGroupMessage`.
  - Each pass of the wrap loop stores line `i` in `wrapped_lines[i]`, for `i` up to 5. A C++ array does not check its
    index, so lines 4 to 6 (`i` 3 to 5) are stored in the memory after the list, and in `AddGroupMessage` lines 5 and 6.
  - The add loop then reads those same places back for the tab.
- **When it happens:** Every message these functions add whose text needs more than three wrapped lines (more than four
  for the group tab): about 1,260 pixels of text or more in MS Sans Serif 8. They are called for local, private, global
  and group chat and for most server notices.
  - Search of the tree for `AddLocalMessage\(`: 31 line(s), Ctrl_chat.cpp:466, Ctrl_chat.h:74, Packetproces.cpp:267,
    Packetproces.cpp:1726, Packetproces.cpp:1734, Packetproces.cpp:1742, Packetproces.cpp:2081, Packetproces.cpp:2108,
    ....
  - Search of the tree for `AddPrivateMessage\(`: 5 line(s), Ctrl_chat.cpp:349, Ctrl_chat.h:73, Packetproces.cpp:2306,
    Sys_keyboard.cpp:948, Sys_keyboard.cpp:991.
  - Search of the tree for `AddGlobalMessage\(`: 14 line(s), Ctrl_chat.cpp:717, Ctrl_chat.h:76, Packetproces.cpp:269,
    Packetproces.cpp:271, Packetproces.cpp:2228, Packetproces.cpp:2239, Packetproces.cpp:2264, Packetproces.cpp:2356,
    ....
  - Search of the tree for `AddGroupMessage\(`: 12 line(s), Ctrl_chat.cpp:593, Ctrl_chat.h:75, Packetproces.cpp:2326,
    Packetproces.cpp:2346, Packetproces.cpp:2370, Packetproces.cpp:2413, Packetproces.cpp:3977, Packetproces.cpp:3980,
    ....
  - Reachable with the shipped code: unknown. It is decided by the length of the text. Typed chat is limited to 128
    characters (edit 11, `SetMaxLength`); text that comes from the server, such as an npc's speech (family 26, action
    34), has no limit in the client.
- **What you see:** The fourth to sixth lines are written into memory the code does not define, next to the function's
  other local values, and the tab shows whatever is read back from there.

## The chat log counts as open even when its file could not be opened, and chat lines are then written to no file

`Filelogger.cpp`, `FFileLog::Open` (`0x0055bbd4`)

```cpp
void FFileLog::Open(String path)
{
    try
    {
        file = fopen(file_path.c_str(), "a");
        is_open = 1;
    }
    catch (...)
    {
        is_open = 0;
    }
```

- **What the function is for:** `FFileLog::Open` opens the chat log file so that `WriteLine` can add each chat line to
  it.
- **What the line was written to do:** Mark the log open only when the file opened. The evidence: the open is wrapped in
  `try` with `catch (...)` setting `is_open` to 0, and `WriteLine`, `Close` and the destructor all test `is_open` before
  they use `file`.
- **What it does:** Step by step:
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
  - Search of the tree for `new FFileLog\(`: 1 line(s), TMainForm.cpp:171.
  - Reachable with the shipped code: reachable. Any LogFile path whose folder does not exist, or a file the user may not
    write, with the log switched on.
- **What you see:** With "LogFile=C:\My Logs\chat.txt" the client opens "C:\MYLOGS\CHAT.TXT"; when that folder does not
  exist, the log counts as open and every chat line is handed to `fprintf` with no file, which the code does not define.

## The last item of the item file has no graphic, type, weight or other values

`Itemvalues.cpp`, `ItemValues::GetWeight` (`0x005310b4`); `Itemvalues.cpp`, `ItemValues::GetGender` (`0x005310f4`);
`Itemvalues.cpp`, `ItemValues::GetGraphic` (`0x00531134`); `Itemvalues.cpp`, `ItemValues::GetSize` (`0x00531170`);
`Itemvalues.cpp`, `ItemValues::GetSpecial` (`0x005311ac`); `Itemvalues.cpp`, `ItemValues::GetType` (`0x005311e8`);
`Itemvalues.cpp`, `ItemValues::GetSubType` (`0x00531224`); `Itemvalues.cpp`, `ItemValues::GetClassRequirement`
(`0x00531260`); `Itemvalues.cpp`, `ItemValues::GetSpecValue` (`0x0053129c`)

```cpp
int ItemValues::GetGraphic(int item_id)
{
    int graphic = -1;
    if (item_id > 0 && Count() > (unsigned int)item_id)
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
  - ids: `Itemvalues.cpp:186` `AddItem(Count() + 1, file_text.SubString(2, name_end - 1),` item n is records[n - 1]
  - key: `Itemvalues.cpp:424` `if (it->type == 9 && key_id == it->spec_value)` the key lookup searches every record
- **When it happens:** Every caller that asks for a value of the last item: the inventory, paperdoll, shop, chest,
  locker and map code call these getters. `UseItem` reads the type first.
  - Search of the tree for `Count\(\) > \(unsigned int\)item_id`: 9 line(s), Itemvalues.cpp:340, Itemvalues.cpp:353,
    Itemvalues.cpp:365, Itemvalues.cpp:373, Itemvalues.cpp:381, Itemvalues.cpp:389, Itemvalues.cpp:397,
    Itemvalues.cpp:405, ....
  - Search of the tree for `item_values->GetGraphic\(`: 28 line(s), Ctrl_inventory.cpp:852, Ctrl_inventory.cpp:915,
    Ctrl_inventory.cpp:1573, Ctrl_inventory.cpp:1584, Ctrl_inventory.cpp:1601, Ctrl_paperdoll.cpp:623,
    Ctrl_paperdoll.cpp:871, Ctrl_paperdoll.cpp:878, ....
  - Reachable with the shipped code: reachable. The item with the highest id in the server's item file is the last
    record of the list.
- **What you see:** With 500 items, item 500 is drawn with graphic -1, weighs 0, and `UseItem` gets type -1, which
  matches none of its item types, so using it does nothing; its name still shows, from `GetName`.

## An item's STR requirement is read one byte early, from the class field's last byte and its own first byte

`Itemvalues.cpp`, `ItemValues::Load` (`0x0052e798`)

```cpp
                        DecodeNumber(file_text.SubString(name_end + 38, 2)),
                        DecodeNumber(file_text.SubString(name_end + 40, 2)),
                        DecodeNumber(file_text.SubString(name_end + 41, 2)),
                        DecodeNumber(file_text.SubString(name_end + 44, 2)),
                        DecodeNumber(file_text.SubString(name_end + 46, 2)),
                        DecodeNumber(file_text.SubString(name_end + 48, 2)),
```

- **What the function is for:** `ItemValues::Load` cuts each item record of the item file into its fields and passes
  them to `AddItem`.
- **What the line was written to do:** Read the STR requirement from the two bytes after the class requirement, 42 and
  43, counted from the name. The evidence: the fields before it start at 38 and 40, and the ones after it at 44, 46, 48,
  50 and 52, each two bytes on from the last; byte 43 is the only byte of the record that no field reads.
- **What it does:** Step by step:
  - The class requirement is read from bytes 40 and 41.
  - The STR requirement is read from bytes 41 and 42.
  - `DecodeNumber` stops at a byte of 254; otherwise byte 41 gives the low digit and byte 42 the digit worth 253.
- **Evidence, from the code and not from the names:**
  - use: `Ctrl_inventory.cpp:235` `stat_requirements = stat_requirements + IntToStr(handle.required_str) + " STR ";` the
    field read from bytes 41 and 42 is shown as the STR requirement
  - decode: `Itemvalues.cpp:471` `value += NUMBER_ENCODING_BASE * digit;` the second byte is the digit worth 253
- **When it happens:** Every item, each time `RequestGameData` loads the files. `InventoryController::UseItem` reads the
  STR requirement when an item of type 10 to 21 is used, and refuses the item with a message when it is not met.
  - `ItemValues::Load` is called from `PacketOperator::RequestGameData` (`MainForm->item_values->Load();`).
  - Search of the tree for `name_end \+ 4[0-4],`: 3 line(s), Itemvalues.cpp:215, Itemvalues.cpp:216, Itemvalues.cpp:217.
  - Search of the tree for `required_str > MainForm`: 1 line(s), Ctrl_inventory.cpp:246.
  - Reachable with the shipped code: reachable. Every item of types 10 to 21 that is used from the inventory goes
    through the check.
- **What you see:** When the class byte 41 is 254, the STR requirement reads 0 and the client never refuses an item for
  STR. Otherwise it reads the class byte's digit plus 253 times the first STR byte's digit, so the message can name a
  STR requirement the item does not have.

## A word wider than the text window makes the client add empty lines without end

`Ctrl_menu.cpp`, `MenuController::AddWrappedText` (`0x0041b6d4`)

```cpp
            do
            {
                String fitting_part;
                if (MainForm->draw_canvas->TextWidth(text) <= max_width)
                {
                    text_lines->Add(text);
                    text = "";
                }
                else
                {
                    char line_cut = 0;
                    for (int i = 1; i <= text.Length(); i++)
                    {
                        if (text[i] != ' ' && i != text.Length())
                            continue;
                        String prefix = text.SubString(1, i);
                        if (MainForm->draw_canvas->TextWidth(prefix) >= max_width)
                        {
                            text_lines->Add(fitting_part);
                            text.Delete(1, fitting_part.Length());
                            line_cut = 1;
                            break;
                        }
                        fitting_part = prefix;
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
  - loop: `Ctrl_menu.cpp:212` `} while (text.Length() > 0);` the loop ends only when text is empty
  - width: `Ctrl_menu.cpp:173` `max_width = 280;` the widest line, in pixels
- **When it happens:** When one of the texts given to it holds a word, or a word with the space before it, of 280 pixels
  or more. The two callers fill the text window of the new account screen with strings 358, 359 and 360 of the string
  file.
  - `MenuController::AddWrappedText` is called from `Client::ShowPendingScreen`
    (`MainForm->animation->menu->AddWrappedText(MainForm->strings->GetText(358), true);`); `Client::ShowPendingScreen`
    (`MainForm->animation->menu->AddWrappedText(MainForm->strings->GetText(359), true);`); `Client::ShowPendingScreen`
    (`MainForm->animation->menu->AddWrappedText(MainForm->strings->GetText(360), false);`); `Sys_mouse_HandleClick`
    (`MainForm->animation->menu->AddWrappedText(MainForm->strings->GetText(358), true);`); `Sys_mouse_HandleClick`
    (`MainForm->animation->menu->AddWrappedText(MainForm->strings->GetText(359), true);`); `Sys_mouse_HandleClick`
    (`MainForm->animation->menu->AddWrappedText(MainForm->strings->GetText(360), false);`).
  - Reachable with the shipped code: unknown. It turns on the words of strings 358 to 360 in the string file, which the
    exe does not hold; one word of 280 pixels or more in MS Sans Serif 7 would reach it.
- **What you see:** The new account screen never opens: the client stays in the loop, adding an empty line to the text
  window on every pass, until it runs out of memory.

## A click on the bottom edge of the friend or ignore list acts on the name below the last one shown

`Ctrl_social.cpp`, `SocialController::OnMouseClick` (`0x0056a2dc`)

```cpp
        if (mouse_y > window_y + 43 && mouse_y < window_y + 243 && window_x + 17 < mouse_x && window_x + 252 > mouse_x)
        {
            is_dragging_window = 0;
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
  - hover: `Ctrl_social.cpp:563` `if (row > 11)` the hover of the same box stops at row 11
  - draw: `Gfx_game.cpp:3040` `for (int i = friend_top_line; i < friend_line_count && friend_top_line + 12 > i; i++)` 12
    names are drawn
- **When it happens:** A click on the bottom 4 pixel lines of the list box while the open list has a name below the
  shown ones. `Sys_mouse_HandleClick` calls `OnMouseClick` for clicks while the window is open.
  - `SocialController::OnMouseClick` is called from `Sys_mouse_HandleClick`
    (`MainForm->animation->social->OnMouseClick(x, y, button);`).
  - Search of the tree for `row > 11`: 6 line(s), Ctrl_guild.cpp:501, Ctrl_inn.cpp:310, Ctrl_law.cpp:278,
    Ctrl_learn.cpp:492, Ctrl_msgboard.cpp:383, Ctrl_social.cpp:563.
  - Reachable with the shipped code: reachable. Any list of 13 names or more, not scrolled to its end, and a click on
    the bottom edge of the box.
- **What you see:** With 20 friends at the top of the list, a right click on the bottom edge takes the 13th friend off
  the list, although only the first 12 are shown and none is lit under the mouse. On the ignore list it unignores the
  13th name.

## With more than 100 friends, a friend who is online can be shown as offline

`Ctrl_social.cpp`, `SocialController::AddOnlineName` (`0x005687cc`)

```cpp
void SocialController::AddOnlineName(String name)
{
    char is_listed = 0;
    if (friend_names->Count > 100)
    {
        int index = friend_names->Count - 1;
        while (index >= 0)
        {
            if (friend_names->Strings[index] == name)
            {
                is_listed = 1;
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
  - draw: `Gfx_game.cpp:3043` `MainForm->text_canvas->Font->Color = (TColor)0xF0F0F0;` the online colour, chosen when
    IsOnline is true
- **When it happens:** Each time the server sends the list of players online, `PacketProcessor::ParseOnlineNames`
  empties the list and calls `AddOnlineName` for each name. The friend list has no limit in `AddFriend`, `ToggleFriend`
  or `LoadFriendList`.
  - `SocialController::AddOnlineName` is called from `PacketProcessor::ParseOnlineNames`
    (`MainForm->animation->social->AddOnlineName(name);`).
  - `SocialController::IsOnline` is called from `GameGraphics::DrawSocialText`
    (`if (MainForm->animation->social->IsOnline(MainForm->animation->social->friend_names->Strings[i]))`).
  - Search of the tree for `IsOnline\(`: 3 line(s), Ctrl_social.cpp:275, Ctrl_social.h:51, Gfx_game.cpp:3042.
  - Reachable with the shipped code: reachable. A friend list of 101 names or more, and a friend online whose name the
    server spells as the list does.
- **What you see:** With 101 friends, a friend saved as the server spells the name is drawn in the offline colour while
  online. A friend saved with other case, such as "Bob" for "bob", still shows as online, because `IsOnline` ignores
  case.

## Every tune that starts loads a new segment and keeps the last one loaded until the client ends

`Midiplayer.cpp`, `Dx_MidiPlayer::LoadSegment` (`0x0049e03c`)

```cpp
    memset(&object_desc, 0, sizeof(object_desc));
    object_desc.dwSize = 848;
    MultiByteToWideChar(0, 1, file_name, -1, Source, 260);
    object_desc.guidClass = CLSID_DirectMusicSegment;
    wcscpy(object_desc.wszFileName, Source);
    object_desc.dwValidData = 18;
    if (g_6CB470->GetObject(&object_desc, IID_IDirectMusicSegment, (void **)&g_6CB46C) < 0)
        return false;
```

- **What the function is for:** `Dx_MidiPlayer::LoadSegment` loads the MIDI file of a tune as a DirectMusic segment and
  downloads its instruments to the performance, so that `PlaySegment` can play it.
- **What the line was written to do:** Hold one segment at a time and free it when it is done with. The evidence: `Play`
  stops the segment that plays before it loads the next one, and `ReleaseDirectMusic` frees the segment it holds with an
  unload (`GUID_Unload`) and `Release` before it sets `g_6CB46C` to 0.
- **What it does:** Step by step:
  - `GetObject` writes the new segment into `g_6CB46C` over the one already there.
  - The old segment gets no unload and no `Release`, so its instruments stay downloaded and its reference stays held.
  - The only unload and `Release` of `g_6CB46C` are in `ReleaseDirectMusic`, which only the destructor calls, and no
    code deletes the player, so not even the last segment is freed.
- **Evidence, from the code and not from the names:**
  - free: `Midiplayer.cpp:70` `g_6CB46C->SetParam(GUID_Unload, -1, 0, 0, g_658380);` the one place the segment is
    unloaded
  - stop: `Midiplayer.cpp:123` `if (g_658380->Stop(g_6CB46C, 0, 0, 0) < 0)` Play stops the playing segment here before
    it loads the next
- **When it happens:** Each tune change: `SfxController::PlayMusic` calls `Play` for a map tune or a jukebox song, from
  `TMainForm::OnIdle` when a tune is queued and from `TMainForm::FormCreate` for the title tune.
  - `Dx_MidiPlayer::LoadSegment` is called from `Dx_MidiPlayer::Play` (`LoadSegment(file_path.c_str());`).
  - Search of the tree for `g_6CB46C->Release\(\)`: 1 line(s), Midiplayer.cpp:71.
  - Search of the tree for `LoadSegment\(`: 3 line(s), Midiplayer.cpp:38, Midiplayer.cpp:89, Midiplayer.h:17.
  - Search of the tree for `delete.*midi_player`: 0 line(s).
  - Reachable with the shipped code: reachable. Every tune after the first, with music on.
- **What you see:** The segment and the downloaded instruments of every tune played stay in memory until the client
  ends, so the client's memory grows with each new map tune or jukebox song in a session.

## Turning music on from the message box marks it working without checking that DirectMusic started

`Sfxprotection.cpp`, `SfxController::CreateMidiPlayer` (`0x0041a6a0`)

```cpp
void SfxController::CreateMidiPlayer()
{
    midi_player = new Dx_MidiPlayer();
}
```

- **What the function is for:** `SfxController::CreateMidiPlayer` makes the music player when the player answers yes to
  the message box that the music switch of the settings panel shows while music is marked unavailable.
- **What the line was written to do:** Keep music off when the new player could not start DirectMusic. The evidence: the
  constructor makes the player the same way and then tests `is_initialized`, turning music off and unavailable when it
  is 0.
- **What it does:** Step by step:
  - A new `Dx_MidiPlayer` is made; its constructor sets `is_initialized` from `InitDirectMusic`.
  - `CreateMidiPlayer` does not test `is_initialized`, and its caller then turns music on and marks it available.
  - When the first `CoCreateInstance` of `InitDirectMusic` fails, the performance pointer `g_658380` is NULL.
  - The next tune reaches `PlayMusic`, which asks `IsSegmentPlaying`, and that calls `IsPlaying` through `g_658380`.
- **Evidence, from the code and not from the names:**
  - ctor: `Sfxprotection.cpp:24` `MainForm->config->SetMusicAvailable(0);` the constructor marks music unavailable when
    the player did not start
  - box: `Sys_mouse.cpp:798` `if (MainForm->animation->game->message_kind == ConfirmAction_EnableMusic)` the yes button
    of the music box
  - switch: `Ctrl_settings.cpp:91`
    `MainForm->animation->game->ShowMessageBox(3, ConfirmAction_EnableMusic, MainForm->strings->GetDialogText(40, false),`
    the music switch shows box 6 while music is unavailable
- **When it happens:** Music is marked unavailable at start when setup.ini turns it off or when DirectMusic failed in
  the constructor. A click on the music switch then shows message box kind 6, and its yes button in
  `Sys_mouse_HandleClick` calls `CreateMidiPlayer` and `SetMusicEnabled(1)`.
  - `SfxController::CreateMidiPlayer` is called from `Sys_mouse_HandleClick` (`MainForm->sfx->CreateMidiPlayer();`).
  - Search of the tree for `is_initialized\)`: 4 line(s), Packetcontrols.cpp:846, Sfxprotection.cpp:22,
    Sys_mouse.cpp:4149, Sys_mouse.cpp:4206.
  - Reachable with the shipped code: reachable. A computer on which DirectMusic cannot be created, and a yes to the box
    that offers music again.
- **What you see:** The next tune change calls through a NULL interface pointer, which raises an access violation in
  `TMainForm::OnIdle`. On a computer where DirectMusic works, the box only makes the player and music plays.

## An npc's second animation (sheet frame 8) is never shown: no line sets the flag that starts it

`Gfx_npcs.cpp`, `NpcGraphics::DrawNpc` (`0x00526518`); `Sys_npcs.cpp`, `Npcs::Animate` (`0x00521a98`)

```cpp
            if (npc.alt_attacking && no_state_drawn)
            {
                no_state_drawn = 0;
                if (npc.alt_attack_frame <= 2)
                {
                    if (!npc.direction || npc.direction == 3)
                        BlitFrame(dest_rect, npc.resources->front_sheet, Gfx_npc_GetFrameRect(npc.resources, 8), true);
...
        if (it->alt_attack_start)
            {
            it->alt_attack_start = false;
            it->alt_attack_direction = it->direction;
            it->alt_attack_frame = -1;
            it->alt_attacking = true;
                }
        if (it->alt_attacking)
                {
            if (it->alt_attack_frame <= 2)
                {
                it->alt_attack_frame++;
                if (it->alt_attack_frame == 1)
                    MainForm->sfx->PlaySfx(11);
```

- **What the function is for:** `DrawNpc` draws one npc with the frame of its current action; `Npcs::Animate` steps
  every npc's action counters once per tick.
- **What the line was written to do:** The second animation is written the same way as the attack. `Npcs::StartAttack`
  sets `attack_start` (`it->attack_start = 1`), `Animate` turns it into `attacking`, counts `attack_frame`
  (`it->attack_frame = -1`, then `it->attack_frame++`) and plays sound 10 on its first step, and `DrawNpc` draws the
  attack frames while `attacking` is set. For the second animation `Animate` turns `alt_attack_start` into
  `alt_attacking`, sets `alt_attack_frame` to -1, counts it and plays sound 11 on its first step, and `DrawNpc` draws
  frame 8 while `alt_attacking` is set. It has no starter like `StartAttack`.
- **What it does:** 
  - Both `Npc` constructors set `alt_attack_start` (the member at `alt_attack_start`) to 0.
  - The only other line that writes `alt_attack_start` is `Animate` clearing it; `Npcs::SortByPosition` and the list
    code move it only inside whole npc record copies.
  - No line of the tree sets it to 1 (search below), so `Animate` never reaches `it->alt_attacking = 1;` and
    `alt_attacking` stays 0 from the constructors on.
  - So the block `if (npc.alt_attacking && no_state_drawn)` in `DrawNpc` is never entered, and
    `MainForm->sfx->PlaySfx(11);` in `Animate` never runs.
- **Evidence, from the code and not from the names:**
  - attack start: `Sys_npcs.cpp:1393` `it->attack_start = 1;` The attack has its starter: `StartAttack` sets the flag
    `Animate` turns into the attack.
- **When it happens:** `TMainForm::OnIdle` calls `DrawNpc` for every npc in view on each frame (three calls below), and
  `Animate` runs on each tick; the frame-8 branch of either needs `alt_attack_start` set first.
  - `NpcGraphics::DrawNpc` is called from `TMainForm::OnIdle`
    (`gfx->npcs->DrawNpc(npc, 30 * npc.death_countdown - 60,`); `TMainForm::OnIdle`
    (`gfx->npcs->DrawNpc(npc, npc_alpha, shake_offset);`); `TMainForm::OnIdle`
    (`gfx->npcs->DrawNpc(npc, 255, shake_offset);`).
  - Search of the tree for `(alt_attack_start|alt_attack_start) = (1|true)\b`: 0 line(s).
  - Search of the tree for `(alt_attack_start|(it|it)->alt_attack_start) =[^=]`: 3 line(s), Sys_npc.cpp:26,
    Sys_npc.cpp:68, Sys_npcs.cpp:857.
  - Search of the tree for `PlaySfx\(11\)`: 1 line(s), Sys_npcs.cpp:868.
  - Reachable with the shipped code: never reached. The search for a store of 1 or true into `alt_attack_start` or
    `alt_attack_start` finds 0 lines. The search for any store into the npc's member finds 3: the two constructors' 0
    and `Animate`'s 0 (Sys_npcs.cpp:849). A scan of the program's machine code for every store to byte 76 of an npc
    record, with any value, finds the same set: the two constructors and `Animate` store 0; the sort, the list insert,
    the record getters and the vector copies copy the byte from another npc record; the npc add copies it from a record
    its constructor has just set to 0. No bit-OR, block copy from a packet or file, or store through another pointer
    reaches that byte. The program's own code of the functions whose source is not yet rebuilt exactly, and the lines
    kept as best-effort text, store byte 76 of an npc record only as `Animate`'s 0 or a copy from another npc record;
    the other byte-76 lines there fill tables of the npc list in its constructor.
- **What you see:** No npc is ever drawn with frame 8 of its sheet, and sound 11 is never played: `PlaySfx(11)` is on
  one line of the tree, the one in `Animate`.

## Balloon text is trimmed into `trimmed_text`, but the balloon lines are cut from the untrimmed `text`

`Gfx_npcs.cpp`, `NpcGraphics::ShowSpeechBalloon` (`0x00528128`); `Gfx_sprites.cpp`, `SpriteGraphics::ShowSpeechBalloon`
(`0x004995c0`)

```cpp
        String trimmed_text = text.Trim();
...
                    if (text.Length() < next_line_start)
                        continue;
                    int line_start;
                    line_start = next_line_start;
                    char seeking_break = 1;
                    for (int j = next_line_start; j <= text.Length() && seeking_break; j++)
...
                            balloon_lines[line_index] = text.SubString(line_start, j - line_start + 1);
```

- **What the function is for:** `ShowSpeechBalloon` (one for npcs, one for players) wraps a chat text into at most six
  balloon lines, measures them and hands them to the npc or player list.
- **What the line was written to do:** Both functions store `text.Trim()` in a local before the wrap starts, the way a
  value is kept for the code that follows; the wrap loop that follows is the only code that could use it.
- **What it does:** 
  - `trimmed_text` gets the text without its leading and trailing spaces and is not read again (search below: its two
    declarations are its only lines).
  - The wrap measures and cuts `text`: `text.Length()`, `text[j]` and `text.SubString(line_start, ...)`.
  - For a text of " hello " (spaces at both ends) the single balloon line is " hello ", and `text_width` is the
    `TextWidth` of that line, spaces included.
- **When it happens:** The npc version runs for npc speech in `PacketProcessor::DispatchPacket` (three calls below). The
  player version runs for chat from the server in `DispatchPacket`, for the player's own chat in
  `KeyBoard::PollKeyboard`, and once from `GameController::Tick` (ten calls below).
  - `NpcGraphics::ShowSpeechBalloon` is called from `PacketProcessor::DispatchPacket`
    (`MainForm->gfx->npcs->ShowSpeechBalloon(npc_message, talking_npc_index, 0);`); `PacketProcessor::DispatchPacket`
    (`MainForm->gfx->npcs->ShowSpeechBalloon(npc_speech, speaking_npc_index, 0);`); `PacketProcessor::DispatchPacket`
    (`MainForm->gfx->npcs->ShowSpeechBalloon(dialog_line, dialog_npc_index, 0);`).
  - `SpriteGraphics::ShowSpeechBalloon` is called from `GameController::Tick`
    (`MainForm->gfx->sprites->ShowSpeechBalloon(MainForm->strings->GetText(318),`); `PacketProcessor::DispatchPacket`
    (`MainForm->gfx->sprites->ShowSpeechBalloon(group_text, group_speaker_id, 1);`); `PacketProcessor::DispatchPacket`
    (`MainForm->gfx->sprites->ShowSpeechBalloon(broadcast_text, MainForm->net_client->GetPlayerId(), 0);`);
    `PacketProcessor::DispatchPacket` (`MainForm->gfx->sprites->ShowSpeechBalloon(talk_text, talker_id, 0);`);
    `KeyBoard::PollKeyboard` (`MainForm->gfx->sprites->ShowSpeechBalloon(public_censored,`); `KeyBoard::PollKeyboard`
    (`MainForm->gfx->sprites->ShowSpeechBalloon(non_admin_censored,`); `KeyBoard::PollKeyboard`
    (`MainForm->gfx->sprites->ShowSpeechBalloon(announce_censored,`); `KeyBoard::PollKeyboard`
    (`MainForm->gfx->sprites->ShowSpeechBalloon(non_announcer_censored,`); `KeyBoard::PollKeyboard`
    (`MainForm->gfx->sprites->ShowSpeechBalloon(guildless_censored,`); `KeyBoard::PollKeyboard`
    (`MainForm->gfx->sprites->ShowSpeechBalloon(party_censored,`).
  - Search of the tree for `\btrimmed_text\b`: 2 line(s), Gfx_npcs.cpp:610, Gfx_sprites.cpp:1769.
  - Reachable with the shipped code: reachable. Every caller passes a chat or text string, and nothing on these paths
    trims it first.
- **What you see:** A message with spaces at either end shows them in its balloon, and the balloon is wider by their
  width, because `text_width` sets the balloon size handed to `ShowBalloon`.

## A map change during graphic loading frees the graphics library but leaves `library_open` set

`Gfx_mapwalls.cpp`, `WallGraphics::ReopenLibrary` (`0x00518a6c`); `Gfx_mapwalls.cpp`, `WallGraphics::LoadPendingGraphic`
(`0x00518b3c`); `Gfx_maptiles.cpp`, `TileGraphics::ReopenLibrary` (`0x0043fab4`); `Gfx_mapobjects.cpp`,
`ObjectGraphics::ReopenLibrary` (`0x004f9040`); `Gfx_mapmasks.cpp`, `MaskGraphics::ReopenLibrary` (`0x00517884`);
`Gfx_maptops.cpp`, `TopGraphics::ReopenLibrary` (`0x00519800`); `Gfx_mapblends.cpp`, `BlendGraphics::ReopenLibrary`
(`0x0052a148`); `Gfx_mapitems.cpp`, `ItemGraphics::ReopenLibrary` (`0x0052c8d0`); `Gfx_mapicons.cpp`,
`IconGraphics::ReopenLibrary` (`0x006201c4`)

```cpp
    if (pending_count > 0 || keep_library_open)
    {
        FreeLibrary(library);
        if (!keep_library_open)
            pending_count = 0;
    }
...
    if (queue_drained)
{
        if (library_open && !keep_library_open)
    {
        FreeLibrary(library);
            library_open = 0;
    }
...
    if (pending_count > 0)
{
        if (!library_open && !keep_library_open)
    {
        library = LoadLibraryA(MainForm->strings->GetGfxPath(6).c_str());
            library_open = 1;
}
```

- **What the function is for:** Each map graphics list (walls, ground tiles, objects, masks, top layer, blends, items,
  icons) loads its graphics one per call from a library it opens on demand; `ReopenLibrary` runs on a map change or a
  logout.
- **What the line was written to do:** `LoadPendingGraphic` pairs every `FreeLibrary(library);` with `library_open = 0;`
  and opens the library again only when `library_open` is 0. `ReopenLibrary` frees the same handle when loads are
  pending, so it was written to leave the list in the closed state the next load expects.
- **What it does:** 
  - While loads are pending, `LoadPendingGraphic` has opened the library and set `library_open` to 1.
  - `ReopenLibrary` then calls `FreeLibrary(library);` and sets `pending_count` to 0 when the library is not kept open,
    but leaves `library_open` at 1 and `library` holding the freed handle.
  - The next `QueueGraphic` raises `pending_count` to 1. `LoadPendingGraphic` skips `LoadLibraryA` because
    `library_open` is 1, and passes the freed handle to the resource load as `open_library`.
  - The entries that were waiting when the library was freed stay in the list unloaded, and the queue count no longer
    counts them.
- **When it happens:** `ReopenLibrary` runs from `InterfaceController::Logout` and from two map packets in
  `PacketProcessor::DispatchPacket`: the map file reply and the warp to another map (calls below, for the walls; the
  other lists are called next to it). It frees the library only while that list has loads pending, so the map change has
  to arrive before the current map's graphics have finished loading. Logout releases every graphic first, so its pending
  count is already 0 there.
  - `WallGraphics::ReopenLibrary` is called from `InterfaceController::Logout`
    (`MainForm->gfx->walls->ReopenLibrary();`); `PacketProcessor::DispatchPacket`
    (`MainForm->gfx->walls->ReopenLibrary();`, 2 calls).
  - Reachable with the shipped code: reachable. A warp or map file reply that arrives while graphics of the old map are
    still queued reaches the `FreeLibrary(library);` line with `library_open` set; no code between the two clears it.
    The program's own code of the functions whose source is not yet rebuilt exactly, and the lines kept as best-effort
    text, hold no store to the byte that holds `library_open` in these lists.
- **What you see:** After such a map change, each new graphic of that list is loaded from a module handle that
  `FreeLibrary` has released. What `LoadFromResourceID` returns for a released module the code does not define; the
  entry is marked loaded only when the load reports a size, and `library_open` stays 1 until a load succeeds and drains
  the queue.

## An npc's sheets hold 8 frames, but its frame-8 images are loaded at the right edge, so they are never kept

`Gfx_npc.cpp`, `NpcResources::CreateSheets` (`0x00525d4c`); `Gfx_npc.cpp`, `NpcResources::LoadFrontFrame`
(`0x00525e94`); `Sys_npcs.cpp`, `Npcs::LoadPendingGraphics` (`0x00522704`)

```cpp
        int frame_count = 8;
...
        frame_width = MainForm->resource_bitmap->Width;
        frame_height = MainForm->resource_bitmap->Height;
        sheets_created = true;
        surface_desc->Width = frame_width * frame_count;
...
                MainForm->draw_canvas->Draw(frame * frame_width, 0, MainForm->resource_bitmap);
...
                if (i->load_stage == 2)
{
                    int attack_library = (int)graphics_library;
                    if (graphics_library)
    {
                        i->resources->LoadFrontFrame(6, attack_library, base_resource + 12);
                        i->resources->LoadBackFrame(6, attack_library, base_resource + 14);
                        i->resources->LoadFrontFrame(7, attack_library, base_resource + 13);
                        i->resources->LoadBackFrame(7, attack_library, base_resource + 15);
                        i->resources->LoadFrontFrame(8, attack_library, base_resource + 16);
                        i->resources->LoadBackFrame(8, attack_library, base_resource + 17);
```

- **What the function is for:** `CreateSheets` makes an npc's front and back sheets, one row of frames side by side;
  `LoadFrontFrame` and `LoadBackFrame` draw one image of the graphics library into a frame slot of a sheet;
  `Npcs::LoadPendingGraphics` loads each npc's images in steps, one step per call.
- **What the line was written to do:** `LoadPendingGraphics` fills nine slots: frames 0 and 1 in its first step, 2 to 5
  in its second, and 6, 7 and 8 in its third, from resources `base_resource + 16` and `+ 17` for frame 8. `DrawNpc` asks
  `Gfx_npc_GetFrameRect` for frame 8 in its second-animation block. So the sheets were meant to be wide enough for frame
  8.
- **What it does:** 
  - `CreateSheets` sets `frame_width` to the width of the first image and makes both sheets `frame_width * frame_count`
    wide with `frame_count` 8, so the slots are frames 0 to 7; `sheet_width` gets the same value. These are the only
    stores to either member of the sheet record (searches below; the first finds them by what they store, since other
    records' members are also named `sheet_width`); `DrawNpc` only reads `frame_width` (as
    `npc.resources->frame_width`).
  - `LoadFrontFrame` and `LoadBackFrame` draw at x `frame * frame_width`, by `draw_canvas->Draw` or by `BitBlt`. For
    frame 8 that is `8 * frame_width`, the sheet's right edge, so the whole image falls outside the surface and the
    surface's drawing context keeps none of it. Frames 0 to 7 are unchanged.
  - The image is still loaded from the library each time: `LoadFromResourceID` runs before the draw.
  - The only reads of frame 8 are the two `Gfx_npc_GetFrameRect(npc.resources, 8)` blits in `DrawNpc`: left
    `8 * frame_width`, right `9 * frame_width`, wholly past the sheet; `GetClippedSourceRect` trims them only against
    the view. That block needs `alt_attacking`, which quirk 201 shows is never set, so they never run. The other frame
    reads stay below 8: the attack frame is capped at 7, the walk frame is `walk_frame + 2` with `walk_frame` at most 3,
    the stand frame is 0 or 1.
  - The program's own code agrees: `CreateSheets` stores eight times the frame width as the surface width and the sheet
    width, and `LoadPendingGraphics` passes 8 to both frame loaders in its third step.
- **When it happens:** `TMainForm::Timer1Timer` calls `LoadPendingGraphics` on each tick. Every npc record starts with
  its graphics pending, and each call moves up to three pending npcs one step (`load_stage`); the step with
  `load_stage == 2`, the third call for that npc, runs the two frame-8 loads, while the library is open.
  - `NpcResources::CreateSheets` is called from `Npcs::LoadPendingGraphics`
    (`i->resources->CreateSheets((int)library_handle, base_resource, has_idle_animation[i->graphic - 1]);`).
  - `Npcs::LoadPendingGraphics` is called from `TMainForm::Timer1Timer` (`npcs->LoadPendingGraphics();`).
  - Search of the tree for `^\s*(sheet_width = frame_width|frame_width =[^=])`: 5 line(s), Gfx_npc.cpp:24,
    Gfx_npc.cpp:53, Gfx_npc.cpp:64, Sys_npc.cpp:40, Sys_npc.cpp:82.
  - Search of the tree for `resources->(field_0x14|frame_width) =[^=]`: 0 line(s).
  - Search of the tree for `Load(Front|Back)Frame\(8,`: 2 line(s), Sys_npcs.cpp:1130, Sys_npcs.cpp:1131.
  - Search of the tree for `Gfx_npc_GetFrameRect\(npc\.resources, 8\)`: 2 line(s), Gfx_npcs.cpp:227, Gfx_npcs.cpp:229.
  - Reachable with the shipped code: reachable. The third step runs once for every npc whose graphics load, with
    `base_resource` `40 * graphic + 61`.
- **What you see:** Each npc graphics load reads two images from the library and throws them away, and no npc sheet ever
  holds a frame-8 image. Nothing shows the loss, because the only draw of frame 8 is never reached (quirk 201).

## The start-up file check only tests that each file exists: every expected file size is empty

`Fileprotection.cpp`, `FileController::VerifyFiles` (`0x00414fc8`); `Fileprotection.cpp`,
`FileController::BuildFileList` (`0x004148e4`)

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
  - `FileController::VerifyFiles` is called from `TMainForm::FormCreate` (`if (files->VerifyFiles())`).
  - Search of the tree for `\bfile_sizes->Add\(""\)`: 16 line(s), Fileprotection.cpp:33, Fileprotection.cpp:35,
    Fileprotection.cpp:37, Fileprotection.cpp:39, Fileprotection.cpp:41, Fileprotection.cpp:43, Fileprotection.cpp:45,
    Fileprotection.cpp:47, ....
  - Search of the tree for `\bfile_sizes\b(?!->Add\(""\)|->Strings\[i\]|->Clear\(\)| = new TStringList\(\);|;)`: 0
    line(s).
  - Reachable with the shipped code: never reached. The search for any use of `file_sizes` that is not an empty `Add`, a
    read of `Strings[i]`, `Clear`, its creation or its delete finds 0 lines. The program's own code of the functions
    whose source is not yet rebuilt exactly, and the lines kept as best-effort text, hold no call of these functions and
    no use of the controller.
- **What you see:** A truncated or altered data, graphics, sound or music file passes the start-up check; the "Corrupt
  file" error is shown only for a file that is missing.

## The spot check tests the tile below the player twice and never the tile above, so a spot tile above starts nothing

`Ctrl_game.cpp`, `GameController::StartSpotCountdown` (`0x00493928`)

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
  prints the notice text, with the own character's tile (`own_spot_x`, `own_spot_y`). It is the only call. Open rows
  checked: `GameController::StartSpotCountdown` is not an open row and holds no best-effort line, so its text is the
  exe's.
  - `GameController::StartSpotCountdown` is called from `PacketProcessor::DispatchPacket`
    (`MainForm->animation->game->StartSpotCountdown(own_spot_x, own_spot_y);`).
  - Search of the tree for `GetTileSpec\(x, y \+ 1\) == TileSpec_CountdownSpot`: 2 line(s), Ctrl_game.cpp:1690,
    Ctrl_game.cpp:1692.
  - Search of the tree for `GetTileSpec\(x, y - 1\)`: 0 line(s).
  - Reachable with the shipped code: reachable. The arena notice is a server packet and nothing guards the call; the
    condition depends only on the map tiles around the player.
- **What you see:** A player whose only neighbouring spot tile is the one above them gets no countdown: `Walk` does not
  refuse the step back onto their tile, and `Tick` neither warns them nor closes the connection. A player with the spot
  tile below them is tested twice, with the same result as once.

## Cancelling a drag from the left type-21 paperdoll box draws its picture in the right box instead

`Ctrl_inventory.cpp`, `InventoryController::MouseDown` (`0x00535cfc`)

```cpp
        if (button == 1)
        {
            int item_type = MainForm->item_values->GetType(this->drag_item_id);
    ...
            if (item_type == EifItemType_Armlet)
            {
                if (!MainForm->animation->paperdoll->type20_right_graphic)
                    MainForm->animation->paperdoll->type20_right_graphic = paperdoll_slot_graphic;
                else
                    MainForm->animation->paperdoll->type20_left_graphic = paperdoll_slot_graphic;
            }
            if (item_type == EifItemType_Bracer)
                if (!MainForm->animation->paperdoll->type21_right_graphic)
                    MainForm->animation->paperdoll->type21_right_graphic = paperdoll_slot_graphic;
                else
                    MainForm->animation->paperdoll->type21_right_graphic = paperdoll_slot_graphic;
```

- **What the function is for:** `InventoryController::MouseDown` handles a click while an item taken from the paperdoll
  is dragged; a click with any button but the left one (`button` 1) cancels the drag and puts the item's picture back
  into its box.
- **What the line was written to do:** Put the picture back into the box it came from. The evidence:
  `PaperdollController::MouseDown` sets the graphic of the box the item leaves to 0 (`type21_left_graphic = 0` for the
  left box, `type21_right_graphic = 0` for the right one), so a right graphic of 0 means the item came from the right
  box. For types 19 and 20 the `else` branch writes `type19_left_graphic` and `type20_left_graphic`.
- **What it does:** Step by step:
  - For an item of type 21, `if (!type21_right_graphic)` picks the branch; both branches write
    `type21_right_graphic = paperdoll_slot_graphic`.
  - When the item came from the left box, `type21_left_graphic` stays 0 and the left item's picture is written over
    `type21_right_graphic`, whether the right box held another item or was empty.
- **Evidence, from the code and not from the names:**
  - left: `Ctrl_paperdoll.cpp:541` `type21_left_graphic = 0;` The left box's graphic is set to 0 when its item is picked
    up, and the drag keeps the old value in paperdoll_slot_graphic.
  - left: `Ctrl_inventory.cpp:999` `MainForm->animation->paperdoll->type20_left_graphic = paperdoll_slot_graphic;` The
    type-20 cancel puts the picture back into the left box in its else branch.
  - right: `Ctrl_paperdoll.cpp:563` `type21_right_graphic = 0;` The right box's graphic is set to 0 when its item is
    picked up, the case the if branch tests.
- **When it happens:** A player drags the item out of the left type-21 box of their own paperdoll
  (`PaperdollController::MouseDown` starts the drag with `dragging_from_paperdoll = 1`) and then clicks with the right
  mouse button. `Sys_mouse_HandleClick` passes `button` from `TMainForm::FormMouseDown`, 0 for the left button and 1
  otherwise.
  - `InventoryController::MouseDown` is called from `InventoryController::MouseUp`
    (`MouseDown(last_mouse_x, last_mouse_y, 0);`); `Sys_mouse_HandleClick`
    (`MainForm->animation->interface_controller->inventory->MouseDown(x, y, button);`).
  - Reachable with the shipped code: reachable. The right-button call comes from `Sys_mouse_HandleClick`, and nothing
    before the branch tests the box the item came from.
- **What you see:** The paperdoll draws only graphics above 0, so the left type-21 box shows nothing and the right box
  shows the left item's picture (an item in the right box loses its picture there) until the paperdoll's slots are set
  again, for example by the next paperdoll reply. The items equipped do not change; only the pictures do.

## The junk button works with no item dragged: it junks the item that was dragged last

`Ctrl_inventory.cpp`, `InventoryController::MouseDown` (`0x00535cfc`)

```cpp
            if (is_dragging && x >= 492 && x <= 530 && y >= 398 && y <= 436)
            {
    ...
            if (x >= 533 && x <= 571 && y >= 398 && y <= 436)
            {
                if (GetItemAmount(drag_item_id) < 2)
                {
                    if (drag_item_id == 1)
                    {
    ...
                        else
                        {
                            MainForm->net_client->sender->SendJunkItem(drag_item_id, 1);
                        }
                    }
                    else
                    {
                    MainForm->animation->game->OpenAmountBox(AmountBoxKind_JunkItem, GetItemAmount(drag_item_id), drag_item_id, 0);
                    input_enabled = false;
                }
                CancelDrag();
                UpdateItemHover(x, y);
            }
```

- **What the function is for:** `InventoryController::MouseDown` handles a click in the inventory panel, including the
  drop button and the junk button next to it, which act on the item being dragged (`drag_item_id`).
- **What the line was written to do:** Act only while an item is dragged, like the drop button. The evidence: the drop
  button's test starts with `is_dragging &&`, and the junk button's block ends with `CancelDrag()` and
  `UpdateItemHover(x, y)` like the drop button's, the calls that end a drag.
- **What it does:** Step by step:
  - The junk test checks only the click position, not `is_dragging`.
  - `drag_item_id` keeps the id of the last item picked up: the full write-site scan finds three stores, each of an item
    id when a drag starts (two in this function, one in `PaperdollController::MouseDown`), and none when a drag ends;
    the constructor does not set it.
  - So with no drag, a click on the junk button reads `GetItemAmount(drag_item_id)`: with 1 of that item still held (and
    it is not gold) `SendJunkItem(drag_item_id, 1)` is sent at once; with 2 or more the amount box opens for junking it
    (`OpenAmountBox(2, ...)`).
- **When it happens:** In the inventory panel (`Sys_mouse_HandleClick` calls this function while the sub state is the
  inventory's), any click on the junk button after an item was dragged and placed earlier in the session. Open rows
  checked for the inventory panel's pointer: 4 hits, all in `TMainForm::OnIdle`, calls to its dirty-flag test and mark
  methods, which store nothing in `drag_item_id`; no best-effort line touches it.
  - `InventoryController::MouseDown` is called from `InventoryController::MouseUp`
    (`MouseDown(last_mouse_x, last_mouse_y, 0);`); `Sys_mouse_HandleClick`
    (`MainForm->animation->interface_controller->inventory->MouseDown(x, y, button);`).
  - Search of the tree for `drag_item_id = [a-z]`: 3 line(s), Ctrl_inventory.cpp:850, Ctrl_inventory.cpp:914,
    Ctrl_paperdoll.cpp:622.
  - Search of the tree for `is_dragging && x >= 492`: 1 line(s), Ctrl_inventory.cpp:624.
  - Reachable with the shipped code: reachable. Nothing before the junk test checks for a drag, and the stores show
    `drag_item_id` keeps its last value.
- **What you see:** A player who moved an item in the grid and later clicks the junk button without dragging anything
  junks one of that item, if they hold exactly one, with no amount box; with a stack they get the junk amount box for
  it. Before the first drag of the session `drag_item_id` holds memory the code does not set.

## A right-click anywhere while the own paperdoll is open sends an unequip request for item 0

`Ctrl_paperdoll.cpp`, `PaperdollController::MouseDown` (`0x0053aaa8`)

```cpp
    char slot_clicked = 0;
    char no_room = 0;
    char cursed = 0;
    int item_id = 0;
    int sub_slot = 0;
    ...
        else if (is_own_character)
        {
        if (x >= window_x + 86 && y >= window_y + 19 && x <= window_x + 141 && y <= window_y + 72 && head_graphic > 0)
            item_id = head_item_id;
    ...
        if (MainForm->animation->interface_controller->inventory->HasRoomForItem(item_id))
        {
            if (MainForm->item_values->GetSpecial(item_id) == ItemSpecial_Cursed)
            {
    ...
            else
                MainForm->net_client->sender->SendUnequipItem(item_id, sub_slot);
        }
```

- **What the function is for:** `PaperdollController::MouseDown` handles a click while the paperdoll window is open.
  With the left button it starts dragging an equipped item; with another button it asks the server to unequip the item
  under the mouse.
- **What the line was written to do:** Send the unequip request only when the click is on a box that holds an item. The
  evidence: the left-button path sets `slot_clicked` in each box test and acts only `if (slot_clicked)`; in the other
  path each box test ends with an assignment such as `item_id = head_item_id`, so `item_id` keeps its start value when
  no box was hit.
- **What it does:** Step by step:
  - `item_id` starts at 0. When the click is on no box with an item, none of the box tests sets it.
  - `HasRoomForItem(0)` is then true: `GetSize(0)` gives -1 (it answers -1 for ids below 1), so the place it tries needs
    no cells and the first try finds no overlap.
  - `GetSpecial(0)` also gives -1, not 5, so the code reaches `SendUnequipItem(item_id, sub_slot)` with item 0 and sub
    slot 0.
- **When it happens:** `Sys_mouse_HandleClick` calls this function for any click not handled before it while the
  paperdoll is open, wherever the click lands, with `button` 1 for any button but the left one. The path needs
  `is_own_character`, the own character's paperdoll. Open rows checked: `PaperdollController::MouseDown` is not an open
  row and holds no best-effort line.
  - `PaperdollController::MouseDown` is called from `Sys_mouse_HandleClick`
    (`MainForm->animation->paperdoll->MouseDown(x, y, button);`).
  - Reachable with the shipped code: reachable. The right-button path has no test for a hit box before the request, and
    the size and special lookups for id 0 let it through.
- **What you see:** Each right-click outside the equipped boxes, on the open own paperdoll or anywhere else on the
  screen that no earlier handler takes, sends an unequip request for item 0 in sub slot 0 to the server; nothing in this
  function changes the screen.

## On a scrolled spell or skill grid, every mouse move over an icon shows its name in the status bar again

`Ctrl_spells.cpp`, `SpellController::UpdateHover` (`0x005bb04c`); `Ctrl_skills.cpp`, `SkillController::UpdateHover`
(`0x00620f60`)

```cpp
                if (col == it->grid_x && grid_row == it->grid_y && it->is_visible)
                {
                    found = 1;
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
  within an icon's cell. With the grid at its top, `top_row` is 0 and the test works. Open rows checked: neither
  `UpdateHover` is an open row or holds a best-effort line.
  - `SpellController::UpdateHover` is called from `SpellController::MouseMove` (`UpdateHover(x, y);`);
    `SpellController::MouseDown` (`UpdateHover(x, y);`).
  - `SkillController::UpdateHover` is called from `SkillController::MouseMove` (`UpdateHover(x, y);`);
    `SkillController::MouseDown` (`UpdateHover(x, y);`).
  - Search of the tree for `hover_row != it->grid_y`: 2 line(s), Ctrl_skills.cpp:255, Ctrl_spells.cpp:296.
  - Search of the tree for `[^_]hover_row = it->grid_y - top_row`: 2 line(s), Ctrl_skills.cpp:259, Ctrl_spells.cpp:300.
  - Reachable with the shipped code: reachable. `ScrollDown` raises `top_row` while it is below its limit, and
    `UpdateHover` runs on every mouse move.
- **What you see:** On a scrolled grid each mouse move over an icon calls `ShowStatusMessage` again, which replaces the
  status text and restarts its timer, so the hovered name pushes out any other status message; the panel is redrawn and
  the name measured again on every move.

## The skill panel is never filled: no code adds a skill to its grid, so it always shows empty

`Ctrl_skills.cpp`, `SkillController::SaveLayout` (`0x00621d2c`)

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
  - spells: `Ctrl_spells.cpp:783` `spells.insert(spells.end(), handle);` The spell grid is filled here and from
    `RestoreLayout`.
  - spells: `Ctrl_spells.cpp:800` `int slot_spell_id = MainForm->registry->ReadSlotValue(2, i);` The saved spell layout
    is read back.
  - skills: `Ctrl_skills.cpp:649` `MainForm->registry->WriteSlotValue(3, slot, slot_skill_id);` The skill layout is
    written, and nothing reads it back or adds to `skills`.
- **When it happens:** Always. Scan of every write: the tree has no insert into `skills`, and in the exe the pointer to
  this panel is used only for calls to its own methods, for `skills.size()` and for its scalar members; its methods
  contain no insert either. Open rows checked for the panel's pointer: 3 hits, all in `TMainForm::OnIdle`, all calls to
  its dirty-flag test methods, which only read; no best-effort line touches it.
  - Search of the tree for `[^_a-z]skills\.(insert|push_back)`: 0 line(s).
  - Search of the tree for `interface_controller->skills->TestAndClear`: 3 line(s), TMainForm.cpp:2661,
    TMainForm.cpp:2682, TMainForm.cpp:3294.
  - Search of the tree for `ReadSlotValue\(3`: 0 line(s).
  - Search of the tree for `WriteSlotValue\(3`: 1 line(s), Ctrl_skills.cpp:649.
  - Reachable with the shipped code: reachable. The panel opens and draws like the spell panel; only its list is never
    filled.
- **What you see:** The skill panel always shows an empty grid. The learn window and the forget box still work, because
  each also asks `HasSpell` or counts the spell grid.

## The skill info link is clicked at a fixed screen spot, so it stops lining up once the window is moved

`Ctrl_learn.cpp`, `LearnController::MouseDown` (`0x00624240`)

```cpp
        if (window_x + 17 < x && window_x + 252 > x)
        {
    ...
                    int row_pixel = (y - this->window_y - 50) % 37;
                    if (x > 125 && x < 190 && row_pixel > 18)
```

- **What the function is for:** On the list page of the skill master's window each row shows a skill's name and, under
  it, an info link. `LearnController::MouseDown` shows the skill's details for a click on the link and asks to learn the
  skill for a click elsewhere on the row.
- **What the line was written to do:** Test the click against the link where it is drawn. The evidence:
  `GameGraphics::DrawLearnText` draws the link at `window_x + 78`, and every other test in `LearnController::MouseDown`
  adds `window_x` to its x bounds.
- **What it does:** Step by step:
  - The link test compares `x` with fixed screen columns and leaves out `window_x`.
  - At the window's start place (`window_x` 49) those columns sit over the drawn link.
  - `MouseMove` lets the window be dragged to any `window_x` between 10 and 346; the test area stays where it was.
- **Evidence, from the code and not from the names:**
  - drawn: `Gfx_game.cpp:3585` `int name_x = MainForm->animation->learn->window_x + 78;` The name and the link under it
    are drawn from the window's position.
  - clicked: `Ctrl_learn.cpp:235` `if (window_x + 17 < x && window_x + 252 > x)` The row test follows the window; the
    link test inside it does not.
- **When it happens:** The window has been dragged sideways, and a row on the list page is clicked on its lower half.
  Open rows checked: `LearnController::MouseDown` and `GameGraphics::DrawLearnText` are not open rows and hold no
  best-effort line.
  - `LearnController::MouseDown` is called from `Sys_mouse_HandleClick`
    (`MainForm->animation->learn->MouseDown(x, y, button);`).
  - Search of the tree for `x > 125`: 1 line(s), Ctrl_learn.cpp:257.
  - Reachable with the shipped code: reachable. The window can be dragged by any spot outside the rows, and the link
    test runs on every lower-half click.
- **What you see:** After the window is moved, a click on the drawn info link asks to learn the skill instead of showing
  its details, while a click on the fixed spot, now elsewhere on the row, shows the details.

## On a scrolled shop or skill list, the status bar names the entry from the unscrolled list, not the hovered one

`Ctrl_shop.cpp`, `ShopController::MouseMove` (`0x00565284`); `Ctrl_learn.cpp`, `LearnController::MouseMove`
(`0x00626864`)

```cpp
        else
        {
            row = (y - window_y - 50) / NPC_LIST_ROW_HEIGHT;
            if ((int)shown_items.size() <= row || row > 4)
                row = -1;
        }
    ...
                    String tooltip_text = Client::Capitalize(MainForm->net_client,
                        MainForm->item_values->GetName(shown_items[row].item_id));
```

- **What the function is for:** `ShopController::MouseMove` and `LearnController::MouseMove` track the hovered row of a
  list page. When the row changes they name its entry in the status bar: an item with its price and class, or a skill
  with its requirements.
- **What the line was written to do:** Name the entry under the mouse. The evidence: `ShopController::MouseDown` and
  `LearnController::MouseDown` pick the clicked entry as `top_row + (y - window_y - 50) / 37` and
  `top_line + (y - window_y - 50) / 37`, and the list draws start from that top entry.
- **What it does:** Step by step:
  - `row` is the shown row, counted from the top of the window's list area.
  - The status text reads `shown_items[row]` or `offered_skills[row]`, with no `top_row` or `top_line` added.
  - With the list scrolled, that is the entry that many rows above the hovered one.
- **Evidence, from the code and not from the names:**
  - click: `Ctrl_shop.cpp:207` `int row = top_row + (y - window_y - 50) / NPC_LIST_ROW_HEIGHT;` The click adds the
    scroll position.
  - hover: `Ctrl_shop.cpp:499` `if ((int)shown_items.size() <= row || row > 4)` The hover row is limited to the five
    shown rows and used as an index into the list as it is, with no scroll position added.
- **When it happens:** The buy or craft list of a shop, or the list of skills on offer, has more than five entries and
  is scrolled down, and the mouse moves onto a row. Open rows checked: neither function is an open row or holds a
  best-effort line.
  - `ShopController::MouseMove` is called from `Mouser::HandleMove` (`MainForm->animation->shop->MouseMove(x, y);`).
  - `LearnController::MouseMove` is called from `Mouser::HandleMove` (`MainForm->animation->learn->MouseMove(x, y);`).
  - Search of the tree for `int row = top_row \+ \(y - window_y - 50\) / NPC_LIST_ROW_HEIGHT;`: 3 line(s),
    Ctrl_locker.cpp:129, Ctrl_locker.cpp:208, Ctrl_shop.cpp:207.
  - Search of the tree for `row = top_line \+ \(y - window_y - 50\) / NPC_LIST_ROW_HEIGHT;`: 4 line(s),
    Ctrl_guild.cpp:174, Ctrl_inn.cpp:174, Ctrl_law.cpp:167, Ctrl_learn.cpp:238.
  - Reachable with the shipped code: reachable. Both lists scroll once they hold more than five entries, and the status
    text is set on every row change.
- **What you see:** The highlight and the click both use the hovered entry, but the status bar names a different one,
  with that entry's price and class or its level, stat, gold and class requirements.

## Experience from a group share is left out of the gain totals the stats window shows

`Ctrl_status.cpp`, `StatusController::AddExperience` (`0x004f3fc8`)

```cpp
    if (!skip_next_exp_gain)
    {
        if (gained_experience > experience)
        {
            last_exp_gain = gained_experience - experience;
            total_exp_gained += last_exp_gain;
    ...
    experience += gained_experience;
```

- **What the function is for:** `StatusController::AddExperience` adds the experience a group share brings. Like
  `SetExperience`, it also keeps the gain totals for the stats window: `total_exp_gained`, `last_exp_gain` and
  `best_exp_gain`.
- **What the line was written to do:** Count the share as a gain. The evidence: `PacketProcessor::DispatchPacket` passes
  `shared_experience`, which it shows in the status bar and the chat as the experience gained, and the function adds it
  to `experience`.
- **What it does:** Step by step:
  - The gain block is the one from `SetExperience`, where the argument is the new total: it counts
    `gained_experience - experience` only when the argument is larger than the current total.
  - Here the argument is the share itself, so for any character whose total is larger than the share nothing is counted.
  - The next `SetExperience` measures from the total that already holds the share, so the share is never counted.
  - When the share is larger than the whole total, the amount counted is the share minus the total.
- **Evidence, from the code and not from the names:**
  - share: `Packetproces.cpp:3439` `MainForm->strings->GetText(87) + " " + IntToStr(shared_experience) + " EXP");` The
    share is shown as the gain just before it is passed to `AddExperience`.
  - total: `Packetproces.cpp:1931` `int kill_experience = DecodeNumber(packet.SubString(19, 4));` The kill reply carries
    the new total: the gain shown is this minus `experience`, and `SetExperience` gets the total.
- **When it happens:** The own character gets experience from a group share (family 24, action 33), the only call. Open
  rows checked: `StatusController::AddExperience` and `PacketProcessor::DispatchPacket` are not open rows; the
  best-effort lines in this unit are constructor stores.
  - `StatusController::AddExperience` is called from `PacketProcessor::DispatchPacket`
    (`MainForm->animation->interface_controller->status->AddExperience(shared_experience);`).
  - Reachable with the shipped code: reachable. Any experience shared in a group goes through this function.
- **What you see:** The stats window's total, best and last gain, and the hourly rates worked out from the total, leave
  out every group share.

## On a scrolled locker list the hover bar is drawn below the hovered item, or not at all

`Ctrl_locker.cpp`, `LockerController::MouseMove` (`0x005675e0`)

```cpp
        int row = top_row + (y - window_y - 50) / NPC_LIST_ROW_HEIGHT;
        if (row >= (int)items.size() || row > 4)
            row = -1;
        if (row != hovered_row)
        {
            hovered_row = row;
```

- **What the function is for:** `LockerController::MouseMove` keeps `hovered_row`, the row the window draws its hover
  bar on.
- **What the line was written to do:** Keep the shown row of the hovered item. The evidence:
  `GameGraphics::DrawLockerWindow` draws the bar at `37 * hovered_row` below the list top while it draws the items from
  `top_row`; the `row > 4` limit fits a shown row of a five-row list; the shop and trade windows keep the shown row in
  `hovered_row`.
- **What it does:** Step by step:
  - `row` adds `top_row`, so it is the item's place in the whole list.
  - The bar is drawn `top_row` rows below the hovered item.
  - Once `top_row` plus the shown row passes 4, `row` becomes -1 and no bar is drawn.
- **Evidence, from the code and not from the names:**
  - draw: `Gfx_game.cpp:914` `if (MainForm->animation->locker->hovered_row >= 0)` The bar is placed by the shown row.
  - hover: `Ctrl_locker.cpp:208` `int row = top_row + (y - window_y - 50) / NPC_LIST_ROW_HEIGHT;` The row kept is the
    place in the whole list.
- **When it happens:** The locker holds more than five items, the list is scrolled down, and the mouse is over the list.
  Open rows checked: `LockerController::MouseMove` and `GameGraphics::DrawLockerWindow` are not open rows and hold no
  best-effort line.
  - `LockerController::MouseMove` is called from `Mouser::HandleMove` (`MainForm->animation->locker->MouseMove(x, y);`).
  - Reachable with the shipped code: reachable. A locker list scrolls once it holds more than five items.
- **What you see:** On a scrolled list the hover bar marks the wrong item, and over the lower rows it disappears. A
  right-click still takes the hovered item, because `MouseDown` picks it the same way.

## In five npc windows a click on a row also grabs the window, so dragging from a row moves it

`Ctrl_guild.cpp`, `GuildController::MouseDown` (`0x0056c888`); `Ctrl_inn.cpp`, `InnController::MouseDown`
(`0x0062dca4`); `Ctrl_law.cpp`, `LawController::MouseDown` (`0x006226c8`); `Ctrl_locker.cpp`,
`LockerController::MouseDown` (`0x00567154`); `Ctrl_barber.cpp`, `BarberController::MouseDown` (`0x0056b47c`)

```cpp
    char row_handled = 0;
    ...
            is_dragging = 0;
    ...
    if (!row_handled)
    {
    ...
        if (this->window_x <= x && this->window_x + 284 >= x && y >= this->window_y && this->window_y + 290 >= y &&
            !this->thumb_grabbed)
        {
            this->is_dragging = 1;
    ...
    if (!flag)
{
```

- **What the function is for:** Each of these `MouseDown` functions first handles a click on a row of the window, then,
  under a "not handled" test, the scroll bar and the start of a window move.
- **What the line was written to do:** Skip the scroll bar and window-move block after a row took the click. The
  evidence: the block is guarded by a flag that starts at 0, and the windows built the same way set their flag when a
  row is clicked, as `ChestController::MouseDown` does with `handled = 1` and `ShopController::MouseDown` in each row
  branch.
- **What it does:** Step by step:
  - `row_handled` in `GuildController::MouseDown` and `flag` in the inn, law, locker and barber versions start at 0 and
    are never set.
  - So after a row click the last test of the block still runs, and inside the window it sets `is_dragging`.
  - The row branches of the guild, inn, law and locker windows even clear `is_dragging` first; the block sets it again.
- **Evidence, from the code and not from the names:**
  - set: `Ctrl_chest.cpp:79` `handled = 1;` The chest window marks a row click handled and skips the move block.
  - unset: `Ctrl_guild.cpp:168` `char row_handled = 0;` The guild window's flag keeps this value.
- **When it happens:** A row of one of these windows is clicked and the mouse moves before the button is let go. Open
  rows checked: none of the five functions is an open row or holds a best-effort line; the flags are locals of each
  function.
  - `GuildController::MouseDown` is called from `Sys_mouse_HandleClick`
    (`MainForm->animation->guild->MouseDown(x, y, button);`).
  - `InnController::MouseDown` is called from `Sys_mouse_HandleClick`
    (`MainForm->animation->inn->MouseDown(x, y, button);`).
  - `LawController::MouseDown` is called from `Sys_mouse_HandleClick`
    (`MainForm->animation->law->MouseDown(x, y, button);`).
  - `LockerController::MouseDown` is called from `Sys_mouse_HandleClick`
    (`MainForm->animation->locker->MouseDown(x, y, button);`).
  - `BarberController::MouseDown` is called from `Sys_mouse_HandleClick`
    (`MainForm->animation->barber->MouseDown(x, y, button);`).
  - Search of the tree for `if \(!flag\)`: 4 line(s), Ctrl_barber.cpp:127, Ctrl_inn.cpp:241, Ctrl_law.cpp:209,
    Ctrl_locker.cpp:156.
  - Search of the tree for `if \(!row_handled\)`: 1 line(s), Ctrl_guild.cpp:395.
  - Search of the tree for `[^_]flag = (1|true)|row_handled = 1`: 0 line(s).
  - Reachable with the shipped code: reachable. Every row click in these windows runs the block, and `MouseMove` moves
    the window while `is_dragging` is set.
- **What you see:** Pressing on a row and moving the mouse before letting go drags the whole window, and while it is
  dragged the row hover is not tracked; `MouseUp` ends the drag.

## Opening the locker, memo or law window can leave the learn, guild or inn window open under or over it

`Ctrl_locker.cpp`, `LockerController::Open` (`0x00566f4c`); `Ctrl_memo.cpp`, `MemoController::Open` (`0x0062b430`);
`Ctrl_law.cpp`, `LawController::Open` (`0x006225ac`)

```cpp
    MainForm->animation->shop->is_visible = 0;
    MainForm->animation->guild->is_visible = 0;
    MainForm->animation->memo->is_visible = 0;
    MainForm->animation->locker->is_visible = 0;
    MainForm->animation->law->is_visible = 0;
    MainForm->animation->inn->is_visible = 0;
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
  - full: `Ctrl_inn.cpp:112` `MainForm->animation->law->is_visible = 0;` The inn window hides all six others.
  - short: `Ctrl_law.cpp:110` `MainForm->animation->memo->is_visible = 0;` The law window's list ends here, without the
    inn.
- **When it happens:** The server's reply that opens the locker, a memo or the law window arrives while the learn, guild
  or inn window is open. Open rows checked: the three `Open` functions are not open rows and hold no best-effort line.
  - `LockerController::Open` is called from `PacketProcessor::DispatchPacket`
    (`MainForm->animation->locker->Open(locker_title, 0, locker_x, locker_y);`).
  - `MemoController::Open` is called from `PacketProcessor::DispatchPacket`
    (`MainForm->animation->memo->Open(info_name + " - " + info_time_text);`); `PacketProcessor::DispatchPacket`
    (`MainForm->animation->memo->Open(inventory_owner_name + " - " + inventory_time_text);`);
    `PacketProcessor::DispatchPacket` (`MainForm->animation->memo->Open(notice_title);`).
  - `LawController::Open` is called from `PacketProcessor::DispatchPacket`
    (`MainForm->animation->law->Open(law_request_id);`).
  - Reachable with the shipped code: unknown. While any of these windows is open, map clicks go to the window code in
    `Sys_mouse_HandleClick`, so the request for the second window has to leave before the first one opens, or the server
    has to send the reply on its own.
- **What you see:** Both windows stay visible. `TMainForm::OnIdle` draws memo, guild, inn, law, locker, shop and learn
  in that order, so the learn window covers the locker, the guild window covers the new memo, and the law window covers
  the inn window.

## A board post with a subject over 32 characters is sent with that subject's start as its body

`Ctrl_msgboard.cpp`, `MsgBoardController::SubmitPost` (`0x005b4a48`)

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
  - subject: `Ctrl_msgboard.cpp:50` `MainForm->edits->SetMaxLength(20, 32);` The subject edit holds at most 32
    characters.
  - body: `Ctrl_msgboard.cpp:197` `body = body.SubString(1, 1200);` The body is cut into itself.
- **When it happens:** A post whose subject is longer than 32 characters is sent. The store is
  `body = subject.SubString(1, 32)` in `SubmitPost` (0x5b4a48): the cut subject goes into `body`, the body argument.
  - `MsgBoardController::SubmitPost` is called from `Sys_mouse_HandleClick`
    (`MainForm->animation->message_board->SubmitPost(post_subject, post_message);`).
  - Reachable with the shipped code: unknown. The subject comes from edit 20, which `CreatePostEdits` limits with
    `SetMaxLength(20, 32)`; a longer subject reaches this test only if the edit lets one through.
- **What you see:** The post goes to the server with the long subject and, as its body, the subject's first 32
  characters.

## The board opens a post only for clicks on a row's left part, and also for clicks above and below the list

`Ctrl_msgboard.cpp`, `MsgBoardController::MouseDown` (`0x005b5020`)

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
  - hover: `Ctrl_msgboard.cpp:383` `if (row > 11)` The hovered row is one of the twelve shown rows.
  - click: `Ctrl_msgboard.cpp:276` `int row = top_line + (y - window_y - 47) / NPC_TEXT_LINE_HEIGHT;` The clicked row
    has no such limit.
- **When it happens:** The post list is shown and a click lands on the right part of a row, above the list, or below it.
  Open rows checked: `MsgBoardController::MouseDown` is not an open row and holds no best-effort line.
  - `MsgBoardController::MouseDown` is called from `Sys_mouse_HandleClick`
    (`MainForm->animation->message_board->MouseDown(x, y, button);`).
  - Reachable with the shipped code: reachable. Every click on the post list goes through this test.
- **What you see:** A click on a highlighted row right of `window_x + 282` opens nothing and starts moving the window. A
  click near the window's top opens the first shown post, or on a scrolled list one of the two posts above it, and a
  click below the list opens a post that is not shown, when the board has one there.

## A help line that wraps into more than ten pieces is written past the end of its ten-piece array

`Ctrl_help.cpp`, `HelpController::AddText` (`0x00628484`)

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
  - array: `Ctrl_help.cpp:201` `String wrapped_pieces[MAX_WRAP_PIECES];` Ten pieces fit.
  - loop: `Ctrl_help.cpp:213` `for (int i = 0; i <= 12; i++)` Up to thirteen are made.
- **When it happens:** A help file holds a line so long that it wraps into more than ten pieces of the panel's width.
  Open rows checked: `HelpController::AddText` is not an open row and holds no best-effort line. In the exe the array is
  the last local of the frame (at ebp-0x2C, ten slots), so the eleventh piece lands on the next slot and the twelfth and
  thirteenth on the saved frame pointer and the return address.
  - `HelpController::AddText` is called from `HelpController::LoadPage`
    (`AddText(line, link_target, line_flag, hide_icon);`).
  - Reachable with the shipped code: unknown. It depends on the line lengths in the help files, which are data outside
    the exe.
- **What you see:** The extra pieces overwrite what lies after the array on the stack, including the function's saved
  frame pointer and return address for the twelfth and thirteenth.

## Holding the book's scroll arrow scrolls only one line, because nothing repeats it

`Ctrl_book.cpp`, `BookController::MouseDown` (`0x006342a0`)

```cpp
    if (x >= window_x + 190 && x <= window_x + 205 && y >= window_y + 35 && y <= window_y + 50)
    {
        scroll_up_held = 1;
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
  - repeat: `Ctrl_shop.cpp:156` `if (scroll_repeat_delay > 0)` The shop window counts the delay down and repeats a held
    arrow.
  - book: `Ctrl_book.cpp:70` `scroll_up_held = 1;` The book marks the arrow held, and nothing reads it.
- **When it happens:** The book window is open and a scroll arrow is held down. Scan of every read: the tree has none;
  in the exe the three fields (+72, +73, +76) are written only by `MouseDown` and cleared by `MouseUp`, and the other
  uses of the book pointer are draw reads of other members and calls. Open rows checked: 0 hits for the book pointer.
  - `BookController::MouseDown` is called from `Sys_mouse_HandleClick`
    (`MainForm->animation->book->MouseDown(x, y, button);`).
  - Search of the tree for `book->scroll_(up|down)_held|book->scroll_repeat_delay`: 0 line(s).
  - Reachable with the shipped code: reachable. Any book with more entries than fit has working arrows, and holding one
    is ordinary use.
- **What you see:** Each press scrolls one line; a long book is scrolled by clicking repeatedly or by dragging the
  thumb.

## A news item that wraps into more than ten lines is written past the end of its ten-piece array

`Ctrl_news.cpp`, `NewslistController::AddNewsItem` (`0x005325d0`)

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
  - array: `Ctrl_news.cpp:112` `String wrapped_pieces[MAX_WRAP_PIECES];` Ten pieces fit.
  - loop: `Ctrl_news.cpp:124` `for (int i = 0; i <= 12; i++)` Up to thirteen are made.
- **When it happens:** The server's welcome reply carries a news item so long that it wraps into more than ten pieces of
  the panel's width. Open rows checked: `NewslistController::AddNewsItem` is not an open row and holds no best-effort
  line. In the exe the array is the last local of the frame (at ebp-0x2C, ten slots), so the eleventh piece lands on the
  next slot and the twelfth and thirteenth on the saved frame pointer and the return address.
  - `NewslistController::AddNewsItem` is called from `PacketProcessor::DispatchPacket`
    (`MainForm->animation->interface_controller->news->AddNewsItem(MainForm->net_client->NextToken());`).
  - Reachable with the shipped code: unknown. It depends on the length of the news items the server sends, which is
    outside the exe.
- **What you see:** The extra pieces overwrite what lies after the array on the stack, including the function's saved
  frame pointer and return address for the twelfth and thirteenth.

## A word too wide for the help window makes it add empty lines without end, so the client stops responding

`Ctrl_report.cpp`, `ReportController::AddText` (`0x0062bc08`)

```cpp
            do
            {
                String fitted_text;
    ...
                        String prefix = text.SubString(1, i);
                        if (MainForm->text_canvas->TextWidth(prefix) >= max_width)
                        {
                            lines->Add(fitted_text);
                            line_values->Add(right_text);
                            text.Delete(1, fitted_text.Length());
    ...
                        fitted_text = prefix;
    ...
            } while (text.Length() > 0);
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
  - width: `Ctrl_report.cpp:77` `int max_width = 200;` Every line is cut to this width.
  - cut: `Ctrl_report.cpp:107` `text.Delete(1, fitted_text.Length());` The cut removes only the part that fitted before
    the next word, which is empty when the first word is already too wide.
- **When it happens:** A text given to `ReportController::AddText` has a word, or a run without spaces, at least as wide
  as the window's line width at the start of a line. Every call passes a text that `StringContainer::GetText` reads from
  a data file with `LoadFromFile`. Open rows checked: `ReportController::AddText` is not an open row and holds no
  best-effort line. In the exe the fitted part is set to empty at the top of each turn, and the delete takes its length.
  - `ReportController::AddText` is called from `Sys_mouse_HandleClick`
    (`MainForm->animation->report->AddText(MainForm->strings->GetText(377), "", true);`); `Sys_mouse_HandleClick`
    (`MainForm->animation->report->AddText(MainForm->strings->GetText(378), "", true);`); `Sys_mouse_HandleClick`
    (`MainForm->animation->report->AddText(MainForm->strings->GetText(379), "", false);`); `Sys_mouse_HandleClick`
    (`MainForm->animation->report->AddText(MainForm->strings->GetText(380), "", false);`); `Sys_mouse_HandleClick`
    (`MainForm->animation->report->AddText(MainForm->strings->GetText(381), "", false);`).
  - Search of the tree for `report->AddText\(`: 5 line(s), Sys_mouse.cpp:3291, Sys_mouse.cpp:3292, Sys_mouse.cpp:3293,
    Sys_mouse.cpp:3294, Sys_mouse.cpp:3295.
  - Reachable with the shipped code: unknown. It depends on the texts in the client's data file, which is outside the
    exe.
- **What you see:** Opening the help window then hangs the client, with both line lists growing by an empty line on
  every turn.

## Without preloading, an npc can miss its walking or attack frames when another npc finishes loading first

`Sys_npcs.cpp`, `Npcs::LoadPendingGraphics` (`0x00522704`); `Sys_npcs.cpp`, `Npcs::Npcs` (`0x0051fa48`)

```cpp
    preload_graphics = preload_option;
    if (preload_graphics)
{
                        graphics_library = LoadLibraryA(MainForm->strings->GetGfxPath(21).c_str());
        library_ref_count++;
                        }
    ...
                if (!i->load_stage)
                {
                    if (!preload_graphics && library_ref_count <= 0)
                        graphics_library = LoadLibraryA(MainForm->strings->GetGfxPath(21).c_str());
    ...
                if (i->load_stage == 1)
                {
                    library = (int)graphics_library;
                    if (graphics_library)
    ...
                if (i->load_stage == 2)
                {
                    int attack_library = (int)graphics_library;
                    if (graphics_library)
    ...
                if (Application->Active || MainForm->config->GetFullscreen() == 0)
                    i->load_stage++;
                if (i->load_stage >= 4)
                {
                    i->graphics_pending = 0;
                    i->redraw = 1;
                    if (!preload_graphics)
                    {
                        if (library_ref_count > 0)
                            library_ref_count--;
                        if (!library_ref_count)
                        {
                            FreeLibrary(graphics_library);
                            graphics_library = 0;
                        }
                    }
                }
```

- **What the function is for:** `Npcs::LoadPendingGraphics` loads each pending npc's frames from the npc library over
  four calls (standing, walking, attacking, done), opening the library when it is not held and closing it when the last
  npc is done.
- **What the line was written to do:** Keep the library open while any npc still needs it, by counting the npcs that
  opened it. The evidence: the constructor follows its `LoadLibraryA` with `library_ref_count++`, and the done path
  takes one off the count and closes the library only when the count reaches 0.
- **What it does:** Step by step, with preload_graphics off:
  - An npc's first stage calls `LoadLibraryA` whenever `library_ref_count` is 0 or less, and puts the handle in
    `graphics_library`. Nothing adds to `library_ref_count` here, so it stays 0.
  - When an npc reaches its last stage, `library_ref_count` is already 0, so the library is freed and `graphics_library`
    becomes 0, whether or not other npcs are still loading.
  - An npc at its walking stage (stage 1) or attack stage (stage 2) copies `graphics_library` and loads its frames only
    when it is not 0. With 0 it loads nothing, and its stage still goes up by one, so that stage is not tried again.
  - For example: npc A starts loading one call before npc B, and B comes after A in the list. In the call where A
    finishes, A sets `graphics_library` to 0 before B, now at its attack stage, reads it, so B gets no attack frames. An
    npc starting its first stage in between would have opened the library again.
- **When it happens:** In game, with the preload-character setting off (`TMainForm::FormCreate` passes it to the `Npcs`
  constructor), whenever npcs come into view at different times: `AddNpc` is called for each npc of the nearby-entities
  packet and for an npc that appears on its own (`DispatchPacket`), and `TMainForm::Timer1Timer` runs
  `LoadPendingGraphics` on every tick. The write scan: in the tree, `library_ref_count` gains only in the constructor's
  preload branch, loses only on the done path, and is set to 0 in the constructor and in `ReloadGraphicsLibrary`
  (searches below). Over every function of the exe, the stores to the count's offset are the same four: two clears, the
  constructor's increment and the done path's decrement. The stores to the handle are the two `LoadLibraryA` calls of
  the constructor and `ReloadGraphicsLibrary` (both only with preloading), the one here, and the 0 here. The held scan
  of the open rows and best-effort lines finds no store to either offset.
  - `Npcs::LoadPendingGraphics` is called from `TMainForm::Timer1Timer` (`npcs->LoadPendingGraphics();`).
  - `Npcs::AddNpc` is called from `PacketProcessor::DispatchPacket` (`MainForm->npcs->AddNpc(`);
    `PacketProcessor::ParseNearbyEntities` (`MainForm->npcs->AddNpc(`).
  - Search of the tree for `library_ref_count\+\+`: 1 line(s), Sys_npcs.cpp:21.
  - Search of the tree for `library_ref_count--`: 1 line(s), Sys_npcs.cpp:1143.
  - Search of the tree for `library_ref_count = 0;`: 2 line(s), Sys_npcs.cpp:16, Sys_npcs.cpp:762.
  - Search of the tree for `graphics_library = 0;`: 1 line(s), Sys_npcs.cpp:1147.
  - Reachable with the shipped code: reachable. It needs preloading off and two npcs whose loading overlaps with the
    later one behind in the list; npcs that appear one at a time while others are still loading give exactly that.
- **What you see:** The npc stands and idles normally, but its walking or attack cells were never drawn into, so while
  it walks or attacks those cells show whatever the new surface held when `CreateSheets` made it; the code never fills
  them. It lasts until the npc leaves view and is added again.

## With the soft keypad, the login OK and Cancel buttons take clicks 22 pixels left of where they are drawn

`Sys_mouse.cpp`, `Sys_mouse_HandleClick` (`0x0045def0`)

```cpp
                if (MainForm->animation->GetSubState() == MenuScreen_PasswordGrid && *mouse >= 360 && *mouse <= 450 && mouse[1] >= 388 &&
                    mouse[1] <= 417 && MainForm->animation->menu->login_enabled)
    ...
                if (*mouse >= 452 && *mouse <= 542 && mouse[1] >= 388 && mouse[1] <= 417)
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
  - `Sys_mouse_HandleClick` tests OK on the password grid (sub-state 8) at x 360 to 450, and Cancel at x 452 to 542 in
    sub-states 4, 7 and 8 alike: the areas of the layout without the keypad.
  - Sub-states 7 and 8 come only with the keypad: two of the three `SetSubState(7)` calls open the login screen in the
    branch taken when `GetSkipSoftpad` gives 0, the third goes back from the grid, and the grid opens only from 7.
- **When it happens:** Each click on the login screen with the keypad layout (the skip-keypad setting not set), which
  `TMainForm::FormMouseDown` hands to `Sys_mouse_HandleClick`. The searches below find the one Cancel test, the two OK
  tests at x 360, and the two draws of each keypad-layout button.
  - `MenuGraphics::DrawConfirmButtons` is called from `TMainForm::OnIdle` (`gfx->menu->DrawConfirmButtons();`).
  - `Mouser::HandleMove` is called from `TMainForm::HandleMMTimerEvent` (`mouse->HandleMove();`).
  - Search of the tree for `\*mouse >= 452 && \*mouse <= 542`: 1 line(s), Sys_mouse.cpp:4043.
  - Search of the tree for `\*mouse >= 360 && \*mouse <= 450`: 2 line(s), Sys_mouse.cpp:3991, Sys_mouse.cpp:4006.
  - Search of the tree for `BltFast\(382, 391`: 2 line(s), Gfx_menu.cpp:1201, Gfx_menu.cpp:1204.
  - Search of the tree for `BltFast\(474, 391`: 2 line(s), Gfx_menu.cpp:1207, Gfx_menu.cpp:1209.
  - Search of the tree for `SetSubState\(MenuScreen_GridAccountName\)`: 3 line(s), Packetcontrols.cpp:829,
    Packetproces.cpp:342, Sys_mouse.cpp:4195.
  - Reachable with the shipped code: reachable. The keypad layout is the one used unless the skip-keypad setting is on.
- **What you see:** On the password grid a click on the right fifth of the lit OK button (x 452 to 471) cancels back to
  the main menu, the strip just left of OK (x 360 to 381) logs in, and the right end of Cancel does nothing.

## If another box has the focus as the chat lock runs out, the chat box stays hidden and closed to typing

`Sys_edits.cpp`, `Edits::TimerTick` (`0x00444510`); `Sys_edits.cpp`, `Edits::LockChatInput` (`0x0044726c`)

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
            if (chat_locked && !lock_countdown && it->id == 11)
            {
                it->visible = 1;
                it->editable = 1;
            }
    ...
            it->editable = 0;
            it->visible = 0;
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
  `SetEditable` or `SetVisible` passes 11, and `LockChatInput` has the one caller. The held scan of the open rows and
  best-effort lines finds no store to either flag's offset.
  - `Edits::LockChatInput` is called from `PacketProcessor::DispatchPacket` (`MainForm->edits->LockChatInput();`).
  - `Edits::TimerTick` is called from `TMainForm::HandleMMTimerEvent` (`edits->TimerTick();`).
  - Search of the tree for `(?<=->)editable =`: 3 line(s), Sys_edits.cpp:53, Sys_edits.cpp:765, Sys_edits.cpp:884.
  - Search of the tree for `(?<=->)visible =`: 3 line(s), Sys_edits.cpp:52, Sys_edits.cpp:471, Sys_edits.cpp:885.
  - Search of the tree for `SetEditable\(11`: 0 line(s).
  - Search of the tree for `SetVisible\(11`: 0 line(s).
  - Search of the tree for `AddEdit\(11`: 1 line(s), Packetproces.cpp:252.
  - Reachable with the shipped code: reachable. A locked player who has a drop, trade or other amount box, or an input
    box, open at the moment the lock runs out.
- **What you see:** The lock icon goes away, but the chat box stays hidden and takes no typing. It comes back only when
  the game is entered again and the chat box is made anew.

## A map graphic exactly 1 pixel high is not marked loaded, because its height is compared with 1, not -1

`Gfx_mapblends.cpp`, `BlendGraphics::LoadPendingGraphic` (`0x0052a218`); `Gfx_mapeffects.cpp`,
`EffectGraphics::LoadPendingGraphic` (`0x005b0f10`); `Gfx_mapitems.cpp`, `ItemGraphics::LoadPendingGraphics`
(`0x0052c9dc`); `Gfx_mapmasks.cpp`, `MaskGraphics::LoadPendingGraphic` (`0x00517954`); `Gfx_mapobjects.cpp`,
`ObjectGraphics::LoadPendingGraphic` (`0x004f9110`); `Gfx_maptops.cpp`, `TopGraphics::LoadPendingGraphic`
(`0x005198d0`); `Gfx_mapwalls.cpp`, `WallGraphics::LoadPendingGraphic` (`0x00518b3c`); `Gfx_mapwalls.cpp`,
`WallGraphics::LoadGraphicNow` (`0x00518d84`)

```cpp
        for (vector<BlendWrapper>::iterator it = entries.begin(); it != entries.end(); it++)
        {
            if (!it->is_loaded)
            {
                HMODULE open_library = library;
                it->CreateResource();
                GraphicSize size = it->resource->LoadSurface((int)open_library, it->graphic_id + 100);
                it->width = size.width;
                it->height = size.height;
                if (size.width != -1 && size.height != 1)
                {
                    it->is_loaded = 1;
                    it->idle_ticks = 0;
                    pending_count--;
                }
                if (!pending_count)
                    queue_drained = 1;
                break;
            }
        }
...
                if (sub_item_size.width != -1 && sub_item_size.height != 1)
```

- **What the function is for:** Each map graphic list loads one queued graphic per call from its gfx file, stores its
  width and height, and marks the entry loaded so the next call moves on to the next queued graphic.
- **What the line was written to do:** Mark the entry loaded unless the load failed. The evidence:
  `BlendResource::LoadSurface` reports a failed draw by setting both `size.width` and `size.height` to -1, and the same
  line tests the width against -1. The exe holds the 1 at all nine sites: in `BlendGraphics::LoadPendingGraphic` the two
  compares are 0x52a30b cmp dword ptr [ebp - 0x30], -1 and 0x52a311 cmp dword ptr [ebp - 0x2c], 1. The same pattern is
  at 0x5b100c (effects), 0x52cad4 and 0x52cbb7 (items and sub-items), 0x517a4d (masks), 0x4f9209 (objects), 0x5199aa
  (tops), 0x518c35 and 0x518eec (walls). The tile list, `TileGraphics::LoadPendingGraphic`, makes no test and marks
  every entry loaded.
- **What it does:** Step by step, for `BlendGraphics::LoadPendingGraphic` (the other eight sites are the same):
  - The first entry with `is_loaded` 0 (not loaded) is loaded with `BlendResource::LoadSurface`, which returns the
    bitmap's width in `size.width` and its height in `size.height`, or -1 and -1 when the draw throws.
  - The entry is marked loaded (`is_loaded = 1`) and `pending_count` drops by one only when the width is not -1 and the
    height is not 1.
  - A failed load is still caught by the width test, so it stays queued as intended.
  - A bitmap exactly 1 pixel high stays unloaded, and `pending_count` keeps counting it.
  - The loop stops at the first unloaded entry (`break`), so the next call loads the same 1-pixel bitmap again.
- **Evidence, from the code and not from the names:**
  - fail: `Gfx_blend.cpp:76` `size.height = -1;` A failed draw sets the height to -1 (and the width to -1 on the line
    above), so -1 is the failure value the test was written against. The BitBlt path does the same.
  - size: `Gfx_blend.cpp:89` `BitBlt(dc, 0, 0, size.width, size.height, CompatibleDC, 0, 0, 13369376);` The two halves
    of the result are the width and height of the copied bitmap in pixels.
  - test: `Gfx_mapblends.cpp:86` `if (size.width != -1 && size.height != 1)` The exe compares with 1 here: 0x52a311 cmp
    dword ptr [ebp - 0x2c], 1.
- **When it happens:** `TMainForm::Timer1Timer` calls the seven `LoadPendingGraphic` and `LoadPendingGraphics` functions
  while graphics are queued; `MapController::OpenDoor` calls `WallGraphics::LoadGraphicNow` for a door's wall graphic.
  - `BlendGraphics::LoadPendingGraphic` is called from `TMainForm::Timer1Timer` (`gfx->blends->LoadPendingGraphic();`, 2
    calls).
  - `EffectGraphics::LoadPendingGraphic` is called from `TMainForm::Timer1Timer` (`gfx->effects->LoadPendingGraphic();`,
    2 calls).
  - `ItemGraphics::LoadPendingGraphics` is called from `TMainForm::Timer1Timer` (`gfx->items->LoadPendingGraphics();`, 2
    calls).
  - `MaskGraphics::LoadPendingGraphic` is called from `TMainForm::Timer1Timer` (`gfx->masks->LoadPendingGraphic();`).
  - `ObjectGraphics::LoadPendingGraphic` is called from `TMainForm::Timer1Timer` (`gfx->objects->LoadPendingGraphic();`,
    2 calls).
  - `TopGraphics::LoadPendingGraphic` is called from `TMainForm::Timer1Timer` (`gfx->tops->LoadPendingGraphic();`, 2
    calls).
  - `WallGraphics::LoadPendingGraphic` is called from `TMainForm::Timer1Timer` (`gfx->walls->LoadPendingGraphic();`, 2
    calls).
  - `WallGraphics::LoadGraphicNow` is called from `MapController::OpenDoor`
    (`MainForm->gfx->walls->LoadGraphicNow(walls[x][y]);`); `MapController::OpenDoor`
    (`MainForm->gfx->walls->LoadGraphicNow(walls_next_row[x][y]);`).
  - Search of the tree for `\.height != 1\)`: 9 line(s), Gfx_mapblends.cpp:86, Gfx_mapeffects.cpp:77,
    Gfx_mapitems.cpp:106, Gfx_mapitems.cpp:135, Gfx_mapmasks.cpp:86, Gfx_mapobjects.cpp:86, Gfx_maptops.cpp:83,
    Gfx_mapwalls.cpp:83, ....
  - Reachable with the shipped code: only through a graphic 1 pixel high.
- **What you see:** With a graphic 1 pixel high, that list would load the
  same graphic on every call and the graphics queued after it in the list would stay unloaded.

## Small ones with no effect

- **The ping shown in chat is a large negative number when midnight passes before the reply.** `Packetproces.cpp`,
  `PacketProcessor::DispatchPacket` (`0x004af778`).
  - Code:
    `TDateTime now1 = Now(); TTimeStamp sent_stamp; sent_stamp = DateTimeToTimeStamp(MainForm->animation->interface_controller->status->ping_sent_time); TTimeStamp now_stamp; now_stamp = DateTimeToTimeStamp(now1); int round_trip_ms = now_stamp.Time - sent_stamp.Time;`
  - What it does: `DateTimeToTimeStamp` splits a time into the day number and `Time`, the milliseconds since midnight.
    `round_trip_ms` subtracts only the `Time` of `ping_sent_time` from the `Time` of now, so a reply after midnight to a
    request sent before it gives a negative difference.
  - When it happens: The ping command typed in the chat box sets `ping_sent_time` and sends `SendPingRequest`; the reply
    is family 35, action 241. Reachable with the shipped code: reachable. Any ping sent just before midnight whose reply
    arrives after it.
  - What you see: Sent at 23:59:59.900 and answered at 00:00:00.100, the chat line shows -86399800 ms instead of 200 ms.
- **A map item the server shows is sorted into place twice in a row.** `Packetproces.cpp`,
  `PacketProcessor::DispatchPacket` (`0x004af778`).
  - Code:
    `MainForm->map_items->AddProtectedItem(shown_item_uid, shown_item_id, shown_item_x, shown_item_y, shown_item_amount, 11110000, 10); MainForm->map_items->SortByPosition();`
  - What it does: The handler adds the item with `AddProtectedItem`, then calls `SortByPosition` twice in a row. The
    second call sorts a list that is already in order.
  - When it happens: Each item reply of family 14, action 7. Reachable with the shipped code: reachable. It runs every
    time that reply arrives.
  - What you see: Nothing a player can see; the map's item list is sorted one more time.
- **Walking through a player who blocks the way passes that player's id to the request, which leaves it out.**
  `Packetoperations.cpp`, `PacketOperator::SendWalkGhost` (`0x004a643c`).
  - Code:
    `void PacketOperator::SendWalkGhost(unsigned int direction, unsigned int x, unsigned int y, int blocking_player_id) { MainForm->net_client->CommitActionTime(); String packet = packet + Client::EncodeNumber(MainForm->net_client, direction, 1); packet = packet + Client::EncodeNumber(MainForm->net_client, MainForm->net_client->last_action_tick, 3); packet = packet + Client::EncodeNumber(MainForm->net_client, x, 1); packet = packet + Client::EncodeNumber(MainForm->net_client, y, 1); if (packet.Length() >= 1) MainForm->net_client->SendPacket(AC_HIT, OP_MOVE, packet);`
  - What it does: `GameController::Tick` calls `SendWalkGhost` with `last_blocking_player_id` as the fourth argument.
    The function packs only the direction, `last_action_tick` and the tile, so the id goes nowhere.
  - When it happens: When a player stands in the way for more than 42 ticks and the player is not casting, attacking or
    sitting. Reachable with the shipped code: reachable. `GameController::Tick` makes the call each time that wait runs
    out.
  - What you see: Nothing a player can see; the request is the same as a step without the id.
- **A path step down or up tests the map edge of the other direction.** `Pathfinder.cpp`, `PathFinder::FollowPath`
  (`0x0051e1ac`).
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
  `GameGraphics::DrawItemAmountText` (`0x0047b520`); `TMainForm.cpp`, `TMainForm::OnIdle` (`0x004049ac`).
  - Code:
    `int prompt_text_id = 57; if (MainForm->animation->game->amount_box_kind == 2 || MainForm->animation->game->amount_box_kind == 3) prompt_text_id = prompt_text_id + MainForm->animation->game->amount_box_kind - 1; if (animation->game->amount_box_kind == 3) OffscreenSurface->BltFast(animation->game->amount_box_x + 11, animation->game->amount_box_y + 10, gfx->interface_graphics->control_sheet, &gfx->game->amount_title_rect, 16);`
  - What it does: - `OpenAmountBox` and `OpenAmountBoxAtTile` store their first argument in `amount_box_kind`; the
    search below finds no other store. - The seventeen call sites pass 1, 2, 4, 5, 6, 7, 8, 9, 10 or 11 (calls below). -
    So `amount_box_kind == 3` never holds: the strip `amount_title_rect` and the third prompt text are not drawn, and
    the mouse handler `Sys_mouse_HandleClick` has no kind-3 branch of its own.
  - When it happens: The amount box opens from the inventory, bank, guild, shop and trade clicks listed below, and from
    dropping an item on a map tile (`OpenAmountBoxAtTile`). Called from `BankController::MouseDown`,
    `BankController::MouseDown`, `GuildController::MouseDown`, `InventoryController::MouseDown`,
    `InventoryController::MouseDown`, `ShopController::MouseDown`, `ShopController::MouseDown`, `Sys_mouse_HandleClick`,
    `Sys_mouse_HandleClick`, `Sys_mouse_HandleClick`, `Sys_mouse_HandleClick`, `Mouser::HandleMouseUp`,
    `Mouser::HandleMouseUp`, `Mouser::HandleMouseUp`, `Mouser::HandleMouseUp`, `Sys_mouse_HandleClick`,
    `Mouser::HandleMouseUp`. The search for `\bamount_box_kind =[^=]` finds 2 line(s). The search for
    `amount_box_kind == 3\b` finds 2 line(s). Reachable with the shipped code: never reached. Every call of
    `OpenAmountBox` and `OpenAmountBoxAtTile` passes a literal kind other than 3 (calls below), and `amount_box_kind` is
    stored on two lines only, both from that argument. The program's own code of the functions whose source is not yet
    rebuilt exactly, and the lines kept as best-effort text, hold no call of either function and no store to the
    member's offset in the game controller.
  - What you see: None visible. The kind-3 title strip of the control sheet and its prompt text never appear on screen.
- **The player's graphics load stage copied for BlitFrame is overwritten with 99, so the frame-buffer draw always
  runs.** `Gfx_sprites.cpp`, `SpriteGraphics::DrawPlayer` (`0x00494e90`); `Gfx_sprites.cpp`, `SpriteGraphics::BlitFrame`
  (`0x004989c8`).
  - Code:
    `if (player.graphics_pending) player_load_stage = player.load_progress; else player_load_stage = 99; player_load_stage = 99; if (use_frame_buffer) { if (player_load_stage > 1)`
  - What it does: - `player_load_stage = player.load_progress;` when the player's graphics are pending, else
    `player_load_stage = 99;`. - The next line sets `player_load_stage = 99;` in every case, so the first value is
    dropped. - `BlitFrame` then always finds `player_load_stage > 1` true. - The value it would have kept is
    `load_progress`, which both `Player` constructors set to 0 and no line raises.
  - When it happens: Every player draw from `TMainForm::OnIdle` runs these lines. The search for `\bplayer_load_stage =`
    finds 3 line(s). The search for `(?<![\w>.])\bload_progress =` finds 2 line(s). Reachable with the shipped code:
    reachable. `DrawPlayer` runs for every player in view, and the overwrite has no condition.
  - What you see: None visible: `player_load_stage` is 99 on every draw. Had the first value been kept, a player whose
    graphics are still pending (`load_progress` 0) would skip the frame-buffer blit in `BlitFrame`.
- **Each bitmap loader calls Restore on its surface only when the surface pointer is null.** `Gfx_wall.cpp`,
  `WallResource::LoadSurface` (`0x005183cc`); `Gfx_bank.cpp`, `BankGraphics::LoadSurface` (`0x00565e6c`);
  `Gfx_barber.cpp`, `BarberGraphics::LoadSurface` (`0x0056be3c`); `Gfx_blend.cpp`, `BlendResource::LoadSurface`
  (`0x00529ac0`); `Gfx_dialog.cpp`, `DialogGraphics::LoadSurface` (`0x006324cc`); `Gfx_icon.cpp`,
  `IconResource::LoadSurface` (`0x0061fbc8`); `Gfx_item.cpp`, `ItemResource::LoadSurface` (`0x0052c1d4`);
  `Gfx_jukebox.cpp`, `JukeboxGraphics::LoadSurface` (`0x005b5f38`); `Gfx_mask.cpp`, `MaskResource::LoadSurface`
  (`0x005171fc`); `Gfx_msgboard.cpp`, `MsgBoardGraphics::LoadSurface` (`0x005b36e4`); `Gfx_object.cpp`,
  `ObjectResource::LoadSurface` (`0x005165d8`); `Gfx_social.cpp`, `SocialGraphics::LoadSurface` (`0x0056ad40`);
  `Gfx_stats.cpp`, `StatGraphics::LoadSurface` (`0x005b6d50`); `Gfx_subitem.cpp`, `SubItemResource::LoadSurface`
  (`0x00538ad4`); `Gfx_tile.cpp`, `TileResource::LoadSurface` (`0x00516b64`); `Gfx_top.cpp`, `TopResource::LoadSurface`
  (`0x0045a928`); `Gfx_effect.cpp`, `EffectResource::LoadSurfaces` (`0x005b03d4`); `Gfx_chest.cpp`,
  `ChestGraphics::LoadBackground` (`0x0055de98`); `Gfx_chest.cpp`, `ChestGraphics::LoadItemSlot` (`0x0055e1d0`);
  `Gfx_shop.cpp`, `ShopGraphics::LoadBackground` (`0x00561ac4`); `Gfx_shop.cpp`, `ShopGraphics::LoadItemSlot`
  (`0x00561dfc`); `Gfx_trade.cpp`, `TradeGraphics::LoadBackground` (`0x0055d614`); `Gfx_trade.cpp`,
  `TradeGraphics::LoadItemSlot` (`0x0055d94c`); `Gfx_musician.cpp`, `MusicianGraphics::LoadBackground` (`0x0062cfc8`);
  `Gfx_musician.cpp`, `MusicianGraphics::LoadCellSheet` (`0x0062d300`).
  - Code:
    `if (!surface) surface->Restore(); if (surface->Created) surface->Destroy(); if (!layer0_surface) if (!layer1_surface) if (!layer2_surface) if (!background_surface) if (!item_slot_surface) if (!cell_sheet_surface)`
  - What it does: - `if (!self->surface)` calls `self->surface->Restore();` only when `surface` is null, so the call
    would go through a null pointer. - Every store to these surface members is `new TDx_DrawSurface(NULL)` in a
    constructor (searches below), so the test is always false and `Restore` is never called.
  - When it happens: Every bitmap load of the map lists, the spell icons, the effects and the window graphics runs the
    test. The search for `((\b|->)surface|layer[012]_surface|background_surface|item_slot_surface|cell_sheet_surface) =`
    finds 30 line(s). The search for
    `((\b|->)surface|layer[012]_surface|background_surface|item_slot_surface|cell_sheet_surface) = (?!new TDx_DrawSurface\(NULL\);)`
    finds 0 line(s). Reachable with the shipped code: never reached. The search for any store to `surface`, a
    `layer0_surface`..`layer2_surface`, `background_surface`, `item_slot_surface` or `cell_sheet_surface` member finds
    30 lines, and the search for one that is not `new TDx_DrawSurface(NULL)` finds 0. The program's own code of the
    functions whose source is not yet rebuilt exactly, and the lines kept as best-effort text, hold no store of a
    surface pointer or a surface constructor call.
  - What you see: None visible: no surface is restored here, lost or not.
- **A failed canvas draw onto a loaded bitmap surface gives the surface DC back twice.** `Gfx_wall.cpp`,
  `WallResource::LoadSurface` (`0x005183cc`); `Gfx_bank.cpp`, `BankGraphics::LoadSurface` (`0x00565e6c`);
  `Gfx_barber.cpp`, `BarberGraphics::LoadSurface` (`0x0056be3c`); `Gfx_blend.cpp`, `BlendResource::LoadSurface`
  (`0x00529ac0`); `Gfx_dialog.cpp`, `DialogGraphics::LoadSurface` (`0x006324cc`); `Gfx_icon.cpp`,
  `IconResource::LoadSurface` (`0x0061fbc8`); `Gfx_item.cpp`, `ItemResource::LoadSurface` (`0x0052c1d4`);
  `Gfx_jukebox.cpp`, `JukeboxGraphics::LoadSurface` (`0x005b5f38`); `Gfx_mask.cpp`, `MaskResource::LoadSurface`
  (`0x005171fc`); `Gfx_msgboard.cpp`, `MsgBoardGraphics::LoadSurface` (`0x005b36e4`); `Gfx_object.cpp`,
  `ObjectResource::LoadSurface` (`0x005165d8`); `Gfx_social.cpp`, `SocialGraphics::LoadSurface` (`0x0056ad40`);
  `Gfx_stats.cpp`, `StatGraphics::LoadSurface` (`0x005b6d50`); `Gfx_subitem.cpp`, `SubItemResource::LoadSurface`
  (`0x00538ad4`); `Gfx_tile.cpp`, `TileResource::LoadSurface` (`0x00516b64`); `Gfx_top.cpp`, `TopResource::LoadSurface`
  (`0x0045a928`); `Gfx_chest.cpp`, `ChestGraphics::LoadBackground` (`0x0055de98`); `Gfx_chest.cpp`,
  `ChestGraphics::LoadItemSlot` (`0x0055e1d0`); `Gfx_shop.cpp`, `ShopGraphics::LoadBackground` (`0x00561ac4`);
  `Gfx_shop.cpp`, `ShopGraphics::LoadItemSlot` (`0x00561dfc`); `Gfx_trade.cpp`, `TradeGraphics::LoadBackground`
  (`0x0055d614`); `Gfx_trade.cpp`, `TradeGraphics::LoadItemSlot` (`0x0055d94c`); `Gfx_musician.cpp`,
  `MusicianGraphics::LoadBackground` (`0x0062cfc8`); `Gfx_musician.cpp`, `MusicianGraphics::LoadCellSheet`
  (`0x0062d300`).
  - Code:
    `catch (Exception &) { MainForm->CountDrawError(); surface->ReleaseDC(dc); size.width = -1; size.height = -1; } surface->ReleaseDC(dc);`
  - What it does: - When the canvas `Draw` throws, the `catch` block counts the draw error, calls `ReleaseDC(dc)` and
    sets the size to -1. - Control then leaves the `catch` and reaches the `ReleaseDC(dc)` after the block, so the same
    DC is given back a second time.
  - When it happens: Only when drawing the bitmap onto the surface raises an exception, with BitBlt drawing turned off.
    Reachable with the shipped code: unknown. It needs the canvas `Draw` to throw, which depends on the surface and the
    display at run time.
  - What you see: None visible: the second `ReleaseDC` has its result ignored.
- **A wall sheet exactly 120 pixels wide, or a tile sheet exactly 250, is flagged animated but drawn whole.**
  `Gfx_wall.cpp`, `WallResource::LoadSurface` (`0x005183cc`); `Gfx_wall.cpp`, `Gfx_wall_GetFrameRect` (`0x00518364`);
  `Gfx_tile.cpp`, `TileResource::LoadSurface` (`0x00516b64`); `Gfx_tile.cpp`, `Gfx_tile_GetFrameRect` (`0x00516af8`).
  - Code:
    `if (MainForm->resource_bitmap->Width >= 120) if (self->sheet_width > 120) if (MainForm->resource_bitmap->Width >= 250) if (self->surface_rect.right > 250)`
  - What it does: - `LoadSurface` sets the flag at `Width >= 120` for walls and `>= 250` for tiles. - `GetFrameRect`
    cuts the sheet into quarters only at `> 120` and `> 250`. - At exactly that width the flag is set, so the map keeps
    redrawing for animation, but every frame is the whole sheet.
  - When it happens: Every wall and ground tile load and draw. Reachable with the shipped code: unknown. It depends on
    whether the wall or tile library holds a bitmap of exactly that width.
  - What you see: Such a sheet would be drawn whole, its four frames side by side, on every animation step.
- **A map graphic exactly one pixel tall counts as a failed load and holds up the rest of its queue.**
  `Gfx_mapwalls.cpp`, `WallGraphics::LoadPendingGraphic` (`0x00518b3c`); `Gfx_mapwalls.cpp`,
  `WallGraphics::LoadGraphicNow` (`0x00518d84`); `Gfx_mapobjects.cpp`, `ObjectGraphics::LoadPendingGraphic`
  (`0x004f9110`); `Gfx_mapmasks.cpp`, `MaskGraphics::LoadPendingGraphic` (`0x00517954`); `Gfx_maptops.cpp`,
  `TopGraphics::LoadPendingGraphic` (`0x005198d0`); `Gfx_mapblends.cpp`, `BlendGraphics::LoadPendingGraphic`
  (`0x0052a218`); `Gfx_mapeffects.cpp`, `EffectGraphics::LoadPendingGraphic` (`0x005b0f10`); `Gfx_mapitems.cpp`,
  `ItemGraphics::LoadPendingGraphics` (`0x0052c9dc`).
  - Code: `if (size.width != -1 && size.height != 1) if (sub_item_size.width != -1 && sub_item_size.height != 1)`
  - What it does: - The height is compared with 1, not -1, in all nine load tests (walls twice, objects, masks, top
    layer, blends, effects, items and item pictures); the program's own code has `!= 1` at each of them. - A bitmap of
    height 1 therefore leaves its entry unloaded and the queue count unchanged. - Each load call takes the first
    unloaded entry and stops, so the next call tries the same entry again, and the entries queued after it are not
    reached.
  - When it happens: Every queued load of those lists. Reachable with the shipped code: unknown. It depends on whether a
    graphics library holds a bitmap exactly one pixel tall.
  - What you see: Such a graphic would be loaded again on every call and not shown, and the graphics queued after it in
    the same list would wait until the list is released.
- **The game interface loads only if the first graphics file opened, whether or not the interface file did.**
  `Gfxprotection.cpp`, `GfxController::LoadGameGraphics` (`0x0041f0f4`).
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
- **Each time a shop, chest, bank or other game window opens, its graphics files are opened again and not closed.**
  `Gfxprotection.cpp`, `GfxController::LoadTradeGraphics` (`0x0041e658`); `Gfxprotection.cpp`,
  `GfxController::LoadShopGraphics` (`0x0041e754`); `Gfxprotection.cpp`, `GfxController::LoadDialogGraphics`
  (`0x0041e850`); `Gfxprotection.cpp`, `GfxController::LoadReportGraphics` (`0x0041e8e4`); `Gfxprotection.cpp`,
  `GfxController::LoadStatGraphics` (`0x0041e9e0`); `Gfxprotection.cpp`, `GfxController::LoadMsgBoardGraphics`
  (`0x0041ea74`); `Gfxprotection.cpp`, `GfxController::LoadSocialGraphics` (`0x0041eb08`); `Gfxprotection.cpp`,
  `GfxController::LoadBankGraphics` (`0x0041eb9c`); `Gfxprotection.cpp`, `GfxController::LoadJukeboxGraphics`
  (`0x0041ec30`); `Gfxprotection.cpp`, `GfxController::LoadMusicianGraphics` (`0x0041ecc4`); `Gfxprotection.cpp`,
  `GfxController::LoadBarberGraphics` (`0x0041ed6c`); `Gfxprotection.cpp`, `GfxController::LoadChestGraphics`
  (`0x0041ee00`).
  - Code: `HMODULE LibraryA = LoadLibraryA(GetGfxFileName(INTERFACE_GFX_FILE).c_str());`
  - What it does: - These twelve loaders call `LoadLibraryA` sixteen times in all (trade, shop, report and chest open
    two files) and have no `FreeLibrary`; the program's own code of the twelve has none either. - Each open of a file
    that is already open adds one to its use count, so the count rises on every window opening.
  - When it happens: Each opening of the trade, shop, dialog, report, stat, message board, social, bank, jukebox,
    musician, barber and chest windows; the shop loader also runs for the guild, inn, law, learn, locker and memo
    windows. Reachable with the shipped code: reachable. Opening any of these windows runs its loader, and the open
    succeeds whenever the graphics file exists.
  - What you see: The two graphics files stay loaded for the rest of the session once such a window has opened, and the
    `FreeLibrary` calls of the other loaders on the same file no longer unload it.
- **Started from another working folder, the start-up file check passes without checking any file.**
  `Fileprotection.cpp`, `FileController::VerifyFiles` (`0x00414fc8`).
  - Code: `else { all_ok = 0; if (!i) all_ok = PrefixAppDirectory(); break; }`
  - What it does: - On that first miss `VerifyFiles` sets its result to what `PrefixAppDirectory` returns and leaves the
    loop at once. - `PrefixAppDirectory` returns true whenever the program path has a folder, so the check reports
    success. - None of the other listed files is checked, with or without the folder in front; the program's own code
    returns the prefix result directly too.
  - When it happens: Once at start-up, from `TMainForm::FormCreate`, when the working folder is not the program's
    folder. Reachable with the shipped code: reachable. Starting the program from a shortcut or a command line whose
    working folder is elsewhere makes the bare program name miss.
  - What you see: In that case missing data, graphics, sound or music files are not reported at start-up; the "Endless
    File Protection Error" message cannot appear.
- **Screenshots check for the program's screen folder but are saved into a screen folder under the working folder.**
  `Gfx_interface.cpp`, `InterfaceGraphics::SaveScreenshot` (`0x00429178`); `Configuration.cpp`, `Config::Config`
  (`0x00440340`).
  - Code:
    `String screen_folder = MainForm->config->GetAppPath() + "/screen/"; String relative_folder = ".\\screen\\"; lpFileName = relative_folder + "ENDL" + number_text + ".BMP"; if (access(lpFileName.c_str(), 0)) String lpFileName = exe_dir + "screen";`
  - What it does: - The config sets `screen_folder_exists` from a folder beside the program (`exe_dir + "screen"`). -
    `SaveScreenshot` builds `screen_folder` from the program's path and does not use it; the free name is searched and
    the file saved under `relative_folder`, a path relative to the working folder. - The program's own code builds both
    strings and passes only the relative one to `access` and the save.
  - When it happens: Each screenshot key press in game, from `KeyBoard::PollKeyboard` and from the key test in
    `TMainForm::OnIdle`. Reachable with the shipped code: reachable. Starting the program from a shortcut or command
    line whose working folder is not the program's folder makes the two folders differ.
  - What you see: In that case the screenshot goes to a screen folder under the working folder; with none there, no
    screenshot is kept, while the screen folder beside the program stays empty.
- **A jumping player's health bar and damage number rise twice as far as the body.** `Gfx_sprites.cpp`,
  `SpriteGraphics::DrawPlayer` (`0x00494e90`).
  - Code:
    `if (!player.jump_frame || player.jump_frame == 2) { frame_rect.top = frame_rect.top - 14; frame_rect.bottom = frame_rect.bottom - 14; } if (!player.jump_frame || player.jump_frame == 2) { move_bar_y -= 14; } if (!player.jump_frame || player.jump_frame == 2) { move_damage_y -= 14; }`
  - What it does: - The jump branch of `DrawPlayer` (jump flag `jumping`, step `jump_frame`) lifts the body frame by 14
    pixels on steps 0 and 2 and by 22 on step 1, one subtraction each from `frame_rect.top` and `frame_rect.bottom`. -
    The health bar and damage number positions repeat that lift as two subtractions from the same variable
    (`move_bar_y -= 14;` twice, `move_damage_y -= 22;` twice, and so on), so they rise 28 and 44 pixels. - Apart from
    the lift, the jump places the bar and number exactly as the walk branch does.
  - When it happens: A player jumps (`Players::StartJump`, called when a step lands on or leaves a jump tile) while that
    player's overhead health bar (`health_bar_shown`) or damage number (`damage_shown`) is showing. The search for
    `(move_bar_y|move_damage_y) -=` finds 8 line(s). Reachable with the shipped code: reachable. `Players::StartJump` is
    called from the arrow-key walk in `Ctrl_game.cpp`, from `Pathfinder.cpp` and from the walk packet in
    `Packetproces.cpp`; the health bar and damage number show after any hit (`Players::ShowHealthBar`,
    `Players::ShowDamage`).
  - What you see: During the jump the bar and the number float 14 or 22 pixels higher above the body than usual, then
    drop back when the jump ends.
- **The help report window brightens its link lines by the guild window's hovered row, not its own.** `Gfx_game.cpp`,
  `GameGraphics::DrawReportText` (`0x00488c98`).
  - Code:
    `if (line[1] == 42) { line.Delete(1, 1); if (MainForm->animation->guild->hovered_row == row) MainForm->text_canvas->Font->Color = (TColor)0xDCDCDC; else MainForm->text_canvas->Font->Color = (TColor)0xB4B4B4;`
  - What it does: - `DrawReportText` draws a line that starts with an asterisk as a link, underlined, and picks the
    bright link colour when `MainForm->animation->guild->hovered_row == row`, else the dim link colour. - The report
    window keeps its own hovered line in `report->hovered_line`, which `ReportController::MouseMove` sets and
    `DrawReportWindow` uses for the row highlight bar; `DrawReportText` does not read it. - The same test with
    `guild->hovered_row` is the guild window's own link test in `DrawGuildText`, the function before it in the file.
  - When it happens: The help report window is open (`ReportController::Open`, from the click handled at
    Sys_mouse.cpp:3258) and one of its lines is a link. The search for `guild->hovered_row == row` finds 2 line(s). The
    search for `report->hovered_line` finds 2 line(s). Reachable with the shipped code: unknown. The draw runs on every
    frame while the report window is open. Its lines are language texts added by `ReportController::AddText`; whether
    any starts with an asterisk depends on the text file the client reads, which is data outside the program.
    `ReportController::MouseDown` acts on up to three such link lines.
  - What you see: A hovered link in the help report window keeps the dim link colour. The link drawn bright, if any, is
    the one whose row equals the guild window's last hovered row (0 after `GuildController::ClearText`, -1 once the
    mouse leaves the guild list).
- **The experience window's per-hour rates divide whole minutes and whole hours, so they move in steps.**
  `Gfx_game.cpp`, `GameGraphics::DrawStatText` (`0x00485b60`).
  - Code:
    `if (session_rate < 60) { if (session_rate > 0) { double session_rate_scaled = (double)(60 / session_rate) * (double)MainForm->animation->interface_controller->status->total_exp_gained; session_rate = session_rate_scaled; } } else { double session_rate_hourly = (double)MainForm->animation->interface_controller->status->total_exp_gained / (double)(session_rate / 60); session_rate = session_rate_hourly; }`
  - What it does: - Under an hour the rate is the gain times 60 / minutes, a whole-number division taken before the
    conversion to double: 7 minutes gives 8, 25 minutes gives 2, and every length from 31 to 59 minutes gives 1. - From
    an hour on the rate is the gain divided by minutes / 60, in whole hours: 60 to 119 minutes divide by 1, 120 to 179
    by 2. - Both rates work this way: the session rate over the minutes since login (`usage_start_time`), and the
    all-play rate over `usage_minutes` plus the session. - Under an hour of all play the all-play rate scales the
    session gain `total_exp_gained`; from an hour on it divides the character's whole `experience`.
  - When it happens: Each draw of the experience window, opened from the game bar (`GameBars::MouseDown`). The search
    for `60 / (session_rate|total_rate)\)` finds 2 line(s). The search for `(session_rate|total_rate) / 60\)` finds 2
    line(s). Reachable with the shipped code: reachable. `DrawStatText` runs on every frame while the experience window
    is open, and the session length grows through every band above as the player stays logged in.
  - What you see: The rates are off by up to the size of the dropped fraction and jump at band edges. After 45 minutes
    the session rate shows the session gain unscaled; after 90 minutes it shows the whole session gain as one hour's
    worth, and at 120 minutes it halves at once.
- **Players stay marked as loading graphics for good: the done check waits on a cell flag that is only set to 0.**
  `Sys_players.cpp`, `Players::LoadPendingGraphics` (`0x004e69f0`); `Sys_players.cpp`, `Players::ReloadGraphics`
  (`0x004ea638`).
  - Code:
    `if (i->reload_requested && Application->Active) { i->FreeResources(); i->CreateResources(); i->reload_requested = 0; i->graphics_pending = 1; i->extra_sheets_loaded = false; if (i->graphics_pending && !i->reload_requested && (Application->Active || !MainForm->config->GetFullscreen())) i->redraw = 1; i->cast_sheets_loaded = true; loaded_count++; if (i->sheets_cleared && i->walk_sheets_loaded && i->walk_tail_sheets_loaded && i->stand_sheet_loaded && i->sit_sheets_loaded && i->attack_sheets_loaded && i->extra_sheets_loaded && i->cast_sheets_loaded && i->emote_sheets_loaded) { i->graphics_pending = 0; i->redraw = 1; } it->graphics_pending = 1; it->extra_sheets_loaded = false;`
  - What it does: Step by step: - When a player is marked pending, the flags from `sheets_cleared` to
    `emote_sheets_loaded` are set to 0: in `LoadPendingGraphics` when a reload is asked for (`reload_requested`), and in
    `ReloadGraphics`. - Each group block sets its own flag to 1 when it draws. The player's byte `extra_sheets_loaded`
    (the member the header declares as `extra_sheets_loaded`) is only ever given 0, or copied from another player
    record. - The test at the end needs `extra_sheets_loaded` along with the other eight flags, so it is false for every
    player, and `graphics_pending = 0` with the redraw after it is not reached. - So `graphics_pending` stays 1. On each
    call the function works out the six part numbers again for every player in the list (until four groups are drawn),
    and every group block finds its flag set and draws nothing.
  - When it happens: On every call of `LoadPendingGraphics`, which `TMainForm::Timer1Timer` makes on each tick of its
    timer, for every player that `AddPlayer` adds (it passes 1 to the `Player` constructor, which marks the player
    pending), and for every player `ReloadGraphics` marks after a change of looks. The write scan behind "only ever
    given 0": in the tree, the searches below find the two constructor lines and the two clears in this file, and no
    other line names the byte. Over every function of the exe, each store that reaches byte 22 of a record (byte, word
    or dword stores, ORs, copies and fills) was listed: inside the player code there are 21. Four give 0 (the two
    constructors, these two clears), and the remaining 17 copy byte 22 from another player record (the record copies of
    the list, of `SortByPosition` swaps, and of the local player that `AddPlayer`, `AddPreviewPlayer` and
    `AddCharacterSlots` insert). The code outside reaches players only through the `Players` functions, or by copies
    such as `GetPlayerById`. The held scan of the open rows and best-effort lines finds stores at that offset only in
    npc, path and effect records (`Npcs`, `PathFinder` and `Effects` code), and no open row is player code. Called from
    `TMainForm::Timer1Timer`. The search for `(extra_sheets_loaded|extra_sheets_loaded) = 1` finds 0 line(s). The search
    for `(i|it)->extra_sheets_loaded = false;` finds 2 line(s). The search for `(?<![\w>.])extra_sheets_loaded = false;`
    finds 2 line(s). Reachable with the shipped code: never reached. The clearing line needs `extra_sheets_loaded` to be
    non-zero. The write scan in the when field finds every store to that byte giving 0 or a copy of the same byte of
    another player, so no player ever holds a non-zero value there.
  - What you see: Nothing a player sees changes: the groups are drawn as their actions need them either way, and the
    other blocks ask for their own redraws. The cost is the work of the loop on every timer tick for every player on the
    list, for as long as they stay on it.
- **Weapon 67 is listed in two swing-effect groups; the later entry wins, so it swings with the group 3 effect.**
  `Sys_weaponmap.cpp`, `WeaponMapper::WeaponMapper` (`0x005b2cb0`).
  - Code:
    `weapon_effect_types[66] = 1; weapon_effect_types[67] = 2; weapon_effect_types[3] = 2; weapon_effect_types[1] = 3; weapon_effect_types[9] = 3; weapon_effect_types[65] = 3; weapon_effect_types[67] = 3;`
  - What it does: Step by step: - The table starts at 0 for every weapon, then each group's weapons are set: group 1,
    group 2, group 3 and on. - Weapon 67 is set to 2 as the first entry of the group 2 lines, and again to 3 among the
    group 3 lines. - The second store replaces the first, so the table holds 3 for weapon 67 and its group 2 entry has
    no effect.
  - When it happens: Every time a player swings weapon 67: `TMainForm::OnIdle` reads
    `weapon_effect_types[player.weapon]` for a weapon that `IsRangedWeapon` does not list, and draws that group's swing
    effect. The table is filled once, when the form builds its `WeaponMapper`; the search below finds the two stores and
    no other line that names entry 67. The search for `weapon_effect_types\[67\]` finds 2 line(s). The search for
    `weapon_effect_types\[` finds 41 line(s). Reachable with the shipped code: reachable. Any player attacking with
    weapon 67.
  - What you see: A player swinging weapon 67 shows the swing effect of group 3, the group of weapons 1, 9 and 65; the
    group 2 line for it does nothing.
- **Freeing the sound device frees sound buffer 4 in place of buffers 5, 6 and 7, which stay as they are.**
  `TMainForm.cpp`, `TMainForm::Dx_Sound1Destroy` (`0x0040437c`).
  - Code:
    `if (Dx_SoundBuffer7->Created) Dx_SoundBuffer4->Destroy(); if (Dx_SoundBuffer6->Created) Dx_SoundBuffer4->Destroy(); if (Dx_SoundBuffer5->Created) Dx_SoundBuffer4->Destroy(); if (Dx_SoundBuffer4->Created) Dx_SoundBuffer4->Destroy(); if (Dx_SoundBuffer3->Created) Dx_SoundBuffer3->Destroy();`
  - What it does: Step by step: - `Dx_SoundBuffer7->Created` is tested, and when it holds, `Dx_SoundBuffer4->Destroy()`
    runs. - The same goes for buffers 6 and 5: each test frees buffer 4. - Then buffer 4 is tested and freed, and
    buffers 3, 2 and 1 are each tested and freed. - So buffers 5, 6 and 7 are left as they are, and when they exist,
    buffer 4's `Destroy` runs up to four times, the later times on a buffer that is already freed. What a second
    `Destroy` does is up to the TDx library.
  - When it happens: The handler has no caller in the tree; the form file, which the tree does not hold, binds it to the
    sound device. The device is freed on every way out (`FormClose`, `FormDestroy`, Alt+F4 on the menu screens, the exit
    button in `Sys_mouse_HandleClick`) and in game when a message box of kind 5 turns sound back on, which frees and
    makes the device again. The searches below count the frees of the device and of buffers 5 to 7. The search for
    `Dx_Sound1->Destroy\(\)` finds 8 line(s). The search for `Dx_SoundBuffer[567]->Destroy\(\)` finds 3 line(s).
    Reachable with the shipped code: unknown. It depends on the form file binding this handler to the sound device's
    destroy event; the tree holds no call to it.
  - What you see: Buffers 5 to 7 still playing when the device goes stay marked as made until `UpdateSounds` sees them
    stop and frees them. Nothing a player hears changes on the way out.
- **The surface restores before each frame is shown run only when the surface pointer is null, so they restore
  nothing.** `TMainForm.cpp`, `TMainForm::OnIdle` (`0x004049ac`).
  - Code:
    `if (!BackBuffer) BackBuffer->Restore(); BackBuffer->BltFast(0, 0, OffscreenSurface, &screen_rect, 16); if (!PrimarySurface) PrimarySurface->Restore(); PrimarySurface->Flip(OffscreenSurface, 1); Dx_Draw1->WaitForVerticalBlank(1); if (!OffscreenSurface) OffscreenSurface->Restore(); PrimarySurface->Blt(&window_rect, OffscreenSurface, 0, 16777216, 0); if (PrimarySurface->ErrorValue == DDERR_SURFACELOST || OffscreenSurface->ErrorValue == DDERR_SURFACELOST) Dx_Draw1->RestoreAllSurfaces();`
  - What it does: Step by step: - Before each blit or flip, `if (!PrimarySurface)`, `if (!BackBuffer)` and
    `if (!OffscreenSurface)` test the pointer to the surface object, not whether the surface is lost. - The pointers are
    form components and are not null, so the restore calls under them are skipped; a null pointer would call `Restore`
    through that null pointer. - The very next line uses the same pointer to blit or flip.
  - When it happens: On every frame shown. The form sets these pointers itself; the searches below find no line of the
    tree that assigns them, and `FormCreate` installs `OnIdle` only after testing `PrimarySurface->Created` and
    `OffscreenSurface->Created` through them. The held scan of the open rows and best-effort lines finds no store to
    these members either. The search for `(PrimarySurface|BackBuffer|OffscreenSurface) =` finds 0 line(s). The search
    for `if \(!(PrimarySurface|BackBuffer|OffscreenSurface)\)$` finds 5 line(s). Reachable with the shipped code: never
    reached. The restores need a null surface pointer, and no line sets these pointers; a lost surface is restored only
    by the `RestoreAllSurfaces` test after the frame is shown.
  - What you see: A lost surface makes one frame's blit or flip fail, and the test after it restores the surfaces for
    the next frame; the lines before the draw add nothing.
- **On the password grid, 2, 4, 6 and 8 do nothing, while letters and other digits open the grid's message box.**
  `Sys_keyboard.cpp`, `KeyBoard::PollKeyboard` (`0x00447434`).
  - Code:
    `if ((MainForm->DIKeyboardState1->Keys[DIK_0] & 0x80) != 0 || (MainForm->DIKeyboardState1->Keys[DIK_1] & 0x80) != 0 || (MainForm->DIKeyboardState1->Keys[DIK_3] & 0x80) != 0 || (MainForm->DIKeyboardState1->Keys[DIK_5] & 0x80) != 0 || (MainForm->DIKeyboardState1->Keys[DIK_7] & 0x80) != 0 || (MainForm->DIKeyboardState1->Keys[DIK_9] & 0x80) != 0 || (MainForm->DIKeyboardState1->Keys[DIK_A] & 0x80) != 0 || (MainForm->DIKeyboardState1->Keys[DIK_B] & 0x80) != 0 || (MainForm->DIKeyboardState1->Keys[DIK_C] & 0x80) != 0 || (MainForm->DIKeyboardState1->Keys[DIK_D] & 0x80) != 0 || (MainForm->DIKeyboardState1->Keys[DIK_E] & 0x80) != 0 || (MainForm->DIKeyboardState1->Keys[DIK_F] & 0x80) != 0 || (MainForm->DIKeyboardState1->Keys[DIK_G] & 0x80) != 0 || (MainForm->DIKeyboardState1->Keys[DIK_H] & 0x80) != 0 || (MainForm->DIKeyboardState1->Keys[DIK_I] & 0x80) != 0 || (MainForm->DIKeyboardState1->Keys[DIK_J] & 0x80) != 0 || (MainForm->DIKeyboardState1->Keys[DIK_K] & 0x80) != 0 || (MainForm->DIKeyboardState1->Keys[DIK_L] & 0x80) != 0 || (MainForm->DIKeyboardState1->Keys[DIK_M] & 0x80) != 0 || (MainForm->DIKeyboardState1->Keys[DIK_N] & 0x80) != 0 || (MainForm->DIKeyboardState1->Keys[DIK_O] & 0x80) != 0 || (MainForm->DIKeyboardState1->Keys[DIK_P] & 0x80) != 0 || (MainForm->DIKeyboardState1->Keys[DIK_Q] & 0x80) != 0 || (MainForm->DIKeyboardState1->Keys[DIK_R] & 0x80) != 0 || (MainForm->DIKeyboardState1->Keys[DIK_S] & 0x80) != 0 || (MainForm->DIKeyboardState1->Keys[DIK_T] & 0x80) != 0 || (MainForm->DIKeyboardState1->Keys[DIK_U] & 0x80) != 0 || (MainForm->DIKeyboardState1->Keys[DIK_V] & 0x80) != 0 || (MainForm->DIKeyboardState1->Keys[DIK_W] & 0x80) != 0 || (MainForm->DIKeyboardState1->Keys[DIK_X] & 0x80) != 0 || (MainForm->DIKeyboardState1->Keys[DIK_Y] & 0x80) != 0 || (MainForm->DIKeyboardState1->Keys[DIK_Z] & 0x80) != 0)`
  - What it does: Step by step: - The test reads the key state of the digit keys 0, 1, 3, 5, 7 and 9, then of the 26
    letter keys. - When any of them is down, the box opens with `GetText(41)` and `GetText(384)`, with its picture, and
    the caret is turned off. - The keys for 2, 4, 6 and 8 are not in the test, and the grid takes no typed text, so
    pressing one of them does nothing at all.
  - When it happens: Each keyboard read while the password grid is up and no message box is open:
    `TMainForm::HandleMMTimerEvent` calls `KeyBoard::PollKeyboard`, and the grid is entered by Enter or a click on the
    account name screen. The searches below find the one line that starts the test, no test line for the keys of 2, 4, 6
    and 8, and the two places that open this box. The same exe's own listing of the function tests the same six digit
    keys before the letters. Called from `TMainForm::HandleMMTimerEvent`. The search for
    `Keys\[DIK_0\] & 0x80\) != 0 \|\|` finds 1 line(s). The search for `Keys\[[3579]\] & 0x80\) != 0 \|\|` finds 0
    line(s). The search for `GetText\(384\)` finds 2 line(s). Reachable with the shipped code: reachable. Any player who
    types on the password grid.
  - What you see: A player who tries to type a password sees the box for most keys, but 2, 4, 6 and 8 give no answer, so
    the box does not come up for them.
- **While a spell waits for its target, a click puts up status text 122 whenever an untargetable npc is in the list.**
  `Sys_mouse.cpp`, `Sys_mouse_HandleClick` (`0x0045def0`).
  - Code:
    `if (target_type.npc_type == NpcType_Unknown1 || target_type.npc_type == NpcType_Unknown2) { if (target_npc.frame_width > 0 && target_npc.frame_height > 0) } else MainForm->animation->interface_controller->ShowStatusMessage(3, MainForm->strings->GetText(122));`
  - What it does: Step by step: - The npcs are tried from the last in the list to the first, and the loop stops at the
    first one hit. - For an npc of one of the two targetable types, its box on screen is tested against the click. - For
    an npc of any other type nothing is tested: the status line is set to `GetText(122)` at once, whether or not the
    click is anywhere near that npc. - So every untargetable npc tried before the loop stops sets the status line, and
    with no npc hit, that is every untargetable npc in the list. The spell is still cast at the npc or player that was
    hit.
  - When it happens: Each left click on the map while `awaiting_target` is set, which `SpellController` does when a
    spell that needs a target is chosen; `TMainForm::FormMouseDown` hands every click to `Sys_mouse_HandleClick`. The
    searches below find the one use of text 122 and the one place that starts waiting for a target. The search for
    `GetText\(122\)` finds 1 line(s). The search for `awaiting_target = true;` finds 1 line(s). Reachable with the
    shipped code: reachable. Any spell aimed on a map that also holds a shop keeper, a guard or any other npc a spell
    can not target.
  - What you see: The status line shows text 122 on clicks that hit a monster, hit a player or hit nothing, as long as
    an untargetable npc sits later in the list than the npc hit, or anywhere in it when none is hit. The cast goes on.
- **On the account name screen, once a name is typed, a click anywhere moves on to the password grid.** `Sys_mouse.cpp`,
  `Sys_mouse_HandleClick` (`0x0045def0`).
  - Code:
    `if (MainForm->animation->GetSubState() == MenuScreen_GridAccountName) { if (MainForm->edits->GetText(1) != "") { MainForm->registry->SetAccountName(MainForm->edits->GetText(1));`
  - What it does: Step by step: - For sub-states 4 and 8 the press is first tested against the OK button's area; for
    sub-state 7 only the name box is tested, for not being empty. - So any press on the screen opens the grid once a
    name is in the box, wherever it lands. - `TMainForm::FormMouseDown` focuses the edit box under the press before it
    calls `Sys_mouse_HandleClick`, so a press on the name box itself also moves on, with the name as it stands. - A
    press on Cancel first opens the grid and then, in the same call, goes back to the main menu.
  - When it happens: Each press on the account name screen with a name typed; `TMainForm::FormMouseDown` hands every
    press to `Sys_mouse_HandleClick`. The search below finds this test and the Enter key's test in
    `KeyBoard::PollKeyboard`, the only other way to the grid. The search for
    `GetSubState\(\) == MenuScreen_GridAccountName\)$` finds 2 line(s). Reachable with the shipped code: reachable. Any
    player who types a name and then clicks, for example on the name box to correct it.
  - What you see: A click meant to place the caret in the name box, or on empty space, opens the password grid with the
    name as typed so far.
- **On the password grid only the right third of the delete key lights up and takes clicks.** `Sys_mouse.cpp`,
  `Sys_mouse_HandleClick` (`0x0045def0`); `Sys_mouse.cpp`, `Mouser::HandleMove` (`0x0045bf28`).
  - Code:
    `if (*mouse >= 558 && *mouse <= 562 && mouse[1] >= 284 && mouse[1] <= 298) { MainForm->sfx->PlaySfx(19); MainForm->animation->menu->DeleteLastLetter(); } if (x >= 558 && x <= 562 && y >= 284 && y <= 298) MainForm->animation->menu->delete_key_lit = true;`
  - What it does: Step by step: - `MenuGraphics` draws the lit delete key at x 548, y 283 from `small_key_rect`, which
    is 15 pixels square, so the key covers x 548 to 562. - `Mouser::HandleMove` lights it, and `Sys_mouse_HandleClick`
    acts on it, only for x 558 to 562. - The other keys light and take clicks over the whole of their drawn area, for
    example the space and select keys at x 514 to 562.
  - When it happens: Each move and click over the delete key on the password grid. The searches below find the two tests
    of the narrow area and the one draw of the key. The search for `558 &&` finds 2 line(s). The search for
    `BltFast\(548, 283` finds 1 line(s). Reachable with the shipped code: reachable. Any player who uses the mouse to
    remove a letter on the password grid.
  - What you see: Clicking the left two thirds of the delete key does nothing, and the key does not light there.
- **Leaving a guild sends the leave request with an empty guild tag, as the tag is cleared one line earlier.**
  `Sys_mouse.cpp`, `Sys_mouse_HandleClick` (`0x0045def0`).
  - Code:
    `MainForm->animation->interface_controller->status->guild_tag = ""; MainForm->animation->interface_controller->status->guild_rank = GUILD_RANK_LOWEST; MainForm->animation->interface_controller->status->guild_name = ""; MainForm->animation->interface_controller->status->guild_rank_name = ""; MainForm->animation->interface_controller->status->MarkDirty(); MainForm->net_client->sender->LeaveGuild(MainForm->animation->guild->npc_session_id, MainForm->animation->interface_controller->status->guild_tag);`
  - What it does: Step by step: - `guild_tag`, the rank, `guild_name` and the rank name are cleared first. -
    `LeaveGuild` is then called with `status->guild_tag`, which is now empty. - `LeaveGuild` appends the tag after the
    four-character session value, so the request ends after the session.
  - When it happens: Each time a player confirms leaving a guild in the guild window, the only call of `LeaveGuild` in
    the tree (searches below). Called from `Sys_mouse_HandleClick`. The search for `LeaveGuild\(` finds 3 line(s). The
    search for `guild_tag = "";` finds 2 line(s). Reachable with the shipped code: reachable. Any guild member who
    leaves through the guild window.
  - What you see: The leave request reaches the server without the guild tag. What the server makes of that the client
    does not show; the client has already cleared its own guild details.
