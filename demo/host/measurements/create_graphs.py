from os import error, listdir, name
from os.path import isfile, join
from numpy import average, mean, std

import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
import matplotlib.ticker as ticker

def plot_clustered_bars(title, x_datas, y_datas, y_labels):
   
    width = 1/len(y_datas)  # the width of the bars
    from itertools import chain
    ticks = list(np.unique(np.concatenate(x_datas)))
    fig, ax = plt.subplots(figsize=(10,4))

    for i, (x, y, y_label) in enumerate(zip(x_datas, y_datas, y_labels)):
        ax.bar(x + (i - len(y_datas)/2)*width, y, width, label=y_label)

    plt.grid(axis='y')
    # Add some text for labels, title and custom x-axis tick labels, etc.
    ax.set_ylabel('Latency [us]')
    #ax.set_title(title)
    ax.set_xlabel('Message Size [KB]')
    ax.set_xticks(ticks)
    ax.legend()
    plt.show()
    #plt.savefig(f"{title}.png")

def plot_lines(title, x_datas, y_datas, y_labels, y_styles=None, logx=True, logy=True, y_errors=None):
    if not(y_styles):
        y_styles = [None for _ in range(len(y_labels))]
    
    if not(y_errors):
        y_errors = [None for _ in range(len(y_labels))]

    fig, ax = plt.subplots(figsize=(7,6))

    for x, y, y_label, y_style, y_error in zip(x_datas, y_datas, y_labels, y_styles, y_errors):
        if y_style:
            if not y_error is None:
                ax.errorbar(x, y,  yerr = y_error, fmt=y_style, label=y_label, capsize=2.0, linewidth=1)
            else:
                ax.plot(x, y, y_style, label=y_label)
        else:
            if not y_error is None:
                ax.errorbar(x, y, yerr = y_error, capsize=2.0, linewidth=1)
            else:
                ax.plot(x, y, label=y_label)
    plt.grid(axis='y')
    # Add some text for labels, title and custom x-axis tick labels, etc.
    ax.set_ylabel('Latency [us]')
    #ax.set_title(title)
    if logy:
        ax.set_yscale('log')
    if logx:
        ax.set_xscale('log', base=2)
    ax.xaxis.set_major_formatter(ticker.FuncFormatter(lambda y, _: sizeof_fmt(y)))
    plt.xticks(rotation=0)
    ax.set_xlabel('Message Size')
    ax.legend(loc="lower right")
    plt.show()
    plt.savefig(f"{title}.png", format='png')

def sizeof_fmt(num, suffix='B'):
    for unit in ['','K','M','G','T','P','E','Z']:
        if abs(num) < 1024.0:
            return "%3.f%s%s" % (num, unit, suffix)
        num /= 1024.0
    return "%.1f%s%s" % (num, 'Yi', suffix)

def normality_test(df):
    df              = df[ df["rank id"] == 0]
    collectives     = df["collective name"].unique()
    for collective in collectives:
        subset              = df[(df["collective name"] == collective) ]
        buffer_sizes        = subset["buffer size[KB]"].unique()
        segmentation_sizes  = subset["segment_size[KB]"].unique() 
        grouped             = subset.groupby(["board_instance", "number of banks", "buffer size[KB]", "segment_size[KB]"])
        
        import scipy.stats as stats
        import math
        for (board_instance, banks, bsize, ssize), group in grouped:
            print(bsize, ssize, group)
            exe     = group['Latency [us]'].to_numpy()
            #bufsize = grouped["buffer size[KB]"].to_numpy()
            plt.clf()
            plt.hist(exe, bins=20)
            mu      = mean(exe)
            sigma   = std(exe, ddof=1)
            x = np.linspace(mu - 3*sigma, mu + 3*sigma, 100)
            plt.twinx()
            plt.plot(x, stats.norm.pdf(x, mu, sigma), "r--")
            # normality test
            stat, p = stats.shapiro(exe)
            print('Statistics=%.3f, p=%.3f' % (stat, p))
            # interpret
            alpha = 0.05
            if p > alpha:
                print('Sample looks Gaussian (fail to reject H0)')
            else:
                print('Sample does not look Gaussian (reject H0)')
            plt.title(f"{board_instance} #banks{banks} {collective} b{bsize} s{ssize}\n"+("sample "+  ("looks" if p > alpha else "doesn't look" ) +" Gaussian"))
            plt.show()


