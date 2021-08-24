from os import error, listdir, name, terminal_size
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


def plot_lines(title, x_datas, y_datas, y_series_labels, y_styles=None, logx=True, logy=True, x_label='Message Size', y_label='Latency [us]', y_errors=None, legend_loc=None, throughput=False):
    if not(y_styles):
        y_styles = [None for _ in range(len(y_series_labels))]
    
    if not(y_errors):
        y_errors = [None for _ in range(len(y_series_labels))]

    fig, ax = plt.subplots(figsize=(5,6))

    for x, y, y_series_label, y_style, y_error in zip(x_datas, y_datas, y_series_labels, y_styles, y_errors):
        if y_style:
            if not y_error is None:
                ax.errorbar(x, y,  yerr = y_error, fmt=y_style, label=y_series_label, capsize=3.0, linewidth=2)
            else:
                ax.plot(x, y, y_style, label=y_series_label)
        else:
            if not y_error is None:
                ax.errorbar(x, y,  yerr = y_error, fmt=y_style, label=y_series_label, capsize=3.0, linewidth=2)
            else:
                ax.plot(x, y, label=y_series_label)

    plt.grid(axis='y')
    # Add some text for labels, title and custom x-axis tick labels, etc.
    ax.set_ylabel(y_label)
    if throughput:
        ax.set_ylabel('Throughput [Gbps]', fontsize=14)
        ax.axis(ymin=0,ymax=100)
    else:
        ax.set_ylabel('Latency [us]' , fontsize=14)
    #ax.set_title(title)
    if logy:
        ax.set_yscale('log')
    
    if logx:
        ax.set_xscale('log', base=2)
        
    if legend_loc is None :
        if logy:
            ax.legend(loc="lower right", fontsize=14)
        else:
            ax.legend(loc="upper left", fontsize=14)
    else:
        ax.legend(loc=legend_loc, fontsize=14)
    if x_label == "Message Size":
        ax.xaxis.set_major_formatter(ticker.FuncFormatter(lambda y, _: sizeof_fmt(y)))
    plt.xticks(rotation=0, fontsize = 12)
    plt.yticks(fontsize = 12)
    ax.set_xlabel(x_label, fontsize=14)
    plt.show()
    plt.savefig(f"{title}.png", format='png', bbox_inches='tight')

def plot_lines2(title, x_datas, y_datas, y_labels, y_styles=None, logx=True, logy=True, y_errors=None):
    if not(y_styles):
        y_styles = [None for _ in range(len(y_labels))]
    
    if not(y_errors):
        y_errors = [None for _ in range(len(y_labels))]

    fig, ax = plt.subplots(figsize=(7,6))

    for x, y, y_label, y_style, y_error in zip(x_datas, y_datas, y_labels, y_styles, y_errors):
        if y_style:
            ax.plot(x, y, y_style, label=y_label)
        else:
            ax.plot(x, y, label=y_label)
        
        if y_error is not None:
            ax.fill_between(x,y-y_error,y+y_error,alpha=.1)

    plt.grid(axis='y')
    # Add some text for labels, title and custom x-axis tick labels, etc.
    ax.set_ylabel('Latency [us]')
    #ax.set_title(title)
    if logy:
        ax.set_yscale('log')
        ax.legend(loc="lower right")
    else:
        ax.legend(loc="upper left")
        
    if logx:
        ax.set_xscale('log', base=2)

    ax.xaxis.set_major_formatter(ticker.FuncFormatter(lambda y, _: sizeof_fmt(y)))
    plt.xticks(rotation=0)
    ax.set_xlabel('Message Size')
    plt.show()
    plt.savefig(f"{title}.png", format='png')

