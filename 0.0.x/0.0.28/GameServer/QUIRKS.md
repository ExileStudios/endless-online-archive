# GameServer: quirks

These are behaviours of the original server as it shipped. The source keeps every one of them byte for byte, because
changing any of them would change the exe. They are collected here, each with what the line was written to do and what
it does instead, so the comments in `src/` can stay about what the code is for.

## The per-character save in the game tick does not run, because its gate is shut at the top of every tick

`Packets.cpp`, `Packets::Game_Tick` (`0x00416b84`)

```cpp
    save_gate = false;
    ...
                if (save_gate && players->GetOnlineCount() + 600 < (*player)->save_ticks)
                {
                    if (!(*player)->removing)
                        mysql_controls->Mysql_SubmitExec((*player)->account_ident,
                            players->Character_BuildSaveQuery(*player, 1));
                    (*player)->save_ticks = 0;
                    save_gate = false;
                }
```

- **What the function is for:** `Packets::Game_Tick` is the server's game clock. It runs ten times a second, and once a
  second it walks the characters in the game and moves each one's timers on by one.
- **What the line was written to do:** Save a character's whole row once their count of seconds passes 600 plus the
  number of characters in the game, and then start their count again. The evidence: the branch sets `save_ticks` back to
  0 after its save and sets `save_gate` to false, so the gate was there to let one save through per pass.
- **What it does:** Step by step:
  - The first line of every call sets `save_gate` to false, before the loop over the characters.
  - No line sets it to true. The search below finds the declaration, this line, the test, and the line inside the
    branch.
  - So `save_gate && ...` is false for every character, and the save in the branch never runs. That save is
    `players->Character_BuildSaveQuery(*player, 1)`, queued for the database thread by
    `mysql_controls->Mysql_SubmitExec`.
  - `save_ticks` starts at 0 when the connection's `Player` is made, and goes up by one each second while a character is
    in the game. Its reset in the branch does not run either.
  - `players->GetOnlineCount()` gives `online_count`, the number of characters in the game: `UpdatePeakOnline` counts
    the characters with `logged_in` set into it at each game entry, and `Players::Remove` takes one off.
- **Evidence, from the code and not from the names:**
  - the save: `Players.cpp:287` `String query = "UPDATE endl_characters SET ";` The query the branch would send updates
    the character's row of `endl_characters`.
  - the number of characters: `MainForm.cpp:240`
    `caption.Insert(IntToStr(players->GetOnlineCount()) + " players", caption.Length() + 1);` The window caption shows
    the same number followed by " players".
  - the number of characters: `Players.cpp:1498` `online_count++;` It is counted up once for each character with
    `logged_in` set.
  - the seconds: `MainForm.cpp:133` `timer->Interval = 10;` The timer fires every 10 ms; `TGUI::timerTimer` calls
    `Game_Tick` on every tenth firing, and `Game_Tick` walks the characters on every tenth call.
- **When it happens:** On every pass of the loop, once a second, for every character with `logged_in` set.
  - `Packets::Game_Tick` is called from `TGUI::timerTimer` (`server_ctrl->Game_Tick();`).
  - `Players::Character_BuildSaveQuery` is called from `Packets::Game_Tick`
    (`players->Character_BuildSaveQuery(*player, 1)`); `Players::Remove` (`Character_BuildSaveQuery(player, 0)`).
  - Search of the tree for `\bsave_gate\b`: 4 line(s), Packets.cpp:112, Packets.cpp:183, Packets.cpp:189, Packets.h:272.
  - Reachable with the shipped code: never reached. The branch's test starts with `save_gate`, and the search shows that
    the only assignments to it are the two `= false` lines.
- **What you see:** A character's whole row is saved only when the character leaves the game: `Players::Remove` queues
  the save for the database thread. That row holds their place, stats, inventory, bank, spells and quests. When the
  server shuts down, it waits until every character has been removed, so all of them are saved then. For example, a
  player who plays for two hours is saved once, when they log out.

## When a partner leaves during the vows, the wedding countdown is compared with 1 instead of set to 1

`Weddings.cpp`, `WeddingController::Tick` (`0x0052efa0`)

```cpp
            if ((*it)->countdown == 18)
        {
                if (BothPresent(*it))
                    BroadcastPriestLine(*it,
                        "Let these rings be given and received as a token of your affection, sincerity and trust in "
                        "one another.");
                else
                    (*it)->countdown == 1;
            }
```

- **What the function is for:** `WeddingController::Tick` runs each wedding once a second. It counts the wedding's
  `countdown` down, and the priest says each line of the ceremony at its second.
