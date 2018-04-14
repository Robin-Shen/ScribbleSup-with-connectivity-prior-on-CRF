import math
import numpy as np
from PIL import Image
m = []
k=20
for i in range(k):
    mitte = math.floor(k/2)
    zeile = [1/(abs(mitte-i)+abs(mitte-j)+1) for j in range(k)]
    m.append(zeile)
m = np.array(m)
print(m)
im = Image.fromarray(A)
im.save("your_file.jpeg")