def plot_lines3(title, x_datas, y_datas, y_labels, y_styles=None, logx=True, logy=True, y_errors=None):
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
                ax.errorbar(x, y,  yerr = y_error, fmt=y_style, label=y_label, capsize=2.0, linewidth=1)
            else:
                ax.plot(x, y, label=y_label)
        
        if y_error is not None:
            ax.fill_between(x,y-y_error,y+y_error,alpha=.1)

    plt.grid(axis='y')
    # Add some text for labels, title and custom x-axis tick labels, etc.
    ax.set_ylabel('Latency [us]')
    #ax.set_title(title)
    if logy:
        ax.set_yscale('log')
        ax.legend(loc="lower right")
    else:
        ax.legend(loc="upper left")
        
    if logx:
        ax.set_xscale('log', base=2)

    ax.xaxis.set_major_formatter(ticker.FuncFormatter(lambda y, _: sizeof_fmt(y)))
    plt.xticks(rotation=0)
    ax.set_xlabel('Message Size')
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

def get_statistics(df):
    df              = df[ df["rank id"] == 0]
    collectives     = df["collective name"].unique()

    seg_sizes = (1024,512,256,128)
    data_sizes = (1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768)
    # check sendrecv u280
    # subset = df[(df["experiment"] == "u280_sendrecv") & (df["collective name"] == "Send/recv") & (df["board_instance"] == "xilinx_u280_xdma_201920_3")]
    # print(subset)
    # for seg_size in seg_sizes:
    #     for data_size in data_sizes:
    #         subsubset = subset[(subset["segment_size[KB]"] == seg_size) & (subset["number of banks"] == 6) & (subset["buffer size[KB]"] == data_size)]
    #         # print(subsubset)
    #         mean = subsubset["throughput[Gbps]"].mean()
    #         count = subsubset["throughput[Gbps]"].count()
    #         std = subsubset["throughput[Gbps]"].std()
    #         if count < 100:
    #             print(f"U280, sendrecv, seg size:{seg_size}, num banks:{6}, data size:{data_size}, mean:{mean}, std:{std}, count:{count}")

    # for num_bank in range (2,6):
    #     for data_size in data_sizes:
    #         subsubset = subset[(subset["segment_size[KB]"] == 1024) & (subset["number of banks"] == num_bank) & (subset["buffer size[KB]"] == data_size) ]
    #         mean = subsubset["throughput[Gbps]"].mean()
    #         count = subsubset["throughput[Gbps]"].count()
    #         std = subsubset["throughput[Gbps]"].std()
    #         if count < 100:
    #             print(f"U280, sendrecv, seg size:{1024}, num banks:{num_bank}, data size:{data_size}, mean:{mean}, std:{std}, count:{count}")

    num_nodes = (3,4)
    #check U280 collectives (<= 4 nodes)
    subset = df[ (df["board_instance"] == "xilinx_u280_xdma_201920_3") & (df["collective name"] != "Send/recv") & (df["number of nodes"] <= 4 )]
    for num_node in num_nodes:
        for collective in collectives:
            if collective != "Send/recv":
                for data_size in data_sizes:
                    subsubset = subset[(subset["collective name"] == collective) &(subset["segment_size[KB]"] == 1024) & (subset["number of banks"] == 6) & (subset["buffer size[KB]"] == data_size) & (subset["number of nodes"] == num_node)]
                    print(subsubset)
                    mean = subsubset["execution_time[us]"].mean()
                    count = subsubset["execution_time[us]"].count()
                    std = subsubset["execution_time[us]"].std()
                    if count < 100:
                        print(f"U280, {collective}, num nodes:{num_node}, seg size:{1024}, num banks:{6}, data size:{data_size}, mean:{mean}, std:{std}, count:{count}")

    #check U250 collectives (<= 4 nodes)
    # subset = df[(df["board_instance"] == "xilinx_u250_gen3x16_xdma_shell_3_1") & (df["collective name"] != "Send/recv") & (df["number of nodes"] <= 4 )]
    # # print(subset)
    # for num_node in num_nodes:
    #     for collective in collectives:
    #         if collective != "Send/recv":
    #             for data_size in data_sizes:
    #                 subsubset = subset[(subset["collective name"] == collective) &(subset["segment_size[KB]"] == 1024) & (subset["number of banks"] == 3) & (subset["buffer size[KB]"] == data_size) & (subset["number of nodes"] == num_node)]
    #                 # print(subsubset)
    #                 mean = subsubset["execution_time[us]"].mean()
    #                 count = subsubset["execution_time[us]"].count()
    #                 std = subsubset["execution_time[us]"].std()
    #                 print(f"U250, {collective}, num nodes:{num_node}, seg size:{1024}, num banks:{3}, data size:{data_size}, mean:{mean}, std:{std}, count:{count}")
        
    #check collectives (> 4 nodes)
    # num_nodes = (5,6,7,8)
    # subset = df[(df["collective name"] != "Send/recv") & (df["number of nodes"] > 4 )]
    # for num_node in num_nodes:
    #     for collective in collectives:
    #         if collective != "Send/recv":
    #             for data_size in data_sizes:
    #                 subsubset = subset[(subset["collective name"] == collective) & (subset["segment_size[KB]"] == 1024) & (subset["buffer size[KB]"] == data_size) & (subset["number of nodes"] == num_node)]
    #                 print(subsubset)
    #                 mean = subsubset["execution_time[us]"].mean()
    #                 count = subsubset["execution_time[us]"].count()
    #                 std = subsubset["execution_time[us]"].std()
    #                 print(f"{collective}, num nodes:{num_node}, seg size:{1024}, data size:{data_size}, mean:{mean}, std:{std}, count:{count}")

