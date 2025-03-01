This is adds a "cut-down" version of the macOS Globe Key to your build.
As the full Globe Key functionality requires an Apple vendor/product ID pair, this isn't completely functional and things like F-key support may not work.

Add the following to the list of modules in your `keymap.json` to enable this module:

```json
{
    "modules": ["drashna/unicode_typing"]
}
```

After this, your keymap can add one of the many typing modes to type in unicode text.

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
