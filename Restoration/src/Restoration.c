
#include <math.h>
#include "tiff.h"
#include "allocate.h"
#include "randlib.h"
#include "typeutil.h"

#define FILTER_LENGTH 5
#define HALF_FILT 2
#define FILTER_STRUCT_LEN 25

// Sort filter array in descending pixel order
void pixel_sort(struct filter_struct* filt, int len);

struct filter_struct
{
  double pixel;
  double weight;
  double product;
};

void error(char *name);

void filter_image(struct TIFF_img input_img, struct TIFF_img color_img, struct filter_struct* med_filt);

// Multiply one pixel element by filter
double multiply_one_pixel(int source_height, int source_width,
                         int src_cur_row, int src_cur_col,
                         double** source_img, struct filter_struct* med_filt);

// Multiply pixel by one filter element, considering boundaries.
double calculate_one_filter_element(int source_height, int source_width,
                         int src_cur_row, int src_cur_col, int filt_cur_row,
                         int filt_cur_col, double** source_img);

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
  pixel_sort(filt, FILTER_STRUCT_LEN);

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
void pixel_sort(struct filter_struct* src, int len)
{
  int idx;
  int sorted_idx;
  // Outer loop steps through entire array once
  for (sorted_idx = 0; sorted_idx < len; sorted_idx++)
  {
    // Inner loop steps through unsorted elements
    for (idx = sorted_idx + 1; idx < len; idx++)
    {
      if (src[idx].pixel > src[sorted_idx].pixel)
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
    //printf("Large: %f small: %f\n", large_weight_sum, small_weight_sum);
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
  //int median_idx;
  //median_idx = find_median_idx(med_filt, 25);
  //test_sort(med_filt);

  /* open image file */
  if ((fp_input = fopen("img14gn.tif", "rb")) == NULL) {
    fprintf ( stderr, "cannot open file %s\n", "img14g.tif" );
    exit ( 1 );
  }

  /* read image */
  if ( read_TIFF ( fp_input, &input_img ) ) {
    fprintf ( stderr, "error reading file %s\n", argv[1] );
    exit ( 1 );
  }

  /* close image file */
  fclose ( fp_input );
  get_TIFF ( &filter_img, input_img.height,
            input_img.width, 'g' );

  filter_image(input_img, filter_img, med_filt);

  if ( ( fp_filter = fopen ( "Filtered img14gn.tif", "wb" ) ) == NULL ) {
    fprintf ( stderr, "cannot open file HW7Prob2.tif\n");
    exit ( 1 );
  }

  // Write filtered image
  if ( write_TIFF (fp_filter, &filter_img) ) {
    fprintf ( stderr, "error writing TIFF file\n");
    exit ( 1 );
  }

  // Close filtered image file
  fclose ( fp_filter );

  free_TIFF ( &(input_img) );
  free_TIFF ( &(filter_img) );

  return(0);
}

// Add an element to filter, considering boundaries.
double calculate_one_filter_element(int source_height, int source_width,
                         int src_cur_row, int src_cur_col, int filt_cur_row,
                         int filt_cur_col, double** source_img)
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
    ret_val = source_img[src_cur_row + filt_cur_row][src_cur_col + filt_cur_col];
  }
  return ret_val;
}

// Fill filter, sort filter, return median
double calculate_one_pixel(int source_height, int source_width,
                         int src_cur_row, int src_cur_col,
                         double** source_img, struct filter_struct* med_filt)
{
  double ret_val;

  // Fill filter
  for(int filt_row = -HALF_FILT; filt_row <= HALF_FILT; filt_row++)
  {
    for (int filt_col = -HALF_FILT; filt_col <= HALF_FILT; filt_col++)
    {
      med_filt[(filt_row + HALF_FILT)*5 + filt_col+HALF_FILT].pixel =
        calculate_one_filter_element(source_height,source_width,src_cur_row, src_cur_col,
                                    filt_row, filt_col, source_img);
    }
  }
  pixel_sort(med_filt, FILTER_LENGTH * FILTER_LENGTH);
  ret_val = med_filt[find_median_idx(med_filt, FILTER_LENGTH * FILTER_LENGTH)].pixel;
  return ret_val;
}

void filter_image(struct TIFF_img input_img, struct TIFF_img filter_img, struct filter_struct* med_filt)
{
  double **source_img;
  double **filt_img;
  int cur_row, cur_col;
  int32_t pixel;

  source_img = (double **)get_img(input_img.width,
                                     input_img.height,
                                     sizeof(double));
  filt_img = (double **)get_img(input_img.width,
                                     input_img.height,
                                     sizeof(double));
  // Copy all components to respective double array
  for ( cur_row = 0; cur_row < input_img.height; cur_row++ )
  {
    for ( cur_col = 0; cur_col < input_img.width; cur_col++ )
    {
        source_img[cur_row][cur_col] = input_img.mono[cur_row][cur_col];
    }
  }

  // Step through rows and columns applying filter
  for ( cur_row = 0; cur_row < input_img.height; cur_row++ )
  {
    for ( cur_col = 0; cur_col < input_img.width; cur_col++ )
    {
      filt_img[cur_row][cur_col] = calculate_one_pixel(input_img.height,input_img.width,
                                                      cur_row,cur_col,
                                                      source_img, med_filt);
    }
  }
  // Threshold
  for ( cur_row = 0; cur_row < input_img.height; cur_row++ )
  {
      for ( cur_col = 0; cur_col < input_img.width; cur_col++ )
      {
          pixel = (int32_t) filt_img[cur_row][cur_col];
          if (pixel > 255)
          {
              filter_img.mono[cur_row][cur_col] = 255;
          }
          else if (pixel < 0)
          {
              filter_img.mono[cur_row][cur_col] = 0;
          }
          else
          {
              filter_img.mono[cur_row][cur_col] = pixel;
          }
      }
  }

  free_img( (void**)source_img );
  free_img( (void**)filt_img );
}
