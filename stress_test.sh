#!/bin/bash

echo "=== Stress test: launching 10 players ==="
pids=()

for i in {1..10}; do
	./lemipc &
	pids+=($!)
	sleep 0.2 # léger décalage pour simuler des connexions asynchrones
done

sleep 15 # attendre que le jeu tourne un peu

echo "== Killing all players =="
for pid in "${pids[@]}"; do
	kill -INT "$pid" 2>/dev/null
	sleep 0.05
done

wait

echo "== Checking IPCs (should be empty if cleanup OK) =="
ipcs | grep "$(whoami)"
if ipcs | grep "$(whoami)" | grep -q "Shared Memory\|Semaphore"; then
	echo "❌ IPCs still present after cleanup"
else
	echo "✅ All IPCs cleaned successfully"
fi
