
#include <math.h>
#include "tiff.h"
#include "allocate.h"
#include "randlib.h"
#include "typeutil.h"
#include "pixel.h"
#include "LInkedList.h"

void error(char *name);

void SetArrayToValue(unsigned int** array, int height, int width, unsigned int value);

void ConnectedSet(
struct pixel s,
double T,
unsigned char **img,
int width,
int height,
int ClassLabel,
unsigned int **seg,
int *NumConPixels);

void CopyGrayscaleTiffIntoArray(struct TIFF_img img, uint8_t **img_array);

void OutputClassArrayAsTiff(unsigned int** class_array, int height, int width, int class_label);

void ConnectedSetDriver(struct TIFF_img input_img);

void ConnectedNeighbors(
struct pixel s,
double T,
uint8_t **img,
int width,
int height,
int *M,
struct pixel c[4]);

void TestConnectedNeighbors(struct TIFF_img input_img);

int main (int argc, char **argv)
{
  FILE *fp_input;
//  FILE *fp_color;
  struct TIFF_img input_img;
//  struct TIFF_img color_img;
//  double** filt;
//  filt = (double **)get_img(FILTER_LENGTH,
//                            FILTER_LENGTH,
//                            sizeof(double));
//  InitFilter(filt);
//  CreatePointSpreadFunction(filt, 256, 256);

  if ( argc != 2 ) error( argv[0] );

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
  ConnectedSetDriver(input_img);
//  TestConnectedNeighbors(input_img);
  /* close image file */
  fclose ( fp_input );
//  get_TIFF ( &color_img, input_img.height,
//            input_img.width, 'g' );


  // Process red, green, blue
//  for(int color = 0; color < 3; color++)
//  {
//    ProcessSingleColor(color, input_img, color_img, filt);
//  }

//  if ( ( fp_color = fopen ( "Prob5Filt.tif", "wb" ) ) == NULL ) {
//    fprintf ( stderr, "cannot open file Prob5Filt.tif\n");
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

  free_TIFF ( &(input_img) );
//  free_TIFF ( &(color_img) );

  return(0);
}

void CopyGrayscaleTiffIntoArray(struct TIFF_img img, unsigned char **img_array)
{
  uint8_t pixel;
  for(int cur_row = 0; cur_row < img.height; cur_row++)
  {
    for(int cur_col = 0; cur_col < img.width; cur_col++)
    {
      pixel = img.mono[cur_row][cur_col];
      img_array[cur_row][cur_col] = img.mono[cur_row][cur_col];
    }
  }
}

void ConnectedNeighbors(
struct pixel s,
double T,
uint8_t **img,
int width,
int height,
int *M,
struct pixel c[4])
{
  uint8_t center_value;
  uint8_t neighbor_value;
  // Center value doesn't change, so set here
  center_value = img[s.m][s.n];
  // Init M
  *M = 0;
  // Test neighbor above, including boundary check
  if (s.m > 0)
  {
    neighbor_value = img[s.m-1][s.n];
    if (abs(img[s.m][s.n] - img[s.m-1][s.n]) <= T)
    {
      // Connected, so add pixel then increment M
      c[*M].m = s.m-1;
      c[*M].n = s.n;
      (*M)++;
    }
  }
  // Test neighbor to left, including boundary check
  if (s.n > 0)
  {
    if (abs(img[s.m][s.n] - img[s.m][s.n-1]) <= T)
    {
      // Connected, so add pixel then increment M
      c[*M].m = s.m;
      c[*M].n = s.n-1;
      (*M)++;
    }
  }
  // Test neighbor to right, including boundary check
  if (s.n < (width-1))
  {
    if (abs(img[s.m][s.n] - img[s.m][s.n+1]) <= T)
    {
      // Connected, so add pixel then increment M
      c[*M].m = s.m;
      c[*M].n = s.n+1;
      (*M)++;
    }
  }
  // Test neighbor below, including boundary check
  if (s.m < (height-1))
  {
    if (abs(img[s.m][s.n] - img[s.m+1][s.n]) <= T)
    {
      // Connected, so add pixel then increment M
      c[*M].m = s.m+1;
      c[*M].n = s.n;
      (*M)++;
    }
  }
};

void TestConnectedNeighbors(struct TIFF_img input_img)
{
  int debug;
  int cur_col = 0;
  int cur_row = 0;
  uint8_t **img_array;
  img_array = (uint8_t **)get_img(input_img.height,
                            input_img.width,
                            sizeof(uint8_t));
  CopyGrayscaleTiffIntoArray(input_img, img_array);
  // Input, output arguments
  struct pixel test_pixel;
  test_pixel.m = 45; // row
  test_pixel.n = 67; // col
  int num_neighbors;
  struct pixel neighbors[4];
  // 0 connected neighbors
  img_array[45][67] = 5;
  img_array[44][67] = 0;
  img_array[45][66] = 0;
  img_array[45][68] = 0;
  img_array[46][67] = 0;
  ConnectedNeighbors(test_pixel, 1, img_array, input_img.width, input_img.height,
                     &num_neighbors, neighbors);
  if (num_neighbors == 0)
  {
    debug = 1;
  }
  else
  {
    debug = 0;
  }
  // 1 connected neighbors
  img_array[45][67] = 5;
  img_array[44][67] = 4;
  img_array[45][66] = 0;
  img_array[45][68] = 0;
  img_array[46][67] = 0;
  ConnectedNeighbors(test_pixel, 1, img_array, input_img.width, input_img.height,
                     &num_neighbors, neighbors);
  if (num_neighbors == 1)
  {
    debug = 1;
  }
  else
  {
    debug = 0;
  }
  // 2 connected neighbors
  img_array[45][67] = 5;
  img_array[44][67] = 4;
  img_array[45][66] = 4;
  img_array[45][68] = 0;
  img_array[46][67] = 0;
  ConnectedNeighbors(test_pixel, 1, img_array, input_img.width, input_img.height,
                     &num_neighbors, neighbors);
  if (num_neighbors == 2)
  {
    debug = 1;
  }
  else
  {
    debug = 0;
  }
  // 3 connected neighbors
  img_array[45][67] = 5;
  img_array[44][67] = 4;
  img_array[45][66] = 4;
  img_array[45][68] = 4;
  img_array[46][67] = 0;
  ConnectedNeighbors(test_pixel, 1, img_array, input_img.width, input_img.height,
                     &num_neighbors, neighbors);
  if (num_neighbors == 3)
  {
    debug = 1;
  }
  else
  {
    debug = 0;
  }
  // 4 connected neighbors
  img_array[45][67] = 5;
  img_array[44][67] = 4;
  img_array[45][66] = 4;
  img_array[45][68] = 4;
  img_array[46][67] = 4;
  ConnectedNeighbors(test_pixel, 1, img_array, input_img.width, input_img.height,
                     &num_neighbors, neighbors);
  if (num_neighbors == 4)
  {
    debug = 1;
  }
  else
  {
    debug = 0;
  }
  free_img( (void**)img_array );
}

