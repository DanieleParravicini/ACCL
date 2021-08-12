def cluster_bar(figure_path,labels, y_datas, y_labels):
    labels = size

    x = np.arange(len(labels))  # the label locations
    width = 0.30  # the width of the bars


    fig, ax = plt.subplots(figsize=(10,4))
    # fig = plt.figure(figsize=(8,4))
    for i, y, y_label in enumerate(zip(y_datas, y_labels)):
        ax.bar(x + (i - len(y_datas)/2)*width, y, width, label=y_label)

    plt.grid(axis='y')
    # Add some text for labels, title and custom x-axis tick labels, etc.
    ax.set_ylabel('Duration [us]')
    ax.set_title('Broadcast')
    ax.set_xticks(x)
    ax.set_xticklabels(labels)
    ax.set_xlabel('Message Size [KB]')
    ax.legend()

    plt.savefig(figurepath)
    