def compare_openMPI(df, H2H=True, F2F=True):
    df              = df[ (df["rank id"] == 0) & ( (df["number of nodes"]==4)  | (df["collective name"] == "Send/recv"))]
    collectives     = df["collective name"].unique()
    segment_size    = 1024
    for collective in collectives:
        print(collective)
        subset              = df[(df["collective name"] == collective) & (df["segment_size[KB]"] == segment_size) &
                                    (  
                                    ( ( df["board_instance"] == "xilinx_u280_xdma_201920_3") & (df["number of banks"] == 6)) |
                                    ( ( df["board_instance"] == "xilinx_u250_gen3x16_xdma_shell_3_1") & (df["number of banks"] == 3))                                     
                                    )]
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
            print(group)
            exe          = group['execution_time[us]']['mean'].to_numpy()
            exe_std      = group['execution_time[us]']['std'].to_numpy()
            bufsize      = group['buffer size[KB]'].to_numpy()*1024
            exe_full     = group['execution_time_fullpath[us]']['mean'].to_numpy()
            exe_full_std = group['execution_time_fullpath[us]']['std'].to_numpy()

            board = simplify_board_name(board)
            i+=1
            if np.any(exe != 0) and F2F:
                series_label.append(f"{board} F2F")
                series_y.append(exe)
                series_x.append(bufsize)
                stdevs.append(exe_std)
                styles.append(f"C{i}-")
            if np.any(exe_full != 0) and H2H:
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
                if average_delta is not None and F2F:
                    series_label.append(f"{board} F2F")
                    exe = list(map(sum, zip(exe_full,average_delta)))
                    series_y.append(exe)
                    series_x.append(bufsize[:len(exe)])
                    stdevs.append(None)
                    styles.append(f"C{i}-")
                if H2H:
                    series_label.append(f"{board} H2H")
                    series_y.append(exe_full)
                    series_x.append(bufsize)
                    stdevs.append(exe_full_std)
                    styles.append(f"C{i}--")

        
        plot_lines("compare_OMPI"+("H2H" if H2H else "") + ("F2F" if F2F else "")+collective.replace("/", ""), series_x, series_y, series_label, styles, y_label='Latency [us]', logx=True, legend_loc ="upper left")
        #plot_clustered_bars(collective, series_x, series_y, series_label)

