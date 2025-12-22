# Copyright 2021 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
"""Python program to make autocorrect_data.h.
This program reads from prepared dictionary files (up to 8) and generates a C source file
"autocorrect_data.h" with serialized tries embedded as an array. Run this
program and pass dictionary files as arguments like:
$ python make_autocorrect_data.py dict1.txt dict2.txt [output_file.h]
Each line of the dict file defines one typo and its correction with the syntax
"typo -> correction". Blank lines or lines starting with '#' are ignored.
Example:
  :thier        -> their
  fitler        -> filter
  lenght        -> length
  ouput         -> output
  widht         -> width
For full documentation, see QMK Docs
"""

import os.path
import sys
import textwrap
from pathlib import Path
from typing import Any, Dict, Iterator, List, Tuple


KC_A = 4
KC_SPC = 0x2C
KC_QUOT = 0x34

TYPO_CHARS = dict(
    [
        ("'", KC_QUOT),
        (":", KC_SPC),  # "Word break" character.
    ]
    + [(chr(c), c + KC_A - ord("a")) for c in range(ord("a"), ord("z") + 1)]
)  # Characters a-z.


def parse_file(file_name: str) -> List[Tuple[str, str]]:
    """Parses autocorrections dictionary file.
    Each line of the file defines one typo and its correction with the syntax
    "typo -> correction". Blank lines or lines starting with '#' are ignored. The
    function validates that typos only have characters a-z and that typos are not
    substrings of other typos, otherwise the longer typo would never trigger.
    Args:
      file_name: String, path of the autocorrections dictionary.
    Returns:
      List of (typo, correction) tuples.
    """

    try:
        import english_words

        correct_words = english_words.get_english_words_set(
            ["web2"], lower=True, alpha=True
        )
    except AttributeError:
        from english_words import english_words_lower_alpha_set as correct_words

        print("The english_words package is outdated, update by running:")
        print("  python3 -m pip install english_words --upgrade")
    except ImportError:
        print("Autocorrection will falsely trigger when a typo is a substring of a correctly spelled word.")
        print("To check for this, install the english_words package and rerun this script:")
        print("  python3 -m pip install english_words")
        # Use a minimal word list as a fallback.
        correct_words = (
            "information",
            "available",
            "international",
            "language",
            "loosest",
            "reference",
            "wealthier",
            "entertainment",
            "association",
            "provides",
            "technology",
            "statehood",
        )

    autocorrections = []
    typos = set()
    for line_number, typo, correction in parse_file_lines(file_name):
        if typo in typos:
            print(f'Warning:{line_number}: Ignoring duplicate typo: "{typo}"')
            continue

        # Check that `typo` is valid.
        if not (all([c in TYPO_CHARS for c in typo])):
            print(
                f'Error:{line_number}: Typo "{typo}" has '
                "characters other than " + "".join(TYPO_CHARS.keys())
            )
            sys.exit(1)
        for other_typo in typos:
            if typo in other_typo or other_typo in typo:
                print(
                    f"Error:{line_number}: Typos may not be substrings of one "
                    f"another, otherwise the longer typo would never trigger: "
                    f'"{typo}" vs. "{other_typo}".'
                )
                sys.exit(1)
        if len(typo) < 5:
            print(
                f'Warning:{line_number}: It is suggested that typos are at least 5 characters long to avoid false triggers: "{typo}"'
            )
        if len(typo) > 127:
            print(f'Error:{line_number}: Typo exceeds 127 chars: "{typo}"')
            sys.exit(1)

        check_typo_against_dictionary(typo, line_number, correct_words)

        autocorrections.append((typo, correction))
        typos.add(typo)

    return autocorrections


def make_trie(autocorrections: List[Tuple[str, str]]) -> Dict[str, Any]:
    """Makes a trie from the the typos, writing in reverse.
    Args:
      autocorrections: List of (typo, correction) tuples.
    Returns:
      Dict of dict, representing the trie.
    """
    trie = {}
    for typo, correction in autocorrections:
        node = trie
        for letter in typo[::-1]:
            node = node.setdefault(letter, {})
        node["LEAF"] = (typo, correction)

    return trie


