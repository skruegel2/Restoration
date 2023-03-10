from pickletools import uint8
import matplotlib.pyplot as plt
from matplotlib import cm
import numpy as np
from PIL import Image

def equalize(X):
    x = np.array(X)
    print(x.shape[0], x.shape[1])
    F_x = np.ndarray(shape=(256,))
    F_xtot = np.ndarray(shape=(256,))
    N,test_bins,patches = plt.hist(x.flatten(), bins=np.linspace(0,255,256))
    F_total = 0
    for cur_bin in range(0,255):
        F_total = F_total + N[cur_bin]
        F_xtot[cur_bin] = F_total
    for cur_bin in range(0,256):
        F_x[cur_bin] = F_xtot[cur_bin]/F_total
    x_axis = np.linspace(0,255,256)
    F_x[255] = 1.0
    plt.clf()
    plt.xlabel("i")
    plt.ylabel("Fx[i]")
    plt.title("Cumulative Distribution Function")    
    plt.plot(F_x)
    plt.show()
    # Find Ymin and Ymax.  Set them to the max, min respectively
    Ymin = 1.0
    Ymax = 0.0
    image_len = x.flatten().size
    x_flat = x.flatten()
    #print(image_len)
    for cur_pixel in range(0,image_len):
        if (Ymin > F_x[x_flat[cur_pixel]]):
            Ymin = F_x[x_flat[cur_pixel]]
        if (Ymax < F_x[x_flat[cur_pixel]]):
            Ymax = F_x[x_flat[cur_pixel]]
    print('Ymin:', Ymin)
    print('Ymax:', Ymax)
    #Initialize the equalized image
    x_eq = np.array(X)
    num_rows = x.shape[0]
    num_cols = x.shape[1]
    for cur_row in range(0,num_rows):
         for cur_col in range(0, num_cols):
             x_eq[cur_row][cur_col] = (255*(F_x[x[cur_row][cur_col]] - Ymin))/(Ymax -Ymin)
    gray = cm.get_cmap('gray', 256)
    plt.clf()
    plt.imshow(x_eq, cmap=gray);
    plt.show()

    # Histogram
    plt.clf()
    plt.hist(x_eq.flatten(), bins=np.linspace(0, 255, 266))
    plt.xlabel("Pixel Intensity")
    plt.ylabel("Number of Pixels")
    plt.title("Equalized Kids.tif Histogram")
    plt.show()
        
gray = cm.get_cmap('gray', 256)
#plt.clf()
im = Image.open('kids.tif')

equalize(im)