def compare_board(df,H2H=True, F2F=True):
    df              = df[ (df["rank id"] == 0) & ( (df["number of nodes"]==4)  | (df["collective name"] == "Send/recv"))]
    collectives     = df["collective name"].unique()
    segment_size    = 1024
    for collective in collectives:
        print(collective)
        subset              = df[   (df["collective name"] == collective) & 
                                    (
                                        ((df["segment_size[KB]"] == segment_size) &
                                            (  
                                                ( ( df["board_instance"] == "xilinx_u280_xdma_201920_3") & (df["number of banks"] == 6)) |
                                                ( ( df["board_instance"] == "xilinx_u250_gen3x16_xdma_shell_3_1") & (df["number of banks"] == 3))                                     
                                            )
                                        )| (df["board_instance"] != "OpenMPI" ))]
        grouped             = subset.groupby(["board_instance", "buffer size[KB]"]).agg({'execution_time[us]':['mean','std'], 'execution_time_fullpath[us]':['mean','std']})
        grouped.reset_index(inplace=True)
        grouped             = grouped.groupby(["board_instance"])
        series_label = []
        series_y     = []
        series_x     = []
        styles       = []
        stdevs       = []

        i = 0
        for board, group in grouped:
            print(group)
            exe          = group['execution_time[us]']['mean'].to_numpy()
            exe_std      = group['execution_time[us]']['std'].to_numpy()
            bufsize      = group['buffer size[KB]'].to_numpy()*1024
            exe_full     = group['execution_time_fullpath[us]']['mean'].to_numpy()
            exe_full_std = group['execution_time_fullpath[us]']['std'].to_numpy()

            board = simplify_board_name(board)
            i+=1
            if np.any(exe != 0) and F2F:
                series_label.append(f"{board} F2F")
                series_y.append(exe)
                series_x.append(bufsize)
                stdevs.append(exe_std)
                styles.append(f"C{i}-")
            if np.any(exe_full != 0) and H2H:
                series_label.append(f"{board} H2H")
                series_y.append(exe_full)
                series_x.append(bufsize)
                stdevs.append(exe_full_std)
                styles.append(f"C{i}--")
        
        plot_lines("board_comparison"+("H2H" if H2H else "") + ("F2F" if F2F else "")+collective.replace("/", ""), series_x, series_y, series_label, styles, y_label='Latency [us]', logx=True, legend_loc ="upper left")
        #plot_clustered_bars(collective, series_x, series_y, series_label)

def sendrecv_banks(df,H2H=False, F2F=True):
    df              = df[(df["experiment"] == "u280_sendrecv") & (df["rank id"] == 0) & (df["number of nodes"]==2)  & (df["collective name"] == "Send/recv")]
    collectives     = df["collective name"].unique()
    segment_size    = 1024
    for collective in collectives:
        print(collective)
        subset              = df[(df["collective name"] == collective) & (df["segment_size[KB]"] == segment_size)]
        grouped             = subset.groupby(["number of banks", "buffer size[KB]" ]).agg({'throughput[Gbps]':['mean','std'], 'throughput_fullpath[Gbps]':['mean','std']})
        grouped.reset_index(inplace=True)
        grouped             = grouped.groupby(["number of banks"])
        series_label = []
        series_y     = []
        series_x     = []
        styles       = []
        stdevs       = []

        i = 0
        for banks, group in grouped:
            print(group)
            exe          = group['throughput[Gbps]']['mean'].to_numpy()
            exe_std      = group['throughput[Gbps]']['std'].to_numpy()
            bufsize      = group['buffer size[KB]'].to_numpy()*1024
            exe_full     = group['throughput_fullpath[Gbps]']['mean'].to_numpy()
            exe_full_std = group['throughput_fullpath[Gbps]']['std'].to_numpy()

            i+=1
            spare_buffer_bank = banks - 1
            if np.any(exe != 0) and F2F:
                series_label.append(f"U280 {spare_buffer_bank} banks F2F")
                series_y.append(exe)
                series_x.append(bufsize)
                stdevs.append(exe_std)
                styles.append(f"C{i}-")
            if np.any(exe_full != 0) and H2H:
                series_label.append(f"U280 {spare_buffer_bank} banks H2H")
                series_y.append(exe_full)
                series_x.append(bufsize)
                stdevs.append(exe_full_std)
                styles.append(f"C{i}--")

        
        plot_lines("U280_Bank"+("H2H" if H2H else "") + ("F2F" if F2F else "")+collective.replace("/", ""), series_x, series_y, series_label, styles, y_errors=stdevs, logx=True, logy=False, throughput=True)
        #plot_clustered_bars(collective, series_x, series_y, series_label)
            

