bazel build broadcast:broadcast &&
    ./maelstrom/maelstrom test -w broadcast --bin ./bazel-bin/broadcast/broadcast --node-count 25 --time-limit 20 --rate 100 --latency 100