def parse_file_lines(file_name: str) -> Iterator[Tuple[int, str, str]]:
    """Parses lines read from `file_name` into typo-correction pairs."""

    line_number = 0
    for line in open(file_name, "rt"):
        line_number += 1
        line = line.strip()
        if line and line[0] != "#":
            # Parse syntax "typo -> correction", using strip to ignore indenting.
            tokens = [token.strip() for token in line.split("->", 1)]
            if len(tokens) != 2 or not tokens[0]:
                print(f'Error:{line_number}: Invalid syntax: "{line}"')
                sys.exit(1)

            typo, correction = tokens
            typo = typo.lower()  # Force typos to lowercase.
            typo = typo.replace(" ", ":")

            yield line_number, typo, correction


def check_typo_against_dictionary(typo: str, line_number: int, correct_words) -> None:
    """Checks `typo` against English dictionary words."""

    if typo.startswith(":") and typo.endswith(":"):
        if typo[1:-1] in correct_words:
            print(f'Warning:{line_number}: Typo "{typo}" is a correctly spelled '
                    'dictionary word.')
    elif typo.startswith(":") and not typo.endswith(":"):
        for word in correct_words:
            if word.startswith(typo[1:]):
                print(f'Warning:{line_number}: Typo "{typo}" would falsely trigger on correctly spelled word "{word}".')
    elif not typo.startswith(":") and typo.endswith(":"):
        for word in correct_words:
            if word.endswith(typo[:-1]):
                print(f'Warning:{line_number}: Typo "{typo}" would falsely trigger on correctly spelled word "{word}".')
    elif not typo.startswith(":") and not typo.endswith(":"):
        for word in correct_words:
            if typo in word:
                print(f'Warning:{line_number}: Typo "{typo}" would falsely trigger on correctly spelled word "{word}".')


