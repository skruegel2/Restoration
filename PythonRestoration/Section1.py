from itertools import zip_longest
import matplotlib.pyplot as plt
from matplotlib import cm
import numpy as np
from PIL import Image
from numpy import linalg as LA

# Section 1
img14g = Image.open('img14g.tif')
#img14g_plot = plt.imshow(img14g)
#plt.show()
img14bl = Image.open('img14bl.tif')
#img14bl_plot = plt.imshow(img14bl)
#plt.show()
img14gn = Image.open('img14gn.tif')
#img14gn_plot = plt.imshow(img14gn)
#plt.show()
img14sp = Image.open('img14sp.tif')
#img14sp_plot = plt.imshow(img14sp)
#plt.show()

def calculate_Y(img):
    y_list = []
    # Convert into array
    y_data = np.array(img)
    for row_idx in range(y_data.shape[0]):
        for col_idx in range(y_data.shape[1]):
            if ((row_idx % 20 == 0) and (col_idx % 20 == 0)):
                y_list.append(y_data[row_idx,col_idx]) 
    Y = np.asarray(y_list)
    return Y

def get_window_pixels(z_data, row_idx, col_idx):
    z_row = []
    num_win_elements = 0
    for win_row_idx in range(-3,4):
        for win_col_idx in range(-3, 4):
            if (win_row_idx + row_idx < 0):
                z_row.append(0)
                num_win_elements += 1
            elif (win_row_idx + row_idx >= z_data.shape[0]):
                z_row.append(0)
                num_win_elements += 1
            elif (win_col_idx + col_idx < 0):
                z_row.append(0)
                num_win_elements += 1
            elif (win_row_idx + row_idx >= z_data.shape[1]):
                z_row.append(0)
                num_win_elements += 1
            else:
                z_row.append(z_data[row_idx+win_row_idx,col_idx+win_col_idx])
                num_win_elements += 1
    print("Win elements: ",num_win_elements)
    return z_row

def calculate_Z(img, Y):
    Z = np.zeros((Y.shape[0],49))
    z_row_idx = 0
    # Convert into array
    z_data = np.array(img)
    for row_idx in range(z_data.shape[0]):
        for col_idx in range(z_data.shape[1]):
            if ((row_idx % 20 == 0) and (col_idx % 20 == 0)):
                z_row = get_window_pixels(z_data, row_idx, col_idx)
                for z_col_idx in range(49):
                    Z[z_row_idx,z_col_idx] = z_row[z_col_idx]
                z_row_idx += 1;
    return Z

Y = calculate_Y(img14g)
Z = calculate_Z(img14bl,Y)
