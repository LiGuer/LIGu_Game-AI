import numpy as np
import torch
import torch. nn as nn
import time
import matplotlib.image as image
import matplotlib.pyplot as plt
import cv2
import scipy.misc
import warnings
import os
from torchsummary import summary
warnings.filterwarnings('ignore')
from cnn import *

class WrittenSymbol():
    def __init__(self,cnn_parameter_path):
        self.cnn = CNN()
        self.cnn.load_state_dict(torch.load(cnn_parameter_path))

    def imgprocess(self,img):
        img = np.dot(img[...,:3], [0.299, 0.587, 0.114]) #gray
        img = cv2.resize(img,(28,28),interpolation=cv2.INTER_CUBIC) #resize
        if(np.max(img)>1):
            img = img / 255
        img[img < 0.3] = 0
        return img

    def getAns(self,url):
        img = image.imread(url)
        img = self.imgprocess(img)
        img = np.expand_dims(np.expand_dims(img,axis=0),axis=0)
        img = torch.from_numpy(img).float()
        ans = self.cnn(img)
        ans = torch.max(ans, 1)[1].data.numpy().squeeze()
        return ans

def FileWalker(path):
    file_list = []
    for root, dirs, files in os.walk(path):
        for name in files:
            file_list.append(root+name)
    return file_list
    
'''
A B C D E F G H I J K L M N O P Q R S T U V W X Y Z a b d e f g h n r t 2 3 4 5 6 7 8 9
'''
if __name__ == "__main__":
    written_wum = WrittenSymbol('cnn_parameter0.000874.pkl')
    Y_cur=['A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S',
        'T','U','V','W','X','Y','Z','a','b','d','e','f','g','h','n','r','t','2',
        '3','4','5','6','7','8','9']
    cur=0
    file_list = FileWalker("img/")
    #summary(written_wum.cnn.cuda(), input_size=(1, 28, 28))

    for i in range(len(file_list)):
        num = written_wum.getAns(file_list[i])
        if(file_list[i][4]!=Y_cur[num]):
            print(file_list[i][4:],end=': ')
            print(Y_cur[num],end='      ')
            cur+=1
            if(cur%5==0):print("")