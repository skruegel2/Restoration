
#include <math.h>
#include "tiff.h"
#include "allocate.h"
#include "randlib.h"
#include "typeutil.h"

#define FILTER_LENGTH 5
#define HALF_FILT 2

void error(char *name);

void ProcessSingleColor(int color, struct TIFF_img input_img, struct TIFF_img color_img, double** filt, double lambda);

// Multiply one pixel element by filter
double MultiplyOnePixelEl(int source_height, int source_width,
                         int src_cur_row, int src_cur_col,
                         double** source_img, double** filt, double lambda);

// Multiply pixel by one filter element, considering boundaries.
double MultiplyOneFilterEl(int source_height, int source_width,
                         int src_cur_row, int src_cur_col, int filt_cur_row,
                         int filt_cur_col, double** source_img, double** filt, double lambda);

// Init filter
void InitFilter(double** filt)
{
  for(int cur_row = 0; cur_row < FILTER_LENGTH; cur_row++)
  {
    for(int cur_col = 0; cur_col < FILTER_LENGTH; cur_col++)
    {
      filt[cur_row][cur_col] = 1.0/25.0;
    }
  }
}

int main (int argc, char **argv)
{
  FILE *fp_input;
  FILE *fp_color;
  struct TIFF_img input_img;
  struct TIFF_img color_img;
  double** filt;
  filt = (double **)get_img(FILTER_LENGTH,
                            FILTER_LENGTH,
                            sizeof(double));
  InitFilter(filt);

  if ( argc != 3 ) error( argv[0] );

  /* open image file */
  if ( ( fp_input = fopen ( argv[1], "rb" ) ) == NULL ) {
    fprintf ( stderr, "cannot open file %s\n", argv[1] );
    exit ( 1 );
  }

  /* read image */
  if ( read_TIFF ( fp_input, &input_img ) ) {
    fprintf ( stderr, "error reading file %s\n", argv[1] );
    exit ( 1 );
  }

  /* close image file */
  fclose ( fp_input );
  get_TIFF ( &color_img, input_img.height,
            input_img.width, 'c' );

  float lambda;

  sscanf(argv[2], "%3f", &lambda);
  // Process red, green, blue
  for(int color = 0; color < 3; color++)
  {
    ProcessSingleColor(color, input_img, color_img, filt, lambda);
  }

  if ( ( fp_color = fopen ( "Prob4Filt.tif", "wb" ) ) == NULL ) {
    fprintf ( stderr, "cannot open file Prob4Filt.tif\n");
    exit ( 1 );
  }

  // Write color image
  if ( write_TIFF ( fp_color, &color_img ) ) {
    fprintf ( stderr, "error writing TIFF file\n");
    exit ( 1 );
  }

  // Close color image file
  fclose ( fp_color );

  free_TIFF ( &(input_img) );
  free_TIFF ( &(color_img) );

  return(0);
}

// Multiply pixel by one filter element, considering boundaries.
double MultiplyOneFilterEl(int source_height, int source_width,
                         int src_cur_row, int src_cur_col, int filt_cur_row,
                         int filt_cur_col, double** source_img, double** filt, double lambda)
{
  double ret_val;
  // Check for out of bounds to the left
  if (src_cur_col + filt_cur_col < 0)
  {
    ret_val = 0;
  }
  // Check for out of bounds to the right
  else if (src_cur_col + filt_cur_col > (source_width - 1))
  {
    ret_val = 0;
  }
  // Check for out of bounds on top
  else if (src_cur_row + filt_cur_row < 0)
  {
    ret_val = 0;
  }
  // Check for out of bounds at bottom
  else if (src_cur_row + filt_cur_row > (source_height - 1))
  {
    ret_val = 0;
  }
  else
  {
    ret_val = source_img[src_cur_row + filt_cur_row][src_cur_col + filt_cur_col]*filt[filt_cur_row+HALF_FILT][filt_cur_col+HALF_FILT];
  }
  return ret_val;
}

// Multiply one pixel element by filter
double MultiplyOnePixelEl(int source_height, int source_width,
                         int src_cur_row, int src_cur_col,
                         double** source_img, double** filt, double lambda)
{
  double ret_val = 0;   // Init before accumulating
  for(int filt_row = -HALF_FILT; filt_row <= HALF_FILT; filt_row++)
  {
    for (int filt_col = -HALF_FILT; filt_col <= HALF_FILT; filt_col++)
    {
      ret_val += MultiplyOneFilterEl(source_height,source_width,src_cur_row, src_cur_col,
                                    filt_row, filt_col, source_img, filt, lambda);
    }
  }
  ret_val *= -1;
  ret_val += source_img[src_cur_row][src_cur_col];
  ret_val *= lambda;
  ret_val += source_img[src_cur_row][src_cur_col];

  return ret_val;
}

void error(char *name)
{
    printf("usage:  %s  image.tiff \n\n",name);
    printf("this program reads in a 24-bit color TIFF image.\n");
    printf("It then horizontally filters the green component, adds noise,\n");
    printf("and writes out the result as an 8-bit image\n");
    printf("with the name 'green.tiff'.\n");
    printf("It also generates an 8-bit color image,\n");
    printf("that swaps red and green components from the input image");
    exit(1);
}


void ProcessSingleColor(int color, struct TIFF_img input_img, struct TIFF_img color_img, double** filt, double lambda)
{
  double **img_orig;
  double **img_filt;
  int cur_row, cur_col;
  int32_t pixel;

  img_orig = (double **)get_img(input_img.width,
                                     input_img.height,
                                     sizeof(double));
  img_filt = (double **)get_img(input_img.width,
                                     input_img.height,
                                     sizeof(double));
  // Copy all components to respective double array
  for ( cur_row = 0; cur_row < input_img.height; cur_row++ )
  {
    for ( cur_col = 0; cur_col < input_img.width; cur_col++ )
    {
      img_orig[cur_row][cur_col] = input_img.color[color][cur_row][cur_col];
    }
  }

  // Step through rows and columns applying filter
  for ( cur_row = 0; cur_row < input_img.height; cur_row++ )
  {
    for ( cur_col = 0; cur_col < input_img.width; cur_col++ )
    {
      img_filt[cur_row][cur_col] = MultiplyOnePixelEl(input_img.height,input_img.width,
                                                      cur_row,cur_col,
                                                      img_orig, filt, lambda);
    }
  }

  for ( cur_row = 0; cur_row < input_img.height; cur_row++ )
  {
      for ( cur_col = 0; cur_col < input_img.width; cur_col++ )
      {
          pixel = (int32_t) img_filt[cur_row][cur_col];
          if (pixel > 255)
          {
              color_img.color[color][cur_row][cur_col] = 255;
          }
          else if (pixel < 0)
          {
              color_img.color[color][cur_row][cur_col] = 0;
          }
          else
          {
              color_img.color[color][cur_row][cur_col] = pixel;
          }
      }
  }

  free_img( (void**)img_orig );
  free_img( (void**)img_filt );
}
