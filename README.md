# function-sandbox

Try a change to **one function** without touching your source.

`function-sandbox <function>` makes a throwaway copy of your project where every
file is a **symlink back to your original**, except the single file that defines
that function, which is a real, writable **copy**. You edit that one copy, build
and run the copy, and see the result. Your real tree is never modified, so
there's nothing to back up and nothing to restore: when done, delete the copy.

```
your-project/                 your-project.sandbox-area/   (the throwaway copy)
  main.cpp                       main.cpp   -> symlink to your original
  area.cpp   (defines area)      area.cpp   == REAL COPY  (edit this one)
  area.h                         area.h     -> symlink
  build.sh                       build.sh   -> symlink
```

## Why a copy of symlinks, instead of editing in place or an override file

- **Not in-place + restore:** that would modify your real file; one crash mid-way
  and your source is in an unknown state. Here the original is opened read-only.
- **Not "add an override file":** to make an added file win over the original in
  the same binary you'd need link-time symbol tricks (weak symbols, `--wrap`),
  which are fiddly and don't work cleanly everywhere. Editing a real copy of the
  one file, with the rest symlinked, is trivial and portable, and the compiler
  follows the symlinks so the build is unchanged.

The version lives in the **copy**; near-zero duplication (one real file, the rest
pointers); undo is just `rm -rf` of the copy.

## Build

```sh
./build.sh        # compiles function-sandbox.cpp -> ./function-sandbox
```

## Use

Run it from the root of any C++ project:

```sh
cd example
../function-sandbox area
#  -> made a throwaway copy at  .../example.sandbox-area
#     edit area() there:  area.cpp:4
#     then: cd '.../example.sandbox-area' && ./build.sh && ./build/app
#     delete it when done: rm -rf '.../example.sandbox-area'
```

It finds where the function is **defined** (searching the whole tree, picking the
definition: the form where the next `{` precedes the next `;`), copies the tree
with that one file made real and everything else symlinked, and prints where to
edit and how to run. It does **not** edit, build, or run, you drive that.

## Limits (be honest)

- **Edit only the file it points you at.** Every other file is a symlink, so
  editing one of those would write through to your original. If an experiment
  grows to a second file, re-run it on a function in that file.
- **Function location is a light parser** (regex for the definition + a brace/
  semicolon heuristic). Fine for normal C++; it can be fooled by overloads
  (returns the first) or a prototype with a brace default-arg. A clang AST would
  be exact; this is the lightweight trade.
- **Build convention:** the printed hint assumes `./build.sh` produces
  `build/app` (as the bundled `example/` does). Adjust to your project's commands.
