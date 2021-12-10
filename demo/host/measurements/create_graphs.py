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

    fig, ax = plt.subplots(figsize=(9,6))
    series  = []
    for x, y, y_series_label, y_style, y_error in zip(x_datas, y_datas, y_series_labels, y_styles, y_errors):
        if y_style:
            if not y_error is None:
                series.append(ax.errorbar(x, y,  yerr = y_error, fmt=y_style, label=y_series_label, capsize=4.0, linewidth=1, markersize=8, markeredgewidth=3))
            else:
                line, = ax.plot(x, y, y_style, label=y_series_label, linewidth=1, markersize=8, markeredgewidth=3)
                series.append(line)
        else:
            if not y_error is None:
                series.append(ax.errorbar(x, y,  yerr = y_error, fmt=y_style, label=y_series_label, capsize=4.0, linewidth=1, markersize=8, markeredgewidth=3))
            else:
                line, = ax.plot(x, y, label=y_series_label, linewidth=1, markersize=8, markeredgewidth=3)
                series.append(line)

    plt.grid(axis='y')
    # Add some text for labels, title and custom x-axis tick labels, etc.
    if throughput:
        ax.set_ylabel('Throughput [Gbps]', fontsize=20)
        ax.axis(ymin=0,ymax=100)
    else:
        ax.set_ylabel(y_label,  fontsize=20)
    #ax.set_title(title)
    if logy:
        ax.set_yscale('log')
    
    if logx:
        ax.set_xscale('log', base=2)
        
    if legend_loc is None :
        if logy:
            ax.legend(series, y_series_labels, loc="lower right", handlelength=4)
        else:
            ax.legend(series, y_series_labels, loc="upper left", handlelength=4)
    else:
        ax.legend(    series, y_series_labels, loc=legend_loc, fontsize=14, handlelength=4)

    if x_label == "Message Size":
        ax.xaxis.set_major_formatter(ticker.FuncFormatter(lambda y, _: sizeof_fmt(y)))
    plt.xticks(rotation=0, fontsize=18)
    plt.yticks(fontsize=18)
    ax.set_xlabel(x_label, fontsize=20)
    # plt.show()
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

    seg_sizes = (2048,1024,512,256,128)
    data_sizes = (1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768)
    # check sendrecv u280
    subset = df[(df["experiment"] == "u280_sendrecv") & (df["collective name"] == "Send/recv") & (df["board_instance"] == "xilinx_u280_xdma_201920_3")]
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

    
    for seg_size in seg_sizes:
        for num_bank in range (2,7):
            for data_size in data_sizes:
                subsubset = subset[(subset["segment_size[KB]"] == seg_size) & (subset["number of banks"] == num_bank) & (subset["buffer size[KB]"] == data_size)]
                # print(subsubset)
                mean = subsubset["throughput[Gbps]"].mean()
                count = subsubset["throughput[Gbps]"].count()
                std = subsubset["throughput[Gbps]"].std()
                # if count < 100:
                print(f"U280, sendrecv, seg size:{seg_size}, num banks:{num_bank}, data size:{data_size}, mean:{mean}, std:{std}, count:{count}")

    # num_nodes = (3,4)
    # #check U280 collectives (<= 4 nodes)
    # subset = df[ (df["board_instance"] == "xilinx_u280_xdma_201920_3") & (df["collective name"] != "Send/recv") & (df["number of nodes"] <= 4 )]
    # for num_node in num_nodes:
    #     for collective in collectives:
    #         if collective != "Send/recv":
    #             for data_size in data_sizes:
    #                 subsubset = subset[(subset["collective name"] == collective) &(subset["segment_size[KB]"] == 1024) & (subset["number of banks"] == 6) & (subset["buffer size[KB]"] == data_size) & (subset["number of nodes"] == num_node)]
    #                 print(subsubset)
    #                 mean = subsubset["execution_time[us]"].mean()
    #                 count = subsubset["execution_time[us]"].count()
    #                 std = subsubset["execution_time[us]"].std()
    #                 if count < 100:
    #                     print(f"U280, {collective}, num nodes:{num_node}, seg size:{1024}, num banks:{6}, data size:{data_size}, mean:{mean}, std:{std}, count:{count}")

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

