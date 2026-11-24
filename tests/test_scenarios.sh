#!/bin/bash
set -e

echo "=== 1. Building project ==="
make clean
make

echo "=== 2. Starting watchdog ==="
./watchdog &
WATCHDOG_PID=$!
sleep 2

echo "=== 3. Testing SIGUSR1 status report ==="
kill -SIGUSR1 $WATCHDOG_PID
sleep 1

echo "=== 4. Testing unexpected termination and restart limit (3 restarts) ==="
for i in 1 2 3 4; do
    if [ $i -eq 1 ]; then
        WORKER_PID=$(pgrep -P $WATCHDOG_PID | head -n 1)
    else
        WORKER_PID=$(pgrep -P $WATCHDOG_PID | tail -n 1)
    fi
    if [ -n "$WORKER_PID" ]; then
        echo "Killing worker PID $WORKER_PID (attempt $i)"
        kill -SIGKILL $WORKER_PID
        sleep 2
    fi
done

echo "=== 5. Testing normal termination of remaining worker ==="
REMAINING_PID=$(pgrep -P $WATCHDOG_PID | head -n 1)
if [ -n "$REMAINING_PID" ]; then
    echo "Sending SIGTERM to worker PID $REMAINING_PID"
    kill -SIGTERM $REMAINING_PID
    sleep 2
fi

echo "=== 6. Testing graceful shutdown (SIGTERM to watchdog) ==="
kill -SIGTERM $WATCHDOG_PID
wait $WATCHDOG_PID

echo "=== 7. Verifying watchdog.log ==="
cat watchdog.log
