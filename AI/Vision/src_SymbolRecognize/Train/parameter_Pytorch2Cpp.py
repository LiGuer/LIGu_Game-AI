import numpy as np
import torch
import re
torch.set_printoptions(precision=8, threshold=10000000, edgeitems=10000000, linewidth=10000000, profile=None)


fo = open("parameter-Temp.txt", "w")
parameter = str(torch.load('train-Symbol_parameter0.000874.pkl'))
parameter = re.sub(r'[\[,\]\(\);=]', "", parameter)
parameter = re.sub(r'\'(\S+?)\'', "", parameter)
parameter = re.sub(r'device', "", parameter)
parameter = re.sub(r'tensor', "", parameter)
parameter = re.sub(r'OrderedDict', "", parameter)
parameter = re.sub(r'\n\n', "\n", parameter)
fo.write(parameter)