def sendrecv_segmentation(df,H2H=False, F2F=True):
    df              = df[(df["experiment"] == "u280_sendrecv") & (df["rank id"] == 0) & (df["number of nodes"]==2)  & (df["collective name"] == "Send/recv")]
    collectives     = df["collective name"].unique()
    segment_size    = (1024,512,256,128)
    num_bank        = 6
    for collective in collectives:
        print(collective)
        subset              = df[(df["collective name"] == collective) & (df["number of banks"] == num_bank)]
        grouped             = subset.groupby(["segment_size[KB]", "buffer size[KB]" ]).agg({'throughput[Gbps]':['mean','std'], 'throughput_fullpath[Gbps]':['mean','std']})
        grouped.reset_index(inplace=True)
        grouped             = grouped.groupby(["segment_size[KB]"])
        series_label = []
        series_y     = []
        series_x     = []
        styles       = []
        stdevs       = []

        i = 0
        for seg_size, group in grouped:
            print(group)
            exe          = group['throughput[Gbps]']['mean'].to_numpy()
            exe_std      = group['throughput[Gbps]']['std'].to_numpy()
            bufsize      = group['buffer size[KB]'].to_numpy()*1024
            exe_full     = group['throughput_fullpath[Gbps]']['mean'].to_numpy()
            exe_full_std = group['throughput_fullpath[Gbps]']['std'].to_numpy()

            i+=1
            if np.any(exe != 0) and F2F:
                series_label.append(f"U280 Seg Size {seg_size} KB  F2F")
                series_y.append(exe)
                series_x.append(bufsize)
                stdevs.append(exe_std)
                styles.append(f"C{i}-")
            if np.any(exe_full != 0) and H2H:
                series_label.append(f"U280 Seg Size {seg_size} KB H2H")
                series_y.append(exe_full)
                series_x.append(bufsize)
                stdevs.append(exe_full_std)
                styles.append(f"C{i}--")

        
        plot_lines("U280_Segment"+("H2H" if H2H else "") + ("F2F" if F2F else "")+collective.replace("/", ""), series_x, series_y, series_label, styles, y_errors=stdevs, logx=True, logy=False, throughput=True)
        #plot_clustered_bars(collective, series_x, series_y, series_label)
            