- **What the line was written to do:** Set `countdown` to 1 when `BothPresent` finds a partner missing. The ceremony
  would then jump to its last second: "Congratulations to the couple!" at `countdown == 1`, then the wedding ends at 0.
  The evidence: the line stands alone as a statement in the `else` of a `BothPresent` test, where only an assignment
  does anything. Every other change of the countdown in the unit uses `=` (`countdown = 20`, `countdown = 25`,
  `countdown = 26`).
- **What it does:** Step by step:
  - Step 3 is the vows, after both partners have confirmed in `WeddingController::Confirm`. At `countdown` 18, 7 and 6
    it tests `BothPresent(*it)`, which is true only while both partners are online and on the wedding's map.
  - When the test is false, `(*it)->countdown == 1;` compares the countdown with 1 and throws the answer away, so
    `countdown` keeps its value.
  - The countdown goes on down by one a second. The lines that need both partners are left out: at 18, "Let these rings
    be given ..."; at 7, the ring effect; at 6, "... have consented together in marriage ..." and its effect.
  - The line at 12, "Please place these rings on eachothers finger..", does not test `BothPresent`, so it is still said.
    At 1 the priest says "Congratulations to the couple!".
- **When it happens:** In the last step of a wedding at a priest, when a partner logs out or leaves the map before
  second 18, 7 or 6.
  - `WeddingController::Tick` is called from `TGUI::timerTimer` (`weddings->Tick();`).
  - Reachable with the shipped code: reachable. `TGUI::timerTimer` calls `weddings->Tick()` once in 100 ticks of its 10
    ms timer, and `BothPresent` is false as soon as a partner is offline or on another map.
- **What you see:** The players still on the map hear the rest of the ceremony, without the absent partner's parts. It
  ends with "Congratulations to the couple!" at its usual second, up to 17 seconds later, instead of at once. The
  marriage is unaffected: `Confirm` already set both partner names and gave each partner item 374.

## A map id of 0 or less is compared with 1 instead of set to 1, so the tile lookup would read `maps[-1]`

`Mapcontrol.cpp`, `MapContainer::GetTileSpecObject` (`0x0047c114`); `Packets.cpp`, `Packets::Chair_Execute`
(`0x00466fcc`)

```cpp
    if (map_id <= 0)
        map_id == 1;
    for (spec_iter = maps[map_id - 1].tile_specs.begin(); spec_iter != maps[map_id - 1].tile_specs.end(); spec_iter++)
    ...
            if (player->map_id < 1 || (int)map_control->maps.size() < player->map_id)
                return true;
```

- **What the function is for:** `MapContainer::GetTileSpecObject` returns the special-tile record at x, y of a map, from
  that map's `tile_specs` list.
- **What the line was written to do:** Fall back to map 1 when `map_id` is 0 or less, so that the `maps[map_id - 1]`
  reads below stay inside the list. The evidence: the `if (map_id <= 0)` guard holds one statement, `map_id == 1;`,
  which only does something as `map_id = 1;`.
- **What it does:** Step by step:
  - `maps` is the server's `vector` of loaded maps, and map n is `maps[n - 1]`.
  - With `map_id` 0, `map_id == 1;` compares 0 with 1 and throws the answer away. `map_id` stays 0.
  - The loop then reads `maps[map_id - 1]`, which is `maps[-1]`: the space of one map record before the start of the
    vector's storage. A `vector` does not check its index.
  - The loop walks that record's `tile_specs` until it finds x, y or reaches the end, and `return *spec_iter;` copies
    the record the iterator stands on.
- **When it happens:** Only when a caller passes a `map_id` of 0 or less. There is one caller, `Packets::Chair_Execute`,
  which runs when a player sits on a chair. It returns before the call when `player->map_id` is below 1 or above the
  number of maps.
  - `MapContainer::GetTileSpecObject` is called from `Packets::Chair_Execute`
    (`GetTileSpecObject(player->map_id, x, y)`).
  - Reachable with the shipped code: never reached. The call search finds one call, in `Packets::Chair_Execute`, and
    that function's guard returns first on any `map_id` below 1.
- **What you see:** None with the shipped code, because no call passes 0 or less. If a call did, `maps[-1]` would be the
  memory just before the vector's storage on the heap. That is the memory manager's header for the block, then whatever
  lies before it, not a map record. What the loop and the copy would read there is not defined.

## A spell list longer than 255 characters is saved whole in `skillblob`, and its tail is saved again in `skillblob2`

