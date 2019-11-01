#!/bin/bash

function mysql {
    /mnt/c/Program\ Files/MySQL/MySQL\ Server\ 8.0/bin/mysql.exe -u icon -picon < <(echo "use iconation; SET FOREIGN_KEY_CHECKS=0;" ; echo "${1};"; echo "SET FOREIGN_KEY_CHECKS=1;")
}

function disable_foreign_checks {
     mysql "SET FOREIGN_KEY_CHECKS=0"
}

function enable_foreign_checks {
    mysql "SET FOREIGN_KEY_CHECKS=1"
}

function get_block {
    tbears blockbyheight ${1} -u  https://ctz.solidwallet.io/api/v3 | sed "s/block info : //g"
}

function get_block_hash {
    echo ${1} | jq '.result.block_hash'
}

function get_prev_block_hash {
    echo ${1} | jq '.result.prev_block_hash'
}

function get_time_stamp {
    echo ${1} | jq '.result.time_stamp'
}

function get_height {
    echo ${1} | jq '.result.height'
}

block=$(get_block ${1})
hash=$(get_block_hash "$block")
prev_hash=$(get_prev_block_hash "$block")
timestamp=$(get_time_stamp "$block")
height=$(get_height "$block")

mysql "INSERT INTO block (previous_block, hash, timestamp, height) VALUES (${prev_hash}, ${hash}, '${timestamp}', '${height}')"