def serialize_trie(
    autocorrections: List[Tuple[str, str]], trie: Dict[str, Any]
) -> Tuple[List[int], int]:
    """Serializes trie and correction data in a form readable by the C code.
    Args:
      autocorrections: List of (typo, correction) tuples.
      trie: Dict of dicts.
    Returns:
      Tuple of (List of ints in the range 0-255, link_byte_count).
    """
    table = []

    # Traverse trie in depth first order.
    def traverse(trie_node):
        if "LEAF" in trie_node:  # Handle a leaf trie node.
            typo, correction = trie_node["LEAF"]
            word_boundary_ending = typo[-1] == ":"
            typo = typo.strip(":")
            i = 0  # Make the autocorrection data for this entry and serialize it.
            while i < min(len(typo), len(correction)) and typo[i] == correction[i]:
                i += 1
            backspaces = len(typo) - i - 1 + word_boundary_ending
            assert 0 <= backspaces <= 63
            correction = correction[i:]
            bs_count = [backspaces + 128]  # Set high bit to mark as leaf, encode backspaces in lower 6 bits
            data = bs_count + list(bytes(correction, "ascii")) + [0]

            entry = {"data": data, "links": [], "byte_offset": 0}
            table.append(entry)
        elif len(trie_node) == 1:  # Handle trie node with a single child.
            c, trie_node = next(iter(trie_node.items()))
            entry = {"chars": c, "byte_offset": 0}

            # It's common for a trie to have long chains of single-child nodes. We
            # find the whole chain so that we can serialize it more efficiently.
            while len(trie_node) == 1 and "LEAF" not in trie_node:
                c, trie_node = next(iter(trie_node.items()))
                entry["chars"] += c

            table.append(entry)
            entry["links"] = [traverse(trie_node)]
        else:  # Handle trie node with multiple children.
            entry = {"chars": "".join(sorted(trie_node.keys())), "byte_offset": 0}
            table.append(entry)
            entry["links"] = [traverse(trie_node[c]) for c in entry["chars"]]
        return entry

    traverse(trie)

    def serialize(e: Dict[str, Any], link_byte_count: int) -> List[int]:
        if not e["links"]:  # Handle a leaf table entry.
            return e["data"]
        elif len(e["links"]) == 1:  # Handle a chain table entry.
            return [TYPO_CHARS[c] for c in e["chars"]] + [
                0
            ]  # + encode_link(e['links'][0]))
        else:  # Handle a branch table entry.
            data = []
            for c, link in zip(e["chars"], e["links"]):
                data += [TYPO_CHARS[c] | (0 if data else 64)] + encode_link(
                    link, link_byte_count
                )
            return data + [0]

    def serialized_entry_length(entry: Dict[str, Any], link_byte_count: int) -> int:
        if not entry["links"]:
            return len(entry["data"])
        elif len(entry["links"]) == 1:
            return len(entry["chars"]) + 1
        else:
            return (len(entry["chars"]) * (1 + link_byte_count)) + 1

    def try_assign_offsets(link_byte_count: int) -> Tuple[int, bool]:
        byte_offset = 0
        max_offset = (1 << (link_byte_count * 8)) - 1

        for entry in table:
            entry["byte_offset"] = byte_offset
            byte_offset += serialized_entry_length(entry, link_byte_count)

            if not (0 <= byte_offset <= max_offset):
                return byte_offset, False

        return byte_offset, True

    link_byte_count = 2
    byte_offset, success = try_assign_offsets(link_byte_count)

    if not success:
        print("Autocorrection table exceeds 64KB, switching to 3-byte node links.")
        link_byte_count = 3
        byte_offset, success = try_assign_offsets(link_byte_count)

    if not success:
        print(
            "Error: The autocorrection table is too large, a node link exceeds 16MB limit. Try reducing the autocorrection dict to fewer entries."
        )
        sys.exit(1)

    serialized_data = [
        b for e in table for b in serialize(e, link_byte_count)
    ]  # Serialize final table.
    assert len(serialized_data) == byte_offset

    return serialized_data, link_byte_count


def encode_link(link: Dict[str, Any], link_byte_count: int) -> List[int]:
    """Encodes a node link using `link_byte_count` bytes."""
    byte_offset = link["byte_offset"]
    if not (0 <= byte_offset < (1 << (link_byte_count * 8))):
        print(
            "Error: The autocorrection table is too large, a node link exceeds "
            "the supported limit. Try reducing the autocorrection dict to fewer entries."
        )
        sys.exit(1)
    return [(byte_offset >> (8 * i)) & 255 for i in range(link_byte_count)]


def typo_len(e: Tuple[str, str]) -> int:
    return len(e[0])


def to_hex(b: int) -> str:
    return f"0x{b:02X}"


class AutocorrectDict:
    """Class to generate autocorrect data from a file."""

    def __init__(self, path: Path):
        self.path = path
        self.autocorrections = parse_file(str(self.path))

        self.trie = make_trie(self.autocorrections)
        self.data, self.link_byte_count = serialize_trie(
            self.autocorrections, self.trie
        )

        assert all(0 <= b <= 255 for b in self.data)

        self.min_typo = min(self.autocorrections, key=typo_len)[0]
        self.max_typo = max(self.autocorrections, key=typo_len)[0]


