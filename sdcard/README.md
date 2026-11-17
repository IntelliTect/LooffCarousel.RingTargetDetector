# DFPlayer Mini SD card

Copy the `mp3` folder to the root of a FAT32 microSD card. The DFPlayer plays these with `playMp3Folder(n)`, so the folder name and the 4-digit file names must stay as they are.

| File | Plays when | Sound | Length |
|---|---|---|---|
| `0001.mp3` | Spinning, toss 1 | Single brass bell strike over an organ chord, a nod to the original bell and servo | 1.8 s |
| `0002.mp3` | Spinning, toss 2 | Rising organ, shouted "TWO!!" | 2.0 s |
| `0003.mp3` | Spinning, toss 3 | Faster organ, "THREE of five!!" | 2.2 s |
| `0004.mp3` | Spinning, toss 4 | Organ trill and drum roll, "FOUR OF FIVE!" | 2.2 s |
| `0005.mp3` | Spinning, toss 5 | Band organ finale, screamed "JACKPOT!", screaming goat | 7.0 s |
| `0006.mp3` | Score while not spinning | Short organ chirp | 1.0 s |

Tracks 1-4 are kept under `COMBO_WINDOW_MS` (2500 ms) so a sound finishes before the next toss can extend the combo. Track 5 is long, so the finale animation should run for about 7 s.
