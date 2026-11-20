for i in $(seq 1 8); do
  ( for _ in $(seq 1 1000); do cat /sys/kernel/race_cond/race_cond; done ) > "out_$i" &
done
wait
echo "Finito!"