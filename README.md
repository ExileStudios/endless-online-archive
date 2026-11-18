# Endless Online Archive

Source reconstructions of Endless Online's programs, sorted by release, for the developer community to review and
reference. Each program's source was regenerated from its shipped executable and builds back to it byte for byte. The
source is named and commented. Behaviours of the original that look odd are kept as they are and written up in the
program's `QUIRKS.md`.

## Layout

The archive keeps one folder per release line. Each release in a line is a commit on that folder, so the history of a
folder is the history of the program: `git log -p -- 0.3.x/GameClient` shows what changed from one release to the next.

- `0.0.x/0.0.28/`: the 0.0.28 release, the original engine.
  - `GameClient/`: the game client.
  - `GameServer/`: the game server.
  - `Watchdog/`: the server's companion. It starts the server again when it stops answering and writes its statistics
    pages.
  - `Econfig/`: the client's settings program.
- `0.1.x/GameClient/`: the 0.1.29 client.
- `0.2.x/GameClient/`: the 0.2.29 client.
- `0.3.x/GameClient/`: the 0.3.29 clients, one commit per build: 0.3.29D, D2, E, F, G, H, I, J and K.
- `misc/`: programs that belong to no single client release.
  - `EOSoda/`: a chat helper that turns typed text into drunk-sounding "sodanized" text and can copy it to the
    clipboard.
  - `EoImport/`: reads the server's `.ini` data (skill masters, drops, shops, crafts and so on) and writes it out as pub
    files.
  - `EoPub/`: Eo Pub Studio, the editor for the pub files (items, NPCs, spells and classes).

Each program's folder holds:

- `src/`: the C++ Builder 5 project (`.bpr`, or `Endless.bpg` for a client) with its units (`.cpp`, `.h`), forms
  (`.dfm`) and resources (`.res`);
- `QUIRKS.md`: the behaviours of the original that look odd, each with what the line was written to do and what it
  does instead.

## History

The commits run in release order:

1. The 0.0.28 programs: GameClient, GameServer, Watchdog and Econfig.
2. `misc/EOSoda`.
3. Each later client line begins with a commit that copies the line before it unchanged: 0.1.x starts as 0.0.28, 0.2.x
   as 0.1.29 and 0.3.x as 0.2.29. The commit after that one is the release itself. So every version diff, including
   the first one of a line, shows only what changed in the code.
4. 0.1.29, 0.2.29, then the 0.3.29 builds D to K, each one commit.
5. EoImport and EoPub.

Each release commit's message lists the functions it adds and the ones whose behaviour changed. The same thing has the
same name in every release, so a diff between releases shows only real code changes, not renames.

## Building

The programs are C++ Builder 5 projects. They link Borland's own runtime and form library and, for some programs,
licensed third-party component libraries. None of those libraries are in this repository; supply your own copies.
Only the reconstructed source, the project files, the `QUIRKS.md` pages and this README are tracked (`.gitignore`).
