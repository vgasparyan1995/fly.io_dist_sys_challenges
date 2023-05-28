bazel build broadcast:broadcast &&
    ./maelstrom/maelstrom test -w broadcast --bin ./bazel-bin/broadcast/broadcast --node-count 5 --time-limit 20 --rate 10 --nemesis partition
