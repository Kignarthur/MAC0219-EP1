import re
import csv
import pandas as pd
from matplotlib import pyplot as plt

class Mandelbroter():
    """ Creates csv file and plots the data """
    def __init__(self, header, exe_type, file_path, file_name):
        self.header = header
        self.exe_type = exe_type
        self.file_path = file_path
        self.file_name = file_name

    def process_data(self):
        self.create_csv_file()
        self.plot_csv()

    def create_csv_file(self):
        log_file = self.file_path + '.log'
        csv_file = self.file_path + '.csv'

        # Reads lines
        with open(log_file, "r") as file:
            lines = file.readlines()

        # Write to csv
        with open(csv_file, "w") as csvf:
            writer = csv.writer(csvf)
            writer.writerow(self.header)

            row = []
            run_offset = 1
            time_offset = 12
            space_between_measurements = 14

            for line_number, line in enumerate(lines):
                # Get size and last parameter (allocmem or threads)
                if (line_number - run_offset) % space_between_measurements == 0:
                    parameters = re.findall("'(.*?)'", line, re.DOTALL)[0].split()
                    row += parameters[-2:]

                # Get execution time and confidence interval
                elif (line_number - time_offset) % space_between_measurements == 0:
                    times = re.findall("\d+\.\d+", line)
                    row += times[:2]

                    writer.writerow(row); row = []

    def plot_csv(self):
        df = pd.read_csv(self.file_path + '.csv')
        x_coord, y_coord = self.header[1], self.header[2]

        # Create a plot comparing different image sizes
        image_sizes = df.groupby('Image Size')

        for img_size, _ in image_sizes:
            group = image_sizes.get_group(img_size).convert_dtypes()
            x = group[x_coord]
            y = group[y_coord]
            label = f'{img_size} px'
            plt.plot(x, y, label=label, marker='o')

        # Put labels
        plt.xlabel(x_coord)
        plt.ylabel(y_coord)
        plt.title(f'{self.exe_type} {self.file_name}')
        plt.legend(loc='upper right', bbox_to_anchor=(1.25, 1))

        # Save plot and clear
        plot_path = self.file_path + '.png'
        plt.savefig(plot_path, bbox_inches='tight')
        plt.clf()


def main():
    # All possible files
    results_dir = 'results/'
    dir_names = ['mandelbrot_seq/', 'mandelbrot_pth/', 'mandelbrot_omp/']
    file_names = ['full', 'elephant', 'seahorse', 'triple_spiral']

    # Naming
    header = ['Image Size', None, 'Execution Time (s)', 'Confidence Interval']
    exe_types = ['Sequential', 'Pthreads', 'OpenMP']
    formated_fnames = ['Full', 'Elephant', 'Seahorse', 'Triple Spiral']

    # Permutate and process
    for i, dir_name in enumerate(dir_names):
        for file_name in file_names:
            header[1] = 'Threads' if dir_name != 'mandelbrot_seq/' else 'Alloc Memory'
            exe_type = exe_types[i]
            formated_fname = formated_fnames[i]
            file_path = results_dir + dir_name + file_name

            mandel = Mandelbroter(header, exe_type, file_path, formated_fname)
            mandel.process_data()

if __name__ == '__main__':
    main()
