# Endless Online Archive

Source reconstructions of Endless Online's programs, sorted by release, for the developer community to review and
reference. Each program's source was regenerated from its shipped executable and corresponds to it byte for byte;
it is named and commented, and the behaviours of the original that look odd are written up in its `QUIRKS.md`.

## Layout

- `0.0.x/0.0.28/`: the 0.0.28 release, the original engine.
  - `GameClient/`: the game client.
  - `GameServer/`: the game server.
  - `Watchdog/`: the server's companion: starts the server again when it stops answering and writes its statistics
    pages.
  - `Econfig/`: the client's settings program.

The `0.3.x/` folder arrives with its releases, and `misc/` with the programs that belong to no release.

Each program's folder holds its source and project files in `src/`, and its `QUIRKS.md`.
