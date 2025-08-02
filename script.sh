#!/bin/bash

BIN="./lemipc"
NUM_PLAYERS=10
WAIT_TIME=15

function launch_players() {
	local team1=$1
	local team2=$2
	local total=$3
	local use_visual=$4
	pids=()

	echo "Launching $total players (Team $team1 and Team $team2)..."
	for ((i = 1; i <= total; i++)); do
		team=$((i % 2 == 0 ? team1 : team2))
		if [ "$use_visual" = "1" ]; then
			$BIN -v $team &
		else
			$BIN $team &
		fi
		pids+=($!)
		sleep 0.1
	done
}

function kill_all_players() {
	if [ ${#pids[@]} -eq 0 ]; then
		echo "No players to kill."
		return
	fi
	echo "== Killing all players =="
	for pid in "${pids[@]}"; do
		kill -INT "$pid" 2>/dev/null
		sleep 0.05
	done
	wait
	check_ipcs
	pids=() # reset pids after killing
}

function check_ipcs() {
	echo "== Checking IPCs (should be empty if cleanup OK) =="
	if ipcs | grep "$(whoami)" | grep -q "Shared Memory\|Semaphore\|Message"; then
		ipcs | grep "$(whoami)"
		echo "❌ IPCs still present after cleanup"
	else
		echo "✅ All IPCs cleaned successfully"
	fi
}

function stress_test() {
	echo "=== Stress Test: $NUM_PLAYERS random players ==="
	read -p "Enable visual mode ? (y/n): " visual_choice
	if [[ "$visual_choice" =~ ^[Yy]$ ]]; then
		use_visual=1
	else
		use_visual=0
	fi

	pids=()
	for i in $(seq 1 $NUM_PLAYERS); do
		if [ "$use_visual" = "1" ]; then
			$BIN -v &
		else
			$BIN &
		fi
		pids+=($!)
		sleep 0.2
	done
	sleep $WAIT_TIME
	kill_all_players
}

function sigint_test() {
	echo "=== SIGINT Test: Launching 1 player and killing ==="
	$BIN &
	pid=$!
	sleep 3
	echo "Sending SIGINT to player $pid"
	kill -INT "$pid"
	wait $pid
	check_ipcs
}

function two_teams_game() {
	echo "=== Two Teams Game Test ==="
	read -p "Enable visual mode ? (y/n): " visual_choice
	if [[ "$visual_choice" =~ ^[Yy]$ ]]; then
		use_visual=1
	else
		use_visual=0
	fi
	launch_players 1 2 $NUM_PLAYERS $use_visual
	sleep $WAIT_TIME
	kill_all_players
}

function unbalanced_teams_game() {
	echo "=== Unbalanced Teams Game Test ==="
	read -p "Team 1 ID: " t1
	read -p "Number of players in Team $t1: " t1_count
	read -p "Team 2 ID: " t2
	read -p "Number of players in Team $t2: " t2_count
	read -p "Enable visual mode ? (y/n): " visual_choice
	if [[ "$visual_choice" =~ ^[Yy]$ ]]; then
		use_visual=1
	else
		use_visual=0
	fi

	total=$((t1_count + t2_count))
	pids=()

	echo "Launching $total players: $t1_count in Team $t1, $t2_count in Team $t2..."

	for ((i = 0; i < t1_count; i++)); do
		if [ "$use_visual" = "1" ]; then
			$BIN -v "$t1" &
		else
			$BIN "$t1" &
		fi
		pids+=($!)
		sleep 0.1
	done
	for ((i = 0; i < t2_count; i++)); do
		if [ "$use_visual" = "1" ]; then
			$BIN -v "$t2" &
		else
			$BIN "$t2" &
		fi
		pids+=($!)
		sleep 0.1
	done
}

function main_menu() {
	echo "==== LEMIPC Test Runner ===="
	echo "1. Stress Test ($NUM_PLAYERS players)"
	echo "2. SIGINT Test (clean exit)"
	echo "3. Two Teams Game (1 vs 2)"
	echo "4. Unbalanced Teams Game"
	echo "5. Custom Player Count Test"
	echo "6. Exit"
	read -p "Choose a test: " choice

	case $choice in
	1) stress_test ;;
	2) sigint_test ;;
	3) two_teams_game ;;
	4) unbalanced_teams_game ;;
	5)
		read -p "Number of total players: " custom_players
		read -p "Team 1 number: " t1
		read -p "Team 2 number: " t2
		read -p "Enable visual mode ? (y/n): " visual_choice
		if [[ "$visual_choice" =~ ^[Yy]$ ]]; then
			use_visual=1
		else
			use_visual=0
		fi
		launch_players "$t1" "$t2" "$custom_players" "$use_visual"
		;;
	6) exit 0 ;;
	esac
}

while true; do
	main_menu
	read -p "Press Enter to return to menu..."
done