def compare_rank_number_and_bsize(df, H2H=False, F2F=True):
    df              = df[ (df["rank id"] == 0) ]
    collectives     = df["collective name"].unique()
    segment_size    = 1024
    for collective in collectives:
        series_label = []
        series_y     = []
        series_x     = []
        styles       = []
        stdevs       = []
        average_delta = None
        subset              = df[ (df["collective name"] == collective) & 
                                  (
                                    ((df["segment_size[KB]"] == segment_size) & 
                                     (  
                                        ( ( df["board_instance"] == "xilinx_u280_xdma_201920_3") & (df["number of banks"] == 6)) |
                                        ( ( df["board_instance"] == "xilinx_u250_gen3x16_xdma_shell_3_1") & (df["number of banks"] == 3))                                     
                                     )
                                    ) | (df["board_instance"] != "OpenMPI" ))]
        grouped             = subset.groupby(["number of nodes", "buffer size[KB]"]).agg({'execution_time[us]':['mean','std'], 'execution_time_fullpath[us]':['mean','std']})
        grouped.reset_index(inplace=True)
        print(collective, grouped)
        nodes_available = [4,5,6,7]
        ls = ['-', '--', '-.', ':']
        j=0
        for  num_nodes, sub_group in grouped.groupby(["number of nodes"]):
            if num_nodes not in nodes_available:
                continue
            exe          = sub_group['execution_time[us]']['mean'].to_numpy()
            exe_std      = sub_group['execution_time[us]']['std'].to_numpy()
            bufsize      = sub_group['buffer size[KB]'].to_numpy()*1024
            exe_full     = sub_group['execution_time_fullpath[us]']['mean'].to_numpy()
            exe_full_std = sub_group['execution_time_fullpath[us]']['std'].to_numpy()
            

            if np.any(exe != 0) and F2F:
                series_label.append(f"ACCL {num_nodes} F2F")
                series_y.append(exe)
                series_x.append(bufsize)
                stdevs.append(exe_std)
                styles.append(f"C4{ls[j % len(ls)]}+")
            if np.any(exe_full != 0) and H2H:
                series_label.append(f"ACCL {num_nodes} H2H")
                series_y.append(exe_full)
                series_x.append(bufsize)
                stdevs.append(exe_full_std)
                styles.append(f"C4{ls[j % len(ls)]}+")
            j+=1
        
        #OpenMPI
        subset              = df[(df["collective name"] == collective) & (df["board_instance"] == "OpenMPI" )]
        grouped             = subset.groupby(["number of nodes", "buffer size[KB]"]).agg({'execution_time[us]':['mean','std'], 'execution_time_fullpath[us]':['mean','std']})
        grouped.reset_index(inplace=True)
        print(grouped)

        j=0
        for  num_nodes, sub_group in grouped.groupby(["number of nodes"]):
            if num_nodes not in nodes_available:
                continue
            exe          = sub_group['execution_time[us]']['mean'].to_numpy()
            exe_std      = sub_group['execution_time[us]']['std'].to_numpy()
            bufsize      = sub_group['buffer size[KB]'].to_numpy()*1024
            exe_full     = sub_group['execution_time_fullpath[us]']['mean'].to_numpy()
            exe_full_std = sub_group['execution_time_fullpath[us]']['std'].to_numpy()
            if np.any(exe_full != 0):
                if average_delta is not None and F2F:
                    series_label.append(f"OpenMPI {num_nodes} F2F")
                    exe = list(map(sum, zip(exe_full,average_delta)))
                    series_y.append(exe)
                    series_x.append(bufsize[:len(exe)])
                    stdevs.append(None)
                    styles.append(f"C3{ls[j % len(ls)]}+")

                if H2H or F2F and average_delta is None:
                    series_label.append(f"OpenMPI {num_nodes} H2H")
                    series_y.append(exe_full)
                    series_x.append(bufsize)
                    stdevs.append(exe_full_std)
                    styles.append(f"C3{ls[j % len(ls)]}+")
            j+=1
     
        
        plot_lines("rank_comparison"+collective.replace("/", ""), series_x, series_y, series_label, styles, y_label='Latency [us]',legend_loc ="upper left")

        #plot_clustered_bars(collective, series_x, series_y, series_label)

