bazel build broadcast:broadcast &&
    ./maelstrom/maelstrom test -w broadcast --bin ./bazel-bin/broadcast/broadcast --node-count 1 --time-limit 20 --rate 10
