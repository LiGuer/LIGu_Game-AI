import cv2
import numpy as np
import yaml
import matplotlib.pyplot as plt
import matplotlib.image as mpimg
from mpl_toolkits.mplot3d import Axes3D

from FaceBoxes.FaceBoxes import *
from TDDFA.TDDFA import *
from TDDFA.utils.functions import draw_landmarks, get_suffix
from TDDFA.utils.pose import viz_pose

if __name__ == '__main__':
    figure = plt.figure()
    ax = figure.add_subplot(111, projection='3d')

    img_fp = "OIP.jpg"
    img = cv2.imread(img_fp)
    # FaceBoxes
    face_boxes = FaceBoxes()
    boxes = face_boxes(img)
    face = boxes[0]
    # tddfa
    cfg = yaml.load(open('TDDFA/configs/mb1_120x120.yml'), Loader=yaml.SafeLoader)
    tddfa = TDDFA(gpu_mode='gpu', **cfg)
    param_lst, roi_box_lst = tddfa(img, boxes)
    ver_lst = tddfa.recon_vers(param_lst, roi_box_lst, dense_flag=False)

    draw_landmarks(img, ver_lst, show_flag=False, dense_flag=False, wfp=f'Ans_jpg' + '.jpg')
    viz_pose(img, param_lst, ver_lst, show_flag=False, wfp=f'Anspose_jpg' + '.jpg')

    ver_lst = ver_lst[0]
    print(ver_lst)
    ax.scatter(ver_lst[0], ver_lst[1], ver_lst[2])
    plt.show()

    