#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <time.h>

double X_MIN;
double X_MAX;
double Y_MIN;
double Y_MAX;

double pixel_width;
double pixel_height;

const int MAX_ITER = 200;

int image_size;
unsigned char **image_buffer;

int IMAGE_WIDTH;
int IMAGE_HEIGHT;
int image_buffer_size;

int gradient_size = 16;
int colors[17][3] = {
                        {66, 30, 15},
                        {25, 7, 26},
                        {9, 1, 47},
                        {4, 4, 73},
                        {0, 7, 100},
                        {12, 44, 138},
                        {24, 82, 177},
                        {57, 125, 209},
                        {134, 181, 229},
                        {211, 236, 248},
                        {241, 233, 191},
                        {248, 201, 95},
                        {255, 170, 0},
                        {204, 128, 0},
                        {153, 87, 0},
                        {106, 52, 3},
                        {16, 16, 16},
                    };

int NUM_THREADS;

void allocate_image_buffer(){
    int rgb_size = 3;
    image_buffer = (unsigned char **) malloc(sizeof(unsigned char *) * image_buffer_size);

    for(int i = 0; i < image_buffer_size; i++){
        image_buffer[i] = (unsigned char *) malloc(sizeof(unsigned char) * rgb_size);
    };
};

void free_image_buffer(){
    for(int i = 0; i < image_buffer_size; i++)
        free(image_buffer[i]);
    free(image_buffer);
}

void init(int argc, char *argv[]){
    if(argc != 7){
        printf("usage: ./mandelbrot_pth c_x_min c_x_max c_y_min c_y_max image_size num_threads\n");
        printf("examples with image_size = 11500:\n");
        printf("    Full Picture:         ./mandelbrot_pth -2.5 1.5 -2.0 2.0 11500 4\n");
        printf("    Seahorse Valley:      ./mandelbrot_pth -0.8 -0.7 0.05 0.15 11500 16\n");
        printf("    Elephant Valley:      ./mandelbrot_pth 0.175 0.375 -0.1 0.1 11500 32\n");
        printf("    Triple Spiral Valley: ./mandelbrot_pth -0.188 -0.012 0.554 0.754 11500 64\n");
        exit(0);
    }
    else{
        sscanf(argv[1], "%lf", &X_MIN);
        sscanf(argv[2], "%lf", &X_MAX);
        sscanf(argv[3], "%lf", &Y_MIN);
        sscanf(argv[4], "%lf", &Y_MAX);
        sscanf(argv[5], "%d", &image_size);
        sscanf(argv[6], "%d", &NUM_THREADS);

        IMAGE_WIDTH           = image_size;
        IMAGE_HEIGHT           = image_size;
        image_buffer_size = image_size * image_size;

        pixel_width       = (X_MAX - X_MIN) / IMAGE_WIDTH;
        pixel_height      = (Y_MAX - Y_MIN) / IMAGE_HEIGHT;
    };
};

void update_rgb_buffer(int iteration, int x, int y){
    int color;

    if(iteration == MAX_ITER){
        image_buffer[(IMAGE_HEIGHT * y) + x][0] = colors[gradient_size][0];
        image_buffer[(IMAGE_HEIGHT * y) + x][1] = colors[gradient_size][1];
        image_buffer[(IMAGE_HEIGHT * y) + x][2] = colors[gradient_size][2];
    }
    else{
        color = iteration % gradient_size;

        image_buffer[(IMAGE_HEIGHT * y) + x][0] = colors[color][0];
        image_buffer[(IMAGE_HEIGHT * y) + x][1] = colors[color][1];
        image_buffer[(IMAGE_HEIGHT * y) + x][2] = colors[color][2];
    };
};

void write_to_file(){
    FILE * file;
    char * filename               = "output.ppm";
    char * comment                = "# ";

    int max_color_component_value = 255;

    file = fopen(filename,"wb");

    fprintf(file, "P6\n %s\n %d\n %d\n %d\n", comment,
            IMAGE_WIDTH, IMAGE_HEIGHT, max_color_component_value);

    for(int i = 0; i < image_buffer_size; i++){
        fwrite(image_buffer[i], 1 , 3, file);
    };

    fclose(file);
};

void compute_mandelbrot(){
    const double escape_radius_squared = 4;

    #pragma omp parallel for schedule(dynamic) num_threads(NUM_THREADS)
    for(int y_i = 0; y_i < IMAGE_HEIGHT; y_i++){
        for(int x_i = 0; x_i < IMAGE_WIDTH; x_i++){
            double c_y = Y_MIN + y_i * pixel_height;
            double c_x = X_MIN + x_i * pixel_width;

            if(fabs(c_y) < pixel_height / 2) c_y = 0.0;

            double z_x = 0.0;
            double z_y = 0.0;

            double z_x_squared = 0.0;
            double z_y_squared = 0.0;

            int iteration;

            for(iteration = 0;
                iteration < MAX_ITER && \
                ((z_x_squared + z_y_squared) < escape_radius_squared);
                iteration++){
                z_y         = 2 * z_x * z_y + c_y;
                z_x         = z_x_squared - z_y_squared + c_x;

                z_x_squared = z_x * z_x;
                z_y_squared = z_y * z_y;
            };

            // update_rgb_buffer(iteration, x_i, y_i);
        };
    };
};

int main(int argc, char *argv[]){
    init(argc, argv);

    // allocate_image_buffer();

    compute_mandelbrot();

    // write_to_file();

    // free_image_buffer();

    return 0;
};
