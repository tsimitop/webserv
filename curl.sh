#!/bin/bash

for i in {1..10}; do
  curl -vv -d "test$i" http://localhost:4242 &
done

for i in {1..10}; do
  curl -vv http://localhost:4242 &
done

wait
echo "All requests are done"