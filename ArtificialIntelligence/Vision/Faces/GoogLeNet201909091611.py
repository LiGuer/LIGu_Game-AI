import pandas as pd
import numpy as np
import scipy.misc as sm
import os
import torch
import torch. nn as nn
import torch.utils.data as Data
import torchvision

x=np.load('C:/Users/������/Desktop/CNN_Work/Data_x.npy')
y=np.load('C:/Users/������/Desktop/CNN_Work/Data_y.npy')
t_x=np.load('C:/Users/������/Desktop/CNN_Work/Test_x.npy')
t_y=np.load('C:/Users/������/Desktop/CNN_Work/Test_y.npy')
x = torch.from_numpy(x)
y = torch.from_numpy(y)
t_x = torch.from_numpy(t_x)
t_y = torch.from_numpy(t_y)
x=x.float()
t_x=t_x.float()

print('Data Ready')

torch_dataset = Data.TensorDataset(x,y)
train_loader = Data.DataLoader(
    dataset=torch_dataset,
    batch_size=15,
    shuffle=True,  
    num_workers=2, 
)

torch_dataset_test = Data.TensorDataset(t_x,t_y)
test_loader = Data.DataLoader(
    dataset=torch_dataset_test,
    batch_size=5,
    shuffle=True,  
    num_workers=2, 
)

class BasicConv2d(nn.Module):
    def __init__(self, in_channels, out_channals, **kwargs):
        super(BasicConv2d, self).__init__()
        self.conv = nn.Conv2d(in_channels, out_channals, **kwargs)
        self.bn = nn.BatchNorm2d(out_channals)
        self.relu = nn.ReLU(inplace=True)
    def forward(self, x):
        x = self.conv(x)
        x = self.bn(x)
        x = self.relu(x)
        return x

class Inception(nn.Module):
    def __init__(self, in_planes,n1x1, n3x3red, n3x3, n5x5red, n5x5, pool_planes):
        super(Inception, self).__init__()
        		# 1x1 conv branch
        self.b1 = BasicConv2d(in_planes, n1x1, kernel_size=1)
        		# 1x1 conv -> 3x3 conv branch
        self.b2_1x1_a = BasicConv2d(in_planes, n3x3red, kernel_size=1)
        self.b2_3x3_b = BasicConv2d(n3x3red, n3x3, kernel_size=3, padding=1)
        		# 1x1 conv -> 3x3 conv -> 3x3 conv branch
        self.b3_1x1_a = BasicConv2d(in_planes, n5x5red, kernel_size=1)
        self.b3_3x3_b = BasicConv2d(n5x5red, n5x5, kernel_size=3, padding=1)
        self.b3_3x3_c = BasicConv2d(n5x5, n5x5, kernel_size=3, padding=1)
        		# 3x3 pool -> 1x1 conv branch
        self.b4_pool = nn.MaxPool2d(3, stride=1, padding=1)
        self.b4_1x1 = BasicConv2d(in_planes, pool_planes, kernel_size=1)
    def forward(self, x):
        y1 = self.b1(x)
        y2 = self.b2_3x3_b(self.b2_1x1_a(x))
        y3 = self.b3_3x3_c(self.b3_3x3_b(self.b3_1x1_a(x)))
        y4 = self.b4_1x1(self.b4_pool(x))
        return torch.cat([y1, y2, y3, y4], 1)

class GoogLeNet(nn.Module):
    def __init__(self):
        super(GoogLeNet, self).__init__()
        self.pre_layers = BasicConv2d(1, 64, kernel_size=5,padding=2)
        self.a3 = Inception(64,  64,  96, 128, 16, 32, 32)
        self.b3 = Inception(256, 128, 128, 192, 32, 96, 64)
        self.maxpool = nn.MaxPool2d(3, stride=2, padding=1)
        self.a4 = Inception(480, 192,  96, 208, 16,  48,  64)
        self.b4 = Inception(512, 160, 112, 224, 24,  64,  64)
        self.c4 = Inception(512, 128, 128, 256, 24,  64,  64)
        self.d4 = Inception(512, 112, 144, 288, 32,  64,  64)
        self.e4 = Inception(528, 256, 160, 320, 32, 128, 128)
        self.a5 = Inception(832, 256, 160, 320, 32, 128, 128)
        self.b5 = Inception(832, 384, 192, 384, 48, 128, 128)
        self.avgpool = nn.AvgPool2d(8, stride=1)
        self.linear = nn.Linear(1024*5*5, 7)
    def forward(self, x):
        out = self.pre_layers(x)
        out = self.a3(out)
        out = self.b3(out)
        out = self.maxpool(out)
        out = self.a4(out)
        out = self.b4(out)
        out = self.c4(out)
        out = self.d4(out)
        out = self.e4(out)
        out = self.maxpool(out)
        out = self.a5(out)
        out = self.b5(out)
        out = self.avgpool(out)
        out = out.view(out.size(0), -1)
        out = self.linear(out)
        return out
    
