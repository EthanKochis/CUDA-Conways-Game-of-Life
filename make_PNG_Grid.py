import matplotlib.pyplot as plt
from matplotlib import colors
import subprocess
import sys

def create_colormap(file_data, numRows, numCols, gen_number):
    
    data = []
    for i in range(0, numRows):
        temp = []
        for j in range(0, numCols):
            if file_data[i][j] == '0':
                temp.append(0)
            else:
                temp.append(1)
        data.append(temp)

    # create colormap
    cmap = colors.ListedColormap(['#777777', '#28F1B3'])
    bounds = [0,1,2]
    norm = colors.BoundaryNorm(bounds, cmap.N)

    fig, ax = plt.subplots(1, 1, figsize=(numRows/4, numCols/4), dpi=1200/(numRows/4))
    ax.imshow(data, cmap=cmap, norm=norm)

    # draw gridlines
    ax.grid(which='major', axis='both', linestyle='-', color='k', linewidth=2)
    # ax.set_xticks(np.arange(-.5, 10, 1))
    # ax.set_yticks(np.arange(-.5, 10, 1))
    for x in range(numRows + 1):
        ax.axhline(x-0.5, lw=1.1, color='k', zorder=5)
    for x in range(numCols + 1):
        ax.axvline(x-0.5, lw=1.1, color='k', zorder=5)

    ax.set_xticklabels([])
    ax.set_yticklabels([])
    ax.set_xticks([])
    ax.set_yticks([])

    plt.savefig("results/gen" + str(gen_number) + ".png", transparent=True)

if __name__ == "__main__":
    
    generation_number = int(sys.argv[1])

    file_data = []
    with open("results/gen" + str(generation_number) + ".txt") as f:
        for i, line in enumerate(f):
            if i == 0:
                num_rows, num_cols = line.strip("\n").split(" ")
            else:
                row = line.strip("\n").split(" ")
                file_data.append(row)
    create_colormap(file_data, int(num_rows), int(num_cols), generation_number)