`Players.cpp`, `Players::Character_BuildSaveQuery` (`0x0040902c`)

```cpp
    if (invblob.Length() > 255)
    {
        invblob_tail = invblob.SubString(256, invblob.Length() - 255);
        invblob.Delete(256, invblob.Length() - 255);
    }
    ...
    if (skillblob.Length() > 255)
    {
        skillblob_tail = skillblob.SubString(256, skillblob.Length() - 255);
        skillblob.Delete(256, invblob.Length() - 255);
    }
```

- **What the function is for:** `Players::Character_BuildSaveQuery` builds the UPDATE query that saves a character's
  row. A long list goes into two columns: its first 255 characters in one, the rest in the second. The pairs are
  `invblob` and `invblob2` for the inventory, `invblob3` and `invblob4` for the bank, `skillblob` and `skillblob2` for
  the spells, and `questblob` and `questblob2` for the quests.
- **What the line was written to do:** Cut `skillblob` to its first 255 characters after copying the rest into
  `skillblob_tail`, the way the inventory and bank lists next to it are cut. The evidence: the sibling lines
  `invblob.Delete(256, invblob.Length() - 255);` and `bankblob.Delete(256, bankblob.Length() - 255);` each use their own
  list's length.
- **What it does:** Step by step:
  - `skillblob` is the character's spell list as text: `id:level:` for each spell, then `EOF`. When it is longer than
    255 characters, `skillblob_tail` gets its characters from 256 on.
  - `Delete(256, invblob.Length() - 255)` takes its count from `invblob`, the inventory list. The lines above have
    already cut that list to at most 255 characters, so the count is 0 or less.
  - `Delete(index, count)` returns at once when the count is not positive, so `skillblob` stays whole.
  - The query writes the whole list to `skillblob`, and its part from character 256 on to `skillblob2`.
- **Evidence, from the code and not from the names:**
  - the list being cut: `Players.cpp:360` `query.Insert(",skillblob = '" + skillblob + "'", query.Length() + 1);` The
    text `skillblob` is written to the `skillblob` column.
  - the list being cut: `Players.cpp:361` `query.Insert(",skillblob2 = '" + skillblob_tail + "'", query.Length() + 1);`
    Its tail, `skillblob_tail`, is written to the `skillblob2` column.
  - the list being cut: `Packets.cpp:7123`
    `newplayer->skillblob = mysql_controls->Db_GetString("skillblob") + mysql_controls->Db_GetString("skillblob2");` At
    login the two columns are read back and joined, `skillblob` first.
  - the list whose length is used: `Players.cpp:350` `query.Insert(",invblob = '" + invblob + "'", query.Length() + 1);`
    The text `invblob` is written to the `invblob` column, so the count comes from a different column's list.
  - the list whose length is used: `Players.cpp:267` `if (invblob.Length() > 255)` The lines above cut `invblob` to at
    most 255 characters before the spell list is cut.
- **When it happens:** Every time a character is saved, which is in `Players::Remove`, when the character leaves the
  game. The other caller, in `Packets::Game_Tick`, sits in a branch that never runs (the first entry above).
  - `Players::Character_BuildSaveQuery` is called from `Packets::Game_Tick`
    (`players->Character_BuildSaveQuery(*player, 1)`); `Players::Remove` (`Character_BuildSaveQuery(player, 0)`).
  - Reachable with the shipped code: unknown. It needs a spell list longer than 255 characters. A spell like 12:3: takes
    5 characters, so that is 51 spells or more. Whether one character can learn that many is decided by the spell file
    and the skill masters' lists, which are data, not code.
- **What you see:** The database row gets the list's tail twice: once at the end of `skillblob`, and again in
  `skillblob2`. At login, `Db_GetString("skillblob") + Db_GetString("skillblob2")` joins the two columns, so the joined
  text repeats every spell after character 255. The exception is a `skillblob` column that holds fewer characters, which
  the code does not show.

## `ResetMap` leaves a kept map's file text in memory, and reads the next map's record to do it

`Mapcontrol.cpp`, `MapContainer::ResetMap` (`0x00481e0c`)

```cpp
    maps[map_id - 1].width = 0;
    maps[map_id - 1].height = 0;
    maps[map_id - 1].buf.Delete(0, maps[map_id].buf.Length());
```

- **What the function is for:** `MapContainer::ResetMap` empties a map's tiles, warps, chests, ground items and npcs
  before `ReloadMap` reads its file again.