def write_generated_code(
    autocorrect_dicts: List[AutocorrectDict], file_name: str
) -> None:
    """Writes autocorrection data as generated C code to `file_name`.

    Args:
      autocorrect_dicts: List of AutocorrectDict objects.
      file_name: String, path of the output C file.
    """

    n_entries = sum(len(dict_.autocorrections) for dict_ in autocorrect_dicts)

    # Collect all information
    data, maxs, mins, sizes, nodes = [], [], [], [], []

    autocorrect_comments = [f"// Autocorrection dictionary ({n_entries} entries):\n"]

    for dict_ in autocorrect_dicts:
        autocorrect_comments.append(f"// From {dict_.path}\n")
        for typo, correction in dict_.autocorrections:
            autocorrect_comments.append(
                f"//   {typo:<{len(dict_.max_typo)}} -> {correction}\n"
            )
        autocorrect_comments.append("// " + "-" * 15 + "\n")

        data.extend(dict_.data)
        maxs.append(len(dict_.max_typo))
        mins.append(len(dict_.min_typo))
        sizes.append(len(dict_.data))
        nodes.append(dict_.link_byte_count)

    assert sum(sizes) == len(data)

    offsets = [0]
    for size in sizes[:-1]:
        offsets.append(offsets[-1] + size)

    def list_to_str(x: list) -> str:
        """Helper to stringify the lists"""
        return ", ".join(map(str, x))

    offsets_str = list_to_str(offsets)
    maxs_str = list_to_str(maxs)
    mins_str = list_to_str(mins)
    sizes_str = list_to_str(sizes)
    nodes_str = list_to_str(nodes)

    generated_code = "".join(
        [
            "// Generated code.\n\n",
        ]
        + autocorrect_comments
        + [
            "\n#pragma once\n\n",
            f"#define N_DICTS {len(autocorrect_dicts)}\n\n",
            f"static const uint32_t autocorrect_offsets[N_DICTS] PROGMEM     = {{{offsets_str}}};\n",
            f"static const uint16_t autocorrect_min_lengths[N_DICTS] PROGMEM = {{{mins_str}}};\n",
            f"static const uint16_t autocorrect_max_lengths[N_DICTS] PROGMEM = {{{maxs_str}}};\n",
            f"static const uint32_t autocorrect_sizes[N_DICTS] PROGMEM       = {{{sizes_str}}};\n",
            f"static const uint8_t  autocorrect_node_size[N_DICTS] PROGMEM   = {{{nodes_str}}};\n\n",
            f"#define DICTIONARY_SIZE {sum(sizes)}\n",
            f"#define TYPO_BUFFER_SIZE {max(maxs)}\n\n",
            "static const uint8_t autocorrect_data[DICTIONARY_SIZE] PROGMEM = {\n",
            textwrap.fill(
                "    %s" % (", ".join(map(to_hex, data))),
                width=100,
                subsequent_indent="    ",
            ),
            "\n};\n\n",
        ]
    )

    with open(file_name, "wt") as f:
        f.write(generated_code)

    print(
        f"Processed {n_entries} autocorrection entries to table with {sum(sizes)} bytes."
    )
    print(f"Wrote autocorrection data to {file_name}")


def get_default_h_file(dict_file: str) -> str:
    return os.path.join(os.path.dirname(dict_file), "autocorrect_data.h")


def main(argv):
    """Main entry point for the script."""
    if len(argv) < 2:
        print(
            "Usage: python make_autocorrect_data.py dict1.txt [dict2.txt ...] [output.h]"
        )
        print("Supports up to 8 dictionary files.")
        sys.exit(1)

    # Separate dict files from output file
    # If last arg ends with .h, it's the output file
    if argv[-1].endswith(".h"):
        dict_files = argv[1:-1]
        h_file = argv[-1]
    else:
        dict_files = argv[1:]
        h_file = get_default_h_file(dict_files[0])

    if len(dict_files) > 8:
        print("Error: Current EEPROM settings can only index up to 8 dicts")
        sys.exit(1)

    if len(dict_files) == 0:
        print("Error: At least one dictionary file must be provided")
        sys.exit(1)

    # Convert to Path objects and create AutocorrectDict instances
    autocorrect_dicts = [AutocorrectDict(Path(f)) for f in dict_files]

    # Write the generated code
    write_generated_code(autocorrect_dicts, h_file)


if __name__ == "__main__":
    main(sys.argv)