def compare_openMPI(df, H2H=True, F2F=True, error=False):
    df              = df[ (df["rank id"] == 0) & ( (df["number of nodes"]==4)  | (df["collective name"] == "Send/recv"))]
    collectives     = df["collective name"].unique()
    segment_size    = 1024
    for collective in collectives:
        print(collective)
        subset              = df[(df["collective name"] == collective) & (df["segment_size[KB]"] == segment_size) &
                                    (  
                                    ( ( df["board_instance"] == "xilinx_u280_xdma_201920_3") & (df["number of banks"] == 6))                                   
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
                series_label.append(f"ACCL F2F")
                series_y.append(exe)
                series_x.append(bufsize)
                stdevs.append(exe_std)
                styles.append(f"C{2}+-")
            if np.any(exe_full != 0) and H2H:
                series_label.append(f"ACCL H2H")
                series_y.append(exe_full)
                series_x.append(bufsize)
                stdevs.append(exe_full_std)
                styles.append(f"C{2}+--")

            if board.find("U280") != -1:
                average_delta = np.abs(exe_full - exe)
        #For OpenMPI
        subset              = df[(df["collective name"] == collective) & ((df["board_instance"] == "OpenMPI 4_1 new" ) )]
        grouped             = subset.groupby(["board_instance", "buffer size[KB]"]).agg({'execution_time[us]':['mean','std'], 'execution_time_fullpath[us]':['mean','std']})
        grouped.reset_index(inplace=True)
        grouped             = grouped.groupby(["board_instance"])
        i = 2
        for board, group in grouped:
            exe          = group['execution_time[us]']['mean'].to_numpy()
            exe_std      = group['execution_time[us]']['std'].to_numpy()
            bufsize      = group['buffer size[KB]'].to_numpy()*1024
            exe_full     = group['execution_time_fullpath[us]']['mean'].to_numpy()
            exe_full_std = group['execution_time_fullpath[us]']['std'].to_numpy()

            board = simplify_board_name(board)
            i+=1
            
            if np.any(exe) and F2F:
                series_label.append(f"{board} F2F")
                series_y.append(exe)
                series_x.append(bufsize)
                stdevs.append(exe_std)
                styles.append(f"C{i}+-")
            if np.any(exe_full != 0) and H2H:
                series_label.append(f"{board} H2H")
                series_y.append(exe_full)
                series_x.append(bufsize)
                stdevs.append(exe_full_std)
                styles.append(f"C{i}+--")

        
        plot_lines("4_1_compare_OMPI"+("H2H" if H2H else "") + ("F2F" if F2F else "")+collective.replace("/", ""), series_x, series_y, series_label, styles, y_label='Latency [us]', logx=True, legend_loc ="upper left", y_errors=(stdevs if error else None))

def plot_box_plot(title,xs, ys, y_labels, xlabel, ylabel, logy=False, logx=True):

    def set_box_color(bp, color):
        plt.setp(bp['boxes'],    color=color)
        plt.setp(bp['whiskers'], color=color)
        plt.setp(bp['caps'],     color=color)
        plt.setp(bp['medians'],  color=color)
    colours = ['#d7191c', '#fdae61', '#abd9e9', '#2c7bb6', "#7b3294"]
    plt.figure()
    if xs is None:
        xs = [None for _ in y_labels]
    for (i, ( y,x,  y_label, colour)) in enumerate(zip( ys,xs, y_labels, colours)):
        #positions = np.array(range(i, len(y)*len(ys), len(ys)))
        #print(positions)
        if x is not None:
            bpl = plt.boxplot(y, positions=x, widths=1/len(ys))
        else:
            bpl = plt.boxplot(y,  widths=1/len(ys))
        set_box_color(bpl, colour) 
        plt.plot([], c=colour, label=y_label)

    plt.ylabel(ylabel,  fontsize=20)
    plt.xlabel(xlabel,  fontsize=20)
    #ax.set_title(title)
    if logy:
        plt.yscale('log')
    if logx:
        plt.xscale('log', base=2)
    if xlabel == "Message Size":
        plt.gca().xaxis.set_major_formatter(ticker.FuncFormatter(lambda y, _: sizeof_fmt(y)))
    plt.legend()

    #plt.xticks(range(0, len(xs[0]) * 2, 2), xs[0])
    #plt.xlim(-2, len(xs[0])*2)
    plt.savefig(f"{title}.png", format='png',bbox_inches='tight')

def compare_box_plot(df, F2F=True, H2H=True):
    selection_params = [{"collective name":"Send/recv",
                        "Segment_size[KB]":1024,
                        "board_instance":"xilinx_u280_xdma_201920_3", 
                        "number of banks":6},
                        {"collective name":"Send/recv",
                        "Segment_size[KB]":1024,
                        "board_instance":"xilinx_u250_gen3x16_xdma_shell_3_1", 
                        "number of banks":3}, 
                        {"collective name":"Send/recv",
                        "Segment_size[KB]":0,
                        "board_instance":"OpenMPI", 
                        "number of banks":0}]
    series_label    = []
    series_y        = []
    series_x        = []

    for selection_param in selection_params:

        collective_name = selection_param["collective name"]
        seg_size        = selection_param["Segment_size[KB]"]
        board           = selection_param["board_instance"]
        num_banks       = selection_param["number of banks"]

        subset              = df[   (df["rank id"]         == 0) & 
                                    (df["collective name"] == collective_name) & (df["segment_size[KB]"] == seg_size ) &
                                    (df["board_instance"]  == board)           & (df["number of banks"]  == num_banks)
                                ]
        tmp_0y = []
        tmp_0x = []
        tmp_1y = []
        tmp_1x = []
        grouped             = subset.groupby(["buffer size[KB]"])
        for bsize, group in grouped:
            exe          = group['execution_time[us]'].to_numpy()
            exe_full     = group['execution_time_fullpath[us]'].to_numpy()
            board = simplify_board_name(board)
        
            if np.any(exe != 0) and F2F:
                tmp_0y.append(exe)
                tmp_0x.append(bsize*1024)
            if np.any(exe_full != 0) and H2H:
                tmp_1y.append(exe_full)
                tmp_1x.append(bsize*1024)
        
        if F2F and len(tmp_0x) > 0:
            series_label.append(f"{board} F2F")
            series_y.append(tmp_0y)
            series_x.append(tmp_0x)
        if H2H  and len(tmp_1x) > 0:
            series_label.append(f"{board} H2H")
            series_y.append(tmp_1y)
            series_x.append(tmp_1x)

    plot_box_plot("send_recv_distribution", series_x, series_y, series_label,  "Message Size", "Latency [uS]", logy=True)

def compare_box_plot_with_fixed_bsize(df):
    selection_params = [{"collective name":"Send/recv",
                        "Segment_size[KB]":1024,
                        "board_instance":"xilinx_u280_xdma_201920_3", 
                        "number of banks":6},
                        {"collective name":"Send/recv",
                        "Segment_size[KB]":1024,
                        "board_instance":"xilinx_u250_gen3x16_xdma_shell_3_1", 
                        "number of banks":3}, 
                        {"collective name":"Send/recv",
                        "Segment_size[KB]":0,
                        "board_instance":"OpenMPI", 
                        "number of banks":0}]

    for bsize in df["buffer size[KB]"].unique():
        series_label    = []
        series_y        = []
        series_x        = []
        i = 0
        for selection_param in selection_params:

            collective_name = selection_param["collective name"]
            seg_size        = selection_param["Segment_size[KB]"]
            board           = selection_param["board_instance"]
            num_banks       = selection_param["number of banks"]

            subset              = df[   (df["rank id"]         == 0)               & (df["buffer size[KB]"]  == bsize)     &
                                        (df["collective name"] == collective_name) & (df["segment_size[KB]"] == seg_size ) &
                                        (df["board_instance"]  == board)           & (df["number of banks"]  == num_banks) ]
            if board in ["xilinx_u280_xdma_201920_3","xilinx_u250_gen3x16_xdma_shell_3_1"]:
                F2F = True
                H2H = False
            else:
                F2F = False
                H2H = True
            print(subset)
            tmp_0y = []
            tmp_0x = []
            tmp_1y = []
            tmp_1x = []
            grouped             = subset.groupby(["buffer size[KB]"])
            for bsize, group in grouped:
                exe          = group['execution_time[us]'].to_numpy()
                exe_full     = group['execution_time_fullpath[us]'].to_numpy()
                print(board, F2F, H2H, exe)
                if np.any(exe != 0) and F2F:
                    tmp_0y.append(exe)
                    tmp_0x.append(i)
                    i+=1
                if np.any(exe_full != 0) and H2H:
                    tmp_1y.append(exe_full)
                    tmp_1x.append(i)
                    i+=1
            
            board = simplify_board_name(board)
            if len(tmp_0y) > 0:
                series_label.append(f"{board}")
                series_y.append(tmp_0y)
                series_x.append(tmp_0x)
            if len(tmp_1y) > 0:
                series_label.append(f"{board}")
                series_y.append(tmp_1y)
                series_x.append(tmp_1x)

        plot_box_plot(f"send_recv_distribution_bsize{bsize}", series_x, series_y, series_label,  "", "Latency [uS]", logy=False, logx=False)

def compare_board(df,H2H=True, F2F=True, error =False):
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
                series_label.append(f"ACCL {board} F2F")
                series_y.append(exe)
                series_x.append(bufsize)
                stdevs.append(exe_std)
                styles.append(f"C{i}-")
            if np.any(exe_full != 0) and H2H:
                series_label.append(f"ACCL {board} H2H")
                series_y.append(exe_full)
                series_x.append(bufsize)
                stdevs.append(exe_full_std)
                styles.append(f"C{i}--")
        
        plot_lines("4_1_board_comparison"+("H2H" if H2H else "") + ("F2F" if F2F else "")+collective.replace("/", ""), series_x, series_y, series_label, styles, y_label='Latency [us]', logx=True, legend_loc ="upper left", y_errors=(stdevs if error else None))
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
            
def compare_rank_number_and_bsize(df, H2H=False, F2F=True, error = False):
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
     
        
        plot_lines("rank_comparison"+collective.replace("/", ""), series_x, series_y, series_label, styles, y_label='Latency [us]',legend_loc ="upper left" , y_errors=(stdevs if error else None))

        #plot_clustered_bars(collective, series_x, series_y, series_label)

def compare_rank_with_fixed_bsize(df, H2H=True, F2F=True, error=False):
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
                styles.append(f"C2-+")
            if np.any(exe_full != 0) and H2H:
                series_label.append("ACCL H2H")
                series_y.append(exe_full)
                series_x.append(num_nodes)
                stdevs.append(exe_full_std)
                styles.append(f"C2--+")
            
            #OpenMPI
            subset              = df[(df["collective name"] == collective) & (df[ "buffer size[KB]"] == bsize) & ( (df["board_instance"] == "OpenMPI 4_1 new" ) ) & (df["number of nodes"] > 2)]
            i=2
            for board_name, group in subset.groupby(["board_instance"]): 
                grouped             = group.groupby(["number of nodes"]).agg({'execution_time[us]':['mean','std'], 'execution_time_fullpath[us]':['mean','std']})
                grouped.reset_index(inplace=True)

    
                exe          = grouped['execution_time[us]']['mean'].to_numpy()
                exe_std      = grouped['execution_time[us]']['std'].to_numpy()
                num_nodes    = grouped['number of nodes'].to_numpy()
                exe_full     = grouped['execution_time_fullpath[us]']['mean'].to_numpy()
                exe_full_std = grouped['execution_time_fullpath[us]']['std'].to_numpy()
                i+=1
                board_name = simplify_board_name(board_name)
                if np.any(exe != 0) and F2F:
                    series_label.append(f"{board_name} F2F")
                    series_y.append(exe)
                    series_x.append(num_nodes[:len(exe)])
                    stdevs.append(exe_std)
                    styles.append(f"C{i}-+")

                if np.any(exe_full) and H2H:
                        series_label.append(f"{board_name} H2H")
                        series_y.append(exe_full)
                        series_x.append(num_nodes)
                        stdevs.append(exe_full_std)
                        styles.append(f"C{i}--+")

            
            plot_lines("4_1_rank_comparison"+collective.replace("/", "")+str(bsize), series_x, series_y, series_label, styles, x_label="Number of ranks", y_label='Latency [us]', legend_loc ="upper left", logx=False, logy = False, y_errors=(stdevs if error else None))

            #plot_clustered_bars(collective, series_x, series_y, series_label)

def simplify_board_name(name):
    if   name == "xilinx_u250_gen3x16_xdma_shell_3_1":
        return "U250"
    elif name == "xilinx_u280_xdma_201920_3":
        return "U280"
    elif name == "OpenMPI 4_1 new":
        return "OpenMPI"
    else:
        return name

def compare_throughput(df):
    df              = df[ (df["rank id"] == 0)  & ( df["collective name"] == "Send/recv")]
  
    segment_size    = 1024
    series_label = []
    series_y     = []
    series_x     = []
    styles       = []
    stdevs       = []

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
        #if np.any(thr != 0) and F2F:
        if np.any(thr != 0) and board== "U280":
            series_label.append(f"ACCL")
            series_y.append(thr)
            series_x.append(bufsize)
            stdevs.append(thr_std)
            styles.append(f"C{i+1}-+")
        #if np.any(thr_full != 0) and H2H:
        #    series_label.append(f"ACCL {board} H2H")
        #    series_y.append(thr_full)
        #    series_x.append(bufsize)
        #    stdevs.append(thr_full_std)
        #    styles.append(f"C{i+1}--+")
        
    #OpenMPI
    subset              = df[( (df["rank id"] == 0) & ((df["board_instance"] == "OpenMPI 4_1 new" )) & ( df["collective name"] == "Send/recv") )]
    grouped             = subset.groupby(["board_instance","buffer size[KB]"]).agg({'throughput[Gbps]':['mean','std'], 'throughput_fullpath[Gbps]':['mean','std']})
    grouped.reset_index(inplace=True)
    grouped             = grouped.groupby(["board_instance"])
    i = 2
    for board, group in grouped:
        print(group)
        thr          = group['throughput[Gbps]']['mean'].to_numpy()
        thr_std      = group['throughput[Gbps]']['std'].to_numpy()
        bufsize      = group['buffer size[KB]'].to_numpy()*1024
        thr_full     = group['throughput_fullpath[Gbps]']['mean'].to_numpy()
        thr_full_std = group['throughput_fullpath[Gbps]']['std'].to_numpy()
        #if np.any(thr != 0) and F2F:
        #        series_label.append("OpenMPI F2F")
        #        series_y.append(thr)
        #        series_x.append(bufsize[:len(thr)])
        #        stdevs.append(thr_std)
        #        styles.append(f"C3-+")
        #if np.any(thr_full != 0) and H2H :
        board = simplify_board_name(board)
        if np.any(thr_full != 0) :
                series_label.append(f"{board}")
                series_y.append(thr_full)
                series_x.append(bufsize)
                stdevs.append(thr_full_std)
                styles.append(f"C{3}--+")
        i+=1

        
    plot_lines("throughput_comparsion_4_1", series_x, series_y, series_label, styles, x_label="Message Size", y_label='Throughput [Gbps]', legend_loc ="upper left", logx=True, logy = False)

def segment_vs_membank(df):
    df              = df[ (df["experiment"] == "u280_sendrecv") & (df["rank id"] == 0)  & ( df["collective name"] == "Send/recv")]
    
    for board in ["xilinx_u280_xdma_201920_3" , "xilinx_u250_gen3x16_xdma_shell_3_1"]:
        data_to_be_plotted  = []
        subset          = df[(df["board_instance"] == board)]
        if subset["number of banks"].count() == 0:
            continue
        max_banks       = subset["number of banks"].max()-1
        min_banks       = subset["number of banks"].min()-1

        
        banks           = list(range(min_banks,max_banks+1))
        segment_sizes   = list(sorted(subset["segment_size[KB]"].unique()))
        grouped         = subset.groupby(["number of banks", "segment_size[KB]", "buffer size[KB]"]).agg({'throughput[Gbps]':['mean','std'], 'throughput_fullpath[Gbps]':['mean','std']})
        
        for _ in banks:
            data_to_be_plotted.append([ 0 for _ in segment_sizes])

        grouped.reset_index(inplace=True)
        for ((curr_num_banks, curr_segment ),  group) in grouped.groupby(["number of banks", "segment_size[KB]"]):
            thr          = group['throughput[Gbps]']['mean'].max()
            print(curr_num_banks, curr_segment, thr)

            data_to_be_plotted[curr_num_banks-1-min_banks][np.argwhere(segment_sizes == curr_segment)[0][0]] = thr
        


        fig, ax = plt.subplots()
        im = ax.imshow(data_to_be_plotted, cmap="Wistia" )
        #im = ax.imshow(data_to_be_plotted, cmap="Wistia", vmin=0, vmax=100 )
        ax.invert_yaxis()
        #ax.invert_xaxis()
        cbar = ax.figure.colorbar(im, ax=ax)
        cbar.ax.tick_params(labelsize=14)
        # We want to show all ticks...
        ax.set_yticks(np.arange(len(banks)))
        ax.set_xticks(np.arange(len(segment_sizes)))
        # ... and label them with the respective list entries
        ax.set_yticklabels(banks,           fontsize=14)
        ax.set_xticklabels(segment_sizes,   fontsize=14)
        ax.set_ylabel("Number of Banks",    fontsize=16)
        ax.set_xlabel("Segment size [KB]",  fontsize=16)
        # Loop over data dimensions and create text annotations.
        for i in range(len(banks)):
            for j in range(len(segment_sizes)):
                if data_to_be_plotted[i][j] > 0:
                    d = data_to_be_plotted[i][j]
                    ax.text(j, i, f"{d:.1f}", ha="center", va="center", fontsize=14)
        
        ax.set_title("Peak throughput [Gbps]", fontsize=16)
        plt.show()
        plt.savefig(f"segment_vs_bank_{board}.png", format='png', bbox_inches='tight')

def optimized_vs_base(df, selection_params,  error = False, logy=False, figure_label=""):
    df              = df[ (df["rank id"] == 0) ]

    series_label = []
    series_y     = []
    series_x     = []
    styles       = []
    stdevs       = []
    i=0
    collectives  = []
    for selection_param in selection_params:
        exp             = selection_param["experiment"]
        label           = selection_param["label"]
        collective_name = selection_param["collective name"]
        seg_size        = selection_param["Segment_size[KB]"]
        board           = selection_param["board_instance"]
        num_banks       = selection_param["number of banks"]
        num_nodes       = selection_param["number of nodes"]
        F2F             = selection_param["F2F"]
        H2H             = selection_param["H2H"]
        collectives.append(collective_name.replace("/", ""))

        subset              = df[(df["experiment"]      == exp)             &
                                 (df["collective name"] == collective_name) & (df["segment_size[KB]"]   == seg_size ) &
                                 (df["board_instance"]  == board)           & (df["number of banks"]    == num_banks) &
                                 (df["number of nodes"] == num_nodes)]
        grouped             = subset.groupby(["buffer size[KB]"]).agg({'execution_time[us]':['mean','std'], 'execution_time_fullpath[us]':['mean','std']})
        grouped.reset_index(inplace=True)
        print(grouped)
        

        exe          = grouped['execution_time[us]']['mean'].to_numpy()
        exe_std      = grouped['execution_time[us]']['std'].to_numpy()
        bufsize      = grouped['buffer size[KB]'].to_numpy()*1024
        exe_full     = grouped['execution_time_fullpath[us]']['mean'].to_numpy()
        exe_full_std = grouped['execution_time_fullpath[us]']['std'].to_numpy()

        board = simplify_board_name(board)
        if np.any(exe != 0) and F2F:
            series_label.append(f"{label} F2F")
            series_y.append(exe)
            series_x.append(bufsize)
            stdevs.append(exe_std)
            linestyle = selection_param["line style"] if selection_param["line style"] else f"C{i}-"
            styles.append(linestyle)
            i+=1
        if np.any(exe_full != 0) and H2H:
            series_label.append(f"{label} H2H")
            series_y.append(exe_full)
            series_x.append(bufsize)
            stdevs.append(exe_full_std)
            linestyle = selection_param["line style"] if selection_param["line style"] else f"C{i}-"
            styles.append(linestyle)
            i+=1

    #optimized version
    figure_label = (figure_label if figure_label != "" else ("comparison"+"_".join(collectives)))
    plot_lines(figure_label, series_x, series_y, series_label, styles, y_label='Latency [us]', logx=True, logy=logy, legend_loc ="upper left", y_errors=(stdevs if error else None))
        
def remove_multiple_headers(df):
    headers = df.columns.tolist()
    df = df[df[headers[0]]!=headers[0]].reset_index(drop=True)
    for column_name in ["number of nodes","rank id","number of banks","buffer size[KB]","segment_size[KB]","execution_time[us]","throughput[Gbps]","execution_time_fullpath[us]","throughput_fullpath[Gbps]"]:
        df[column_name] = pd.to_numeric(df[column_name])
    return df
def load_csvs_under(path):

    csv_files    = [join(path, f) for f in listdir(path)  if (isfile(join(path, f)) and f.find(".csv") != -1)]
    print("csv files ingested", csv_files)
    csvs = []
    for csv_path in csv_files:
        csvs.append(pd.read_csv(csv_path))
    return pd.concat(csvs)

if __name__ == "__main__":
    #mypath ="C:\\Users\\danielep\\Documents\\github\\ACCL_measure"
    accl_path            ="accl"
    openmpi_path         ="open_mpi"

    df = pd.concat([load_csvs_under(accl_path), load_csvs_under(openmpi_path)])
    #filter multiple headers
    df = remove_multiple_headers(df)

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
    parser.add_argument('--segment_vs_membank'  , action='store_true', default=False,     help='compare throughput changing segment size and number of memory banks used'   )
    parser.add_argument('--optimized_vs_base'   , action='store_true', default=False,     help='comapre execution time of bcast when optimized'   )
    

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
        compare_rank_with_fixed_bsize(df, error=True)
    if args.statistic:
        get_statistics(df)
    if args.openMPI:
        compare_openMPI(df, error=False)
        #compare_openMPI(df, H2H=False)
        #compare_openMPI(df, F2F=False)
        #compare_box_plot(df)
        compare_box_plot_with_fixed_bsize(df)
    if args.throughput:
        compare_throughput(df)
    if args.sendrecv_banks:
        sendrecv_banks(df)
    if args.sendrecv_seg:
        sendrecv_segmentation(df)
    if args.segment_vs_membank:
        segment_vs_membank(df)
    if args.optimized_vs_base:
        other = pd.concat([df, load_csvs_under("accl/dual_path_daniele")])
        other = remove_multiple_headers(other)

        #optimized_vs_base(other,[{ "experiment":"bcast",
        #                        "label":"baseline: dual datapath",
        #                        "collective name":"Broadcast",
        #                        "Segment_size[KB]":1024,
        #                        "board_instance":"xilinx_u280_xdma_201920_3", 
        #                        "number of banks":6,
        #                        "number of nodes": 4,
        #                        "F2F":True,
        #                        "H2H":False},
        #                        { "experiment":"bcast1",
        #                        "label":"baseline: dual datapath 2",
        #                        "collective name":"Broadcast",
        #                        "Segment_size[KB]":1024,
        #                        "board_instance":"xilinx_u280_xdma_201920_3", 
        #                        "number of banks":6,
        #                        "number of nodes": 4,
        #                        "F2F":True,
        #                        "H2H":False}, 
        #                        { "experiment":"bcast_rr",
        #                        "label":"baseline: dual datapath rr",
        #                        "collective name":"Broadcast",
        #                        "Segment_size[KB]":1024,
        #                        "board_instance":"xilinx_u280_xdma_201920_3", 
        #                        "number of banks":6,
        #                        "number of nodes": 4,
        #                        "F2F":True,
        #                        "H2H":False},
        #                        { "experiment":"u280_bcast",
        #                        "label":"single datapath: bcast rr",
        #                        "collective name":"Broadcast",
        #                        "Segment_size[KB]":1024,
        #                        "board_instance":"xilinx_u280_xdma_201920_3", 
        #                        "number of banks":6,
        #                        "number of nodes": 4,
        #                        "F2F":True,
        #                        "H2H":False}, 
        #                        { "experiment":"converted from log",
        #                        "label":"OpenMPI",
        #                        "collective name":"Broadcast",
        #                        "Segment_size[KB]":0,
        #                        "board_instance":"OpenMPI", 
        #                        "number of banks": 0,
        #                        "number of nodes": 4,
        #                        "F2F":False,
        #                        "H2H":True}], logy=True)

        #optimized_vs_base(other,[{ "experiment":"u280_dual_path_scatter",
        #                        "label":"baseline",
        #                        "collective name":"Scatter",
        #                        "Segment_size[KB]":1024,
        #                        "board_instance":"xilinx_u280_xdma_201920_3", 
        #                        "number of banks":6,
        #                        "number of nodes": 4,
        #                        "F2F":True,
        #                        "H2H":False}, 
        #                        { "experiment":"u280_scatter",
        #                        "label":"alternative",
        #                        "collective name":"Scatter",
        #                        "Segment_size[KB]":1024,
        #                        "board_instance":"xilinx_u280_xdma_201920_3", 
        #                        "number of banks":6,
        #                        "number of nodes": 4,
        #                        "F2F":True,
        #                        "H2H":False}])

        #optimized_vs_base(other,[
        #                         { "experiment":"19_dma",
        #                        "label":"dual datapath",
        #                        "collective name":"Reduce",
        #                        "Segment_size[KB]":1024,
        #                        "board_instance":"xilinx_u280_xdma_201920_3", 
        #                        "number of banks":6,
        #                        "number of nodes": 3,
        #                        "F2F":True,
        #                        "H2H":False}, 
        #                        { "experiment":"u280_u250_reduce",
        #                        "label":"single datapath",
        #                        "collective name":"Reduce",
        #                        "Segment_size[KB]":1024,
        #                        "board_instance":"xilinx_u280_xdma_201920_3", 
        #                        "number of banks":6,
        #                        "number of nodes": 3,
        #                        "F2F":True,
        #                        "H2H":False}], logy=True)
        #
        #optimized_vs_base(other,[
        #                         { "experiment":"allreduce",
        #                        "label":"dual datapath",
        #                        "collective name":"Allreduce",
        #                        "Segment_size[KB]":1024,
        #                        "board_instance":"xilinx_u280_xdma_201920_3", 
        #                        "number of banks":6,
        #                        "number of nodes": 3,
        #                        "F2F":True,
        #                        "H2H":False}, 
        #                        { "experiment":"u280_u250_allreduce",
        #                        "label":"single datapath",
        #                        "collective name":"Allreduce",
        #                        "Segment_size[KB]":1024,
        #                        "board_instance":"xilinx_u280_xdma_201920_3", 
        #                        "number of banks":6,
        #                        "number of nodes": 3,
        #                        "F2F":True,
        #                        "H2H":False}], logy=True)
        #
        #optimized_vs_base(other,[
        #                         { "experiment":"allgather",
        #                        "label":"dual datapath",
        #                        "collective name":"Allgather",
        #                        "Segment_size[KB]":1024,
        #                        "board_instance":"xilinx_u280_xdma_201920_3", 
        #                        "number of banks":6,
        #                        "number of nodes": 3,
        #                        "F2F":True,
        #                        "H2H":False}, 
        #                        { "experiment":"u280_u250_allgather",
        #                        "label":"single datapath",
        #                        "collective name":"Allgather",
        #                        "Segment_size[KB]":1024,
        #                        "board_instance":"xilinx_u280_xdma_201920_3", 
        #                        "number of banks":6,
        #                        "number of nodes": 3,
        #                        "F2F":True,
        #                        "H2H":False}], logy=True)
        #
        #optimized_vs_base(other,[
        #                         { "experiment":"gather",
        #                        "label":"dual datapath",
        #                        "collective name":"Gather",
        #                        "Segment_size[KB]":1024,
        #                        "board_instance":"xilinx_u280_xdma_201920_3", 
        #                        "number of banks":6,
        #                        "number of nodes": 3,
        #                        "F2F":True,
        #                        "H2H":False}, 
        #                        { "experiment":"u280_u250_gather",
        #                        "label":"single datapath",
        #                        "collective name":"Gather",
        #                        "Segment_size[KB]":1024,
        #                        "board_instance":"xilinx_u280_xdma_201920_3", 
        #                        "number of banks":6,
        #                        "number of nodes": 3,
        #                        "F2F":True,
        #                        "H2H":False}], logy=True)
        #
        #optimized_vs_base(other,[
        #                         { "experiment":"bcast",
        #                        "label":"dual datapath",
        #                        "collective name":"Broadcast",
        #                        "Segment_size[KB]":1024,
        #                        "board_instance":"xilinx_u280_xdma_201920_3", 
        #                        "number of banks":6,
        #                        "number of nodes": 3,
        #                        "F2F":True,
        #                        "H2H":False}, 
        #                        { "experiment":"u280_u250_bcast",
        #                        "label":"single datapath",
        #                        "collective name":"Broadcast",
        #                        "Segment_size[KB]":1024,
        #                        "board_instance":"xilinx_u280_xdma_201920_3", 
        #                        "number of banks":6,
        #                        "number of nodes": 3,
        #                        "F2F":True,
        #                        "H2H":False}], logy=True)

        #other = pd.concat([df, load_csvs_under("accl/dual_path_daniele"), load_csvs_under("accl/dma_hardenend")])
        #other = remove_multiple_headers(other)
        #experiments_filtering = [
        #            { "experiment":"allreduce",
        #                "label":"dual datapath (BASELINE)",
        #                "collective name":"Allreduce",
        #                "Segment_size[KB]":1024,
        #                "board_instance":"xilinx_u280_xdma_201920_3", 
        #                "number of banks":6,
        #                "number of nodes": 3,
        #                "F2F":True,
        #                "H2H":False}
        #                ]
        #for segment_size in [128, 256, 512, 1024]:
        #    experiments_filtering.append(
        #    { "experiment":"dma_hardened",
        #    "label": f"dma hardened segment_size {segment_size}",
        #    "collective name":"Allreduce",
        #    "Segment_size[KB]":segment_size,
        #    "board_instance":"xilinx_u280_xdma_201920_3", 
        #    "number of banks":6,
        #    "number of nodes": 3,
        #    "F2F":True,
        #    "H2H":False})
        #optimized_vs_base(other, experiments_filtering , logy=False, figure_label="dma_hardened_allreduce_comparison")
        #
        #experiments_filtering = [{
        #    "experiment":"19_dma",
        #    "label":"dual datapath (BASELINE)",
        #    "collective name":"Reduce",
        #    "Segment_size[KB]":1024,
        #    "board_instance":"xilinx_u280_xdma_201920_3", 
        #    "number of banks":6,
        #    "number of nodes": 3,
        #    "F2F":True,
        #    "H2H":False
        #}]
        #
        #for segment_size in [128, 256, 512, 1024]:
        #    experiments_filtering.append({
        #        "experiment":"dma_hardened",
        #        "label":f"dma hardened {segment_size}",
        #        "collective name":"Reduce",
        #        "Segment_size[KB]":segment_size,
        #        "board_instance":"xilinx_u280_xdma_201920_3", 
        #        "number of banks":6,
        #        "number of nodes": 3,
        #        "F2F":True,
        #        "H2H":False
        #    })
        #optimized_vs_base(other, experiments_filtering , logy=False, figure_label="dma_hardened_reduce_comparison")
        #
        #experiments_filtering = [{
        #    "experiment":"allgather",
        #    "label":"dual datapath",
        #    "collective name":"Allgather",
        #    "Segment_size[KB]":1024,
        #    "board_instance":"xilinx_u280_xdma_201920_3", 
        #    "number of banks":6,
        #    "number of nodes": 3,
        #    "F2F":True,
        #    "H2H":False
        #}]
        #
        #for segment_size in [128, 256, 512, 1024]:
        #    experiments_filtering.append({
        #        "experiment":"dma_hardened",
        #        "label":f"dma hardened {segment_size}",
        #        "collective name":"Allgather",
        #        "Segment_size[KB]":segment_size,
        #        "board_instance":"xilinx_u280_xdma_201920_3", 
        #        "number of banks":6,
        #        "number of nodes": 3,
        #        "F2F":True,
        #        "H2H":False
        #    })
        #
        #optimized_vs_base(other, experiments_filtering, logy=False, figure_label="dma_hardened_allgather_comparison")
        #
        #experiments_filtering = [{ 
        #    "experiment":"gather",
        #    "label":"dual datapath",
        #    "collective name":"Gather",
        #    "Segment_size[KB]":1024,
        #    "board_instance":"xilinx_u280_xdma_201920_3", 
        #    "number of banks":6,
        #    "number of nodes": 3,
        #    "F2F":True,
        #    "H2H":False
        #}]
        #
        #for segment_size in [128, 256, 512, 1024]:
        #    experiments_filtering.append({
        #        "experiment":"dma_hardened",
        #        "label": f"dma_hardened {segment_size}",
        #        "collective name":"Gather",
        #        "Segment_size[KB]": segment_size,
        #        "board_instance":"xilinx_u280_xdma_201920_3", 
        #        "number of banks":6,
        #        "number of nodes": 3,
        #        "F2F":True,
        #        "H2H":False
        #    })
        #optimized_vs_base(other,experiments_filtering, logy=False, figure_label="dma_hardened_gather_comparison")
        #
        #
        #experiments_filtering = [{ 
        #    "experiment":"bcast",
        #    "label":"dual datapath",
        #    "collective name":"Broadcast",
        #    "Segment_size[KB]":1024,
        #    "board_instance":"xilinx_u280_xdma_201920_3", 
        #    "number of banks":6,
        #    "number of nodes": 3,
        #    "F2F":True,
        #    "H2H":False
        #}]
        #
        #for segment_size in [128, 256, 512, 1024]:
        #    experiments_filtering.append({
        #        "experiment":"dma_hardened",
        #        "label": f"dma_hardened {segment_size}",
        #        "collective name":"Broadcast",
        #        "Segment_size[KB]":segment_size,
        #        "board_instance":"xilinx_u280_xdma_201920_3", 
        #        "number of banks":6,
        #        "number of nodes": 3,
        #        "F2F":True,
        #        "H2H":False
        #    })
        #optimized_vs_base(other,experiments_filtering, logy=False, figure_label="dma_hardened_broadcast_comparison")
        #

        #other = pd.concat([load_csvs_under("accl/dma_hardened2")])
        #other = remove_multiple_headers(other)
        #for collective_op in ["Send/recv","Broadcast", "Scatter", "Gather", "Allgather", "Reduce", "Allreduce"]:
        #    experiments_filtering = []
        #    for segment_size, marker in [(128,"o"), (256, "*"), (512, "v"), (1024, ">")]:
        #        experiments_filtering.append(
        #            { 
        #                "experiment":"dma_mcu",
        #                "line style":f"C0-{marker}",
        #                "label":f"MCU s:{segment_size}",
        #                "collective name": collective_op,
        #                "Segment_size[KB]":segment_size,
        #                "board_instance":"xilinx_u280_xdma_201920_3", 
        #                "number of banks":6,
        #                "number of nodes": 3,
        #                "F2F":True,
        #                "H2H":False})
        #    
        #    collective_label = collective_op.replace("/","")
        #    print(collective_op)
        #    optimized_vs_base(other, experiments_filtering , logy=True, figure_label=f"dma_mcu_{collective_label}_trend")
        #for collective_op in ["Send/recv","Broadcast", "Scatter", "Gather", "Allgather", "Reduce", "Allreduce"]:
        #    experiments_filtering = []
        #
        #    for segment_size, marker in [(128,"o"), (256, "*"), (512, "v"), (1024, ">")]:
        #        experiments_filtering.append(
        #            { 
        #                "experiment":"dma_hardened2",
        #                "line style":f"C1-{marker}",
        #                "label": f"dma hardened s:{segment_size}",
        #                "collective name": collective_op,
        #                "Segment_size[KB]":segment_size,
        #                "board_instance":"xilinx_u280_xdma_201920_3", 
        #                "number of banks":6,
        #                "number of nodes": 3,
        #                "F2F":True,
        #                "H2H":False})
        #    collective_label = collective_op.replace("/","")
        #    print(collective_op)
        #    optimized_vs_base(other, experiments_filtering , logy=True, figure_label=f"dma_hardened_{collective_label}_trend")
        #for collective_op in ["Send/recv","Broadcast", "Scatter", "Gather", "Allgather", "Reduce", "Allreduce"]:
        #    experiments_filtering = []
        #    for segment_size, marker in [(128,"o"), (256, "*"), (512, "v"), (1024, ">")]:
        #        experiments_filtering.append(
        #            { 
        #                "experiment":"dma_mcu",
        #                "line style":f"C0-{marker}",
        #                "label":f"MCU s:{segment_size}",
        #                "collective name": collective_op,
        #                "Segment_size[KB]":segment_size,
        #                "board_instance":"xilinx_u280_xdma_201920_3", 
        #                "number of banks":6,
        #                "number of nodes": 3,
        #                "F2F":True,
        #                "H2H":False})
        #    for segment_size, marker in [(128,"o"), (256, "*"), (512, "v"), (1024, ">")]:
        #        experiments_filtering.append(
        #            { 
        #                "experiment":"dma_hardened2",
        #                "line style":f"C1-{marker}",
        #                "label": f"dma hardened s:{segment_size}",
        #                "collective name": collective_op,
        #                "Segment_size[KB]":segment_size,
        #                "board_instance":"xilinx_u280_xdma_201920_3", 
        #                "number of banks":6,
        #                "number of nodes": 3,
        #                "F2F":True,
        #                "H2H":False})
        #    collective_label = collective_op.replace("/","")
        #    print(collective_op)
        #    optimized_vs_base(other, experiments_filtering , logy=True, figure_label=f"dma_hardened_{collective_label}_comparison")

        other = pd.concat([load_csvs_under("accl/dma_mover")])
        other = remove_multiple_headers(other)
        for collective_op in ["Send/recv", "Accumulate"]:
            experiments_filtering = []
            for segment_size, marker in [(1024, ">"), (1, "x")]:
                experiments_filtering.append(
                    { 
                        "experiment":"dma_mover",
                        "line style":f"C0-{marker}",
                        "label":f"dma mover:{segment_size}",
                        "collective name": collective_op,
                        "Segment_size[KB]":segment_size,
                        "board_instance":"xilinx_u280_xdma_201920_3", 
                        "number of banks":6,
                        "number of nodes": 2,
                        "F2F":True,
                        "H2H":False})
                
                experiments_filtering.append(
                    { 
                        "experiment":"old_tcp",
                        "line style":f"C1-{marker}",
                        "label":f"MCU s:{segment_size}",
                        "collective name": collective_op,
                        "Segment_size[KB]":segment_size,
                        "board_instance":"xilinx_u280_xdma_201920_3", 
                        "number of banks":6,
                        "number of nodes": 2,
                        "F2F":True,
                        "H2H":False})
                
            
            collective_label = collective_op.replace("/","")
            print(collective_op)
            optimized_vs_base(other, experiments_filtering , logy=True, figure_label=f"dmamover_vs_old_tcp_{collective_label}_trend")