def compare_rank_with_fixed_bsize(df, H2H=True, F2F=True):
    df              = df[ (df["rank id"] == 0) ]
    collectives     = df["collective name"].unique()
    bsizes           = df[ "buffer size[KB]"].unique()
    segment_size    = 1024
    for collective in collectives:
        ls = ['-', '--', '-.', ':']
        for bsize in bsizes:
            series_label = []
            series_y     = []
            series_x     = []
            styles       = []
            stdevs       = []
            average_delta = None
            subset              = df[(df["collective name"] == collective) &
                                     (df["buffer size[KB]"] == bsize) & 
                                     (df["segment_size[KB]"] == segment_size) & 
                                     (df["number of nodes"] > 2) &
                                     (  
                                        ( ( df["board_instance"] == "xilinx_u280_xdma_201920_3")            & (df["number of banks"] == 6)) |
                                        ( ( df["board_instance"] == "xilinx_u250_gen3x16_xdma_shell_3_1")   & (df["number of banks"] == 3))                                     
                                     )]
            grouped             = subset.groupby(["number of nodes"]).agg({'execution_time[us]':['mean','std'], 'execution_time_fullpath[us]':['mean','std']})
            grouped.reset_index(inplace=True)
            print(collective, bsize, grouped)
    
            exe          = grouped['execution_time[us]']['mean'].to_numpy()
            exe_std      = grouped['execution_time[us]']['std'].to_numpy()
            num_nodes    = grouped['number of nodes']
            exe_full     = grouped['execution_time_fullpath[us]']['mean'].to_numpy()
            exe_full_std = grouped['execution_time_fullpath[us]']['std'].to_numpy()
        
            if np.any(exe != 0) and F2F:
                series_label.append("ACCL F2F")
                series_y.append(exe)
                series_x.append(num_nodes)
                stdevs.append(exe_std)
                styles.append(f"C4-+")
            if np.any(exe_full != 0) and H2H:
                series_label.append("ACCL H2H")
                series_y.append(exe_full)
                series_x.append(num_nodes)
                stdevs.append(exe_full_std)
                styles.append(f"C4--+")
            average_delta = np.abs(exe_full - exe)
            #OpenMPI
            subset              = df[(df["collective name"] == collective) & (df[ "buffer size[KB]"] == bsize) & (df["board_instance"] == "OpenMPI" ) & (df["number of nodes"] > 2)]
            grouped             = subset.groupby(["number of nodes"]).agg({'execution_time[us]':['mean','std'], 'execution_time_fullpath[us]':['mean','std']})
            grouped.reset_index(inplace=True)

 
            exe          = grouped['execution_time[us]']['mean'].to_numpy()
            exe_std      = grouped['execution_time[us]']['std'].to_numpy()
            num_nodes    = grouped['number of nodes'].to_numpy()
            exe_full     = grouped['execution_time_fullpath[us]']['mean'].to_numpy()
            exe_full_std = grouped['execution_time_fullpath[us]']['std'].to_numpy()
            if np.any(exe_full != 0):
                if average_delta is not None and F2F:
                    series_label.append("OpenMPI F2F")
                    exe = list(map(sum, zip(exe_full,average_delta)))
                    series_y.append(exe)
                    series_x.append(num_nodes[:len(exe)])
                    stdevs.append(None)
                    styles.append(f"C3-+")

                if H2H or F2F and average_delta is None:
                    series_label.append("OpenMPI  H2H")
                    series_y.append(exe_full)
                    series_x.append(num_nodes)
                    stdevs.append(exe_full_std)
                    styles.append(f"C3--+")

            
            plot_lines("rank_comparison"+collective.replace("/", "")+str(bsize), series_x, series_y, series_label, styles, x_label="Number of ranks", y_label='Latency [us]', legend_loc ="upper left", logx=False, logy = False)

            #plot_clustered_bars(collective, series_x, series_y, series_label)
def simplify_board_name(name):
    if   name == "xilinx_u250_gen3x16_xdma_shell_3_1":
        return "U250"
    elif name == "xilinx_u280_xdma_201920_3":
        return "U280"
    else:
        return name

