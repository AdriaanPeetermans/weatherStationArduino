import imageio

name = '50d'

im = imageio.imread(name+'.png')
f = open(name+'.bm','wb')

def toBytes(r,g,b):
    r = int(r/256.0*32)
    g = int(g/256.0*64)
    b = int(b/256.0*32)
    b1 = r*8 + int(g/8.0)
    b2 = (g-int(g/8.0)*8)*32 + b
    return bytes([b1,b2])

for i in range(50):
    for j in range(50):
        col = im[i,j,:]
        r = col[0]
        g = col[1]
        b = col[2]
        f.write(toBytes(r,g,b))
f.close()


