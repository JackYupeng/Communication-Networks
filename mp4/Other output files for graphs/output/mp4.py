import matplotlib.pyplot as plt
from sys import exit
from scipy.ndimage.filters import gaussian_filter1d
from scipy.interpolate import spline


with open('output1.txt') as f: #initial value of R:1. L=20
    x, y = [], []
    for l in f:
        #x, y = l.split()
        #plt.plot(x, y)
        #plt.show()
        row = l.split()
        x.append(int(row[0]))
        y.append(row[1])

with open('output2.txt') as f: #initial value of R:2. L=40
    x2, y2 = [], []
    for l in f:
        #x, y = l.split()
        #plt.plot(x, y)
        #plt.show()
        row = l.split()
        x2.append(int(row[0]))
        y2.append(row[1])

with open('output4.txt') as f: #initial value of R:4. L=60
    x3, y3 = [], []
    for l in f:
        #x, y = l.split()
        #plt.plot(x, y)
        #plt.show()
        row = l.split()
        x3.append(int(row[0]))
        y3.append(row[1])

with open('output8.txt') as f: #initial value of R:8. L=80
    x4, y4 = [], []
    for l in f:
        #x, y = l.split()
        #plt.plot(x, y)
        #plt.show()
        row = l.split()
        x4.append(int(row[0]))
        y4.append(row[1])

with open('output16.txt') as f: #initial value of R:16. L=100
    x5, y5 = [], []
    for l in f:
        #x, y = l.split()
        #plt.plot(x, y)
        #plt.show()
        row = l.split()
        x5.append(int(row[0]))
        y5.append(row[1])

x_smooth = np.linspace(min(x), max(x), 300)
y_smooth = spline(x, y, x_smooth)

plt.plot(x_smooth, y_smooth, color = 'red')

#ysmoothed = gaussian_filter1d(y, sigma=2)

#y2smoothed = gaussian_filter1d(y2, sigma=2)
#y3smoothed = gaussian_filter1d(y3, sigma=2)
#y4smoothed = gaussian_filter1d(y4, sigma=2)
#y5smoothed = gaussian_filter1d(y5, sigma=2)

#plt.plot(x, ysmoothed, color = 'red')
#plt.plot(x2,y2smoothed, color = 'blue')
#plt.plot(x3,y3smoothed, color = 'green')
#plt.plot(x4,y4smoothed, color = 'black')
#plt.plot(x5,y5smoothed, color = 'pink')


plt.xlabel('Number of Nodes')
plt.ylabel('Channel utilization (in percentage)')
axes = plt.gca()
#axes.set_xlim([xmin,xmax])
#axes.set_ylim([min(y),max(y)])
#plt.axes()
#plt.axes((5, 500, 500, 8000), facecolor = 'w')
#plt.axis([5, 500, 0, 1])
#plt.xlim(5,500)
plt.yticks([])
plt.show()
exit()

