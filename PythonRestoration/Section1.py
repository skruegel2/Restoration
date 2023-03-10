import matplotlib.pyplot as plt
from matplotlib import cm
import numpy as np
from PIL import Image
from numpy import linalg as LA

# Section 1
img14g = Image.open('img14g.tif')
img14g_plot = plt.imshow(img14g)
plt.show()
img14bl = Image.open('img14bl.tif')
img14bl_plot = plt.imshow(img14bl)
plt.show()
img14gn = Image.open('img14gn.tif')
img14gn_plot = plt.imshow(img14gn)
plt.show()
img14sp = Image.open('img14sp.tif')
img14sp_plot = plt.imshow(img14sp)
plt.show()
