# Portfolio

## Description
This repository contains the html document which is my portfolio. It also contains the C++ code that I use to generate it from a JSON configuration files according to a set of rules.

## How to use
The executable expects two command line arguments, which are the path to a `layout` JSON file and the `target` path. The `target` path is the constucted file's destination. The paths may be absolute or relative to the current directory of the command line.
The file type of the `target` or any part used to construct it is not known to the program, it will simply attempt to read and write these files with `char`s (which means that it is a bug for it to read wider character files).

The `layout` JSON file should evaluate to an `array` of `object`s. Each `object` constructs a part of the `target` file and the parts are positioned in the order of the `array`. Each object should have three keys, `"name"`, `"template"` and `"specializer"`. The value of `"name"` is only relevant to identify an `object` when erroneous. The value of `"template"` should be a `string` which is the path to a file to be read, which may be absolute or relative to the directory of the `layout` file. If the value of `"specializer"` is `null`, the file at `"template"` is inserted as read into the `target`. Otherwise the value `"specializer"` should be a `string` which is the path to a JSON file.

The `specialier` JSON file should evaluate to an `array` of `objects`. For each element of this `array`, an instantiation of its corresponding `template` is inserted to the `target`. The `object` specifies how the `template` is written. The program will read the `template` input and upon matching a key in the `object`, replace it with its corresponding value (which should be a `string`) in the output, which is written to the `target`.
