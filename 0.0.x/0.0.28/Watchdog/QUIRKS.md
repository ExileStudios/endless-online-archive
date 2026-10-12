# Watchdog: quirks

These are behaviours of the original Watchdog as it shipped. The source keeps every one of them byte for byte, because
changing any of them would change the exe. They are collected here, each with what the line was written to do and what
it does instead, so the comments in `src/` can stay about what the code is for.

## Only the first tag on each line of a page template's list block is filled in

`Stats_guilds.cpp`, `GuildStats::FillListLine` (`0x0041221c`); `Stats_players.cpp`, `PlayerStats::FillListLine`
(`0x0040f574`); `Stats_top100.cpp`, `Top100Stats::FillListLine` (`0x00411670`)

```cpp
                String tag = line.SubString(tag_start, tag_length);
                line.Delete(tag_start, tag_length);
    ...
                i = line.Length();
                in_tag = 1;
```

- **What the function is for:** The `FillListLine` functions of the guild, online and top 100 pages turn one line of a
  template's list block into the line for one guild or player. They replace tags such as `{NAME}` and `{EXP}` with the
  values.
- **What the line was written to do:** Go on scanning the line after a tag, to fill the next one. The evidence: the scan
  is a `for` loop over every character of the line, and `ServerStats::FillTemplateLine` restarts that same loop with
  `i = 1;` after each tag it fills.
- **What it does:** Step by step:
  - The loop looks for `{` and then `}` (codes 123 and 125). At `}` it cuts the tag out of the line and inserts its
    value.
  - `i = line.Length();` then moves the loop index to the end of the line, so the `for` loop ends.
  - Any tag after the first is left in the line as it stands in the template.
- **When it happens:** Every page build while the page generator is on. `BuildPage` calls the line filler once for each
  line of the template's list block, for each guild or player.
  - `GuildStats::FillListLine` is called from `GuildStats::BuildPage` (`FillListLine(template_lines->Strings[j])`).
  - `PlayerStats::FillListLine` is called from `PlayerStats::BuildPage`
    (`FillListLine(by_name, template_lines->Strings[j])`).
  - `Top100Stats::FillListLine` is called from `Top100Stats::BuildPage` (`FillListLine(template_lines->Strings[j])`).
  - Reachable with the shipped code: reachable. Any template whose list block has two tags on one line shows it. The
    templates are files next to the exe (`templates/guilds.eht` and the others), not code.
- **What you see:** For example, a list line written as <td>{NAME}</td><td>{EXP}</td> comes out on the page as
  <td>Ann</td><td>{EXP}</td>. To fill every tag, a template has to put each tag on a line of its own.

## A function of the server page tests the connection count and changes nothing

`Stats_server.cpp`, `ServerStats::CheckConnectionLoad` (`0x00403e24`)

```cpp
    if (connections > max_connections / 2)
        return;
```

- **What the function is for:** `ServerStats` is the server page (`server.html`). `CheckConnectionLoad` is called before
  the page is written.
- **What the line was written to do:** Not shown by the code. The function holds only the test and its `return`, and
  both paths end the function. Where it is called points to a step before the page is written: the constructor ends with
  it, and the generator's cycle calls it at second 11, after the stats request at second 10 and before `GenerateHTML` at
  second 15.
- **What it does:** It tests whether `connections` is above half of `max_connections`. When the test is true it returns;
  when it is false it reaches the end of the function. Either way, nothing is read into the page or written.
- **When it happens:** Once when the server page is created at start, and in each 120-second generator cycle, at second
  11, while a `templates/server.eht` is there.
  - `ServerStats::CheckConnectionLoad` is called from `ServerStats::ServerStats` (`CheckConnectionLoad();`);
    `TMainForm::watchtimerTimer` (`server_stats->CheckConnectionLoad();`).
  - Reachable with the shipped code: reachable. Both calls run in a normal start with the generator on.
- **What you see:** None. The server page shows what the last stats answer from the game server put in.

## Small ones with no effect

- **`Client::SendPacket` returns false whether or not it sent the packet.** `Packetcontrols.cpp`, `Client::SendPacket`
  (`0x0040dcf4`).
  - Code: `int result = 0; result = 0; return result;`
  - What it does: `result` starts at 0, and the only other assignment, in the `catch`, is also 0, so `return result;`
    gives false.
  - When it happens: For every packet the Watchdog sends to the game server. Called from `Client::HandleInit`,
    `PacketOperator::RequestServerStats`, `PacketOperator::RequestTopPlayers`, `PacketOperator::RequestTopGuilds`,
    `PacketOperator::RequestOnlineList`, `PacketOperator::SendPing`. Reachable with the shipped code: reachable. Every
    send goes through it.
  - What you see: None. Each of the six calls stands as a statement, so no caller reads the answer.
- **The licence check reads the PC's name and does not use it.** `Serial.cpp`, `SerialKey::Validate` (`0x00406acc`).
  - Code: `GetComputerNameA(computer_name, &name_size); String computer_name_text = computer_name;`
  - What it does: `GetComputerNameA` fills `computer_name`, which is copied into `computer_name_text`. No later line
    reads `computer_name_text`: the search finds only the line that sets it.
  - When it happens: Once, when `SerialKey` is created at start. Called from `SerialKey::SerialKey`. The search for
    `\bcomputer_name_text\b` finds 1 line(s). Reachable with the shipped code: reachable. Every start runs it.
  - What you see: None. The check uses the drive serial (`GetVolumeInformationA`) and the key.
- **The packet decoder asks whether a stack is empty and drops the answer.** `Packetcontrols.cpp`,
  `Client::DecodeDeinterleave` (`0x0040d698`).
  - Code: `pending.empty();`
  - What it does: `pending.empty();` comes right after `while (!pending.empty())`, the loop that empties `pending`. It
    only returns true, and nothing uses the answer.
  - When it happens: For every packet from the game server. Called from `Client::DecodePacket`. Reachable with the
    shipped code: reachable. Every packet from the game server is decoded by it.
  - What you see: None.