cnn = GoogLeNet().cuda()
print(cnn)
import time

optimizer = torch.optim.Adam(cnn.parameters(), lr=0.0002,weight_decay=1e-8) 
loss_func = nn.CrossEntropyLoss().cuda()
scheduler = torch.optim.lr_scheduler.ReduceLROnPlateau(optimizer, 'max')

print('Start Train')

def AccuracyOutput():
    sum = 0
    for step, (b_x, b_y) in enumerate(test_loader): 
        b_x,b_y=b_x.cuda(),b_y.cuda()
        test_output = cnn(b_x).cpu()
        pred_y = torch.max(test_output, 1)[1].data.numpy().squeeze()
        pred_y = torch.from_numpy(pred_y)
        b_x,b_y=b_x.cpu(),b_y.cpu()
        for i in range(len(b_y)):
            if b_y[i] == pred_y[i]:
                sum = sum + 1
    accuracy = sum/(len(t_y))
    print('Accuracy: {:.5f}'.format(accuracy)) 
    return accuracy

maxaccuracy = 0.01
for epoch in range(100):
    start = time.time()
    train_loss = 0
    for step, (b_x, b_y) in enumerate(train_loader):
        b_x,b_y=b_x.cuda(),b_y.cuda()
        output = cnn(b_x) 
        loss = loss_func(output, b_y).cpu()
        optimizer.zero_grad()
        loss.backward() 
        optimizer.step()
        train_loss += loss
        if step%500==400:
            accuracy = AccuracyOutput()
    print('epoch: {}, Train Loss: {:.6f}'.format(epoch, train_loss / len(x)))
    accuracy = AccuracyOutput()
    scheduler.step(accuracy) 
    if(accuracy>maxaccuracy):
        torch.save(cnn, 'google201909091611.pkl')
        maxaccuracy = accuracy
    end = time.time() 
    print('use time: {:.5f} s'.format(end - start))




############################################################









import pandas as pd
import numpy as np
import scipy.misc as sm
import os
import torch
import torch. nn as nn
import torch.utils.data as Data
import torchvision

x=np.load('C:/Users/������/Desktop/CNN_Work/Data_x.npy')
y=np.load('C:/Users/������/Desktop/CNN_Work/Data_y.npy')
t_x=np.load('C:/Users/������/Desktop/CNN_Work/Test_x.npy')
t_y=np.load('C:/Users/������/Desktop/CNN_Work/Test_y.npy')
x = torch.from_numpy(x)
y = torch.from_numpy(y)
t_x = torch.from_numpy(t_x)
t_y = torch.from_numpy(t_y)
x=x.float()
t_x=t_x.float()

print('Data Ready')

torch_dataset = Data.TensorDataset(x,y)
train_loader = Data.DataLoader(
    dataset=torch_dataset,
    batch_size=20,
    shuffle=True,  
    num_workers=4, 
)
torch_dataset_test = Data.TensorDataset(t_x,t_y)
test_loader = Data.DataLoader(
    dataset=torch_dataset_test,
    batch_size=10,
    shuffle=True,  
    num_workers=2, 
)

class BasicConv2d(nn.Module):
    def __init__(self, in_channels, out_channals, **kwargs):
        super(BasicConv2d, self).__init__()
        self.conv = nn.Conv2d(in_channels, out_channals, **kwargs)
        self.bn = nn.BatchNorm2d(out_channals)
        self.relu = nn.ReLU(inplace=True)
    def forward(self, x):
        x = self.conv(x)
        x = self.bn(x)
        x = self.relu(x)
        return x

