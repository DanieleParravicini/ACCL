from os import listdir, remove
from os.path import isfile, join
from itertools import chain
import re
import csv
from tqdm import tqdm
src_base_path ="."
dst_base_path = "../../measurements/open_mpi"

if __name__ == "__main__":

    log_dirs    = map(lambda x: join(src_base_path,x), listdir(src_base_path))
    log_dirs    = filter(lambda x: not(isfile(x))    , log_dirs)
    log_content = map(lambda x: (x, listdir(x)), log_dirs)
    log_content = chain.from_iterable(map(lambda x: [join(x[0], f) for f in x[1] ], log_content))
    log_content = filter(lambda x: isfile(x) and (x.find(".txt") != -1), log_content)
        
    for filename in tqdm(list(log_content)):
        print("file now processing", filename)

        filename_parts = re.findall("mpi_size_(\d+)_n_(\d+)_(\w+).txt", filename)
        if len(filename_parts) == 0 or len(filename_parts) > 1:
            continue
        
        experiment      = "converted from log"
        board_instance  = "open_mpi"
        number_of_nodes = filename_parts[0][1]
        rank_id         = 0
        number_of_banks = 0
        buffer_size     = filename_parts[0][0] 
        segment_size    = 0
        collective_name = filename_parts[0][2][0].upper() + filename_parts[0][2][1:].lower().replace("_","")
        execution_time_fullpath     = 0
        throughput_fullpath         = 0
                
        with open(join(dst_base_path, f"open_mpi_{collective_name}.csv"), "a+", newline="") as f_out:
            csv_writer = csv.writer(f_out, delimiter=",", quoting=csv.QUOTE_MINIMAL)
            #csv structure 
            # [experiment,board_instance,number of nodes,rank id,number of banks,buffer size[KB],segment_size[KB],collective name,execution_time[us],throughput[Gbps],execution_time_fullpath[us],throughput_fullpath[Gbps]]
            csv_writer.writerow(["experiment", "board_instance", "number of nodes", "rank id", "number of banks", "buffer size[KB]", "segment_size[KB]", "collective name", "execution_time[us]", "throughput[Gbps]","execution_time_fullpath[us]", "throughput_fullpath[Gbps]"])

            for line in open(filename, "r").readlines():
                line_parts = re.findall("iteration:(\d+), time\[s\]:(\d+\.\d+)", line)
                if len(line_parts) == 0 or len(line_parts) > 1:
                    continue

                execution_time = float(line_parts[0][1]) * pow(10,6)                
                throughput      = 0

                csv_writer.writerow([experiment, board_instance, number_of_nodes, rank_id, number_of_banks, buffer_size, segment_size, collective_name, execution_time, throughput, execution_time_fullpath, throughput_fullpath])
        