void ConnectedSet(
struct pixel s,
double T,
unsigned char **img,
int width,
int height,
int ClassLabel,
unsigned int **seg,
int *NumConPixels)

{
  struct node *cur_node;
  struct node *test_node;
  struct pixel c[4];
  int num_neighbors;
  int neighbor_index;
  // Empty B
  deleteFirst();
  if (isEmpty())
  {
    printf("List empty\n");
  }
  insertFirst(s,0);
  while (!isEmpty())
  {
    //printList();
    cur_node = getHead();
    CopyPixel(cur_node->key, &s);
    delete(s);
    // Mark pixel with ClassLabel
    seg[s.m][s.n] = ClassLabel;
//    printf("%d,%d ClassLabel %d",s.m, s.n, ClassLabel);
    ConnectedNeighbors(s,T,img,width, height, &num_neighbors,c);
    // Step through all neighbors.  If there are any without a class, add them
    for(neighbor_index = 0; neighbor_index < num_neighbors;neighbor_index++)
    {
      if (seg[c[neighbor_index].m][c[neighbor_index].n] == 0)
      {
        test_node = find(c[neighbor_index]);
        if (test_node == NULL)
        {
          insertFirst(c[neighbor_index],0);
        }
      }
    }
  }
}

void ConnectedSetDriver(struct TIFF_img input_img)
{
  unsigned int **class_label_array;
  uint8_t **img_array;
  int ClassLabel = 1;
  img_array = (uint8_t **)get_img(input_img.height,
                            input_img.width,
                            sizeof(uint8_t));
  class_label_array = (unsigned int **)get_img(input_img.height,
                            input_img.width,
                            sizeof(unsigned int));
  CopyGrayscaleTiffIntoArray(input_img, img_array);
  // Set class label to 0
  SetArrayToValue(class_label_array, input_img.height, input_img.width, 0);
  struct pixel seed_pixel;
  seed_pixel.m = 67;
  seed_pixel.n = 45;
  int num_conn_pixels = 0;
  ConnectedSet(seed_pixel,1,img_array,input_img.width,input_img.height,ClassLabel,
               class_label_array,&num_conn_pixels);
  OutputClassArrayAsTiff(class_label_array, input_img.height, input_img.width, ClassLabel);
  // Display/save image of connected pixels
  free_img( (void**)img_array );
  free_img( (void**)class_label_array );
}

void SetArrayToValue(unsigned int** array, int height, int width, unsigned int value)
{
  for(int cur_row = 0; cur_row < height; cur_row++)
  {
    for(int cur_col = 0; cur_col < width; cur_col++)
    {
      array[cur_row][cur_col]=value;
    }
  }
}

void OutputClassArrayAsTiff(unsigned int** class_array, int height, int width, int class_label)
{
    int cur_row;
    int cur_col;
    FILE *fp_set;
    struct TIFF_img set_img;
    get_TIFF(&set_img, height,
        width, 'g');
    for (cur_row = 0; cur_row < height; cur_row++)
    {
        for (cur_col = 0; cur_col < width; cur_col++)
        {
            if (class_array[cur_row][cur_col] == class_label)
            {
                set_img.mono[cur_row][cur_col] = 255;
            }
            else
            {
                set_img.mono[cur_row][cur_col] = 0;
            }
        }
    }
    if ((fp_set = fopen("Set.tif", "wb")) == NULL) {
        fprintf(stderr, "cannot open Set.tif\n");
        exit(1);
    }

    if (write_TIFF(fp_set, &set_img)) {
        fprintf(stderr, "error writing TIFF file\n");
        exit(1);
    }

    fclose(fp_set);

    free_TIFF(&(set_img));
}

//  if ( ( fp_point_spread_file = fopen ( "PP.tif", "wb" ) ) == NULL ) {
//    fprintf ( stderr, "cannot open file PS.tif\n");
//    exit ( 1 );
//  }
//
//  // Write color image
//  if ( write_TIFF ( fp_point_spread_file, &point_spread_img ) ) {
//    fprintf ( stderr, "error writing TIFF file\n");
//    exit ( 1 );
//  }
//
//  // Close color image file
//  fclose ( fp_point_spread_file );
//
//  free_img( (void**)ps );
//}


void error(char *name)
{
    printf("usage:  %s  image.tiff \n\n",name);
    exit(1);
}
