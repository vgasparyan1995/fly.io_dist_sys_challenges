bazel build echo:echo &&
    ./maelstrom/maelstrom test -w echo --bin ./bazel-bin/echo/echo --node-count 1 --time-limit 10
