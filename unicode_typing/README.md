# Unicode Typing modes

This enables the ability to type in unicode text, without using custom keycodes.  This intercepts and replaces keypresses and uses the Unicode system to output the text.

Add the following to the list of modules in your `keymap.json` to enable this module:

```json
{
    "modules": ["drashna/unicode_typing"]
}
```

After this, your keymap can add one of the many typing modes to type in unicode text.

!> Note that this enables the shared Unicode code, but not any of the main features.

## Functions

* `set_unicode_tying_mode(uint8_t mode)` - sets the unicode typing mode.  See below for list of modes.
* `get_unicode_tying_mode()` - Gets the current unicode typing mode.

## Information

The list of unicode modes:

```c
enum {
    UCTM_NO_MODE,
    UCTM_WIDE,
    UCTM_SCRIPT,
    UCTM_BLOCKS,
    UCTM_REGIONAL,
    UCTM_AUSSIE,
    UCTM_ZALGO,
    UCTM_SUPER,
    UCTM_COMIC,
    UCTM_FRAKTUR,
    UCTM_DOUBLE_STRUCK,
};
```

## Keycodes

| Keycode            | Alias     | Description                                                 |
|--------------------|-----------|-------------------------------------------------------------|
| `KC_NOMODE`        | `KC_NORM` | Disables the typing modes, uses normal input.               |
| `KC_WIDE`          | `KC_WIDE` | Ｔｙｐｅｓ ｉｎ ｗｉｄｅ ｔｅｘｔ.                       |
| `KC_SCRIPT`        | `KC_SCPT` | 𝓣𝔂𝓹𝓮𝓼 𝓲𝓷 𝓯𝓪𝓷𝓬𝔂 𝓼𝓬𝓻𝓲𝓹𝓽.                                  |
| `KC_BLOCKS`        | `KC_BLCK` | 🆃🆈🅿🅴🆂 🅸🅽 🅱🅻🅾🅲🅺🆃🅴🆇🆃.                         |
| `KC_REGIONAL`      | `KC_REG`  | 🇹‌‌🇾‌‌🇵‌‌🇪‌‌🇸‌‌ ‌‌🇮‌🇳‌‌‌ ‌‌🇷‌‌🇪‌‌🇬‌‌🇮‌🇴‌🇳‌‌🇦‌‌‌‌🇱‌‌ ‌‌🇧‌‌🇱‌‌🇴‌‌🇨‌‌🇰‌‌🇸‌‌. |
| `KC_AUSSIE`        | `KC_AUSS` | ˙ǝᴉssnɐ uɐ ǝɹ‚noʎ ǝʞᴉl sǝdʎʇ                              |
| `KC_ZALGO`         | `KC_ZALG` | c̛͗ͅȕ̗̲ͥ̆̽r̖̔̈s̻̪͗ͧ̎͠ͅe̱̳͛̈͠d̡̘̽ͪ̚ t̢̡͖̃̿̐y̛̳͉̿͂p̡͈ị̴͙̾ͮ̉͢͡n͚ͦg̴͓̤ͭͥ͝ m̸̨͓͔o̵̘̦̹̭͗ͮ͜d͎͈̓ͭ̌e̴̘̩͆̑ f͔̠̑ͦ̿ͧ̕͟o̲̩ṟ̵͉͐ r̢̲̰̚͏̜̈e͚͇̼̯͞a̡͂̐̕l̡ͮ̏́͌̍ f̺̮̩͑̆̈́ù͖̺̩̆ͯ͟͝n̢͇̥͒.     |
| `KC_SUPERSCRIPT`   | `KC_SUPR` | ᵗʸᵖᵉ ⁱⁿ ᵃ ʰⁱᵍʰˡʸ ᵉˡᵉᵛᵃᵗᵉᵈ ᶜᵃˢᵉ.                        |
| `KC_COMIC`         | `KC_COMC` | ₸ℽℙℇ ⅈℕ ℂℴmⅈℂÅ⅃ ₷ℂℛⅈℙ₸.                                  |
| `KC_FRAKTUR`       | `KC_FRAK` | 𝔱𝔶𝔭𝔢 𝔦𝔫 𝔣𝔞𝔫𝔠𝔶 𝔣𝔯𝔞𝔨𝔱𝔲𝔯 𝔰𝔠𝔯𝔦𝔭𝔱.                                 |
| `KC_DOUBLE_STRUCK` | `KC_DBSK` | 𝕋𝕪𝕡𝕖 𝕚𝕟 𝔻𝕠𝕦𝕓𝕝𝕖𝕤𝕥𝕦𝕔𝕥𝕜 𝕤𝕔𝕣𝕚𝕡𝕥.                                |