- **What the line was written to do:** Empty `buf`, the copy of the map's file kept in memory (for the start map, or for
  every map when `memorymap` is on). The evidence: every other line of the function empties one of the map's lists or
  zeroes one of its fields.
- **What it does:** Step by step:
  - String positions start at 1, and `Delete` returns at once for a position below 1. So `Delete(0, ...)` removes
    nothing, and `buf` keeps the old file text.
  - The count, `maps[map_id].buf.Length()`, is read from the next map in the list: `maps[map_id]` is map `map_id + 1`.
    For the last map, it is one record past the end of the vector.
  - Next, `ReloadMap` calls `ParseMapFile`. That reads `maps/<id>.emf` from disk again and, for a kept map, sets `buf`
    to the new text.
- **Evidence, from the code and not from the names:**
  - the map being reset: `Mapcontrol.cpp:790` `maps[map_id - 1].width = 0;` Every other line of the function works on
    `maps[map_id - 1]`, the record of map `map_id`.
  - the map being reset: `Mapcontrol.cpp:810` `file_name.Insert("./maps/", 0);` `ReadRawFile` reads
    `maps[map_id - 1].buf` for map `map_id`, and when it is empty reads the file `./maps/` + the id + `.emf` instead.
  - the record whose length is read: `Mapcontrol.cpp:792` `maps[map_id - 1].buf.Delete(0, maps[map_id].buf.Length());`
    The count is read from `maps[map_id]`, one record further on than every other line of the function.
- **When it happens:** A player at the top admin level, 4 (`AdminLevel_HighGameMaster`), types `$r map` to reload the
  map they stand on. `Packets::Player_HandlePacket` calls `ReloadMap(player->map_id)`, which checks that the id is
  between 1 and the number of maps, then calls `ResetMap`.
  - `MapContainer::ResetMap` is called from `MapContainer::ReloadMap` (`ResetMap(map_id);`).
  - `MapContainer::ReloadMap` is called from `Packets::Player_HandlePacket` (`ReloadMap(player->map_id)`).
  - Reachable with the shipped code: reachable. Any `$r map` from a player at admin level 4 runs it. The read past the
    vector's end happens on the last map of the list.
- **What you see:** When the file reads back, nothing shows, because the kept text is replaced by the new one. The
  reload can stop early, when the file is gone or no longer starts `EMF`. Then `ResetMap` has left the map empty and
  `ReloadMap` returns false, but `buf` still holds the old file text. `ReadRawFile` hands that old text to every client
  that asks for the map's file. On the last map, the length read from past the end is not defined, but nothing uses it,
  because `Delete` returns before it looks at the count.

## A member over the kill limit sets the share to 0 for every later member and for the killer

`Packets.cpp`, `Packets::Party_ShareExp` (`0x0046690c`)

```cpp
            if (settings->GetMaxKills())
            {
                if (kill_counters->IncrementAndGet(member->name) > settings->GetMaxKills())
                    exp = 0;
            }
            member->experience += exp;
```

- **What the function is for:** `Packets::Party_ShareExp` splits an npc kill's experience among the killer's party
  members on the same map. It gives each member their share, and returns the share the killer keeps.
- **What the line was written to do:** Give 0 to the one member who is past `maxkills` for the day. The evidence: the
  test reads that member's own count (`IncrementAndGet(member->name)`), and the 0 is set inside it, for that member.
- **What it does:** Step by step:
  - `exp` holds one share, the same for everybody. It is the npc's experience, plus one share's bonus when three or more
    members are on the map, divided by the members on the map.
  - The loop goes through `party_ids` in order. With `maxkills` set, each other member on the map has the kill counted
    by `IncrementAndGet`. When a count passes `maxkills`, the code sets `exp = 0`, then adds `exp` to that member's
    experience.
  - `exp` is not set back. So every member after that one in `party_ids` also gets 0, and `return exp;` hands 0 back as
    the killer's share.
- **Evidence, from the code and not from the names:**
  - the kill limit: `Settings.cpp:239` `max_kills = ReadIniInt("maxkills", max_kills);` The limit is the `maxkills` key
    of the settings file, and `Settings::GetMaxKills` returns `max_kills`.
  - the member's count: `Killcounters.cpp:41` `int bucket = name[1] - 97;` `IncrementAndGet` files the name under its
    first letter, adds one to that name's count, and returns the count.
- **When it happens:** When a party member kills an npc, `maxkills` in the server settings is above 0, and at least one
  other member on the map has passed the limit today.
  - `Packets::Party_ShareExp` is called from `Packets::Attack_Execute`
    (`exp_gain = Party_ShareExp(attacker, exp_gain);`); `Packets::Spell_Execute`
    (`exp_gain = Party_ShareExp(caster, exp_gain);`).
  - Reachable with the shipped code: reachable. Both kill paths call it for every npc killed by a player in a party, and
    `maxkills` is read from the settings file.
