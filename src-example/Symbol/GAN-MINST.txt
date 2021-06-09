import torch
import torch. nn as nn

class Generator(nn.Module):
    def __init__(self, z_dim, ):
        super().__init__()
        self.z_dim = z_dim
        net = []
        # 1:设定每次反卷积的输入和输出通道数等，卷积核尺寸固定为4，反卷积输出为“SAME”模式
        channels_in  = [self.z_dim, 512, 256, 128, 64]
        channels_out = [512, 256, 128, 64, 1]
        active = ["R", "R", "R", "R", "tanh"]
        stride = [2, 2, 2, 1, 1]
        padding= [0, 1, 1, 1, 0]
        for i in range(len(channels_in)):
            net.append(nn.ConvTranspose2d(in_channels=channels_in[i], out_channels=channels_out[i], kernel_size=4, stride=stride[i], padding=padding[i], bias=False))
            if   active[i] == "R":
                net.append(nn.BatchNorm2d(num_features=channels_out[i]))
                net.append(nn.ReLU())
            elif active[i] == "tanh":
                net.append(nn.Tanh())
        self.generator = nn.Sequential(*net)
        self.weight_init()
    def forward(self, x):
        out = self.generator(x)
        return out
    def weight_init(self):
        for m in self.generator.modules():
            if isinstance(m, nn.ConvTranspose2d):
                nn.init.normal_(m.weight.data, 0, 0.02)
            elif isinstance(m, nn.BatchNorm2d):
                nn.init.normal_(m.weight.data, 0, 0.02)
                nn.init.constant_(m.bias.data, 0)
                
class Discriminator(nn.Module):
    def __init__(self):
        """
        initialize
        :param image_size: tuple (3, h, w)
        """
        super().__init__()
        net = []
        # 1:预先定义
        channels_in  = [1, 64, 128, 256, 512]
        channels_out = [64, 128, 256, 512, 1]
        padding = [1, 1, 1, 1, 1]
        active = ["LR", "LR", "LR", "LR", "sigmoid"]
        for i in range(len(channels_in)):
            net.append(nn.Conv2d(in_channels=channels_in[i], out_channels=channels_out[i],
                                 kernel_size=3, stride=2, padding=padding[i], bias=False))
            if i == 0:
                net.append(nn.LeakyReLU(0.2))
            elif active[i] == "LR":
                net.append(nn.BatchNorm2d(num_features=channels_out[i]))
                net.append(nn.LeakyReLU(0.2))
            elif active[i] == "sigmoid":
                net.append(nn.Sigmoid())

        self.discriminator = nn.Sequential(*net)
        self.weight_init()

    def weight_init(self):
        for m in self.discriminator.modules():
            if isinstance(m, nn.ConvTranspose2d):
                nn.init.normal_(m.weight.data, 0, 0.02)
            elif isinstance(m, nn.BatchNorm2d):
                nn.init.normal_(m.weight.data, 0, 0.02)
                nn.init.constant_(m.bias.data, 0)
    def forward(self, x):
        out = self.discriminator(x)
        out = out.view(x.size(0), -1)
        return out
    
import torchvision
import torch.utils.data as Data
train_data = torchvision.datasets.MNIST(root='./mnist/', train=True, transform=torchvision.transforms.ToTensor(),download=False)
test_data  = torchvision.datasets.MNIST(root='./mnist/', train=False,transform=torchvision.transforms.ToTensor(),download=False)
train_loader = Data.DataLoader(dataset = train_data, batch_size=5, shuffle=True)
test_loader  = Data.DataLoader(dataset = test_data,  batch_size=25, shuffle=True, num_workers=2, )

from PIL import Image
import numpy as np
lr = 0.01
batch_size = 5
z_dim = 1
generator     = Generator(z_dim).cuda()
discriminator = Discriminator().cuda()
bce_loss = torch.nn.BCELoss().cuda()
optimizer_G = torch.optim.Adam(generator.    parameters(), lr=lr, betas=(0.5, 0.999))
optimizer_D = torch.optim.Adam(discriminator.parameters(), lr=lr, betas=(0.5, 0.999))
z = torch.randn(size=(batch_size, z_dim))
real_label = torch. ones(size=(batch_size, 1), requires_grad=False).cuda()
fake_label = torch.zeros(size=(batch_size, 1), requires_grad=False).cuda()
iteration = 0
for _ in range(1000):
    for image, label in train_loader:
        # 1：数据准备 
        image = image.cuda()
        z = torch.randn(size=(batch_size,z_dim,2,2)).cuda()
        # #################################################
        optimizer_G.zero_grad()                        # 2:训练生成器
        #print(z.size())
        generated_image = generator(z)                 # 2.1：生成伪造样本
        #print(generated_image.size())
        d_out_fake = discriminator(generated_image)    # 2.2：计算判别器对伪造样本的输出的为真样本的概率值
        #print(d_out_fake.size())
        g_loss = bce_loss(d_out_fake, real_label).cpu()      # 2.3：计算生成器伪造样本不被认为是真的损失
        g_loss.backward()                              # 2.4：更新生成器
        optimizer_G.step()
        # #################################################
        optimizer_D.zero_grad()                        # 3：训练判别器
        d_out_real = discriminator(image)              # 3.1：计算判别器对真实样本给出为真的概率
        real_loss = bce_loss(d_out_real, real_label).cpu()   # 3.2：计算判别器对真实样本的su's
        d_out_fake = discriminator(generated_image.detach())  # 3.3:计算判别器
        fake_loss = bce_loss(d_out_fake, fake_label).cpu()
        d_loss = real_loss + fake_loss
        d_loss.backward()                              # 3.4:更新判别器参数
        optimizer_D.step()
        #record_iter.append(iteration)                  # 4:记录损失
        #record_g_loss.append(g_loss.item())
        #record_d_loss.append(d_loss.item())
        # #################################################
        iteration += 1
        if iteration % 100 == 0:                      # 5：打印损失，保存图片
            with torch.no_grad():
                generator.eval()
                #fixed_image = generator(fixed_z)
                generator.train()
                print("[iter: {}], [G loss: {}], [D loss: {}]".format(iteration, g_loss.item(), d_loss.item()))
                torchvision.utils.save_image(generated_image[0].squeeze(),"D:/GANdata/"+str(iteration)+".jpg")