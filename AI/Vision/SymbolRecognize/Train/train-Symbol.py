import torch
import torch.nn as nn
import torch.utils.data as Data
import torchvision      # 数据库模块
import matplotlib.pyplot as plt
import numpy as np
import time
# ============数据集提取============
x=np.load('D:/实验台/手写符号/dateset/Train_X.npy')
y=np.load('D:/实验台/手写符号/dateset/Train_Y.npy')
#test_x=np.load('D:/实验台/手写符号/dateset/Test_X.npy')
#test_y=np.load('D:/实验台/手写符号/dateset/Test_Y.npy')
x = torch.from_numpy(x)
y = torch.from_numpy(y)
#test_x = torch.from_numpy(test_x)
#test_y = torch.from_numpy(test_y)
y=y.long()
#test_y=test_y.long()

# ============批处理============
train_data = Data.TensorDataset(x,y)
train_loader = Data.DataLoader(
    dataset=train_data,
    batch_size=100,
    shuffle=True,  
    num_workers=2, 
)
#test_data = Data.TensorDataset(test_x,test_y)
#test_loader = Data.DataLoader(
#    dataset=test_data,
#    batch_size=25,
#    shuffle=True,  
#    num_workers=2, 
#)
# ============网络结构============
class BasicConv2d(nn.Module):
    def __init__(self, in_channels, out_channals, **kwargs):
        super().__init__()
        self.conv = nn.Conv2d(in_channels, out_channals, **kwargs)
        self.bn = nn.BatchNorm2d(out_channals)
        self.relu = nn.ReLU(inplace=True)
    def forward(self, x):
        x = self.conv(x)
        x = self.bn(x)
        x = self.relu(x)
        return x

class CNN(nn.Module):
    def __init__(self):
        super(CNN, self).__init__()
        super(CNN, self).__init__()
        self.conv1 = BasicConv2d(1,16,kernel_size=3, padding=1)
        self.maxpool = nn.MaxPool2d(2)
        self.conv2 = BasicConv2d(16,32,kernel_size=3, padding=1)
        # ======fc======
        self.linear1 = nn.Linear(32*7*7,128)
        self.relu = nn.ReLU()
        self.linear2 = nn.Linear(128,128)
        self.linear3 = nn.Linear(128,44)

    def forward(self, x):
        x = self.conv1(x)
        x = self.maxpool(x)
        #======
        x = self.conv2(x)
        x = self.maxpool(x)
        #======
        x = x.view(x.size(0), -1)   # 展平多维的卷积图成 (batch_size, 32 * 7 * 7)
        x = self.linear1(x)
        x = self.relu(x)
        x = self.linear2(x)
        x = self.relu(x)
        x = self.linear3(x)
        return x

maxaccuracy = 0
EPOCH = 100           # 训练整批数据多少次, 为了节约时间, 我们只训练一次
LR = 0.001          # 学习率
cnn = CNN().cuda()
#cnn.load_state_dict(torch.load('Written_Num0.9919.pkl'))
optimizer = torch.optim.Adam(cnn.parameters(), lr=LR)   # optimize all cnn parameters
loss_func = nn.CrossEntropyLoss().cuda()   # the target label is not one-hotted
# ============Accuracy============
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
    accuracy = sum/(len(test_y))
    print('Accuracy: {:.5f}'.format(accuracy)) 
    return accuracy
# ============Training============
if __name__ == "__main__":
    for epoch in range(EPOCH):
        start = time.time()
        train_loss = 0
        for step, (b_x, b_y) in enumerate(train_loader):   # 分配 batch data, normalize x when iterate train_loader
            b_x,b_y=b_x.cuda(),b_y.cuda()
            output = cnn(b_x)               # cnn output
            loss = loss_func(output, b_y).cpu()   # cross entropy loss
            optimizer.zero_grad()           # clear gradients for this training step
            loss.backward()                 # backpropagation, compute gradients
            optimizer.step()                # apply gradients
            train_loss += loss
        print('epoch: {}, Train Loss: {:.6f}'.format(epoch, train_loss / len(x)))
        #accuracy = AccuracyOutput()
        accuracy = 1 - (train_loss / len(x))
        print(accuracy)
        if(accuracy>maxaccuracy):
            torch.save(cnn.state_dict(), 'D:/实验台/手写符号/'+str(epoch)+'.pkl')
            #torch.save(cnn.state_dict(), 'D:/实验台/手写符号/'+str(accuracy)+'.pkl')
            maxaccuracy = accuracy
        print('use time: {:.5f} s'.format(time.time()  - start))