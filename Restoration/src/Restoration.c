
#include <math.h>
#include "tiff.h"
#include "allocate.h"
#include "randlib.h"
#include "typeutil.h"

#define FILTER_LENGTH 5
#define HALF_FILT 2
#define FILTER_STRUCT_LEN 25

// Filter element comparison
int filter_el_compare(const void* elem1, const void* elem2);

// Sort filter array in descending product order
void product_sort(struct filter_struct* filt, int len);

struct filter_struct
{
  double pixel;
  double weight;
  double product;
};

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
void init_filter(struct filter_struct* filt)
{
  int idx;
  // Row 0
  for (idx = 0; idx < 5; idx++)
  {
    filt[idx].weight = 1;
  }
  // Row 1
  filt[5].weight = 1;
  filt[6].weight = 2;
  filt[7].weight = 2;
  filt[8].weight = 2;
  filt[9].weight = 1;
  // Row 2
  filt[10].weight = 1;
  filt[11].weight = 2;
  filt[12].weight = 2;
  filt[13].weight = 2;
  filt[14].weight = 1;
  // Row 3
  filt[15].weight = 1;
  filt[16].weight = 2;
  filt[17].weight = 2;
  filt[18].weight = 2;
  filt[19].weight = 1;
  // Row 4
  for (idx = 20; idx < FILTER_STRUCT_LEN; idx++)
  {
    filt[idx].weight = 1;
  }
  for (idx = 0; idx < FILTER_STRUCT_LEN; idx++)
  {
    filt[idx].product = 0;
    filt[idx].pixel = 0;
  }
}

void test_sort(struct filter_struct* filt)
{
  struct filter_struct dest[FILTER_STRUCT_LEN];
  struct filter_struct test[FILTER_STRUCT_LEN];
  int idx;

  // Fill with random pixels
  for (idx = 0; idx < FILTER_STRUCT_LEN; idx++)
  {
    double pixel_val = rand() % 100;
    filt[idx].pixel = rand() % 100;
    filt[idx].product = filt[idx].weight * filt[idx].pixel;
  }
  // Copy into debug variable test
  for (idx = 0; idx < FILTER_STRUCT_LEN; idx++)
  {
    test[idx].pixel = filt[idx].pixel;
    test[idx].weight = filt[idx].weight;
    test[idx].product = filt[idx].product;
  }
  // print pixel before sort
  //for (idx = 0; idx < FILTER_STRUCT_LEN; idx++)
  //{
  //  printf("Index %d\tPixel %f\n", idx, filt[idx].pixel);
  //}

  // Sort product
  product_sort(filt, FILTER_STRUCT_LEN);

  // print pixel after sort
  //for (idx = 0; idx < FILTER_STRUCT_LEN; idx++)
  //{
  //  printf("Index %d\tPixel %f\n", idx, filt[idx].pixel);
  //}

}

void swap(struct filter_struct* src, int el_1_idx, int el_2_idx)
{
  struct filter_struct temp;
  // Copy element 1 into temp
  temp.pixel = src[el_1_idx].pixel;
  temp.weight = src[el_1_idx].weight;
  temp.product = src[el_1_idx].product;
  // Copy element 2 into location 1
  src[el_1_idx].pixel = src[el_2_idx].pixel;
  src[el_1_idx].weight = src[el_2_idx].weight;
  src[el_1_idx].product = src[el_2_idx].product;
  // Copy temp into element 2
  src[el_2_idx].pixel = temp.pixel;
  src[el_2_idx].weight = temp.weight;
  src[el_2_idx].product = temp.product;
}

// Sort filter array in descending product order
void product_sort(struct filter_struct* src, int len)
{
  int idx;
  int sorted_idx;
  // Outer loop steps through entire array once
  for (sorted_idx = 0; sorted_idx < len; sorted_idx++)
  {
    // Inner loop steps through unsorted elements
    for (idx = sorted_idx + 1; idx < len; idx++)
    {
      if (src[idx].product > src[sorted_idx].product)
      {
        swap(src, sorted_idx, idx);
      }
    }
  }
}

int find_median_idx(struct filter_struct* src, int len)
{
  int src_idx = 0;    // Index to step through source 
  int sum_idx = 0;
  int median_idx = 0;
  float large_weight_sum = 0;
  float small_weight_sum = 0;
  for (src_idx = 0; src_idx < len; src_idx++)
  {
    large_weight_sum = 0;     // Reset sums before each summation
    small_weight_sum = 0;
    for (sum_idx = 0; sum_idx <= src_idx; sum_idx++)
    {
      large_weight_sum += src[sum_idx].weight;
    }
    for (sum_idx = src_idx+1; sum_idx < len; sum_idx++)
    {
      small_weight_sum += src[sum_idx].weight;
    }
    printf("Large: %f small: %f\n", large_weight_sum, small_weight_sum);
    // Compare to see if median has been reached
    if (large_weight_sum >= small_weight_sum)
    {
      median_idx = src_idx;

      break;
    }
  }
  return median_idx;
}

