import numpy as np
import matplotlib.image as image
import cv2
import re

def fileWalker(path):
    file_list = []
    for root, dirs, files in os.walk(path):
        for name in files:
            file_list.append(root+name)
    return file_list

def imgProcess(inUrl,outUrl):
    img = image.imread(inUrl)
    img = np.dot(img[...,:3], [0.299, 0.587, 0.114]) #gray
    img = cv2.resize(img,(28,28),interpolation=cv2.INTER_CUBIC) #resize
    img[img < 50] = 0
    fo = open(outUrl, "w")
    t = str(np.round(img))
    t = re.sub(r'[\[,.\]]', "", t)
    fo.write(t)
    return img

if __name__  == '__main__':
    url = input()
    imgProcess(url, "data.txt")