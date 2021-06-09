import cv2 as cv
import pandas as pd
import numpy as np
import scipy.misc as sm
import os
import torch
import torch.nn as nn
import torch.utils.data as Data
import torchvision
import matplotlib.pyplot as plt

face_detector=cv.CascadeClassifier("D:/opencv/sources/data/haarcascades/haarcascade_frontalface_alt_tree.xml")

def cv_imread(file_path):
    root_dir, file_name = os.path.split(file_path)
    pwd = os.getcwd()
    if root_dir:
        os.chdir(root_dir)
    cv_img = cv.imread(file_name)
    os.chdir(pwd)
    return cv_img

def face_detect(srcPath,dstPath) :
    for filename in os.listdir(srcPath):
        srcFile=os.path.join(srcPath,filename)
        dstFile=os.path.join(dstPath,filename)
        if os.path.isfile(srcFile) and ( filename[-4:] in ['.jpg', '.png'] or filename[-5:] in ['.jpeg']):
            #try:
            image = cv_imread(srcFile)
            faces = face_detector.detectMultiScale(image, 1.02, 5)
            for x, y, w, h in faces:
                face_image= image[y:y+h, x:x+w]
                cv.imwrite(dstPath + filename + str(x) +  ".jpg", face_image)
            print(srcFile+" Ok")
            #except Exception:
                #print(srcFile+" No")
        if os.path.isdir(srcFile):
            face_detect(srcFile, dstPath)


face_detect('D:/wamp64/www/仓库/图片/相册/QQ/', 'Data/') 