- **What you see:** For example, a party lists A, B and C in that order, and C kills an npc worth 300. With three
  members on the map, each share is (300 + 100) / 3 = 133. If B is past the limit, A gets 133, B gets 0, and C, the
  killer, also gets 0. If A is past the limit instead, B gets 0 too. The party packet sent to the members lists those
  0s.

## Learning a spell checks its intelligence requirement against wisdom, and its wisdom requirement against intelligence

`Packets.cpp`, `Packets::Player_HandlePacket` (`0x0041794c`)

```cpp
            if (player->adj_strength < skill.str_requirement)
                return true;
            if (player->adj_intelligence < skill.wis_requirement)
                return true;
            if (player->adj_wisdom < skill.int_requirement)
                return true;
            if (player->adj_agility < skill.agi_requirement)
                return true;
```

- **What the function is for:** The `OP_SKILL` / `AC_GET` branch of
  `Packets::Player_HandlePacket` teaches a player a spell at a skill master. It first checks the spell's class, level
  and stat requirements.
- **What the line was written to do:** Compare each stat with its own requirement, as the lines around it do:
  `adj_strength` with `str_requirement`, `adj_agility` with `agi_requirement`, `adj_constitution` with
  `con_requirement`. The evidence is listed below. The pub files of skill masters, items and classes all keep the six
  stats in one order, two bytes each, and everywhere else the second of them is used as intelligence and the third as
  wisdom.
- **What it does:** Step by step:
  - `skill` is the skill master's entry for the spell, read from `pub/dsm001.emf`. Its six stat requirements are the
    two-byte numbers at positions 17, 19, 21, 23, 25 and 27 of the spell's 28-byte record (counting from 1), in that
    order.
  - `adj_intelligence` is the character's intelligence with the equipment's bonus added, and `adj_wisdom` their wisdom.
  - The check compares `adj_intelligence` with the number at position 21, and `adj_wisdom` with the number at position
    19. The two are crossed only here: the item check for equipping compares `adj_intelligence` with the item record's
    second stat requirement, and the class bonus adds the class record's second stat to `adj_intelligence`.
  - When either is lower, the branch returns `true` without a reply. The class check above sends a
    `SkillMasterReply_WrongClass` reply, but the stat checks send nothing.
- **Evidence, from the code and not from the names:**
  - intelligence: `Packets.cpp:7074` `newplayer->base_intelligence = mysql_controls->Db_GetInt("stat_intelligence");`
    The character's intelligence comes from the `stat_intelligence` column; `Player::CalculateAdjStats` makes
    `adj_intelligence` from it (`adj_intelligence = base_intelligence + equip_intelligence_bonus;`).
  - intelligence: `Learnvalues.cpp:64` `DecodeNumber(data.SubString(17, 2)), DecodeNumber(data.SubString(19, 2)),` The
    number at position 19 is the second stat requirement of the spell, and `AddSkill` stores it as `int_requirement`.
  - intelligence: `Itemvalues.cpp:98`
    `DecodeNumber(data.SubString(namelen + 44, 2)), DecodeNumber(data.SubString(namelen + 46, 2)),` The item record's
    second stat requirement, at position `namelen + 44`, becomes `intelligence_requirement`, which the equip check
    compares with `adj_intelligence` (`if (player->adj_intelligence < equip_item->intelligence_requirement)`).
  - intelligence: `Classvalues.cpp:77`
    `DecodeNumber(data.SubString(namelen + 6, 2)), DecodeNumber(data.SubString(namelen + 8, 2)),` The class record's
    second stat, at position `namelen + 6`, becomes `intl`, which `Player_CalculateStats` adds to `adj_intelligence`.
  - wisdom: `Packets.cpp:7073` `newplayer->base_wisdom = mysql_controls->Db_GetInt("stat_wisdom");` The character's
    wisdom comes from the `stat_wisdom` column, and becomes `adj_wisdom` the same way.
  - wisdom: `Learnvalues.cpp:65` `DecodeNumber(data.SubString(21, 2)), DecodeNumber(data.SubString(23, 2)),` The number
    at position 21 is the spell's third stat requirement, stored as `wis_requirement`. The item record's third, at
    `namelen + 46`, is compared with `adj_wisdom`, and the class record's third, at `namelen + 8`, is added to
    `adj_wisdom`.
  - wisdom: `Learnvalues.cpp:179` `data.Insert(EncodeNumber(it->wis_requirement, 2), data.Length() + 1);` The list sent
    to the client writes the requirements back in file order, `int_requirement` then `wis_requirement`.
