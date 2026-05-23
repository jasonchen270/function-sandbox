# function-sandbox

Try a change to **one function** without touching your source.

`function-sandbox <function>` makes a throwaway copy of your project where every
file is a **symlink back to your original**, except the single file that defines
that function, which is a real, writable **copy**. You edit that one copy, build
and run the copy, and see the result. Your real tree is never modified, so
there's nothing to back up and nothing to restore: when done, delete the copy.

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
