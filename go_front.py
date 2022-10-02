import numpy as np
import sympy as sp
import matplotlib.pyplot as plt


x = np.linspace(0,2*np.pi,12)

temp = list(x[:7])+list(x[8:11])+list(x[11:])
x = np.array(temp)
y = np.array([0,8,18,30,20,10,3,2,1,0,0])
l = np.polyfit(x,y,7)

print(l)
n1_x = np.linspace(0,2*np.pi,50)
n1_y=np.polyval(l,n1_x)
plt.title("Leg Movement on z Axis")
plt.xlabel("time")
plt.ylabel("position")
plt.plot(n1_x,n1_y)
plt.scatter(x,y)
plt.show()

x1 = np.linspace(0,2*np.pi,9)
y1 = np.array([0,5,12,18,12,10,4,2,0])
f1 = np.polyfit(x1,y1,3)
print(f1)
n2_x = np.linspace(0,2*np.pi,50)
n2_y=np.polyval(f1,n2_x)
plt.title("Leg Movement on x Axis")
plt.xlabel("time")
plt.ylabel("position")
plt.scatter(x1,y1)
plt.plot(n2_x,n2_y)
plt.show()


n1_y = n1_y[3:-10]
n2_y = n2_y[3:-10]
plt.scatter(n1_y,n2_y)
plt.plot(n1_y,n2_y)
plt.title("Leg Movement in x-z Plane")
plt.xlabel("x direction")
plt.ylabel("z direction")
plt.show()