import re

from termcolor import colored

filenames = ['output1FIFO', 'output1RR', 'output2FIFO', 'output2RR', 'output4FIFO', 'output4RR', 'output8FIFO', 'output8RR']

for filename in filenames:
    
    high_prio_threads = []
    low_prio_threads = []
    default_threads = []

    with open(filename + '.txt', 'r') as file:
        output_regex = re.compile(r'\[(\d+)\] Thread: \d+\tSched Policy: (\w+_\w+)(\tPriority: (\d+))?')
        for line in file.readlines():
            result = output_regex.search(line)
            if result:
                if result.group(4) is None:
                    default_threads.append(float(result.group(1))/1000000.0)
                elif int(result.group(4)) == 99:
                    high_prio_threads.append(float(result.group(1))/1000000.0)
                elif int(result.group(4)) == 1:
                    low_prio_threads.append(float(result.group(1))/1000000.0)

    with open(filename + 'formatted.csv', 'w') as file:
        for i in range(min(len(high_prio_threads), len(low_prio_threads), len(default_threads))):
            file.write('{},{},{},\n'.format(high_prio_threads[i], low_prio_threads[i], default_threads[i]))

    num_lines = 8 * 201
    missing_lines_high = len(high_prio_threads) < num_lines
    missing_lines_low = len(low_prio_threads) < num_lines
    missing_lines_def = len(default_threads) < num_lines

    print('File: {}.txt; 99: {}, 1: {}, def: {}'.format(
        filename,
        colored(len(high_prio_threads), 'red' if missing_lines_high else 'green'),
        colored(len(low_prio_threads), 'red' if missing_lines_low else 'green'),
        colored(len(default_threads), 'red' if missing_lines_def else 'green')
    ))
    