def compare_throughput(df, F2F=True, H2H=True):
    df              = df[ (df["rank id"] == 0)  & ( df["collective name"] == "Send/recv")]
  
    
    segment_size    = 1024
    series_label = []
    series_y     = []
    series_x     = []
    styles       = []
    stdevs       = []
    average_delta = None

    subset              = df[
                                (df["segment_size[KB]"] == segment_size) & 
                                (  
                                ( ( df["board_instance"] == "xilinx_u280_xdma_201920_3")            & (df["number of banks"] == 6)) |
                                ( ( df["board_instance"] == "xilinx_u250_gen3x16_xdma_shell_3_1")   & (df["number of banks"] == 3))                                     
                                )]
    grouped             = subset.groupby(["board_instance", "buffer size[KB]"]).agg({'throughput[Gbps]':['mean','std'], 'throughput_fullpath[Gbps]':['mean','std']})
    grouped.reset_index(inplace=True)
    grouped             = grouped.groupby(["board_instance"])
    for i,(board, group) in enumerate(grouped):
        print(group)
        thr          = group['throughput[Gbps]']['mean'].to_numpy()
        thr_std      = group['throughput[Gbps]']['std'].to_numpy()
        bufsize      = group['buffer size[KB]'].to_numpy()*1024
        thr_full     = group['throughput_fullpath[Gbps]']['mean'].to_numpy()
        thr_full_std = group['throughput_fullpath[Gbps]']['std'].to_numpy()
        board = simplify_board_name(board)
        if np.any(thr != 0) and F2F:
            series_label.append(f"{board} F2F")
            series_y.append(thr)
            series_x.append(bufsize)
            stdevs.append(thr_std)
            styles.append(f"C{i+1}-+")
        if np.any(thr_full != 0) and H2H:
            series_label.append(f"{board} H2H")
            series_y.append(thr_full)
            series_x.append(bufsize)
            stdevs.append(thr_full_std)
            styles.append(f"C{i+1}--+")
        
    #OpenMPI
    subset              = df[( (df["rank id"] == 0) & (df["board_instance"] == "OpenMPI" ) & ( df["collective name"] == "Send/recv") )]
    grouped             = subset.groupby(["buffer size[KB]"]).agg({'throughput[Gbps]':['mean','std'], 'throughput_fullpath[Gbps]':['mean','std']})
    grouped.reset_index(inplace=True)

    print(grouped)
    thr          = grouped['throughput[Gbps]']['mean'].to_numpy()
    thr_std      = grouped['throughput[Gbps]']['std'].to_numpy()
    bufsize      = grouped['buffer size[KB]'].to_numpy()*1024
    thr_full     = grouped['throughput_fullpath[Gbps]']['mean'].to_numpy()
    thr_full_std = grouped['throughput_fullpath[Gbps]']['std'].to_numpy()
    if np.any(thr != 0) and F2F:
            series_label.append("OpenMPI F2F")
            series_y.append(thr)
            series_x.append(bufsize[:len(thr)])
            stdevs.append(thr_std)
            styles.append(f"C3-+")
    if np.any(thr_full != 0) and H2H :
            series_label.append("OpenMPI  H2H")
            series_y.append(thr_full)
            series_x.append(bufsize)
            stdevs.append(thr_full_std)
            styles.append(f"C3--+")

        
    plot_lines("throughput_comparsion", series_x, series_y, series_label, styles, x_label="Message Size", y_label='Throughput [Gbps]', legend_loc ="upper left", logx=True, logy = False)



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

    parser = argparse.ArgumentParser(description='Creates some graphs.')
    parser.add_argument('--statistic'           , action='store_true', default=False,     help='count runs of experiments'                     )
    parser.add_argument('--sendrecv_banks'      , action='store_true', default=False,     help='send recv throughput with different number of banks'                     )
    parser.add_argument('--sendrecv_seg'        , action='store_true', default=False,     help='send recv throughput with different number of segmentation size'                     )
    parser.add_argument('--norm'                , action='store_true', default=False,     help='test normality'                          )
    parser.add_argument('--ssize'               , action='store_true', default=False,     help='ssize vs buffer size'                     )
    parser.add_argument('--board'               , action='store_true', default=False,     help='compare performance of different alveo'   )
    parser.add_argument('--openMPI'             , action='store_true', default=False,     help='compare performance against OpenMPI'   )
    parser.add_argument('--rank1_number'        , action='store_true', default=False,    help='compare performance of different number of ranks'   )
    parser.add_argument('--rank2_number'        , action='store_true', default=False,     help='compare performance of different number of ranks'   )
    parser.add_argument('--throughput'          , action='store_true', default=False,     help='compare throughput'   )
    parser.add_argument('--segment_vs_membank'  , action='store_true', default=False,     help='compare throughput'   )
    

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
    if args.board:
        compare_board(df)
    if args.rank1_number:
        compare_rank_number_and_bsize(df)
    if args.rank2_number:
        compare_rank_with_fixed_bsize(df)
    if args.statistic:
        get_statistics(df)
    if args.openMPI:
        compare_openMPI(df)
        compare_openMPI(df, H2H=False)
        compare_openMPI(df, F2F=False)
    if args.throughput:
        compare_throughput(df)
    if args.sendrecv_banks:
        sendrecv_banks(df)
    if args.sendrecv_seg:
        sendrecv_segmentation(df)