- **When it happens:** When a player asks a skill master to teach a spell whose intelligence and wisdom requirements
  differ.
  - `Packets::Player_HandlePacket` is called from `Packets::Server_ClientRead` (`Player_HandlePacket(player,`).
  - Reachable with the shipped code: reachable. Every spell a skill master teaches goes through these checks.
- **What you see:** For example, a spell that needs intelligence 10 and wisdom 0 is taught to a character with wisdom 10
  and intelligence 0. It is refused to a character with intelligence 10 and wisdom 0. A refused player gets no message:
  the spell just does not appear.

## The 27th kill list, for names that do not start with a to z, is left out of the end-of-day clear and the save

`Killcounters.cpp`, `KillCounters::Clear` (`0x0053e5d0`); `Killcounters.cpp`, `KillCounters::Save` (`0x0053f2d0`);
`Killcounters.cpp`, `KillCounters::IncrementAndGet` (`0x0053e784`)

```cpp
    if (bucket < 0 || bucket > 26)
        bucket = 26;
    ...
    for (int i = 0; i < 26; i++)
        buckets[i].clear();
    ...
    for (i = 0; i < 26; i++)
```

- **What the function is for:** `KillCounters` counts each character's npc kills for the day, for the `maxkills` limit.
  It keeps 27 lists, split by the name's first letter. `Clear` empties the lists at the end of the day, and `Save`
  writes them to `cache/kills.chk` at shutdown.
- **What the line was written to do:** Go over all 27 lists. The evidence: `IncrementAndGet`, `Add` and `Get` put every
  name that does not start with a to z into `buckets[26]`, the 27th list, but these two loops stop before 26.
- **What it does:** Step by step:
  - `IncrementAndGet` lowercases the name and picks list `name[1] - 97`, so a is 0 and z is 25. Anything outside 0 to 26
    goes to list 26.
  - `Clear` and `Save` loop `i` from 0 to 25, so list 26 is neither cleared nor written. `Clear` also deletes
    `cache/kills.chk`.
- **Evidence, from the code and not from the names:**
  - the 27 lists: `Killcounters.cpp:43` `bucket = 26;` A first letter outside a to z (`name[1] - 97` outside 0 to 26)
    files the name in list 26.
  - the 27 lists: `Killcounters.cpp:25` `for (int i = 0; i < 26; i++)` The end-of-day clear stops at 25.
  - the 27 lists: `Killcounters.cpp:129` `string_list->SaveToFile("./cache/kills.chk");` The save writes the lists its
    loop went over to `cache/kills.chk`.
  - the end of the day: `Packets.cpp:220` `if (stamp > 8620000)` The clear runs once the time of day passes 23:56:39
    (`stamp` is the hundredths of a second since midnight, plus 100), while `GetMaxKills` is above 0.
- **When it happens:** Only for a name that does not start with a to z, and no character name can. Creating a character
  with any character other than a letter bans the ip for 4 hours (the `IsAlphabeticText` check in the create branch).
  Entering the game refuses a name that is not letters only, except `vult-r`, which starts with v. The only other way in
  is a line of `cache/kills.chk`, which `Init` reads at start.
  - `KillCounters::Clear` is called from `Packets::Game_Tick` (`kill_counters->Clear();`).
  - `KillCounters::Save` is called from `Packets::Game_Tick` (`kill_counters->Save();`).
  - `KillCounters::IncrementAndGet` is called from `Packets::Party_ShareExp` (`IncrementAndGet(member->name)`);
    `Packets::Attack_Execute` (`IncrementAndGet(attacker->name)`); `Packets::Spell_Execute`
    (`IncrementAndGet(caster->name)`).
  - `mySQLdb::IsAlphabeticText` is called from `Packets::Player_HandlePacket` (`IsAlphabeticText(character_name)`);
    `Packets::Player_HandlePacket` (`IsAlphabeticText(player->name)`); `Packets::Player_HandlePacket`
    (`IsAlphabeticText(tag_upper)`).
  - Reachable with the shipped code: never reached. Every character in the game has a name of letters only (the two
    `IsAlphabeticText` checks on names; the third checks a guild tag). The three kill paths call `IncrementAndGet` with
    a character's name, so they never pick list 26. Only a hand-edited `cache/kills.chk` line can fill it.
