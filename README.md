# Shot-Caller-WoW

Real-time WoW Mythic+ shotcaller. Parses the combat log, tracks enemy ability timers, and assigns interrupts/CC to available party members. Callouts are spoken in Discord voice chat via a TTS bot.

## Architecture

```
WoW Combat Log → C++ Engine → TCP Socket → Discord Bot → ElevenLabs TTS → Voice Channel
```

- **C++ engine** — Tail-follows the combat log, identifies players/enemies from spells, generates a time-sorted queue of upcoming ability casts, and dispatches callouts assigning the best available interrupter or CCer.
- **Discord bot** — Python bot that receives callouts over TCP (port 9999) and plays them as TTS audio using ElevenLabs.

## Build

```bash
cmake -B build && cmake --build build
```

## Run

```bash
# C++ engine (pass your WoW logs directory)
./build/ShotCallerWow /path/to/WoW/Logs

# Discord bot (requires ELEVENLABS_API_KEY and DISCORD_BOT_TOKEN env vars)
python discord_bot/main.py
```

## Tests

```bash
cmake --build build && cd build && ctest --output-on-failure
```

## Discord Bot Commands

| Command         | Description                               |
| --------------- | ----------------------------------------- |
| `!join`         | Join your voice channel                   |
| `!leave`        | Leave voice channel                       |
| `!say <text>`   | Speak text via TTS                        |
| `!voice [name]` | Change TTS voice or list available voices |