class Inception(nn.Module):
    def __init__(self, in_planes,n1x1, n3x3red, n3x3, n5x5red, n5x5, pool_planes):
        super(Inception, self).__init__()
        		# 1x1 conv branch
        self.b1 = BasicConv2d(in_planes, n1x1, kernel_size=1)
        		# 1x1 conv -> 3x3 conv branch
        self.b2_1x1_a = BasicConv2d(in_planes, n3x3red, kernel_size=1)
        self.b2_3x3_b = BasicConv2d(n3x3red, n3x3, kernel_size=3, padding=1)
        		# 1x1 conv -> 3x3 conv -> 3x3 conv branch
        self.b3_1x1_a = BasicConv2d(in_planes, n5x5red, kernel_size=1)
        self.b3_3x3_b = BasicConv2d(n5x5red, n5x5, kernel_size=3, padding=1)
        self.b3_3x3_c = BasicConv2d(n5x5, n5x5, kernel_size=3, padding=1)
        		# 3x3 pool -> 1x1 conv branch
        self.b4_pool = nn.MaxPool2d(3, stride=1, padding=1)
        self.b4_1x1 = BasicConv2d(in_planes, pool_planes, kernel_size=1)
    def forward(self, x):
        y1 = self.b1(x)
        y2 = self.b2_3x3_b(self.b2_1x1_a(x))
        y3 = self.b3_3x3_c(self.b3_3x3_b(self.b3_1x1_a(x)))
        y4 = self.b4_1x1(self.b4_pool(x))
        return torch.cat([y1, y2, y3, y4], 1)

class GoogLeNet(nn.Module):
    def __init__(self):
        super(GoogLeNet, self).__init__()
        self.pre_layers = BasicConv2d(1, 64, kernel_size=5,padding=2)
        self.a3 = Inception(64,  64,  96, 128, 16, 32, 32)
        self.b3 = Inception(256, 128, 128, 192, 32, 96, 64)
        self.maxpool = nn.MaxPool2d(3, stride=2, padding=1)
        self.a4 = Inception(480, 192,  96, 208, 16,  48,  64)
        self.b4 = Inception(512, 160, 112, 224, 24,  64,  64)
        self.c4 = Inception(512, 128, 128, 256, 24,  64,  64)
        self.d4 = Inception(512, 112, 144, 288, 32,  64,  64)
        self.e4 = Inception(528, 256, 160, 320, 32, 128, 128)
        self.a5 = Inception(832, 256, 160, 320, 32, 128, 128)
        self.b5 = Inception(832, 384, 192, 384, 48, 128, 128)
        self.avgpool = nn.AvgPool2d(8, stride=1)
        self.linear = nn.Linear(1024*5*5, 7)
    def forward(self, x):
        out = self.pre_layers(x)
        out = self.a3(out)
        out = self.b3(out)
        out = self.maxpool(out)
        out = self.a4(out)
        out = self.b4(out)
        out = self.c4(out)
        out = self.d4(out)
        out = self.e4(out)
        out = self.maxpool(out)
        out = self.a5(out)
        out = self.b5(out)
        out = self.avgpool(out)
        out = out.view(out.size(0), -1)
        out = self.linear(out)
        return out
    
cnn = torch.load('google201909091611.pkl').cuda()
#print(cnn)
import time

optimizer = torch.optim.Adam(cnn.parameters(), lr=0.0002,weight_decay=1e-8) 
loss_func = nn.CrossEntropyLoss().cuda()
scheduler = torch.optim.lr_scheduler.ReduceLROnPlateau(optimizer, 'max')

print('Start Train')

def AccuracyOutput():
    sum = 0
    for step, (b_x, b_y) in enumerate(test_loader): 
        b_x,b_y=b_x.cuda(),b_y.cuda()
        test_output = cnn(b_x).cpu()
        pred_y = torch.max(test_output, 1)[1].data.numpy().squeeze()
        pred_y = torch.from_numpy(pred_y)
        b_x,b_y=b_x.cpu(),b_y.cpu()
        for i in range(len(b_y)):
            if b_y[i] == pred_y[i]:
                sum = sum + 1
    accuracy = sum/(len(t_y))
    print('Accuracy: {:.5f}'.format(accuracy)) 
    return accuracy

maxaccuracy = 0.62571
for epoch in range(100):
    start = time.time()
    train_loss = 0
    for step, (b_x, b_y) in enumerate(train_loader):
        b_x,b_y=b_x.cuda(),b_y.cuda()
        output = cnn(b_x) 
        loss = loss_func(output, b_y).cpu()
        optimizer.zero_grad()
        loss.backward() 
        optimizer.step()
        train_loss += loss
        if step==1000:
            accuracy = AccuracyOutput()
    print('epoch: {}, Train Loss: {:.6f}'.format(epoch, train_loss / len(x)))
    accuracy = AccuracyOutput()
    scheduler.step(accuracy) 
    if(accuracy>maxaccuracy):
        torch.save(cnn, 'google201909091611.pkl')
        maxaccuracy = accuracy
    end = time.time() 
    print('use time: {:.5f} s'.format(end - start))