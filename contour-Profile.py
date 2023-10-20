#! /usr/bin/env python
from pylab import *
from numpy import *
import matplotlib.patches as patches
from matplotlib import pyplot as plt
import matplotlib.patches as patches
import matplotlib as mpl
mpl.rc('figure', max_open_warning = 0)
a = 1
b = 261121
z = zeros(b)
VX = zeros(b)
VY = zeros(b)
#data1 = loadtxt("difference_1")
for i in range(1,2,1):
   data = loadtxt('difference'+str(i))
   #data = loadtxt('QUIVER_time_1000')
   x =  data[:,1]
   y =  data[:,2]
   vx = data[:,2]
   vy = data[:,3]
   ke =  data[:,0]
   y1 = y
   VX += vx
   VY += vy
   #z += Gamma

#Gamma = z/400
   T = ke
   xi = np.linspace(min(x), max(x))
   yi = np.linspace(min(y), max(y))
   X, Y = np.meshgrid(xi, yi)
   T1 = griddata(x, y, T, xi, yi,'linear')


   fig = figure(figsize=(10,10))
   ax = fig.add_subplot(111)
   
   
   cp=plt.contourf(X,Y,T1,100, extend='both',cmap=mpl.cm.jet)#,np.linspace(-1,1,11))
   
   #ax.add_patch(rect)
   plt.colorbar(cp)
   #cbar = plt.colorbar(orientation='horizontal')
   #cbar.ax.tick_params(color="black", width=1, length=4, labelsize=22)
   #plt.set_label(r"time="+str(i), fontsize=22)
   plt.title( 'DIFF(nabla^2 $psi$ , rho)',fontsize=22)

   xlabel(r"$x$", fontsize=24)
   ylabel(r"$y$",fontsize=24)
   ax.tick_params(axis='both', which='major', labelsize=22)
   ax.yaxis.set_major_locator(plt.MultipleLocator(40))

   #quiver(x, y1, vx, vy,color='black')#,scale=0.08, units='inches',width = 0.025)
   savefig("difference_contour"+str(i)+".png")
   #show()
   #savefig("AR2_Contour_Temp_RTD_1p4.pdf")
   
   
   show()

