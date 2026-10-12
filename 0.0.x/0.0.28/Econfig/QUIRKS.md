# Econfig: quirks

These are behaviours of the original settings program as it shipped. The source keeps every one of them byte for byte,
because changing any of them would change the exe. They are collected here, each with what the line was written to do
and what it does instead, so the comments in `src/` can stay about what the code is for.

## Values read from setup.ini come back upper case and without spaces, and are saved back that way

`Configuration.cpp`, `Config::LoadConfig` (`0x004055fc`)

```cpp
                    ini_lines->Add(raw_line);
                    ini_keys->Add(UpperCase(key_line));
    ...
            String line = ini_keys->Strings[j];
    ...
                    if (key == "LOGFILE")
                    {
                        for (int value_pos = k + 1; value_pos <= line.Length(); value_pos++)
                            value = value + line[value_pos];
                        log_file = value;
```

- **What the function is for:** `Config::LoadConfig` reads `config/setup.ini` into the settings the window shows.
- **What the line was written to do:** Match the keys whatever their case and spacing, and read each value as written.
  The evidence: next to the upper-cased copy it matches on, `ini_keys`, the function keeps a copy of every line as
  written, `ini_lines`. And `SetValue` writes values into `ini_lines` exactly as given.
- **What it does:** Step by step:
  - Each line goes into `ini_lines` as written (`raw_line`). It also goes into `ini_keys` upper-cased (`UpperCase`),
    with every space (code 32) left out (`key_line`).
  - The key loop reads `line = ini_keys->Strings[j]` and takes each value from that same copy. So every value is upper
    case and has no spaces, `host` and `log_file` included.
  - For the on/off keys, only the O, N and F letters of the value are kept, and the value is compared with `ON`.
- **When it happens:** Every time Econfig starts. `Config::Config` calls `LoadConfig`, and the window's `FormCreate`
  fills its boxes from the result (`ElEdit1` for the host, `ElEdit2` for the log file). Pressing save
  (`SpeedButton1Click`) writes each box back with `SetValue`.
  - `Config::LoadConfig` is called from `Config::Config` (`LoadConfig();`).
  - `Config::SetValue` is called from `TForm1::SpeedButton1Click` (`config->SetValue(`, 41 calls).
  - Reachable with the shipped code: reachable. It happens on every start, for any host or log-file value with a
    lower-case letter or a space.
- **What you see:** For example, with the line "LogFile=my chat.log" in setup.ini, the window shows "MYCHAT.LOG", and
  saving writes "LogFile=MYCHAT.LOG". A host written in lower case comes back in upper case the same way.

## Saving a setting whose line is missing from setup.ini writes nothing for it

`Configuration.cpp`, `Config::SetValue` (`0x00408030`)

```cpp
            if (line[j] == '=')
            {
                if (line_key == UpperCase(name))
                    ini_lines->Strings[i] = name + "=" + value;
            }
```

- **What the function is for:** `Config::SetValue` puts a setting's new value on its line of setup.ini, in memory,
  before `SaveConfig` writes the file.
- **What the line was written to do:** Save every setting the window shows. The evidence: `SpeedButton1Click` calls
  `SetValue` for every one of them, then `SaveConfig`.
- **What it does:** Step by step:
  - The loop goes over the lines. When a line's key, upper-cased, equals `UpperCase(name)`, the line becomes
    `name=value`.
  - When no line matches, nothing is added.
  - `SaveConfig` then writes `ini_lines` as they are.
- **When it happens:** Pressing save, for each setting whose key has no line in setup.ini, for example a file with that
  line deleted.
  - `Config::SetValue` is called from `TForm1::SpeedButton1Click` (`config->SetValue(`, 41 calls).
  - `Config::SaveConfig` is called from `TForm1::SpeedButton1Click` (`config->SaveConfig();`).
  - Reachable with the shipped code: reachable. Any setup.ini that lacks one of the keys the window saves.
- **What you see:** For example, with no "Music=" line in setup.ini, the window shows music on, which is the default
  from `Config::Config`. Switch it off and save: the file still has no Music line, and the next start shows music on
  again.