- **What you see:** None in play. A line of `cache/kills.chk` whose name starts with a digit or a sign is loaded into
  list 26 at start. It is kept through the end-of-day clear, and dropped from the file at the next shutdown's save.

## The character-pick reply sends wisdom before intelligence, the other way round from the seven other stat packets

`Packets.cpp`, `Packets::Player_HandlePacket` (`0x0041794c`)

```cpp
                welcome_reply.Insert(EO_EncodeNumber(welcome_select_slot->adj_strength, 2), welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(welcome_select_slot->adj_wisdom, 2), welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(welcome_select_slot->adj_intelligence, 2),
                    welcome_reply.Length() + 1);
```

- **What the function is for:** The `OP_PLAY` / `AC_REQUEST` branch of
  `Packets::Player_HandlePacket` answers a player who picks a character: the map's and the pub files' ids and sizes,
  then the character.
- **What the line was written to do:** Send the six stats in the order the other packets with them use: strength,
  intelligence, wisdom, agility, constitution, charisma. The evidence is below: the seven other packets that carry them
  write `adj_intelligence` right before `adj_wisdom`.
- **What it does:** Step by step:
  - The reply writes `adj_strength`, then `adj_wisdom`, then `adj_intelligence`, then `adj_agility`, `adj_constitution`
    and `adj_charisma`, two bytes each.
  - The seven other packets with the six stats write intelligence second and wisdom third: Packets.cpp:918, 2549, 3090,
    3164, 5557, 5639 and 6935 (the evidence shows the first of them). The two searches below find these eight packets'
    lines for each stat: this reply's and the seven others'.
  - No line of the tree reads this reply back. The client reads it, so which number it shows as which stat is not in
    this code.
- **Evidence, from the code and not from the names:**
  - intelligence: `Packets.cpp:7074` `newplayer->base_intelligence = mysql_controls->Db_GetInt("stat_intelligence");`
    The character's intelligence comes from the `stat_intelligence` column, and `adj_intelligence` from it.
  - wisdom: `Packets.cpp:7073` `newplayer->base_wisdom = mysql_controls->Db_GetInt("stat_wisdom");` The character's
    wisdom comes from the `stat_wisdom` column, and `adj_wisdom` from it.
  - the order of the other packets: `Packets.cpp:918`
    `lookup_reply.Insert(EO_EncodeNumber(lookup_target->adj_intelligence, 2),` The `lookup_reply`, the first of the
    seven listed, writes `adj_intelligence` and then `adj_wisdom`.
- **When it happens:** Every time a player picks a character on the character list.
  - `Packets::Player_HandlePacket` is called from `Packets::Server_ClientRead` (`Player_HandlePacket(player,`).
  - Search of the tree for `EncodeNumber\([^)]*adj_intelligence, 2\)`: 8 line(s), Packets.cpp:918, Packets.cpp:1713,
    Packets.cpp:2549, Packets.cpp:3090, Packets.cpp:3164, Packets.cpp:5557, Packets.cpp:5639, Packets.cpp:6935.
  - Search of the tree for `EncodeNumber\([^)]*adj_wisdom, 2\)`: 8 line(s), Packets.cpp:920, Packets.cpp:1712,
    Packets.cpp:2550, Packets.cpp:3092, Packets.cpp:3166, Packets.cpp:5558, Packets.cpp:5640, Packets.cpp:6936.
  - Reachable with the shipped code: reachable. Every character pick sends the reply.
- **What you see:** Not decided by this code. If the client reads the fields in the order of the other packets, it shows
  the character's wisdom as intelligence and their intelligence as wisdom, until a later packet with the stats (a stat
  point, an equip, a level) sends them again. The client's reader is not in this tree.

## The item loader reads each item's strength requirement from position 41 of its record, inside the class requirement

`Itemvalues.cpp`, `ItemValues::LoadItems` (`0x0047849c`)

```cpp
                        DecodeNumber(data.SubString(namelen + 37, 1)), DecodeNumber(data.SubString(namelen + 38, 2)),
                        DecodeNumber(data.SubString(namelen + 40, 2)), DecodeNumber(data.SubString(namelen + 41, 2)),
                        DecodeNumber(data.SubString(namelen + 44, 2)), DecodeNumber(data.SubString(namelen + 46, 2)),
```

- **What the function is for:** `ItemValues::LoadItems` reads the item files into the item list, one `AddItem` call
  per item record.