def compare_ssize(df):
    df              = df[ df["rank id"] == 0]
    collectives     = df["collective name"].unique()
    for collective in collectives:
        subset              = df[(df["collective name"] == collective) & (df["experiment"] == "test1") & (df["board_instance"]=="xilinx_u280_xdma_201920_3") & (df["number of banks"] == 6)]
        grouped             = subset.groupby(["segment_size[KB]", "buffer size[KB]"]).mean(['execution_time[us]', 'execution_time_fullpath[us]'])
        grouped.reset_index(inplace=True)
        print("grouped", grouped)
        grouped             = grouped.groupby(["segment_size[KB]"])

        series_label = []
        series_y     = []
        series_x     = []
        for ssize, group in grouped:
            exe     = group['execution_time[us]'].to_numpy()
            bufsize = group['buffer size[KB]'].to_numpy()*1024
            #print("group", ssize,  group)
            #print(exe)
            #print(bufsize)
            series_label.append(ssize)
            series_y.append(exe)
            series_x.append(bufsize)
        plot_clustered_bars(collective, series_x, series_y, series_label)

def compare_board(df):
    df              = df[ (df["rank id"] == 0) & ( (df["number of nodes"]==4)  | (df["collective name"] == "Send/recv"))]
    collectives     = df["collective name"].unique()
    segment_size    = 1024
    for collective in collectives:
        subset              = df[(df["collective name"] == collective) & ((df["segment_size[KB]"] == segment_size) | (df["board_instance"] != "OpenMPI" ))]
        grouped             = subset.groupby(["board_instance", "buffer size[KB]"]).agg({'execution_time[us]':['mean','std'], 'execution_time_fullpath[us]':['mean','std']})
        grouped.reset_index(inplace=True)
        grouped             = grouped.groupby(["board_instance"])

        series_label = []
        series_y     = []
        series_x     = []
        styles       = []
        stdevs       = []
        average_delta = None
        i = 0
        for board, group in grouped:
            exe          = group['execution_time[us]']['mean'].to_numpy()
            exe_std      = group['execution_time[us]']['std'].to_numpy()
            bufsize      = group['buffer size[KB]'].to_numpy()*1024
            exe_full     = group['execution_time_fullpath[us]']['mean'].to_numpy()
            exe_full_std = group['execution_time_fullpath[us]']['std'].to_numpy()

            board = simplify_board_name(board)
            i+=1
            if np.any(exe != 0):
                series_label.append(f"{board} F2F")
                series_y.append(exe)
                series_x.append(bufsize)
                stdevs.append(exe_std)
                styles.append(f"C{i}-")
            if np.any(exe_full != 0):
                series_label.append(f"{board} H2H")
                series_y.append(exe_full)
                series_x.append(bufsize)
                stdevs.append(exe_full_std)
                styles.append(f"C{i}--")

            if board.find("U280") != -1:
                average_delta = np.abs(exe_full - exe)
        #For OpenMPI
        subset              = df[(df["collective name"] == collective) & (df["board_instance"] == "OpenMPI" )]
        grouped             = subset.groupby(["board_instance", "buffer size[KB]"]).agg({'execution_time[us]':['mean','std'], 'execution_time_fullpath[us]':['mean','std']})
        grouped.reset_index(inplace=True)
        grouped             = grouped.groupby(["board_instance"])

        for board, group in grouped:
            exe          = group['execution_time[us]']['mean'].to_numpy()
            exe_std      = group['execution_time[us]']['std'].to_numpy()
            bufsize      = group['buffer size[KB]'].to_numpy()*1024
            exe_full     = group['execution_time_fullpath[us]']['mean'].to_numpy()
            exe_full_std = group['execution_time_fullpath[us]']['std'].to_numpy()

            board = simplify_board_name(board)
            i+=1
            if np.any(exe_full != 0):
                if average_delta is not None:
                    series_label.append(f"{board} F2F")
                    exe = list(map(sum, zip(exe_full,average_delta)))
                    series_y.append(exe)
                    series_x.append(bufsize[:len(exe)])
                    stdevs.append(None)
                    styles.append(f"C{i}-")
                
                series_label.append(f"{board} H2H")
                series_y.append(exe_full)
                series_x.append(bufsize)
                stdevs.append(exe_full_std)
                styles.append(f"C{i}--")

        
        plot_lines("board_comparison"+collective.replace("/", ""), series_x, series_y, series_label, styles, y_errors=stdevs, logx=True)
        #plot_clustered_bars(collective, series_x, series_y, series_label)

