import torch
import torch.nn as nn
import torch.utils.data as Data
import torchvision
import matplotlib.pyplot as plt
from torchsummary import summary
import re
import time
class CNN(nn.Module):
    def __init__(self):
        super(CNN, self).__init__()
        self.conv1 = nn.Sequential(nn.Conv2d(in_channels=1,out_channels=16,kernel_size=5,stride=1,padding=2, bias=False
                 ), nn.ReLU(),nn.MaxPool2d(kernel_size=2))
        self.conv2 = nn.Sequential(nn.Conv2d(16, 32, 5, 1, 2, bias=False),nn.ReLU(),nn.MaxPool2d(2))
        self.Linear1 = nn.Linear(32 * 7 * 7, 128)
        self.Linear2 = nn.Linear(128, 64)
        self.Linear3 = nn.Linear(64, 10)
    def forward(self, x):
        x = self.conv1(x)
        x = self.conv2(x)
        x = x.view(x.size(0), -1)
        x = self.Linear1(x)
        x = self.Linear2(x)
        output = self.Linear3(x)
        return output
torch.set_printoptions(precision=8, threshold=1000000, edgeitems=100000, linewidth=100000, profile=None)

train_data = torchvision.datasets.MNIST(root='./mnist/', train=True,transform=torchvision.transforms.ToTensor(),download=False)
test_data = torchvision.datasets.MNIST(root='./mnist/', train=False,transform=torchvision.transforms.ToTensor())
train_loader = Data.DataLoader(dataset=train_data, batch_size=50, shuffle=True)
test_y = test_data.test_labels
test_loader = Data.DataLoader(dataset=test_data,batch_size=25,shuffle=True,num_workers=2, )
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

maxaccuracy = 0
EPOCH = 100           # 训练整批数据多少次, 为了节约时间, 我们只训练一次
LR = 0.001          # 学习率
cnn = CNN().cuda()
cnn.load_state_dict(torch.load('0.pkl'))
optimizer = torch.optim.Adam(cnn.parameters(), lr=LR)   # optimize all cnn parameters
loss_func = nn.CrossEntropyLoss().cuda()   # the target label is not one-hotted

def save():
    file = open("D:/cnn.txt","w+")
    output = str(cnn.state_dict())
    t = output
    t = re.sub('[0-9,.\-e\[\]\s]', "", t)
    print(t)
    output = re.sub('\'(\S+)\'', "", output)
    output = re.sub(r'e\-', "@", output)
    output = re.sub(r'[,\[\]()A-Za-z\=]', "", output)
    output = re.sub(r'\n\n', "\n", output)
    output = re.sub(r'\@', "e-", output)
    file.write(output)
    file.close

if __name__ == '__main__':
    # training and testing
    for epoch in range(20):
        train_loss = 0
        start = time.time()
        for step, (b_x, b_y) in enumerate(train_loader):
            b_x,b_y=b_x.cuda(),b_y.cuda()
            output = cnn(b_x)
            loss = loss_func(output, b_y).cpu()
            optimizer.zero_grad() 
            loss.backward()
            optimizer.step()
            train_loss += loss
        print('epoch: {}, Train Loss: {:.6f}'.format(epoch, train_loss / 60000))
        accuracy = AccuracyOutput()
        print(accuracy)
        if(accuracy>maxaccuracy):
            torch.save(cnn.state_dict(), 'D:/实验台/'+str(epoch)+'.pkl')
            save()
            maxaccuracy = accuracy
        print('use time: {:.5f} s'.format(time.time()  - start))