- **What the line was written to do:** Read the strength requirement from the two bytes after the class requirement, at
  `namelen + 42`. The evidence: every field of the call starts where the one before it ends (`namelen + 38, 2`,
  `namelen + 40, 2`, then `namelen + 44, 2`, `namelen + 46, 2`), and nothing else is read from `namelen + 42`.
- **What it does:** Step by step:
  - `strength_requirement` is read with `DecodeNumber(data.SubString(namelen + 41, 2))`: the second byte of the class
    requirement, then the first byte of the field at `namelen + 42`.
  - `ItemValues::DecodeNumber` stops at a byte of 254 (`EO_NUMBER_EMPTY`) or 0. Otherwise it adds the first byte less
    one, and 253 times the second byte less one.
  - With the class requirement's second byte 254, the strength requirement reads 0. With that byte 1 (a digit of 0), it
    reads 253 times the strength the item needs: 10 reads as 2530.
- **Evidence, from the code and not from the names:**
  - the fields around it: `Itemvalues.cpp:98`
    `DecodeNumber(data.SubString(namelen + 44, 2)), DecodeNumber(data.SubString(namelen + 46, 2)),` The two fields after
    it start at 44 and 46, two bytes apart.
- **When it happens:** At start, for every item, while `ItemValues::ItemValues` loads the item files. The equip check
  then compares `adj_strength` with it.
  - `ItemValues::LoadItems` is called from `ItemValues::ItemValues` (`LoadItems();`).
  - Reachable with the shipped code: unknown. It runs for every item. What it reads depends on bytes of the item files,
    which are data: the class requirement's second byte (254 or 1) and the strength requirement.
- **What you see:** With the class requirement's second byte 254, every item's `strength_requirement` is 0, and any
  character can equip it whatever their strength. With that byte 1, an item that needs strength 10 needs 2530. Which
  bytes the shipped item files hold is not in this code.

## Small ones with no effect

- **The licence check reads the PC's name and does not use it.** `Serial.cpp`, `SerialKey::Validate` (`0x00412d48`).
  - Code: `GetComputerNameA(computer_name, &csize); String cn = computer_name;`
  - What it does: `GetComputerNameA` fills `computer_name`, which is copied into `cn`. No later line reads `cn`: the
    search finds only the line that sets it.
  - When it happens: Once, when `SerialKey` is created at start. Called from `SerialKey::SerialKey`. The search for
    `\bcn\b` finds 1 line(s). Reachable with the shipped code: reachable. Every start runs it.
  - What you see: None. The check uses the drive serial (`GetVolumeInformationA`) and the key.
- **The packet decoder asks whether a stack is empty and drops the answer.** `Packets.cpp`,
  `Packets::EO_Decode_Deinterleave` (`0x00472414`).
  - Code: `pending.empty();`
  - What it does: `pending.empty();` comes right after `while (!pending.empty())`, the loop that empties `pending`. It
    only returns true, and nothing uses the answer.
  - When it happens: For every packet a client sends. Called from `Packets::Player_HandlePacket`. Reachable with the
    shipped code: reachable. Every packet from a client is decoded by it.
  - What you see: None.
- **The shutdown check has no `return` when the database thread is still running, and gives 0 there.**
  `Mysqlcontrols.cpp`, `mySQLdb::Database_TryShutdown` (`0x00477258`).
  - Code:
    `if (task_queue->job_queue.size() > 0) return 0; else if (task_queue->last_player_id > 0) return 0; else if (worker_thread->Suspended) { worker_thread->Terminate(); return 1; } }`
  - What it does: `task_queue->job_queue.size() > 0` and `task_queue->last_player_id > 0` each `return 0;` while the
    thread has work, and when `worker_thread->Suspended` is true the thread is told to end and the function returns 1.
    When it is false, the `if` chain ends and so does the function, with no `return`. The C++ language leaves the result
    undefined there, and the compiler warns about it (W8070, "Function should return a value"). In the shipped program a
    `char` result comes back in the AL register, and on this path AL still holds the 0 that `job_queue.size()` returned,
    so the function gives 0.
  - When it happens: While the server shuts down: once every character has left, `Packets::Game_Tick` calls it on each
    tick until it gives true. Called from `Packets::Game_Tick`. The search for `\bDatabase_TryShutdown\b` finds 3
    line(s). Reachable with the shipped code: reachable. A shutdown whose database thread has not yet suspended when the
    last character leaves takes this path, on each tick until the thread suspends.
  - What you see: None. The shutdown waits one more tick, as `return 0;` would make it. Once the thread has suspended,
    the check returns 1, and the server writes its cache and counters and quits.
