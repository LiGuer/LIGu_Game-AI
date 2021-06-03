import numpy as np
import torch
import torch. nn as nn
import scipy.misc
import os
# ============网络结构============
class BasicConv2d(nn.Module):
    def __init__(self, in_channels, out_channals, **kwargs):
        super().__init__()
        self.conv = nn.Conv2d(in_channels, out_channals, **kwargs)
        #self.bn = nn.BatchNorm2d(out_channals)
        self.relu = nn.ReLU(inplace=True)

    def forward(self, x):
        x = self.conv(x)
        #x = self.bn(x)
        x = self.relu(x)
        return x

class Inception(nn.Module):
    def __init__(self, in_planes, n1x1, n3x3red, n3x3, n5x5red, n5x5, pool_planes):
        super().__init__()
        # ======1x1 conv branch======
        self.b1 = BasicConv2d(in_planes, n1x1, kernel_size=1)
        # ======1x1 conv -> 3x3 conv branch======
        self.b2_1x1_a = BasicConv2d(in_planes, n3x3red, kernel_size=1)
        self.b2_3x3_b = BasicConv2d(n3x3red, n3x3, kernel_size=3, padding=1)
        # ======1x1 conv -> 3x3 conv -> 3x3 conv branch======
        self.b3_1x1_a = BasicConv2d(in_planes, n5x5red, kernel_size=1)
        self.b3_3x3_b = BasicConv2d(n5x5red, n5x5, kernel_size=3, padding=1)
        self.b3_3x3_c = BasicConv2d(n5x5, n5x5, kernel_size=3, padding=1)
        # ======x3 pool -> 1x1 conv branch======
        self.b4_pool = nn.MaxPool2d(3, stride=1, padding=1)
        self.b4_1x1 = BasicConv2d(in_planes, pool_planes, kernel_size=1)

    def forward(self, x):
        y1 = self.b1(x)
        y2 = self.b2_3x3_b(self.b2_1x1_a(x))
        y3 = self.b3_3x3_c(self.b3_3x3_b(self.b3_1x1_a(x)))
        y4 = self.b4_1x1(self.b4_pool(x))
        return torch.cat([y1, y2, y3, y4], 1)

class CNN(nn.Module):
    def __init__(self):
        super().__init__()
        self.conv1 = BasicConv2d(1,16,kernel_size=3, padding=1)
        self.maxpool = nn.MaxPool2d(2)
        self.conv2 = BasicConv2d(16,32,kernel_size=3, padding=1)
        # ======fc======
        self.linear1 = nn.Linear(32*7*7,128)
        self.relu = nn.ReLU()
        self.linear2 = nn.Linear(128,128)
        self.linear3 = nn.Linear(128,44)

    def viewer(self,x):
        _x=x[0][0].cpu().detach().numpy().squeeze()
        for i in range(len(x[0])):
            if(i==0):
                continue
            _x=np.r_[_x,x[0][i].cpu().detach().numpy().squeeze()]
        scipy.misc.imsave("imgcnnconv-"+str(len(x[0]))+".jpg",_x)

    def forward(self, x):
        self.viewer(x)
        #======
        x = self.conv1(x)
        x = self.maxpool(x)
        self.viewer(x)
        #======
        x = self.conv2(x)
        x = self.maxpool(x)
        self.viewer(x)
        #======
        x = x.view(x.size(0), -1)   # 展平多维的卷积图成 (batch_size, 32 * 7 * 7)
        x = self.linear1(x)
        x = self.relu(x)
        x = self.linear2(x)
        x = self.relu(x)
        x = self.linear3(x)
        return x