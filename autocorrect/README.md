# Autocorrect

There are a lot of words that are prone to being typed incorrectly, due to habit, sequence, or just user error. This feature leverages your firmware to automatically correct these errors, to help reduce typos.

## How does it work?

The feature maintains a small buffer of recent key presses. On each key press, it checks whether the buffer ends in a recognized typo, and if so, automatically sends keystrokes to correct it.

The tricky part is how to efficiently check the buffer for typos. We do not want to spend too much memory or time on storing or searching the typos. A good solution is to represent the typos with a trie data structure. A trie is a tree data structure where each node is a letter, and words are formed by following a path to one of the leaves.

![An example trie](/HL5DP8H.png)

Since we search whether the buffer ends in a typo, we store the trie writing in reverse. The trie is queried starting from the last letter, then second to last letter, and so on, until either a letter does not match or we reach a leaf, meaning a typo was found.

## How do I enable Autocorrection

Add this module to your `keymap.json` modules list:

```json
{
    "modules": ["drashna/autocorrect"]
}
```

This enables the community module and defines `COMMUNITY_MODULE_AUTOCORRECT_ENABLE` and `AUTOCORRECT_ENABLE`

By default, autocorrect is enabled. The status can be toggled, using the `AC_TOGG` keycode. The status is stored in persistent memory.

## Customizing autocorrect library

To provide a custom library, create a text file with the corrections. For instance:

```text
:thier        -> their
fitler        -> filter
lenght        -> length
ouput         -> output
widht         -> width
```

The syntax is `typo -> correction`. Typos and corrections are case insensitive, and any whitespace before or after the typo and correction is ignored. The typo must be only the letters a-z, or the special character `:` representing a word break. The correction may have any non-Unicode ASCII characters.

### Module auto-generation behavior (this module)

This module can auto-generate `autocorrect_data.h` at compile time via `post_rules.mk`.

- Output file is generated at: `$(INTERMEDIATE_OUTPUT)/src/autocorrect_data.h`
- Dictionary search uses `VPATH` plus module/current-directory paths
- Candidate file names are provided by: `AUTOCORRECT_DICT_LIST`
- First matching file wins
- Auto-generation can be disabled with: `AUTOCORRECT_AUTO_GENERATE = no`

Current defaults:

```make
AUTOCORRECT_AUTO_GENERATE ?= yes
AUTOCORRECT_DICT_LIST ?= autocorrection_dict.txt autocorrect_dict.txt
```

The generated header target depends on the resolved dictionary file, so it will regenerate when that dictionary changes.

### Manual generation

You can still generate manually:

```sh
qmk generate-autocorrect-data autocorrect_dictionary.txt
```

This processes the file and produces an `autocorrect_data.h` file with the trie library in your current directory. You can specify keyboard and keymap (for example `-kb planck/rev6 -km jackhumbert`) and it will place the file there instead.

Generated output looks like this:

```c
// :thier        -> their
// fitler        -> filter
// lenght        -> length
// ouput         -> output
// widht         -> width

#define AUTOCORRECT_MIN_LENGTH 5  // "ouput"
#define AUTOCORRECT_MAX_LENGTH 6  // ":thier"

#define DICTIONARY_SIZE 74

static const uint8_t autocorrect_data[DICTIONARY_SIZE] PROGMEM = {85, 7, 0, 23, 35, 0, 0, 8, 0, 76, 16, 0, 15, 25, 0, 0,
    11, 23, 44, 0, 130, 101, 105, 114, 0, 23, 12, 9, 0, 131, 108, 116, 101, 114, 0, 75, 42, 0, 24, 64, 0, 0, 71, 49, 0,
    10, 56, 0, 0, 12, 26, 0, 129, 116, 104, 0, 17, 8, 15, 0, 129, 116, 104, 0, 19, 24, 18, 0, 130, 116, 112, 117, 116,
    0};
```

### Avoiding false triggers

By default, typos are searched within words to find typos within longer identifiers like `maxFitlerOuput`. While this is useful, a consequence is that autocorrection can falsely trigger when a typo is a substring of a correctly spelled word. For instance, if we had `thier -> their` as an entry, it would falsely trigger on (correct, though relatively uncommon) words like `wealthier` and `filthier`.