def compare_rank_number(df):
    df              = df[ (df["rank id"] == 0) ]
    collectives     = df["collective name"].unique()
    segment_size    = 1024
    for collective in collectives:
        print(collective)
        subset              = df[(df["collective name"] == collective) & ((df["segment_size[KB]"] == segment_size) | (df["board_instance"] == "OpenMPI" ))]
        grouped             = subset.groupby(["board_instance", "number of nodes", "buffer size[KB]"]).mean(['execution_time[us]', 'execution_time_fullpath[us]'])
        grouped.reset_index(inplace=True)
        grouped             = grouped.groupby(["board_instance"])

        series_label = []
        series_y     = []
        series_x     = []
        styles       = []
        stdevs       = []
        i = 0
        ls = ['-', '--', '-.', ':']
        for board, group in grouped:
            board = simplify_board_name(board)
            j = 0
            for  num_nodes, sub_group in group.groupby(["number of nodes"]):
                if num_nodes not in [2,4,5,6,7]:
                    continue
                j+=1
                exe          = sub_group['execution_time[us]'].to_numpy()
                bufsize      = sub_group['buffer size[KB]'].to_numpy()
                
                if np.any(exe != 0):
                    series_label.append(f"{board} {num_nodes}")
                    series_y.append(exe)
                    stdevs.append(np.ones_like(exe))
                    series_x.append(bufsize)
                    styles.append(f"C{i}{ls[j % len(ls)]}+")
            i+=1
        plot_lines("rank_comparison"+collective.replace("/", ""), series_x, series_y, series_label, styles)

        #plot_clustered_bars(collective, series_x, series_y, series_label)
def simplify_board_name(name):
    if   name == "xilinx_u250_gen3x16_xdma_shell_3_1":
        return "U250"
    elif name == "xilinx_u280_xdma_201920_3":
        return "U280"
    else:
        return name

if __name__ == "__main__":
    #mypath ="C:\\Users\\danielep\\Documents\\github\\ACCL_measure"
    accl                 ="accl"
    openmpi              ="open_mpi"
    csv_files_accl       = [join(accl, f) for f in listdir(accl) if (isfile(join(accl, f)) and f.find(".csv") != -1)]
    csv_files_openmpi    = [join(openmpi, f) for f in listdir(openmpi) if (isfile(join(openmpi, f)) and f.find(".csv") != -1)]
    
    csv_files            = csv_files_accl + csv_files_openmpi
    print(csv_files)
    csvs = []
    for csv_path in csv_files:
        csvs.append(pd.read_csv(csv_path))
    df = pd.concat(csvs)
    #filter multiple headers
    headers = df.columns.tolist()
    df = df[df[headers[0]]!=headers[0]].reset_index(drop=True)
    for column_name in ["number of nodes","rank id","number of banks","buffer size[KB]","segment_size[KB]","execution_time[us]","throughput[Gbps]","execution_time_fullpath[us]","throughput_fullpath[Gbps]"]:
        df[column_name] = pd.to_numeric(df[column_name])
    print(df.dtypes)
    import argparse

    parser = argparse.ArgumentParser(description='Process some integers.')
    parser.add_argument('--norm'                , action='store_true', default=False,     help='test normality'                          )
    parser.add_argument('--ssize'               , action='store_true', default=False,     help='ssize vs buffer size'                     )
    parser.add_argument('--compare_board'       , action='store_true', default=False,     help='comapre performance of different alveo'   )
    parser.add_argument('--compare_rank_number' , action='store_true', default=False,     help='comapre performance of different number of ranks'   )
    #                                                                                                                       fig           data  
    #D)for every collectiveopen_mpi_and_fpga_at_different_ranks (probably 4-8 with fpga not full_path)                      6xfig         ok    
    #Z)send and receive throughput with different banks (1)                                                                 1xfig         ok
    #Z)send and receive throughput with different segment size (2)                                                          1             ko  
    #Z)send and receive latency    with different banks (1)                 (needed?)                                                     ok
    #Z)send and receive latency    with different segment size (2)          (needed?)                                                     ko   
    #Z)show throughput send-recv and compare with openmpi                                                                   1xfig         ok
    #optimizations 
    #D)   bcast scatter   : one of them                                                                                                   ok(u280 dual datapath baseline/ u280 rr)  
    #D)   reduce          : avoid store intermediate results (reduce UXX vs reduce UXX dual datapath. Shows streaming kernels)  x)        ok(upto 16 MB, dualpath / u280 baseline)
    #D)   allreduce       : can be overlapped with reduce                                                                       x)        ok(upto 32 MB, dualpath / u280 baseline, similar to reduce(we don't show naive implementation), difference w.r.t reduce would be more pronounced)
    # WE need to rename the experiments (data)
    #   different algorithm(scatter and bcast rr/none)
    # the other u280 baseline might be better than dual datapath (waiting for packetizer)

    args = parser.parse_args()
    if args.norm:
        normality_test(df)
    if args.ssize:
        compare_ssize(df)
    if args.compare_board:
        compare_board(df)
    if args.compare_rank_number:
        compare_rank_number(df)