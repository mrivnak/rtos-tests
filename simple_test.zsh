echo "Running tests with $(nproc) core(s)";

for x in {0..200}; do echo "RR Pass: $x"; ./simpleRR >> output$(nproc)RR.txt; done;
for x in {0..200}; do echo "FIFO Pass: $x"; ./simpleFIFO >> output$(nproc)FIFO.txt; done;

chown michaelrivnak output$(nproc)RR.txt output$(nproc)FIFO.txt;