The solution is to set a word break `:` before and/or after the typo to constrain matching. `:` matches space, period, comma, underscore, digits, and most other non-alpha characters.

|Text             |thier   |:thier  |thier:  |:thier: |
|-----------------|:------:|:------:|:------:|:------:|
|see `thier` typo |matches |matches |matches |matches |
|it is `thiers`   |matches |matches |no      |no      |
|wealthier words  |matches |no      |matches |no      |

`:thier:` is most restrictive, matching only when `thier` is a whole word.

The `qmk generate-autocorrect-data` command can check for entries that would falsely trigger as substrings of correct words. It searches each typo against a dictionary of 25K English words from the `english_words` Python package, provided it is installed:

```sh
python3 -m pip install english_words
```

## Overriding Autocorrect

Occasionally you may actually want to type a typo (for instance, while editing `autocorrect_dict.txt`) without being autocorrected. There are a couple of ways to do this:

1. Begin typing the typo.
2. Before typing the last letter, press and release Ctrl or Alt.
3. Type the remaining letters.

This works because the autocorrection implementation does not understand hotkeys, so it resets itself whenever a modifier other than Shift is held.

Additionally, you can use the `AC_TOGG` keycode to toggle the on/off status for Autocorrect.

### Keycodes

|Keycode                |Aliases  |Description                                   |
|-----------------------|---------|----------------------------------------------|
|`QK_AUTOCORRECT_ON`    |`AC_ON`  |Turns on the Autocorrect feature.             |
|`QK_AUTOCORRECT_OFF`   |`AC_OFF` |Turns off the Autocorrect feature.            |
|`QK_AUTOCORRECT_TOGGLE`|`AC_TOGG`|Toggles the status of the Autocorrect feature.|

## User Callback Functions

### Process Autocorrect

Callback function `bool process_autocorrect_user(uint16_t *keycode, keyrecord_t *record, uint8_t *typo_buffer_size, uint8_t *mods)` is available to customize incoming keycodes and handle exceptions. You can use this function to sanitize input before it is passed to the autocorrect engine.

The default callback in `quantum/process_keycode/process_autocorrect.c` covers most QMK special functions and quantum keycodes, including overriding autocorrect with a modifier other than Shift. `process_autocorrect_user` is weak-defined, so a user implementation can override it.

### Apply Autocorrect

`apply_autocorrect(uint8_t backspaces, const char *str, char *typo, char *correct)` allows additional handling or replacing autocorrection behavior entirely. This passes the number of backspaces needed, replacement substring, and the typo/correct words.

If you return `false`, you must handle backspaces and replacement typing yourself.

Important: `str` points to `PROGMEM` data, so if you send it manually use `send_string_P`, not `send_string` or `SEND_STRING`.

### Autocorrect Status

Additional functions to manipulate autocorrect:

| Function                   | Description                                  |
|----------------------------|----------------------------------------------|
| `autocorrect_enable()`     | Turns Autocorrect on.                        |
| `autocorrect_disable()`    | Turns Autocorrect off.                       |
| `autocorrect_toggle()`     | Toggles Autocorrect.                         |
| `autocorrect_is_enabled()` | Returns true if Autocorrect is currently on. |

## Appendix: Trie binary data format

This section details how the trie is serialized to byte data in `autocorrect_data`. You do not need this to use the feature, but it documents the format.

All autocorrection data is stored in a single flat array `autocorrect_data`. Each trie node is associated with a byte offset into this array, where data for that node is encoded, beginning with root at offset 0.

Kinds of nodes (highest 2 bits of first byte):

- `00`: chain node (single child)
- `01`: branching node (multiple children)
- `10`: leaf node (typo and correction)

Links between nodes are 16-bit little-endian byte offsets.

## Credits

Credit goes to [getreuer](https://github.com/getreuer) for originally implementing this in [Autocorrection design notes](https://getreuer.info/posts/keyboards/autocorrection/#how-does-it-work), and to [filterpaper](https://github.com/filterpaper) for converting the code to use `PROGMEM` and additional improvements.