int main (int argc, char **argv)
{
  FILE *fp_input;
  FILE *fp_filter;
  struct TIFF_img input_img;
  struct TIFF_img filter_img;
  struct filter_struct med_filt[25];
  init_filter(med_filt);
  int median_idx;
  median_idx = find_median_idx(med_filt, 25);
  test_sort(med_filt);

  /* open image file */
  if ((fp_input = fopen("img14g.tif", "rb")) == NULL) {
    fprintf ( stderr, "cannot open file %s\n", "img14g.tif" );
    exit ( 1 );
  }
//
//  /* read image */
//  if ( read_TIFF ( fp_input, &input_img ) ) {
//    fprintf ( stderr, "error reading file %s\n", argv[1] );
//    exit ( 1 );
//  }
//
//  /* close image file */
//  fclose ( fp_input );
//  get_TIFF ( &color_img, input_img.height,
//            input_img.width, 'c' );
//
//  float lambda;
//
//  sscanf(argv[2], "%3f", &lambda);
//  // Process red, green, blue
//  for(int color = 0; color < 3; color++)
//  {
//    ProcessSingleColor(color, input_img, color_img, filt, lambda);
//  }
//
//  if ( ( fp_color = fopen ( "Prob4Filt.tif", "wb" ) ) == NULL ) {
//    fprintf ( stderr, "cannot open file Prob4Filt.tif\n");
//    exit ( 1 );
//  }
//
//  // Write color image
//  if ( write_TIFF ( fp_color, &color_img ) ) {
//    fprintf ( stderr, "error writing TIFF file\n");
//    exit ( 1 );
//  }
//
//  // Close color image file
//  fclose ( fp_color );
//
//  free_TIFF ( &(input_img) );
//  free_TIFF ( &(color_img) );
//
  return(0);
}
//
//// Multiply pixel by one filter element, considering boundaries.
//double MultiplyOneFilterEl(int source_height, int source_width,
//                         int src_cur_row, int src_cur_col, int filt_cur_row,
//                         int filt_cur_col, double** source_img, double** filt, double lambda)
//{
//  double ret_val;
//  // Check for out of bounds to the left
//  if (src_cur_col + filt_cur_col < 0)
//  {
//    ret_val = 0;
//  }
//  // Check for out of bounds to the right
//  else if (src_cur_col + filt_cur_col > (source_width - 1))
//  {
//    ret_val = 0;
//  }
//  // Check for out of bounds on top
//  else if (src_cur_row + filt_cur_row < 0)
//  {
//    ret_val = 0;
//  }
//  // Check for out of bounds at bottom
//  else if (src_cur_row + filt_cur_row > (source_height - 1))
//  {
//    ret_val = 0;
//  }
//  else
//  {
//    ret_val = source_img[src_cur_row + filt_cur_row][src_cur_col + filt_cur_col]*filt[filt_cur_row+HALF_FILT][filt_cur_col+HALF_FILT];
//  }
//  return ret_val;
//}
//
//// Multiply one pixel element by filter
//double MultiplyOnePixelEl(int source_height, int source_width,
//                         int src_cur_row, int src_cur_col,
//                         double** source_img, double** filt, double lambda)
//{
//  double ret_val = 0;   // Init before accumulating
//  for(int filt_row = -HALF_FILT; filt_row <= HALF_FILT; filt_row++)
//  {
//    for (int filt_col = -HALF_FILT; filt_col <= HALF_FILT; filt_col++)
//    {
//      ret_val += MultiplyOneFilterEl(source_height,source_width,src_cur_row, src_cur_col,
//                                    filt_row, filt_col, source_img, filt, lambda);
//    }
//  }
//  ret_val *= -1;
//  ret_val += source_img[src_cur_row][src_cur_col];
//  ret_val *= lambda;
//  ret_val += source_img[src_cur_row][src_cur_col];
//
//  return ret_val;
//}
//
//void error(char *name)
//{
//    printf("usage:  %s  image.tiff \n\n",name);
//    printf("this program reads in a 24-bit color TIFF image.\n");
//    printf("It then horizontally filters the green component, adds noise,\n");
//    printf("and writes out the result as an 8-bit image\n");
//    printf("with the name 'green.tiff'.\n");
//    printf("It also generates an 8-bit color image,\n");
//    printf("that swaps red and green components from the input image");
//    exit(1);
//}
//
//
//void ProcessSingleColor(int color, struct TIFF_img input_img, struct TIFF_img color_img, double** filt, double lambda)
//{
//  double **img_orig;
//  double **img_filt;
//  int cur_row, cur_col;
//  int32_t pixel;
//
//  img_orig = (double **)get_img(input_img.width,
//                                     input_img.height,
//                                     sizeof(double));
//  img_filt = (double **)get_img(input_img.width,
//                                     input_img.height,
//                                     sizeof(double));
//  // Copy all components to respective double array
//  for ( cur_row = 0; cur_row < input_img.height; cur_row++ )
//  {
//    for ( cur_col = 0; cur_col < input_img.width; cur_col++ )
//    {
//      img_orig[cur_row][cur_col] = input_img.color[color][cur_row][cur_col];
//    }
//  }
//
//  // Step through rows and columns applying filter
//  for ( cur_row = 0; cur_row < input_img.height; cur_row++ )
//  {
//    for ( cur_col = 0; cur_col < input_img.width; cur_col++ )
//    {
//      img_filt[cur_row][cur_col] = MultiplyOnePixelEl(input_img.height,input_img.width,
//                                                      cur_row,cur_col,
//                                                      img_orig, filt, lambda);
//    }
//  }
//
//  for ( cur_row = 0; cur_row < input_img.height; cur_row++ )
//  {
//      for ( cur_col = 0; cur_col < input_img.width; cur_col++ )
//      {
//          pixel = (int32_t) img_filt[cur_row][cur_col];
//          if (pixel > 255)
//          {
//              color_img.color[color][cur_row][cur_col] = 255;
//          }
//          else if (pixel < 0)
//          {
//              color_img.color[color][cur_row][cur_col] = 0;
//          }
//          else
//          {
//              color_img.color[color][cur_row][cur_col] = pixel;
//          }
//      }
//  }
//
//  free_img( (void**)img_orig );
//  free_img( (void**)img_filt );
//}
