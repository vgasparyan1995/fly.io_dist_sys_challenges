bazel build unique_id:unique_id &&
    ./maelstrom/maelstrom test -w unique-ids --bin ./bazel-bin/unique_id/unique_id --time-limit 30 --rate 1000 --node-count 3 --availability total --nemesis partition
